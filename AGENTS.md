# AGENTS.md — Quy tắc cho agent lập trình (Codex, Antigravity, các agent khác)

> Mortal Ascension (định danh kỹ thuật `ProjectAscendant`): action RPG isometric 2.5D pixel art, Unreal Engine 5.8.2, C++ + GAS + PaperZD.
> File này là bản rút gọn cho agent không đọc được `CLAUDE.md`. Khi hai file khác nhau, `CLAUDE.md` và `production/DECISIONS.md` thắng; báo chỗ lệch trong PR.

## 1. Đọc theo thứ tự này trước khi làm

1. `production/DECISIONS.md`: quyết định đã chốt. Thắng mọi file khác.
2. `production/ROADMAP.md`: trạng thái và thứ tự đầu việc.
3. `production/roadmap/README.md`: mẫu thẻ và **quy tắc nhận việc**; sau đó file thẻ của mảng mình làm (`production/roadmap/*.md`).
4. Mọi file ở dòng "Đọc trước" của thẻ.
5. Làm art: `design/art/art-production-playbook.md` (nếu đã có trên `main`), `design/art/pixel-asset-specifications.md` §0.
6. Viết nội dung game (tên, lore, thoại): `design/narrative/world/world-bible.md`, `tone-guide.md`, `design/registry/entities.yaml`.

`CLAUDE.md` dùng cú pháp `@file` để nhúng file khác; agent khác phải tự mở các file đó: `.claude/docs/coding-standards.md`, `.claude/docs/technical-preferences.md`, `.claude/docs/coordination-rules.md`.

**Lưu ý**: thư mục `.claude/` bị `.gitignore` bỏ qua (chỉ `.claude/agents/reviewer.md` và `.claude/settings.json` được commit). Các file `.claude/docs/*` và `.claude/skills/*` chỉ có ở bản checkout chính `/mnt/Data/Projects/project-games/ProjectAscendant`; nếu làm trong worktree hoặc clone khác, đọc chúng từ đường dẫn đó. Không thấy file → ghi vào PR, không đoán nội dung.

## 2. Quy trình mỗi đầu việc

1. Chọn thẻ theo quy tắc nhận việc (`production/roadmap/README.md`). Một PR = một thẻ.
2. Tạo nhánh từ `main` mới nhất: `<loại>/<mã-thẻ>-<mô-tả>`. Lần đầu trên máy: `git config core.hooksPath Tools/git-hooks`.
3. Đọc file gốc trước khi sửa. Không refactor ngoài phạm vi thẻ.
4. Chạy cổng tự động (mục 3) **trước và sau** khi sửa; dán kết quả vào PR.
5. Mở PR vào `main`, tiêu đề `[<MÃ>] ...`. **Không tự merge.** Một phiên Claude (subagent `reviewer`) rà PR, tìm lỗi kèm file:dòng; sửa hết rồi chạy lại cổng. Chỉ merge khi mọi cổng pass và reviewer đồng ý.
6. Sau merge: thêm mục vào `production/PROGRESS.md` (việc đã làm, commit hash trên remote, log test, kết luận reviewer).

## 3. Cổng tự động

| Khi sửa | Lệnh | Đạt khi |
|---|---|---|
| `Source/`, `Config/`, `Content/` | `/mnt/Data/Engine/Build/BatchFiles/Linux/Build.sh ProjectAscendantEditor Linux Development -Project="$PWD/ProjectAscendant.uproject" -WaitMutex` | build thành công |
| `Source/`, `Config/`, `Content/` | `./Tools/QA/run_headless_tests.sh --ue-only` | `Failed=0`, `QueueFinished=1` |
| Mảng G (greybox) | `./Tools/QA/run_headless_tests.sh --ue-only --filter=Greybox.CoreLoop` | test của thẻ chuyển đỏ → xanh |
| `design/`, `production/` | `python3 Tools/QA/validate_gdd_consistency.py` | exit 0, 0 lỗi |
| `Tools/QA/validate_gdd_consistency.py` | `python3 Tools/QA/test_validate_gdd_consistency.py` | exit 0 |
| `Services/Backend/` | `python3 Tools/QA/test_backend_service_unit.py`; `python3 Tools/QA/test_backend_postgres.py` | exit 0 |
| Art | lệnh kiểm trong playbook art | 100% pass |

CI trên GitHub chỉ chạy "Fast Gates (GDD & Backend QA)", **không biên dịch C++**. PR sửa `Source/` phải dán log build và tóm tắt test UE; reviewer build lại để đối chiếu (thẻ G0 sẽ thêm `Tools/QA/run_local_gate.sh`).

## 4. Bắt buộc dừng và hỏi chủ dự án (ghi câu hỏi vào PR, không tự quyết)

- Bất kỳ dòng nào có 🛑 trong ROADMAP hoặc thẻ.
- Duyệt thẩm mỹ (art nhân vật, cảm giác chơi).
- Tiêu tiền: asset pack, dịch vụ trả phí, API sinh ảnh trả phí.
- Thêm asset không phải CC0, hoặc asset sinh bằng AI. Asset AI duyệt theo lô; mỗi lô dừng chờ duyệt.
- Xóa asset ngoài danh sách DELETE đã duyệt; đổi một quyết định đã chốt.
- Mâu thuẫn chạm tới `DECISIONS.md` hoặc `ROADMAP.md`.
- Thử 3 lần vẫn không qua một cổng.

## 5. Cấm

- Push thẳng vào `main`, force push, viết lại lịch sử git, xóa nhánh của người khác.
- Sửa `production/DECISIONS.md`, `production/ROADMAP.md`, `production/roadmap/*.md`, `CLAUDE.md`, `AGENTS.md`. Đề xuất thay đổi bằng câu hỏi trong PR.
- Báo "đã xong" khi chưa có commit hash trên remote và log test.
- Tự tạo thuật ngữ, tiền tệ, thang độ hiếm, hệ thống, tên riêng (vùng, phe phái, NPC, boss) không có trong GDD/DECISIONS/registry.
- Commit secret, token, email hoặc thông tin cá nhân.
- Sửa test cho pass mà không sửa nguyên nhân; tự đăng ký GameplayTag lúc chạy trong test để che tag thiếu trong `Config/DefaultGameplayTags.ini`.

## 6. Lưu ý kỹ thuật

- Đường dẫn engine: `/mnt/Data/Engine`. Map chơi thử: `Content/Maps/L_VerdantFrontier_Outpost.umap`. Chạy game: `./run_game.sh` (xem `production/PLAYTEST_CHECKLIST.md`).
- Dynamic delegate không dùng `CreateLambda`; dùng `BindUFunction` + `GET_FUNCTION_NAME_CHECKED`.
- Class có `UPROPERTY(Config)` phải khai báo `UCLASS(Config = Game)`.
- Tài liệu viết tiếng Việt; tên riêng canon bằng tiếng Anh (DECISIONS §13 "Ngôn ngữ tên gọi").
- File lớn (ảnh, `.uasset`, `.umap`) đi qua Git LFS; kiểm bằng `python3 Tools/QA/verify_git_lfs_pointers.py`.
