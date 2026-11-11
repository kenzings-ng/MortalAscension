#!/usr/bin/env python3
"""
Project Ascendant - Real PostgreSQL Backend Database & Anti-Dupe Test Suite (ADR-0006)
Validates schema, constraints, real BackendService business logic, deferred slot swapping,
concurrent row-locking (SELECT ... FOR UPDATE), and REST API endpoints against
a real PostgreSQL database (via CI service container or local docker compose).
"""

import os
import sys
import uuid
import time
import threading
import json
import urllib.request
import urllib.error

# Ensure workspace root is in sys.path
REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
if REPO_ROOT not in sys.path:
    sys.path.insert(0, REPO_ROOT)

from Services.Backend.config import (
    PG_HOST,
    PG_PORT,
    PG_USER,
    PG_PASSWORD,
    PG_DATABASE,
    SERVER_SECRET,
)
from Services.Backend.db import (
    is_db_reachable,
    get_connection,
    init_schema,
    PG_DRIVER,
)
from Services.Backend.auth import (
    generate_jwt,
    verify_jwt,
    is_valid_email,
    is_valid_password,
)
from Services.Backend.service import BackendService
from Services.Backend.app import ThreadedHTTPServer, AscendantApiHandler

EXIT_SERVER_UNREACHABLE = 3


def test_real_backend_auth_and_character():
    """
    Tests real BackendService methods: register_account, login_account,
    validate_server_token, create_character, and get_character.
    """
    print("\n[BACKEND TEST 1] Testing Real BackendService Authentication & Character Persistence...")
    email = f"player_{uuid.uuid4().hex[:8]}@ascendant.io"
    password = "StrongPassword2026!"

    # 1. Register Account through real service
    reg_result = BackendService.register_account(email, password)
    assert "account_id" in reg_result, "Registration failed to return account_id."
    account_id = reg_result["account_id"]
    print(f"  -> Account registered successfully: ID={account_id}")

    # 2. Login Account and verify JWT issue
    login_result = BackendService.login_account(email, password)
    assert "access_token" in login_result, "Login failed to return access_token."
    token = login_result["access_token"]
    print("  -> Login successful, JWT token issued.")

    # 3. Dedicated Server Token Handshake Validation
    token_val = BackendService.validate_server_token(token, account_id)
    assert token_val["valid"] is True, "Token validation returned false."
    assert token_val["account_id"] == account_id, "Token validation returned incorrect account_id."
    print("  -> Dedicated Server token validation PASSED.")

    # 4. Create Character and query state
    char_name = f"Vanguard_{uuid.uuid4().hex[:6]}"
    char_res = BackendService.create_character(account_id, char_name, "Class.Line.Guard.Vanguard")
    char_id = char_res["character_id"]

    loaded_char = BackendService.get_character(char_id)
    assert loaded_char["character_name"] == char_name
    assert loaded_char["primary_class_tag"] == "Class.Line.Guard.Vanguard"
    assert loaded_char["character_level"] == 1
    print("  -> Character created and loaded through real backend service successfully.")

    return account_id, char_id


def test_atomic_slot_swapping_via_service(account_id: str, char_id: str):
    """
    Tests that BackendService.swap_inventory_slots leverages the
    uk_owner_slot DEFERRABLE INITIALLY DEFERRED unique constraint to atomically
    swap two items without triggering unique violations.
    """
    print("\n[BACKEND TEST 2] Testing Atomic Slot Swapping via Real BackendService...")
    item_a_id = str(uuid.uuid4())
    item_b_id = str(uuid.uuid4())

    conn = get_connection()
    with conn.cursor() as cur:
        # Seed Item A at slot 0, Item B at slot 1
        cur.execute("""
            INSERT INTO items (item_instance_id, item_def_id, owner_type, owner_id, slot_type, slot_index)
            VALUES (%s, 'sword_iron', 'CHARACTER', %s, 'INVENTORY', 0);
        """, (item_a_id, char_id))
        cur.execute("""
            INSERT INTO items (item_instance_id, item_def_id, owner_type, owner_id, slot_type, slot_index)
            VALUES (%s, 'shield_square', 'CHARACTER', %s, 'INVENTORY', 1);
        """, (item_b_id, char_id))
        conn.commit()
    conn.close()

    # Call the real BackendService atomic swap method
    swap_res = BackendService.swap_inventory_slots("CHARACTER", char_id, "INVENTORY", 0, 1)
    assert swap_res["status"] == "swapped", f"Expected 'swapped', got: {swap_res}"

    # Verify directly from DB that slots are swapped
    char_data = BackendService.get_character(char_id)
    items_by_id = {it["item_instance_id"]: it for it in char_data["items"]}

    assert items_by_id[item_a_id]["slot_index"] == 1, "Item A was not moved to slot 1."
    assert items_by_id[item_b_id]["slot_index"] == 0, "Item B was not moved to slot 0."
    print("  -> [PASS] Atomic slot swapping successfully executed and committed via BackendService.")


