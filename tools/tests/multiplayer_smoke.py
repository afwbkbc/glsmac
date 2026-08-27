#!/usr/bin/env python3

import argparse
import os
from pathlib import Path
import shutil
import socket
import subprocess
import sys
import time


OUTPUT_MARKER = ".glsmac-multiplayer-smoke-output"
PASS_HOST = "MULTIPLAYER_SMOKE_PASS_HOST"
PASS_CLIENT = "MULTIPLAYER_SMOKE_PASS_CLIENT"
FAIL_MARKER = "MULTIPLAYER_SMOKE_FAIL_"
LIFECYCLE_WARNING = "WARNING: connection destroyed while still disconnecting!"
LOBBY_PROBE_READY = "MULTIPLAYER_LOBBY_PROBE_CONNECTED"
BUFFER_SIZE = 65536
MAP_SEED = "2717637413:2797703573:4189968696:1409582894"


def parse_args():
    parser = argparse.ArgumentParser(
        description="Run a bounded GLSMAC host/client real-assets smoke test."
    )
    parser.add_argument("--executable", required=True, type=Path)
    parser.add_argument("--smac-path", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path)
    parser.add_argument("--workdir", type=Path, default=Path.cwd())
    parser.add_argument("--timeout", type=float, default=90.0)
    parser.add_argument("--host-ready-timeout", type=float, default=20.0)
    parser.add_argument("--peer-exit-timeout", type=float, default=10.0)
    parser.add_argument("--probe-timeout", type=float, default=5.0)
    parser.add_argument("--port", type=int, default=4888)
    parser.add_argument("--malformed-probes", action="store_true")
    parser.add_argument("--exercise-reconnect", action="store_true")
    return parser.parse_args()


def prepare_output(path):
    if path.exists():
        marker = path / OUTPUT_MARKER
        if not marker.is_file():
            raise RuntimeError(
                "refusing to replace output directory without smoke-test marker: "
                + str(path)
            )
        shutil.rmtree(path)
    path.mkdir(parents=True)
    (path / OUTPUT_MARKER).write_text("managed by multiplayer_smoke.py\n", encoding="ascii")
    profile_ids = {
        "host": "00000000-0000-4000-8000-000000000001",
        "client": "00000000-0000-4000-8000-000000000002",
        "reconnect": "00000000-0000-4000-8000-000000000003",
    }
    for profile_name, gsid in profile_ids.items():
        profile_path = path / profile_name
        profile_path.mkdir()
        (profile_path / "config.yml").write_text("{}\n", encoding="ascii")
        (profile_path / "accounts.yml").write_text(
            "{0}:\n"
            "  gsid: {0}\n"
            "  last_values:\n"
            '    player_name: ""\n'
            '    game_name: ""\n'
            '    remote_address: ""\n'.format(gsid),
            encoding="ascii",
        )


def require_free_port(port):
    probe = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        probe.bind(("127.0.0.1", port))
    except OSError as exc:
        raise RuntimeError("TCP port {} is not available: {}".format(port, exc)) from exc
    finally:
        probe.close()


def frame(payload):
    return len(payload).to_bytes(4, byteorder=sys.byteorder, signed=True) + payload


def serialized_int_packet(value):
    value_bytes = value.to_bytes(8, byteorder=sys.byteorder, signed=True)
    checksum = 0
    for byte in value_bytes:
        checksum ^= byte
    return (
        bytes([2])
        + len(value_bytes).to_bytes(4, byteorder=sys.byteorder, signed=False)
        + value_bytes
        + bytes([checksum])
    )


def expect_server_disconnect(port, data, timeout):
    deadline = time.monotonic() + timeout
    with socket.create_connection(("127.0.0.1", port), timeout=timeout) as probe:
        probe.settimeout(0.2)
        probe.sendall(data)
        while time.monotonic() < deadline:
            try:
                if probe.recv(4096) == b"":
                    return
            except (ConnectionResetError, ConnectionAbortedError):
                return
            except socket.timeout:
                continue
    raise RuntimeError("server did not reject malformed network probe")


def run_malformed_probes(port, timeout):
    probes = [
        (
            "oversized-frame",
            BUFFER_SIZE.to_bytes(4, byteorder=sys.byteorder, signed=True),
        ),
        ("truncated-packet", frame(b"\x00")),
        ("unknown-packet-type", frame(serialized_int_packet(255))),
        ("unauthenticated-download", frame(serialized_int_packet(14))),
    ]
    for name, data in probes:
        expect_server_disconnect(port, data, timeout)
        print("MalformedProbePass: {}".format(name))


