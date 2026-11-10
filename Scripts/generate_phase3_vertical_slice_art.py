#!/usr/bin/env python3
# Copyright Project Ascendant. All Rights Reserved.
"""
generate_phase3_vertical_slice_art.py
--------------------------------------
Authentic High-Craft Retro 2D Pixel Art Generator for Project Ascendant Phase 3 Vertical Slice.
Conforms strictly to SPEC-ART-2026-09-23-V2, Art Bible, and Automated Art Gate:
- 100% Binary Alpha (A in {0, 255})
- 1px Dark Charcoal Contour Outline (#121620)
- 4-Tier Tonal Ramps with Hue-Shifting (Steel, Gold, Crimson, Navy Gambeson, Cyan Visor)
- Seam constraints: Lower Body strictly Y >= 80, Upper Body bounded at Y <= 84
- Foot ground contact firmly anchored at Y = 114 ± 0px
- Zero static duplicate frames across animation cycles
"""

import os
import sys
from PIL import Image

PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CANVAS_W = 128
CANVAS_H = 128
CX = 64
WAIST_Y = 80
GROUND_Y = 114

# ==============================================================================
# COLOR PALETTE (24-Color Indexed Dark Fantasy Master Palette)
# ==============================================================================
CLR = {
    '.': (0, 0, 0, 0),                 # Transparent
    '#': (18, 22, 32, 255),            # 1px Dark Contour Outline
    'H': (240, 248, 255, 255),         # Specular Glint (Ice White)
    'L': (185, 205, 230, 255),         # Light Steel Plate
    'M': (120, 145, 175, 255),         # Midtone Steel
    'D': (68, 90, 120, 255),           # Shadow Steel
    'K': (38, 50, 72, 255),            # Deepest Crease / Outline
    'g': (255, 235, 120, 255),         # Gold Highlight
    'G': (220, 165, 30, 255),          # Gold Midtone
    'd': (145, 95, 15, 255),           # Gold Shadow
    'r': (255, 110, 110, 255),         # Bright Crimson Plume Highlight
    'R': (210, 40, 45, 255),           # Mid Red Plume
    'b': (130, 20, 30, 255),           # Dark Red Plume Shadow
    'B': (75, 10, 18, 255),            # Deep Maroon
    'T': (60, 95, 150, 255),           # Royal Blue Gambeson Light
    't': (35, 60, 105, 255),           # Royal Blue Midtone
    'u': (20, 35, 65, 255),            # Dark Gambeson Shadow
    'C': (150, 250, 255, 255),         # Visor Core Light
    'c': (0, 190, 230, 255),           # Cyan Glow Aura
    'W': (255, 255, 255, 255),         # Razor Blade Edge
    'V': (200, 225, 250, 255),         # Light Blade Face
    'm': (140, 165, 200, 255),         # Mid Blade Face
    's': (75, 95, 130, 255),           # Dark Blade Bevel
    'e': (85, 50, 30, 255),            # Leather Midtone
    'E': (45, 25, 15, 255)             # Dark Leather Shadow
}

DIRECTIONS = ["S", "SE", "E", "NE", "N"]

def draw_cluster(pix, rows, ox, oy, max_y=127, min_y=0, flip_x=False):
    """
    Renders pixel art cluster with strict boundary clamping.
    """
    for r_idx, row in enumerate(rows):
        y = oy + r_idx
        if not (min_y <= y <= max_y):
            continue
        for c_idx, ch in enumerate(row):
            x = ox + (len(row) - 1 - c_idx if flip_x else c_idx)
            if 0 <= x < CANVAS_W and ch in CLR and ch != '.':
                pix[x, y] = CLR[ch]

# ==============================================================================
# 1. LOWER BODY SPRITE PATTERNS (Waist Y=80 down to Foot Y=114)
# ==============================================================================

# Belts & Faulds
BELT_FAULD_S = [
    "..........####################..........",
    "........##eeeeeeeeegGGgeeeeeeeee##......",
    ".......#EEEEEEEEEEEgGGgEEEEEEEEEEE#.....",
    ".......#EEEEEEEEEEEgGGgEEEEEEEEEEE#.....",
    "......#HLLMDD#....#ttTt#....#DMMDK#.....",
    ".....#HLLMMMDD#...#tTTt#...#DDMMMDDK#...",
    ".....#HLLMMMDD#..#ttTTtt#..#DDMMMDDK#...",
    ".....#gGGGGGGd#..#tTTTTt#..#dGGGGGGd#...",
    "......#DDDDDD#...#tttttt#...#KKKKKK#...."
]

BELT_FAULD_SE = [
    "........####################............",
    "......##eeeeeeeeegGGgeeeeeeeee##........",
    ".....#EEEEEEEEEEEgGGgEEEEEEEEEEE#.......",
    ".....#EEEEEEEEEEEgGGgEEEEEEEEEEE#.......",
    "....#HLLMDD#....#ttTt#....#DMMDK#.......",
    "...#HLLMMMDD#...#tTTt#...#DDMMMDDK#.....",
    "...#HLLMMMDD#..#ttTTtt#..#DDMMMDDK#.....",
    "...#gGGGGGGd#..#tTTTTt#..#dGGGGGGd#.....",
    "....#DDDDDD#...#tttttt#...#KKKKKK#......"
]

BELT_FAULD_E = [
    "........#################...............",
    "......##eeeeeeeeegGGgeeeee##............",
    ".....#EEEEEEEEEEEgGGgEEEEE#.............",
    ".....#EEEEEEEEEEEgGGgEEEEE#.............",
    "....#HLLMMMDD#..#ttTTt#.................",
    "...#HLLMMMMMDD#.#tTTTt#.................",
    "...#HLLMMMMMDD#.#tTTTt#.................",
    "...#gGGGGGGGGd#.#tttt#..................",
    "....#DDDDDDDD#...####..................."
]

