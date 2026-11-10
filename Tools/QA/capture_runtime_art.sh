#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
ENGINE_BIN="${UE_EDITOR_PATH:-/mnt/Data/Engine/Binaries/Linux/UnrealEditor}"
MAP="${1:-/Game/Maps/L_Ruins_Blockout}"
RESOLUTION="${2:-1280x720}"
TIMEOUT_SECONDS="${CAPTURE_TIMEOUT_SECONDS:-240}"

if [[ ! "${MAP}" =~ ^/Game/Maps/[A-Za-z0-9_]+$ ]] || [[ ! "${RESOLUTION}" =~ ^[1-9][0-9]{2,3}x[1-9][0-9]{2,3}$ ]]; then
    echo 'Usage: bash Tools/QA/capture_runtime_art.sh /Game/Maps/MAP 1280x720' >&2
    exit 2
fi
if [[ ! "${TIMEOUT_SECONDS}" =~ ^[1-9][0-9]*$ ]]; then
    echo 'CAPTURE_TIMEOUT_SECONDS must be a positive integer.' >&2
    exit 2
fi
if [[ ! -x "${ENGINE_BIN}" ]]; then
    echo "Unreal Editor not found: ${ENGINE_BIN}" >&2
    exit 2
fi
if [[ ! -f "${PROJECT_ROOT}/Content/${MAP#/Game/}.umap" ]]; then
    echo "Map not found: ${MAP}" >&2
    exit 2
fi

mkdir -p "${PROJECT_ROOT}/Saved/VisualReview"
OUTPUT_DIR="$(mktemp -d "${PROJECT_ROOT}/Saved/VisualReview/${MAP##*/}-XXXXXX")"
SCREENSHOT="${OUTPUT_DIR}/runtime.png"
LOG="${OUTPUT_DIR}/unreal.log"
WIDTH="${RESOLUTION%x*}"
HEIGHT="${RESOLUTION#*x}"

printf 'Map: %s\nResolution: %s\nSource: Unreal game renderer, UI excluded\nPost-processing outside engine: none\nCommit: %s\n' \
    "${MAP}" "${RESOLUTION}" "$(git -C "${PROJECT_ROOT}" rev-parse HEAD)" > "${OUTPUT_DIR}/capture.txt"
git -C "${PROJECT_ROOT}" status --short >> "${OUTPUT_DIR}/capture.txt"

export SDL_VIDEODRIVER="${SDL_VIDEODRIVER:-x11}"
export __NV_PRIME_RENDER_OFFLOAD="${__NV_PRIME_RENDER_OFFLOAD:-1}"
export __GLX_VENDOR_LIBRARY_NAME="${__GLX_VENDOR_LIBRARY_NAME:-nvidia}"
export __VK_LAYER_NV_optimus="${__VK_LAYER_NV_optimus:-NVIDIA_only}"
if [[ -f /usr/share/vulkan/icd.d/nvidia_icd.json ]]; then
    export VK_ICD_FILENAMES="${VK_ICD_FILENAMES:-/usr/share/vulkan/icd.d/nvidia_icd.json}"
fi

"${ENGINE_BIN}" "${PROJECT_ROOT}/ProjectAscendant.uproject" "${MAP}" \
    -game -RenderOffscreen -Unattended -NoSound -NoSplash \
    -ResX="${WIDTH}" -ResY="${HEIGHT}" -Windowed \
    "-ExecCmds=t.MaxFPS 30,r.HighResScreenshotDelay 30,HighResShot ${RESOLUTION} filename=MortalAscension_${OUTPUT_DIR##*/}.png" \
    > "${LOG}" 2>&1 &
ENGINE_PID=$!

cleanup() {
    kill -TERM "${ENGINE_PID}" 2>/dev/null || true
    wait "${ENGINE_PID}" 2>/dev/null || true
}
trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

DEADLINE=$((SECONDS + TIMEOUT_SECONDS))
while (( SECONDS < DEADLINE )); do
    SOURCE_SCREENSHOT="$(sed -n 's/^.*LogClient: High resolution screenshot saved as //p' "${LOG}" | tail -n 1)"
    if [[ -s "${SOURCE_SCREENSHOT}" ]] && python3 -c 'import pathlib, struct, sys; data = pathlib.Path(sys.argv[1]).read_bytes(); assert data[:8] == b"\x89PNG\r\n\x1a\n"; assert data[-12:] == b"\x00\x00\x00\x00IEND\xaeB\x60\x82"; assert struct.unpack(">II", data[16:24]) == (int(sys.argv[2]), int(sys.argv[3]))' "${SOURCE_SCREENSHOT}" "${WIDTH}" "${HEIGHT}" 2>/dev/null; then
        cp -- "${SOURCE_SCREENSHOT}" "${SCREENSHOT}"
        printf 'Original engine output: %s\n' "${SOURCE_SCREENSHOT}" >> "${OUTPUT_DIR}/capture.txt"
        printf 'Runtime capture: %s\nEvidence: %s\n' "${SCREENSHOT}" "${OUTPUT_DIR}/capture.txt"
        exit 0
    fi
    if ! kill -0 "${ENGINE_PID}" 2>/dev/null; then
        echo "Unreal exited before capture. Inspect ${LOG}" >&2
        exit 1
    fi
    sleep 2
done
echo "Capture timed out. Inspect ${LOG}" >&2
exit 1
