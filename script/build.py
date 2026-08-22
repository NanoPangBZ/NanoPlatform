#!/usr/bin/env python3
"""Cross-platform entry point for the NanoPlatform build workflow."""

from __future__ import annotations

import argparse
import locale
import os
import subprocess
import sys
from pathlib import Path


def detect_language() -> str:
    """Use Chinese only when the host UI language is Chinese; otherwise English."""
    language = ""
    if os.name == "nt":
        try:
            import ctypes

            buffer = ctypes.create_unicode_buffer(85)
            if ctypes.windll.kernel32.GetUserDefaultLocaleName(buffer, len(buffer)):
                language = buffer.value
        except (AttributeError, OSError):
            pass
    if not language:
        language = locale.getlocale()[0] or os.environ.get("LANG", "")
    return "zh" if language.lower().startswith("zh") else "en"


def text(language: str, english: str, chinese: str) -> str:
    return chinese if language == "zh" else english


def configure_console(language: str) -> None:
    """Use UTF-8 for Chinese output without putting localized text in batch files."""
    if os.name != "nt" or language != "zh":
        return
    try:
        import ctypes

        ctypes.windll.kernel32.SetConsoleCP(65001)
        ctypes.windll.kernel32.SetConsoleOutputCP(65001)
        sys.stdin.reconfigure(encoding="utf-8")
        sys.stdout.reconfigure(encoding="utf-8")
        sys.stderr.reconfigure(encoding="utf-8")
    except (AttributeError, OSError):
        pass


def run_script(script: Path, arguments: list[str], repo: Path) -> int:
    return subprocess.run([sys.executable, str(script), *arguments], cwd=repo, check=False).returncode


def pause(language: str) -> None:
    if not (sys.stdin.isatty() and sys.stdout.isatty()):
        return
    prompt = text(language, "Press Enter to close...", "按 Enter 键关闭...")
    try:
        input(f"\n{prompt}")
    except (EOFError, KeyboardInterrupt):
        pass


def main() -> int:
    parser = argparse.ArgumentParser(description="Check the environment and start the NanoPlatform build UI")
    parser.add_argument("--language", choices=("auto", "en", "zh"), default="en")
    parser.add_argument("--distro", help="WSL distribution to use on Windows")
    parser.add_argument("--no-pause", action="store_true", help="do not wait for Enter before exiting")
    args = parser.parse_args()

    language = detect_language() if args.language == "auto" else args.language
    configure_console(language)
    repo = Path(__file__).resolve().parent.parent
    common = ["--language", language]
    if args.distro:
        common += ["--distro", args.distro]

    try:
        checker = repo / "script" / "check_build_environment.py"
        result = run_script(checker, common, repo)
        if result != 0:
            print()
            print(text(language, "[ERROR] The build environment is not ready.", "[错误] 构建环境尚未就绪。"))
            print(text(language, "Use the repository skill to install or repair it:", "请使用仓库中的 skill 安装或修复环境："))
            print("  .agents/skills/develop-environment/SKILL.md")
            return result

        builder = repo / "script" / "build_gui.py"
        return run_script(builder, common, repo)
    finally:
        if not args.no_pause:
            pause(language)


if __name__ == "__main__":
    raise SystemExit(main())
