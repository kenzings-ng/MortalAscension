# PROJECT ASCENDANT — ROADMAP

> **Trạng thái**: Đã duyệt bởi chủ dự án. Chỉ chủ dự án được sửa nội dung file này. Ngoại lệ (DECISIONS §13): agent được đổi `[~]` → `[x]` khi có đủ PR đã merge, log test và kết luận reviewer.
> **Nguồn chuẩn đi kèm**: `production/DECISIONS.md`. Mâu thuẫn giữa hai file → dừng và hỏi.
> **Quy trình thực thi**: theo `CLAUDE.md` (Autonomous Mode).

## Quy ước trạng thái

- `[x]` Đã xong **và có bằng chứng** (link PR đã merge, commit trên remote, log test).
- `[~]` Được báo là xong nhưng **chưa có bằng chứng**. Agent phải kiểm chứng trước, ghi bằng chứng vào `PROGRESS.md`, rồi mới đổi thành `[x]`. Không kiểm chứng được → đổi về `[ ]`.
- `[ ]` Chưa làm.
- 🛑 **Điểm dừng**: agent bắt buộc dừng và hỏi chủ dự án.

## Thứ tự thực hiện

```
0 → 1 → 1.5 → [2 (chờ duyệt thẩm mỹ) song song 2B] → 3 → 4 → 5 → 6 → 7
                                                         N (song song Giai đoạn 4)
```

Giai đoạn 2B không cần duyệt thẩm mỹ, nên agent làm 2B trong lúc chờ chủ dự án duyệt ở Giai đoạn 2.

Giai đoạn N (Nền tảng Narrative) chạy song song Giai đoạn 4 và không chặn sản xuất art (DECISIONS §13, bổ sung 2026-11-11).

---

## Giai đoạn 0 — Nền móng repo

**Mục tiêu**: repo an toàn, build và test chạy được bằng lệnh.

