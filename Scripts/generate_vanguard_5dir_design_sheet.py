#!/usr/bin/env python3
"""
generate_vanguard_5dir_design_sheet.py
Mortal Ascension - Phase 2 Character Style Finalization
Generates the 5-direction Vanguard 'Iron Bastion' design sheet according to:
- Benchmarks: Octopath Traveler II, Dark Souls / Elden Ring, Children of Morta, Triangle Strategy
- SPEC-ART-2026-09-23-V2 §5.3 (3.2-3.5 heads, anchor (64, 114), 4-tone palette)
- 5 directions: S (0°), SE (45°), E (90°), NE (135°), N (180°)
- Generates 128x128 native frames, combined 5-dir sheet, 32x32 crop, 16x16 icon
- Generates interactive HTML review tool at Art_Gallery/vanguard_phase2_design_sheet_review.html
"""

import os
import math
from PIL import Image, ImageDraw

OUTPUT_DIR = "Art_Gallery"
os.makedirs(OUTPUT_DIR, exist_ok=True)

# 4-Tone Palettes (SPEC-ART V2 §5.3)
PALETTE = {
    # Blackened Steel Armor
    "STEEL_SPEC": (224, 230, 237, 255),    # Tone 1: Specular highlight
    "STEEL_MID":  (122, 136, 155, 255),    # Tone 2: Base iron
    "STEEL_SHAD": (55, 63, 77, 255),       # Tone 3: Navy core shadow
    "STEEL_DEEP": (25, 29, 36, 255),       # Tone 4: Charcoal ambient occlusion

    # Worn Azure Tabard Cloth
    "CLOTH_SPEC": (91, 138, 181, 255),     # Tone 1
    "CLOTH_MID":  (50, 82, 123, 255),      # Tone 2
    "CLOTH_SHAD": (30, 47, 71, 255),       # Tone 3
    "CLOTH_DEEP": (16, 24, 38, 255),       # Tone 4

    # Bronze / Brass Rivets & Cross Emblem
    "BRONZE_SPEC": (245, 208, 110, 255),
    "BRONZE_MID":  (184, 138, 48, 255),
    "BRONZE_SHAD": (115, 78, 26, 255),

    # Leather Belts & Boots
    "LEATHER_MID":  (84, 52, 34, 255),
    "LEATHER_SHAD": (48, 28, 18, 255),

    # Visor Slit / Shadow
    "VISOR_GLOW": (255, 180, 70, 255),
    "DEEP_VOID":  (10, 10, 14, 255),
}

