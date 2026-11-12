# Mảng A — Sản xuất art

> Chi tiết hóa Giai đoạn 4 của `ROADMAP.md`. Trừ A0 và A2, không bắt đầu trước khi G10 chọn "giữ".
> **Chuẩn duy nhất đã được chủ dự án duyệt**: design sheet Vanguard `Art_Gallery/vanguard_design_sheet_{S,SE,E,NE,N,5dir,32x32,16x16}.png` (Giai đoạn 2, PR #48). Đo ngày 2026-11-12: canvas 128×128, đỉnh Y=40, chân Y=114, cao 75 px; rộng S 53, SE 52, E 53, NE 48, N 43 px; 4–12 màu; không bán trong suốt; không viền tối. Chưa có mào (DECISIONS §13 N2).
> Bộ art Giai đoạn 3 (`Content/art/characters/MasterRigs`, `UpperBodyWeapons`, `ClassIdentity`) là **proxy**, chưa được chủ dự án chơi thử và duyệt; không dùng làm mẫu.
> Bản Ashen Gate (V1 trong `ROADMAP.md`) là art sinh bằng AI chờ duyệt, **không** phải chuẩn.

## Công cụ: điều agent cần biết trước

- `.claude/skills/*` (gồm `game-art-studio`) **không có trong git**; chỉ có ở bản checkout chính. Codex/Antigravity không gọi được `/skill`; dùng script theo đường dẫn.
- `game-art-studio` dựa vào sinh ảnh AI của agent chủ, và tài liệu của nó nhắc dịch vụ trả phí Meowa (`MEOWART_API_KEY`). Mọi bước sinh ảnh: 🛑 asset AI (duyệt theo lô) và 🛑 tiêu tiền nếu dùng dịch vụ trả phí. Các script hậu xử lý của nó (cắt, ghép atlas, GIF xem trước) không sinh ảnh, dùng tự do.
- `Tools/aseprite-ai-artist` (MCP, đã commit): agent vẽ từng pixel trong cửa sổ Aseprite đang mở; không gọi API ảnh. Cần Aseprite chạy được trên máy (hiện chưa tìm thấy binary trên PATH).
- Nhiều script sinh asset ghi đè file đã commit ở đường dẫn cố định (`Tools/Aseprite/generate_vanguard_character.py`, `generate_environment_assets.py`, `Tools/Spine/*`, `Scripts/setup_art_gallery.py`, `Scripts/generate_vanguard_5dir_design_sheet.py`). **Không chạy** khi thẻ không yêu cầu; chạy thì PR phải nêu file bị ghi đè.
- Danh sách lệnh đã chạy thử: playbook ở A0.

---

### A0 — Playbook art và cổng đo spec
- Phụ thuộc: không.
- Đọc trước: `design/art/pixel-asset-specifications.md` §0, `design/art/art-bible.md`, `production/qa/art-gate-criteria.md`, `THIRD_PARTY_ASSETS.md`, `Scripts/test_vanguard_design_sheet_art_gate.py`.
- Làm:
  1. `design/art/art-production-playbook.md`: theo từng loại asset (sprite nhân vật, animation/flipbook, tileset/môi trường, icon, UI, VFX, import UE, QA), ghi công cụ, lệnh đã chạy thử, đầu vào/đầu ra, điểm 🛑; quy trình mỗi lô; cách ghi nguồn gốc vào `THIRD_PARTY_ASSETS.md`.
  2. `Tools/QA/sprite_spec_gate.py`: đo PNG theo §0 (canvas, đỉnh/chân, chiều cao ±4 px, số màu, bán trong suốt), in bảng, exit khác 0 khi lệch. Chế độ nhân vật đầy đủ đo chiều cao/đỉnh/chân; rig/lớp rời phải được ghép thành nhân vật trước khi đo chiều cao. Icon/tileset/VFX dùng bộ tiêu chí và lệnh riêng theo loại asset trong playbook, không áp chiều cao 75 px của nhân vật cho chúng. Script gate hiện có không đo chiều cao, đỉnh Y hay số màu tối thiểu.
- Nghiệm thu: `python3 Tools/QA/sprite_spec_gate.py Art_Gallery/vanguard_design_sheet_{S,SE,E,NE,N}.png` exit 0; chạy trên một PNG cố ý sai (cao 86 px) → exit khác 0.
- 🛑 `vanguard_design_sheet_32x32.png` có 8 màu trong khi §0 ghi icon 32×32 là 16–24 màu: chủ dự án quyết file này là icon hay bản thu nhỏ để xem trước.

### A1 — Nhân vật mẫu trọn bộ: Vanguard
- Phụ thuộc: G10 ("giữ"), H1, A0.
- Đọc trước: playbook A0; `design/art/vanguard-mood-board.md`; `design/gdd/character-visual-system.md` §4.3 (danh sách animation); ghi chú PaperZD (P1 trong `ROADMAP.md`).
- Làm: Vanguard đủ bộ animation §4.3 + Stunned, Dead; 5 hướng + lật; mào đỏ thẫm, tabard navy "Worn Azure"; metadata socket và hitbox theo H1; import Paper2D (Nearest, không mipmap, PPU 1.0); thay hình tạm của greybox.
- Nghiệm thu: `sprite_spec_gate.py` pass cho mọi frame; test `ProjectAscendant.CoreLoop.*` vẫn xanh; ảnh chụp trong game.
- 🛑 Duyệt thẩm mỹ (hình có mào lần đầu); 🛑 nếu có bước sinh ảnh AI.

### A2 — Sổ cái CC0
- Phụ thuộc: không.
- Làm: thẩm định các pack Pending, thay 7 asset bị từ chối, rà `doficia/project-cordon-sprites` (mục Giai đoạn 4 cũ); ghi kết quả vào `THIRD_PARTY_ASSETS.md`.
- Nghiệm thu: `python3 Tools/QA/validate_gdd_consistency.py` và `python3 Tools/QA/verify_git_lfs_pointers.py --base origin/main` exit 0; ledger có URL/license gốc, phạm vi file, checksum và kết luận cho từng pack được rà. Cả 7 asset bị từ chối phải có bản thay thế được thẩm định hoặc có quyết định loại khỏi bản phân phối đã được chủ dự án duyệt; nếu còn chờ 🛑 thì A2 chưa `[x]`. Reviewer đối chiếu từng file với nguồn/license, không lấy validator làm chứng cứ pháp lý.
- 🛑 Asset không phải CC0; mọi khoản mua pack.

### A3 — Lower Body Master Rig và Weapon Family (visual-004, visual-005)
- Phụ thuộc: A1.
- Làm: theo lô (mỗi lô 1 rig hoặc 1 weapon family), mỗi lô qua `sprite_spec_gate.py` và Art Gate trước khi import. Thay proxy Giai đoạn 3, giữ tên asset để không sửa code.
- Nghiệm thu: `python3 Tools/QA/sprite_spec_gate.py <các-frame-nhân-vật-đã-ghép>` exit 0; chạy các lệnh QA rig/weapon family đã xác nhận trong playbook A0 (seam, pivot, socket, frame, alpha, palette) cho đúng lô, exit 0; `./Tools/QA/run_headless_tests.sh --ue-only --filter=ProjectAscendant.CoreLoop` xanh; giữ ảnh runtime và ghi nguồn gốc từng file.
- 🛑 Mỗi lô có art AI; duyệt thẩm mỹ lô đầu.

### A4 — Bản sắc 16 class (visual-006)
- Phụ thuộc: A1.
- Đọc trước: `character-visual-system.md`, SPEC §0, DECISIONS §2/§7/§10/§13; character sheet N5 đã được duyệt nếu có. N5 chạy song song, không là điều kiện nhận A4 (DECISIONS §13). Nếu thiếu quyết định thẩm mỹ của riêng class, dừng hỏi cho lô đó, không tự tạo canon.
- Làm: Idle + Crest + Tabard theo GDD/DECISIONS và character sheet N5 đã được duyệt nếu có, cánh Seraph; làm lại `Tabard_Vanguard_CrimsonSash` (đỏ → navy, DECISIONS §13 N2).
- Nghiệm thu: `python3 Tools/QA/sprite_spec_gate.py <các-frame-nhân-vật-đã-ghép>` exit 0; lệnh QA từng lớp/màu/socket trong playbook A0 exit 0; `./Tools/QA/run_headless_tests.sh --ue-only --filter=ProjectAscendant.CoreLoop` xanh; giữ screenshot runtime mỗi class trong lô, ledger và câu trả lời duyệt lô của chủ dự án.
- 🛑 Mỗi lô.

### A5 — Môi trường và đạo cụ thị trấn (visual-007)
- Phụ thuộc: G10 ("giữ"), A0, L1 (danh sách vật thể theo vùng).
- Làm: tileset và đạo cụ theo L1; dọn asset legacy `REWORK` chỉ sau khi bản thay thế qua Art Gate.
- Nghiệm thu: chạy lệnh QA tileset/đạo cụ đã xác nhận trong playbook A0 (kích thước tile 32×32 khi áp dụng, bitmask, seam, alpha, palette và license) cho toàn lô, exit 0; `./Tools/QA/run_headless_tests.sh --ue-only --filter=ProjectAscendant.CoreLoop` vẫn xanh; giữ screenshot runtime và test đường đi L2/L3 nếu map bị sửa. Không dùng gate chiều cao nhân vật cho môi trường.
- Lưu ý kỹ thuật: `Content/Art` và `Content/art` cùng tồn tại, sẽ xung đột trên hệ thống không phân biệt hoa thường; gom về một tên trước khi thêm asset môi trường (thẻ riêng nếu cần sửa đường dẫn trong code).
- 🛑 Mỗi lô.
