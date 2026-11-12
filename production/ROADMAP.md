# PROJECT ASCENDANT — ROADMAP

> **Trạng thái**: Bản v2 đang đề xuất, chờ chủ dự án duyệt câu chữ và bảng trạng thái cũ/mới. Các quyết định gốc đã duyệt vẫn giữ nguyên. Chỉ chủ dự án được sửa nội dung file này. Ngoại lệ (DECISIONS §13): agent được đổi `[~]` → `[x]` khi có đủ PR đã merge, log test và kết luận reviewer.
> **Nguồn chuẩn đi kèm**: `production/DECISIONS.md`. Mâu thuẫn giữa hai file → dừng và hỏi.
> **Quy trình thực thi**: Claude theo `CLAUDE.md` (Autonomous Mode); Codex, Antigravity và agent khác theo `AGENTS.md`.
> **Bản v2 (2026-11-12)**: thêm thứ tự greybox trước, các mảng G/H/A/L/S và thẻ việc trong `production/roadmap/`; trạng thái Giai đoạn 0–7 đề xuất đánh lại theo kiểm chứng (bảng so sánh: `production/qa/roadmap-restatus-2026-11-12.md`). Mã cũ giữ nguyên.

> **Điều kiện merge bản v2**: chỉ chủ dự án thực hiện merge sau khi duyệt câu chữ, chú giải trạng thái, bảng cũ/mới và ngoại lệ baseline đỏ G1; reviewer PASS hoặc CI xanh không thay phê duyệt này.

## Quy ước trạng thái

- `[x]` Đã xong **và có bằng chứng** (link PR đã merge, commit trên remote, log test).
- `[~]` Có phần đã làm hoặc bằng chứng lịch sử, nhưng **chưa đủ bằng chứng nghiệm thu hiện tại**. Ghi rõ phần đã có, phần thiếu và phép kiểm cần chạy; không coi thiếu log là chưa hề triển khai. Chỉ đổi về `[ ]` khi xác nhận chưa triển khai. Định nghĩa v2 này là đề xuất cần chủ dự án duyệt trong PR.
- `[ ]` Chưa làm.
- ⏸ Tạm hoãn theo quyết định của chủ dự án; không nhận việc.
- 🛑 **Điểm dừng**: agent bắt buộc dừng và hỏi chủ dự án.

Mỗi đầu việc có **mã** (ví dụ `G3`, `N4`). Cách nhận việc, mẫu thẻ và quy tắc tránh hai agent làm trùng: `production/roadmap/README.md`.

## Thứ tự thực hiện

```
Nền đã có: code, test và proxy; trạng thái từng mục bên dưới phân biệt phần đã chứng minh và chưa nghiệm thu.

Bây giờ: G0 🛑 (xử lý startup UE + cổng build) → G1 → G2…G8 theo phụ thuộc từng thẻ → G9 → G10 🛑 chơi thử
         N4 🛑, A0 (playbook/cổng spec), A2 (sổ CC0), S1–S4 có thể chuẩn bị song song; S5 sau G8 + S2 + G9.
         Không sản xuất art mới khi chưa qua điểm duyệt tương ứng.

Sau G10 "giữ": H1 🛑 → H2; A1 (nhân vật mẫu, cần H1/A0) → A3
                A1 → A4 theo lô (N5 hỗ trợ, không chặn art); N5 🛑 → N6 🛑 + H2 → H3
                N4 → L1 → A5; L1 + G10 → L2 → L3 → L4 🛑
                N6 → N9 → N7 → N8 → Giai đoạn 7 (Alpha)
Sau G10 "dừng lại sửa": quay lại mảng G với ghi chú chơi thử; không mở sản xuất nội dung mới.
```

Nguyên tắc: không làm art hàng loạt hay nội dung lớn khi vòng lõi chưa được chủ dự án chơi thử và chọn "giữ" (DECISIONS §13 "Hướng đi và phân công agent").

## Bảng mảng việc