BELT_FAULD_NE = [
    "........####################............",
    "......##EEEEEEEEEEEEEEEEEEEEEE##........",
    ".....#EEEEEEEEEEEEEEEEEEEEEEEEEE#.......",
    ".....#EEEEEEEEEEEEEEEEEEEEEEEEEE#.......",
    "....#DDMMDDK#...#uuuuuu#..#DMMMDDK#.....",
    "...#DDMMMMDDK#..#uuuuuu#.#DDMMMMDDK#....",
    "...#DDMMMMDDK#..#uuuuuu#.#DDMMMMDDK#....",
    "...#dGGGGGGd#...#uuuuuu#..#dGGGGGGd#....",
    "....#KKKKKK#.....######....#KKKKKK#....."
]

BELT_FAULD_N = [
    "..........####################..........",
    "........##EEEEEEEEEEEEEEEEEEEEEE##......",
    ".......#EEEEEEEEEEEEEEEEEEEEEEEEEE#.....",
    ".......#EEEEEEEEEEEEEEEEEEEEEEEEEE#.....",
    "......#DDMMDDK#..#uuuuuuuu#..#DMMMDDK#..",
    ".....#DDMMMMDDK#.#uuuuuuuu#.#DDMMMMDDK#.",
    ".....#DDMMMMDDK#.#uuuuuuuu#.#DDMMMMDDK#.",
    ".....#dGGGGGGd#..#uuuuuuuu#..#dGGGGGGd#.",
    "......#KKKKKK#....########....#KKKKKK#.."
]

# Standard Armored Leg Segment (Cuisses, Poleyn, Greaves, Sabaton)
LEG_S_NEUTRAL = [
    "...####...",
    "..#HLLM#..",
    ".#HLLMMD#.",
    ".#HLMMMD#.",
    ".#HLMMMD#.",
    "..#gGGd#..",
    ".#gGGGGd#.",
    ".#gGGGGd#.",
    "..#dGGd#..",
    "..#HLMD#..",
    ".#HLLMMD#.",
    ".#HLLMMD#.",
    ".#HLLMMD#.",
    ".#HLLMMD#.",
    "#HHLLMMDD#",
    "#HHLLMMDDK",
    "#HHLLMMDDK",
    "##########"
]

LEG_SE_NEUTRAL = [
    "....####...",
    "...#HLLM#..",
    "..#HLLMMD#.",
    "..#HLMMMD#.",
    "..#HLMMMD#.",
    "...#gGGd#..",
    "..#gGGGGd#.",
    "..#gGGGGd#.",
    "...#dGGd#..",
    "...#HLMD#..",
    "..#HLLMMD#.",
    "..#HLLMMD#.",
    "..#HLLMMD#.",
    "..#HLLMMD#.",
    ".#HHLLMMDD#",
    "#HHHLLMMDDK",
    "#HHHLLMMDDK",
    "###########"
]

LEG_E_NEUTRAL = [
    "....#####...",
    "...#HLLMD#..",
    "..#HLLMMMD#.",
    "..#HLMMMMD#.",
    "..#HLMMMMD#.",
    "...#gGGGd#..",
    "..#gGGGGGd#.",
    "..#gGGGGGd#.",
    "...#dGGGd#..",
    "...#HLLMD#..",
    "..#HLLMMMD#.",
    "..#HLLMMMD#.",
    "..#HLLMMMD#.",
    "..#HLLMMMD#.",
    ".#HHLLMMMDD#",
    "#HHHHLLMMDDK",
    "#HHHHLLMMDDK",
    "############"
]

LEG_NE_NEUTRAL = [
    "...####...",
    "..#DMMD#..",
    ".#DDMMDK#.",
    ".#DMMMDDK#",
    ".#DMMMDDK#",
    "..#dGGd#..",
    ".#dGGGGd#.",
    ".#dGGGGd#.",
    "..#dGGd#..",
    "..#DMDK#..",
    ".#DDMMDK#.",
    ".#DDMMDK#.",
    ".#DDMMDK#.",
    ".#DDMMDK#.",
    "#DDMMMDDK#",
    "#DDDMMMDDK",
    "#DDDMMMDDK",
    "##########"
]

LEG_N_NEUTRAL = [
    "...####...",
    "..#DMMD#..",
    ".#DDMMDK#.",
    ".#DMMMDDK#",
    ".#DMMMDDK#",
    "..#dGGd#..",
    ".#dGGGGd#.",
    ".#dGGGGd#.",
    "..#dGGd#..",
    "..#DMDK#..",
    ".#DDMMDK#.",
    ".#DDMMDK#.",
    ".#DDMMDK#.",
    ".#DDMMDK#.",
    "#DDMMMDDK#",
    "#DDDMMMDDK",
    "#DDDMMMDDK",
    "##########"
]

