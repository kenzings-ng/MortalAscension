"""
Project Ascendant - Core Backend Business Logic & Transaction Services (ADR-0006)
"""

import json
import uuid
from typing import Dict, Any, List, Optional, Tuple
from Services.Backend.db import get_connection
from Services.Backend.auth import (
    is_valid_email,
    is_valid_password,
    hash_password,
    verify_password,
    generate_jwt,
    verify_jwt,
)


class BackendService:
    """
    Core backend game service providing authentication, server-token handshake,
    atomic inventory operations (anti-dupe), and row-locked class promotions.
    """

    @staticmethod
    def register_account(email: str, password: str) -> Dict[str, Any]:
        """
        Registers a new player account into PostgreSQL 'accounts' table.
        """
        norm_email = email.strip()
        if not is_valid_email(norm_email):
            raise ValueError("Invalid email format.")

        ok, err_msg = is_valid_password(password)
        if not ok:
            raise ValueError(err_msg)

        conn = get_connection()
        try:
            with conn.cursor() as cur:
                # Check if account already exists
                cur.execute("SELECT account_id FROM accounts WHERE email = %s;", (norm_email,))
                if cur.fetchone():
                    raise ValueError("Account with this email already exists.")

                account_id = str(uuid.uuid4())
                pwd_hash = hash_password(password, norm_email)

                cur.execute("""
                    INSERT INTO accounts (account_id, email, password_hash, gold_balance)
                    VALUES (%s, %s, %s, 0);
                """, (account_id, norm_email, pwd_hash))
                conn.commit()

            return {
                "account_id": account_id,
                "email": norm_email,
                "message": "Account registered successfully."
            }
        finally:
            conn.close()

    @staticmethod
    def login_account(email: str, password: str) -> Dict[str, Any]:
        """
        Authenticates account credentials and issues a signed JWT token.
        """
        norm_email = email.strip()
        if not is_valid_email(norm_email):
            raise ValueError("Invalid email format.")

        conn = get_connection()
        try:
            with conn.cursor() as cur:
                cur.execute("SELECT account_id, password_hash FROM accounts WHERE email = %s;", (norm_email,))
                row = cur.fetchone()
                if not row:
                    raise ValueError("Account not found.")

                account_id, stored_hash = row
                if not verify_password(password, norm_email, stored_hash):
                    raise ValueError("Incorrect password.")

            token = generate_jwt(str(account_id), norm_email)
            return {
                "access_token": token,
                "token_type": "bearer",
                "account_id": str(account_id),
                "email": norm_email
            }
        finally:
            conn.close()

    @staticmethod
    def validate_server_token(token: str, expected_account_id: Optional[str] = None) -> Dict[str, Any]:
        """
        Dedicated Server endpoint to validate client's JWT session token before spawning.
        """
        payload = verify_jwt(token)
        account_id = payload.get("sub")
        if not account_id:
            raise ValueError("Invalid token: missing subject (account_id).")

        if expected_account_id and expected_account_id != account_id:
            raise ValueError(f"Token account mismatch: expected {expected_account_id}, got {account_id}.")

        return {
            "valid": True,
            "account_id": account_id,
            "email": payload.get("email", ""),
            "issued_at": payload.get("iat"),
            "expires_at": payload.get("exp")
        }

    @staticmethod
    def create_character(account_id: str, character_name: str, primary_class_tag: str) -> Dict[str, Any]:
        """
        Creates a new character linked to an account.
        """
        conn = get_connection()
        try:
            char_id = str(uuid.uuid4())
            with conn.cursor() as cur:
                cur.execute("""
                    INSERT INTO characters (
                        character_id, account_id, character_name, character_level, character_exp,
                        primary_class_tag, secondary_class_tag, class_progression_history, completed_quests
                    )
                    VALUES (%s, %s, %s, 1, 0, %s, NULL, '{}', '{}')
                    RETURNING character_id, created_at;
                """, (char_id, account_id, character_name, primary_class_tag))
                row = cur.fetchone()
                conn.commit()

            return {
                "character_id": char_id,
                "account_id": account_id,
                "character_name": character_name,
                "primary_class_tag": primary_class_tag,
                "character_level": 1,
                "created_at": str(row[1]) if row else ""
            }
        finally:
            conn.close()

    @staticmethod
    def get_character(character_id: str) -> Dict[str, Any]:
        """
        Loads full character state and equipment/inventory items for Dedicated Server.
        """
        conn = get_connection()
        try:
            with conn.cursor() as cur:
                cur.execute("""
                    SELECT character_id, account_id, character_name, character_level, character_exp,
                           primary_class_tag, secondary_class_tag, class_progression_history, completed_quests
                    FROM characters
                    WHERE character_id = %s;
                """, (character_id,))
                char_row = cur.fetchone()
                if not char_row:
                    raise ValueError(f"Character {character_id} not found.")

                cur.execute("""
                    SELECT item_instance_id, item_def_id, owner_type, owner_id, slot_type,
                           slot_index, quantity, durability, enhancement_level, item_data, b_is_locked
                    FROM items
                    WHERE owner_type = 'CHARACTER' AND owner_id = %s
                    ORDER BY slot_type, slot_index;
                """, (character_id,))
                item_rows = cur.fetchall()

            items = []
            for r in item_rows:
                items.append({
                    "item_instance_id": str(r[0]),
                    "item_def_id": r[1],
                    "owner_type": r[2],
                    "owner_id": str(r[3]),
                    "slot_type": r[4],
                    "slot_index": r[5],
                    "quantity": r[6],
                    "durability": float(r[7]),
                    "enhancement_level": r[8],
                    "item_data": r[9] if isinstance(r[9], dict) else json.loads(r[9] or "{}"),
                    "b_is_locked": bool(r[10])
                })

            history = char_row[7]
            if isinstance(history, str):
                history = json.loads(history)

            return {
                "character_id": str(char_row[0]),
                "account_id": str(char_row[1]),
                "character_name": char_row[2],
                "character_level": char_row[3],
                "character_exp": char_row[4],
                "primary_class_tag": char_row[5],
                "secondary_class_tag": char_row[6],
                "class_progression_history": history,
                "completed_quests": list(char_row[8]) if char_row[8] else [],
                "items": items
            }
        finally:
            conn.close()

    @staticmethod
    def save_character(character_id: str,
                       character_level: int,
                       character_exp: int,
                       primary_class_tag: str,
                       secondary_class_tag: Optional[str] = None,
                       class_progression_history: Optional[Dict[str, Any]] = None,
                       completed_quests: Optional[List[str]] = None,
                       items: Optional[List[Dict[str, Any]]] = None) -> Dict[str, Any]:
        """
        Saves character state and synchronizes items (Dedicated Server Auto-Save / Disconnect-Save).
        """
        conn = get_connection()
        try:
            with conn.cursor() as cur:
                history_json = json.dumps(class_progression_history or {})
                quests_arr = completed_quests or []

                cur.execute("""
                    UPDATE characters
                    SET character_level = %s,
                        character_exp = %s,
                        primary_class_tag = %s,
                        secondary_class_tag = %s,
                        class_progression_history = %s::jsonb,
                        completed_quests = %s
                    WHERE character_id = %s;
                """, (character_level, character_exp, primary_class_tag, secondary_class_tag,
                      history_json, quests_arr, character_id))

                if items is not None:
                    for it in items:
                        item_id = it.get("item_instance_id")
                        durability = it.get("durability", 100.0)
                        enhancement = it.get("enhancement_level", 0)
                        quantity = it.get("quantity", 1)
                        slot_index = it.get("slot_index")
                        slot_type = it.get("slot_type", "INVENTORY")
                        b_locked = it.get("b_is_locked", False)
                        item_data = json.dumps(it.get("item_data", {}))

                        if item_id:
                            cur.execute("""
                                UPDATE items
                                SET slot_type = %s,
                                    slot_index = %s,
                                    quantity = %s,
                                    durability = %s,
                                    enhancement_level = %s,
                                    b_is_locked = %s,
                                    item_data = %s::jsonb
                                WHERE item_instance_id = %s;
                            """, (slot_type, slot_index, quantity, durability, enhancement,
                                  b_locked, item_data, item_id))

                conn.commit()

            return {
                "status": "saved",
                "character_id": character_id,
                "character_level": character_level
            }
        except Exception:
            conn.rollback()
            raise
        finally:
            conn.close()

    @staticmethod
    def swap_inventory_slots(owner_type: str, owner_id: str, slot_type: str,
                             slot_a: int, slot_b: int) -> Dict[str, Any]:
        """
        Atomically swaps two inventory slots inside a single transaction using
        the PostgreSQL 'uk_owner_slot DEFERRABLE INITIALLY DEFERRED' unique constraint.
        """
        if slot_a == slot_b:
            return {"status": "unchanged", "slot_a": slot_a, "slot_b": slot_b}

        conn = get_connection()
        try:
            with conn.cursor() as cur:
                # Find item currently at slot_a
                cur.execute("""
                    SELECT item_instance_id FROM items
                    WHERE owner_type = %s AND owner_id = %s AND slot_type = %s AND slot_index = %s;
                """, (owner_type, owner_id, slot_type, slot_a))
                row_a = cur.fetchone()
                item_a = row_a[0] if row_a else None

                # Find item currently at slot_b
                cur.execute("""
                    SELECT item_instance_id FROM items
                    WHERE owner_type = %s AND owner_id = %s AND slot_type = %s AND slot_index = %s;
                """, (owner_type, owner_id, slot_type, slot_b))
                row_b = cur.fetchone()
                item_b = row_b[0] if row_b else None

                if not item_a and not item_b:
                    return {"status": "empty_slots", "slot_a": slot_a, "slot_b": slot_b}

                # Perform the atomic swap. Intermediate states may duplicate a slot index,
                # but because uk_owner_slot is DEFERRABLE INITIALLY DEFERRED, it succeeds!
                if item_a and item_b:
                    cur.execute("UPDATE items SET slot_index = %s WHERE item_instance_id = %s;", (slot_b, item_a))
                    cur.execute("UPDATE items SET slot_index = %s WHERE item_instance_id = %s;", (slot_a, item_b))
                elif item_a and not item_b:
                    cur.execute("UPDATE items SET slot_index = %s WHERE item_instance_id = %s;", (slot_b, item_a))
                elif item_b and not item_a:
                    cur.execute("UPDATE items SET slot_index = %s WHERE item_instance_id = %s;", (slot_a, item_b))

                conn.commit()

            return {
                "status": "swapped",
                "owner_id": owner_id,
                "slot_type": slot_type,
                "item_a": str(item_a) if item_a else None,
                "new_slot_a": slot_b if item_a else None,
                "item_b": str(item_b) if item_b else None,
                "new_slot_b": slot_a if item_b else None
            }
        except Exception:
            conn.rollback()
            raise
        finally:
            conn.close()

    @staticmethod
    def promote_class(character_id: str, scroll_item_instance_id: str,
                      target_class_tag: str, is_secondary: bool = False) -> Dict[str, Any]:
        """
        Executes an atomic promotion transaction with row-level locking (SELECT ... FOR UPDATE)
        on the promotion scroll item to prevent concurrent duplicate consumption (DECISIONS.md §9).
        """
        conn = get_connection()
        try:
            with conn.cursor() as cur:
                # 1. Acquire exclusive row lock on the scroll item
                cur.execute("""
                    SELECT item_instance_id, item_def_id, quantity, owner_type, owner_id
                    FROM items
                    WHERE item_instance_id = %s
                    FOR UPDATE;
                """, (scroll_item_instance_id,))
                scroll_row = cur.fetchone()

                if not scroll_row:
                    conn.rollback()
                    raise ValueError(f"Promotion scroll {scroll_item_instance_id} not found or already consumed.")

                _, def_id, quantity, owner_type, owner_id = scroll_row

                if owner_type != "CHARACTER" or str(owner_id) != character_id:
                    conn.rollback()
                    raise ValueError("Promotion scroll does not belong to the requesting character.")

                if quantity <= 0:
                    conn.rollback()
                    raise ValueError("Promotion scroll quantity is zero.")

                # 2. Lock and check character record
                cur.execute("""
                    SELECT character_level, primary_class_tag, secondary_class_tag, class_progression_history
                    FROM characters
                    WHERE character_id = %s
                    FOR UPDATE;
                """, (character_id,))
                char_row = cur.fetchone()
                if not char_row:
                    conn.rollback()
                    raise ValueError(f"Character {character_id} not found.")

                char_level, cur_primary, cur_secondary, history = char_row
                if isinstance(history, str):
                    history = json.loads(history)

                # Validation per DECISIONS.md §3 & §4: Level 25 required for secondary class
                if is_secondary and char_level < 25:
                    conn.rollback()
                    raise ValueError(f"Character level {char_level} is below required level 25 for Secondary Class.")

                # 3. Update character class tag & progression history
                if is_secondary:
                    cur.execute("""
                        UPDATE characters
                        SET secondary_class_tag = %s
                        WHERE character_id = %s;
                    """, (target_class_tag, character_id))
                else:
                    cur.execute("""
                        UPDATE characters
                        SET primary_class_tag = %s
                        WHERE character_id = %s;
                    """, (target_class_tag, character_id))

                # 4. Consume the scroll item
                if quantity > 1:
                    cur.execute("""
                        UPDATE items SET quantity = quantity - 1 WHERE item_instance_id = %s;
                    """, (scroll_item_instance_id,))
                else:
                    cur.execute("DELETE FROM items WHERE item_instance_id = %s;", (scroll_item_instance_id,))

                conn.commit()

            return {
                "status": "promoted",
                "character_id": character_id,
                "target_class_tag": target_class_tag,
                "is_secondary": is_secondary,
                "consumed_scroll_id": scroll_item_instance_id
            }
        except Exception:
            conn.rollback()
            raise
        finally:
            conn.close()