| Mảng | Nội dung | Thẻ việc | Trạng thái |
|---|---|---|---|
| G | Greybox vòng lõi | `production/roadmap/G-greybox.md` | [ ] G0–G10 |
| H | Hitbox bản chính thức | `production/roadmap/H-hitbox.md` | [ ] H1–H3 |
| A | Sản xuất art (chi tiết hóa Giai đoạn 4) | `production/roadmap/A-art.md` | [ ] A0–A5 |
| L | Bối cảnh và màn chơi | `production/roadmap/L-levels.md` | [ ] L1–L4 |
| S | Theo dõi bảo mật, backend, CI | `production/roadmap/S-followups.md` | [ ] S1–S5 |
| N | Nền tảng Narrative | mục Giai đoạn N bên dưới | xem bên dưới |


### Trạng thái từng thẻ

Các ô dưới là nguồn xét phụ thuộc; không dùng ô tổng hợp của mảng để suy ra từng thẻ đã xong. Mọi thay đổi nội dung/quyền cập nhật vẫn theo DECISIONS §13. Với thẻ mới đang `[ ]`, chủ dự án đổi trạng thái theo bằng chứng PR; agent chỉ đề xuất trạng thái trong PR/PROGRESS, không tự đổi `[ ]` → `[x]`. G10 chỉ được đánh `[x]` khi chủ dự án chọn "giữ"; nếu chọn "dừng lại sửa" thì giữ `[~]` và ghi nguyên văn yêu cầu sửa.

**G**

- [ ] G0
- [ ] G1
- [ ] G2
- [ ] G3
- [ ] G4
- [ ] G5
- [ ] G6
- [ ] G7
- [ ] G8
- [ ] G9
- [ ] G10

**H**

- [ ] H1
- [ ] H2
- [ ] H3

**A**

- [ ] A0
- [ ] A1
- [ ] A2
- [ ] A3
- [ ] A4
- [ ] A5

**L**

- [ ] L1
- [ ] L2
- [ ] L3
- [ ] L4

**S**

- [ ] S1
- [ ] S2
- [ ] S3
- [ ] S4
- [ ] S5

## Đầu vào đang chờ chủ dự án

- [ ] **V1** 🛑 Mẫu hình Ashen Gate: worktree `/tmp/mortal-ashen-gate-runtime` không còn tồn tại khi kiểm ngày 2026-11-12; Git còn đăng ký đường dẫn nhưng nhánh chỉ ở `9251f82`. Phần chưa commit chưa tìm lại được. Xem biên bản đối chiếu; nếu khôi phục được phải giữ ngoài main, rà xung đột PR #71 và xin duyệt lô AI trước khi gộp.
- [ ] **U1** 🛑 Spec UX và mockup 6 màn hình (nhánh `design/z1-ux-frontend-hud`, đã có trên remote): login, đăng ký, chọn nhân vật, HUD, shop, lò rèn. Chờ chủ dự án duyệt mockup; HUD debug của G9 không phụ thuộc mục này.
- [ ] **P1** Ghi chú tích hợp PaperZD cho nhân vật mới (nhánh `chore/m5-paperzd-integration-research`, **chỉ có ở máy cục bộ**, cần push). Đầu vào cho H2 và A1.

---

## Giai đoạn 0 — Nền móng repo

**Mục tiêu**: repo an toàn, build và test chạy được bằng lệnh.

