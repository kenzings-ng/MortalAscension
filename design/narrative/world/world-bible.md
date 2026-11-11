# World Bible — Mortal Ascension

> **Canon**: Established cho các mục 1–10, trừ chỗ ghi rõ "Provisional". Mục 11 là danh sách mâu thuẫn còn mở, chưa phải canon.
> **Người chơi thấy**: Một phần. Mục "Sự thật ẩn" chỉ dành cho người làm game.
> **Nguồn quyết định**: chủ dự án chọn qua hộp chọn trong terminal ngày 2026-11-11 (ROADMAP Giai đoạn N, mục N3): **Gói B — Xác Thần** (`1A · 2B · 3B · 4B · 5B · 6B · 7B · 8B`), từ vựng `9C`, tông `10A`; tên nhóm người chơi **Ascendant**; tên thế giới **chưa đặt** (kiểm tra trùng tên trước, hỏi lại ở N4). Ghi tại `production/DECISIONS.md` §13 "World bible (N3)".
> **Tiền đề đã chốt** (DECISIONS §13, 2026-11-11): thế giới fantasy có thật; Hệ Thống là quy luật mà cư dân nhìn thấy.
> **Tông**: theo `tone-guide.md` cùng thư mục.

Tài liệu này là nguồn duy nhất cho thế giới. GDD trong `design/gdd/` trỏ về đây, không lặp lại nội dung. Tên vùng, thành, phe phái, NPC, boss mới thuộc N4 và N6; tài liệu này chỉ dùng tên đã có trong GDD.

---

## 1. Core Concept

Từ thuở xưa, một vị thần đã rơi xuống và chết. Nơi thần rơi nay là một lòng chảo khổng lồ; xác thần nằm ở tâm, nơi gọi là **Forbidden Sanctum** (Cấm Địa Thần Tích, theo đúng nghĩa đen: chỗ còn dấu thần).

Thân xác ấy vẫn chưa nguội hẳn. **Hệ Thống** (cấp độ, class, kỹ năng) là **luật còn sót lại** của vị thần, vẫn chạy trên mọi sinh vật sống trong lòng chảo, như phản xạ của một cơ thể đã chết. Mọi cư dân nhìn thấy nó: ai cũng biết cấp và class của mình, thấy màu tên (Karma) của người khác.

Phàm nhân thăng lên bằng **thần tính vay mượn**: nuốt chữ thần trong sách, nhặt mảnh thần từ xác những sinh vật đã nuốt nó. Câu hỏi treo suốt game: nếu một người gom đủ, họ có thành thần mới không, và có nên không? Đó là nghĩa của **Mortal Ascension**.

## 2. Rules (luật thế giới)

Các luật dưới đây là tuyệt đối trong truyện. Muốn phá luật nào phải có quyết định mới của chủ dự án.

### 2.1 Hệ Thống
1. **Hệ Thống là chữ của thần.** Con số hiện ra như chữ viết, không có giao diện máy móc. Sách Kỹ Năng là bản chép một đoạn chữ thần tìm thấy trong tàn tích; khi học, chữ "tan biến vào tâm trí" (`skill-progression-system.md:22`). Quyển Trục thăng chức là đoạn chữ dài hơn, dùng để định hình phần thần tính đã ngấm. Tàn Trang là trang chữ thần cháy dở.
2. **Không ai viết thêm được.** Người ta chỉ tìm, chép, đọc lại chữ đã có. Không có "người tạo ra Hệ Thống" còn sống.
3. **Hệ Thống đo sức, không đo tài.** Nó đếm được phần thần tính một sinh vật đã ngấm (cấp độ), nhưng không đếm được phản xạ, căn thời gian, đọc vị đòn. Vì vậy một người cấp 10 giỏi né có thể hạ Lãnh Chúa cấp 40 (`game-concept.md:44`).
4. **Class đến từ giác ngộ, không phải được ban.** Class mở khi thân và hồn đã ngấm đủ một phần thần tính: hạ một Lãnh Chúa, giải mã một tàn tích. Quyển Trục chỉ giúp định hình phần đó.
5. **Mọi sinh vật có cấp, kể cả cư dân và quái.** Vệ Binh cấp 50 là người đã ngấm nhiều thần tính, không phải người được ưu ái.
6. **Đổi class chỉ ở Tòa Thành** (`zone-system.md:62-63`), vì mỗi Tòa Thành dựng quanh một mảnh thần lớn; chỉ ở đó thần tính trong người mới được định hình lại.

