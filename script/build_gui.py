#!/usr/bin/env python3
"""Interactive terminal build selector for NanoPlatform."""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

from check_build_environment import decode_output, list_wsl_distros


OPTIMIZATIONS = (
    ("Debug / no optimization (-O0)", "1"),
    ("Release / optimize for size (-Os)", "0"),
)

BUILD_MODES = (
    ("Incremental build (keep existing objects)", False),
    ("Full build (clean selected target first)", True),
)

OZONE_OPTIONS = (
    ("Do not generate an OZone debug project", False),
    ("Generate an OZone debug project after a successful build", True),
)

RESET = "\033[0m"
HIGHLIGHT = "\033[7m"
CLEAR_LINE = "\033[2K"


def enable_virtual_terminal() -> None:
    if os.name != "nt":
        return
    try:
        import ctypes

        kernel32 = ctypes.windll.kernel32
        stdout_handle = kernel32.GetStdHandle(-11)
        mode = ctypes.c_uint()
        if kernel32.GetConsoleMode(stdout_handle, ctypes.byref(mode)):
            kernel32.SetConsoleMode(stdout_handle, mode.value | 0x0004)
    except (AttributeError, OSError):
        pass


def discover_targets(repo: Path) -> list[str]:
    targets_dir = repo / "src" / "target"
    return sorted(
        target_mk.parent.name
        for target_mk in targets_dir.glob("*/target.mk")
        if target_mk.is_file() and target_mk.stat().st_size > 0
    )


def read_menu_key() -> str:
    if os.name == "nt":
        import msvcrt

        key = msvcrt.getwch()
        if key in {"\x00", "\xe0"}:
            return {"H": "up", "P": "down"}.get(msvcrt.getwch(), "other")
        return {"\r": "enter", "\x1b": "escape"}.get(key, "other")

    import termios
    import tty

    descriptor = sys.stdin.fileno()
    previous = termios.tcgetattr(descriptor)
    try:
        tty.setraw(descriptor)
        key = sys.stdin.read(1)
        if key == "\x1b":
            sequence = sys.stdin.read(2)
            return {"[A": "up", "[B": "down"}.get(sequence, "escape")
        return {"\r": "enter", "\n": "enter"}.get(key, "other")
    finally:
        termios.tcsetattr(descriptor, termios.TCSADRAIN, previous)


def choose_with_arrows(title: str, options: list[str], default_index: int) -> int:
    enable_virtual_terminal()
    selected = default_index
    print(f"\n{title}")
    print("Use Up/Down to move, Enter to confirm, Esc to cancel.")

    def render(move_up: bool) -> None:
        if move_up:
            sys.stdout.write(f"\033[{len(options)}F")
        for index, option in enumerate(options):
            if index == selected:
                sys.stdout.write(f"{CLEAR_LINE}{HIGHLIGHT}> {option}{RESET}\n")
            else:
                sys.stdout.write(f"{CLEAR_LINE}  {option}\n")
        sys.stdout.flush()

    render(move_up=False)
    while True:
        key = read_menu_key()
        if key == "up":
            selected = (selected - 1) % len(options)
            render(move_up=True)
        elif key == "down":
            selected = (selected + 1) % len(options)
            render(move_up=True)
        elif key == "enter":
            return selected
        elif key == "escape":
            raise KeyboardInterrupt


def choose_numbered(title: str, options: list[str], default_index: int) -> int:
    while True:
        print(f"\n{title}")
        for index, option in enumerate(options, 1):
            default = " [default]" if index - 1 == default_index else ""
            print(f"  {index}. {option}{default}")
        raw = input(f"Select [default {default_index + 1}]: ").strip()
        if not raw:
            return default_index
        try:
            selected = int(raw) - 1
        except ValueError:
            selected = -1
        if 0 <= selected < len(options):
            return selected
        print(f"Please enter a number from 1 to {len(options)}.")


def choose(title: str, options: list[str], default_index: int = 0) -> int:
    if not options:
        raise ValueError(f"no choices available for {title}")
    if sys.stdin.isatty() and sys.stdout.isatty():
        return choose_with_arrows(title, options, default_index)
    return choose_numbered(title, options, default_index)


def choose_yes_no(title: str, default: bool = False) -> bool:
    if sys.stdin.isatty() and sys.stdout.isatty():
        default_index = 1 if default else 0
        return choose(title, ["No", "Yes"], default_index) == 1
    suffix = "Y/n" if default else "y/N"
    while True:
        raw = input(f"{title} [{suffix}]: ").strip().lower()
        if not raw:
            return default
        if raw in {"y", "yes"}:
            return True
        if raw in {"n", "no"}:
            return False
        print("Please enter y or n.")


