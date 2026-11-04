#!/usr/bin/env bash
# ==============================================================================
# Project Ascendant - Game & Editor Launcher
# 2.5D Isometric HD-2D Dark Fantasy Action RPG / MMO
# Playable Slice & Multi-Client Playtest Runner (DECISIONS §13 & ROADMAP 1.5)
# ==============================================================================

set -e

PROJECT_DIR="/mnt/Data/Projects/project-games/ProjectAscendant"
UPROJECT="${PROJECT_DIR}/ProjectAscendant.uproject"
ENGINE_BIN="/mnt/Data/Engine/Binaries/Linux/UnrealEditor"

MODE="${1:-game}"

# Cấu hình GPU Offload tối ưu cho kiến trúc Hybrid Linux (Intel Display + NVIDIA P104-100 Headless 3D)
export __NV_PRIME_RENDER_OFFLOAD=1
export __GLX_VENDOR_LIBRARY_NAME=nvidia
export __VK_LAYER_NV_optimus=NVIDIA_only
export VK_ICD_FILENAMES=/usr/share/vulkan/icd.d/nvidia_icd.json

# Khắc phục lỗi dma-buf swapchain trắng trên Wayland với secondary headless GPU: chạy qua Xwayland / X11
export SDL_VIDEODRIVER="${SDL_VIDEODRIVER:-x11}"

if [ ! -f "${ENGINE_BIN}" ]; then
    echo "[ERROR] UnrealEditor binary not found at: ${ENGINE_BIN}"
    exit 1
fi

if [ ! -f "${UPROJECT}" ]; then
    echo "[ERROR] .uproject file not found at: ${UPROJECT}"
    exit 1
fi

echo "============================================================"
echo " Project Ascendant Launcher"
echo " Engine: Unreal Engine 5.8 (Linux x86_64)"
echo " Mode  : ${MODE}"
echo "============================================================"

