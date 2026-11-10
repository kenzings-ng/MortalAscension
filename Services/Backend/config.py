"""
Project Ascendant - Backend Game Service Configuration (ADR-0006)
"""

import os

# PostgreSQL Database Configuration
PG_HOST = os.environ.get("PGHOST", os.environ.get("DB_HOST", "localhost"))
PG_PORT = int(os.environ.get("PGPORT", os.environ.get("DB_PORT", "5432")))
PG_USER = os.environ.get("PGUSER", os.environ.get("DB_USER", "postgres"))
PG_PASSWORD = os.environ.get("PGPASSWORD", os.environ.get("DB_PASSWORD", "password"))
PG_DATABASE = os.environ.get("PGDATABASE", os.environ.get("DB_NAME", "project_ascendant"))

# Security & Gateway Configuration
JWT_SECRET = os.environ.get("JWT_SECRET", "ascendant_jwt_secret_dev_key_32chars_long!")
JWT_ALGORITHM = "HS256"
JWT_EXPIRATION_SECONDS = int(os.environ.get("JWT_EXPIRATION_SECONDS", "86400"))  # 24 hours

# Pre-Shared Secret for Dedicated Server Authority Endpoints (/api/v1/server/*)
SERVER_SECRET = os.environ.get("SERVER_SECRET", "ascendant_dedicated_server_secret_2026")

# Service HTTP Port
SERVICE_PORT = int(os.environ.get("SERVICE_PORT", "8080"))
SERVICE_HOST = os.environ.get("SERVICE_HOST", "0.0.0.0")
