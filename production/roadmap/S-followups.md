# Mảng S — Việc theo dõi bảo mật, backend, CI

> Nguồn: kết luận reviewer PR #71 (`production/PROGRESS.md` mục 27). Không chặn greybox; nên làm trước khi mở server cho người ngoài (Giai đoạn 7).

### S1 — Bỏ secret viết cứng
- Đọc trước: `Source/ProjectAscendant/Public/Backend/PABackendPersistenceSubsystem.h:117`, `Services/Backend/config.py:20`, `Services/Backend/app.py:55`.
- Làm: secret đọc từ biến môi trường/config không commit; server từ chối khởi động ở build phát hành khi thiếu secret; so sánh bằng hàm hằng thời gian (`hmac.compare_digest`).
- Nghiệm thu: test backend chứng minh secret sai bị từ chối và không còn chuỗi secret trong repo (`git grep ascendant_dedicated_server_secret` rỗng).

### S2 — Xác thực token thật khi đăng nhập
- Đọc trước: `PAGameModeBase.cpp` (`CheckAuthToken` chỉ kiểm định dạng), `PABackendPersistenceSubsystem::ValidateClientToken` (chưa ai gọi).
- Làm: PreLogin/Login gọi backend kiểm token; giữ đường dev của G8.
- Nghiệm thu: test với token giả đúng định dạng bị từ chối.

### S3 — Auto-save dùng dữ liệu thật
- Đọc trước: phần auto-save 300 s trong `PAGameModeBase.cpp` (CharacterId = tên actor, cấp và class viết cứng).
- Làm: lưu đúng CharacterId/cấp/class thật, hoặc tắt auto-save khi chưa có nhân vật đăng nhập; chỉ tạo subsystem trên server.
- Nghiệm thu: test kiểm payload gửi đi khớp trạng thái nhân vật.

### S4 — Backend kiểm quyền sở hữu khi lưu
- Đọc trước: `save_character` trong `Services/Backend/`.
- Làm: từ chối lưu nhân vật không thuộc tài khoản của phiên.
- Nghiệm thu: `python3 Tools/QA/test_backend_postgres.py` có ca "lưu nhân vật của người khác" bị từ chối.

### S5 — Test đường kết nối thật
- Làm: test PreLogin qua đường kết nối (không chỉ gọi `CheckAuthToken`), trong `Tools/QA/test_multi_client_replication.py` hoặc test UE có world.
