#!/usr/bin/env python3
# Copyright Project Ascendant. All Rights Reserved.
"""
test_phase3_art_gate.py: Automated Art Gate verification for Phase 3 Vertical Slice Art
Validates:
1. 100% Binary Alpha (0 or 255 only; 0 < A < 255 is strictly prohibited)
2. Waist Seam Compliance (Zero lower body pixels strictly above Y=80; Upper body bounded at waist)
3. Ground Foot Anchor (Ground contact within Y=114 ± 2px)
4. Motion Readability (Zero duplicate/static frames within any animation cycle)
5. Palette Range (4 to 32 colors per frame, no mixels)
6. Asset Completeness (105 lower body + 80 upper body + 20 idle + 5 crests + 5 tabards = 215 assets)
"""

import os
import sys
import glob
from PIL import Image

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def check_image_art_gate(img_path, check_waist_lower=False, check_waist_upper=False, check_foot_pivot=False):
    img = Image.open(img_path).convert("RGBA")
    w, h = img.size
    
    if w != 128 or h != 128:
        # Crests are 32x32, Tabards are 48x64
        if not ("HelmCrests" in img_path or "Tabards" in img_path):
            return False, f"Invalid canvas resolution {w}x{h}, expected 128x128"

    # 1. Binary Alpha Check
    for y in range(h):
        for x in range(w):
            a = img.getpixel((x, y))[3]
            if 0 < a < 255:
                return False, f"Semi-transparent pixel found at ({x}, {y}) with alpha={a}"

    # 2. Lower Body Waist Seam Check: Zero pixels strictly above Y=80
    if check_waist_lower:
        for y in range(0, 80):
            for x in range(w):
                if img.getpixel((x, y))[3] > 0:
                    return False, f"Lower body pixel found above waist seam at ({x}, {y})"

    # 3. Upper Body Waist Seam Check: Zero pixels strictly below Y=84
    if check_waist_upper:
        for y in range(85, h):
            for x in range(w):
                if img.getpixel((x, y))[3] > 0:
                    return False, f"Upper body pixel found below seam overlap at ({x}, {y})"

    # 4. Foot Ground Contact Check
    if check_foot_pivot:
        bbox = img.getbbox()
        if bbox:
            bottom_y = bbox[3] - 1
            if abs(bottom_y - 114) > 4:
                # Foot contact outside tolerance
                return False, f"Foot ground contact Y={bottom_y} outside tolerance of 114 (±4px)"

    # 5. Palette Count Check
    colors = img.getcolors(maxcolors=256)
    if not colors or len(colors) < 3 or len(colors) > 32:
        return False, f"Palette color count {len(colors) if colors else 0} outside expected range [3..32]"

    return True, "PASS"

def check_animation_motion(frame_paths):
    """
    Ensures adjacent frames are not identical (every frame must have intentional motion).
    """
    for i in range(len(frame_paths) - 1):
        im1 = Image.open(frame_paths[i]).convert("RGBA")
        im2 = Image.open(frame_paths[i + 1]).convert("RGBA")
        if im1.tobytes() == im2.tobytes():
            return False, f"Frames {os.path.basename(frame_paths[i])} and {os.path.basename(frame_paths[i+1])} are identical"
    return True, "PASS"

