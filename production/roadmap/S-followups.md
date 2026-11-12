# Mảng S — Việc theo dõi bảo mật, backend, CI

> Nguồn: kết luận reviewer PR #71 (`production/PROGRESS.md` mục 27). Không chặn thử greybox cục bộ; S1–S5 phải được nghiệm thu trước khi mở server cho người ngoài (Giai đoạn 7), theo DECISIONS §11/§13. Kết nối dev ở G8 không thay nghiệm thu xác thực/mạng phát hành.

### S1 — Bỏ secret viết cứng
- Phụ thuộc: không.
- Đọc trước: `Source/ProjectAscendant/Public/Backend/PABackendPersistenceSubsystem.h:117`, `Services/Backend/config.py:20`, `Services/Backend/app.py:55`.
- Làm: secret đọc từ biến môi trường/config không commit; server từ chối khởi động ở build phát hành khi thiếu secret; so sánh bằng hàm hằng thời gian (`hmac.compare_digest`).
- Nghiệm thu: test backend chứng minh secret sai bị từ chối và không còn chuỗi secret trong repo (`git grep -n ascendant_dedicated_server_secret -- Source Services Config Tools` rỗng).

### S2 — Xác thực token thật khi đăng nhập
- Phụ thuộc: không.
- Đọc trước: `PAGameModeBase.cpp` (`CheckAuthToken` chỉ kiểm định dạng), `PABackendPersistenceSubsystem::ValidateClientToken` (chưa ai gọi).
- Làm: PreLogin/Login gọi backend kiểm token; giữ đường dev của G8.
- Nghiệm thu: test với token giả đúng định dạng bị từ chối.

### S3 — Auto-save dùng dữ liệu thật
- Phụ thuộc: không.
- Đọc trước: phần auto-save 300 s trong `PAGameModeBase.cpp` (CharacterId = tên actor, cấp và class viết cứng).
- Làm: lưu đúng CharacterId/cấp/class thật, hoặc tắt auto-save khi chưa có nhân vật đăng nhập; chỉ tạo subsystem trên server.
- Nghiệm thu: test kiểm payload gửi đi khớp trạng thái nhân vật.

### S4 — Backend kiểm quyền sở hữu khi lưu
- Phụ thuộc: không.
- Đọc trước: `save_character` trong `Services/Backend/`.
- Làm: từ chối lưu nhân vật không thuộc tài khoản của phiên.
- Nghiệm thu: `python3 Tools/QA/test_backend_postgres.py` có ca "lưu nhân vật của người khác" bị từ chối.

### S5 — Test đường kết nối thật và replication gameplay
- Phụ thuộc: G8, S2, G9.
- Đọc trước: `Tools/QA/test_multi_client_replication.py`, DECISIONS §11/§13, các ca CoreLoop đã xanh.
- Làm: test PreLogin qua đường kết nối (không chỉ gọi `CheckAuthToken`), trong runner multi-process; không chỉ kiểm chuỗi "Join succeeded". Đối chiếu server và ít nhất 2 client về HP, tag Hurt/Dead, dash/i-frame và boss/stagger; bổ sung các tính năng gameplay được nhận nghiệm thu ở giai đoạn sau.
- Nghiệm thu: 1 server + 2 client, token giả đúng định dạng bị từ chối; ca hợp lệ cùng quan sát trạng thái gameplay nhất quán, teardown sạch; lưu log với commit và dữ liệu assert. Mỗi tính năng chỉ được ghi Complete khi có ca mạng tương ứng.
