# SỔ TAY HƯỚNG DẪN CHƠI THỬ (PLAYTEST CHECKLIST)
## BẢN CHƠI ĐƯỢC (PLAYABLE SLICE) — PROJECT ASCENDANT

> **Phiên bản**: Playable Slice Integration (Bước Z4 / Giai đoạn 1.5)  
> **Mục tiêu**: Cung cấp tài liệu hướng dẫn và checklist chi tiết để Chủ dự án tự mình trải nghiệm trọn vẹn vòng lặp gameplay (Core Game Loop) theo DECISIONS §13 (`feel-core=playtest`).

---

## 0. Câu Hỏi Duy Nhất Của Lần Chơi Thử Greybox (cập nhật 2026-11-12)

> Theo lựa chọn của chủ dự án (DECISIONS §13 "Hướng đi và phân công agent"): chơi thử vòng lõi bằng greybox trước khi thêm nội dung lớn. Mục này dùng khi thẻ G10 (`production/roadmap/G-greybox.md`) sẵn sàng; trước đó nhân vật chưa hiện và đòn đánh chưa trúng.

**Câu hỏi**: *Né chuẩn để hạ một Lãnh Chúa mạnh hơn mình có đã tay không?* (Core Fantasy, `design/gdd/game-concept.md`)

**Phạm vi**: chỉ Vanguard, Stone Golem, map `L_VerdantFrontier_Outpost` (Bước 3 và Bước 5 bên dưới). Bỏ qua shop, forge, 2 client trong lần này. Hình là khối tạm; đánh giá cảm giác, không đánh giá hình. Bật hitbox debug bằng `pa.Debug.Hitbox 1` trong console nếu cần.

**Giữ (vòng lõi đạt) khi**:
- Chết vì lỗi của mình (né sai nhịp), không phải vì không thấy đòn tới.
- Sau khi chết, muốn đánh lại "thêm một lần" mà không cần ai nhắc.
- Lần đầu làm Boss vỡ thế và kết liễu thấy thỏa mãn.

**Dừng lại sửa trước khi làm thêm nội dung khi**:
- Không đọc được đòn của Boss (không biết lúc nào phải né).
- Né đúng nhịp mà vẫn trúng đòn, hoặc né sai mà vẫn thoát.
- Đánh trúng Boss không có cảm giác lực (cần hit-stop, rung, âm thanh).
- Thấy chán trước khi Boss vỡ thế lần đầu.

**Ghi lại**: điền bảng ở mục 3, thêm một câu "giữ / dừng lại sửa" và lý do. Một người ngoài chơi thử mà không được hướng dẫn trước (ROADMAP Giai đoạn 7) sẽ cho thêm thông tin, nhưng không bắt buộc ở lần này.

---

## 1. Cấu Hình & Lệnh Khởi Chạy (Launcher Commands)

Dự án đã tối ưu hóa script khởi chạy `run_game.sh` cho môi trường đồ họa Hybrid (Intel Display + NVIDIA Headless GPU trên Linux/Xwayland):

| Chế độ | Lệnh thực thi | Mô tả |
| :--- | :--- | :--- |
| **Standalone (Khuyến nghị)** | `./run_game.sh` | Mở game 1 người chơi ở độ phân giải cửa sổ 1280x720. |
| **Toàn màn hình** | `./run_game.sh game 1920 1080` | Mở game toàn màn hình Full HD (chống swapchain freeze). |
| **Mở kèm Console Log** | `./run_game.sh game-log` | Mở game kèm cửa sổ log trực tiếp để theo dõi sự kiện gameplay. |
| **Mạng Cục Bộ (1 Server + 2 Clients)** | `./run_game.sh playtest` | Tự động mở 1 Dedicated Server trong nền và 2 cửa sổ Client song song kết nối vào `127.0.0.1:7777`. |
| **Mạng Mô Phỏng Lag & Loss** | `./run_game.sh playtest-lag` | Mở 1 Server + 2 Client kèm mô phỏng độ trễ 150ms và mất gói 2% (ROADMAP Giai đoạn 1.5, điểm dừng chơi thử mạng). |
| **Kết Nối Client Đơn Lẻ** | `./run_game.sh client [IP:PORT]` | Kết nối một client vào server chỉ định (mặc định `127.0.0.1:7777`). |

