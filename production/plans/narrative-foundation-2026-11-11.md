# Kế hoạch: Nền tảng Narrative — thế giới, phe phái, thiết kế nhân vật

## Context

GDD ban đầu sinh từ skills Claude-Code-Game-Studios chỉ có flavor text theo từng hệ thống. Khảo sát cho thấy:

- **Không có cốt truyện/thế giới**: không tên thế giới, không nhân vật chính, không phản diện, không lịch sử, không phe phái (chỉ nhắc "Liên Minh" 2 lần). `design/narrative/` chưa tồn tại; `design/registry/entities.yaml` không có vùng, NPC, boss, phe phái.
- **Tên gọi lệch**: hầu hết class có 2–4 tên tiếng Việt; game là "Mortal Ascension" (README, Config) nhưng GDD ghi "Project Ascendant"; vùng và thành có 4–5 bí danh.
- **Số class lệch theo tầng**: 16 (DECISIONS §2, GDD, tag) / 12 (art, rig, registry) / 4 (art bible) / 3 (`EPACharacterClass`).
- **Thông số sprite mâu thuẫn**: chiều cao 58–86 px, "chibi" vừa quy định vừa cấm, lớp paperdoll 5/6/7/9, hướng 1/2/5/8, PPU 32 vs 1.0, màu rarity hai bộ.
- **Boss không khớp**: Ironclad Warlord có thiết kế, không art; Stone Golem có art, chạy moveset Warlord; phần thân boss trong DECISIONS §13 có "Cánh" mà không boss nào có.
- **Tông trôi**: dark medieval fantasy (cấm máy móc) lẫn Void vũ trụ, plasma, ma pháp thời gian, từ vựng tiên hiệp.

Quyết định của chủ dự án (phiên này):
1. Phạm vi: nền tảng đầy đủ (world bible → phe phái/vùng → bản sắc 16 class → character sheet → boss/quái/NPC).
2. Canon: agent đề xuất 2–3 phương án, chủ dự án chọn. Không tên mới nào vào GDD khi chưa được chọn.
3. Tiền đề: **thế giới fantasy có thật, Hệ Thống (cấp, class, kỹ năng) là quy luật mà cư dân nhìn thấy**.
4. Roadmap: thêm giai đoạn mới, **chạy song song** Giai đoạn 4 (không chặn art).

Kết quả mong muốn: một nguồn canon duy nhất cho thế giới và nhân vật, mọi GDD/registry tham chiếu về đó, có cổng tự động chặn tên lệch tái phát.

## Nguyên tắc

- `production/DECISIONS.md` thắng mọi file khác. Tên tiếng Việt của class lấy theo DECISIONS §2; vũ khí theo §7; 5 hướng + mirror theo §10.
- Mỗi đầu việc dưới đây = 1 nhánh + 1 PR, theo đủ quy trình 6 bước trong CLAUDE.md.
- Mọi điểm chạm DECISIONS/ROADMAP đều dừng hỏi (đánh dấu 🛑).
- Mỗi lô đề xuất canon gom vào một trang xem trước (Artifact) để chủ dự án chọn; chỉ ghi phương án đã chọn vào repo.
- Không sửa code ngoài chuỗi hiển thị; lỗi code phát hiện được ghi thành việc riêng.

## Đầu việc

### N0 — Hạ tầng và ROADMAP 🛑
- Thêm giai đoạn "Nền tảng Narrative" (N1–N8) vào `production/ROADMAP.md` và một dòng ghi nhận ở `production/DECISIONS.md` §13 (tiền đề "thế giới thật có Hệ Thống", chạy song song GĐ4). Duyệt kế hoạch này = cho phép hai chỉnh sửa đó; nội dung chính xác vẫn đưa bạn xem trong PR.
- Tạo `design/narrative/` (`world/`, `factions/`, `characters/`, `bestiary/`, `npcs/`) theo cấu trúc mà `.claude/skills/team-narrative`, `asset-spec`, `design-review` đã mong đợi; áp dụng `.claude/rules/narrative.md` (canon level cho mỗi mục).
- Mở rộng `Tools/QA/validate_gdd_consistency.py`: thêm `design/narrative/**` vào `SCAN_TARGETS`; thêm test tương ứng trong `Tools/QA/test_validate_gdd_consistency.py`.
- Thêm các nhóm rỗng `regions`, `citadels`, `factions`, `npcs`, `bosses`, `enemies` vào `design/registry/entities.yaml`.

