# Mảng H — Hitbox bản chính thức

> Làm **sau** khi chủ dự án chơi thử greybox (G10) và chọn "giữ". Greybox (G4) dùng hitbox theo thời gian với số tạm; mảng này thay bằng hitbox theo frame, có spec được duyệt.

### H1 — Spec hitbox/hurtbox
- Phụ thuộc: G10 (chủ dự án chọn "giữ").
- Đọc trước: `design/gdd/combat-system.md`, `design/gdd/boss-ai.md`, `design/gdd/stagger-system.md`, `design/art/pixel-asset-specifications.md` §0 (canvas 128×128, PPU 1.0, 5 hướng + lật), DECISIONS §10, §12, §13 "Cảm giác chơi"; log chơi thử G10 trong PROGRESS.
- Làm: GDD mới `design/gdd/hitbox-hurtbox.md` theo các mục bắt buộc của GDD (`.claude/docs/coding-standards.md` dòng 25–30): hình dạng theo weapon family, frame bắt đầu/kết thúc, độ lệch theo 5 hướng và cách lật, hurtbox người chơi/boss, tương tác i-frame, định dạng dữ liệu (JSON theo frame, cùng chỗ với socket trong metadata), công cụ debug.
- Nghiệm thu: `python3 Tools/QA/validate_gdd_consistency.py` exit 0; reviewer đồng ý.
- 🛑 Chủ dự án duyệt các con số (cửa sổ, kích thước) sau khi chơi thử; không tự chốt.

### H2 — Pipeline dữ liệu hitbox theo frame
- Phụ thuộc: H1.
- Làm: hitbox lưu trong metadata của từng flipbook (cùng cách `Tools/Paper2D/apply_vanguard_sockets.py` đọc socket); ability đọc cửa sổ hitbox theo frame thay cho bộ hẹn giờ của G4; test tự động so số frame và vị trí hitbox với metadata.
- Nghiệm thu: `ProjectAscendant.CoreLoop.MeleeHitsDummy` vẫn xanh khi đã bỏ bộ hẹn giờ; test mới cho từng hướng xanh.

### H3 — Hitbox boss và hurtbox theo bộ phận
- Phụ thuộc: H2, N6.
- 🛑 Danh sách bộ phận boss chạm DECISIONS §13 ("Sừng, Vảy đuôi, Giáp ngực, Cánh") và N6; chủ dự án chốt trước.
- Làm: hitbox từng đòn boss; hurtbox theo bộ phận để phá bộ phận.
