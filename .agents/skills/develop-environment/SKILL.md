---
name: develop-environment
description: Check and provision the NanoPlatform embedded build environment by running script/check_build_environment.py first, then installing missing Windows WSL support, an Ubuntu WSL distribution, and the complete ARM GNU bare-metal toolchain in dependency order. Use when the user asks to prepare, repair, bootstrap, install, or verify the NanoPlatform development environment on Windows or Linux.
---

# Develop Environment

Work from the NanoPlatform repository root. Use `Ubuntu-24.04` as the default WSL distribution unless the user explicitly selects another Ubuntu distribution.

## Check first

1. Run `python script/check_build_environment.py`.
2. On Windows, pass `--distro <name>` after selecting or installing a distribution.
3. If the check returns exit code 0, report that the environment is ready and install nothing.
4. If it returns exit code 1, use its checklist to identify the first missing dependency. Do not reinstall components that already pass.

## Provision Windows in dependency order

Perform one stage at a time and re-run the checker after every stage.

### 1. Install WSL when missing

Use an elevated Windows shell:

```powershell
wsl.exe --install --no-distribution
```

This changes Windows optional features and may require administrator approval and a reboot. Use the host's elevation/approval mechanism. Do not bypass UAC. If Windows reports that a restart is required:

1. Stop provisioning.
2. Tell the user to restart Windows.
3. On the next invocation, begin again with the checker.
4. Never restart the computer automatically unless the user explicitly requests it in the current turn.

### 2. Install Ubuntu when no distribution is available

First confirm the requested name is available with `wsl.exe --list --online`.

Install the default distribution without launching an interactive shell:

```powershell
wsl.exe --install --distribution Ubuntu-24.04 --no-launch
```

If `Ubuntu-24.04` is unavailable, show the available Ubuntu names and ask the user to select one. Do not substitute a different distribution silently.

After installation, verify it appears in `wsl.exe --list --quiet`. If installation requires a restart or first-launch user setup, stop and explain the required user action. Resume by running the checker with `--distro <installed-name>`.

### 3. Install the complete ARM GNU toolchain inside Ubuntu

Use the installed distribution and run package management as root:

```powershell
wsl.exe --distribution Ubuntu-24.04 --user root -- sh -lc "apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y make gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib"
```

Install all listed packages. `gcc-arm-none-eabi` alone is not the complete C/C++ bare-metal environment used by this repository.

Do not run `apt-get upgrade`, remove packages, unregister distributions, or change the default distribution.

Re-run `python script/check_build_environment.py --distro Ubuntu-24.04`. Finish only when it returns exit code 0.

## Provision Linux

Run the checker directly. On Debian or Ubuntu, install the same package set:

```bash
sudo apt-get update
sudo apt-get install -y make gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
```

Use the host's approval mechanism before `sudo`. On non-APT distributions, identify the operating system and official package manager, then present the exact proposed package command before executing it. Do not guess package names.

Re-run `python3 script/check_build_environment.py` and finish only when it returns exit code 0.

## Handle failures

- Preserve the complete command output for a failed installation stage.
- Do not continue to later stages when the current dependency is still missing.
- Do not treat a pending reboot, distro first-launch prompt, unavailable package, or failed network download as success.
- Do not install a separate native Windows ARM toolchain; on Windows this workflow provisions the toolchain inside WSL.
- Report the final checker table, selected distribution, packages installed, and whether a restart or manual action remains.

Read `references/package-set.md` when explaining why the complete package set is required.