def process_options():
    if os.name != "nt":
        return {}
    startup_info = subprocess.STARTUPINFO()
    startup_info.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    startup_info.wShowWindow = subprocess.SW_HIDE
    return {"startupinfo": startup_info}


def launch(command, workdir, stdout_path, stderr_path):
    stdout_file = stdout_path.open("wb")
    stderr_file = stderr_path.open("wb")
    try:
        process = subprocess.Popen(
            command,
            cwd=workdir,
            stdin=subprocess.DEVNULL,
            stdout=stdout_file,
            stderr=stderr_file,
            shell=False,
            **process_options()
        )
    except Exception:
        stdout_file.close()
        stderr_file.close()
        raise
    return process, stdout_file, stderr_file


def read_log(path):
    if not path.exists():
        return ""
    return path.read_text(encoding="utf-8", errors="replace")


def wait_for_log(process, path, marker, timeout):
    deadline = time.monotonic() + timeout
    while process.poll() is None and time.monotonic() < deadline:
        if marker in read_log(path):
            return True
        time.sleep(0.1)
    return marker in read_log(path)


def wait_for_log_after(process, path, marker, start, timeout):
    deadline = time.monotonic() + timeout
    while process.poll() is None and time.monotonic() < deadline:
        if marker in read_log(path)[start:]:
            return True
        time.sleep(0.1)
    return marker in read_log(path)[start:]


def stop_process(process):
    if process is None or process.poll() is not None:
        return
    process.terminate()
    try:
        process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        process.kill()
        process.wait(timeout=5)


def marker_lines(text):
    return [line for line in text.splitlines() if "MULTIPLAYER_SMOKE_" in line]


def print_tail(label, text, count=40):
    lines = text.splitlines()
    if not lines:
        return
    print("--- {} ---".format(label))
    for line in lines[-count:]:
        print(line)


