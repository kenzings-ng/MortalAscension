"""
Project Ascendant - PostgreSQL Database Connection & Schema Management (ADR-0006)
"""

import os
import socket
from Services.Backend.config import (
    PG_HOST,
    PG_PORT,
    PG_USER,
    PG_PASSWORD,
    PG_DATABASE,
)

# Driver resolution (psycopg2 or psycopg3)
try:
    import psycopg2
    from psycopg2 import errors as pg_errors
    from psycopg2.extras import RealDictCursor
    PG_DRIVER = "psycopg2"
except ImportError:
    try:
        import psycopg
        from psycopg import errors as pg_errors
        from psycopg.rows import dict_row
        PG_DRIVER = "psycopg"
        RealDictCursor = None
    except ImportError:
        PG_DRIVER = None
        pg_errors = None
        RealDictCursor = None


def is_db_reachable(timeout: float = 3.0) -> bool:
    """Returns True if the PostgreSQL port accepts TCP or UNIX socket connections."""
    if PG_HOST.startswith("/"):
        sock_path = os.path.join(PG_HOST, f".s.PGSQL.{PG_PORT}")
        if not os.path.exists(sock_path):
            return False
        try:
            with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as s:
                s.settimeout(timeout)
                s.connect(sock_path)
            return True
        except OSError:
            return False
    try:
        with socket.create_connection((PG_HOST, PG_PORT), timeout=timeout):
            return True
    except OSError:
        return False


def get_connection():
    """Returns a new connection to the PostgreSQL database."""
    if PG_DRIVER == "psycopg2":
        return psycopg2.connect(
            host=PG_HOST,
            port=PG_PORT,
            user=PG_USER,
            password=PG_PASSWORD,
            dbname=PG_DATABASE,
        )
    elif PG_DRIVER == "psycopg":
        return psycopg.connect(
            host=PG_HOST,
            port=PG_PORT,
            user=PG_USER,
            password=PG_PASSWORD,
            dbname=PG_DATABASE,
        )
    else:
        raise RuntimeError("No PostgreSQL driver found. Please install psycopg2-binary.")


def init_schema(conn):
    """
    Applies the canonical PostgreSQL schema per DECISIONS.md §9 and ADR-0006.
    Ensures uk_owner_slot DEFERRABLE INITIALLY DEFERRED is present.
    """
    with conn.cursor() as cur:
        cur.execute("""
        CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

        -- 1. ACCOUNTS TABLE
        CREATE TABLE IF NOT EXISTS accounts (
            account_id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
            email VARCHAR(255) UNIQUE NOT NULL,
            password_hash VARCHAR(255) NOT NULL,
            gold_balance BIGINT NOT NULL DEFAULT 0 CHECK (gold_balance >= 0),
            created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
        );

        -- 2. CHARACTERS TABLE
        CREATE TABLE IF NOT EXISTS characters (
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

        -- 3. ITEMS TABLE (With DEFERRABLE INITIALLY DEFERRED unique constraint)
        CREATE TABLE IF NOT EXISTS items (
            item_instance_id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
            item_def_id VARCHAR(64) NOT NULL,
            owner_type VARCHAR(16) NOT NULL,
            owner_id UUID NOT NULL,
            slot_type VARCHAR(32) NOT NULL,
            slot_index INT NOT NULL,
            quantity INT NOT NULL DEFAULT 1 CHECK (quantity > 0),
            durability FLOAT NOT NULL DEFAULT 100.0,
            enhancement_level INT NOT NULL DEFAULT 0,
            item_data JSONB NOT NULL DEFAULT '{}',
            b_is_locked BOOLEAN NOT NULL DEFAULT FALSE,
            created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
            CONSTRAINT uk_owner_slot UNIQUE (owner_type, owner_id, slot_type, slot_index) DEFERRABLE INITIALLY DEFERRED
        );
        """)
        conn.commit()