def wsl_repo_path(wsl: str, distro: str, repo: Path) -> str:
    windows_path = repo.as_posix()
    result = subprocess.run(
        [wsl, "-d", distro, "--", "wslpath", "-a", "-u", windows_path],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    lines = [line.strip() for line in decode_output(result.stdout).splitlines() if line.strip()]
    if result.returncode != 0 or not lines:
        detail = decode_output(result.stderr).strip() or "wslpath returned no path"
        raise RuntimeError(f"failed to convert repository path for WSL: {detail}")
    return lines[-1]


def make_command(
    repo: Path,
    target: str,
    debug: str,
    unit_test: bool,
    distro: str | None,
    clean: bool = False,
) -> list[str]:
    if clean:
        make_args = [f"TARGET={target}", "clean"]
    else:
        make_args = ["-j", "8", f"TARGET={target}", f"DEBUG={debug}", f"UNIT_TEST={1 if unit_test else 0}"]
    if os.name == "nt" or sys.platform.startswith("win"):
        wsl = shutil.which("wsl.exe") or shutil.which("wsl")
        if not wsl:
            raise RuntimeError("wsl.exe was not found; run $develop-environment")
        distros, error = list_wsl_distros(wsl)
        if not distros:
            raise RuntimeError(error or "no WSL distribution is installed; run $develop-environment")
        selected_distro = distro or distros[0]
        if selected_distro not in distros:
            raise RuntimeError(f"WSL distribution is not installed: {selected_distro}")
        linux_repo = wsl_repo_path(wsl, selected_distro, repo)
        return [wsl, "-d", selected_distro, "--cd", linux_repo, "--", "make", *make_args]
    return ["make", *make_args]


def print_build_result(repo: Path, target: str, returncode: int) -> None:
    build_dir = repo / "build" / target
    print("\nBuild result")
    print(f"  Status    : {'SUCCESS' if returncode == 0 else 'FAILED'}")
    print(f"  Exit code : {returncode}")
    print(f"  Directory : {build_dir}")

    artifacts = [
        build_dir / f"{target}.elf",
        build_dir / f"{target}.bin",
        build_dir / f"{target}.hex",
        build_dir / f"{target}.map",
    ]
    existing = [artifact for artifact in artifacts if artifact.is_file()]
    if existing:
        print("  Artifacts :")
        for artifact in existing:
            print(f"    - {artifact}")
    else:
        print("  Artifacts : none")

    build_log = build_dir / ".build.log"
    if build_log.is_file():
        print(f"  Build log : {build_log}")


def main() -> int:
    parser = argparse.ArgumentParser(description="Choose and build a NanoPlatform target")
    parser.add_argument("--distro", help="WSL distribution to use on Windows")
    parser.add_argument("--dry-run", action="store_true", help="show the selected build command without executing it")
    args = parser.parse_args()

    repo = Path(__file__).resolve().parent.parent
    targets = discover_targets(repo)
    if not targets:
        print("[ERROR] No non-empty src/target/*/target.mk files were found.", file=sys.stderr)
        return 1

    try:
        target = targets[choose("Build target", targets)]
        optimization_index = choose("Optimization", [item[0] for item in OPTIMIZATIONS], default_index=1)
        optimization_label, debug = OPTIMIZATIONS[optimization_index]
        unit_test = choose_yes_no("Compile unit tests?", default=False)
        build_mode_index = choose("Build mode", [item[0] for item in BUILD_MODES], default_index=0)
        build_mode_label, full_build = BUILD_MODES[build_mode_index]
        ozone_index = choose("OZone project", [item[0] for item in OZONE_OPTIONS], default_index=0)
        ozone_label, generate_ozone = OZONE_OPTIONS[ozone_index]
        command = make_command(repo, target, debug, unit_test, args.distro)
        clean_command = make_command(repo, target, debug, unit_test, args.distro, clean=True) if full_build else None
    except (EOFError, KeyboardInterrupt):
        print("\n[CANCELLED] Build selection cancelled.")
        return 130
    except (ValueError, RuntimeError) as error:
        print(f"[ERROR] {error}", file=sys.stderr)
        return 1

    print("\nBuild configuration")
    print(f"  Target       : {target}")
    print(f"  Optimization : {optimization_label}")
    print(f"  Unit tests   : {'yes' if unit_test else 'no'}")
    print(f"  Build mode   : {build_mode_label}")
    print(f"  OZone project: {ozone_label}")
    if clean_command:
        print("  Clean command: " + subprocess.list2cmdline(clean_command))
    print("  Build command: " + subprocess.list2cmdline(command))

    if args.dry_run:
        print(f"  Output       : {repo / 'build' / target}")
        print("[DRY RUN] Build was not started.")
        return 0

    if clean_command:
        print(f"\n[INFO] Cleaning build/{target} before full build...\n")
        clean_result = subprocess.run(clean_command, cwd=repo, check=False)
        if clean_result.returncode != 0:
            print_build_result(repo, target, clean_result.returncode)
            print(f"[ERROR] Clean failed with exit code {clean_result.returncode}; build was not started.", file=sys.stderr)
            return clean_result.returncode

    print("\n[INFO] Starting build...\n")
    result = subprocess.run(command, cwd=repo, check=False)
    ozone_project: Path | None = None
    if result.returncode == 0 and generate_ozone:
        generator = repo / "script" / "generate_ozone_project.py"
        generator_command = [sys.executable, str(generator), "--target", target]
        if args.distro:
            generator_command += ["--distro", args.distro]
        ozone_result = subprocess.run(
            generator_command,
            cwd=repo,
            check=False,
        )
        if ozone_result.returncode != 0:
            print_build_result(repo, target, result.returncode)
            print("[ERROR] Build succeeded, but OZone project generation failed.", file=sys.stderr)
            return ozone_result.returncode
        ozone_project = repo / "build" / target / f"{target}.jdebug"
    print_build_result(repo, target, result.returncode)
    if result.returncode == 0:
        print("[SUCCESS] Build completed.")
        if ozone_project:
            print(f"[SUCCESS] OZone project: {ozone_project}")
    else:
        print(f"[ERROR] Build failed with exit code {result.returncode}.", file=sys.stderr)
    return result.returncode


if __name__ == "__main__":
    raise SystemExit(main())
