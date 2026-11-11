# Narrative — Thế giới, phe phái, nhân vật

> **Trạng thái**: Khung thư mục (N0, 2026-11-11). Chưa có nội dung canon.
> **Lộ trình**: `production/ROADMAP.md` — Giai đoạn N.
> **Nguồn chuẩn**: `production/DECISIONS.md` thắng mọi file trong thư mục này.

Thư mục này là nguồn duy nhất cho thế giới và nhân vật của game. GDD trong `design/gdd/` chỉ trỏ về đây, không lặp lại nội dung.

## Tiền đề đã chốt

Thế giới fantasy có thật. Hệ Thống (cấp độ, class, kỹ năng) là quy luật của thế giới mà cư dân nhìn thấy (DECISIONS §13, bổ sung 2026-11-11). Mọi nội dung khác chưa được quyết.

## Cấu trúc

| Thư mục | Nội dung | Mẫu | Đầu việc |
|---|---|---|---|
| `world/` | `world-bible.md`, `tone-guide.md`, địa lý | Chuẩn mục bên dưới | N3, N4 |
| `factions/` | Mỗi phe phái một file | `.claude/docs/templates/faction-design.md` | N4 |
| `characters/` | Mỗi class một file `<class>.md` | `.claude/docs/templates/narrative-character-sheet.md` + khối hình ảnh | N5 |
| `bestiary/` | Boss và quái theo vùng | Chuẩn mục bên dưới | N6 |
| `npcs/` | NPC có tên theo từng thành | `.claude/docs/templates/narrative-character-sheet.md` | N6 |

## Quy tắc

1. **Không tự đặt tên mới.** Tên thế giới, vùng, phe phái, NPC, boss chỉ được ghi vào đây sau khi chủ dự án chọn trên trang đề xuất (CLAUDE.md, mục CẤM).
2. **Mỗi mục ghi mức canon** ở đầu file: `Established` (chủ dự án đã chọn), `Provisional` (đang chờ chọn), `Under Review` (đang mâu thuẫn với file khác).
3. **Tên class, nhánh, bậc, vũ khí** lấy nguyên từ DECISIONS §2 và §7. Không viết tên tiếng Việt khác.
4. **Một nơi, một tên.** Bí danh bị loại ghi ở `design/registry/entities.yaml` (`deprecated_aliases`), không dùng lại trong văn bản.
5. **Đăng ký** mọi tên xuất hiện ở hơn một file vào `design/registry/entities.yaml` (`entities` với `kind:`, hoặc `locations`).
6. Các quy tắc còn lại theo `.claude/rules/narrative.md`: luật thế giới ghi rõ, bí ẩn có đáp án, lời thoại tối đa 120 ký tự mỗi dòng.

## Chuẩn mục cho file trong `world/` và `bestiary/`

```markdown
# <Tên>

> **Canon**: Established | Provisional | Under Review
> **Người chơi thấy**: Có | Một phần | Không
> **Nguồn quyết định**: <trang đề xuất / ngày chủ dự án chọn>

## Core Concept
## Rules
## History
## Connections
## Player Relevance
## Contradictions Check
```

## Cổng kiểm tra

`python3 Tools/QA/validate_gdd_consistency.py` quét `design/narrative/**/*.md` với cùng bộ luật như GDD.
