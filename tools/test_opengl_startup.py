#!/usr/bin/env python3
"""Test real OpenGL startup using an existing Linux/Ninja Release build.

Configure with CMAKE_EXPORT_COMPILE_COMMANDS=ON, then build GLSMAC first.
The runner compiles a test main and substitutes it for main.cpp.o when linking
a temporary executable. Game objects and driver libraries retain their original
compile/link flags; no build output is replaced. ASan/UBSan flags are inherited
when included in the Release compile/link flags. SDL/GLEW entry points in the test main
inject failures without a display or proprietary game assets.
"""

import argparse
import json
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, required=True)
    args = parser.parse_args()
    if sys.platform != "linux":
        parser.error("This driver-substitution harness requires Linux and a Ninja build")
    build = args.build_dir.resolve()
    root = Path(__file__).resolve().parent.parent
    commands = json.loads((build / "compile_commands.json").read_text())
    main_command = next(item for item in commands if Path(item["file"]).resolve() == root / "src/main.cpp")
    compile_args = shlex.split(main_command["command"])
    if any(flag in compile_args for flag in ("-DDEBUG=1", "-DFASTDEBUG=1")):
        parser.error("Use a Release build so Debug's driver wrappers do not replace the injected SDL/GLEW calls")
    links = subprocess.check_output(["ninja", "-t", "commands", "GLSMAC"], cwd=build, text=True).splitlines()
    link_args = shlex.split(next(line for line in reversed(links) if " -o bin/GLSMAC " in line))
    if link_args[:2] == [":", "&&"]:
        link_args = link_args[2:]
    if link_args[-2:] == ["&&", ":"]:
        link_args = link_args[:-2]
    if any(arg in {"&&", ";", "|"} for arg in link_args):
        parser.error("Unsupported Ninja link command")
    with tempfile.TemporaryDirectory(prefix="glsmac-opengl-startup-") as temporary:
        temporary = Path(temporary)
        test_object = temporary / "opengl-startup.o"
        executable = temporary / "opengl-startup"
        original_object = compile_args[compile_args.index("-o") + 1]
        compile_args[compile_args.index("-o") + 1] = str(test_object)
        compile_args[compile_args.index("-c") + 1] = str(root / "tools/tests/opengl_startup.cpp")
        subprocess.run(compile_args, cwd=main_command["directory"], check=True, timeout=60)
        link_args[link_args.index(original_object)] = str(test_object)
        link_args[link_args.index("-o") + 1] = str(executable)
        subprocess.run(link_args, cwd=build, check=True, timeout=60)
        subprocess.run([str(executable)], check=True, timeout=30)


if __name__ == "__main__":
    main()
