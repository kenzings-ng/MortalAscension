# ADR-0006: Backend Game Services, Dedicated Server Gateway & PostgreSQL Persistence

## Status
Accepted

## Date
2026-11-10

## Engine & Technology Compatibility

| Field | Value |
|---|---|
| **Engine** | Unreal Engine 5.7 / 5.8 (Dedicated Server Authority) |
| **Backend Technology** | Python 3.11+ / FastAPI (ASGI) + PostgreSQL 16 (`psycopg` / `asyncpg`) |
| **Authentication Protocol** | JSON Web Tokens (JWT HS256) + Server Mutual Pre-Shared Secret (`X-Server-Secret`) |
| **Transport Layer** | Unreal Engine `FHttpModule` (REST / JSON over HTTPS) |
| **Domain** | Backend Infrastructure, Persistence, Anti-Dupe Transactions & Security Gateways |
| **Governing Documents** | `DECISIONS.md` §9, §11, §13; `design/gdd/authentication-account-system.md`; `ADR-0001`; `ADR-0003` |

---

## ADR Dependencies

| Field | Value |
|---|---|
| **Depends On** | [`ADR-0001: Open World MMO Combat Networking`](adr-0001-open-world-mmo-combat-networking.md), [`ADR-0003: Server-Authoritative Grid Inventory`](adr-0003-server-authoritative-grid-inventory-fast-array.md) |
| **Enables** | Giai đoạn 6 (Backend & lưu dữ liệu), Auto-Save, Disconnect-Save, Token Handshake trong `AGameModeBase::PreLogin`, Chống dupe vật phẩm và thăng chức an toàn. |
| **Blocks** | Không thể lưu trữ tiến trình nhân vật thực tế qua phiên chơi nếu thiếu backend persistence gateway. |

---

## Context and Problem Statement

Trong cấu trúc MMO thời gian thực theo chuẩn `ADR-0001` (100% Server-Authoritative), trò chơi cần một giải pháp lưu trữ dữ liệu vĩnh viễn (persistence) đáp ứng các điều kiện khắt khe:
1. **Phân Định Quyền Hạn Triệt Để (Zero Trust Client Boundary)**:
   - Phía Client không bao giờ được phép giao tiếp trực tiếp với cơ sở dữ liệu PostgreSQL hoặc gửi lệnh ghi/sửa dữ liệu kinh tế (vàng, vật phẩm, class, cấp độ).
   - Mọi thao tác ghi dữ liệu bắt buộc phải do **Dedicated Server** phát lệnh thông qua REST API (`FHttpModule`).
2. **Chứng Thực Phiên & Chống Giả Mạo (Token Authentication & Handshake)**:
   - Người chơi đăng nhập/đăng ký ở màn hình ngoài game để nhận mã phiên ký số JWT.
   - Khi kết nối vào Dedicated Server, Dedicated Server phải thẩm tra tính hợp lệ của Token trước khi cho phép spawn nhân vật (`AGameModeBase::PreLogin`).
3. **Chống Nhân Bản Vật Phẩm (Anti-Dupe & Atomic Transactions)**:
   - Hoán đổi vị trí ô túi đồ (Slot Swap) không được phát sinh trạng thái trung gian gây nhân bản đồ hoặc lỗi vi phạm khóa duy nhất (Unique Violation).
   - Sử dụng Quyển Trục để thăng chức (Class Promotion) phải thực thi khóa dòng (`SELECT ... FOR UPDATE`) để ngăn chặn việc hai yêu cầu đồng thời cùng dùng một quyển trục để chuyển chức hai lần.
4. **Lưu Định Kỳ & Lưu Khi Thoát (Auto-Save & Disconnect-Save)**:
   - Khi người chơi bước vào Tòa Thành an toàn (Citadel), thoát game (Logout / Disconnect), hoặc theo chu kỳ định kỳ (Auto-Save 300s), trạng thái nhân vật và túi đồ phải được lưu xuống PostgreSQL an toàn.

---

## Decision Drivers

* **Bảo Mật & Toàn Vẹn Dữ Liệu**: Loại bỏ hoàn toàn khả năng người chơi can thiệp payload lưu trữ.
* **Hiệu Năng & Khả Năng Mở Rộng**: Sử dụng FastAPI ASGI cho Backend Game Service nhằm xử lý I/O bất đồng bộ cực nhanh, tương thích 100% với hạ tầng CI hiện tại của dự án.
* **Tuân Thủ Tuyệt Đối GDD & DECISIONS**:
  - Tuân thủ cấu trúc 3 bảng (`accounts`, `characters`, `items`) theo `DECISIONS.md` §9.
  - Sử dụng ràng buộc duy nhất hoãn lại: `uk_owner_slot UNIQUE (owner_type, owner_id, slot_type, slot_index) DEFERRABLE INITIALLY DEFERRED`.
  - Khóa dòng nguyên tử `SELECT ... FOR UPDATE` đối với quyển trục thăng chức.