### N1 — Thống nhất tên gọi hiện có (không tạo nội dung mới)
- Thay mọi tên tiếng Việt của class trong `design/gdd/{foundational-classes,advanced-classes,character-visual-system,game-concept}.md`, `design/art/{art-bible,vanguard-mood-board}.md`, `entities.yaml` về đúng DECISIONS §2. Xóa "Dark Knight" (art-bible:95).
- Bổ sung 4 class thiếu (Swordmaster, Inquisitor, Phantom Stalker, Seraph) vào `entities.yaml`; sửa "12 Class"/"7 Class Nâng Cao" trong `systems-index.md`; sửa mô tả vũ khí lệch §7.
- Tên game: GDD ghi "Mortal Ascension", giữ `ProjectAscendant` làm định danh kỹ thuật (theo README).
- Validator: thêm check `class_vn_name` (mỗi class đúng một tên tiếng Việt nạp từ DECISIONS).
- 🛑 Cần bạn quyết: DECISIONS:32 gọi cả nhánh Scout là "Nhánh Du Hiệp" trong khi GDD dùng "Du Hiệp" cho Ranger (DECISIONS:33 ghi "Xạ Thủ").

### N2 — Hòa giải thông số sprite và art bible
- Lập bảng một-nguồn-sự-thật trong `design/art/pixel-asset-specifications.md`; các file khác trỏ về, không lặp số.
- Áp nguồn đã chốt: 5 hướng + mirror (DECISIONS §10); Vanguard 70 px, "không chibi" (mood board + design sheet đã duyệt); màu rarity theo `inventory-system.md` §1 (DECISIONS:245).
- 🛑 Trình bạn chọn những điểm không có nguồn chốt: mô hình lớp paperdoll (5/6/7/9), PPU, ngân sách màu, khiên Vanguard (vuông vs tròn), tabard (navy vs crimson), tư thế 90°/50-50 trong CVS so với lệnh cấm ở art bible.
- Chạy `/art-bible` chế độ retrofit cho §5 (Character) và §6 (Environment) sau khi chốt.
- Làm sớm vì GĐ4 đang chạy song song.

### N3 — World bible 🛑 (duyệt theo trang đề xuất)
- Đề xuất 2–3 phương án cho mỗi mục, trong khung "thế giới thật có Hệ Thống": tên thế giới; Hệ Thống là gì và từ đâu; nghĩa của "Mortal Ascension"; thần linh (God Slayer, "World Boss Thần Linh"); Hư Không; thế lực phản diện; người chơi là ai; luật Karma/Vệ Binh trong truyện; chỗ đứng của ma pháp thời gian và từ vựng khí công (giữ có giải thích, hay loại).
- Ghi phương án đã chọn vào `design/narrative/world/world-bible.md` và `tone-guide.md` (chuẩn mục: Core Concept, Rules, History, Connections, Player Relevance, Contradictions Check).
- Cập nhật `game-concept.md` thêm mục Setting trỏ về world bible.

### N4 — Địa lý và phe phái 🛑
- 3 vùng, 3 thành, các địa điểm mở khóa class (game-concept:112–128), Ngục Tối: mỗi nơi một mục có lịch sử, cư dân, lý do tên gọi; chốt một tên chính thức, liệt kê bí danh bị loại.
- Phe phái theo `.claude/docs/templates/faction-design.md`: Liên Minh và các nhóm đã được ám chỉ (hội sát thủ, Kiếm Tông, giáo sĩ tha hóa); nhóm mới chỉ thêm khi bạn chọn.
- Trình bạn chốt các mâu thuẫn vị trí: lò rèn T2/T3, Chợ Đen, Lửa Trại (`zone-system.md:15` nói bỏ nhưng cùng file vẫn dùng), bí danh biome trong PCG.
- Đăng ký vào `entities.yaml`; sửa tên trong `zone-system.md`, `merchant-economy.md`, `blacksmithing-system.md`, `itemization.md`, `core-game-loop.md`.

