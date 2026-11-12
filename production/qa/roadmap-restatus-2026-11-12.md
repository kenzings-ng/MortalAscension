# Đối chiếu trạng thái ROADMAP — 2026-11-12

> Bản đề xuất tiếp nối phiên Claude Code đã dừng; chỉ chủ dự án merge sau khi duyệt thay đổi trạng thái/quy trình trong PR roadmap. Không phải báo cáo hoàn thành game.

## Phạm vi và phương pháp

- Mốc trước đối chiếu: `38df548cc4bd8517c54ed2cbfdbbe2fc4ed37301` (nhánh `docs/roadmap-v2`); code main và remote main tại `a28da431fe1e5a4d31e7bb318268e6fbf2879d83`.
- Bao phủ mọi mục `[x]` của Giai đoạn 0–7, kể cả 1.5/2B. N0–N3 giữ nguyên lịch sử phê duyệt/PR trong PROGRESS mục 23–26; không tuyên bố tái nghiệm thu narrative.
- Graph MCP `list_projects` trả danh sách rỗng; không có project/generation để query hoặc check coverage. Đã đọc source/config và kiểm filesystem trực tiếp; không tuyên bố graph bao phủ code.
- `[x]` chỉ giữ cho đầu ra có bằng chứng đúng phạm vi. `[~]` nghĩa là có phần đã làm nhưng chưa đủ nghiệm thu, không có nghĩa tính năng chưa tồn tại. Các mục `[ ]`/`[~]` cũ và điểm dừng của chủ dự án giữ nguyên.
- Bằng chứng mới và giới hạn: [biên bản lệnh](evidence/roadmap-2026-11-12/checks.md). Không chạy generator ghi đè art, không sửa gameplay.

## Bảng cũ / mới