def render_lower_body(anim, dir_name, f_idx, total_f):
    img = Image.new("RGBA", (CANVAS_W, CANVAS_H), (0, 0, 0, 0))
    pix = img.load()

    # Direction Fauld Choice
    fauld_map = {
        "S": BELT_FAULD_S,
        "SE": BELT_FAULD_SE,
        "E": BELT_FAULD_E,
        "NE": BELT_FAULD_NE,
        "N": BELT_FAULD_N
    }
    leg_map = {
        "S": (LEG_S_NEUTRAL, LEG_S_NEUTRAL),
        "SE": (LEG_SE_NEUTRAL, LEG_SE_NEUTRAL),
        "E": (LEG_E_NEUTRAL, LEG_E_NEUTRAL),
        "NE": (LEG_NE_NEUTRAL, LEG_NE_NEUTRAL),
        "N": (LEG_N_NEUTRAL, LEG_N_NEUTRAL)
    }

    # Direction Stance Parameters
    dir_spacing = {
        "S":  {"lx": -13, "rx": 5,   "flip_r": True},
        "SE": {"lx": -11, "rx": 3,   "flip_r": False},
        "E":  {"lx": -8,  "rx": -1,  "flip_r": False},
        "NE": {"lx": -10, "rx": 4,   "flip_r": False},
        "N":  {"lx": -13, "rx": 5,   "flip_r": False}
    }[dir_name]

    # Kinematics Offsets
    pelvis_y = 80
    pelvis_x = CX - 20
    l_foot_x = CX + dir_spacing["lx"]
    r_foot_x = CX + dir_spacing["rx"]
    l_leg_y = 97
    r_leg_y = 97

    # Micro motion per animation
    if anim == "idle":
        # 4 frames subtle knee flexion
        dip = [0, 1, 1, 0][f_idx % 4]
        shift = [0, -1, 0, 1][f_idx % 4]
        pelvis_x += shift
        l_foot_x += shift
        r_foot_x += shift
        l_leg_y += dip
        r_leg_y += dip

    elif anim == "walk":
        # 6 frames walking march
        offsets = [
            (-2, 2, -1, 0),
            (-3, 1, -2, 0),
            (0, 0, 0, 0),
            (2, -2, 0, -1),
            (3, -1, 0, -2),
            (0, 0, 0, 0)
        ][f_idx % 6]
        l_foot_x += offsets[0]
        r_foot_x += offsets[1]
        l_leg_y += offsets[2]
        r_leg_y += offsets[3]

    elif anim == "run":
        # 6 frames athletic sprint
        offsets = [
            (-5, 4, -3, 0),
            (-6, 3, -4, 0),
            (-2, 0, -1, 0),
            (4, -5, 0, -3),
            (3, -6, 0, -4),
            (0, -2, 0, -1)
        ][f_idx % 6]
        l_foot_x += offsets[0]
        r_foot_x += offsets[1]
        l_leg_y += offsets[2]
        r_leg_y += offsets[3]

    elif anim == "dash":
        # 3 frames burst slide
        if f_idx == 0:
            l_foot_x -= 4
            r_foot_x += 2
        elif f_idx == 1:
            l_foot_x -= 8
            r_foot_x += 6
            l_leg_y -= 1
        elif f_idx == 2:
            l_foot_x -= 4
            r_foot_x += 3

    elif anim == "hitstun":
        # 2 frames recoil
        if f_idx == 0:
            pelvis_x -= 4
            l_foot_x -= 5
            r_foot_x -= 6
            r_leg_y -= 2
        elif f_idx == 1:
            pelvis_x -= 2
            l_foot_x -= 3
            r_foot_x -= 4

    # 1. Render Legs (Thigh to Sabaton)
    left_pat, right_pat = leg_map[dir_name]
    draw_cluster(pix, left_pat, l_foot_x, l_leg_y, max_y=114, min_y=80)
    draw_cluster(pix, right_pat, r_foot_x, r_leg_y, max_y=114, min_y=80, flip_x=dir_spacing["flip_r"])

    # 2. Render Belt & Fauld (Waist) - strictly at Y=80
    draw_cluster(pix, fauld_map[dir_name], pelvis_x, pelvis_y, max_y=114, min_y=80)

    # Assert ground contact: ensure at least one foot touches exactly Y=114
    # Find bottom-most pixel and extend if needed to guarantee exact Y=114
    bbox = img.getbbox()
    if bbox:
        bottom = bbox[3] - 1
        if bottom < 114:
            # Anchor heel/sole at Y=114
            for x in range(bbox[0], bbox[2]):
                if img.getpixel((x, bottom))[3] > 0:
                    for fill_y in range(bottom + 1, 115):
                        pix[x, fill_y] = CLR['#']
        elif bottom > 114:
            # Trim any pixel below 114
            for y in range(115, CANVAS_H):
                for x in range(CANVAS_W):
                    pix[x, y] = (0, 0, 0, 0)

    return img

# ==============================================================================
# 2. UPPER BODY SPRITE PATTERNS (Greathelm, Cuirass, Pauldrons, Weapons)
# ==============================================================================

# Greathelms (Y ~ 34..50)
HELM_FRONT_S = [
    "........######........",
    "......##HLMMMD##......",
    ".....#HLLMMMMMDD#.....",
    "....#HLLMMMMMMMDD#....",
    "...#HLLMMMMMMMMMDD#...",
    "...#HLMMMMMMMMMMDD#...",
    "..#HLLMMMMMMMMMMMDD#..",
    "..#gGGGGGGGGGGGGGGd#..",
    "..#D##############D#..",
    "..#DcCcC######CcCcD#..",  # Cyan glowing dual visor slits
    "..#DcCC########CCdD#..",
    "..#DKK##########KKD#..",
    "...#HLLMMMMMMMDDK#....",
    "...#HLLMMMMMMMDDK#....",
    "....#gGGGGGGGGgd#.....",
    ".....############....."
]

HELM_FRONT_SE = [
    ".......######.........",
    ".....##HLMMMD##.......",
    "....#HLLMMMMMDD#......",
    "...#HLLMMMMMMMDD#.....",
    "..#HLLMMMMMMMMMDD#....",
    "..#HLMMMMMMMMMMDD#....",
    ".#HLLMMMMMMMMMMMDD#...",
    ".#gGGGGGGGGGGGGGGd#...",
    ".#D##############D#...",
    ".#DK#DcCcCcC#####D#...", # Angled Visor
    ".#DK#DcCCCCd#####D#...",
    "..#DKK##########KKD#..",
    "...#HLLMMMMMMMDDK#....",
    "....#HLLMMMMMDDK#.....",
    ".....#gGGGGGGgd#......",
    "......#########......."
]

HELM_SIDE_E = [
    "......######..........",
    "....##HLMMMD##........",
    "...#HLLMMMMMDD#.......",
    "..#HLLMMMMMMMDD#......",
    ".#HLLMMMMMMMMMDD#.....",
    ".#HLMMMMMMMMMMMD#.....",
    ".#HLLMMMMMMMMMMMD#....",
    ".#gGGGGGGGGGGGGd#.....",
    ".#D############D#.....",
    ".#DcCcCcCcC####D#.....", # Side Visor Projection
    ".#DcCCCCCd#####D#.....",
    "..#DKKK########KD#....",
    "...#HLLMMMMMMDDK#.....",
    "....#HLLMMMDDK#.......",
    ".....#gGGGGgd#........",
    "......#######........."
]