### 2.2 Mảnh thần, Lãnh Chúa, Hư Không
7. **Lãnh Chúa** là sinh vật hoặc người đã nuốt phải một mảnh thần. Mảnh thần cho chúng sức mạnh vượt cấp, và vì vậy chúng mang dấu của luật thần rõ hơn mọi thứ khác. **Linh Hồn Lãnh Chúa (Boss Soul)** là mảnh thần rơi ra khi chúng chết. Class T3 mở bằng cách hạ một Lãnh Chúa cụ thể (`game-concept.md:130-134`) vì đó là cách ngấm phần thần tính tương ứng.
8. **Hư Không** là chỗ thần tính đã rút đi. Cái còn lại không phải là "không có gì" mà là **một cơn đói** muốn lấp chỗ trống. Nó lan từ tâm Forbidden Sanctum ra ngoài. Hư Không mạnh nhất ở vùng 3 và ở các "rãnh vực không thời gian" (`zone-system.md:42`).
9. **Quặng Hư Không** là nơi thần tính bị Hư Không ăn qua rồi đông lại: cứng nhất, rèn được, nhưng mang theo cơn đói (`blacksmithing-system.md:64`, `:78`).
10. **Thời gian gãy quanh xác thần.** Càng gần tâm, thời gian càng không liền. Đây là nguồn ma pháp của class Chronomancer và của khả năng ngưng đọng thời gian của God Slayer.

### 2.3 Người chơi (Ascendant)
11. **Ascendant** là những người sinh ra mang một **tia lửa** tản mát của chính vị thần đã chết. Họ không phải người được chọn; tia lửa rơi vãi khắp các thế hệ sau khi thần chết, và gần đây ngày càng nhiều người "thức".
12. **Ascendant chết rồi được nắn lại.** Lửa của mảnh thần trong Tòa Thành (cột hào quang, `zone-system.md:300`) nhận ra tia lửa cùng gốc và nắn người đó lại ở điểm lửa gần nhất. Vị trí hồi sinh cụ thể do N4 chốt; lore này dùng được cho cả Tòa Thành lẫn điểm lửa ngoài hoang dã.
13. **Vệt Tro Tàn (Ash Remnant).** Mỗi lần chết, phần chữ thần đang học dở (Tàn Trang) cháy thành tro ở chỗ chết. Chết lần nữa trước khi nhặt lại thì lửa ăn mất tro cũ (`zone-system.md:88-91`).
14. **Cư dân thường không được nắn lại.** Họ không mang tia lửa. Đây là lý do NPC chết là chết hẳn.
15. **Vì sao nhiều Ascendant cùng lúc**: tia lửa đã rơi vãi khắp các thế hệ sau khi thần chết; khi cơn đói Hư Không mạnh lên, những người mang tia lửa lần lượt thức dậy.

### 2.4 Luật giữa người với người
16. **Karma là phản xạ vô tri của luật xác thần.** Không ai viết luật này. Khi một người mang tia lửa giết một người mang tia lửa chưa mang dấu (người vô tội), luật tự đánh dấu kẻ giết. Tự vệ và giết kẻ đã mang dấu không bị đánh dấu (`zone-system.md:135`, `:137`). Dấu hiện ra ở màu tên mà ai cũng thấy (`game-concept.md:60-66`).
17. **Liên Minh** là chính thể của ba Tòa Thành (Verdant Bastion, Ashen Keep, Sanctum Fortress), dựng quanh ba mảnh thần lớn. Liên Minh **không viết** Karma; họ đọc dấu và thi hành. Tên tiếng Anh của Liên Minh do N4 đặt.
18. **Vệ Binh** mượn sức mạnh mảnh thần trong Tòa Thành qua vũ khí và tháp canh. "Thần Uy Trấn Áp" và "Thần Phạt" (`zone-system.md:248`, `:252`) là sức mạnh mảnh thần thật, không phải cách gọi văn chương.
19. **Gột dấu Karma**: dấu phai dần khi kẻ có tội sống xa người khác giữa xác thần (chuộc tội ngoài hoang dã); Chợ Đen cạo dấu bằng cổ vật cấm địa, tức mảnh thần lậu, giá cắt cổ (`merchant-economy.md:22`, `:122-125`). Con số gameplay giữ nguyên GDD.

## 3. History (lịch sử)

> Mốc thời gian dưới đây chỉ có thứ tự, chưa có năm. Số năm và tên các thời kỳ là nội dung mới, chờ N4.