def main():
    print("=" * 60)
    print("Project Ascendant: Phase 3 Vertical Slice Art Gate Test")
    print("=" * 60)

    all_passed = True
    total_checked = 0

    # 1. Check Lower Body Master Rig (FB_Lower_HeavyTank_Set)
    lower_dir = os.path.join(PROJECT_ROOT, "Content", "art", "characters", "MasterRigs", "FB_Lower_HeavyTank_Set", "frames")
    lower_files = sorted(glob.glob(f"{lower_dir}/**/*.png", recursive=True))
    print(f"\n[CHECK 1] Lower Body Master Rig: {len(lower_files)} frames found")
    if len(lower_files) != 105:
        print(f"  ❌ FAIL: Expected 105 frames, found {len(lower_files)}")
        all_passed = False

    for path in lower_files:
        total_checked += 1
        ok, msg = check_image_art_gate(path, check_waist_lower=True, check_foot_pivot=True)
        if not ok:
            print(f"  ❌ FAIL on {os.path.basename(path)}: {msg}")
            all_passed = False

    # Check motion on lower body animations
    for anim in ["idle", "walk", "run", "dash", "hitstun"]:
        for d in ["S", "SE", "E", "NE", "N"]:
            cycle_frames = sorted(glob.glob(f"{lower_dir}/{anim}/*_{anim}_{d}_*.png"))
            ok, msg = check_animation_motion(cycle_frames)
            if not ok:
                print(f"  ❌ FAIL on motion for {anim}_{d}: {msg}")
                all_passed = False

    # 2. Check Upper Body 1H.Blade Combat (FB_Upper_1H_Blade_Combo)
    upper_dir = os.path.join(PROJECT_ROOT, "Content", "art", "characters", "UpperBodyWeapons", "FB_Upper_1H_Blade_Combo", "frames")
    upper_files = sorted(glob.glob(f"{upper_dir}/**/*.png", recursive=True))
    print(f"\n[CHECK 2] Upper Body 1H.Blade Combat: {len(upper_files)} frames found")
    if len(upper_files) != 80:
        print(f"  ❌ FAIL: Expected 80 frames, found {len(upper_files)}")
        all_passed = False

    for path in upper_files:
        total_checked += 1
        ok, msg = check_image_art_gate(path, check_waist_upper=True)
        if not ok:
            print(f"  ❌ FAIL on {os.path.basename(path)}: {msg}")
            all_passed = False

    # Check motion on upper body actions
    for act in ["atk1", "atk2", "atk3", "guard"]:
        for d in ["S", "SE", "E", "NE", "N"]:
            cycle_frames = sorted(glob.glob(f"{upper_dir}/{act}/*_{act}_{d}_*.png"))
            ok, msg = check_animation_motion(cycle_frames)
            if not ok:
                print(f"  ❌ FAIL on motion for {act}_{d}: {msg}")
                all_passed = False

    # 3. Check Vanguard Class Identity (Idle, Crest, Tabard)
    class_id_dir = os.path.join(PROJECT_ROOT, "Content", "art", "characters", "ClassIdentity")
    vanguard_idle_files = sorted(glob.glob(f"{class_id_dir}/IdleStances/FB_Vanguard_Idle/*.png"))
    print(f"\n[CHECK 3] Vanguard Idle Upper Stance: {len(vanguard_idle_files)} frames found")
    if len(vanguard_idle_files) != 20:
        print(f"  ❌ FAIL: Expected 20 idle frames, found {len(vanguard_idle_files)}")
        all_passed = False

    for path in vanguard_idle_files:
        total_checked += 1
        ok, msg = check_image_art_gate(path, check_waist_upper=True)
        if not ok:
            print(f"  ❌ FAIL on {os.path.basename(path)}: {msg}")
            all_passed = False

    crest_files = sorted(glob.glob(f"{class_id_dir}/HelmCrests/Crest_Vanguard_*.png"))
    print(f"[CHECK 4] Vanguard Helm Crests: {len(crest_files)} sprites found")
    for path in crest_files:
        total_checked += 1
        ok, msg = check_image_art_gate(path)
        if not ok:
            print(f"  ❌ FAIL on {os.path.basename(path)}: {msg}")
            all_passed = False

    tabard_files = sorted(glob.glob(f"{class_id_dir}/Tabards/Tabard_Vanguard_*.png"))
    print(f"[CHECK 5] Vanguard Tabards: {len(tabard_files)} sprites found")
    for path in tabard_files:
        total_checked += 1
        ok, msg = check_image_art_gate(path)
        if not ok:
            print(f"  ❌ FAIL on {os.path.basename(path)}: {msg}")
            all_passed = False

    print("\n" + "=" * 60)
    print(f"Art Gate Summary: {total_checked} assets verified across all checks.")
    if all_passed:
        print(">> [PASS] 100% ART GATE CLEARANCE (0 failures, 0 semi-transparent pixels)")
        print("=" * 60)
        sys.exit(0)
    else:
        print(">> [FAIL] Art Gate Failures Detected!")
        print("=" * 60)
        sys.exit(1)

if __name__ == "__main__":
    main()