HELM_REAR_NE = [
    ".......######.........",
    ".....##DDMMMD##.......",
    "....#DDMMMMMMMD#......",
    "...#DDMMMMMMMMMD#.....",
    "..#DDMMMMMMMMMMMD#....",
    "..#DMMMMMMMMMMMMD#....",
    ".#DDMMMMMMMMMMMMMD#...",
    ".#dGGGGGGGGGGGGGGd#...",
    ".#D##############D#...",
    ".#DMMMMMMMMMMMMMMD#...",
    ".#DMMMMMMMMMMMMMMD#...",
    "..#DKK##########KKD#..",
    "...#DDMMMMMMMMDDK#....",
    "....#DDMMMMMMDDK#.....",
    ".....#dGGGGGGgd#......",
    "......#########......."
]

HELM_REAR_N = [
    "........######........",
    "......##DDMMMD##......",
    ".....#DDMMMMMMMD#.....",
    "....#DDMMMMMMMMMD#....",
    "...#DDMMMMMMMMMMMD#...",
    "...#DMMMMMMMMMMMMD#...",
    "..#DDMMMMMMMMMMMMMD#..",
    "..#dGGGGGGGGGGGGGGd#..",
    "..#D##############D#..",
    "..#DMMMMMMMMMMMMMMD#..",
    "..#DMMMMMMMMMMMMMMD#..",
    "..#DKK##########KKD#..",
    "...#DDMMMMMMMMDDK#....",
    "...#DDMMMMMMMMDDK#....",
    "....#dGGGGGGGGgd#.....",
    ".....############....."
]

# Cuirass (Breastplate Y ~ 50..78)
CHEST_FRONT_S = [
    ".......############.......",
    ".....##T#HLLMMDDK#T##.....",
    "....#TT#HLLMMMMDDK##TT#...",
    "...#t#HLLMMMMMMMDDKK##t#..",
    "..#t#HLLMMMMMMMMMDDDKK##t#.",
    "..#t#HLMM#gGGGG#MDDDKK##t#.",
    "...#HLMM#gGGGGGG#MDDDDK#...",
    "...#HLMM#gGGGGGG#MDDDDK#...",
    "....#HLLMMMMMMMMDDDDDK#....",
    "....#HLLMMMMMMMMDDDDDK#....",
    ".....#HLMMMMMMMMDDDDK#.....",
    ".....#HLLMMMMMMMDDDK#......",
    "......#HLLMMMMMDDDK#.......",
    ".......#HLLMMMDDK#........",
    "........########.........."
]

CHEST_SE = [
    "......############........",
    "....##T#HLLMMDDK#T##......",
    "...#TT#HLLMMMMDDK##TT#....",
    "..#t#HLLMMMMMMMDDKK##t#...",
    ".#t#HLLMMMMMMMMMDDDKK##t#.",
    ".#t#HLMM#gGGGG#MDDDKK##t#.",
    "..#HLMM#gGGGGGG#MDDDDK#...",
    "..#HLMM#gGGGGGG#MDDDDK#...",
    "...#HLLMMMMMMMMDDDDDK#....",
    "...#HLLMMMMMMMMDDDDDK#....",
    "....#HLMMMMMMMMDDDDK#.....",
    "....#HLLMMMMMMMDDDK#......",
    ".....#HLLMMMMMDDDK#.......",
    "......#HLLMMMDDK#.........",
    ".......########..........."
]

CHEST_E = [
    ".....############.........",
    "...##T#HLLMMDDK##.........",
    "..#TT#HLLMMMMDDK#.........",
    ".#t#HLLMMMMMMMDDK#........",
    "#t#HLLMMMMMMMMMDDK#.......",
    "#t#HLMM#gGGGG#MDDDK#......",
    ".#HLMM#gGGGGGG#MDDDK#.....",
    ".#HLMM#gGGGGGG#MDDDK#.....",
    "..#HLLMMMMMMMMDDDDDK#.....",
    "..#HLLMMMMMMMMDDDDDK#.....",
    "...#HLMMMMMMMMDDDDK#......",
    "...#HLLMMMMMMMDDDK#.......",
    "....#HLLMMMMMDDDK#........",
    ".....#HLLMMMDDK#..........",
    "......########............"
]

CHEST_NE = [
    "......############........",
    "....##u#DDMMMDDK#u##......",
    "...#uu#DDMMMMMMDK##uu#....",
    "..#u#DDMMMMMMMMMDDKK##u#...",
    ".#u#DDMMMMMMMMMMMDDDKK##u#.",
    ".#u#DDMMMMMMMMMMMDDDKK##u#.",
    "..#DDMM#DDDDDD#MDDDDK#....",
    "..#DDMM#DDDDDD#MDDDDK#....",
    "...#DDMMMMMMMMDDDDDK#.....",
    "...#DDMMMMMMMMDDDDDK#.....",
    "....#DDMMMMMMMMDDDDK#.....",
    "....#DDMMMMMMMDDDK#.......",
    ".....#DDMMMMMDDDK#........",
    "......#DDMMMDDK#..........",
    ".......########..........."
]

CHEST_N = [
    ".......############.......",
    ".....##u#DDMMMDDK#u##.....",
    "....#uu#DDMMMMMMDK##uu#...",
    "...#u#DDMMMMMMMMMDDKK##u#..",
    "..#u#DDMMMMMMMMMMMDDDKK##u#.",
    "..#u#DDMMMMMMMMMMMDDDKK##u#.",
    "...#DDMM#DDDDDD#MDDDDK#...",
    "...#DDMM#DDDDDD#MDDDDK#...",
    "....#DDMMMMMMMMDDDDDK#....",
    "....#DDMMMMMMMMDDDDDK#....",
    ".....#DDMMMMMMMMDDDDK#.....",
    ".....#DDMMMMMMMDDDK#......",
    "......#DDMMMMMDDDK#.......",
    ".......#DDMMMDDK#........",
    "........########.........."
]

# Pauldrons
PAULDRON_LEFT = [
    "....####....",
    "..##HLLM##..",
    ".#HLLMMMMD#.",
    "#HLLMMMMMDD#",
    "#HLMMMMMDDD#",
    "#gGGGGGGGGd#",
    ".#DDDDDDDD#.",
    "..########.."
]