1. **Thần rơi.** Vị thần rơi xuống, tạo lòng chảo. Thần không bị ai bên ngoài giết (xem Sự thật ẩn).
2. Thời kỳ mảnh thần văng ra: mảnh thần văng khắp lòng chảo. Sinh vật nuốt phải thành Lãnh Chúa. Chữ thần khắc trên tàn tích là nguồn của Sách Kỹ Năng.
3. Thời kỳ Hư Không lan: chỗ thần tính rút đi bắt đầu đói. Hư Không lan từ tâm ra.
4. **Dựng ba Tòa Thành.** Người sống sót dựng thành quanh ba mảnh thần lớn nhất, thắp lửa, lập Liên Minh.
5. **Hiện tại: các Ascendant thức dậy.** Cơn đói Hư Không mạnh lên; người mang tia lửa lần lượt thức. Một phe người bắt đầu gom Boss Soul để dựng lại thần.

## 4. Thần linh và tín ngưỡng

- **Chỉ có một vị thần, và đã chết.** Không có thần nào khác đang sống.
- **Tín ngưỡng chính** (nhánh Tín Đồ: Acolyte, Inquisitor, Seraph; Templar thuộc nhánh Hộ Vệ nhưng cũng là người giữ tín ngưỡng) thờ **ánh sáng còn sót trong mảnh thần**, tức lửa của Tòa Thành; coi vị thần như người tử đạo.
- **Dị giáo**: muốn **dựng lại thần** bằng cách gom mảnh thần. Đây là gốc của "giáo sĩ tha hóa" chiếm tu viện (`game-concept.md:123`) và "lăng mộ thánh địa bị ô uế" (`game-concept.md:119`).
- **"World Boss Thần Linh"** (`skill-progression-system.md:201`) là **loại** thực thể: phần lớn nhất còn cử động của xác thần, hoặc một Lãnh Chúa đã nuốt quá nhiều mảnh đến mức thành gần như thần. Boss cụ thể do N6 chọn. Điều kiện mở God Slayer giữ nguyên DECISIONS §2.

## 5. Mối đe dọa trung tâm

- **Void Sovereign** (tên có sẵn, `character-visual-system.md:112`) là ý chí của cơn đói Hư Không. Mục tiêu: ăn hết mảnh thần, kể cả tia lửa trong các Ascendant. Mô tả của nó là thực thể Hư Không, **không phải thực thể vũ trụ** (xem `tone-guide.md`).
- **Phe muốn dựng lại thần**: người gom Boss Soul và mảnh thần, tin rằng có thần thì có trật tự. Họ không biết dựng lại thần cũng là mở phong ấn cho cơn đói. Tên phe do N4 đặt.
- **Màu xám**: Liên Minh cấm dựng lại thần, nhưng chính Liên Minh sống nhờ mảnh thần trong ba Tòa Thành.

## 6. God Slayer (Kẻ Diệt Thần)

- God Slayer **kết liễu phần còn lại của vị thần** trước khi ai đó (hoặc chính Hư Không) lắp nó lại. Việc đó đúng hay sai, game để người chơi tự trả lời.
- Ngưng đọng thời gian 1s khi Né Hoàn Hảo: dùng thời gian đã gãy quanh xác thần (luật 10).
- Sao chép chiêu đặc trưng của Boss vừa hạ: đọc được chữ thần trong Lãnh Chúa.
- **Thần Binh Tự Biến Hình** (DECISIONS §7): một mảnh thần chưa chịu yên một hình, biến đổi giữa kiếm 1 tay và cán dài.
- Điều kiện mở class giữ nguyên DECISIONS §2; lore không thêm điều kiện.

## 7. Chuyển Sinh (chỉ nghĩa trong truyện)

Chuyển Sinh Linh Hồn (`systems-index.md:40`, `:78`) là việc một Ascendant **trả tia lửa về lửa** để được nắn lại từ đầu, giữ lại một phần thần tính. Cơ chế gameplay chưa có GDD (`reincarnation.md` chưa tồn tại); tài liệu này không đặt cơ chế.

## 8. Connections (liên kết)

| Chủ đề | Nguồn |
|---|---|
| Ba vùng, ba Tòa Thành | `design/gdd/zone-system.md:40-42` |
| Hồi sinh, Vệt Tro Tàn | `design/gdd/zone-system.md:88-91` |
| Karma, Wanted, Chợ Đen | `design/gdd/game-concept.md:60-66`; `design/gdd/merchant-economy.md:20-22`, `:122-126` |
| Sách Kỹ Năng, Quyển Trục, Tàn Trang | `design/gdd/skill-progression-system.md`; `design/gdd/advanced-classes.md`; DECISIONS §5 |
| Boss Soul, Thần Binh, Quặng Hư Không | `design/gdd/blacksmithing-system.md`; `design/gdd/itemization.md` |
| Class và điều kiện mở | DECISIONS §2; `design/gdd/game-concept.md:101-141` |
| Quái và boss theo vùng | `design/gdd/character-visual-system.md:107-112` (hợp nhất ở N6) |
| Tông, từ vựng cấm | `design/narrative/world/tone-guide.md` |

