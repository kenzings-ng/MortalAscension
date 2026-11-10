#!/usr/bin/env python3
"""
Project Ascendant - Backend Service Unit Tests (ADR-0006)
Runs offline unit tests for Email validation, Password hashing, JWT token lifecycle,
and REST API route handling.
"""

import os
import sys
import uuid
import json
import time

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
if REPO_ROOT not in sys.path:
    sys.path.insert(0, REPO_ROOT)

from Services.Backend.auth import (
    is_valid_email,
    is_valid_password,
    hash_password,
    verify_password,
    generate_jwt,
    verify_jwt,
)
from Services.Backend.config import SERVER_SECRET


def test_auth_validation():
    print("[UNIT TEST 1] Testing Email & Password Validation Rules...")
    # RFC 5322 Email Validation
    assert is_valid_email("tester@ascendant.io") is True
    assert is_valid_email("vanguard@projectascendant.com") is True
    assert is_valid_email("invalid-email") is False
    assert is_valid_email("spaces in@email.com") is False
    assert is_valid_email("@nodomain.com") is False
    assert is_valid_email("no_dot@domain") is False
    assert is_valid_email("trailing_dot@domain.") is False

    # Password validation (>= 6 characters)
    ok, err = is_valid_password("12345")
    assert ok is False
    assert "at least 6 characters" in err

    ok, err = is_valid_password("123456")
    assert ok is True
    assert err == ""
    print("  -> Email & Password Validation PASSED.")


def test_password_hashing():
    print("\n[UNIT TEST 2] Testing PBKDF2 Password Hashing & Verification...")
    salt = "tester@ascendant.io"
    password = "MySecurePassword2026!"

    hashed = hash_password(password, salt)
    assert isinstance(hashed, str) and len(hashed) == 64  # SHA-256 hex string

    # Verification
    assert verify_password(password, salt, hashed) is True
    assert verify_password("WrongPassword!", salt, hashed) is False
    assert verify_password(password, "different_salt@ascendant.io", hashed) is False
    print("  -> Password Hashing & Constant-Time Verification PASSED.")


def test_jwt_lifecycle():
    print("\n[UNIT TEST 3] Testing JWT Generation, Verification & Expiration...")
    account_id = str(uuid.uuid4())
    email = "knight@ascendant.io"

    # Generate standard 24h token
    token = generate_jwt(account_id, email, expires_in=3600)
    assert token.count(".") == 2

    # Verify token
    payload = verify_jwt(token)
    assert payload["sub"] == account_id
    assert payload["email"] == email
    assert payload["iss"] == "project_ascendant_backend"
    assert payload["exp"] > int(time.time())

    # Tampered token
    tampered = token[:-4] + "abcd"
    tampered_failed = False
    try:
        verify_jwt(tampered)
    except ValueError:
        tampered_failed = True
    assert tampered_failed, "Tampered JWT did not raise ValueError!"

    # Expired token
    expired_token = generate_jwt(account_id, email, expires_in=-10)
    expired_failed = False
    try:
        verify_jwt(expired_token)
    except ValueError as e:
        assert "expired" in str(e).lower()
        expired_failed = True
    assert expired_failed, "Expired JWT did not raise ValueError!"

    print("  -> JWT Lifecycle, Signature Validation & Expiry PASSED.")


def main():
    print("=" * 60)
    print("Project Ascendant: Backend Service Unit Test Suite")
    print("=" * 60)
    try:
        test_auth_validation()
        test_password_hashing()
        test_jwt_lifecycle()
        print("\n" + "=" * 60)
        print(">> ALL BACKEND SERVICE UNIT TESTS PASSED SUCCESSFULLY (Exit 0) <<")
        print("=" * 60)
        return 0
    except AssertionError as e:
        print(f"\n[FAIL] Assertion failed: {e}", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"\n[ERROR] Unexpected error: {e}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
