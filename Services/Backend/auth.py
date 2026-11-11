"""
Project Ascendant - Authentication & JWT Utilities (ADR-0006)
"""

import base64
import hashlib
import hmac
import json
import time
from typing import Tuple, Dict, Any, Optional
from Services.Backend.config import JWT_SECRET, JWT_EXPIRATION_SECONDS


def is_valid_email(email: str) -> bool:
    """
    Validates email format matching UPAAccountSubsystem::IsValidEmail and RFC 5322.
    """
    trimmed = email.strip()
    if len(trimmed) < 5:
        return False
    if " " in trimmed:
        return False
    if "@" not in trimmed:
        return False

    at_index = trimmed.find("@")
    if at_index == 0 or at_index == len(trimmed) - 1:
        return False

    domain_part = trimmed[at_index + 1:]
    if "." not in domain_part:
        return False

    dot_index = domain_part.find(".")
    if dot_index == 0 or dot_index == len(domain_part) - 1:
        return False

    return True


def is_valid_password(password: str) -> Tuple[bool, str]:
    """
    Validates password criteria (min 6 characters) matching UPAAccountSubsystem::IsValidPassword.
    """
    if len(password) < 6:
        return False, "Password must be at least 6 characters in length."
    return True, ""


def hash_password(plain_password: str, salt: str) -> str:
    """
    Hashes password using PBKDF2-HMAC-SHA256 with 100,000 iterations and per-user salt.
    """
    norm_salt = salt.strip().lower().encode("utf-8")
    pwd_bytes = plain_password.encode("utf-8")
    derived = hashlib.pbkdf2_hmac("sha256", pwd_bytes, norm_salt, 100000)
    return derived.hex()


def verify_password(plain_password: str, salt: str, expected_hash: str) -> bool:
    """
    Verifies plain password against expected hash in constant time.
    """
    calculated = hash_password(plain_password, salt)
    return hmac.compare_digest(calculated, expected_hash)


def _b64url_encode(data: bytes) -> str:
    return base64.urlsafe_b64encode(data).decode("utf-8").rstrip("=")


def _b64url_decode(data_str: str) -> bytes:
    padding = "=" * (4 - (len(data_str) % 4)) if (len(data_str) % 4) != 0 else ""
    return base64.urlsafe_b64decode(data_str + padding)


def generate_jwt(account_id: str, email: str, expires_in: Optional[int] = None) -> str:
    """
    Generates a standard RFC 7519 JSON Web Token (JWT) with HS256 signature.
    Zero external dependency implementation (uses standard library hmac/hashlib/base64).
    """
    exp_delta = expires_in if expires_in is not None else JWT_EXPIRATION_SECONDS
    now = int(time.time())
    payload = {
        "sub": account_id,
        "email": email,
        "iat": now,
        "exp": now + exp_delta,
        "iss": "project_ascendant_backend"
    }
    header = {
        "alg": "HS256",
        "typ": "JWT"
    }

    hdr_encoded = _b64url_encode(json.dumps(header, separators=(",", ":")).encode("utf-8"))
    payload_encoded = _b64url_encode(json.dumps(payload, separators=(",", ":")).encode("utf-8"))
    signing_input = f"{hdr_encoded}.{payload_encoded}".encode("utf-8")

    sig = hmac.new(JWT_SECRET.encode("utf-8"), signing_input, hashlib.sha256).digest()
    sig_encoded = _b64url_encode(sig)

    return f"{hdr_encoded}.{payload_encoded}.{sig_encoded}"


def verify_jwt(token: str) -> Dict[str, Any]:
    """
    Validates a JWT token's signature, format, and expiration.
    Returns the payload dictionary on success, or raises ValueError.
    """
    if not token or token.count(".") != 2:
        raise ValueError("Malformed JWT token structure.")

    hdr_str, payload_str, sig_str = token.split(".")
    signing_input = f"{hdr_str}.{payload_str}".encode("utf-8")
    expected_sig = hmac.new(JWT_SECRET.encode("utf-8"), signing_input, hashlib.sha256).digest()
    provided_sig = _b64url_decode(sig_str)

    if not hmac.compare_digest(expected_sig, provided_sig):
        raise ValueError("Invalid JWT signature.")

    payload = json.loads(_b64url_decode(payload_str).decode("utf-8"))
    now = int(time.time())
    if payload.get("exp", 0) < now:
        raise ValueError("JWT token has expired.")

    return payload
