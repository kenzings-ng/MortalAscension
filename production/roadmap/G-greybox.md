# Mảng G — Greybox vòng lõi

> **Câu hỏi duy nhất** (`production/PLAYTEST_CHECKLIST.md` §0): né chuẩn để hạ một Lãnh Chúa mạnh hơn mình có đã tay không?
> **Phạm vi**: Vanguard, Stone Golem, map `Content/Maps/L_VerdantFrontier_Outpost.umap`. Hình là khối tạm hoặc flipbook có sẵn; **không làm art mới** trong mảng này.
> **Vì sao có mảng này**: 89/89 test UE pass nhưng nhân vật không hiện, đòn đánh không trúng, boss không chạy. Các test hiện có không tick world, không bơm input, và tự đăng ký GameplayTag lúc chạy nên che mất tag thiếu trong `Config/DefaultGameplayTags.ini`.

## Quy ước test của mảng G

- Test mới đặt tên `Greybox.CoreLoop.<Tên>`, **không** nằm dưới tiền tố `ProjectAscendant.` để cổng mặc định (`run_headless_tests.sh`, lọc `ProjectAscendant.`) vẫn xanh trong lúc greybox còn đỏ.
- Chạy riêng: `./Tools/QA/run_headless_tests.sh --ue-only --filter=Greybox.CoreLoop`.
- Test của mảng G **không được** gọi `AddNativeGameplayTag`/đăng ký tag lúc chạy; tag phải đến từ file ini.
- G10 chuyển toàn bộ sang `ProjectAscendant.CoreLoop.*` khi tất cả đã xanh.

---