---

## Considered Options

### 1. Công Nghệ Backend Service (DECISIONS §13)
* **Option 1A (Chấp Thuận): Python 3.11+ / FastAPI (ASGI Microservice)**:
  - *Ưu điểm*: Tích hợp hoàn hảo với hệ thống kiểm thử hiện có của dự án (`Tools/QA/test_backend_postgres.py`, GitHub Actions `gates` job, python 3.11). Khởi động cực nhanh (< 0.1s), async I/O thông lượng cao, tích hợp Pydantic schema validation và OpenAPI documentation tự động.
  - *Nhược điểm*: Cần quản lý connection pool PostgreSQL chuẩn xác (`psycopg` / `asyncpg`).
* **Option 1B: Node.js / NestJS**:
  - *Nhược điểm*: Đã bị tạm hoãn xem xét ở `DECISIONS.md` §13 ("chưa chốt NestJS"). Tạo gánh nặng dependency khổng lồ (`node_modules`), kéo dài thời gian build CI trong `gates` job.
* **Option 1C: Go (Gin/Fiber)**:
  - *Nhược điểm*: Cần bổ sung thêm Go toolchain vào môi trường phát triển và CI runner, làm tăng độ phân mảnh công nghệ của repo.

### 2. Mô Hình Giao Tiếp Giữa Dedicated Server và Backend Service
* **Option 2A (Chấp Thuận): REST API JSON qua Unreal Engine `FHttpModule` kèm `X-Server-Secret`**:
  - Dedicated Server sử dụng module chuẩn `HTTP` của Unreal Engine (`FHttpModule` / `IHttpRequest`) gửi request JSON có header bí mật máy chủ `X-Server-Secret`.
  - Backend Service chỉ mở các endpoint `/api/v1/server/*` cho IP nội bộ hoặc request mang đúng `X-Server-Secret`.
* **Option 2B: Direct PostgreSQL Connection từ Unreal Engine C++**:
  - *Nhược điểm*: Rất kém an toàn, yêu cầu nhúng thư viện `libpq` vào binary Unreal Engine, khó quản lý connection pool khi số lượng server shard tăng cao, thiếu lớp logic validation tập trung.

---

## Decision

Nhóm kỹ thuật quyết định chọn **Option 1A** (Python FastAPI) kết hợp **Option 2A** (`FHttpModule` REST API) cho toàn bộ hệ thống Backend Game Service và Lưu Dữ Liệu Vĩnh Viễn của *Project Ascendant*.

### 1. Kiến Trúc Phân Lớp (Layered Architecture)

```
+-----------------------------------------------------------+
|                        GAME CLIENT                        |
|  - UI Login / Register -> POST /api/v1/auth/* (Nhận JWT)  |
|  - Connect Dedicated Server -> Gửi JWT qua PreLogin       |
|  * TUYỆT ĐỐI KHÔNG GỌI API LƯU HOẶC GHI DATABASE          |
+-----------------------------+-----------------------------+
                              | (Game Traffic via Iris/Net)
                              v
+-----------------------------------------------------------+
|                 UNREAL DEDICATED SERVER                   |
|  - Single Source of Truth (Gameplay, Máu, Vị trí, Túi đồ) |
|  - PreLogin -> Gọi /api/v1/server/validate-token          |
|  - PostLogin -> Tải dữ liệu nhân vật / inventory          |
|  - Citadel Enter / Disconnect / Periodic -> Auto-Save     |
|  - Gửi request qua FHttpModule với header X-Server-Secret |
+-----------------------------+-----------------------------+
                              | (REST / HTTPS Internal LAN)
                              v
+-----------------------------------------------------------+
|              BACKEND GAME SERVICE (FastAPI)               |
|  - /api/v1/auth/* (Đăng ký, Đăng nhập, JWT token)          |
|  - /api/v1/server/* (Xác thực token, load/save character, |
|    hoán đổi ô đồ, thăng chức nguyên tử bằng quyển trục)   |
+-----------------------------+-----------------------------+
                              | (SQL Connection Pool)
                              v
+-----------------------------------------------------------+
|                 POSTGRESQL 16 DATABASE                    |
|  - accounts (account_id, email, password_hash, gold)      |
|  - characters (character_id, level, class tags, quests)   |
|  - items (uk_owner_slot DEFERRABLE INITIALLY DEFERRED)    |
+-----------------------------------------------------------+
```

### 2. Đặc Tả Cơ Sở Dữ Liệu PostgreSQL Canonical (DECISIONS §9)