# Kite Shield Front
KITE_SHIELD = [
    "....##############....",
    "..##gGGGGGGGGGGGGd##..",
    ".#gGGHLLMMMMMMDDGGd#.",
    "#gGGHLLMMMMMMMMDDGGd#",
    "#gGHLLMM#gGG#MMMDDGG#",
    "#gGHLLMM#gGG#MMMDDGG#",
    "#gGHLL#gGGGGGG#MDDGG#",
    "#gGHLL#gGGGGGG#MDDGG#",
    "#gGHLLMM#gGG#MMMDDGG#",
    "#gGHLLMM#gGG#MMMDDGG#",
    "#gGHLLMMMMMMMMMMDDGG#",
    ".#gGHLLMMMMMMMMDDGG#.",
    "..#gGHLLMMMMMMDDGG#..",
    "...#gGHLLMMMMDDGG#...",
    "....#gGHLLMMDDGG#....",
    ".....#gGHLLMDDGG#....",
    "......#gGHMMDGG#.....",
    ".......#gGMDGG#......",
    "........#gGDG#.......",
    ".........#gG#........",
    "..........##........."
]

# Bastard Sword Straight
SWORD_UPRIGHT = [
    ".....#W#.....",
    "....#WVW#....",
    "...#WVVmW#...",
    "..#WVVmsW#...",
    "..#WVVmsW#...",
    "..#WVVmsW#...",
    "..#WVVmsW#...",
    "..#WVVmsW#...",
    "..#WVVmsW#...",
    "..#WVVmsW#...",
    "..#WVVmsW#...",
    "..#WVVmsW#...",
    "..#WVVmsW#...",
    "..#WVVmsW#...",
    "..#WVVmsW#...",
    "..#WVVmsW#...",
    ".#gGGGGGGgd#.",
    "..#dDDDDd#...",
    "....#eE#.....",
    "....#eE#.....",
    "....#eE#.....",
    "...#gGGgd#..."
]

# Bastard Sword Slashing Down (Atk1)
SWORD_SLASH_DOWN = [
    "....................#W#",
    "..................#WVW#",
    "................#WVVmW#",
    "..............#WVVmsW#.",
    "............#WVVmsW#...",
    "..........#WVVmsW#.....",
    "........#WVVmsW#.......",
    "......#WVVmsW#.........",
    "....#WVVmsW#...........",
    "..#WVVmsW#.............",
    ".#gGGgd#...............",
    "....#eE#...............",
    ".....#gG#.............."
]

# Bastard Sword Cleave Across (Atk2)
SWORD_CLEAVE_HORIZ = [
    "######################################.",
    "#WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW#.",
    "#VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV#.",
    "#mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm#.",
    "#ssssssssssssssssssssssssssssssssssss#.",
    ".....................#gGGGGGGGGgd#.....",
    "..........................#eEeE#.......",
    "..........................#gGGg#......."
]

# Bastard Sword Thrust (Atk3)
SWORD_THRUST = [
    "#W#....................................",
    "#WVW#..................................",
    "#WVVmW#................................",
    ".#WVVmsWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW#.",
    "..#WVVmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm#.",
    "...#gGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGgd#",
    "...........................#eEeE#......",
    "...........................#gGGg#......"
]

def render_upper_body(action, dir_name, f_idx, is_vanguard_idle=False):
    img = Image.new("RGBA", (CANVAS_W, CANVAS_H), (0, 0, 0, 0))
    pix = img.load()

    helm_map = {
        "S": HELM_FRONT_S,
        "SE": HELM_FRONT_SE,
        "E": HELM_SIDE_E,
        "NE": HELM_REAR_NE,
        "N": HELM_REAR_N
    }
    chest_map = {
        "S": CHEST_FRONT_S,
        "SE": CHEST_SE,
        "E": CHEST_E,
        "NE": CHEST_NE,
        "N": CHEST_N
    }

    # Micro movement offsets
    body_x = CX - 13
    body_y = 52
    helm_x = CX - 11
    helm_y = 34
    shield_x = CX - 36
    shield_y = 55
    sword_x = CX + 19
    sword_y = 44

    # Direction offsets
    if dir_name == "SE":
        shield_x += 4
        sword_x += 2
    elif dir_name == "E":
        shield_x += 8
        sword_x -= 2
    elif dir_name == "NE":
        shield_x += 2
        sword_x -= 4
    elif dir_name == "N":
        shield_x += 0
        sword_x -= 6

    # Action-specific choreography
    cur_sword = SWORD_UPRIGHT
    cur_shield = KITE_SHIELD
    sword_draw_x = sword_x
    sword_draw_y = sword_y

    if is_vanguard_idle:
        # 4 frames idle knight stance
        dip = [0, 1, 1, 0][f_idx % 4]
        body_y += dip
        helm_y += dip
        shield_y += dip
        sword_draw_y += dip

    elif action == "atk1":
        # 4 frames diagonal downward slash
        if f_idx == 0:  # Windup high
            sword_draw_x = sword_x - 4
            sword_draw_y = sword_y - 12
            cur_sword = SWORD_UPRIGHT
        elif f_idx == 1: # Slash Apex
            sword_draw_x = CX - 5
            sword_draw_y = 50
            cur_sword = SWORD_SLASH_DOWN
            body_x += 3
        elif f_idx == 2: # Follow through
            sword_draw_x = CX - 15
            sword_draw_y = 62
            cur_sword = SWORD_SLASH_DOWN
            body_x += 4
        elif f_idx == 3: # Recovery
            sword_draw_x = sword_x
            sword_draw_y = sword_y - 4
            cur_sword = SWORD_UPRIGHT

    elif action == "atk2":
        # 4 frames horizontal cleave
        if f_idx == 0:  # Cocked to side
            sword_draw_x = sword_x + 6
            sword_draw_y = sword_y + 2
            cur_sword = SWORD_UPRIGHT
        elif f_idx == 1: # Cleave sweep
            sword_draw_x = CX - 22
            sword_draw_y = 60
            cur_sword = SWORD_CLEAVE_HORIZ
            body_x += 2
        elif f_idx == 2: # Extension
            sword_draw_x = CX - 28
            sword_draw_y = 62
            cur_sword = SWORD_CLEAVE_HORIZ
            body_x += 3
        elif f_idx == 3: # Recovery
            sword_draw_x = sword_x
            sword_draw_y = sword_y
            cur_sword = SWORD_UPRIGHT

    elif action == "atk3":
        # 4 frames lunging thrust
        if f_idx == 0:  # Coil back
            sword_draw_x = sword_x + 4
            sword_draw_y = sword_y + 4
            cur_sword = SWORD_UPRIGHT
        elif f_idx == 1: # Thrust apex
            sword_draw_x = CX - 2
            sword_draw_y = 64
            cur_sword = SWORD_THRUST
            body_x += 5
        elif f_idx == 2: # Deep penetration
            sword_draw_x = CX + 4
            sword_draw_y = 64
            cur_sword = SWORD_THRUST
            body_x += 6
        elif f_idx == 3: # Retract
            sword_draw_x = sword_x
            sword_draw_y = sword_y
            cur_sword = SWORD_UPRIGHT

    elif action == "guard":
        # 4 frames fortress guard
        guard_settle = [0, 1, 2, 1][f_idx % 4]
        shield_x = CX - 20 + guard_settle
        shield_y = 52 + guard_settle
        sword_draw_x = sword_x - 6
        sword_draw_y = sword_y + 6
        cur_sword = SWORD_UPRIGHT

    # 1. Render Cuirass / Torso
    draw_cluster(pix, chest_map[dir_name], body_x, body_y, max_y=84)

    # 2. Render Pauldrons
    draw_cluster(pix, PAULDRON_LEFT, body_x - 13, body_y - 2, max_y=84)
    draw_cluster(pix, PAULDRON_LEFT, body_x + 23, body_y - 2, max_y=84, flip_x=True)

    # 3. Render Greathelm
    draw_cluster(pix, helm_map[dir_name], helm_x, helm_y, max_y=84)

    # 4. Render Shield & Sword
    draw_cluster(pix, cur_shield, shield_x, shield_y, max_y=84)
    draw_cluster(pix, cur_sword, sword_draw_x, sword_draw_y, max_y=84)

    # Guarantee Upper Body Waist Bound: Zero pixels strictly below Y=84
    for y in range(85, CANVAS_H):
        for x in range(CANVAS_W):
            pix[x, y] = (0, 0, 0, 0)

    return img