def run(args):
    workdir = args.workdir.resolve()
    executable = args.executable.resolve()
    smac_path = args.smac_path.resolve()
    output_dir = args.output_dir.resolve()
    if not executable.is_file():
        raise RuntimeError("GLSMAC executable not found: " + str(executable))
    if not smac_path.is_dir():
        raise RuntimeError("SMAC directory not found: " + str(smac_path))
    if not workdir.is_dir():
        raise RuntimeError("working directory not found: " + str(workdir))
    if (
        args.timeout <= 0
        or args.host_ready_timeout <= 0
        or args.peer_exit_timeout <= 0
        or args.probe_timeout <= 0
    ):
        raise RuntimeError("timeouts must be positive")

    require_free_port(args.port)
    prepare_output(output_dir)

    host_stdout = output_dir / "host.stdout.log"
    host_stderr = output_dir / "host.stderr.log"
    client_stdout = output_dir / "client.stdout.log"
    client_stderr = output_dir / "client.stderr.log"
    reconnect_stdout = output_dir / "reconnect.stdout.log"
    reconnect_stderr = output_dir / "reconnect.stderr.log"

    common = [
        "--smacpath", str(smac_path),
        "--port", str(args.port),
        "--skipintro",
        "--nosound",
        "--headless",
        "--windowed",
        "--window-size", "1024x768",
        "--verbose",
    ]
    host_command = [
        str(executable),
        "--prefix", str(output_dir / "host"),
        "--host",
        "--quickstart-seed", MAP_SEED,
        "--gamename", "SmokeTest",
        "--playername", "Host",
        "--mainscript", "../tests/_multiplayer_runtime_smoke",
    ] + common
    client_command = [
        str(executable),
        "--prefix", str(output_dir / "client"),
        "--join", "127.0.0.1",
        "--playername", "Client",
        "--mainscript", "../tests/_multiplayer_runtime_smoke",
    ] + common
    reconnect_command = [
        str(executable),
        "--prefix", str(output_dir / "reconnect"),
        "--join", "127.0.0.1",
        "--playername", "ReconnectProbe",
        "--mainscript", "../tests/_multiplayer_lobby_probe",
    ] + common

    host = None
    client = None
    reconnect = None
    handles = []
    timed_out = False
    peer_exit_timed_out = False
    malformed_probes_pass = not args.malformed_probes
    reconnect_probe_pass = not args.exercise_reconnect
    try:
        host, host_out_handle, host_err_handle = launch(
            host_command, workdir, host_stdout, host_stderr
        )
        handles.extend([host_out_handle, host_err_handle])
        if not wait_for_log(
            host, host_stdout, "Server started", args.host_ready_timeout
        ):
            raise RuntimeError(
                "host did not reach listening state (exit={})".format(host.poll())
            )

        if args.malformed_probes:
            run_malformed_probes(args.port, args.probe_timeout)
            malformed_probes_pass = True

        if args.exercise_reconnect:
            reconnect, reconnect_out_handle, reconnect_err_handle = launch(
                reconnect_command, workdir, reconnect_stdout, reconnect_stderr
            )
            handles.extend([reconnect_out_handle, reconnect_err_handle])
            if not wait_for_log(
                reconnect,
                reconnect_stdout,
                LOBBY_PROBE_READY,
                args.host_ready_timeout,
            ):
                raise RuntimeError("reconnect probe did not authenticate in the lobby")
            host_log_start = len(read_log(host_stdout))
            stop_process(reconnect)
            if not wait_for_log_after(
                host,
                host_stdout,
                " disconnected",
                host_log_start,
                args.probe_timeout,
            ):
                raise RuntimeError("host did not process the reconnect probe disconnect")
            reconnect_probe_pass = True

        client, client_out_handle, client_err_handle = launch(
            client_command, workdir, client_stdout, client_stderr
        )
        handles.extend([client_out_handle, client_err_handle])

        deadline = time.monotonic() + args.timeout
        first_exit_at = None
        while host.poll() is None or client.poll() is None:
            now = time.monotonic()
            if now >= deadline:
                timed_out = True
                break
            exactly_one_exited = (host.poll() is None) != (client.poll() is None)
            if exactly_one_exited:
                if first_exit_at is None:
                    first_exit_at = now
                elif now - first_exit_at >= args.peer_exit_timeout:
                    peer_exit_timed_out = True
                    break
            else:
                first_exit_at = None
            time.sleep(0.1)
    finally:
        stop_process(client)
        stop_process(reconnect)
        stop_process(host)
        for handle in handles:
            handle.close()

    host_text = read_log(host_stdout)
    client_text = read_log(client_stdout)
    host_error_text = read_log(host_stderr)
    client_error_text = read_log(client_stderr)
    host_exit = None if host is None else host.returncode
    client_exit = None if client is None else client.returncode
    host_pass = PASS_HOST in host_text
    client_pass = PASS_CLIENT in client_text
    explicit_failure = FAIL_MARKER in host_text or FAIL_MARKER in client_text
    lifecycle_warning = LIFECYCLE_WARNING in host_text or LIFECYCLE_WARNING in client_text
    sanitizer_error = any(
        "AddressSanitizer" in text
        for text in (host_text, client_text, host_error_text, client_error_text)
    )

    print("TimedOut: {}".format(timed_out))
    print("PeerExitTimedOut: {}".format(peer_exit_timed_out))
    print("HostExit: {}".format(host_exit))
    print("ClientExit: {}".format(client_exit))
    print("HostPass: {}".format(host_pass))
    print("ClientPass: {}".format(client_pass))
    print("MalformedProbesPass: {}".format(malformed_probes_pass))
    print("ReconnectProbePass: {}".format(reconnect_probe_pass))
    print("LifecycleWarning: {}".format(lifecycle_warning))
    print("Output: {}".format(output_dir))
    print("--- HOST MARKERS ---")
    for line in marker_lines(host_text):
        print(line)
    print("--- CLIENT MARKERS ---")
    for line in marker_lines(client_text):
        print(line)

    success = (
        not timed_out
        and not peer_exit_timed_out
        and host_exit == 0
        and client_exit == 0
        and host_pass
        and client_pass
        and malformed_probes_pass
        and reconnect_probe_pass
        and not explicit_failure
        and not lifecycle_warning
        and not sanitizer_error
    )
    if not success:
        print_tail("HOST STDERR", host_error_text)
        print_tail("CLIENT STDERR", client_error_text)
    return 0 if success else 1


def main():
    try:
        return run(parse_args())
    except Exception as exc:
        print("multiplayer smoke failed: {}".format(exc), file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
