# Nhật ký Claude reviewer — roadmap v2

Phạm vi chỉ đọc: diff từ origin/main, bảng đối chiếu, biên bản lệnh và facts. Reviewer chạy bằng Claude CLI `--agent reviewer --permission-mode plan`, phiên mới từng vòng; không nối lại phiên Claude đang dừng. Reviewer không gọi mạng, không build và không chạy lại UE; CI/ruleset được đối chiếu theo biên bản parent.

## Vòng 1 — FAIL

5 lỗi chặn: quyền cập nhật thẻ mới, G0 chưa có phạm vi xử lý startup crash, chú giải `[~]` lệch nhau, phụ thuộc N5 chặn art, điều kiện merge chưa rõ. Đã sửa trong bản nháp; vòng 2 xác nhận đóng.

## Vòng 2 — FAIL

4 lỗi chặn: A2–A5 thiếu nghiệm thu; grep S1 tự khớp tài liệu; thứ tự S5 không khớp phụ thuộc; dòng phụ thuộc G10 lẫn ghi chú S5. Đã sửa, vòng 3 xác nhận đóng. Các ghi chú khác (event hitbox đã có, heuristic flipbook, MoveOnInput, ngoại lệ nghiệm thu G8 và quyền cập nhật ROADMAP) cũng được sửa.

## Vòng 3 — FAIL: còn một lỗi chặn

Reviewer xác nhận 8/9 điểm vòng 2 đã đóng và chỉ ra lỗi mới từ sửa G4:

> G4 yêu cầu chống hit kép nhưng `Greybox.CoreLoop.MeleeHitsDummy` ở G1 chỉ đòi HP giảm; một đòn gây sát thương hai lần vẫn có thể pass. Nếu tạo test mới thì số test khác 9 đã ghi trong G1/G9/G10.

Đề xuất của reviewer: mở rộng assert của ca MeleeHitsDummy hiện có để HP giảm đúng một lần, bằng đúng sát thương của một đòn trong mỗi cửa sổ hitbox; ghi điều kiện đó ở dòng Nghiệm thu G4. Như vậy vẫn giữ 9 test.

Hai ghi chú không chặn: A1 nên nói rõ gate đo frame nhân vật đã ghép; S2 nên nói rõ đường dev là phần G8 sẽ triển khai nếu S2 làm trước.

## Điểm dừng bắt buộc

Theo AGENTS.md §4: “Thử 3 lần vẫn không qua một cổng.” Parent dừng sửa nội dung kỹ thuật, chỉ giữ nhật ký và đưa bản nháp lên PR để chủ dự án xem. Chưa nhận VERDICT PASS; không merge, không báo nghiệm thu hoàn tất. Câu hỏi trong PR: chủ dự án có cho phép sửa ca MeleeHitsDummy theo đề xuất và review lại không?