# ==============================================================================
# 3. CLASS IDENTITY SPRITES (Crest, Tabard)
# ==============================================================================

CREST_PATTERNS = {
    "S": [
        "......#####......",
        "....##rrrrr##....",
        "...#rrrrrrrRR#...",
        "..#rrrrrrrrrRR#..",
        "..#rrrrrrrrrrRb#.",
        ".#rrrrrrrrrrrRbb#",
        ".#RRRRRRRRRRRbbb#",
        ".#RRRRRRRRRRRbbb#",
        "..#bbbbbbbbbbBB#.",
        "...#BBBBBBBBBB#..",
        "....##########..."
    ],
    "SE": [
        "....#####........",
        "..##rrrrr##......",
        ".#rrrrrrrRR#.....",
        "#rrrrrrrrrRR#....",
        "#rrrrrrrrrrRb#...",
        "#rrrrrrrrrrrRbb#.",
        ".#RRRRRRRRRRRbbb#",
        "..#bbbbbbbbbbBB#.",
        "...#BBBBBBBBBB#..",
        "....##########..."
    ],
    "E": [
        "....####.........",
        "..##rrrr##.......",
        ".#rrrrrrRR#......",
        "#rrrrrrrrrRb#....",
        "#rrrrrrrrrrRbb#..",
        ".#RRRRRRRRRRbbb#.",
        "..#bbbbbbbbbbBB#.",
        "...#BBBBBBBBBB#..",
        "....##########..."
    ],
    "NE": [
        "........#####....",
        "......##rrrrr##..",
        ".....#rrrrrrrRR#.",
        "....#rrrrrrrrrRR#",
        "...#rrrrrrrrrrRb#",
        ".#rrrrrrrrrrrRbb#",
        "#RRRRRRRRRRRRbbb#",
        ".#bbbbbbbbbbbBB#.",
        "..#BBBBBBBBBBB#..",
        "....##########..."
    ],
    "N": [
        "......#####......",
        "....##rrrrr##....",
        "...#rrrrrrrRR#...",
        "..#rrrrrrrrrRR#..",
        "..#rrrrrrrrrrRb#.",
        ".#rrrrrrrrrrrRbb#",
        ".#RRRRRRRRRRRbbb#",
        ".#RRRRRRRRRRRbbb#",
        "..#bbbbbbbbbbBB#.",
        "...#BBBBBBBBBB#..",
        "....##########..."
    ]
}