case "${MODE}" in
    "game"|"play")
        RES_X="${2:-1280}"
        RES_Y="${3:-720}"
        WINDOW_FLAG="-Windowed"
        if [ "${RES_X}" = "1920" ] && [ "${RES_Y}" = "1080" ]; then
            # Tránh lỗi KDE window manager ép resize cửa sổ 1920x1080 làm treo Vulkan swapchain
            WINDOW_FLAG="-Fullscreen"
            echo "[INFO] Running in Fullscreen mode for 1920x1080 to prevent swapchain resize deadlock."
        fi
        echo "[INFO] Launching Project Ascendant in Standalone Mode (${RES_X}x${RES_Y} ${WINDOW_FLAG})..."
        echo "[INFO] Game log is saved to Saved/Logs/ProjectAscendant.log"
        exec "${ENGINE_BIN}" "${UPROJECT}" -game -ResX="${RES_X}" -ResY="${RES_Y}" "${WINDOW_FLAG}"
        ;;

    "game-log")
        RES_X="${2:-1280}"
        RES_Y="${3:-720}"
        echo "[INFO] Launching Project Ascendant with live terminal log (${RES_X}x${RES_Y})..."
        exec "${ENGINE_BIN}" "${UPROJECT}" -game -ResX="${RES_X}" -ResY="${RES_Y}" -Windowed -log
        ;;

    "client")
        SERVER_ADDR="${2:-127.0.0.1:7777}"
        RES_X="${3:-1280}"
        RES_Y="${4:-720}"
        echo "[INFO] Connecting Client to Dedicated Server at ${SERVER_ADDR} (${RES_X}x${RES_Y})..."
        exec "${ENGINE_BIN}" "${UPROJECT}" "${SERVER_ADDR}" -game -ResX="${RES_X}" -ResY="${RES_Y}" -Windowed -log
        ;;

    "server")
        PORT="${2:-7777}"
        echo "[INFO] Launching Dedicated Server on port ${PORT}..."
        exec "${ENGINE_BIN}" "${UPROJECT}" -server -log -PORT="${PORT}"
        ;;

    "playtest"|"multi"|"2clients")
        PORT="${2:-7777}"
        echo "============================================================"
        echo " Khởi chạy Phiên Chơi Thử Mạng Cục Bộ (1 Server + 2 Clients)"
        echo " Cổng Server: ${PORT}"
        echo " Nhấn Ctrl+C để dừng toàn bộ server và các client"
        echo "============================================================"

        # 1. Khởi chạy Dedicated Server
        echo "[1/3] Khởi chạy Dedicated Server trong nền..."
        "${ENGINE_BIN}" "${UPROJECT}" -server -log -PORT="${PORT}" > /dev/null 2>&1 &
        SERVER_PID=$!
        echo "[INFO] Server PID: ${SERVER_PID}. Đợi 4 giây để server nạp map L_VerdantFrontier_Outpost..."
        sleep 4

        # Thiết lập bẫy tín hiệu dọn dẹp tiến trình khi thoát
        cleanup() {
            echo ""
            echo "[INFO] Đang đóng toàn bộ tiến trình playtest (Server: ${SERVER_PID}, Clients: ${CLIENT1_PID} ${CLIENT2_PID})..."
            kill "${CLIENT1_PID}" 2>/dev/null || true
            kill "${CLIENT2_PID}" 2>/dev/null || true
            kill "${SERVER_PID}" 2>/dev/null || true
            wait "${CLIENT1_PID}" "${CLIENT2_PID}" "${SERVER_PID}" 2>/dev/null || true
            echo "[INFO] Hoàn tất dọn dẹp playtest."
            exit 0
        }
        trap cleanup INT TERM EXIT

        # 2. Khởi chạy Client 1 (Nửa màn hình trái: 960x540 tại X=40, Y=60)
        echo "[2/3] Khởi chạy Client 1 (Cửa sổ trái 960x540)..."
        "${ENGINE_BIN}" "${UPROJECT}" "127.0.0.1:${PORT}" -game -ResX=960 -ResY=540 -Windowed -WinX=40 -WinY=60 &
        CLIENT1_PID=$!

        sleep 2

        # 3. Khởi chạy Client 2 (Nửa màn hình phải: 960x540 tại X=1020, Y=60)
        echo "[3/3] Khởi chạy Client 2 (Cửa sổ phải 960x540)..."
        "${ENGINE_BIN}" "${UPROJECT}" "127.0.0.1:${PORT}" -game -ResX=960 -ResY=540 -Windowed -WinX=1020 -WinY=60 &
        CLIENT2_PID=$!

        echo "============================================================"
        echo " Cả 2 Client đã kết nối vào Server 127.0.0.1:${PORT}."
        echo " Đang giữ phiên chơi thử... Nhấn Ctrl+C để kết thúc."
        echo "============================================================"
        wait "${CLIENT1_PID}" "${CLIENT2_PID}" "${SERVER_PID}" 2>/dev/null || true
        ;;

    "playtest-lag")
        PORT="${2:-7777}"
        echo "============================================================"
        echo " Khởi chạy Phiên Chơi Thử Mạng Mô Phỏng Lag & Mất Gói"
        echo " Cấu hình mô phỏng: Net PktLag=150ms, Net PktLoss=2% (ROADMAP §1.5)"
        echo " Cổng Server: ${PORT}"
        echo "============================================================"

        "${ENGINE_BIN}" "${UPROJECT}" -server -log -PORT="${PORT}" > /dev/null 2>&1 &
        SERVER_PID=$!
        echo "[INFO] Server PID: ${SERVER_PID}. Đợi 4 giây..."
        sleep 4

        cleanup_lag() {
            echo ""
            echo "[INFO] Đang đóng toàn bộ tiến trình playtest..."
            kill "${CLIENT1_PID}" 2>/dev/null || true
            kill "${CLIENT2_PID}" 2>/dev/null || true
            kill "${SERVER_PID}" 2>/dev/null || true
            wait "${CLIENT1_PID}" "${CLIENT2_PID}" "${SERVER_PID}" 2>/dev/null || true
            exit 0
        }
        trap cleanup_lag INT TERM EXIT

        echo "[2/3] Khởi chạy Client 1 với mô phỏng PktLag=150, PktLoss=2%..."
        "${ENGINE_BIN}" "${UPROJECT}" "127.0.0.1:${PORT}" -game -ResX=960 -ResY=540 -Windowed -WinX=40 -WinY=60 -ExecCmds="Net PktLag=150; Net PktLoss=2" &
        CLIENT1_PID=$!

        sleep 2

        echo "[3/3] Khởi chạy Client 2 với mô phỏng PktLag=150, PktLoss=2%..."
        "${ENGINE_BIN}" "${UPROJECT}" "127.0.0.1:${PORT}" -game -ResX=960 -ResY=540 -Windowed -WinX=1020 -WinY=60 -ExecCmds="Net PktLag=150; Net PktLoss=2" &
        CLIENT2_PID=$!

        echo "============================================================"
        echo " Phiên mô phỏng mạng độ trễ cao đang chạy. Nhấn Ctrl+C để kết thúc."
        echo "============================================================"
        wait "${CLIENT1_PID}" "${CLIENT2_PID}" "${SERVER_PID}" 2>/dev/null || true
        ;;

    "editor"|"gui")
        echo "[INFO] Launching Unreal Editor GUI..."
        exec "${ENGINE_BIN}" "${UPROJECT}"
        ;;

    *)
        echo "Usage: ./run_game.sh [MODE] [OPTIONS...]"
        echo ""
        echo "Các chế độ hỗ trợ:"
        echo "  ./run_game.sh                  # Chạy đơn Standalone Cửa sổ 1280x720 (Khuyến nghị)"
        echo "  ./run_game.sh game 1920 1080   # Chạy đơn Toàn màn hình 1920x1080"
        echo "  ./run_game.sh game-log         # Chạy đơn kèm console log debug"
        echo "  ./run_game.sh playtest         # [Z4] Tự động mở 1 Server + 2 Client song song trên máy"
        echo "  ./run_game.sh playtest-lag     # [Z4/1.5] Mở 1 Server + 2 Client kèm mô phỏng Lag 150ms, Loss 2%"
        echo "  ./run_game.sh server [PORT]    # Khởi chạy Dedicated Server cục bộ (Mặc định: 7777)"
        echo "  ./run_game.sh client [IP:PORT] # Kết nối Client tới Dedicated Server (Mặc định: 127.0.0.1:7777)"
        echo "  ./run_game.sh editor           # Mở giao diện Unreal Editor GUI"
        exit 1
        ;;
esac