def draw_vanguard_direction(direction_name):
    """
    Renders Vanguard 'Iron Bastion' on 128x128 canvas.
    Anchor is strictly grounded at (64, 114).
    Height: ~70px (head 20px, torso 24px, legs 26px = ~3.5 heads).
    """
    img = Image.new("RGBA", (128, 128), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)

    cx, ground_y = 64, 114

    if direction_name == "S":
        # -------------------------------------------------------------
        # SOUTH (0° - Directly facing front)
        # -------------------------------------------------------------
        # Legs / Greaves
        # Left leg
        d.rectangle([cx - 12, ground_y - 26, cx - 4, ground_y], fill=PALETTE["STEEL_MID"])
        d.rectangle([cx - 12, ground_y - 26, cx - 10, ground_y], fill=PALETTE["STEEL_SPEC"])
        d.rectangle([cx - 6, ground_y - 26, cx - 4, ground_y], fill=PALETTE["STEEL_SHAD"])
        d.rectangle([cx - 14, ground_y - 6, cx - 2, ground_y], fill=PALETTE["STEEL_DEEP"]) # Sabaton boot
        # Right leg
        d.rectangle([cx + 4, ground_y - 26, cx + 12, ground_y], fill=PALETTE["STEEL_MID"])
        d.rectangle([cx + 4, ground_y - 26, cx + 6, ground_y], fill=PALETTE["STEEL_SPEC"])
        d.rectangle([cx + 10, ground_y - 26, cx + 12, ground_y], fill=PALETTE["STEEL_SHAD"])
        d.rectangle([cx + 2, ground_y - 6, cx + 14, ground_y], fill=PALETTE["STEEL_DEEP"])

        # Torso & Waist (Y=56 to Y=80)
        d.rectangle([cx - 14, ground_y - 58, cx + 14, ground_y - 26], fill=PALETTE["CLOTH_MID"])
        # Iron Breastplate center
        d.rectangle([cx - 10, ground_y - 56, cx + 10, ground_y - 34], fill=PALETTE["STEEL_MID"])
        d.rectangle([cx - 10, ground_y - 56, cx - 6, ground_y - 34], fill=PALETTE["STEEL_SPEC"])
        d.rectangle([cx + 6, ground_y - 56, cx + 10, ground_y - 34], fill=PALETTE["STEEL_SHAD"])
        # Belt at Y=80 (ground_y - 34)
        d.rectangle([cx - 12, ground_y - 34, cx + 12, ground_y - 30], fill=PALETTE["LEATHER_MID"])
        d.rectangle([cx - 2, ground_y - 35, cx + 2, ground_y - 29], fill=PALETTE["BRONZE_MID"]) # Buckle

        # Tabard drape over legs
        d.rectangle([cx - 6, ground_y - 30, cx + 6, ground_y - 14], fill=PALETTE["CLOTH_SHAD"])
        d.rectangle([cx - 4, ground_y - 28, cx + 4, ground_y - 16], fill=PALETTE["CLOTH_MID"])

        # Head / Closed Great Helm (Y=44 is eye slit)
        helm_top = ground_y - 74
        d.rectangle([cx - 9, helm_top, cx + 9, helm_top + 18], fill=PALETTE["STEEL_MID"])
        d.rectangle([cx - 9, helm_top, cx - 5, helm_top + 18], fill=PALETTE["STEEL_SPEC"])
        d.rectangle([cx + 5, helm_top, cx + 9, helm_top + 18], fill=PALETTE["STEEL_SHAD"])
        # Visor T-slit
        d.rectangle([cx - 6, helm_top + 8, cx + 6, helm_top + 10], fill=PALETTE["DEEP_VOID"])
        d.rectangle([cx - 1, helm_top + 8, cx + 1, helm_top + 15], fill=PALETTE["DEEP_VOID"])
        d.point([cx - 2, helm_top + 9], fill=PALETTE["VISOR_GLOW"])
        d.point([cx + 2, helm_top + 9], fill=PALETTE["VISOR_GLOW"])

        # Pauldrons (Shoulders)
        d.rectangle([cx - 18, ground_y - 58, cx - 12, ground_y - 44], fill=PALETTE["STEEL_SPEC"])
        d.rectangle([cx + 12, ground_y - 58, cx + 18, ground_y - 44], fill=PALETTE["STEEL_SHAD"])

        # Square Tower Shield (Held on left side from viewer, character's right)
        shield_x = cx - 26
        shield_y = ground_y - 62
        d.rectangle([shield_x, shield_y, shield_x + 16, shield_y + 44], fill=PALETTE["STEEL_MID"])
        # Shield border
        d.rectangle([shield_x, shield_y, shield_x + 2, shield_y + 44], fill=PALETTE["STEEL_SPEC"])
        d.rectangle([shield_x + 14, shield_y, shield_x + 16, shield_y + 44], fill=PALETTE["STEEL_SHAD"])
        d.rectangle([shield_x, shield_y, shield_x + 16, shield_y + 2], fill=PALETTE["STEEL_SPEC"])
        d.rectangle([shield_x, shield_y + 42, shield_x + 16, shield_y + 44], fill=PALETTE["STEEL_DEEP"])
        # Bronze cross on shield
        d.rectangle([shield_x + 7, shield_y + 8, shield_x + 9, shield_y + 36], fill=PALETTE["BRONZE_MID"])
        d.rectangle([shield_x + 3, shield_y + 16, shield_x + 13, shield_y + 18], fill=PALETTE["BRONZE_MID"])

        # Broadsword (Held on right side, resting down)
        sword_x = cx + 18
        sword_y = ground_y - 58
        d.rectangle([sword_x + 3, sword_y + 8, sword_x + 5, sword_y + 46], fill=PALETTE["STEEL_SPEC"]) # Blade
        d.rectangle([sword_x, sword_y + 6, sword_x + 8, sword_y + 8], fill=PALETTE["BRONZE_MID"])     # Crossguard
        d.rectangle([sword_x + 3, sword_y + 1, sword_x + 5, sword_y + 6], fill=PALETTE["LEATHER_MID"]) # Grip
        d.rectangle([sword_x + 2, sword_y, sword_x + 6, sword_y + 2], fill=PALETTE["BRONZE_SPEC"])     # Pommel

    elif direction_name == "SE":
        # -------------------------------------------------------------
        # SOUTH-EAST (45° - Main Isometric Camera Angle)
        # -------------------------------------------------------------
        # Legs
        d.rectangle([cx - 8, ground_y - 26, cx, ground_y], fill=PALETTE["STEEL_MID"])
        d.rectangle([cx - 8, ground_y - 26, cx - 6, ground_y], fill=PALETTE["STEEL_SPEC"])
        d.rectangle([cx - 10, ground_y - 6, cx + 2, ground_y], fill=PALETTE["STEEL_DEEP"])
        # Right rear leg
        d.rectangle([cx + 6, ground_y - 24, cx + 13, ground_y - 2], fill=PALETTE["STEEL_SHAD"])
        d.rectangle([cx + 4, ground_y - 5, cx + 15, ground_y], fill=PALETTE["STEEL_DEEP"])

        # Torso (angled 45°)
        d.rectangle([cx - 12, ground_y - 58, cx + 12, ground_y - 26], fill=PALETTE["CLOTH_MID"])
        d.rectangle([cx - 8, ground_y - 56, cx + 8, ground_y - 34], fill=PALETTE["STEEL_MID"])
        d.rectangle([cx - 8, ground_y - 56, cx - 4, ground_y - 34], fill=PALETTE["STEEL_SPEC"])
        d.rectangle([cx + 4, ground_y - 56, cx + 8, ground_y - 34], fill=PALETTE["STEEL_SHAD"])
        d.rectangle([cx - 10, ground_y - 34, cx + 10, ground_y - 30], fill=PALETTE["LEATHER_MID"])

        # Head / Helm (angled)
        helm_top = ground_y - 74
        d.rectangle([cx - 8, helm_top, cx + 8, helm_top + 18], fill=PALETTE["STEEL_MID"])
        d.rectangle([cx - 8, helm_top, cx - 4, helm_top + 18], fill=PALETTE["STEEL_SPEC"])
        d.rectangle([cx + 4, helm_top, cx + 8, helm_top + 18], fill=PALETTE["STEEL_SHAD"])
        d.rectangle([cx - 4, helm_top + 8, cx + 6, helm_top + 10], fill=PALETTE["DEEP_VOID"])
        d.point([cx + 1, helm_top + 9], fill=PALETTE["VISOR_GLOW"])

        # Large Pauldron front
        d.rectangle([cx - 16, ground_y - 58, cx - 8, ground_y - 42], fill=PALETTE["STEEL_SPEC"])

        # Square Tower Shield angled forward-left
        shield_x = cx - 24
        shield_y = ground_y - 58
        d.rectangle([shield_x, shield_y, shield_x + 18, shield_y + 42], fill=PALETTE["STEEL_MID"])
        d.rectangle([shield_x, shield_y, shield_x + 3, shield_y + 42], fill=PALETTE["STEEL_SPEC"])
        d.rectangle([shield_x + 15, shield_y, shield_x + 18, shield_y + 42], fill=PALETTE["STEEL_SHAD"])
        d.rectangle([shield_x + 8, shield_y + 6, shield_x + 10, shield_y + 36], fill=PALETTE["BRONZE_MID"])
        d.rectangle([shield_x + 3, shield_y + 14, shield_x + 15, shield_y + 16], fill=PALETTE["BRONZE_MID"])

        # Broadsword in right hand, angled forward
        sword_x = cx + 12
        sword_y = ground_y - 54
        d.line([sword_x, sword_y + 8, sword_x + 14, sword_y + 42], fill=PALETTE["STEEL_SPEC"], width=2)
        d.rectangle([sword_x - 3, sword_y + 6, sword_x + 5, sword_y + 8], fill=PALETTE["BRONZE_MID"])

    elif direction_name == "E":
        # -------------------------------------------------------------
        # EAST (90° - Profile looking right)
        # -------------------------------------------------------------
        # Legs
        d.rectangle([cx - 6, ground_y - 26, cx + 4, ground_y], fill=PALETTE["STEEL_MID"])
        d.rectangle([cx - 8, ground_y - 6, cx + 8, ground_y], fill=PALETTE["STEEL_DEEP"]) # Foot pointing right
        d.rectangle([cx + 4, ground_y - 24, cx + 10, ground_y - 2], fill=PALETTE["STEEL_SHAD"])

        # Torso side profile
        d.rectangle([cx - 8, ground_y - 58, cx + 8, ground_y - 26], fill=PALETTE["STEEL_MID"])
        d.rectangle([cx - 8, ground_y - 58, cx - 4, ground_y - 26], fill=PALETTE["CLOTH_SHAD"])
        d.rectangle([cx + 4, ground_y - 56, cx + 8, ground_y - 34], fill=PALETTE["STEEL_SPEC"])
        d.rectangle([cx - 8, ground_y - 34, cx + 8, ground_y - 30], fill=PALETTE["LEATHER_MID"])

        # Helm profile
        helm_top = ground_y - 74
        d.rectangle([cx - 7, helm_top, cx + 7, helm_top + 18], fill=PALETTE["STEEL_MID"])
        d.rectangle([cx + 2, helm_top + 8, cx + 8, helm_top + 10], fill=PALETTE["DEEP_VOID"])
        d.point([cx + 5, helm_top + 9], fill=PALETTE["VISOR_GLOW"])

        # Shield edge-on partially visible on offside
        d.rectangle([cx - 16, ground_y - 56, cx - 10, ground_y - 18], fill=PALETTE["STEEL_SHAD"])

        # Broadsword thrust forward right
        sword_x = cx + 8
        sword_y = ground_y - 48
        d.line([sword_x, sword_y, sword_x + 28, sword_y], fill=PALETTE["STEEL_SPEC"], width=2)
        d.rectangle([sword_x, sword_y - 4, sword_x + 2, sword_y + 4], fill=PALETTE["BRONZE_MID"])

    elif direction_name == "NE":
        # -------------------------------------------------------------
        # NORTH-EAST (135° - Looking back-right)
        # -------------------------------------------------------------
        # Legs
        d.rectangle([cx - 6, ground_y - 26, cx + 2, ground_y], fill=PALETTE["STEEL_SHAD"])
        d.rectangle([cx + 4, ground_y - 26, cx + 12, ground_y], fill=PALETTE["STEEL_DEEP"])

        # Torso back (Backplate & Cloak drape)
        d.rectangle([cx - 12, ground_y - 58, cx + 12, ground_y - 26], fill=PALETTE["CLOTH_SHAD"])
        d.rectangle([cx - 8, ground_y - 56, cx + 4, ground_y - 32], fill=PALETTE["STEEL_SHAD"])
        d.rectangle([cx - 10, ground_y - 34, cx + 10, ground_y - 30], fill=PALETTE["LEATHER_SHAD"])

        # Helm back
        helm_top = ground_y - 74
        d.rectangle([cx - 8, helm_top, cx + 8, helm_top + 18], fill=PALETTE["STEEL_SHAD"])
        d.rectangle([cx - 8, helm_top, cx - 4, helm_top + 18], fill=PALETTE["STEEL_MID"])

        # Shield carried on left flank
        d.rectangle([cx - 22, ground_y - 56, cx - 12, ground_y - 20], fill=PALETTE["STEEL_SHAD"])
        # Sword angled away
        d.line([cx + 10, ground_y - 48, cx + 24, ground_y - 20], fill=PALETTE["STEEL_MID"], width=2)

    elif direction_name == "N":
        # -------------------------------------------------------------
        # NORTH (180° - Directly facing away/back)
        # -------------------------------------------------------------
        # Legs back
        d.rectangle([cx - 12, ground_y - 26, cx - 4, ground_y], fill=PALETTE["STEEL_SHAD"])
        d.rectangle([cx + 4, ground_y - 26, cx + 12, ground_y], fill=PALETTE["STEEL_SHAD"])

        # Torso Backplate & Full Cloth Tabard
        d.rectangle([cx - 14, ground_y - 58, cx + 14, ground_y - 24], fill=PALETTE["CLOTH_SHAD"])
        d.rectangle([cx - 8, ground_y - 56, cx + 8, ground_y - 34], fill=PALETTE["STEEL_SHAD"])
        d.rectangle([cx - 12, ground_y - 34, cx + 12, ground_y - 30], fill=PALETTE["LEATHER_SHAD"])

        # Helm Backplate
        helm_top = ground_y - 74
        d.rectangle([cx - 9, helm_top, cx + 9, helm_top + 18], fill=PALETTE["STEEL_SHAD"])
        d.rectangle([cx - 9, helm_top, cx - 5, helm_top + 18], fill=PALETTE["STEEL_MID"])

        # Shield strapped to left back
        d.rectangle([cx - 24, ground_y - 60, cx - 12, ground_y - 20], fill=PALETTE["STEEL_SHAD"])
        # Sword sheathed/held right
        d.rectangle([cx + 14, ground_y - 56, cx + 18, ground_y - 18], fill=PALETTE["STEEL_MID"])

    return img