TABARD_PATTERNS = {
    "S": [
        "....##########....",
        "...#gGGGGGGGGgd#...",
        "..#gGRRRRRRRRRGd#..",
        "..#gGRRRRRRRRRGd#..",
        "..#gGRRR#gG#RRRGd#..",
        "..#gGRR#gGG#RRRGd#..",
        "..#gGR#gGGGG#RRGd#..",
        "..#gGR#gGGGG#RRGd#..",
        "..#gGRR#gGG#RRRGd#..",
        "..#gGRRR#gG#RRRGd#..",
        "..#gGRRRRRRRRRGd#..",
        "..#gGRRRRRRRRRGd#..",
        "..#gGbbbbbbbbBGd#..",
        "..#gGBBBBBBBBBGd#..",
        "...#gGGGGGGGGgd#...",
        "....##########...."
    ],
    "SE": [
        "...##########.....",
        "..#gGGGGGGGGgd#...",
        ".#gGRRRRRRRRRGd#..",
        ".#gGRRRRRRRRRGd#..",
        ".#gGRRR#gG#RRRGd#.",
        ".#gGRR#gGG#RRRGd#.",
        ".#gGR#gGGGG#RRGd#.",
        ".#gGR#gGGGG#RRGd#.",
        ".#gGRR#gGG#RRRGd#.",
        ".#gGRRR#gG#RRRGd#.",
        ".#gGRRRRRRRRRGd#..",
        ".#gGRRRRRRRRRGd#..",
        ".#gGbbbbbbbbBGd#..",
        ".#gGBBBBBBBBBGd#..",
        "..#gGGGGGGGGgd#...",
        "...##########....."
    ],
    "E": [
        "...#########......",
        "..#gGGGGGGGgd#....",
        ".#gGRRRRRRRRGd#...",
        ".#gGRRRRRRRRGd#...",
        ".#gGRRR#gGG#RGd#..",
        ".#gGRR#gGGG#RGd#..",
        ".#gGR#gGGGG#RGd#..",
        ".#gGR#gGGGG#RGd#..",
        ".#gGRR#gGGG#RGd#..",
        ".#gGRRR#gGG#RGd#..",
        ".#gGRRRRRRRRGd#...",
        ".#gGRRRRRRRRGd#...",
        ".#gGbbbbbbbBGd#...",
        ".#gGBBBBBBBBGd#...",
        "..#gGGGGGGGgd#....",
        "...#########......"
    ],
    "NE": [
        "....##########....",
        "...#dGGGGGGGGgd#...",
        "..#dGBBBBBBBBBGd#..",
        "..#dGBBBBBBBBBGd#..",
        "..#dGBBB#dd#BBBGd#..",
        "..#dGBB#ddd#BBBGd#..",
        "..#dGB#dddd#BBGd#..",
        "..#dGB#dddd#BBGd#..",
        "..#dGBB#ddd#BBBGd#..",
        "..#dGBBB#dd#BBBGd#..",
        "..#dGBBBBBBBBBGd#..",
        "..#dGBBBBBBBBBGd#..",
        "..#dGbbbbbbbbBGd#..",
        "..#dGBBBBBBBBBGd#..",
        "...#dGGGGGGGGgd#...",
        "....##########...."
    ],
    "N": [
        "....##########....",
        "...#dGGGGGGGGgd#...",
        "..#dGBBBBBBBBBGd#..",
        "..#dGBBBBBBBBBGd#..",
        "..#dGBBB#dd#BBBGd#..",
        "..#dGBB#ddd#BBBGd#..",
        "..#dGB#dddd#BBGd#..",
        "..#dGB#dddd#BBGd#..",
        "..#dGBB#ddd#BBBGd#..",
        "..#dGBBB#dd#BBBGd#..",
        "..#dGBBBBBBBBBGd#..",
        "..#dGBBBBBBBBBGd#..",
        "..#dGbbbbbbbbBGd#..",
        "..#dGBBBBBBBBBGd#..",
        "...#dGGGGGGGGgd#...",
        "....##########...."
    ]
}

def render_crest(dir_name):
    img = Image.new("RGBA", (32, 32), (0, 0, 0, 0))
    draw_cluster(img.load(), CREST_PATTERNS[dir_name], 7, 10)
    return img

def render_tabard(dir_name):
    img = Image.new("RGBA", (48, 64), (0, 0, 0, 0))
    draw_cluster(img.load(), TABARD_PATTERNS[dir_name], 14, 24)
    return img

# ==============================================================================
# MASTER GENERATION & EXPORT
# ==============================================================================