### N5 — Bản sắc và character sheet 16 class 🛑 (5 lô theo nhánh)
- Mỗi class một file `design/narrative/characters/<class>.md` theo `.claude/docs/templates/narrative-character-sheet.md`, bổ sung khối hình ảnh: silhouette, tỷ lệ, bảng màu, trang phục, vũ khí (§7), crest/tabard, tư thế, ghi chú 5 hướng; cộng gốc gác trong thế giới, phe phái, địa điểm mở khóa, giọng điệu.
- Vanguard: hợp nhất mood board + design sheet đã duyệt, không đổi hướng "Iron Bastion"; chốt mâu thuẫn "đấu sĩ parry cơ động" (GDD) vs "Heavy Tank" (art).
- 🛑 Hỏi bạn về giới tính/kiểu thân nhân vật (hiện không có ở đâu; 4 lớp paperdoll nữ đang bỏ không).
- `character-visual-system.md` bảng silhouette trỏ về các sheet; `/asset-spec character:<name>` dùng sheet làm đầu vào cho GĐ4.

### N6 — Boss, bestiary, NPC 🛑
- 🛑 Chạm DECISIONS:174, 223, 234: chọn quan hệ Stone Golem ↔ Ironclad Warlord (hai boss riêng, hay một), và danh sách phần thân ("Cánh").
- Hợp nhất ba danh sách boss không trùng nhau (game-concept:120–128, CVS:107–112, skill-progression:196–201) thành `design/narrative/bestiary/` + roster quái theo vùng.
- NPC có tên cho mỗi thành (thợ rèn, thương nhân, dược sư, Smuggler, Broker, người giao nhiệm vụ) trong `design/narrative/npcs/`.
- Đăng ký registry; sửa `boss-ai.md`, `stagger-system.md`, `core-game-loop.md` cho khớp.

### N7 — Đồng bộ chuỗi hiển thị trong code
- Chỉ đổi text: tên/lore class trong `Source/ProjectAscendant/Private/UI/PACharacterSelectTypes.cpp`, tên boss trên HUD (`PAStoneGolemBoss.cpp:561`), thêm lore Acolyte.
- Ghi thành việc riêng, không sửa ở đây: `EPACharacterClass` thiếu Acolyte, đường dẫn `ABP_Vanguard` hỏng, enum `Tier2_Wilderness`/`Tier3_Sanctuary`, moveset đuôi trên Golem.

### N8 — Rà soát chéo cuối
- `/review-all-gdds` + `/consistency-check full`; cổng `nd-consistency` và `cd-narrative` (`.claude/docs/director-gates/`); cập nhật `systems-index.md` thêm nhóm Narrative.

Thứ tự: N0 → (N1 ∥ N2) → N3 → N4 → N5 → N6 → N7 → N8. N1, N2 không phụ thuộc canon mới nên làm trước để giảm làm lại art GĐ4.

## Phân công (plan → assign → verify)

| Đầu việc | Soạn | Phản biện |
|---|---|---|
| N0 | devops-engineer (validator), phiên chính (ROADMAP) | reviewer |
| N1 | phiên chính | reviewer |
| N2 | art-director + technical-artist | reviewer |
| N3 | creative-director + narrative-director + world-builder | reviewer + cổng cd-narrative |
| N4 | world-builder + level-designer | reviewer + nd-consistency |
| N5 | narrative-director + art-director (+ game-designer cho vai trò chiến đấu) | reviewer + nd-consistency |
| N6 | world-builder + game-designer | reviewer |
| N7 | gameplay-programmer | reviewer |
| N8 | phiên chính | reviewer |

Agent chỉ đề xuất; phiên chính ghi file sau khi bạn chọn. Kết quả từ agy (nếu dùng) luôn qua reviewer của Claude.

## Rủi ro đã chấp nhận

Art nhân vật GĐ4 chạy song song có thể phải làm lại cho class chưa có sheet. Giảm thiểu: N2 làm trước; N5 ưu tiên nhánh mà GĐ4 đang sản xuất.

## Kiểm chứng (mỗi PR)

1. `python3 Tools/QA/validate_gdd_consistency.py` và `python3 Tools/QA/test_validate_gdd_consistency.py` — exit 0.
2. N7: build + test `ProjectAscendant.*`.
3. `grep` xác nhận không còn tên bị loại (danh sách bí danh trong từng PR) ngoài dòng đánh dấu deprecated.
4. Subagent `reviewer` context mới, trích file:dòng; sửa hết rồi chạy lại 1–3.
5. Lô canon: bạn duyệt trên trang xem trước trước khi merge.
6. Merge qua PR, `git ls-remote` xác nhận, ghi `production/PROGRESS.md` (hash, log test, kết luận reviewer).