### G0 — Cổng biên dịch cục bộ
- Phụ thuộc: không.
- Đọc trước: `Tools/QA/run_headless_tests.sh`, `.github/workflows/tests.yml`, `.github/PULL_REQUEST_TEMPLATE.md`, `production/PROGRESS.md` mục 27 (main từng không biên dịch từ PR #55 tới PR #71 vì CI không build C++).
- Làm:
  1. `Tools/QA/run_local_gate.sh`: build `ProjectAscendantEditor` (lệnh ở `AGENTS.md`), rồi chạy test UE, validator GDD; in một khối tóm tắt cuối (commit, build OK/FAIL, số test Discovered/Passed/Failed, QueueFinished, exit code). Exit khác 0 nếu bất kỳ bước nào hỏng.
  2. Thêm vào `.github/PULL_REQUEST_TEMPLATE.md` ô bắt buộc "Khối tóm tắt `run_local_gate.sh`" cho mọi PR sửa `Source/`, `Config/`, `Content/`.
- Không làm: không dựng self-hosted runner (để sau MVP, xem cuối `ROADMAP.md`).
- Nghiệm thu: chạy trên `main` → exit 0 và in khối tóm tắt; chạy trong một worktree tạm tại commit `0067e55` (PR #55, chưa biên dịch được) → exit khác 0 ở bước build. Dán cả hai.

### G1 — Bộ test chức năng vòng lõi (viết đỏ trước)
- Phụ thuộc: G0.
- Đọc trước: `Source/ProjectAscendant/Private/Integration/PAPlayableSliceIntegrationTests.cpp`, `Private/Character/PAStoneGolemEncounterTests.cpp` (cách test hiện có tự đăng ký tag), `Private/Character/PABaseCharacter.cpp`, `Private/Controller/PABasePlayerController.cpp`.
- Làm: `Source/ProjectAscendant/Private/Greybox/PACoreLoopFunctionalTests.cpp`, mỗi kiểm tra là một test riêng, chạy trong world thật có tick (latent command hoặc mở map Outpost):
  - `Greybox.CoreLoop.TagsFromIni`: mọi tag mà combat/ability dùng tra được bằng `RequestGameplayTag(Tag, false)` mà không đăng ký lúc chạy.
  - `Greybox.CoreLoop.PawnVisible`: sprite của Vanguard có flipbook với số keyframe > 0, hoặc có mesh khối tạm hiển thị.
  - `Greybox.CoreLoop.MoveOnInput`: bơm input di chuyển, tick ~30 frame, vị trí đổi theo hướng input.
  - `Greybox.CoreLoop.MeleeHitsDummy`: kích hoạt đòn đánh thường cạnh một mục tiêu, HP mục tiêu giảm.
  - `Greybox.CoreLoop.DashStaminaRegen`: dash 4 lần, chờ, stamina hồi và dash lần 5 được.
  - `Greybox.CoreLoop.DashIFrame`: đòn boss trúng lúc đang trong cửa sổ i-frame của dash không gây sát thương; trúng ngoài cửa sổ thì có.
  - `Greybox.CoreLoop.BossStagger`: đánh trúng boss làm thanh thế đứng (posture) giảm; về 0 thì boss vào trạng thái vỡ thế (Stunned) trong một khoảng thời gian.
  - `Greybox.CoreLoop.BossActs`: Stone Golem tick, đổi trạng thái AI và tung đòn trong N giây khi người chơi đứng gần; HP/thông số boss không lấy từ class người chơi.
  - `Greybox.CoreLoop.DeathRespawn`: HP về 0 → trạng thái Dead → hồi sinh trong T giây với HP đầy.
- Không làm: không sửa code gameplay trong PR này.
- Nghiệm thu: test biên dịch và chạy; PR liệt kê test nào đỏ, kèm dòng log lỗi. Kỳ vọng hiện tại (chưa kiểm chứng lúc chạy, chỉ đọc code): đỏ ở TagsFromIni, PawnVisible, MeleeHitsDummy, DashStaminaRegen, BossStagger, BossActs, DeathRespawn; DashIFrame chưa rõ.

### G2 — Nhân vật hiện hình
- Phụ thuộc: G1.
- Đọc trước: `PABaseCharacter.cpp:173-176` (nạp `FB_Vanguard_Idle` ở đường dẫn có flipbook rỗng); `PAStoneGolemBoss.cpp:88-91` (tương tự với `FB_Golem_Idle`); flipbook có keyframe tại `/Game/art/characters/ClassIdentity/IdleStances/FB_Vanguard_Idle`.
- Làm: trỏ Vanguard và Golem về flipbook có keyframe, hoặc gắn mesh khối tạm (capsule/box có màu) khi flipbook rỗng. Thêm log Warning khi flipbook nạp được nhưng 0 keyframe.
- Không làm: không vẽ hay import art mới.
- Nghiệm thu: `Greybox.CoreLoop.PawnVisible` xanh; ảnh chụp từ `Tools/QA/capture_runtime_art.sh` (nếu chạy được) đính kèm PR.

### G3 — GameplayTag khai báo trong ini
- Phụ thuộc: G1.
- Đọc trước: `Config/DefaultGameplayTags.ini` (hiện không có `Event.Combat.Hitbox.*`, `Ability.Skill.*`); grep `RequestGameplayTag` trong `Source/`.
- Làm: khai báo mọi tag code dùng vào ini, theo DECISIONS §8. Không tạo tag mới ngoài những gì code đang dùng.
- Nghiệm thu: `Greybox.CoreLoop.TagsFromIni` xanh; cổng mặc định vẫn 100%.

### G4 — Hitbox greybox cho đòn cận chiến
- Phụ thuộc: G2, G3.
- Đọc trước: `Private/Combat/PAGameplayAbility_MeleeAttack.cpp` (chờ event hitbox mà chưa nơi nào bắn ra); `Content/Art/Characters/Vanguard/anim/ABP_Vanguard.uasset` là stub rỗng.
- Làm: ability tự mở/đóng cửa sổ hitbox theo thời gian (không phụ thuộc PaperZD notify); hình dạng là box/capsule theo hướng nhìn; cvar `pa.Debug.Hitbox 1` vẽ hitbox và hurtbox bằng `DrawDebug*`. Thông số (kích thước, thời điểm) để trong data asset hoặc config, **không** coi là số chốt.
- Không làm: không viết spec hitbox chính thức (để H1).
- Nghiệm thu: `Greybox.CoreLoop.MeleeHitsDummy` xanh; ảnh chụp hitbox debug.

### G5 — Stamina hồi sau dash
- Phụ thuộc: G1.
- Đọc trước: `Private/Combat/PAGameplayAbility_Dash.cpp`, `PADashEvasionComponent.cpp`, `PAStaminaComponent.cpp`, `AscendantAttributeSet.cpp`, test hiện có `PADashTests.cpp`.
- Làm: sửa để stamina hồi sau khi dash; GE tạo động không dùng tên cố định gây đè nhau.
- Nghiệm thu: `Greybox.CoreLoop.DashStaminaRegen` xanh.

### G6 — Boss tự chạy với thông số riêng
- Phụ thuộc: G2, G3.
- Đọc trước: `PAStoneGolemBoss.cpp:30` (chỉ bật `bCanEverTick`), `PABaseCharacter.cpp:37-38` (lớp cha tắt tick lúc bắt đầu); phần gán chỉ số và ability theo class người chơi trong `PABaseCharacter.cpp`.
- Làm: bật tick cho boss; boss dùng bảng thông số và bộ đòn riêng (tối thiểu: 1 đòn quét, 1 đòn đập có báo hiệu rõ), không nhận ability của class người chơi.
- Không làm: không quyết quan hệ Stone Golem ↔ Ironclad Warlord, không thêm bộ phận boss (N6, 🛑).
- Đọc trước thêm: `design/gdd/stagger-system.md`, `design/gdd/boss-ai.md` (báo hiệu đòn, vỡ thế).
- Làm thêm: thanh thế đứng của boss giảm khi trúng đòn và vỡ thế khi về 0; kiểm lại i-frame của dash với đòn boss.
- Nghiệm thu: `Greybox.CoreLoop.BossActs`, `BossStagger`, `DashIFrame` xanh.

### G7 — Chết và hồi sinh tạm
- Phụ thuộc: G1.
- Đọc trước: `design/gdd/zone-system.md` (chết ở PvE, Ash Remnant; DECISIONS §13 "rơi 100% Tàn Trang"); DECISIONS §13 "Hướng đi" (không dùng Lửa Trại).
- Làm: Dead → hồi sinh tại `PlayerStart` sau T giây, HP/stamina đầy. Ghi rõ trong code và PR: vị trí hồi sinh là **tạm**, chốt ở N4.
- Không làm: không làm Ash Remnant, rơi đồ, Karma.
- Nghiệm thu: `Greybox.CoreLoop.DeathRespawn` xanh.

### G8 — Chạy được client và chế độ chơi thử
- Phụ thuộc: G0.
- Đọc trước: `run_game.sh` (chế độ `client`, `playtest`, `playtest-lag` không gửi `?token=`); `PAGameModeBase.cpp:23-27` (dedicated server luôn bắt token).
- Làm: chế độ dev của `run_game.sh` gửi token dev hợp lệ (`?token=PA-TOKEN-dev-<n>`), hoặc server dev tắt bắt buộc token bằng tham số dòng lệnh rõ ràng. Không nới kiểm tra token ở build phát hành.
- Nghiệm thu: `./run_game.sh playtest` → log server có 2 lần đăng nhập thành công, không có "Connection rejected". Dán log.

### G9 — Debug HUD và arena chơi thử
- Phụ thuộc: G2, G4, G5, G6, G7.
- Làm: trên `L_VerdantFrontier_Outpost`, khu đấu Stone Golem có lối vào rõ; HUD debug (bật bằng cvar) hiện HP, stamina, posture của người chơi và boss, trạng thái AI boss, cửa sổ i-frame khi dash.
- Nghiệm thu: `./run_game.sh` vào được khu đấu; ảnh chụp HUD debug; toàn bộ `Greybox.CoreLoop.*` xanh.

### G10 — Đưa test greybox vào cổng mặc định và mời chủ dự án chơi thử
- Phụ thuộc: G9 (và G8 nếu muốn thử 2 client).
- Làm: đổi tên test `Greybox.CoreLoop.*` → `ProjectAscendant.CoreLoop.*`; cập nhật `production/PLAYTEST_CHECKLIST.md` §0 với lệnh chạy đúng.
- Nghiệm thu: `./Tools/QA/run_headless_tests.sh --ue-only` 100% với số test tăng thêm 9 (bằng số test G1).
- 🛑 Chủ dự án chơi thử theo `PLAYTEST_CHECKLIST.md` §0 và chọn "giữ" hoặc "dừng lại sửa". Agent ghi câu trả lời nguyên văn vào PROGRESS.

## Ghi chú chưa xếp thẻ

- Gamepad: chưa có asset Enhanced Input (`Content/Input/` không tồn tại); bàn phím chạy qua `PlayerTick`. Chỉ làm nếu chủ dự án chơi bằng tay cầm.
- Trục Z bị khóa (`PABaseCharacter.cpp:51`, `bConstrainToPlane`): đúng ý đồ 2.5D hay không thì xác nhận khi chơi thử.
