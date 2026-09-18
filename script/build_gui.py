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

LANGUAGE = "en"
TEXT = {
    "Debug / no optimization (-O0)": "调试 / 不优化 (-O0)",
    "Release / optimize for size (-Os)": "发布 / 优化代码大小 (-Os)",
    "Incremental build (keep existing objects)": "增量编译（保留已有中间文件）",
    "Full build (clean selected target first)": "全量编译（先清理所选目标）",
    "Do not generate an OZone debug project": "不生成 OZone 调试工程",
    "Generate an OZone debug project after a successful build": "编译成功后生成 OZone 调试工程",
    "Use Up/Down to move, Enter to confirm, Esc to cancel.": "使用方向键上下移动，Enter 确认，Esc 取消。",
    "default": "默认",
    "Select": "请选择",
    "Please enter a number from {first} to {last}.": "请输入 {first} 到 {last} 之间的数字。",
    "No": "否",
    "Yes": "是",
    "Please enter y or n.": "请输入 y 或 n。",
    "Build target": "编译目标",
    "Optimization": "优化等级",
    "Compile unit tests?": "是否编译单元测试？",
    "Build mode": "编译模式",
    "OZone project": "OZone 调试工程",
    "Build selection cancelled.": "已取消编译选择。",
    "Build configuration": "编译配置",
    "Target": "目标",
    "Unit tests": "单元测试",
    "Clean command": "清理命令",
    "Build command": "编译命令",
    "yes": "是",
    "no": "否",
    "Output": "输出目录",
    "Build was not started.": "未开始编译。",
    "Cleaning build/{target} before full build...": "全量编译前正在清理 build/{target}...",
    "Clean failed with exit code {code}; build was not started.": "清理失败，退出码 {code}；未开始编译。",
    "Starting build...": "开始编译...",
    "Build succeeded, but OZone project generation failed.": "编译成功，但 OZone 工程生成失败。",
    "Build result": "编译结果",
    "Status": "状态",
    "Exit code": "退出码",
    "Directory": "目录",
    "Artifacts": "产物",
    "none": "无",
    "Build log": "编译日志",
    "Build completed.": "编译完成。",
    "Build failed with exit code {code}.": "编译失败，退出码 {code}。",
    "SUCCESS": "成功",
    "FAILED": "失败",
    "No non-empty src/target/*/target.mk files were found.": "未找到非空的 src/target/*/target.mk 文件。",
}


def tr(text: str, **values: object) -> str:
    translated = TEXT.get(text, text) if LANGUAGE == "zh" else text
    return translated.format(**values) if values else translated


def optimization_options() -> tuple[tuple[str, str], ...]:
    return ((tr("Debug / no optimization (-O0)"), "1"), (tr("Release / optimize for size (-Os)"), "0"))


def build_mode_options() -> tuple[tuple[str, bool], ...]:
    return ((tr("Incremental build (keep existing objects)"), False), (tr("Full build (clean selected target first)"), True))


def ozone_options() -> tuple[tuple[str, bool], ...]:
    return ((tr("Do not generate an OZone debug project"), False), (tr("Generate an OZone debug project after a successful build"), True))

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
    print(tr("Use Up/Down to move, Enter to confirm, Esc to cancel."))

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
            default = f" [{tr('default')}]" if index - 1 == default_index else ""
            print(f"  {index}. {option}{default}")
        raw = input(f"{tr('Select')} [{tr('default')} {default_index + 1}]: ").strip()
        if not raw:
            return default_index
        try:
            selected = int(raw) - 1
        except ValueError:
            selected = -1
        if 0 <= selected < len(options):
            return selected
        print(tr("Please enter a number from {first} to {last}.", first=1, last=len(options)))


def choose(title: str, options: list[str], default_index: int = 0) -> int:
    if not options:
        raise ValueError(f"no choices available for {title}")
    if sys.stdin.isatty() and sys.stdout.isatty():
        return choose_with_arrows(title, options, default_index)
    return choose_numbered(title, options, default_index)


def choose_yes_no(title: str, default: bool = False) -> bool:
    if sys.stdin.isatty() and sys.stdout.isatty():
        default_index = 1 if default else 0
        return choose(title, [tr("No"), tr("Yes")], default_index) == 1
    suffix = "Y/n" if default else "y/N"
    while True:
        raw = input(f"{title} [{suffix}]: ").strip().lower()
        if not raw:
            return default
        if raw in {"y", "yes"}:
            return True
        if raw in {"n", "no"}:
            return False
        print(tr("Please enter y or n."))


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
    print(f"\n{tr('Build result')}")
    print(f"  {tr('Status'):<12}: {tr('SUCCESS') if returncode == 0 else tr('FAILED')}")
    print(f"  {tr('Exit code'):<12}: {returncode}")
    print(f"  {tr('Directory'):<12}: {build_dir}")

    artifacts = [
        build_dir / f"{target}.elf",
        build_dir / f"{target}.bin",
        build_dir / f"{target}.hex",
        build_dir / f"{target}.map",
    ]
    existing = [artifact for artifact in artifacts if artifact.is_file()]
    if existing:
        print(f"  {tr('Artifacts'):<12}:")
        for artifact in existing:
            print(f"    - {artifact}")
    else:
        print(f"  {tr('Artifacts'):<12}: {tr('none')}")

    build_log = build_dir / ".build.log"
    if build_log.is_file():
        print(f"  {tr('Build log'):<12}: {build_log}")