def test_concurrent_anti_dupe_promotion(account_id: str):
    """
    Spawns 2 threads executing BackendService.promote_class concurrently against the same
    scroll item using 2 separate PostgreSQL connections with SELECT ... FOR UPDATE.
    Asserts that exactly 1 promotion succeeds and 1 fails with 'not found or already consumed'.
    """
    print("\n[BACKEND TEST 3] Testing Concurrent SELECT ... FOR UPDATE Anti-Dupe via Real BackendService...")
    char_id = str(uuid.uuid4())
    scroll_id = str(uuid.uuid4())

    conn = get_connection()
    with conn.cursor() as cur:
        # Candidate character at Level 25
        cur.execute("""
            INSERT INTO characters (character_id, account_id, character_name, character_level, primary_class_tag)
            VALUES (%s, %s, %s, 25, 'Class.Line.Guard.Vanguard');
        """, (char_id, account_id, f"Candidate_{uuid.uuid4().hex[:6]}"))

        cur.execute("""
            INSERT INTO items (item_instance_id, item_def_id, owner_type, owner_id, slot_type, slot_index, quantity)
            VALUES (%s, 'scroll_dragon_knight', 'CHARACTER', %s, 'INVENTORY', 5, 1);
        """, (scroll_id, char_id))
        conn.commit()
    conn.close()

    results = []

    def worker(worker_id: int):
        try:
            res = BackendService.promote_class(
                character_id=char_id,
                scroll_item_instance_id=scroll_id,
                target_class_tag="Class.Line.Guard.DragonKnight",
                is_secondary=False
            )
            results.append((worker_id, True, res))
        except Exception as e:
            results.append((worker_id, False, str(e)))

    t1 = threading.Thread(target=worker, args=(1,))
    t2 = threading.Thread(target=worker, args=(2,))
    t1.start()
    t2.start()
    t1.join()
    t2.join()

    successes = [r for r in results if r[1] is True]
    failures = [r for r in results if r[1] is False]

    print(f"  Worker 1: {results[0]}")
    print(f"  Worker 2: {results[1]}")

    assert len(successes) == 1, f"Anti-Dupe Failed: Expected 1 success, got {len(successes)}"
    assert len(failures) == 1, f"Anti-Dupe Failed: Expected 1 failure, got {len(failures)}"

    char_final = BackendService.get_character(char_id)
    assert char_final["primary_class_tag"] == "Class.Line.Guard.DragonKnight"
    scroll_found = any(it["item_instance_id"] == scroll_id for it in char_final["items"])
    assert not scroll_found, "Promotion scroll was not deleted from database."
    print("  -> [PASS] SELECT ... FOR UPDATE strictly prevented duplicate promotion scroll consumption.")