```sql
-- 1. BẢNG TÀI KHOẢN (Accounts)
CREATE TABLE accounts (
    account_id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    gold_balance BIGINT NOT NULL DEFAULT 0 CHECK (gold_balance >= 0),
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

-- 2. BẢNG NHÂN VẬT (Characters)
CREATE TABLE characters (
    character_id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    account_id UUID NOT NULL REFERENCES accounts(account_id) ON DELETE CASCADE,
    character_name VARCHAR(64) UNIQUE NOT NULL,
    character_level INT NOT NULL DEFAULT 1 CHECK (character_level BETWEEN 1 AND 50),
    character_exp BIGINT NOT NULL DEFAULT 0 CHECK (character_exp >= 0),
    primary_class_tag VARCHAR(128) NOT NULL,
    secondary_class_tag VARCHAR(128),
    class_progression_history JSONB NOT NULL DEFAULT '{}',
    completed_quests TEXT[] NOT NULL DEFAULT '{}',
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

-- 3. BẢNG VẬT PHẨM (Items - Single Source of Truth)
CREATE TABLE items (
    item_instance_id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    item_def_id VARCHAR(64) NOT NULL,
    owner_type VARCHAR(16) NOT NULL, -- 'CHARACTER' hoặc 'ACCOUNT'
    owner_id UUID NOT NULL,
    slot_type VARCHAR(32) NOT NULL,  -- 'INVENTORY', 'MAINHAND', 'OFFHAND', 'BODY', 'SHARED_STASH'
    slot_index INT NOT NULL,
    quantity INT NOT NULL DEFAULT 1 CHECK (quantity > 0),
    durability FLOAT NOT NULL DEFAULT 100.0,
    enhancement_level INT NOT NULL DEFAULT 0,
    item_data JSONB NOT NULL DEFAULT '{}',
    b_is_locked BOOLEAN NOT NULL DEFAULT FALSE,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
    CONSTRAINT uk_owner_slot UNIQUE (owner_type, owner_id, slot_type, slot_index) DEFERRABLE INITIALLY DEFERRED
);
```

### 3. Quy Chuẩn REST Endpoints

#### A. Nhóm Client Auth (`/api/v1/auth/`)
* `POST /api/v1/auth/register`:
  - Request: `{ "email": "player@ascendant.io", "password": "secure_password" }`
  - Response: `{ "account_id": "UUID", "email": "...", "message": "Registered successfully" }`
* `POST /api/v1/auth/login`:
  - Request: `{ "email": "player@ascendant.io", "password": "secure_password" }`
  - Response: `{ "access_token": "JWT_TOKEN", "token_type": "bearer", "account_id": "UUID" }`

#### B. Nhóm Server Authority (`/api/v1/server/`, yêu cầu header `X-Server-Secret`)
* `POST /api/v1/server/validate-token`:
  - Request: `{ "token": "JWT_TOKEN", "expected_account_id": "UUID" }`
  - Response: `{ "valid": true, "account_id": "UUID", "email": "..." }`
* `GET /api/v1/server/characters/{character_id}`:
  - Trả về toàn bộ dữ liệu nhân vật, cấp độ, class tags, và mảng danh sách vật phẩm trong túi đồ/trang bị.
* `POST /api/v1/server/characters/{character_id}/save`:
  - Lưu trạng thái cấp độ, exp, class history, danh sách quest, và toàn bộ vật phẩm đang mang theo.
* `POST /api/v1/server/inventory/swap-slots`:
  - Hoán đổi vị trí hai vật phẩm trong cùng một giao dịch database, dựa vào ràng buộc `DEFERRABLE INITIALLY DEFERRED`.
* `POST /api/v1/server/characters/{character_id}/promote-class`:
  - Thực thi giao dịch thăng chức: khóa dòng quyển trục bằng `SELECT ... FOR UPDATE`, xác thực điều kiện thăng chức, xóa quyển trục, cập nhật `primary_class_tag` hoặc `secondary_class_tag`, và commit nguyên tử.

---

## Consequences

### Positive
- **Tuyệt đối an toàn trước nguy cơ hack/cheat client**: Client không có kết nối cơ sở dữ liệu và không thể tự phát lệnh lưu đồ/tiền.
- **Triệt tiêu Item Duping**: Nhờ khóa dòng `SELECT ... FOR UPDATE` và ràng buộc duy nhất hoãn lại `uk_owner_slot DEFERRABLE INITIALLY DEFERRED`.
- **Khả năng kiểm thử toàn diện**: Toàn bộ luồng đăng nhập, xác thực token, lưu nhân vật, hoán đổi ô đồ và thăng chức được kiểm thử trực tiếp bằng mã test backend chạy với PostgreSQL thật trong CI.

### Negative / Trade-offs
- Dedicated Server phải tích hợp thêm module `HTTP` (`FHttpModule`) và xử lý bất đồng bộ các request REST API.
- Cần duy trì kết nối mạng ổn định giữa Dedicated Server và Backend Game Service trong cùng mạng LAN ảo (VPC).
