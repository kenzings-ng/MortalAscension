# Mortal Ascension — Runtime visual acceptance

## Required evidence

- A gameplay demo image must be captured from the actual Unreal game renderer, not an illustration, HTML gallery, or image composited after capture.
- Keep the map, camera, resolution, scalability settings and asset revision consistent when comparing before and after.
- Label title-screen artwork and concept art explicitly. A title illustration does not demonstrate environment, character, or animation quality in gameplay.
- Preserve the raw screenshot, capture log and revision information. Do not retouch screenshots to hide integration defects.
- A still frame cannot approve motion. Idle, movement, attack, hit reaction and direction changes need runtime clips before animation approval.
- Mechanical asset tests are necessary but do not constitute visual approval. Owner review remains open until the owner sees and approves runtime evidence.

## SSH capture

Run `bash Tools/QA/capture_runtime_art.sh /Game/Maps/L_Ruins_Blockout 1280x720` on a machine with a working Unreal/Vulkan render device. Set `UE_EDITOR_PATH` when the engine is installed elsewhere. A working display connection may still be required by the platform backend even with offscreen rendering.

Each run creates a unique directory under `Saved/VisualReview` containing the original `runtime.png`, `unreal.log` and `capture.txt`. The screenshot excludes Slate/UI; it is environment/character evidence, not a UI acceptance test. The command caps the frame rate for capture, does not change project settings, and terminates only the Unreal process it launches.

## Current reference artwork

`Content/Art/UI/Source/Title_AshenGate.png` is AI-generated title-screen/background reference artwork produced with the built-in image-generation tool on 2026-11-10. It is not an Unreal gameplay screenshot and has not been approved as a gameplay-quality milestone. No gameplay map is replaced by this image.

The downloaded Cinzel font and its accompanying license reside in `Content/Art/UI/Fonts`; they are not yet integrated into the running UI.
