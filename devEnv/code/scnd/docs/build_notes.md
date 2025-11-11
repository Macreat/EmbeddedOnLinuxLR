# Build Notes

## Toolchains
- **64-bit (default):** Works with the system `gcc` using `-Wall -Wextra -std=c11 -m64`.
- **32-bit:** Requires multilib headers/libraries. On Debian/Ubuntu/WSL: `sudo apt update && sudo apt install gcc-multilib g++-multilib`. On Raspberry Pi OS (armhf), install `gcc-arm-linux-gnueabihf` if cross-compiling from x86_64.

## Commands
```
make ctl64   # builds build/ctl64
make ctl32   # builds build/ctl32 (needs multilib)
make clean   # removes build artifacts
```

Capture the output of `file build/ctl64` and `readelf -h build/ctl64` after compiling and place it in `tests/`.