def main() -> int:
    global LANGUAGE
    parser = argparse.ArgumentParser(description="Choose and build a NanoPlatform target")
    parser.add_argument("--distro", help="WSL distribution to use on Windows")
    parser.add_argument("--dry-run", action="store_true", help="show the selected build command without executing it")
    parser.add_argument("--language", choices=("en", "zh"), default="en", help="interface language (default: en)")
    args = parser.parse_args()
    LANGUAGE = args.language

    repo = Path(__file__).resolve().parent.parent
    targets = discover_targets(repo)
    if not targets:
        print(f"[ERROR] {tr('No non-empty src/target/*/target.mk files were found.')}", file=sys.stderr)
        return 1

    try:
        optimizations = optimization_options()
        build_modes = build_mode_options()
        ozone_choices = ozone_options()
        target = targets[choose(tr("Build target"), targets)]
        optimization_index = choose(tr("Optimization"), [item[0] for item in optimizations], default_index=1)
        optimization_label, debug = optimizations[optimization_index]
        unit_test = choose_yes_no(tr("Compile unit tests?"), default=False)
        build_mode_index = choose(tr("Build mode"), [item[0] for item in build_modes], default_index=0)
        build_mode_label, full_build = build_modes[build_mode_index]
        ozone_index = choose(tr("OZone project"), [item[0] for item in ozone_choices], default_index=0)
        ozone_label, generate_ozone = ozone_choices[ozone_index]
        command = make_command(repo, target, debug, unit_test, args.distro)
        clean_command = make_command(repo, target, debug, unit_test, args.distro, clean=True) if full_build else None
    except (EOFError, KeyboardInterrupt):
        print(f"\n[CANCELLED] {tr('Build selection cancelled.')}")
        return 130
    except (ValueError, RuntimeError) as error:
        print(f"[ERROR] {error}", file=sys.stderr)
        return 1

    print(f"\n{tr('Build configuration')}")
    print(f"  {tr('Target'):<12}: {target}")
    print(f"  {tr('Optimization'):<12}: {optimization_label}")
    print(f"  {tr('Unit tests'):<12}: {tr('yes') if unit_test else tr('no')}")
    print(f"  {tr('Build mode'):<12}: {build_mode_label}")
    print(f"  {tr('OZone project'):<12}: {ozone_label}")
    if clean_command:
        print(f"  {tr('Clean command')}: " + subprocess.list2cmdline(clean_command))
    print(f"  {tr('Build command')}: " + subprocess.list2cmdline(command))

    if args.dry_run:
        print(f"  {tr('Output'):<12}: {repo / 'build' / target}")
        print(f"[DRY RUN] {tr('Build was not started.')}")
        return 0

    if clean_command:
        print(f"\n[INFO] {tr('Cleaning build/{target} before full build...', target=target)}\n")
        clean_result = subprocess.run(clean_command, cwd=repo, check=False)
        if clean_result.returncode != 0:
            print_build_result(repo, target, clean_result.returncode)
            print(f"[ERROR] {tr('Clean failed with exit code {code}; build was not started.', code=clean_result.returncode)}", file=sys.stderr)
            return clean_result.returncode

    print(f"\n[INFO] {tr('Starting build...')}\n")
    result = subprocess.run(command, cwd=repo, check=False)
    ozone_project: Path | None = None
    if result.returncode == 0 and generate_ozone:
        generator = repo / "script" / "generate_ozone_project.py"
        generator_command = [sys.executable, str(generator), "--target", target]
        generator_command += ["--language", args.language]
        if args.distro:
            generator_command += ["--distro", args.distro]
        ozone_result = subprocess.run(
            generator_command,
            cwd=repo,
            check=False,
        )
        if ozone_result.returncode != 0:
            print_build_result(repo, target, result.returncode)
            print(f"[ERROR] {tr('Build succeeded, but OZone project generation failed.')}", file=sys.stderr)
            return ozone_result.returncode
        ozone_project = repo / "build" / target / f"{target}.jdebug"
    print_build_result(repo, target, result.returncode)
    if result.returncode == 0:
        print(f"[SUCCESS] {tr('Build completed.')}")
        if ozone_project:
            print(f"[SUCCESS] OZone project: {ozone_project}")
    else:
        print(f"[ERROR] {tr('Build failed with exit code {code}.', code=result.returncode)}", file=sys.stderr)
    return result.returncode


if __name__ == "__main__":
    raise SystemExit(main())