def test_rest_api_http_server(account_id: str, char_id: str):
    """
    Starts an in-process ThreadedHTTPServer on a random free port, makes real HTTP requests
    via urllib.request, tests authentication, token validation, security header checks,
    and save/load endpoints.
    """
    print("\n[BACKEND TEST 4] Testing Real Backend REST API Endpoints over HTTP...")
    server = ThreadedHTTPServer(("127.0.0.1", 0), AscendantApiHandler)
    host, port = server.server_address
    server_thread = threading.Thread(target=server.serve_forever)
    server_thread.daemon = True
    server_thread.start()

    base_url = f"http://{host}:{port}"

    try:
        # 1. Health check
        with urllib.request.urlopen(f"{base_url}/health") as resp:
            assert resp.status == 200
            data = json.loads(resp.read().decode("utf-8"))
            assert data["status"] == "ok"
        print("  -> GET /health: 200 OK")

        # 2. POST /api/v1/auth/register
        reg_payload = json.dumps({
            "email": f"http_player_{uuid.uuid4().hex[:6]}@ascendant.io",
            "password": "Password123!"
        }).encode("utf-8")
        req = urllib.request.Request(f"{base_url}/api/v1/auth/register", data=reg_payload, headers={"Content-Type": "application/json"})
        with urllib.request.urlopen(req) as resp:
            assert resp.status == 201
            reg_data = json.loads(resp.read().decode("utf-8"))
            http_acc_id = reg_data["account_id"]
        print("  -> POST /api/v1/auth/register: 201 Created")

        # 3. POST /api/v1/auth/login
        login_payload = json.dumps({
            "email": reg_data["email"],
            "password": "Password123!"
        }).encode("utf-8")
        req = urllib.request.Request(f"{base_url}/api/v1/auth/login", data=login_payload, headers={"Content-Type": "application/json"})
        with urllib.request.urlopen(req) as resp:
            assert resp.status == 200
            login_data = json.loads(resp.read().decode("utf-8"))
            jwt_token = login_data["access_token"]
        print("  -> POST /api/v1/auth/login: 200 OK (JWT issued)")

        # 4. Security Check: Access /api/v1/server/* without X-Server-Secret should return 401 Unauthorized
        val_payload = json.dumps({"token": jwt_token, "expected_account_id": http_acc_id}).encode("utf-8")
        req_no_sec = urllib.request.Request(f"{base_url}/api/v1/server/validate-token", data=val_payload, headers={"Content-Type": "application/json"})
        try:
            urllib.request.urlopen(req_no_sec)
            assert False, "Should have returned 401 Unauthorized!"
        except urllib.error.HTTPError as e:
            assert e.code == 401, f"Expected 401, got {e.code}"
        print("  -> Security Verification: Missing X-Server-Secret properly rejected with 401 Unauthorized.")

        # 5. Access /api/v1/server/validate-token with valid X-Server-Secret
        req_with_sec = urllib.request.Request(
            f"{base_url}/api/v1/server/validate-token",
            data=val_payload,
            headers={"Content-Type": "application/json", "X-Server-Secret": SERVER_SECRET}
        )
        with urllib.request.urlopen(req_with_sec) as resp:
            assert resp.status == 200
            val_data = json.loads(resp.read().decode("utf-8"))
            assert val_data["valid"] is True
            assert val_data["account_id"] == http_acc_id
        print("  -> POST /api/v1/server/validate-token: 200 OK with X-Server-Secret")

        # 6. Dedicated Server Save Character: POST /api/v1/server/characters/{id}/save
        save_payload = json.dumps({
            "character_level": 15,
            "character_exp": 12500,
            "primary_class_tag": "Class.Line.Guard.Vanguard",
            "completed_quests": ["quest_ruins_scout"]
        }).encode("utf-8")
        req_save = urllib.request.Request(
            f"{base_url}/api/v1/server/characters/{char_id}/save",
            data=save_payload,
            headers={"Content-Type": "application/json", "X-Server-Secret": SERVER_SECRET}
        )
        with urllib.request.urlopen(req_save) as resp:
            assert resp.status == 200
            save_data = json.loads(resp.read().decode("utf-8"))
            assert save_data["status"] == "saved"
        print("  -> POST /api/v1/server/characters/{id}/save: 200 OK (Auto-Save executed)")

    finally:
        server.shutdown()
        server.server_close()


def main():
    print("=" * 60)
    print("Project Ascendant: Real PostgreSQL Backend Service Test Suite")
    print(f"Target DB: {PG_USER}@{PG_HOST}:{PG_PORT}/{PG_DATABASE}")
    print("=" * 60)

    if not is_db_reachable():
        print(f"[UNREACHABLE] No PostgreSQL server accepting connections at {PG_HOST}:{PG_PORT}.")
        print("\nTo run locally with Docker:")
        print("  docker compose up -d postgres")
        print("\nIn CI (GitHub Actions):")
        print("  This runs automatically in the 'gates' job via the postgres service container.")
        return EXIT_SERVER_UNREACHABLE

    if PG_DRIVER is None:
        print("[ERROR] Neither psycopg2 nor psycopg is installed.")
        print("Install via: pip install psycopg2-binary")
        return 1

    try:
        conn = get_connection()
    except Exception as e:
        print(f"[FAIL] PostgreSQL is reachable at {PG_HOST}:{PG_PORT} but connecting failed: {e}")
        return 1

    try:
        init_schema(conn)
        conn.close()

        # Run real backend tests
        account_id, char_id = test_real_backend_auth_and_character()
        test_atomic_slot_swapping_via_service(account_id, char_id)
        test_concurrent_anti_dupe_promotion(account_id)
        test_rest_api_http_server(account_id, char_id)

        print("\n" + "=" * 60)
        print(">> ALL REAL BACKEND SERVICE POSTGRESQL TESTS PASSED SUCCESSFULLY (Exit 0) <<")
        print("=" * 60)
        return 0
    except AssertionError as e:
        print(f"\n[FAIL] Assertion failed: {e}", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"\n[ERROR] Backend test encountered exception: {e}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
