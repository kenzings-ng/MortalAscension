# Biên bản kiểm chứng roadmap — 2026-11-12

Mốc đầu vào: `38df548cc4bd8517c54ed2cbfdbbe2fc4ed37301`; code main `a28da431fe1e5a4d31e7bb318268e6fbf2879d83`. Đợt này chỉ sửa tài liệu. Không build lại C++ (không sửa Source/Config/Content); lần UE dùng binary sẵn có, không coi crash là bằng chứng đã tìm được lỗi source.

| Lệnh / nguồn | Kết quả thực tế | Giới hạn |
|---|---|---|
| `python3 Tools/QA/validate_gdd_consistency.py` | exit 0; 114 file, 0 lỗi, 0 cảnh báo, 1 allowlist | Không quét ROADMAP hoặc production/roadmap |
| `python3 Tools/QA/test_validate_gdd_consistency.py` | exit 0; 37 test OK | Test validator |
| `python3 Tools/QA/test_backend_service_unit.py` | exit 0; 4 nhóm pass | Lần sandbox lỗi EPERM khi mở socket; chạy ngoài sandbox mới pass |
| `python3 Tools/QA/test_backend_postgres.py` | exit 3, localhost không có PostgreSQL | Đã kiểm lại ngoài sandbox; không phải PASS, không dựng DB mới trong đợt tài liệu |
| `python3 Scripts/test_vanguard_design_sheet_art_gate.py` | exit 0 | Gate script cũ; không duyệt hình có mào và không tự giải đáp palette icon A0 |
| `python3 Scripts/test_phase3_art_gate.py` | exit 0; 215 asset, 0 failure | Proxy; không đo đầy đủ SPEC §0 mới |
| `python3 Tools/QA/verify_git_lfs_pointers.py --base origin/main` | exit 0, 13 file ở diff trước sửa đều không cần LFS | Không kiểm toàn bộ lịch sử LFS |
| `./Tools/QA/run_headless_tests.sh --ue-only` | gate FAIL (script 1); engine exit 139; Discovered=unknown, Passed=0, Failed=0, QueueFinished=0 | Khởi động crash trước khi discover. Không được gọi 0 failed là pass; không sửa engine/game trong PR tài liệu |
| `git config core.hooksPath`, chạy `Tools/git-hooks/pre-push` với ref main giả qua stdin | Hook trỏ đúng thư mục; BLOCKED exit 1 | Không gửi dữ liệu lên remote; không chạy force push thật |
| Parse `.claude/settings.json` | JSON hợp lệ | Chưa chứng minh Claude enforcement |
| `gh api repos/kenzings-ng/MortalAscension/rulesets/24157209` | active; deletion/non_fast_forward/pull_request/required_status_checks | Trạng thái GitHub tại lúc kiểm |
| `gh run list --commit a28da431fe1e5a4d31e7bb318268e6fbf2879d83` | push run 43118180079 success | CI fast gates, không build C++ |
| `gh api .../actions/runs?event=workflow_dispatch` | total_count=0 | Chưa có run workflow_dispatch theo API hiện tại |
| `git ls-remote --heads origin`, `gh pr list --state open` | main a28da43; không có PR mở | Nhánh roadmap chưa push ở lúc kiểm đầu vào |

CI tham chiếu: [run 43118180079](https://github.com/kenzings-ng/MortalAscension/actions/runs/43118180079). Workflow `.github/workflows/tests.yml` chạy PostgreSQL 16 service và test service thật; backend code không đổi so với main trong đợt này.

Lần UE trong sandbox bị chặn ghi Zen cache (read-only), đã ngắt rồi chạy lại ngoài sandbox; kết quả ngoài sandbox là crash startup, chưa có test bắt đầu. Không chạy thêm lần thứ ba hoặc sửa code để đổi kết quả trong PR này. Không chạy mới multi-client vì UE đơn tiến trình chưa khởi động xong.

Chi tiết kiểm filesystem: [facts.json](facts.json). Việc không thấy chuỗi `KeyFrames` trong binary chỉ là heuristic, không phải giải mã định dạng uasset. Cần engine inspection ở G1/G2 để kết luận flipbook có bao nhiêu frame.

Không lưu log thô có đường dẫn cá nhân/token vào repo. Log lệnh cục bộ ở `/tmp/ascendant-roadmap-audit/`; biên bản và facts đã được giữ trong repo để không phụ thuộc thư mục tạm.

## Kiểm tra sau khi sửa tài liệu

- Validator GDD: 114 file, 0 lỗi/0 cảnh báo/1 allowlist, exit 0.
- Validator self-test: 37 test OK, exit 0.
- `git diff --check origin/main`: exit 0.
- Đối chiếu tự động với `git show 38df548:production/ROADMAP.md`: đủ 49/49 mục cũ, trạng thái từng dòng khớp bảng; đủ 29 ô trạng thái cho G/H/A/L/S.
