"""
Project Ascendant - Backend REST API Server (ADR-0006)
Provides Client Auth endpoints and Dedicated Server Gateway endpoints.
"""

import json
import logging
import sys
from http.server import HTTPServer, BaseHTTPRequestHandler
from socketserver import ThreadingMixIn
from typing import Dict, Any, Tuple
from urllib.parse import urlparse

from Services.Backend.config import (
    SERVER_SECRET,
    SERVICE_HOST,
    SERVICE_PORT,
)
from Services.Backend.service import BackendService

logging.basicConfig(level=logging.INFO, format="[%(asctime)s] %(levelname)s %(name)s: %(message)s")
logger = logging.getLogger("AscendantBackend")


class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    daemon_threads = True


class AscendantApiHandler(BaseHTTPRequestHandler):
    """
    HTTP Request Handler implementing REST routes for Authentication and
    Dedicated Server Authority Gateway.
    """

    def _send_json(self, status_code: int, data: Dict[str, Any]):
        body = json.dumps(data).encode("utf-8")
        self.send_response(status_code)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def _read_json_body(self) -> Dict[str, Any]:
        content_length = int(self.headers.get("Content-Length", 0))
        if content_length == 0:
            return {}
        raw = self.rfile.read(content_length).decode("utf-8")
        try:
            return json.loads(raw)
        except Exception as e:
            raise ValueError(f"Invalid JSON body: {e}")

    def _verify_server_secret(self) -> bool:
        provided = self.headers.get("X-Server-Secret")
        return provided == SERVER_SECRET

    def do_GET(self):
        parsed = urlparse(self.path)
        path = parsed.path

        if path == "/health":
            return self._send_json(200, {"status": "ok", "service": "ProjectAscendant-Backend", "version": "1.0.0"})

        # Dedicated Server: GET /api/v1/server/characters/{id}
        if path.startswith("/api/v1/server/characters/"):
            if not self._verify_server_secret():
                return self._send_json(401, {"error": "Unauthorized: Invalid or missing X-Server-Secret."})
            char_id = path[len("/api/v1/server/characters/"):]
            try:
                char_data = BackendService.get_character(char_id)
                return self._send_json(200, char_data)
            except ValueError as e:
                return self._send_json(404, {"error": str(e)})
            except Exception as e:
                logger.exception("Error in GET character:")
                return self._send_json(500, {"error": str(e)})

        return self._send_json(404, {"error": f"Path not found: {path}"})

    def do_POST(self):
        parsed = urlparse(self.path)
        path = parsed.path

        try:
            body = self._read_json_body()
        except ValueError as e:
            return self._send_json(400, {"error": str(e)})

        # 1. CLIENT AUTH ENDPOINTS
        if path == "/api/v1/auth/register":
            email = body.get("email", "")
            password = body.get("password", "")
            try:
                res = BackendService.register_account(email, password)
                return self._send_json(201, res)
            except ValueError as e:
                return self._send_json(400, {"error": str(e)})
            except Exception as e:
                logger.exception("Error registering account:")
                return self._send_json(500, {"error": str(e)})

        if path == "/api/v1/auth/login":
            email = body.get("email", "")
            password = body.get("password", "")
            try:
                res = BackendService.login_account(email, password)
                return self._send_json(200, res)
            except ValueError as e:
                return self._send_json(400, {"error": str(e)})
            except Exception as e:
                logger.exception("Error logging in:")
                return self._send_json(500, {"error": str(e)})

        # 2. DEDICATED SERVER AUTHORITY ENDPOINTS (require X-Server-Secret)
        if path.startswith("/api/v1/server/"):
            if not self._verify_server_secret():
                return self._send_json(401, {"error": "Unauthorized: Invalid or missing X-Server-Secret."})

            if path == "/api/v1/server/validate-token":
                token = body.get("token", "")
                expected_id = body.get("expected_account_id")
                try:
                    res = BackendService.validate_server_token(token, expected_id)
                    return self._send_json(200, res)
                except ValueError as e:
                    return self._send_json(400, {"error": str(e), "valid": False})

            if path == "/api/v1/server/inventory/swap-slots":
                owner_type = body.get("owner_type", "CHARACTER")
                owner_id = body.get("owner_id", "")
                slot_type = body.get("slot_type", "INVENTORY")
                slot_a = body.get("slot_a")
                slot_b = body.get("slot_b")
                if slot_a is None or slot_b is None or not owner_id:
                    return self._send_json(400, {"error": "Missing required fields: owner_id, slot_a, slot_b."})
                try:
                    res = BackendService.swap_inventory_slots(owner_type, owner_id, slot_type, int(slot_a), int(slot_b))
                    return self._send_json(200, res)
                except Exception as e:
                    logger.exception("Error in swap_inventory_slots:")
                    return self._send_json(500, {"error": str(e)})

            if path.startswith("/api/v1/server/characters/") and path.endswith("/promote-class"):
                # POST /api/v1/server/characters/{id}/promote-class
                prefix = "/api/v1/server/characters/"
                suffix = "/promote-class"
                char_id = path[len(prefix):-len(suffix)]
                scroll_id = body.get("scroll_item_instance_id")
                target_class = body.get("target_class_tag")
                is_secondary = bool(body.get("is_secondary", False))
                if not scroll_id or not target_class:
                    return self._send_json(400, {"error": "Missing scroll_item_instance_id or target_class_tag."})
                try:
                    res = BackendService.promote_class(char_id, scroll_id, target_class, is_secondary)
                    return self._send_json(200, res)
                except ValueError as e:
                    return self._send_json(400, {"error": str(e)})
                except Exception as e:
                    logger.exception("Error in promote_class:")
                    return self._send_json(500, {"error": str(e)})

            if path.startswith("/api/v1/server/characters/") and path.endswith("/save"):
                # POST /api/v1/server/characters/{id}/save
                prefix = "/api/v1/server/characters/"
                suffix = "/save"
                char_id = path[len(prefix):-len(suffix)]
                level = body.get("character_level", 1)
                exp = body.get("character_exp", 0)
                primary_class = body.get("primary_class_tag", "")
                secondary_class = body.get("secondary_class_tag")
                history = body.get("class_progression_history")
                quests = body.get("completed_quests")
                items = body.get("items")
                try:
                    res = BackendService.save_character(
                        char_id, level, exp, primary_class, secondary_class,
                        history, quests, items
                    )
                    return self._send_json(200, res)
                except Exception as e:
                    logger.exception("Error in save_character:")
                    return self._send_json(500, {"error": str(e)})

        return self._send_json(404, {"error": f"Path not found: {path}"})


def run_server(host: str = SERVICE_HOST, port: int = SERVICE_PORT):
    server_address = (host, port)
    httpd = ThreadedHTTPServer(server_address, AscendantApiHandler)
    logger.info("Project Ascendant Backend Service running on %s:%d", host, port)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        logger.info("Shutting down backend server...")
        httpd.shutdown()


if __name__ == "__main__":
    host = sys.argv[1] if len(sys.argv) > 1 else SERVICE_HOST
    port = int(sys.argv[2]) if len(sys.argv) > 2 else SERVICE_PORT
    run_server(host, port)
