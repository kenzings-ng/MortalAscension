# Thẻ việc của ROADMAP

> **Trạng thái**: bản đề xuất, chờ chủ dự án duyệt câu chữ (PR roadmap v2).
> **Bảng trạng thái**: `production/ROADMAP.md`. File này và các file thẻ chỉ mô tả *cách làm* từng đầu việc; trạng thái `[ ]`/`[~]`/`[x]` chỉ ghi ở `ROADMAP.md`.

## Các file thẻ

| File | Mảng | Mã |
|---|---|---|
| `G-greybox.md` | Greybox vòng lõi: hình, di chuyển, hitbox, boss, chết/hồi sinh | G0–G10 |
| `H-hitbox.md` | Hitbox bản chính thức (sau chơi thử greybox) | H1–H3 |
| `A-art.md` | Sản xuất art: nhân vật mẫu, sổ cái CC0, nhân bản | A0–A5 |
| `L-levels.md` | Bối cảnh và màn chơi theo vùng | L1–L4 |
| `S-followups.md` | Việc theo dõi bảo mật/backend/CI | S1–S5 |

Các mã cũ (Giai đoạn 0–7, 1.5, 2B, N0–N9) giữ nguyên; thẻ của Giai đoạn N nằm ngay trong `ROADMAP.md` và `production/plans/narrative-foundation-2026-11-11.md`.

## Mẫu thẻ

```
### <MÃ> — <tên ngắn>
- Phụ thuộc: <mã phải [x] trước>
- Đọc trước: <file:dòng>
- Làm: <đầu ra cụ thể; file sẽ tạo/sửa>
- Không làm: <ranh giới phạm vi>
- Nghiệm thu: <lệnh chạy được + kết quả mong đợi>
- 🛑: <điểm phải dừng hỏi chủ dự án, nếu có>
```

## Quy tắc nhận việc (cho mọi agent: Claude, Codex, Antigravity)

1. Chỉ nhận thẻ có mọi mã ở dòng "Phụ thuộc" đã `[x]` trong `ROADMAP.md`.
2. Trước khi bắt đầu, kiểm tra không ai đang làm: `git ls-remote --heads origin | grep -i <mã>` và `gh pr list --state open --search <mã>`. Có kết quả → chọn thẻ khác.
3. Đặt tên nhánh có mã thẻ: `<loại>/<mã-viết-thường>-<mô-tả>` (ví dụ `feat/g2-pawn-visible`). Tiêu đề PR bắt đầu bằng `[<MÃ>]`.
4. Một PR = một thẻ. Không gộp, không làm trước việc của thẻ sau.
5. Dòng "Nghiệm thu" là điều kiện tối thiểu. Lệnh nghiệm thu phải **thất bại trước** khi làm và **pass sau** khi làm; dán cả hai kết quả vào PR.
6. Gặp 🛑 → dừng, ghi câu hỏi vào PR, không tự quyết.
