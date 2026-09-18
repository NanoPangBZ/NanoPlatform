#!/usr/bin/env python3
"""Check NanoPlatform build prerequisites on Linux, macOS, or Windows/WSL."""

from __future__ import annotations

import argparse
import locale
import os
import shutil
import subprocess
import sys
import unicodedata
from dataclasses import dataclass


TOOLS = (
    "make",
    "arm-none-eabi-gcc",
    "arm-none-eabi-g++",
    "arm-none-eabi-ar",
    "arm-none-eabi-objcopy",
    "arm-none-eabi-size",
)

LANGUAGE = "en"
TEXT = {
    "Operating system": "操作系统",
    "WSL executable": "WSL 可执行文件",
    "WSL distribution": "WSL 发行版",
    "Selected distribution": "选定的发行版",
    "not found in PATH": "未在 PATH 中找到",
    "not found in WSL PATH": "未在 WSL PATH 中找到",
    "none installed": "未安装",
    "CHECK": "检查项",
    "STATUS": "状态",
    "DETAIL": "详情",
    "PASS": "通过",
    "FAIL": "失败",
    "RESULT": "结果",
    "PASS - build environment is ready": "通过 - 构建环境已就绪",
    "FAIL - missing build prerequisites": "失败 - 缺少构建依赖",
}


def tr(text: str) -> str:
    return TEXT.get(text, text) if LANGUAGE == "zh" else text


@dataclass
class Check:
    item: str
    passed: bool
    detail: str


def decode_output(data: bytes) -> str:
    if not data:
        return ""
    if b"\x00" in data:
        return data.decode("utf-16-le", errors="replace").lstrip("\ufeff")
    return data.decode(locale.getpreferredencoding(False) or "utf-8", errors="replace")


def run_bytes(command: list[str], timeout: int = 15) -> subprocess.CompletedProcess[bytes]:
    return subprocess.run(
        command,
        stdin=subprocess.DEVNULL,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
        timeout=timeout,
    )


def clean_lines(output: str) -> list[str]:
    return [line.replace("\x00", "").strip() for line in output.splitlines() if line.replace("\x00", "").strip()]


def check_host_tools(os_label: str) -> list[Check]:
    checks = [Check(tr("Operating system"), True, os_label)]
    for tool in TOOLS:
        path = shutil.which(tool)
        checks.append(Check(tool, path is not None, path or tr("not found in PATH")))
    return checks


def check_linux_tools() -> list[Check]:
    return check_host_tools("Linux")


def check_macos_tools() -> list[Check]:
    return check_host_tools("macOS")


def list_wsl_distros(wsl: str) -> tuple[list[str], str]:
    try:
        result = run_bytes([wsl, "--list", "--quiet"])
    except (OSError, subprocess.TimeoutExpired) as error:
        return [], str(error)
    output = decode_output(result.stdout)
    distros = clean_lines(output)
    error = decode_output(result.stderr).strip()
    if result.returncode != 0:
        return [], error or f"wsl.exe exited with {result.returncode}"
    return distros, error


def find_tool_in_wsl(wsl: str, distro: str, tool: str) -> tuple[bool, str]:
    try:
        result = run_bytes([wsl, "-d", distro, "--", "sh", "-lc", f"command -v -- {tool}"])
    except (OSError, subprocess.TimeoutExpired) as error:
        return False, str(error)
    lines = clean_lines(decode_output(result.stdout))
    if result.returncode == 0 and lines:
        return True, lines[-1]
    if result.returncode in (1, 127) and not lines:
        return False, tr("not found in WSL PATH")
    detail = decode_output(result.stderr).strip()
    return False, detail or tr("not found in WSL PATH")


def check_windows_wsl(requested_distro: str | None) -> list[Check]:
    checks = [Check(tr("Operating system"), True, "Windows")]
    wsl = shutil.which("wsl.exe") or shutil.which("wsl")
    checks.append(Check(tr("WSL executable"), wsl is not None, wsl or "wsl.exe not found"))
    if wsl is None:
        return checks

    distros, list_error = list_wsl_distros(wsl)
    checks.append(Check(tr("WSL distribution"), bool(distros), ", ".join(distros) if distros else (list_error or tr("none installed"))))
    if not distros:
        return checks

    distro = requested_distro or distros[0]
    distro_found = distro in distros
    checks.append(Check(tr("Selected distribution"), distro_found, distro if distro_found else f"{distro} is not installed"))
    if not distro_found:
        return checks

    for tool in TOOLS:
        passed, detail = find_tool_in_wsl(wsl, distro, tool)
        checks.append(Check(f"{tool} ({distro})", passed, detail))
    return checks


def render_table(checks: list[Check]) -> None:
    headers = (tr("CHECK"), tr("STATUS"), tr("DETAIL"))
    rows = [(check.item, tr("PASS") if check.passed else tr("FAIL"), check.detail) for check in checks]
    def display_width(value: str) -> int:
        return sum(2 if unicodedata.east_asian_width(character) in {"W", "F"} else 1 for character in value)

    widths = [display_width(header) for header in headers]
    for row in rows:
        for index, value in enumerate(row):
            widths[index] = max(widths[index], display_width(value))

    border = "+" + "+".join("-" * (width + 2) for width in widths) + "+"

    def line(values: tuple[str, str, str]) -> str:
        return "|" + "|".join(f" {value}{' ' * (width - display_width(value))} " for value, width in zip(values, widths)) + "|"

    print(border)
    print(line(headers))
    print(border)
    for row in rows:
        print(line(row))
    print(border)
    passed = all(check.passed for check in checks)
    result = "PASS - build environment is ready" if passed else "FAIL - missing build prerequisites"
    print(f"{tr('RESULT')}: {tr(result)}")


def detect_platform() -> str:
    if os.name == "nt" or sys.platform.startswith("win"):
        return "windows"
    if sys.platform.startswith("linux"):
        return "linux"
    if sys.platform == "darwin":
        return "macos"
    return "unsupported"


def main() -> int:
    global LANGUAGE
    parser = argparse.ArgumentParser(description="Check NanoPlatform Make and ARM GNU toolchain prerequisites")
    parser.add_argument("--distro", help="WSL distribution to check on Windows; defaults to the first installed distribution")
    parser.add_argument("--language", choices=("en", "zh"), default="en", help="output language (default: en)")
    args = parser.parse_args()
    LANGUAGE = args.language

    platform_name = detect_platform()
    if platform_name == "windows":
        checks = check_windows_wsl(args.distro)
    elif platform_name == "linux":
        checks = check_linux_tools()
    elif platform_name == "macos":
        checks = check_macos_tools()
    else:
        checks = [Check(tr("Operating system"), False, f"unsupported platform: {sys.platform}")]

    render_table(checks)
    return 0 if all(check.passed for check in checks) else 1


if __name__ == "__main__":
    raise SystemExit(main())
