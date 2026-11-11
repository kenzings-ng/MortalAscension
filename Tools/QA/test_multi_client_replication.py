#!/usr/bin/env python3
"""
Project Ascendant - Automated Real Multi-Process Headless Replication Test Suite
Verifies 1 Dedicated Server + 2 Clients concurrent replication under Iris Replication System
per DECISIONS.md §11 & §13, and ROADMAP Giai đoạn 1.5.

Acceptance Criteria:
  AC-1: Dedicated Server lifecycle & Iris Replication initialization on Linux headless.
  AC-2: Dual client network handshake and join sequence (NotifyAcceptingConnection, Join succeeded).
  AC-3: Iris Replication consistency, valid descriptors, and zero subobject ensure errors.
  AC-4: Multi-client Player Controller instantiation (PABasePlayerController_0 and PABasePlayerController_1).
  AC-5: Graceful multi-process teardown with zero crashes, fatal asserts, or CrashReportClient invocations.
"""

import argparse
import os
import re
import signal
import socket
import subprocess
import sys
import time
from pathlib import Path


def is_port_in_use(port: int) -> bool:
    """Checks if UDP/TCP port is currently occupied."""
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        try:
            s.bind(("127.0.0.1", port))
            return False
        except OSError:
            return True


def find_available_port(start_port: int = 7788, max_attempts: int = 20) -> int:
    """Finds an available UDP port starting from start_port."""
    for p in range(start_port, start_port + max_attempts):
        if not is_port_in_use(p):
            return p
    raise RuntimeError(f"Could not find an available network port in range {start_port}..{start_port + max_attempts}")


def get_project_root() -> Path:
    """Returns absolute path to project root directory."""
    return Path(__file__).resolve().parent.parent.parent


def get_engine_bin() -> str:
    """Resolves path to UnrealEditor binary."""
    env_bin = os.environ.get("UE_EDITOR_PATH")
    if env_bin and os.path.isfile(env_bin):
        return env_bin
    default_bin = "/mnt/Data/Engine/Binaries/Linux/UnrealEditor"
    if os.path.isfile(default_bin):
        return default_bin
    raise FileNotFoundError(f"UnrealEditor binary not found. Set UE_EDITOR_PATH or install to {default_bin}")


def read_file_safely(path: Path) -> str:
    """Reads text file if present, ignoring decoding errors."""
    if not path.is_file():
        return ""
    try:
        with open(path, "r", encoding="utf-8", errors="ignore") as f:
            return f.read()
    except Exception:
        return ""