| Mã đối chiếu | Mục trong bản trước | Cũ | Đề xuất | Bằng chứng / phần còn thiếu |
|---|---|---|---|---|
| R57 | LFS (ROADMAP bản `38df548` dòng 57) | `[x]` | `[x]` | `.gitattributes` có đủ pattern; đây là cấu hình tracking, không phải xác nhận toàn bộ lịch sử đã migrate. |
| R58 | Fast gates (ROADMAP bản `38df548` dòng 58) | `[x]` | `[x]` | GDD 114 file/0 lỗi, self-test 37/37, backend unit pass; CI main a28da43 success (run 43118180079). PostgreSQL local exit 3: không có server. |
| R59 | Stone Golem Spine (ROADMAP bản `38df548` dòng 59) | `[x]` | `[~]` | Đường dẫn boss cũ đã lưu legacy; chưa có lần quan sát Walk/Slam và thuộc tính texture trong lượt này. Hình ảnh cần review, không suy ra từ test xanh. |
| R60 | Chặn lệnh Claude (ROADMAP bản `38df548` dòng 60) | `[x]` | `[~]` | JSON settings parse được; chưa chạy phép thử chính sách từ chính Claude Code. JSON hợp lệ không chứng minh lệnh nguy hiểm bị chặn. |
| R61 | Hook pre-push (ROADMAP bản `38df548` dòng 61) | `[x]` | `[x]` | core.hooksPath trỏ Tools/git-hooks; đưa ref main giả qua stdin bị BLOCKED, exit 1; không push thật. |
| R63 | Texture Character Select (ROADMAP bản `38df548` dòng 63) | `[x]` | `[x]` | Đọc PACharacterSelectTypes.cpp và kiểm file: ranger_pixel_spritesheet.uasset cùng arcanist_pixel_spritesheet.uasset đều tồn tại. Không khẳng định UI đã được quan sát. |
| R64 | Bảo vệ main (ROADMAP bản `38df548` dòng 64) | `[x]` | `[x]` | GitHub ruleset 24157209 active, đủ deletion/non_fast_forward/pull_request/required_status_checks; main remote = a28da43. |
| R72 | Walk/Run runtime (ROADMAP bản `38df548` dòng 72) | `[x]` | `[~]` | PAPaperZDAnimInstance có cập nhật tốc độ; FB_Vanguard_Walk.uasset không có ở đường dẫn Art chuẩn. FB_Run nhỏ và không thấy chuỗi KeyFrames chỉ là dấu hiệu, cần G1/G2 đọc bằng engine và quan sát. |
| R73 | Hurt/Stunned/Dead runtime (ROADMAP bản `38df548` dòng 73) | `[x]` | `[~]` | Source OnTick đọc tag trạng thái; chưa chứng minh chuyển animation trong map thật. G1/G2/G6/G7. |
| R74 | Socket tự động (ROADMAP bản `38df548` dòng 74) | `[x]` | `[~]` | PAPaper2DSocketUtility có parser/applier; chưa đọc lại socket từ sprite đã lưu bằng Unreal. Không chạy script ghi asset trong đợt tài liệu này. |
| R75 | Test VanguardRuntimeWiring (ROADMAP bản `38df548` dòng 75) | `[x]` | `[~]` | Log lịch sử Tests/evidence/y8-character-automation.log giữ bằng chứng suite Character; chỉ xác nhận test đã chạy trước đây. Lượt chạy mới crash mã 139 trước discover; chưa xác nhận lại suite hiện tại. |
| R76 | Legacy đợt 3/runtime (ROADMAP bản `38df548` dòng 76) | `[x]` | `[~]` | Asset đã chuyển và source có đường dẫn Art chuẩn; chưa đủ bằng chứng AnimBP/flipbook chạy đúng. Bỏ kết luận Giai đoạn 1 hoàn thành 100%. |
| R84 | 2 client + server (ROADMAP bản `38df548` dòng 84) | `[x]` | `[~]` | Config bật Iris; script test mạng có sẵn nhưng chưa chạy lại thành công ở lượt này. Launcher không truyền token trong khi dedicated server đòi token. G8/S5. |
| R85 | Mixed và tag replication (ROADMAP bản `38df548` dòng 85) | `[x]` | `[~]` | Constructor người chơi dùng Mixed, boss Minimal; chưa có bằng chứng mới về cùng trạng thái trên server và 2 client. Không suy từ enum cấu hình. |
| R86 | Suite Network (ROADMAP bản `38df548` dòng 86) | `[x]` | `[~]` | Có test/source và bằng chứng lịch sử; cần log mới của binary hiện tại để xác nhận số lượng/kết quả, không giữ số 13 như số hiện tại mặc định. |
| R93 | Vanguard Iron Bastion (ROADMAP bản `38df548` dòng 93) | `[x]` | `[x]` | Quyết định canon đã chốt tại DECISIONS §10; không phải nghiệm thu gameplay. |
| R94 | Nguồn art Hybrid (ROADMAP bản `38df548` dòng 94) | `[x]` | `[x]` | Quyết định nguồn art đã chốt tại DECISIONS §10. |
| R95 | Mood board (ROADMAP bản `38df548` dòng 95) | `[x]` | `[x]` | Tài liệu mood board và quyết định phê duyệt lịch sử được giữ; không tự thay lựa chọn chủ dự án. |
| R96 | SPEC-ART (ROADMAP bản `38df548` dòng 96) | `[x]` | `[x]` | Bảng §0 và cập nhật N2 đã có trên main fd29b22; giữ lịch sử quyết định, không coi proxy tuân thủ mọi thông số mới. |
| R97 | Design sheet gate (ROADMAP bản `38df548` dòng 97) | `[x]` | `[x]` | Chạy Scripts/test_vanguard_design_sheet_art_gate.py exit 0. Kiểm icon palette là câu hỏi A0, không được tự quyết. |
| R98 | Duyệt design sheet (ROADMAP bản `38df548` dòng 98) | `[x]` | `[x]` | Giữ phê duyệt lịch sử PR #48; không mở lại duyệt thẩm mỹ. |
| R108 | 16 class tags (ROADMAP bản `38df548` dòng 108) | `[x]` | `[x]` | Đếm đúng 16 dòng Class.Line trong DefaultGameplayTags.ini; validator nhận 16 class/5 line kể cả Apex. |
| R109 | Scroll Data Asset (ROADMAP bản `38df548` dòng 109) | `[x]` | `[~]` | Có ClassPromotionScrollDefinition source; chưa xác nhận asset và luồng gameplay/replication trong game. Giai đoạn 2B chưa nghiệm thu toàn bộ. |
| R110 | Progression hai slot (ROADMAP bản `38df548` dòng 110) | `[x]` | `[~]` | Có PAClassProgressionComponent; chưa chứng minh đủ replication/lưu và nạp trạng thái giữa server và 2 client. |
| R111 | A1 đổi class (ROADMAP bản `38df548` dòng 111) | `[x]` | `[~]` | Có SwapPrimarySecondaryClasses và test đơn tiến trình; chưa có nghiệm thu mạng như DECISIONS §11/§13 yêu cầu. |
| R112 | B1 Action Deck (ROADMAP bản `38df548` dòng 112) | `[x]` | `[~]` | Có kiểm tra weapon compatibility; chưa có bằng chứng slot F và passive hoạt động trọn luồng trong game/mạng. |
| R113 | ServerPromoteClass (ROADMAP bản `38df548` dòng 113) | `[x]` | `[~]` | Có RPC và InventoryScrollAndServerRPC test; gọi hàm trong world đơn không thay thế giao dịch server + 2 client. |
| R114 | Test class progression (ROADMAP bản `38df548` dòng 114) | `[x]` | `[~]` | Source hiện khai báo 8 test (gồm InventoryScrollAndServerRPC), không chỉ 7. Chờ log suite mới; ma trận helper không chứng minh các passive được gắn vào nhân vật thật. |
| R115 | Kinh tế scroll MVP (ROADMAP bản `38df548` dòng 115) | `[x]` | `[~]` | Có hàm tính giá/salvage và test; chưa có bằng chứng shop/buyback/salvage trong vòng chơi thật. |
| R123 | Lower Body Rig (ROADMAP bản `38df548` dòng 123) | `[x]` | `[~]` | Proxy hiện có; gate cũ pass không chứng minh bộ final theo SPEC §0/N2. A1/A3, chờ chủ dự án chơi thử art. |
| R124 | 1H.Blade (ROADMAP bản `38df548` dòng 124) | `[x]` | `[~]` | Proxy hiện có; chưa có nghiệm thu gắn animation/hitbox vào game theo H1/H2/A3. |
| R125 | Vanguard Identity (ROADMAP bản `38df548` dòng 125) | `[x]` | `[~]` | Proxy hiện có; tabard CrimsonSash lệch navy đã chốt, hình có mào còn chờ duyệt. A1/A4. |
| R126 | Art Gate 215 proxy (ROADMAP bản `38df548` dòng 126) | `[x]` | `[x]` | Scripts/test_phase3_art_gate.py exit 0: 215 asset, 0 failure. Chỉ các tiêu chí script cũ, không thay sprite_spec_gate A0 hay duyệt hình. |
| R127 | Import Paper2D (ROADMAP bản `38df548` dòng 127) | `[x]` | `[~]` | File uasset có trên disk; chưa kiểm thuộc tính Nearest/NoMipmaps/PPU và keyframe bằng Unreal trong lượt này. |
| R128 | Socket metadata mới (ROADMAP bản `38df548` dòng 128) | `[x]` | `[~]` | Script tồn tại; chưa có log chạy/import và đọc ngược socket trên asset hiện tại. Không sửa asset để tạo bằng chứng giả. |
| R129 | Sổ nguồn gốc proxy (ROADMAP bản `38df548` dòng 129) | `[x]` | `[x]` | THIRD_PARTY_ASSETS §4 có Pack 08–10. Nhãn APPROVED trong ledger không thay phê duyệt thẩm mỹ runtime còn mở. |
| R149 | Legacy đợt 4 (ROADMAP bản `38df548` dòng 149) | `[x]` | `[x]` | Ba ảnh DELETE đã không còn tại đường cũ; setup_art_gallery.py dòng 214/222/230 trỏ bản legacy. Không xóa thêm asset. |
| R157 | Autotile (ROADMAP bản `38df548` dòng 157) | `[x]` | `[~]` | Có JSON bitmask tile_size=32 và mô tả nguồn; chưa chạy kiểm autotile trong engine. |
| R158 | Sort Along Axis (ROADMAP bản `38df548` dòng 158) | `[x]` | `[x]` | DefaultEngine.ini đặt TranslucentSortPolicy=SortAlongAxis và vector trục. Đây là cấu hình đã có, không phải nghiệm thu toàn map. |
| R159 | World biome PCG (ROADMAP bản `38df548` dòng 159) | `[x]` | `[~]` | Content/Maps/PCG chứa JSON mô tả, chưa có graph uasset tại thư mục này; chưa có log execute PCG trong map. |
| R160 | Citadel spline PCG (ROADMAP bản `38df548` dòng 160) | `[x]` | `[~]` | Có PCG_CitadelRoadSplineGraph.json; chưa chứng minh graph chạy để tạo đường/thành. JSON hợp lệ không đồng nghĩa graph Unreal đã hoạt động. |
| R168 | Backend + PostgreSQL (ROADMAP bản `38df548` dòng 168) | `[x]` | `[~]` | Service/schema hiện có, CI main pass; PostgreSQL local chưa chạy. Chưa đủ nghiệm thu toàn hệ thống tích hợp UE và backend. |
| R169 | JWT end-to-end (ROADMAP bản `38df548` dòng 169) | `[x]` | `[~]` | PAGameModeBase::CheckAuthToken chỉ nhận định dạng, chưa xác minh chữ ký/expiry qua ValidateClientToken. S2. |
| R170 | Chỉ server ghi dữ liệu (ROADMAP bản `38df548` dòng 170) | `[x]` | `[~]` | Subsystem Initialize không có server-only guard; chưa có phép thử client bị từ chối gọi API ghi qua đường thực tế. S1/S3/S4. |
| R171 | Test PostgreSQL thật (ROADMAP bản `38df548` dòng 171) | `[x]` | `[x]` | CI main run 43118180079 success; workflow gates gọi test_backend_postgres với PostgreSQL 16 service. Local exit 3 là chưa chạy, không PASS. Giữ phạm vi kiểm test service, không E2E UE. |
| R172 | Autosave/disconnect (ROADMAP bản `38df548` dòng 172) | `[x]` | `[~]` | TriggerImmediateCharacterSave dùng actor name, level=1, exp=0, class Vanguard cố định; chưa phải dữ liệu nhân vật thật. S3. |
| R181 | Profile Insights (ROADMAP bản `38df548` dòng 181) | `[x]` | `[~]` | Hai trace cục bộ tồn tại, nhưng chưa phân tích kết quả/budget và không có báo cáo profile tái lập trong repo. File trace không chứng minh đạt hiệu năng. |
| R182 | Package Alpha (ROADMAP bản `38df548` dòng 182) | `[x]` | `[~]` | Không có log BuildCookRun mới và artifact phân phối kiểm chứng được trong lượt này. Cook thành công lịch sử không chứng minh package hiện tại chạy được. |
| R183 | License cuối (ROADMAP bản `38df548` dòng 183) | `[x]` | `[~]` | Có sổ license và khai báo AI; còn 7 asset bị từ chối ở đợt thẩm định CC0/art intake (khác bộ 215 proxy của R126) và danh mục thay thế ở A2. Chưa chứng minh toàn bộ nội dung package được rà cuối. |

Tổng 49 mục `[x]` cũ: giữ 17 mục, đề xuất hạ 32 mục về `[~]`. Không đổi các quyết định đã chốt hoặc tự duyệt điểm 🛑.

## Đầu vào có nguy cơ mất

- `git worktree list --porcelain` còn đăng ký `/tmp/mortal-ashen-gate-runtime`, nhưng đường dẫn này không tồn tại. Nhánh `art/ashen-gate-runtime` chỉ ở `9251f82`; không có commit mới chứa phần chưa commit được mô tả trong phiên Claude. Chưa tìm lại được bản nháp, không tự kết luận có thể khôi phục.
- P1 vẫn có nhánh cục bộ `chore/m5-paperzd-integration-research` tại `f8b0b59`; không publish hoặc gộp nội dung nhánh khác trong PR này.

## Kết luận

Có nền code, bộ test, proxy art và tài liệu thiết kế; chưa đủ bằng chứng để gọi vòng lõi hoặc Alpha hoàn tất. Ưu tiên vẫn là G0/G1 để đo trực tiếp khả năng chơi; các trạng thái hạ cần bằng chứng tương ứng trước khi nâng lại. Không sửa code để làm các ô xanh trong đợt này.
