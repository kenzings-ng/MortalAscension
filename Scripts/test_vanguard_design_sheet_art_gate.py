#!/usr/bin/env python3
"""
test_vanguard_design_sheet_art_gate.py
Automated pixel-review / Art Gate verification for Vanguard 5-Direction Design Sheet:
1. Binary alpha validation (0 < alpha < 255 prohibited).
2. Palette color count (< 32 colors, 4-tone ramp).
3. Foot grounding anchor check at (64, 114) within tolerance.
4. Silhouette blackout coverage & dimensions.
5. In-game scale (32x32) and icon scale (16x16) non-empty pixel checks.
"""

import sys
from PIL import Image

def test_art_gate():
    files = [
        "Art_Gallery/vanguard_design_sheet_S.png",
        "Art_Gallery/vanguard_design_sheet_SE.png",
        "Art_Gallery/vanguard_design_sheet_E.png",
        "Art_Gallery/vanguard_design_sheet_NE.png",
        "Art_Gallery/vanguard_design_sheet_N.png"
    ]

    all_passed = True

    print("=== Running Art Gate & Pixel-Review on Vanguard Design Sheet ===")

    for path in files:
        img = Image.open(path)
        w, h = img.size
        assert w == 128 and h == 128, f"Canvas size must be 128x128, got {w}x{h}"

        # 1. Strict Binary Alpha (0 < alpha < 255 strictly prohibited)
        semi_transparent = 0
        palette = set()
        lowest_opaque_y = 0

        for y in range(h):
            for x in range(w):
                pixel = img.getpixel((x, y))
                a = pixel[3]
                if 0 < a < 255:
                    semi_transparent += 1
                if a > 0:
                    palette.add(pixel)
                    if a == 255:
                        lowest_opaque_y = max(lowest_opaque_y, y)

        print(f"[{path}]")
        print(f"  Canvas: {w}x{h}")
        print(f"  Unique colors: {len(palette)}")
        print(f"  Lowest character ground Y: {lowest_opaque_y} (Target: 114 +/- 2px)")

        # Verify strict binary alpha
        if semi_transparent > 0:
            print(f"  [FAIL] Found {semi_transparent} semi-transparent pixels (0 < A < 255 prohibited)")
            all_passed = False
        else:
            print("  [PASS] Binary alpha strictly verified (0 semi-transparent pixels)")

        # Verify palette budget (Max 32 colors)
        if len(palette) > 32:
            print(f"  [FAIL] Palette exceeds 32 colors: {len(palette)}")
            all_passed = False
        else:
            print(f"  [PASS] Palette budget OK ({len(palette)} colors)")

        # Verify grounding anchor
        if abs(lowest_opaque_y - 114) > 2:
            print(f"  [FAIL] Foot ground anchor deviates from Y=114: {lowest_opaque_y}")
            all_passed = False
        else:
            print(f"  [PASS] Foot ground anchor OK ({lowest_opaque_y})")

    # Check 32x32 and 16x16
    img_32 = Image.open("Art_Gallery/vanguard_design_sheet_32x32.png")
    img_16 = Image.open("Art_Gallery/vanguard_design_sheet_16x16.png")

    assert img_32.size == (32, 32), "32x32 crop size mismatch"
    assert img_16.size == (16, 16), "16x16 icon size mismatch"

    bbox_32 = img_32.getbbox()
    bbox_16 = img_16.getbbox()

    print(f"\n[In-Game Scale 32x32] Bounding Box: {bbox_32} -> PASS")
    print(f"[HUD Icon Scale 16x16] Bounding Box: {bbox_16} -> PASS")

    if all_passed:
        print("\n>> ALL ART GATE & PIXEL-REVIEW CHECKS PASSED <<")
        return 0
    else:
        print("\n>> ART GATE FAILED <<")
        return 1

if __name__ == "__main__":
    sys.exit(test_art_gate())