> [!TIP]
> Trong chế độ `playtest` hoặc `playtest-lag`, bạn có thể nhấn **`Ctrl + C`** trong cửa sổ terminal để script tự động đóng sạch toàn bộ Dedicated Server và 2 Client cùng lúc.

---

## 2. Checklist Các Bước Trải Nghiệm (Step-by-Step Walkthrough)

### Bước 1: Đăng Nhập & Fast Playtest
- [ ] Chạy `./run_game.sh`.
- [ ] Màn hình Đăng nhập hiển thị đúng phong cách Dark Fantasy (tông màu Dark/Gold, viền bo góc).
- [ ] Chuyển sang tab **Fast Play** và bấm nút **`⚡ 1-CLICK VÀO CHỌN NHÂN VẬT ⚡`** (hoặc đăng ký tài khoản mới tại tab Đăng Ký).
- [ ] Xác nhận giao diện chuyển mượt mà sang màn hình Chọn Nhân Vật.

### Bước 2: Chọn Chức Nghiệp Nền Tảng (Foundational Classes)
- [ ] Màn hình chọn nhân vật hiển thị 3 class: **Vanguard (Tiên Phong)**, **Ranger (Xạ Thủ)**, **Arcanist (Bí Thuật Sư)** *(code hiện hiển thị "Chiến Binh" cho Vanguard; sửa ở N7)*.
- [ ] Click chọn từng class: khung viền chuyển sang màu vàng kim (Gold), thông số Máu (HP), Thể lực (Stamina), Thế đứng (Posture), và Danh sách Kỹ năng starter cập nhật chính xác.
- [ ] Chọn **Vanguard** và nhấn nút **`BẮT ĐẦU HÀNH TRÌNH`**.
- [ ] Giao diện Character Select tự động đóng, đưa nhân vật vào Thị Trấn Tiền Trạm (`L_VerdantFrontier_Outpost`).

### Bước 3: Điều Khiển Nhân Vật & Khám Phá Outpost
- [ ] **Giao diện HUD**:
  - [ ] Cụm Vitals hiển thị ở góc trên-trái: Máu Đỏ (Health), Thể Lực Xanh Lá (Stamina), Nội Lực Xanh Dương (Mana), và Thế Đứng Vàng (Posture).
- [ ] **Di chuyển**:
  - [ ] Bấm các phím **`W`**, **`A`**, **`S`**, **`D`** để di chuyển nhân vật theo góc nhìn 2.5D Isometric (nghiêng 45°).
- [ ] **Lướt né đòn (Dash)**:
  - [ ] Nhấn phím **`Space`**: Nhân vật lướt nhanh theo hướng di chuyển (thời lượng 0.35s).
- [ ] **Tấn công thường (Melee Attack Combo)**:
  - [ ] Nhấn **`Chuột Trái`**: Thi triển đòn chém cơ bản. Nhấp liên tục để kích hoạt chuỗi combo 3 đòn.

### Bước 4: Tương Tác Dịch Vụ NPC Trong Outpost
- [ ] **Đe Rèn / Lò Rèn (`APABlacksmithForge`)**:
  - [ ] Tiến lại gần lò rèn (bán kính <= 300cm).
  - [ ] Nhấn phím **`F`**: Cửa sổ Đe Rèn (Forge UI) mở lên ở trung tâm màn hình.
  - [ ] Nhấn phím **`Escape`** hoặc di chuyển ra xa (> 500cm): Cửa sổ tự động đóng lại an toàn.
- [ ] **Thương Nhân Lang Thang (`APAWanderingSmugglerNPC`)**:
  - [ ] Tiến lại gần Smuggler (bán kính <= 300cm).
  - [ ] Nhấn phím **`F`**: Cửa sổ Shop mở lên, danh mục vật phẩm hiển thị với đơn vị tiền tệ Tàn Trang (`item_skill_shard`) kèm tỷ lệ phụ phí Karma.
  - [ ] Nhấn phím **`Escape`** để đóng cửa sổ.

### Bước 5: Khiêu Chiến Boss Cổ Thạch Golem (Stone Golem Boss Arena)
- [ ] Rời khỏi Outpost qua **Cổng Đông (Palisade Gate East)** tại `X=1800` để tiến vào Đấu trường ở `X=3200`.
- [ ] **Thức tỉnh & Kéo nộ (Aggro)**:
  - [ ] Khi bước vào bán kính 1000cm của Boss, Boss chuyển sang trạng thái chiến đấu.
  - [ ] Thanh máu Boss (Boss HUD) xuất hiện trang trọng ở cạnh trên màn hình với tên "Cổ Thạch Golem" và thanh Thế Đứng (Posture Reticle).
