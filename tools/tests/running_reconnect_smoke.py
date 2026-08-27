#!/usr/bin/env python3

import argparse
from pathlib import Path
import sys
import time

from multiplayer_smoke import (
    LIFECYCLE_WARNING,
    MAP_SEED,
    launch,
    prepare_output,
    print_tail,
    read_log,
    require_free_port,
    stop_process,
    wait_for_log,
    wait_for_log_after,
)


DROP_READY = "RUNNING_RECONNECT_DROP_READY"
RESUMED = "RUNNING_RECONNECT_RESUMED_CLIENT"
PASS_HOST = "RUNNING_RECONNECT_PASS_HOST"
PASS_CLIENT = "RUNNING_RECONNECT_PASS_CLIENT"
FAIL_MARKER = "RUNNING_RECONNECT_FAIL_"


def parse_args():
    parser = argparse.ArgumentParser(
        description="Run a bounded in-game disconnect/reconnect smoke test."
    )
    parser.add_argument("--executable", required=True, type=Path)
    parser.add_argument("--smac-path", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path)
    parser.add_argument("--workdir", type=Path, default=Path.cwd())
    parser.add_argument("--timeout", type=float, default=90.0)
    parser.add_argument("--phase-timeout", type=float, default=60.0)
    parser.add_argument("--peer-exit-timeout", type=float, default=10.0)
    parser.add_argument("--port", type=int, default=4888)
    return parser.parse_args()


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
    if args.timeout <= 0 or args.phase_timeout <= 0 or args.peer_exit_timeout <= 0:
        raise RuntimeError("timeouts must be positive")

    require_free_port(args.port)
    prepare_output(output_dir)

    host_stdout = output_dir / "host.stdout.log"
    host_stderr = output_dir / "host.stderr.log"
    initial_stdout = output_dir / "initial-client.stdout.log"
    initial_stderr = output_dir / "initial-client.stderr.log"
    resumed_stdout = output_dir / "resumed-client.stdout.log"
    resumed_stderr = output_dir / "resumed-client.stderr.log"

    common = [
        "--smacpath", str(smac_path),
        "--port", str(args.port),
        "--skipintro",
        "--nosound",
        "--windowed",
        "--window-size", "1024x768",
        "--verbose",
    ]
    host_command = [
        str(executable),
        "--prefix", str(output_dir / "host"),
        "--host",
        "--quickstart-seed", MAP_SEED,
        "--gamename", "ReconnectSmoke",
        "--playername", "Host",
        "--mainscript", "../tests/_multiplayer_running_reconnect",
    ] + common
    initial_command = [
        str(executable),
        "--prefix", str(output_dir / "client"),
        "--join", "127.0.0.1",
        "--playername", "Client",
        "--mainscript", "../tests/_multiplayer_running_reconnect",
    ] + common
    resumed_command = [
        str(executable),
        "--prefix", str(output_dir / "client"),
        "--join", "127.0.0.1",
        "--playername", "Client",
        "--mainscript", "../tests/_multiplayer_running_reconnect_resume",
    ] + common

    host = None
    initial_client = None
    resumed_client = None
    handles = []
    timed_out = False
    peer_exit_timed_out = False
    disconnect_observed = False
    try:
        host, host_out_handle, host_err_handle = launch(
            host_command, workdir, host_stdout, host_stderr
        )
        handles.extend([host_out_handle, host_err_handle])
        if not wait_for_log(host, host_stdout, "Server started", args.phase_timeout):
            raise RuntimeError(
                "host did not reach listening state (exit={})".format(
                    host.poll()
                )
            )

        initial_client, initial_out_handle, initial_err_handle = launch(
            initial_command, workdir, initial_stdout, initial_stderr
        )
        handles.extend([initial_out_handle, initial_err_handle])
        if not wait_for_log(
            initial_client, initial_stdout, DROP_READY, args.phase_timeout
        ):
            raise RuntimeError(
                "initial client did not complete colony founding (exit={})".format(
                    initial_client.poll()
                )
            )

        host_log_start = len(read_log(host_stdout))
        stop_process(initial_client)
        disconnect_observed = wait_for_log_after(
            host,
            host_stdout,
            " disconnected",
            host_log_start,
            args.phase_timeout,
        )
        if not disconnect_observed:
            raise RuntimeError("host did not process the in-game client disconnect")

        resumed_client, resumed_out_handle, resumed_err_handle = launch(
            resumed_command, workdir, resumed_stdout, resumed_stderr
        )
        handles.extend([resumed_out_handle, resumed_err_handle])

        deadline = time.monotonic() + args.timeout
        first_exit_at = None
        while host.poll() is None or resumed_client.poll() is None:
            now = time.monotonic()
            if now >= deadline:
                timed_out = True
                break
            exactly_one_exited = (host.poll() is None) != (
                resumed_client.poll() is None
            )
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
        stop_process(resumed_client)
        stop_process(initial_client)
        stop_process(host)
        for handle in handles:
            handle.close()

    host_text = read_log(host_stdout)
    initial_text = read_log(initial_stdout)
    resumed_text = read_log(resumed_stdout)
    host_error_text = read_log(host_stderr)
    initial_error_text = read_log(initial_stderr)
    resumed_error_text = read_log(resumed_stderr)
    host_exit = None if host is None else host.returncode
    resumed_exit = None if resumed_client is None else resumed_client.returncode
    host_pass = PASS_HOST in host_text
    client_pass = PASS_CLIENT in resumed_text
    resumed = RESUMED in resumed_text
    explicit_failure = (
        FAIL_MARKER in host_text
        or FAIL_MARKER in initial_text
        or FAIL_MARKER in resumed_text
    )
    lifecycle_warning = LIFECYCLE_WARNING in host_text or LIFECYCLE_WARNING in resumed_text
    sanitizer_error = any(
        "AddressSanitizer" in text
        for text in (
            host_text,
            initial_text,
            resumed_text,
            host_error_text,
            initial_error_text,
            resumed_error_text,
        )
    )

    print("TimedOut: {}".format(timed_out))
    print("PeerExitTimedOut: {}".format(peer_exit_timed_out))
    print("DisconnectObserved: {}".format(disconnect_observed))
    print("ResumedClientInitialized: {}".format(resumed))
    print("HostExit: {}".format(host_exit))
    print("ResumedClientExit: {}".format(resumed_exit))
    print("HostPass: {}".format(host_pass))
    print("ClientPass: {}".format(client_pass))
    print("LifecycleWarning: {}".format(lifecycle_warning))
    print("Output: {}".format(output_dir))

    success = (
        not timed_out
        and not peer_exit_timed_out
        and disconnect_observed
        and resumed
        and host_exit == 0
        and resumed_exit == 0
        and host_pass
        and client_pass
        and not explicit_failure
        and not lifecycle_warning
        and not sanitizer_error
    )
    if not success:
        print_tail("HOST STDOUT", host_text)
        print_tail("INITIAL CLIENT STDOUT", initial_text)
        print_tail("RESUMED CLIENT STDOUT", resumed_text)
        print_tail("HOST STDERR", host_error_text)
        print_tail("INITIAL CLIENT STDERR", initial_error_text)
        print_tail("RESUMED CLIENT STDERR", resumed_error_text)
    return 0 if success else 1


def main():
    try:
        return run(parse_args())
    except Exception as exc:
        print("running reconnect smoke failed: {}".format(exc), file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
