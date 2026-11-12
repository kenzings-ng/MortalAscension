# Mảng L — Bối cảnh và màn chơi

> Làm sau N4 (địa lý, phe phái đã chốt). Bố cục màn chơi dựng bằng khối tạm trước, art môi trường sau (mảng A).

### L1 — Tài liệu thiết kế màn chơi cho từng vùng
- Phụ thuộc: N4.
- Đọc trước: `design/narrative/world/world-bible.md`, kết quả N4, `design/gdd/zone-system.md`, `design/gdd/core-game-loop.md`, mẫu `.claude/docs/templates/level-design-document.md`.
- Làm: `design/levels/<vùng>.md` (thư mục mới) cho 3 vùng: bố cục, nhịp độ, vị trí Tòa Thành, lò rèn, khu boss, điểm hồi sinh theo N4, mốc nhận diện, đường đi chính/phụ, danh sách vật thể môi trường cần vẽ (đầu vào cho A5).
- Nghiệm thu: validator GDD exit 0; reviewer đồng ý.
- 🛑 Tên địa điểm mới phải nằm trong danh sách N4 đã được chọn.

### L2 — Blockout vùng 1 (Verdant Frontier)
- Phụ thuộc: L1, G10.
- Đọc trước: `Content/Maps/L_Ruins_Blockout.umap`, `Content/Maps/L_VerdantFrontier_Outpost.umap`, `Content/Maps/PCG/`.
- Làm: dựng bố cục L1 bằng khối tạm; đi bộ từ Tòa Thành tới khu boss được; không thêm art.
- Nghiệm thu: test UE mở map và kiểm có đường đi (NavMesh hoặc test di chuyển của G1) từ PlayerStart tới khu boss.

### L3 — Blockout vùng 2 và 3
- Phụ thuộc: L2; chủ dự án đã đi thử vùng 1 và cho tiếp tục (ghi câu trả lời vào PROGRESS).
- Đọc trước: tài liệu L1 của hai vùng, bản blockout L2 và ghi chú đi thử vùng 1.
- Làm: dựng hai vùng bằng khối tạm theo L1, nối đường đi và điểm hồi sinh đã chốt N4; không thêm tên/nội dung mới.
- Nghiệm thu: với từng map, test UE mở map và đi từ PlayerStart tới khu boss; giữ screenshot runtime và log vào PR.
- 🛑 Chủ dự án đi thử vùng 1 trước khi nhân rộng bố cục.

### L4 — Chủ dự án đi dạo map
- Phụ thuộc: L3.
- Làm: ghi lệnh mở từng map, checklist đường đi/điểm hồi sinh/khu boss, và giữ ảnh runtime.
- Nghiệm thu: log test đường đi cả 3 vùng pass; chủ dự án đi thử và cho nhận xét được ghi nguyên văn trong PROGRESS.
- 🛑 Trùng điểm dừng cuối Giai đoạn 5. Chủ dự án nhận xét; agent ghi nguyên văn vào PROGRESS.
