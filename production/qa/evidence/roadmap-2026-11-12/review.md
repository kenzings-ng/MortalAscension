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

## Điểm dừng sau vòng 3 (lịch sử)

Theo AGENTS.md §4: “Thử 3 lần vẫn không qua một cổng.” Parent dừng sửa nội dung kỹ thuật, chỉ giữ nhật ký và đưa bản nháp lên PR để chủ dự án xem. Chưa nhận VERDICT PASS; không merge, không báo nghiệm thu hoàn tất. Câu hỏi trong PR: chủ dự án có cho phép sửa ca MeleeHitsDummy theo đề xuất và review lại không?

## Tiếp tục theo chỉ thị chủ dự án

Chủ dự án trả lời “ok” cho yêu cầu sửa MeleeHitsDummy và review tiếp. Đã bổ sung kiểm đúng một lần nhận damage, đúng tổng sát thương với HP/phòng thủ cố định, tín hiệu trùng trong một cửa sổ và đòn kế tiếp vẫn hợp lệ; giữ nguyên 9 ca G1. Đồng thời làm rõ frame đã ghép ở A1 và thứ tự triển khai đường dev ở S2. Đây là sửa tiêu chí kế hoạch, chưa triển khai hay chạy các test Greybox. Reviewer vòng 4 đã kiểm lại phần sửa và kết luận PASS; chưa merge.

## Vòng 4 — PASS cho phần sửa tiêu chí kế hoạch

Claude reviewer xác nhận lỗi chặn chống hit kép giữa G1/G4 đã đóng: fixture cố định, đúng một lần nhận damage và đúng tổng sát thương, kích hoạt trùng vẫn chỉ một hit, đòn kế tiếp vẫn hợp lệ. Danh sách giữ đủ 9 ca. Hai ghi chú A1/S2 đã sửa, không có mâu thuẫn trực tiếp mới. Reviewer không chạy build/UE/test. Kết luận này chỉ áp dụng phần sửa tiêu chí kế hoạch vòng 4, không xác nhận game hay UE gate PASS; startup crash 139 vẫn là việc G0 cần chẩn đoán khi được duyệt.

Kiểm tra parent trước/sau sửa: validator GDD 114 file, 0 lỗi, 0 cảnh báo, 1 allowlist, exit 0; danh sách G1 đúng 9 tên test không trùng; `git diff --check` sạch. Không sửa Source/Config/Content hoặc DECISIONS trong lượt tiếp tục này.
