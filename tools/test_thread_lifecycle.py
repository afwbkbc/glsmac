#!/usr/bin/env python3
"""Compile and run actual Thread/Engine lifecycle code with fake game services.

Requires a C++17 compiler with pthread support; run on Linux/macOS or WSL.
No game assets, SDL, or installed game services are used by this harness.
"""

import argparse
import os
from pathlib import Path
import shlex
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", default=os.environ.get("CXX", "c++"))
    parser.add_argument("--sanitizer", action="store_true", help="Enable AddressSanitizer and UBSan")
    args = parser.parse_args()
    root = Path(__file__).resolve().parent.parent
    service_headers = (
        "config/Config.h", "error_handler/ErrorHandler.h", "logger/Logger.h",
        "resource/ResourceManager.h", "loader/font/FontLoader.h",
        "loader/texture/TextureLoader.h", "loader/sound/SoundLoader.h",
        "loader/txt/TXTLoaders.h", "scheduler/Scheduler.h", "input/Input.h",
        "graphics/Graphics.h", "audio/Audio.h", "network/Network.h",
        "game/backend/Game.h", "gc/GC.h",
    )
    with tempfile.TemporaryDirectory(prefix="glsmac-lifecycle-") as temporary:
        temporary = Path(temporary)
        for relative in service_headers:
            header = temporary / relative
            header.parent.mkdir(parents=True, exist_ok=True)
            header.write_text('#include "lifecycle_services.h"\n', encoding="utf-8")
        executable = temporary / "thread-lifecycle"
        command = shlex.split(args.compiler) + [
            "-std=c++17", "-pthread", "-DFASTDEBUG", "-g", "-O1",
            "-I" + str(temporary), "-I" + str(root / "tools/tests"), "-I" + str(root / "src"),
            str(root / "src/common/Thread.cpp"), str(root / "src/common/Common.cpp"),
            str(root / "src/engine/Engine.cpp"), str(root / "tools/tests/thread_lifecycle.cpp"),
            "-o", str(executable),
        ]
        if args.sanitizer:
            command += ["-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
        subprocess.run(command, check=True, timeout=60)
        subprocess.run([str(executable)], check=True, timeout=20)


if __name__ == "__main__":
    main()