- [ ] **Chiêu thức Ground Slam (Dậm Đất AoE)**:
  - [ ] Quan sát Boss chuẩn bị chiêu dậm đất (gán tag `State.Attacking`).
  - [ ] Dùng phím **`Space` (Dash)** lướt thoát khỏi bán kính 300cm của đòn dậm đất để tránh sát thương diện rộng.
- [ ] **Vỡ thế (Posture Break & Stagger)**:
  - [ ] Tấn công liên tục để rút cạn thanh Posture màu vàng của Boss.
  - [ ] Khi Posture về 0: Boss khụy xuống, nhận tag `State.Staggered` và xuất hiện hiệu ứng choáng trong 5 giây.
  - [ ] Tiếp cận Boss và tung đòn đánh: Kích hoạt đòn Kết Liễu (Finisher) gây lượng sát thương lớn.
- [ ] **Cơ chế Quay Đầu (Leash Reset)**:
  - [ ] Dụ Boss chạy lùi về hướng cổng Outpost (vượt quá 2500cm tính từ điểm spawn của Boss).
  - [ ] Boss tự động dừng truy đuổi, xóa mục tiêu khỏi bảng nộ khí, nhận tag `State.Invulnerable` (bất tử) và quay bước trở lại tâm đấu trường.

### Bước 6: Kiểm Tra Đồng Bộ Mạng 2 Client (`./run_game.sh playtest`)
- [ ] Chạy `./run_game.sh playtest` để mở đồng thời Client 1 (trái) và Client 2 (phải).
- [ ] Ở cả 2 màn hình, chọn Fast Play và đăng nhập 2 nhân vật (ví dụ Client 1 chọn Vanguard, Client 2 chọn Ranger).
- [ ] Cả 2 nhân vật cùng xuất hiện trong Outpost:
  - [ ] Di chuyển Client 1: Màn hình Client 2 thấy Client 1 di chuyển mượt mà theo thời gian thực.
  - [ ] Di chuyển Client 2: Màn hình Client 1 thấy Client 2 di chuyển tương ứng.
- [ ] Cùng tiến ra Đấu trường đánh Boss:
  - [ ] Client 1 đánh Boss trước -> Boss nhắm vào Client 1.
  - [ ] Client 2 dồn sát thương vượt mốc 130% nộ khí -> Boss tự động quay đầu chuyển mục tiêu sang Client 2.
  - [ ] Hiệu ứng vỡ thế (Stagger) và thanh máu Boss đồng bộ chính xác trên cả 2 máy khách.

---

## 3. Biểu Mẫu Ghi Nhận Cảm Giác Chơi (`feel-core=playtest`)

Sau khi chơi thử, Chủ dự án có thể ghi lại các nhận xét về cảm giác chơi để định hình các đợt cân chỉnh tiếp theo:

| Yếu tố cảm giác chơi | Cảm nhận thực tế | Trạng thái đề xuất |
| :--- | :--- | :--- |
| **Góc nhìn Camera (FOV 90° Isometric)** | Góc nhìn có bao quát và rõ ràng không gian đấu trường không? | `[ ] Duyệt` / `[ ] Chỉnh góc` |
| **Nhịp độ Dash (0.35s)** | Quãng đường lướt và thời gian hồi có đủ nhanh để né đòn dậm đất không? | `[ ] Duyệt` / `[ ] Tăng` / `[ ] Giảm` |
| **Trọng lượng đòn đánh (Combo Melee)** | Cảm giác ra đòn của Vanguard có độ đầm chắc không? | `[ ] Duyệt` / `[ ] Cần thêm hit-stop` |
| **Cửa sổ kết liễu Finisher (5.0s)** | Thời gian Boss quỳ khi vỡ thế có vừa vặn để lao vào kết liễu không? | `[ ] Duyệt` / `[ ] Cần tăng/giảm` |
| **Độ mượt mạng (Lag 150ms / Loss 2%)** | Cử động của nhân vật khác khi lag có bị giật hay teleport không? | `[ ] Đạt chuẩn Iris` / `[ ] Cần tuning` |