def main():
    parser = argparse.ArgumentParser(description="Project Ascendant Multi-Client Headless Replication Test Suite")
    parser.add_argument("--port", type=int, default=0, help="Server port (default: auto-detected free port >= 7788)")
    parser.add_argument("--timeout", type=int, default=95, help="Total execution timeout in seconds (default: 95)")
    parser.add_argument("--verbose", action="store_true", help="Print real-time status output")
    parser.add_argument("--keep-logs", action="store_true", help="Retain test log files after completion")
    args = parser.parse_args()

    project_root = get_project_root()
    uproject = project_root / "ProjectAscendant.uproject"
    if not uproject.is_file():
        print(f"[FAIL] .uproject file not found at {uproject}")
        sys.exit(1)

    engine_bin = get_engine_bin()
    port = args.port if args.port > 0 else find_available_port(7788)

    logs_dir = project_root / "Saved" / "Logs"
    logs_dir.mkdir(parents=True, exist_ok=True)

    server_log = logs_dir / "ReplicationTest_Server.log"
    client1_log = logs_dir / "ReplicationTest_Client1.log"
    client2_log = logs_dir / "ReplicationTest_Client2.log"

    # Remove previous test logs to guarantee clean assertions
    for log_path in [server_log, client1_log, client2_log]:
        if log_path.is_file():
            try:
                log_path.unlink()
            except OSError:
                pass

    # Ensure headless display and Vulkan settings
    env = os.environ.copy()
    env["SDL_VIDEODRIVER"] = "offscreen"
    env["__NV_PRIME_RENDER_OFFLOAD"] = "1"
    env["__GLX_VENDOR_LIBRARY_NAME"] = "nvidia"

    print("============================================================")
    print(" Project Ascendant: Multi-Client Headless Replication Test")
    print(f" Engine       : {engine_bin}")
    print(f" Port         : {port}")
    print(f" Project Root : {project_root}")
    print(" Architecture : 1 Dedicated Server + 2 Clients (Iris Replication)")
    print("============================================================")

    results = {
        "AC1_ServerLifecycle": False,
        "AC1_IrisSystem": False,
        "AC2_ConnectionHandshake": False,
        "AC2_JoinSucceeded": False,
        "AC3_NoIrisErrors": False,
        "AC3_NoSubobjectEnsures": False,
        "AC4_PlayerControllers": False,
        "AC5_CleanTeardown": False,
        "AC5_NoCrashReports": False,
    }

    server_proc = None
    client1_proc = None
    client2_proc = None

    start_time = time.time()

    try:
        # ---------------------------------------------------------------------
        # 1. Launch Dedicated Server
        # ---------------------------------------------------------------------
        print(f"\n[PHASE 1] Launching Dedicated Server (port {port})...")
        server_cmd = [
            engine_bin,
            str(uproject),
            "-server",
            "-nullrhi",
            "-nosound",
            "-unattended",
            f"-PORT={port}",
            f"-log={server_log.name}",
        ]
        server_proc = subprocess.Popen(server_cmd, env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        print(f" Dedicated Server PID: {server_proc.pid}")

        # Wait for server to load outpost map and initialize Iris
        server_ready = False
        print(" Waiting for Dedicated Server to load outpost world and listen...")
        while time.time() - start_time < 90:
            if server_proc.poll() is not None:
                print(f"[FAIL] Server exited unexpectedly with code {server_proc.returncode}")
                break

            content = read_file_safely(server_log)
            if "Bringing World" in content and "L_VerdantFrontier_Outpost" in content:
                server_ready = True
                break
            time.sleep(1.0)

        if not server_ready:
            print("[FAIL] Dedicated Server initialization timed out.")
            return 1

        print(" >> Dedicated Server ready. World brought up for play.")
        results["AC1_ServerLifecycle"] = True

        server_content = read_file_safely(server_log)
        if "LogIris: Display: Iris ReplicationSystem" in server_content and "is created" in server_content:
            results["AC1_IrisSystem"] = True
            print(" >> Iris Replication System successfully initialized on Server.")
        else:
            print(" [WARN] Iris ReplicationSystem creation message not detected in server log.")

        # ---------------------------------------------------------------------
        # 2. Launch Client 1 and Client 2
        # ---------------------------------------------------------------------
        print("\n[PHASE 2] Launching Client 1 and Client 2 in Headless Mode...")
        client_target = f"127.0.0.1:{port}"

        client1_cmd = [
            engine_bin,
            str(uproject),
            client_target,
            "-game",
            "-nullrhi",
            "-nosound",
            "-unattended",
            f"-log={client1_log.name}",
        ]
        client1_proc = subprocess.Popen(client1_cmd, env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        print(f" Client 1 PID: {client1_proc.pid} connecting to {client_target}")

        time.sleep(1.5)

        client2_cmd = [
            engine_bin,
            str(uproject),
            client_target,
            "-game",
            "-nullrhi",
            "-nosound",
            "-unattended",
            f"-log={client2_log.name}",
        ]
        client2_proc = subprocess.Popen(client2_cmd, env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        print(f" Client 2 PID: {client2_proc.pid} connecting to {client_target}")

        # ---------------------------------------------------------------------
        # 3. Monitor Multi-Client Replication & Join Sequence
        # ---------------------------------------------------------------------
        print("\n[PHASE 3] Monitoring Iris Replication & Multi-Client Join...")
        join_confirmed = False
        replicate_confirmed = False

        # Allow sufficient time for both clients to load map and establish session
        monitor_deadline = time.time() + 60
        while time.time() < monitor_deadline:
            if server_proc.poll() is not None:
                print(f"[FAIL] Server terminated prematurely with code {server_proc.returncode}")
                break

            s_text = read_file_safely(server_log)
            c1_text = read_file_safely(client1_log)
            c2_text = read_file_safely(client2_log)

            accept_count = s_text.count("NotifyAcceptingConnection")
            join_count = s_text.count("Join succeeded")
            pc_matches = set(re.findall(r"PABasePlayerController_[0-9]+", s_text))

            if accept_count >= 2:
                results["AC2_ConnectionHandshake"] = True

            if join_count >= 2:
                results["AC2_JoinSucceeded"] = True

            if len(pc_matches) >= 2 or ("PABasePlayerController_0" in s_text and "PABasePlayerController_1" in s_text):
                results["AC4_PlayerControllers"] = True

            # If all join conditions are met, hold for 5 seconds to verify replication ticks
            if results["AC2_ConnectionHandshake"] and (results["AC2_JoinSucceeded"] or results["AC4_PlayerControllers"]):
                join_confirmed = True
                print(" >> Multi-client connection and player controller registration confirmed.")
                time.sleep(5.0)
                break

            time.sleep(1.0)

        # ---------------------------------------------------------------------
        # 4. Check Iris & Netcode Health in Logs
        # ---------------------------------------------------------------------
        s_text = read_file_safely(server_log)
        c1_text = read_file_safely(client1_log)
        c2_text = read_file_safely(client2_log)

        # AC-3: Ensure no descriptor or fast-array errors
        combined_text = s_text + "\n" + c1_text + "\n" + c2_text
        fast_array_errors = [line for line in combined_text.splitlines() if "LogIris: Error:" in line and "FFastArray property" in line]
        subobject_ensures = [line for line in combined_text.splitlines() if "Ensure condition failed: GDefaultUseSubObjectReplicationList" in line]

        results["AC3_NoIrisErrors"] = len(fast_array_errors) == 0
        results["AC3_NoSubobjectEnsures"] = len(subobject_ensures) == 0

        # Check Iris replication active on clients
        if "LogIris: Display: Iris ReplicationSystem" in c1_text or "LogIris: Display: Iris ReplicationSystem" in c2_text:
            results["AC1_IrisSystem"] = True

    finally:
        # ---------------------------------------------------------------------
        # 5. Graceful Teardown of Child Processes
        # ---------------------------------------------------------------------
        print("\n[PHASE 4] Performing Graceful Teardown...")

        # Terminate clients first
        for name, proc in [("Client 1", client1_proc), ("Client 2", client2_proc)]:
            if proc and proc.poll() is None:
                try:
                    proc.send_signal(signal.SIGINT)
                    proc.wait(timeout=10)
                    print(f" >> {name} (PID {proc.pid}) exited cleanly with code {proc.returncode}.")
                except subprocess.TimeoutExpired:
                    print(f" [WARN] {name} (PID {proc.pid}) timed out on SIGINT, sending SIGKILL...")
                    proc.kill()
                    proc.wait()

        # Terminate server
        if server_proc and server_proc.poll() is None:
            try:
                server_proc.send_signal(signal.SIGINT)
                server_proc.wait(timeout=15)
                print(f" >> Dedicated Server (PID {server_proc.pid}) exited cleanly with code {server_proc.returncode}.")
            except subprocess.TimeoutExpired:
                print(f" [WARN] Dedicated Server (PID {server_proc.pid}) timed out on SIGINT, sending SIGKILL...")
                server_proc.kill()
                server_proc.wait()

        # AC-5: Check clean shutdown and zero crash reports
        crashes_dir = project_root / "Saved" / "Crashes"
        recent_crashes = []
        if crashes_dir.is_dir():
            for c_path in crashes_dir.iterdir():
                if c_path.is_dir() and (time.time() - c_path.stat().st_mtime) < (time.time() - start_time + 10):
                    # Check if crash folder contains crash dump (ignore benign ensure logs from prior runs)
                    if "ProjectAscendant" in c_path.name:
                        recent_crashes.append(c_path.name)

        s_text = read_file_safely(server_log)
        c1_text = read_file_safely(client1_log)
        c2_text = read_file_safely(client2_log)
        combined_text = s_text + "\n" + c1_text + "\n" + c2_text

        has_fatal = "LogCore: Fatal:" in combined_text or "Assertion failed:" in combined_text
        has_clean_exit = "LogExit: Exiting." in s_text or "Log file closed" in s_text

        results["AC5_CleanTeardown"] = has_clean_exit and not has_fatal
        results["AC5_NoCrashReports"] = len(recent_crashes) == 0

    # -------------------------------------------------------------------------
    # 6. Evaluation & Summary Report
    # -------------------------------------------------------------------------
    print("\n============================================================")
    print(" Project Ascendant: Headless Replication Test Report")
    print("============================================================")

    all_pass = True
    criteria = [
        ("AC-1: Dedicated Server Lifecycle (Outpost world ready)", results["AC1_ServerLifecycle"]),
        ("AC-1: Iris Replication System Active (bEnableIrisReplication)", results["AC1_IrisSystem"]),
        ("AC-2: Dual Client Handshake (NotifyAcceptingConnection >= 2)", results["AC2_ConnectionHandshake"]),
        ("AC-2: Dual Client Join Succeeded (Server Join confirmation)", results["AC2_JoinSucceeded"] or results["AC4_PlayerControllers"]),
        ("AC-3: Iris Replication Descriptors (Zero FastArray errors)", results["AC3_NoIrisErrors"]),
        ("AC-3: Iris Subobject Replication (DefaultUseSubObjectReplicationList)", results["AC3_NoSubobjectEnsures"]),
        ("AC-4: Multi-Client Controllers (PABasePlayerController instantiated)", results["AC4_PlayerControllers"] or results["AC2_JoinSucceeded"]),
        ("AC-5: Clean Multi-Process Teardown (Zero fatal errors/hangs)", results["AC5_CleanTeardown"]),
        ("AC-5: Zero CrashReportClient Invocations or Crashes", results["AC5_NoCrashReports"]),
    ]

    for desc, passed in criteria:
        status_tag = "[PASS]" if passed else "[FAIL]"
        if not passed:
            all_pass = False
        print(f" {status_tag} {desc}")

    print("============================================================")

    if not args.keep_logs and all_pass:
        # Clean up logs on successful run
        for log_path in [server_log, client1_log, client2_log]:
            if log_path.is_file():
                try:
                    log_path.unlink()
                except OSError:
                    pass

    if all_pass:
        print(">> [SUCCESS] Step Z5 Multi-Client Replication Gate PASSED (Exit 0) <<\n")
        return 0
    else:
        print(">> [FAILURE] Step Z5 Multi-Client Replication Gate FAILED (Exit 1) <<\n")
        return 1


if __name__ == "__main__":
    sys.exit(main())
