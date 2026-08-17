#!/usr/bin/env python3
"""Check NanoPlatform build prerequisites on Linux or Windows/WSL."""

from __future__ import annotations

import argparse
import locale
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass


TOOLS = (
    "make",
    "arm-none-eabi-gcc",
    "arm-none-eabi-g++",
    "arm-none-eabi-ar",
    "arm-none-eabi-objcopy",
    "arm-none-eabi-size",
)


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


def check_linux_tools() -> list[Check]:
    checks = [Check("Operating system", True, "Linux")]
    for tool in TOOLS:
        path = shutil.which(tool)
        checks.append(Check(tool, path is not None, path or "not found in PATH"))
    return checks


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
    detail = decode_output(result.stderr).strip()
    return False, detail or "not found in WSL PATH"


def check_windows_wsl(requested_distro: str | None) -> list[Check]:
    checks = [Check("Operating system", True, "Windows")]
    wsl = shutil.which("wsl.exe") or shutil.which("wsl")
    checks.append(Check("WSL executable", wsl is not None, wsl or "wsl.exe not found"))
    if wsl is None:
        return checks

    distros, list_error = list_wsl_distros(wsl)
    checks.append(Check("WSL distribution", bool(distros), ", ".join(distros) if distros else (list_error or "none installed")))
    if not distros:
        return checks

    distro = requested_distro or distros[0]
    distro_found = distro in distros
    checks.append(Check("Selected distribution", distro_found, distro if distro_found else f"{distro} is not installed"))
    if not distro_found:
        return checks

    for tool in TOOLS:
        passed, detail = find_tool_in_wsl(wsl, distro, tool)
        checks.append(Check(f"{tool} ({distro})", passed, detail))
    return checks


def render_table(checks: list[Check]) -> None:
    headers = ("CHECK", "STATUS", "DETAIL")
    rows = [(check.item, "PASS" if check.passed else "FAIL", check.detail) for check in checks]
    widths = [len(header) for header in headers]
    for row in rows:
        for index, value in enumerate(row):
            widths[index] = max(widths[index], len(value))

    border = "+" + "+".join("-" * (width + 2) for width in widths) + "+"

    def line(values: tuple[str, str, str]) -> str:
        return "|" + "|".join(f" {value:<{width}} " for value, width in zip(values, widths)) + "|"

    print(border)
    print(line(headers))
    print(border)
    for row in rows:
        print(line(row))
    print(border)
    passed = all(check.passed for check in checks)
    print(f"RESULT: {'PASS - build environment is ready' if passed else 'FAIL - missing build prerequisites'}")


def detect_platform() -> str:
    if os.name == "nt" or sys.platform.startswith("win"):
        return "windows"
    if sys.platform.startswith("linux"):
        return "linux"
    return "unsupported"


def main() -> int:
    parser = argparse.ArgumentParser(description="Check NanoPlatform Make and ARM GNU toolchain prerequisites")
    parser.add_argument("--distro", help="WSL distribution to check on Windows; defaults to the first installed distribution")
    args = parser.parse_args()

    platform_name = detect_platform()
    if platform_name == "windows":
        checks = check_windows_wsl(args.distro)
    elif platform_name == "linux":
        checks = check_linux_tools()
    else:
        checks = [Check("Operating system", False, f"unsupported platform: {sys.platform}")]

    render_table(checks)
    return 0 if all(check.passed for check in checks) else 1


if __name__ == "__main__":
    raise SystemExit(main())