def main():
    directions = ["S", "SE", "E", "NE", "N"]
    frames = []

    print("Generating Vanguard 5-Direction Design Sheet...")
    for d in directions:
        img = draw_vanguard_direction(d)
        frame_path = f"{OUTPUT_DIR}/vanguard_design_sheet_{d}.png"
        img.save(frame_path)
        frames.append(img)
        print(f"  [OK] Saved {frame_path} (128x128)")

    # 1. Combined 5-direction Spritesheet (640 x 128)
    combined = Image.new("RGBA", (128 * 5, 128), (0, 0, 0, 0))
    for i, f in enumerate(frames):
        combined.paste(f, (i * 128, 0))
    combined_path = f"{OUTPUT_DIR}/vanguard_design_sheet_5dir.png"
    combined.save(combined_path)
    print(f"  [OK] Saved combined sheet {combined_path} (640x128)")

    # 2. 32x32 In-Game Scale Crop (Centered on SE main isometric pose)
    se_img = frames[1] # SE
    # Square 80x80 crop (X: 25..105, Y: 38..118) to preserve exact 1:1 aspect ratio and full silhouette (sword & shield)
    char_crop = se_img.crop((25, 38, 105, 118))
    scale_32 = char_crop.resize((32, 32), Image.Resampling.NEAREST)
    scale_32_path = f"{OUTPUT_DIR}/vanguard_design_sheet_32x32.png"
    scale_32.save(scale_32_path)
    print(f"  [OK] Saved 32x32 in-game scale crop {scale_32_path}")

    # 3. 16x16 Icon / Minimap Scale
    scale_16 = char_crop.resize((16, 16), Image.Resampling.NEAREST)
    scale_16_path = f"{OUTPUT_DIR}/vanguard_design_sheet_16x16.png"
    scale_16.save(scale_16_path)
    print(f"  [OK] Saved 16x16 icon/minimap scale crop {scale_16_path}")

    # 4. Generate Interactive Review HTML
    html_content = f"""<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <title>Vanguard 'Iron Bastion' Design Sheet Review — Mortal Ascension</title>
  <style>
    :root {{
      --bg: #0b0d13;
      --card-bg: #141824;
      --border: #242c40;
      --accent: #5b8ab5;
      --gold: #f5d06e;
      --text: #e0e6ed;
      --subtext: #8e9bb0;
    }}
    body {{
      margin: 0;
      padding: 24px;
      background: var(--bg);
      color: var(--text);
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
    }}
    h1, h2, h3 {{ color: var(--gold); margin-top: 0; }}
    .container {{ max-width: 1200px; margin: 0 auto; }}
    .badge {{
      display: inline-block;
      padding: 4px 10px;
      border-radius: 4px;
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 1px;
      text-transform: uppercase;
      background: #1e293b;
      color: var(--accent);
      border: 1px solid var(--accent);
      margin-bottom: 12px;
    }}
    .grid-2 {{ display: grid; grid-template-columns: 1fr 1fr; gap: 24px; }}
    .card {{
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: 8px;
      padding: 20px;
      margin-bottom: 24px;
    }}
    .pixel-box {{
      background: #050608;
      border: 2px dashed var(--border);
      border-radius: 6px;
      padding: 20px;
      display: flex;
      justify-content: center;
      align-items: center;
      overflow: auto;
    }}
    img.pixelated {{
      image-rendering: pixelated;
      image-rendering: crisp-edges;
    }}
    .swatches {{ display: flex; gap: 8px; flex-wrap: wrap; margin-top: 10px; }}
    .swatch {{
      width: 48px;
      height: 48px;
      border-radius: 4px;
      border: 1px solid #ffffff22;
      display: flex;
      align-items: flex-end;
      padding: 4px;
      font-size: 9px;
      font-family: monospace;
      color: #fff;
      text-shadow: 0 1px 2px #000;
    }}
    .btn {{
      background: #1e293b;
      color: #fff;
      border: 1px solid var(--border);
      padding: 8px 16px;
      border-radius: 4px;
      cursor: pointer;
      font-weight: 600;
      margin-right: 8px;
    }}
    .btn:hover {{ background: var(--accent); }}
    .btn.active {{ background: var(--accent); border-color: #fff; }}
  </style>
</head>
<body>
  <div class="container">
    <span class="badge">Giai Đoạn 2 — Chốt Phong Cách Nhân Vật</span>
    <h1>Thiết Kế Nhân Vật: Vanguard "Iron Bastion" (5 Hướng)</h1>
    <p style="color: var(--subtext)">
      Tổng hợp định hướng từ Tứ Trụ Tham Chiếu: <b>Octopath Traveler II</b> (HD-2D UE5), <b>Dark Souls</b> (Dark Fantasy giáp sắt tôi), <b>Children of Morta</b> (Động lực học hành động), <b>Triangle Strategy</b> (Tỷ lệ mảng khối 3.5 đầu).
    </p>

    <!-- 5 DIRECTIONS SHEET -->
    <div class="card">
      <h2>1. Bản Vẽ 5 Hướng Chuẩn Hóa (5-Directional Isometric Design Sheet)</h2>
      <p style="color: var(--subtext)">Canvas 128×128 pixel per frame | Điểm neo tiếp đất: (64, 114) | Tỷ lệ cơ thể: 3.5 đầu</p>
      <div style="margin-bottom: 12px;">
        <button class="btn active" onclick="setZoom(1)">1x Native</button>
        <button class="btn" onclick="setZoom(2)">2x Zoom</button>
        <button class="btn" onclick="setZoom(3)">3x Zoom</button>
        <button class="btn" onclick="setZoom(4)">4x HD-2D</button>
        <button class="btn" onclick="toggleBlackout()">Toggle Blackout Silhouette</button>
      </div>
      <div class="pixel-box" id="viewport">
        <img id="sheetImg" class="pixelated" src="vanguard_design_sheet_5dir.png" style="transform-origin: center; transform: scale(3); margin: 60px 0;">
      </div>
      <div style="display: flex; justify-content: space-around; margin-top: 12px; font-weight: 600; color: var(--gold);">
        <div>South (0°) - Trực diện</div>
        <div>South-East (45°) - Isometric</div>
        <div>East (90°) - Nhìn ngang</div>
        <div>North-East (135°) - Lưng chếch</div>
        <div>North (180°) - Sau lưng</div>
      </div>
    </div>

    <!-- IN-GAME SCALE REVIEW (32x32 & 16x16) -->
    <div class="grid-2">
      <div class="card">
        <h2>2. Kiểm Thử Kích Thước In-Game 32×32</h2>
        <p style="color: var(--subtext)">Độ hiển thị nhân vật ở tỷ lệ pixel chuẩn trong gameplay:</p>
        <div class="pixel-box" style="gap: 32px;">
          <div>
            <div style="font-size: 11px; margin-bottom: 6px; color: var(--subtext)">1x Native (32×32)</div>
            <img class="pixelated" src="vanguard_design_sheet_32x32.png">
          </div>
          <div>
            <div style="font-size: 11px; margin-bottom: 6px; color: var(--subtext)">4x Camera Zoom</div>
            <img class="pixelated" src="vanguard_design_sheet_32x32.png" style="width: 128px; height: 128px;">
          </div>
          <div>
            <div style="font-size: 11px; margin-bottom: 6px; color: var(--subtext)">8x Inspection</div>
            <img class="pixelated" src="vanguard_design_sheet_32x32.png" style="width: 256px; height: 256px;">
          </div>
        </div>
      </div>

      <div class="card">
        <h2>3. Kiểm Thử Icon Minimap / HUD 16×16</h2>
        <p style="color: var(--subtext)">Nhận diện silhouette tối thiểu ở kích thước siêu nhỏ:</p>
        <div class="pixel-box" style="gap: 32px;">
          <div>
            <div style="font-size: 11px; margin-bottom: 6px; color: var(--subtext)">1x Native (16×16)</div>
            <img class="pixelated" src="vanguard_design_sheet_16x16.png">
          </div>
          <div>
            <div style="font-size: 11px; margin-bottom: 6px; color: var(--subtext)">8x Minimap Zoom</div>
            <img class="pixelated" src="vanguard_design_sheet_16x16.png" style="width: 128px; height: 128px;">
          </div>
        </div>
      </div>
    </div>

    <!-- 4-TONE PALETTES -->
    <div class="card">
      <h2>4. Bảng Màu 4 Nấc Sắc Độ (4-Tone Hue-Shifted Palette Specification)</h2>
      <p style="color: var(--subtext)">Tuyệt đối không pha trộn màu bẩn (Zero Micro-Color Bleed), đổ bóng ngả tím Navy.</p>
      
      <div style="margin-bottom: 16px;">
        <div style="font-weight: 600; margin-bottom: 4px;">Giáp Sắt Tôi Đen (Blackened Steel)</div>
        <div class="swatches">
          <div class="swatch" style="background: #e0e6ed; color: #000;">#E0E6ED<br>Tone 1</div>
          <div class="swatch" style="background: #7a889b;">#7A889B<br>Tone 2</div>
          <div class="swatch" style="background: #373f4d;">#373F4D<br>Tone 3</div>
          <div class="swatch" style="background: #191d24;">#191D24<br>Tone 4</div>
        </div>
      </div>

      <div style="margin-bottom: 16px;">
        <div style="font-weight: 600; margin-bottom: 4px;">Vải Tabard Xanh Chàm Phong Trần (Worn Azure Cloth)</div>
        <div class="swatches">
          <div class="swatch" style="background: #5b8ab5;">#5B8AB5<br>Tone 1</div>
          <div class="swatch" style="background: #32527b;">#32527B<br>Tone 2</div>
          <div class="swatch" style="background: #1e2f47;">#1E2F47<br>Tone 3</div>
          <div class="swatch" style="background: #101826;">#101826<br>Tone 4</div>
        </div>
      </div>

      <div>
        <div style="font-weight: 600; margin-bottom: 4px;">Đinh Tán & Thánh Giá Đồng Cổ (Antique Bronze)</div>
        <div class="swatches">
          <div class="swatch" style="background: #f5d06e; color: #000;">#F5D06E<br>Tone 1</div>
          <div class="swatch" style="background: #b88a30;">#B88A30<br>Tone 2</div>
          <div class="swatch" style="background: #734e1a;">#734E1A<br>Tone 3</div>
        </div>
      </div>
    </div>
  </div>

  <script>
    let isBlackout = false;
    function setZoom(scale) {{
      document.getElementById('sheetImg').style.transform = `scale(${{scale}})`;
      document.querySelectorAll('.btn').forEach(b => b.classList.remove('active'));
      event.target.classList.add('active');
    }}
    function toggleBlackout() {{
      isBlackout = !isBlackout;
      const img = document.getElementById('sheetImg');
      if (isBlackout) {{
        img.style.filter = 'brightness(0)';
      }} else {{
        img.style.filter = 'none';
      }}
    }}
  </script>
</body>
</html>
"""
    html_path = f"{OUTPUT_DIR}/vanguard_phase2_design_sheet_review.html"
    with open(html_path, "w", encoding="utf-8") as f:
        f.write(html_content)
    print(f"  [OK] Saved review HTML {html_path}")

    print("\nAll assets and review tools generated successfully!")

if __name__ == "__main__":
    main()
