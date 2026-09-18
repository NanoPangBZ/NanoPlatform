# Package set

Use this complete Ubuntu package set:

| Package | Purpose |
|---|---|
| `make` | Run the repository Make build |
| `gcc-arm-none-eabi` | ARM Cortex-M/R bare-metal GCC and core compiler tools |
| `binutils-arm-none-eabi` | Assembler, linker, `objcopy`, `size`, and archive utilities |
| `libnewlib-arm-none-eabi` | Bare-metal C and math libraries |
| `libstdc++-arm-none-eabi-newlib` | Bare-metal GNU C++ standard library |

The environment checker verifies the executable surface used by the repository: `make`, `arm-none-eabi-gcc`, `g++`, `ar`, `objcopy`, and `size`.

Package availability and names are defined by the selected distribution. Verify them before adapting this workflow to a non-Ubuntu system.