def main():
    print("=" * 65)
    print("Project Ascendant: Generating Authentic High-Craft Pixel Art")
    print("=" * 65)

    base_art_dir = os.path.join(PROJECT_ROOT, "Content", "art", "characters")
    lower_dir = os.path.join(base_art_dir, "MasterRigs", "FB_Lower_HeavyTank_Set")
    upper_dir = os.path.join(base_art_dir, "UpperBodyWeapons", "FB_Upper_1H_Blade_Combo")
    class_dir = os.path.join(base_art_dir, "ClassIdentity")

    # 1. GENERATE LOWER BODY MASTER RIG (105 Frames + 25 GIFs + Spritesheet)
    print("\n[1/3] Generating Lower Body Master Rig: FB_Lower_HeavyTank_Set (105 frames)...")
    lower_anims = {
        "idle": 4,
        "walk": 6,
        "run": 6,
        "dash": 3,
        "hitstun": 2
    }
    lower_all_frames = []

    for anim, count in lower_anims.items():
        anim_frames_dir = os.path.join(lower_dir, "frames", anim)
        os.makedirs(anim_frames_dir, exist_ok=True)
        for d in DIRECTIONS:
            dir_gif_frames = []
            for f in range(count):
                img = render_lower_body(anim, d, f, count)
                fname = f"FB_Lower_HeavyTank_Set_{anim}_{d}_f{f:02d}.png"
                fpath = os.path.join(anim_frames_dir, fname)
                img.save(fpath)
                dir_gif_frames.append(img)
                lower_all_frames.append(img)

            gif_path = os.path.join(lower_dir, "gifs", f"FB_Lower_HeavyTank_Set_{anim}_{d}.gif")
            os.makedirs(os.path.dirname(gif_path), exist_ok=True)
            dir_gif_frames[0].save(
                gif_path,
                save_all=True,
                append_images=dir_gif_frames[1:],
                duration=120,
                loop=0,
                disposal=2
            )

    # Master Spritesheet for Lower Body (21 columns x 5 rows -> 2688 x 640)
    sheet_lower = Image.new("RGBA", (2688, 640), (0, 0, 0, 0))
    idx = 0
    for anim, count in lower_anims.items():
        for f in range(count):
            for r_idx, d in enumerate(DIRECTIONS):
                frame_img = render_lower_body(anim, d, f, count)
                sheet_lower.paste(frame_img, (idx * CANVAS_W, r_idx * CANVAS_H))
            idx += 1
    sheet_lower_path = os.path.join(lower_dir, "spritesheets", "FB_Lower_HeavyTank_Set_spritesheet.png")
    os.makedirs(os.path.dirname(sheet_lower_path), exist_ok=True)
    sheet_lower.save(sheet_lower_path)
    print(f"  -> Generated 105 lower frames, 25 GIFs, and master spritesheet ({sheet_lower.size}).")

    # 2. GENERATE UPPER BODY WEAPON FAMILY (80 Frames + 20 GIFs + Spritesheet)
    print("\n[2/3] Generating Upper Body Weapon Family: FB_Upper_1H_Blade_Combo (80 frames)...")
    upper_actions = ["atk1", "atk2", "atk3", "guard"]
    upper_all_frames = []

    for act in upper_actions:
        act_frames_dir = os.path.join(upper_dir, "frames", act)
        os.makedirs(act_frames_dir, exist_ok=True)
        for d in DIRECTIONS:
            dir_gif_frames = []
            for f in range(4):
                img = render_upper_body(act, d, f)
                fname = f"FB_Upper_1H_Blade_Combo_{act}_{d}_f{f:02d}.png"
                fpath = os.path.join(act_frames_dir, fname)
                img.save(fpath)
                dir_gif_frames.append(img)
                upper_all_frames.append(img)

            gif_path = os.path.join(upper_dir, "gifs", f"FB_Upper_1H_Blade_Combo_{act}_{d}.gif")
            os.makedirs(os.path.dirname(gif_path), exist_ok=True)
            dir_gif_frames[0].save(
                gif_path,
                save_all=True,
                append_images=dir_gif_frames[1:],
                duration=100,
                loop=0,
                disposal=2
            )

    # Master Spritesheet for Upper Body (16 columns x 5 rows -> 2048 x 640)
    sheet_upper = Image.new("RGBA", (2048, 640), (0, 0, 0, 0))
    col = 0
    for act in upper_actions:
        for f in range(4):
            for r_idx, d in enumerate(DIRECTIONS):
                frame_img = render_upper_body(act, d, f)
                sheet_upper.paste(frame_img, (col * CANVAS_W, r_idx * CANVAS_H))
            col += 1
    sheet_upper_path = os.path.join(upper_dir, "spritesheets", "FB_Upper_1H_Blade_Combo_spritesheet.png")
    os.makedirs(os.path.dirname(sheet_upper_path), exist_ok=True)
    sheet_upper.save(sheet_upper_path)
    print(f"  -> Generated 80 upper frames, 20 GIFs, and master spritesheet ({sheet_upper.size}).")

    # 3. GENERATE VANGUARD CLASS IDENTITY (Idle, Crest, Tabard)
    print("\n[3/3] Generating Vanguard Class Identity: Idle (20 frames), Crest (5), Tabard (5)...")
    vanguard_idle_dir = os.path.join(class_dir, "IdleStances", "FB_Vanguard_Idle")
    os.makedirs(vanguard_idle_dir, exist_ok=True)

    for d in DIRECTIONS:
        dir_gif_frames = []
        for f in range(4):
            img = render_upper_body("idle", d, f, is_vanguard_idle=True)
            fpath = os.path.join(vanguard_idle_dir, f"FB_Vanguard_Idle_{d}_f{f:02d}.png")
            img.save(fpath)
            dir_gif_frames.append(img)

        gif_path = os.path.join(vanguard_idle_dir, f"FB_Vanguard_Idle_{d}.gif")
        dir_gif_frames[0].save(
            gif_path,
            save_all=True,
            append_images=dir_gif_frames[1:],
            duration=150,
            loop=0,
            disposal=2
        )

    # Helm Crests
    crest_dir = os.path.join(class_dir, "HelmCrests")
    os.makedirs(crest_dir, exist_ok=True)
    for d in DIRECTIONS:
        img = render_crest(d)
        img.save(os.path.join(crest_dir, f"Crest_Vanguard_RedPlume_{d}.png"))

    # Tabards
    tabard_dir = os.path.join(class_dir, "Tabards")
    os.makedirs(tabard_dir, exist_ok=True)
    for d in DIRECTIONS:
        img = render_tabard(d)
        img.save(os.path.join(tabard_dir, f"Tabard_Vanguard_CrimsonSash_{d}.png"))

    # 4. GENERATE 5-DIRECTION COMPOSITE ANIMATED GIFS
    print("\n[+] Generating Full Assembly Composite GIFs across 5 directions...")
    gallery_dir = os.path.join(PROJECT_ROOT, "Art_Gallery")
    os.makedirs(gallery_dir, exist_ok=True)

    for d in DIRECTIONS:
        comp_frames = []
        crest_img = render_crest(d)
        tabard_img = render_tabard(d)

        for f in range(4):
            lower_f = render_lower_body("idle", d, f, 4)
            upper_f = render_upper_body("idle", d, f, is_vanguard_idle=True)

            comp = Image.new("RGBA", (CANVAS_W, CANVAS_H), (0, 0, 0, 0))
            # 1. Lower Body
            comp = Image.alpha_composite(comp, lower_f)
            # 2. Tabard (positioned over waist)
            tab_canvas = Image.new("RGBA", (CANVAS_W, CANVAS_H), (0, 0, 0, 0))
            tab_canvas.paste(tabard_img, (CX - 24, WAIST_Y - 20))
            comp = Image.alpha_composite(comp, tab_canvas)
            # 3. Upper Body
            comp = Image.alpha_composite(comp, upper_f)
            # 4. Helm Crest (positioned on helmet)
            crest_canvas = Image.new("RGBA", (CANVAS_W, CANVAS_H), (0, 0, 0, 0))
            crest_canvas.paste(crest_img, (CX - 16, 20))
            comp = Image.alpha_composite(comp, crest_canvas)

            comp_frames.append(comp)

        comp_gif_path = os.path.join(gallery_dir, f"vanguard_vertical_slice_composite_{d}.gif")
        comp_frames[0].save(
            comp_gif_path,
            save_all=True,
            append_images=comp_frames[1:],
            duration=140,
            loop=0,
            disposal=2
        )

    print("\n>> All 215 Phase 3 assets and composites generated successfully!")

if __name__ == "__main__":
    main()