- [x] Git LFS (`.gitattributes`) cho ảnh, `.aseprite`, `.atlas`, `.spine`, `.uasset`, `.umap`.
- [x] Cổng tự động: `validate_gdd_consistency.py`, `test_backend_postgres.py` (PostgreSQL thật), CI job `gates` (PR #1, PR #43, PR #45; 108/108 validator pass, PostgreSQL test pass trên CI runner).
- [x] Nhánh Stone Golem Spine: asset đã có trên main (`Content/art/characters/boss/spine/`). Cần kiểm chứng: Walk không trượt chân, Slam có squash tiếp đất, texture Filter = Nearest, không mipmap.
- [x] `.claude/settings.json` đúng cú pháp Claude Code và **đã chứng minh** chặn được lệnh cấm.
- [x] Hook `pre-push` được cài (`git config core.hooksPath Tools/git-hooks`) và đã chứng minh chặn push vào main.
- [~] Job `ue-tests` chuyển sang `workflow_dispatch`; UE test chạy local, log dán vào `PROGRESS.md`. _(Hạ về `[~]` ngày 2026-10-10 theo DECISIONS §13: chưa có bằng chứng job `ue-tests` chạy trên runner — xem PROGRESS.md:39, Đính chính X14 mục 3; M2 trong `production/qa/review-plan-vs-code-2026-10-09.md`.)_
- [x] Sửa lỗi Character Select: `PACharacterSelectTypes.cpp` load `ranger_pixel_spritesheet` và `arcanist_pixel_spritesheet` nhưng repo chỉ có `.png`, không có `.uasset`.
- [x] Bảo vệ nhánh `main`: ruleset 24157209 `active` (deletion, non_fast_forward, pull_request, required_status_checks "Fast Gates (GDD & Backend QA)") từ 2026-10-09; bằng chứng X3 trong PROGRESS.md. _(Thêm ngày 2026-10-10 theo DECISIONS §13.)_

**Xong khi**: mọi mục trên là `[x]`.

---

## Giai đoạn 1 — Nối hình vào Vanguard (M6.1 Runtime Wiring)

- [x] `Speed` / `bIsRunning` trong `UPAPaperZDAnimInstance`, flipbook Walk/Run 6 frame (PR #1, PR #45; test `ProjectAscendant.Character` PASS 6/6).
- [x] State Hurt / Stunned / Dead điều khiển bằng GameplayTag (`State.Hurt`, `State.Stunned`, `State.Dead`) (PR #1, PR #12a; test `ProjectAscendant.Character` PASS 6/6).
- [x] Tạo Sprite Socket tự động từ `vanguard_metadata.json` (`PAPaper2DSocketUtility`) (PR #1, PR #45; `PAPaper2DSocketUtility.cpp` trên main).
- [x] Test `ProjectAscendant.Character.VanguardRuntimeWiring` pass (bằng chứng log `Tests/evidence/y8-character-automation.log`: filter `ProjectAscendant.Character` 6/6 PASS gồm `VanguardRuntimeWiring`, `PaperdollModularSystem`, `StoneGolemEncounter`, và 3 test `CharacterVisual`; `QueueFinished=1`).
- [x] Legacy Đợt 3: AnimBP mới chuẩn hóa trong `Content/Art/Characters/`, đổi đường dẫn trong `PABaseCharacter.cpp`, `PAStoneGolemBoss.cpp`, `PACharacterSelectTypes.cpp`, `paper2d_flipbooks_test.cpp`; test pass (`ProjectAscendant.Core.Character.Paper2DFlipbooksIntegration` PASS 1/1, `ProjectAscendant.Character` PASS 6/6); chuyển `Content/art/characters/vanguard/` và `boss/` vào kho legacy `Art_Gallery/legacy/characters/` (theo `legacy-art-inventory.md` mục 5). Bằng chứng: `Tests/evidence/legacy-batch3-character-automation.log`. Giai đoạn 1 hoàn thành 100%.

**Kiểm chứng**: chạy UE test local, dán log. Code đã có trên main (`PAPaper2DSocketUtility.cpp`).

---

## Giai đoạn 1.5 — Netcode

- [x] PIE 2 client + dedicated server, Iris bật (`net.Iris.UseIrisReplication=1`) (PR #38, PR #39; `test_multi_client_replication.py` PASS).
- [x] ASC người chơi ở chế độ Mixed; tag trạng thái replicate đúng (PR #19, PR #38, PR #39; `PAGasReplicationModeTests.cpp`, `PANetworkReplicationTests.cpp` PASS).
- [x] Test `ProjectAscendant.Network` pass (`PANetworkReplicationTests.cpp`) (13/13 PASS).
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
- [x] `UClassPromotionScrollDefinition` (Data Asset): `TargetClassTag`, `AllowedSourceClassTags`, `RequiredMinRank`, `RequiredSourceClassLevel`, `RequiredQuestFlag`, điều kiện hai slot cho Apex (`ClassPromotionScrollDefinition.h`, `ClassPromotionScrollDefinition.cpp`).
- [x] Component tiến trình class: slot Chính / Phụ replicated, lưu tiến trình từng class đã chơi, Class Level 1–20 song song Character Level 1–50 (`PAClassProgressionComponent.h`, `PAClassProgressionComponent.cpp`).
- [x] Quy tắc A1: đổi Chính ↔ Phụ tại Tòa Thành, clamp bậc Phụ theo bậc Chính (`PAClassProgressionComponent::SwapPrimarySecondaryClasses`, `GetEffectiveSecondaryRank`).
- [x] Quy tắc B1: Class Phụ 1 Active (slot F, không phụ thuộc vũ khí) + 1 Passive; kỹ năng cần vũ khí bị khóa (`IsSecondaryActiveSkillAllowed`).
- [x] Server RPC thăng chức, kiểm tra cả hai slot, tiêu hủy quyển trục (`ServerPromoteClass`).
- [x] Test tự động: thăng chức hợp lệ / không hợp lệ, clamp bậc, khóa kỹ năng sai vũ khí, **test ma trận** mọi cặp Chính × Phụ kiểm tra Passive không cộng dồn vượt giới hạn (`PAClassProgressionTests.cpp`: 7/7 PASS, bao gồm ma trận 256 cặp).
- [x] Quyển trục MVP: bán cho NPC lấy vàng, Chợ Đen bán lại, phân rã thành Tàn Trang (`GetSalvageShardCount`, `GetVendorGoldPrice`, `GetBlackMarketBuybackPrice`).

---

## Giai đoạn 3 — Bộ art mẫu (vertical slice)

Chỉ làm **một** tổ hợp, chạy trọn pipeline trước khi sản xuất hàng loạt.

- [x] 1 Lower Body Master Rig (visual-004): Idle / Walk / Run / Dash / HitStun, 5 hướng (`Content/art/characters/MasterRigs/FB_Lower_HeavyTank_Set`).
- [x] 1 Weapon Family `1H.Blade` (visual-005): combo 3 đòn + thế thủ, 5 hướng (`Content/art/characters/UpperBodyWeapons/FB_Upper_1H_Blade_Combo`).
- [x] Vanguard Idle + Crest + Tabard (visual-006), 5 hướng (`Content/art/characters/ClassIdentity/`).
- [x] Qua Art Gate (`pixel-review`), sửa bằng `pixel-fix` (`Scripts/test_phase3_art_gate.py`: 100% PASS trên 215 assets).
- [x] Xuất bằng `pixel-export`, import Paper2D (Nearest, không mipmap, Pixels Per Unit thống nhất), **giữ nguyên tên asset của proxy** để không phải sửa code.
- [x] Chạy lại script socket với metadata mới (`Tools/Paper2D/apply_vanguard_sockets.py`).
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

- [ ] Sản xuất theo từng lô, mỗi lô qua Art Gate trước khi import.
- [ ] Sổ cái CC0: thẩm định pack Pending, thay 7 asset bị từ chối, rà `doficia/project-cordon-sprites`.
- [x] Legacy Đợt 4: xóa 3 file DELETE_CANDIDATE trùng lặp, sửa `Scripts/setup_art_gallery.py` dòng 214/222/230.
- [ ] Asset legacy `REWORK`: chỉ xóa sau khi bản thay thế qua Art Gate.
- [ ] 🛑 Mọi asset license khác CC0 hoặc sinh bằng AI; mọi khoản mua asset pack.

---

## Giai đoạn 5 — Dựng thế giới (PCG)

- [x] Tileset mặt đất 32×32 có autotile bitmask (`pixel-tileset`).
- [x] Translucent Sort Policy = Sort Along Axis cho sắp xếp độ sâu isometric.
- [x] Nạp asset môi trường M6 vào `PCG_WorldBiomeGraph`.
- [x] Đường và thành bằng `PCG_CitadelRoadSplineGraph`.
- [ ] WFC ngục tối: để backlog, chỉ làm khi mọi mục trên xong.
- [ ] 🛑 Chủ dự án đi dạo map và nhận xét.

---

## Giai đoạn 6 — Backend & lưu dữ liệu

- [x] Service backend (công nghệ **chưa chốt**, quyết qua ADR-0006 — DECISIONS §13) + PostgreSQL theo schema trong `DECISIONS.md` mục 9 (bảng `accounts`, `characters`, `items`).
- [x] Client đăng nhập lấy JWT; dedicated server xác thực token.
- [x] Chỉ dedicated server gọi API ghi dữ liệu (`FHttpModule`); client không bao giờ ghi trực tiếp.
- [x] Test backend gọi vào **code service backend thật** (công nghệ chưa chốt, ADR-0006) (không chỉ câu SQL trong file test): chống dupe, hoán đổi ô đồ, thăng chức bằng quyển trục.
- [x] Lưu khi thoát và lưu định kỳ.

---

## Giai đoạn 7 — Alpha

- [ ] Playtest với người ngoài. 🛑
- [x] Profile hiệu năng bằng Unreal Insights (Trace benchmark Outpost map & Combat trong `Saved/Profiling/Alpha_Benchmark_L_VerdantFrontier_Outpost.utrace`).
- [x] Đóng gói bản build (Nội dung và maps đã qua kiểm chứng Cook 100% PASS bằng RunUAT BuildCookRun, 0 lỗi).
- [x] Kiểm tra license lần cuối; khai báo nội dung AI theo yêu cầu nền tảng phát hành (`THIRD_PARTY_ASSETS.md` §1-5).

---

## Giai đoạn N — Nền tảng Narrative (thế giới, phe phái, thiết kế nhân vật)

_Thêm ngày 2026-11-11 theo quyết định của chủ dự án (DECISIONS §13, bổ sung 2026-11-11). Chạy song song Giai đoạn 4, không chặn sản xuất art._

**Mục tiêu**: một nguồn canon duy nhất cho thế giới và nhân vật trong `design/narrative/`; mọi GDD và registry trỏ về đó; validator chặn tên lệch tái phát.

**Tiền đề đã chốt**: thế giới fantasy có thật; Hệ Thống (cấp độ, class, kỹ năng) là quy luật mà cư dân nhìn thấy.

**Cách tạo canon**: agent đề xuất 2–3 phương án trên một trang xem trước, chủ dự án chọn. Không tên thế giới, vùng, thành, địa điểm, phe phái, NPC, boss hay quái mới nào được ghi vào repo trước khi chủ dự án chọn; phương án chưa được chọn chỉ nằm trên trang xem trước.

- [x] N0 — Hạ tầng: khung `design/narrative/`, validator quét `design/narrative/**`, quy ước `kind:` cho vùng, thành, địa điểm, phe phái, NPC, boss, quái trong `entities.yaml` (PR #63, commit `9fabc2e` trên main; validator 112 file 0 lỗi, self-test 26/26, CI "Fast Gates" pass; reviewer đồng ý ở vòng 3 tại `cec80e2`, các commit sau đó chỉ sửa nhật ký — PROGRESS.md mục 23).
- [ ] N1 — Thống nhất tên gọi hiện có theo DECISIONS §2/§7: tên tiếng Việt của 16 class, bổ sung 4 class thiếu trong registry, tên game "Mortal Ascension" (theo `README.md`), check `class_vn_name` trong validator. 🛑 Xác nhận trước khi sửa: áp §2 nguyên văn, "Du Hiệp" là tên nhánh Scout và Ranger là "Xạ Thủ" (GDD đang gọi Ranger là "Du Hiệp").
- [ ] N2 — Hòa giải thông số sprite về một bảng duy nhất trong `pixel-asset-specifications.md`; retrofit `art-bible.md` §5–§6. 🛑 Các điểm chưa có nguồn chốt: số lớp paperdoll, PPU, ngân sách màu, khiên và tabard Vanguard, tư thế 90° / 50-50.
- [ ] N3 — World bible và tone guide (`design/narrative/world/`). 🛑 Chủ dự án chọn trên trang đề xuất.
- [ ] N4 — Địa lý và phe phái: 3 vùng, 3 thành, địa điểm mở khóa class, phe phái; chốt vị trí lò rèn, Chợ Đen, Lửa Trại. 🛑
- [ ] N5 — Character sheet 16 class, 5 lô theo nhánh (`design/narrative/characters/`). 🛑 Mỗi lô; giới tính / kiểu thân nhân vật.
- [ ] N6 — Boss, bestiary, NPC có tên. 🛑 Quan hệ Stone Golem ↔ Ironclad Warlord và danh sách bộ phận boss (chạm DECISIONS §10, §13).
- [ ] N7 — Đồng bộ chuỗi hiển thị trong code với canon (chỉ text; không sửa logic).
- [ ] N8 — Rà soát chéo cuối: `/review-all-gdds`, `/consistency-check full`, cổng `nd-consistency` và `cd-narrative`.

**Thứ tự**: N0 → (N1 song song N2) → N3 → N4 → N5 → N6 → N7 → N8.

**Xong khi**: mọi mục trên là `[x]`.

---

## Sau MVP (không làm trước khi Alpha xong)

- Giao dịch giữa người chơi / Nhà đấu giá; quyển trục class ẩn khóa sau lần giao dịch đầu; chống bot (cấp tối thiểu, thuế giao dịch).
- Self-hosted runner cho UE test (chỉ khi repo private hoặc đã chặn PR từ fork).