- [x] Git LFS (`.gitattributes`) cho ảnh, `.aseprite`, `.atlas`, `.spine`, `.uasset`, `.umap`.
- [x] Cổng tự động: `validate_gdd_consistency.py`, `test_backend_postgres.py` (PostgreSQL thật), CI job `gates` (PR #1, PR #43, PR #45; 114 file validator không lỗi trong lượt đối chiếu 2026-11-12; PostgreSQL test pass trên CI runner main a28da43).
- [~] Nhánh Stone Golem Spine: asset đã có trên main (`Content/art/characters/boss/spine/`). Cần kiểm chứng: Walk không trượt chân, Slam có squash tiếp đất, texture Filter = Nearest, không mipmap. _(Đối chiếu 2026-11-12: R59, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] `.claude/settings.json` đúng cú pháp Claude Code và **đã chứng minh** chặn được lệnh cấm. _(Đối chiếu 2026-11-12: R60, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [x] Hook `pre-push` được cài (`git config core.hooksPath Tools/git-hooks`) và đã chứng minh chặn push vào main.
- [~] Job `ue-tests` chuyển sang `workflow_dispatch`; UE test chạy local, log dán vào `PROGRESS.md`. _(Hạ về `[~]` ngày 2026-10-10 theo DECISIONS §13: chưa có bằng chứng job `ue-tests` chạy trên runner — xem PROGRESS.md:39, Đính chính X14 mục 3; M2 trong `production/qa/review-plan-vs-code-2026-10-09.md`.)_
- [x] Sửa lỗi Character Select: `PACharacterSelectTypes.cpp` load `ranger_pixel_spritesheet` và `arcanist_pixel_spritesheet` nhưng repo chỉ có `.png`, không có `.uasset`.
- [x] Bảo vệ nhánh `main`: ruleset 24157209 `active` (deletion, non_fast_forward, pull_request, required_status_checks "Fast Gates (GDD & Backend QA)") từ 2026-10-09; bằng chứng X3 trong PROGRESS.md. _(Thêm ngày 2026-10-10 theo DECISIONS §13.)_

**Xong khi**: mọi mục trên là `[x]`.

---

## Giai đoạn 1 — Nối hình vào Vanguard (M6.1 Runtime Wiring)

- [~] `Speed` / `bIsRunning` trong `UPAPaperZDAnimInstance`, flipbook Walk/Run 6 frame (PR #1, PR #45; test `ProjectAscendant.Character` PASS 6/6). _(Đối chiếu 2026-11-12: R72, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] State Hurt / Stunned / Dead điều khiển bằng GameplayTag (`State.Hurt`, `State.Stunned`, `State.Dead`) (PR #1, PR #12a; test `ProjectAscendant.Character` PASS 6/6). _(Đối chiếu 2026-11-12: R73, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Tạo Sprite Socket tự động từ `vanguard_metadata.json` (`PAPaper2DSocketUtility`) (PR #1, PR #45; `PAPaper2DSocketUtility.cpp` trên main). _(Đối chiếu 2026-11-12: R74, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Test `ProjectAscendant.Character.VanguardRuntimeWiring` pass (bằng chứng log `Tests/evidence/y8-character-automation.log`: filter `ProjectAscendant.Character` 6/6 PASS gồm `VanguardRuntimeWiring`, `PaperdollModularSystem`, `StoneGolemEncounter`, và 3 test `CharacterVisual`; `QueueFinished=1`). _(Log cũ; lượt 2026-11-12 crash trước discover, xem R75 trong bảng đối chiếu.)_
- [~] Legacy Đợt 3: AnimBP mới chuẩn hóa trong `Content/Art/Characters/`, đổi đường dẫn trong `PABaseCharacter.cpp`, `PAStoneGolemBoss.cpp`, `PACharacterSelectTypes.cpp`, `paper2d_flipbooks_test.cpp`; test pass (`ProjectAscendant.Core.Character.Paper2DFlipbooksIntegration` PASS 1/1, `ProjectAscendant.Character` PASS 6/6); chuyển `Content/art/characters/vanguard/` và `boss/` vào kho legacy `Art_Gallery/legacy/characters/` (theo `legacy-art-inventory.md` mục 5). Bằng chứng: `Tests/evidence/legacy-batch3-character-automation.log`. Giai đoạn 1 chưa được nghiệm thu lại trong map thật. _(Đối chiếu 2026-11-12: R76, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_

**Kiểm chứng**: chạy UE test local, dán log. Code đã có trên main (`PAPaper2DSocketUtility.cpp`).

---

## Giai đoạn 1.5 — Netcode

- [~] PIE 2 client + dedicated server, Iris bật (`net.Iris.UseIrisReplication=1`) (PR #38, PR #39; `test_multi_client_replication.py` PASS). _(Đối chiếu 2026-11-12: R84, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] ASC người chơi ở chế độ Mixed; tag trạng thái replicate đúng (PR #19, PR #38, PR #39; `PAGasReplicationModeTests.cpp`, `PANetworkReplicationTests.cpp` PASS). _(Đối chiếu 2026-11-12: R85, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Test `ProjectAscendant.Network` pass (`PANetworkReplicationTests.cpp`) (13/13 PASS). _(Đối chiếu 2026-11-12: R86, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [ ] 🛑 Chủ dự án tự chơi thử: 2 cửa sổ PIE, `Net PktLag=150`, `Net PktLoss=2`, cùng đánh một quái; Hurt/Dead hiển thị giống nhau ở cả hai bên.

---

## Giai đoạn 2 — Chốt phong cách nhân vật

- [x] Vanguard: phương án A "Iron Bastion", khiên sắt vuông (DECISIONS.md).
- [x] Nguồn art Hybrid (DECISIONS.md).
- [x] Mood board: 🛑 chủ dự án đã chọn 4 game tham chiếu (Octopath Traveler II, Dark Souls / Elden Ring, Children of Morta, Triangle Strategy) ngày 2026-11-10; tài liệu tổng hợp tại `design/art/vanguard-mood-board.md`.
- [x] Cập nhật `SPEC-ART V2` theo mood board (tỷ lệ 3.2–3.5 đầu, pivot 64,114, ramp 4 tông, 5 hướng + lật) trong `design/art/pixel-asset-specifications.md:7,141-177`.
- [x] Design sheet Vanguard 5 hướng, qua `pixel-review` ở 32×32 và 16×16 (`Art_Gallery/vanguard_design_sheet_5dir.png`, `vanguard_design_sheet_32x32.png`, `vanguard_design_sheet_16x16.png`, review tool `Art_Gallery/vanguard_phase2_design_sheet_review.html`, test `Scripts/test_vanguard_design_sheet_art_gate.py` PASS 100%).
- [x] 🛑 Chủ dự án duyệt design sheet (đã duyệt ngày 2026-11-10; PR #48).

**Xong khi**: chủ dự án nói "chốt".

---

## Giai đoạn 2B — Hệ thống Class Chính / Phụ & Quyển trục (code, không cần art)

Làm song song với Giai đoạn 2. Mọi quy tắc lấy từ `DECISIONS.md` và `advanced-classes.md`.

- [x] GameplayTags `Class.Line.<Nhánh>.<Class>` cho 16 class, khai báo trong `DefaultGameplayTags.ini:2-17`.
- [~] `UClassPromotionScrollDefinition` (Data Asset): `TargetClassTag`, `AllowedSourceClassTags`, `RequiredMinRank`, `RequiredSourceClassLevel`, `RequiredQuestFlag`, điều kiện hai slot cho Apex (`ClassPromotionScrollDefinition.h`, `ClassPromotionScrollDefinition.cpp`). _(Đối chiếu 2026-11-12: R109, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Component tiến trình class: slot Chính / Phụ replicated, lưu tiến trình từng class đã chơi, Class Level 1–20 song song Character Level 1–50 (`PAClassProgressionComponent.h`, `PAClassProgressionComponent.cpp`). _(Đối chiếu 2026-11-12: R110, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Quy tắc A1: đổi Chính ↔ Phụ tại Tòa Thành, clamp bậc Phụ theo bậc Chính (`PAClassProgressionComponent::SwapPrimarySecondaryClasses`, `GetEffectiveSecondaryRank`). _(Đối chiếu 2026-11-12: R111, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Quy tắc B1: Class Phụ 1 Active (slot F, không phụ thuộc vũ khí) + 1 Passive; kỹ năng cần vũ khí bị khóa (`IsSecondaryActiveSkillAllowed`). _(Đối chiếu 2026-11-12: R112, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Server RPC thăng chức, kiểm tra cả hai slot, tiêu hủy quyển trục (`ServerPromoteClass`). _(Đối chiếu 2026-11-12: R113, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Test tự động: thăng chức hợp lệ / không hợp lệ, clamp bậc, khóa kỹ năng sai vũ khí, **test ma trận** mọi cặp Chính × Phụ kiểm tra Passive không cộng dồn vượt giới hạn (`PAClassProgressionTests.cpp`: 7/7 PASS, bao gồm ma trận 256 cặp). _(Đối chiếu 2026-11-12: R114, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Quyển trục MVP: bán cho NPC lấy vàng, Chợ Đen bán lại, phân rã thành Tàn Trang (`GetSalvageShardCount`, `GetVendorGoldPrice`, `GetBlackMarketBuybackPrice`). _(Đối chiếu 2026-11-12: R115, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_

---

## Giai đoạn 3 — Bộ art mẫu (vertical slice)

Chỉ làm **một** tổ hợp, chạy trọn pipeline trước khi sản xuất hàng loạt.

- [~] 1 Lower Body Master Rig (visual-004): Idle / Walk / Run / Dash / HitStun, 5 hướng (`Content/art/characters/MasterRigs/FB_Lower_HeavyTank_Set`). _(Đối chiếu 2026-11-12: R123, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] 1 Weapon Family `1H.Blade` (visual-005): combo 3 đòn + thế thủ, 5 hướng (`Content/art/characters/UpperBodyWeapons/FB_Upper_1H_Blade_Combo`). _(Đối chiếu 2026-11-12: R124, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Vanguard Idle + Crest + Tabard (visual-006), 5 hướng (`Content/art/characters/ClassIdentity/`). _(Đối chiếu 2026-11-12: R125, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [x] Qua Art Gate (`pixel-review`), sửa bằng `pixel-fix` (`Scripts/test_phase3_art_gate.py`: 100% PASS trên 215 assets).
- [~] Xuất bằng `pixel-export`, import Paper2D (Nearest, không mipmap, Pixels Per Unit thống nhất), **giữ nguyên tên asset của proxy** để không phải sửa code. _(Đối chiếu 2026-11-12: R127, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Chạy lại script socket với metadata mới (`Tools/Paper2D/apply_vanguard_sockets.py`). _(Đối chiếu 2026-11-12: R128, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [x] Ghi nguồn gốc asset (AI / thủ công / pack) vào `THIRD_PARTY_ASSETS.md` (§4).
- [ ] 🛑 Chủ dự án chơi thử với art thật và duyệt.

---

## Giai đoạn 4 — Nhân bản art & dọn dẹp

Khối lượng theo 5 hướng + lật (story-004 → 007), 16 class:

| Story | Nội dung | Khối lượng |
|---|---|---|
| visual-004 | 4 Lower Body Master Rigs | 420 frame |
| visual-005 | 7 Weapon Families | 560 frame |
| visual-006 | 16 class Idle + Crest + Tabard, cộng cánh Seraph | ~480 + 20 asset |
| visual-007 | Civilian Upper Body & Town Props | 255 asset |

Thẻ việc chi tiết: `production/roadmap/A-art.md` (A0 playbook/cổng spec; A1 Vanguard mẫu; A2 sổ CC0; A3–A5 sản xuất). A0/A2 được chuẩn bị trước G10; sản xuất theo phụ thuộc của từng thẻ và chỉ sau khi G10 chọn "giữ".

- [ ] Sản xuất theo từng lô, mỗi lô qua Art Gate trước khi import.
- [ ] Sổ cái CC0: thẩm định pack Pending, thay 7 asset bị từ chối, rà `doficia/project-cordon-sprites`.
- [x] Legacy Đợt 4: xóa 3 file DELETE_CANDIDATE trùng lặp, sửa `Scripts/setup_art_gallery.py` dòng 214/222/230.
- [ ] Asset legacy `REWORK`: chỉ xóa sau khi bản thay thế qua Art Gate.
- [ ] 🛑 Mọi asset license khác CC0 hoặc sinh bằng AI; mọi khoản mua asset pack.

---

## Giai đoạn 5 — Dựng thế giới (PCG)

- [~] Tileset mặt đất 32×32 có autotile bitmask (`pixel-tileset`). _(Đối chiếu 2026-11-12: R157, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [x] Translucent Sort Policy = Sort Along Axis cho sắp xếp độ sâu isometric.
- [~] Nạp asset môi trường M6 vào `PCG_WorldBiomeGraph`. _(Đối chiếu 2026-11-12: R159, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Đường và thành bằng `PCG_CitadelRoadSplineGraph`. _(Đối chiếu 2026-11-12: R160, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [ ] WFC ngục tối: để backlog, chỉ làm khi mọi mục trên xong.
- [ ] 🛑 Chủ dự án đi dạo map và nhận xét.

---

## Giai đoạn 6 — Backend & lưu dữ liệu

- [~] Service backend (công nghệ **chưa chốt**, quyết qua ADR-0006 — DECISIONS §13) + PostgreSQL theo schema trong `DECISIONS.md` mục 9 (bảng `accounts`, `characters`, `items`). _(Đối chiếu 2026-11-12: R168, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Client đăng nhập lấy JWT; dedicated server xác thực token. _(Đối chiếu 2026-11-12: R169, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Chỉ dedicated server gọi API ghi dữ liệu (`FHttpModule`); client không bao giờ ghi trực tiếp. _(Đối chiếu 2026-11-12: R170, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [x] Test backend gọi vào **code service backend thật** (công nghệ chưa chốt, ADR-0006) (không chỉ câu SQL trong file test): chống dupe, hoán đổi ô đồ, thăng chức bằng quyển trục.
- [~] Lưu khi thoát và lưu định kỳ. _(Đối chiếu 2026-11-12: R172, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_

Việc theo dõi bảo mật và backend sau PR #71: `production/roadmap/S-followups.md` (S1–S5).

---

## Giai đoạn 7 — Alpha

- [ ] Playtest với người ngoài. 🛑
- [~] Profile hiệu năng bằng Unreal Insights (Trace benchmark Outpost map & Combat trong `Saved/Profiling/Alpha_Benchmark_L_VerdantFrontier_Outpost.utrace`). _(Đối chiếu 2026-11-12: R181, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Đóng gói bản build (Nội dung và maps đã qua kiểm chứng Cook 100% PASS bằng RunUAT BuildCookRun, 0 lỗi). _(Đối chiếu 2026-11-12: R182, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_
- [~] Kiểm tra license lần cuối; khai báo nội dung AI theo yêu cầu nền tảng phát hành (`THIRD_PARTY_ASSETS.md` §1-5). _(Đối chiếu 2026-11-12: R183, xem [bảng cũ/mới](qa/roadmap-restatus-2026-11-12.md).)_

---

## Giai đoạn N — Nền tảng Narrative (thế giới, phe phái, thiết kế nhân vật)

_Thêm ngày 2026-11-11 theo quyết định của chủ dự án (DECISIONS §13, bổ sung 2026-11-11). Chạy song song Giai đoạn 4, không chặn sản xuất art. Bản v2 (2026-11-12): thứ tự mới theo mục "Thứ tự thực hiện" ở đầu file — N5/N6 chờ G10._

**Mục tiêu**: một nguồn canon duy nhất cho thế giới và nhân vật trong `design/narrative/`; mọi GDD và registry trỏ về đó; validator chặn tên lệch tái phát.

**Tiền đề đã chốt**: thế giới fantasy có thật; Hệ Thống (cấp độ, class, kỹ năng) là quy luật mà cư dân nhìn thấy.

**Cách tạo canon**: agent đề xuất 2–3 phương án trên một trang xem trước, chủ dự án chọn. Không tên thế giới, vùng, thành, địa điểm, phe phái, NPC, boss hay quái mới nào được ghi vào repo trước khi chủ dự án chọn; phương án chưa được chọn chỉ nằm trên trang xem trước.

- [x] N0 — Hạ tầng: khung `design/narrative/`, validator quét `design/narrative/**`, quy ước `kind:` cho vùng, thành, địa điểm, phe phái, NPC, boss, quái trong `entities.yaml` (PR #63, commit `9fabc2e` trên main; validator 112 file 0 lỗi, self-test 26/26, CI "Fast Gates" pass; reviewer đồng ý ở vòng 3 tại `cec80e2`, các commit sau đó chỉ sửa nhật ký — PROGRESS.md mục 23).
- [x] N1 — Thống nhất tên gọi hiện có theo DECISIONS §2/§7: tên tiếng Việt của 16 class, bổ sung 4 class thiếu trong registry, tên game "Mortal Ascension" (theo `README.md`), check `class_vn_name` trong validator. 🛑 Xác nhận trước khi sửa: áp §2 nguyên văn, "Du Hiệp" là tên nhánh Scout và Ranger là "Xạ Thủ" (GDD đang gọi Ranger là "Du Hiệp"). _(Xong 2026-11-11: PR #65, commit `3c8efde` trên main; validator 112 file 0 lỗi 1 allowlist, self-test 37/37, CI "Fast Gates" pass; reviewer đồng ý ở vòng 4; chủ dự án xác nhận áp §2 và duyệt mục "Ngôn ngữ tên gọi" — PROGRESS.md mục 24.)_
- [x] N2 — Hòa giải thông số sprite về một bảng duy nhất trong `pixel-asset-specifications.md`; retrofit `art-bible.md` §5–§6. 🛑 Các điểm chưa có nguồn chốt: số lớp paperdoll, PPU, ngân sách màu, khiên và tabard Vanguard, tư thế 90° / 50-50. _(Xong 2026-11-11: PR #67, commit `fd29b22` trên main; validator 112 file 0 lỗi 1 allowlist, self-test 37/37, CI "Fast Gates" pass; reviewer đồng ý ở vòng 3 tại `b7d2242`, commit sau đó chỉ sửa nhật ký; các điểm 🛑 của dòng này do chủ dự án chọn qua hộp chọn, riêng khiên vuông theo DECISIONS §7 đã chốt từ trước — PROGRESS.md mục 25.)_
- [x] N3 — World bible và tone guide (`design/narrative/world/`). 🛑 Chủ dự án chọn trên trang đề xuất. _(Xong 2026-11-11: PR #69, commit `75cdfda` trên main; validator 114 file 0 lỗi 1 allowlist, self-test 37/37, CI "Fast Gates" pass; reviewer đồng ý ở vòng 3 tại `28f777a`, commit sau đó chỉ sửa nhật ký; hướng thế giới và các điểm 🛑 do chủ dự án chọn qua hai lượt hộp chọn trong terminal (bản đề xuất nằm ngoài repo), phần thân tài liệu do agent viết trong khuôn khổ đó — PROGRESS.md mục 26.)_
- [ ] N4 — Địa lý và phe phái: 3 vùng, 3 thành, địa điểm mở khóa class, phe phái; chốt vị trí lò rèn, Chợ Đen và **vị trí hồi sinh** (không dùng Lửa Trại — DECISIONS §13 "Hướng đi và phân công agent"); tên thế giới sau khi kiểm trùng tên. 🛑 Chạy song song mảng G.
- [ ] N5 — Character sheet 16 class, 5 lô theo nhánh (`design/narrative/characters/`). 🛑 Mỗi lô; giới tính / kiểu thân nhân vật. ⏸ _(Tạm hoãn tới khi chủ dự án chơi thử greybox ở G10.)_
- [ ] N6 — Boss, bestiary, NPC có tên. 🛑 Quan hệ Stone Golem ↔ Ironclad Warlord và danh sách bộ phận boss (chạm DECISIONS §10, §13). ⏸ _(Tạm hoãn tới khi chủ dự án chơi thử greybox ở G10.)_
- [ ] N7 — Đồng bộ chuỗi hiển thị trong code với canon (chỉ text; không sửa logic).
- [ ] N8 — Rà soát chéo cuối: `/review-all-gdds`, `/consistency-check full`, cổng `nd-consistency` và `cd-narrative`.
- [ ] N9 — Hệ thống kể chuyện tối thiểu: lore trên vật phẩm (Boss Soul, Sách Kỹ Năng, Thần Binh…) và NPC ở mỗi Tòa Thành có vài dòng thoại ngắn, không phân nhánh. Viết GDD mới trước khi làm. _(Thêm theo lựa chọn "Lore vật phẩm + NPC có thoại ngắn" của chủ dự án.)_

**Thứ tự**: N0 → (N1 song song N2) → N3 → N4 (song song mảng G) → **G10 chơi thử** → N5 → N6 → N9 → N7 → N8.

**Xong khi**: mọi mục trên là `[x]`.

---

## Sau MVP (không làm trước khi Alpha xong)

- Giao dịch giữa người chơi / Nhà đấu giá; quyển trục class ẩn khóa sau lần giao dịch đầu; chống bot (cấp tối thiểu, thuế giao dịch).
- Self-hosted runner cho UE test (chỉ khi repo private hoặc đã chặn PR từ fork).