## 9. Player Relevance (người chơi thấy gì)

- Thấy: Hệ Thống là chữ; Karma là màu tên; Ascendant được nắn lại, cư dân thì không; Lãnh Chúa rơi mảnh thần; Hư Không lan từ tâm; Liên Minh và phe dựng thần.
- Hé dần: Hệ Thống là luật của xác thần; người chơi mang tia lửa của chính vị thần đó.
- Không bao giờ nói thẳng trong game: Sự thật ẩn bên dưới.

## 10. Sự thật ẩn (chỉ dành cho người làm game)

> Theo `.claude/rules/narrative.md`: mọi bí ẩn phải có đáp án, kể cả khi người chơi không bao giờ biết.

- **Vì sao thần chết**: vị thần không bị ai giết. Nó tự xé mình để phong ấn cơn đói (gốc của Hư Không). Dựng lại thần cũng là mở lại phong ấn.
- **Ai còn biết**: không ai còn sống biết đầy đủ. Người thợ rèn cổ đại mù lòa ở Lò Rèn Cấm Địa (`blacksmithing-system.md:25`) có thể là nhân chứng cuối cùng. *(Provisional: N6 quyết khi làm NPC.)*
- **Vì sao tia lửa thức dậy bây giờ**: cơn đói đang mạnh lên và đã bắt đầu gặm vào mảnh thần trong ba Tòa Thành; tia lửa trong người, cùng gốc với mảnh thần, bị đánh thức như một phản xạ tự vệ của chính xác thần.

## 11. Contradictions Check (mâu thuẫn còn lại)

| # | Mâu thuẫn | Xử lý |
|---|---|---|
| 1 | Hồi sinh: `zone-system.md:15` nói không dùng Lửa Trại, nhưng `:91`, `:96`, `:109`, `:247`, `:386`, `merchant-economy.md:21`, `:95`, `art-bible.md:26`, `:92` vẫn dùng. | Lore dùng "điểm lửa gần nhất", hợp cả hai. **N4** chốt vị trí. |
| 2 | "World Boss Thần Linh" không có trong danh sách boss nào; điều kiện mở God Slayer lệch giữa DECISIONS:45, `game-concept.md:141`, `:174`, `advanced-classes.md:19`, `:27`. | Lore định nghĩa loại thực thể (mục 4). Boss cụ thể: **N6**. Sửa điều kiện mở chạm DECISIONS §2: 🛑 hỏi chủ dự án. |
| 3 | "Lãnh Chúa" vừa là boss thường (`game-concept.md:173`) vừa là World Boss bậc 4 (`character-visual-system.md:100`, `:112`). | Lore: Lãnh Chúa = sinh vật đã nuốt mảnh thần, ở mọi bậc; bậc do lượng mảnh. Phân tầng boss: **N6**. |
| 4 | Nguồn sức mạnh Vệ Binh. | Đã giải quyết: luật 18. |
| 5 | "Liên Minh" vừa là chính thể (`merchant-economy.md:91`) vừa là tổ đội đột kích (`multiplayer-coop.md:314`). | Lore: Liên Minh = chính thể. Tổ đội đột kích cần tên khác: **N4** (ghi lại). |
| 6 | Từ vựng lệch tông (khí công, Chân Khí, plasma, vũ trụ) và từ ngữ meta. | Quy tắc ở `tone-guide.md`; sửa từng chỗ: **N5** (class) và các đầu việc sau. |
| 7 | "chuyển sinh vào Ngục Tối" (`zone-system.md:397`) trùng tên hệ thống Chuyển Sinh. | Đã sửa trong N3 thành "áp giải vào Ngục Tối". |
| 8 | Gần mô típ Dark Souls (lửa, tro, linh hồn, lãnh chúa). | Nét riêng bắt buộc: Hệ Thống là **chữ** của thần; người chơi là **mảnh của chính vị thần**. Art và văn bản không sao chép mô típ đống lửa/linh hồn của tác phẩm khác. |
| 9 | Lãnh Chúa và boss bị nhiều người chơi hạ đi hạ lại (MMO), trong khi lore nói Lãnh Chúa chết thì rơi mảnh thần. | Chưa có lời giải trong truyện. **N6** quyết (ví dụ mảnh thần trở về và tái tụ). |
| 10 | "Ascendant" chưa đăng ký trong `design/registry/entities.yaml` vì các `kind:` hiện có không có loại "nhóm người chơi". | **N4** quyết có thêm `kind` mới không. |
