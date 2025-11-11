# Embedded Session Closed-Loop Controller (Code Track)

Implementation of the “Compilation, Linking & Interfaces” homework under `devEnv/code/scnd`, targeting Linux servers, WSL, and Raspberry Pi 3 Model B deployments.

## Repository layout

```
scnd/
├── Makefile                 # ctl64/ctl32/clean targets
├── README.md                # this file
├── sensor/                  # sensor.h / sensor.c
├── actuators/               # actuator.h + LED/buzzer backends
├── controller/              # ctl.c closed-loop logic
├── data/                    # sensor_feed.csv and future datasets
├── tests/                   # runtime logs + inspection output
└── docs/                    # AI log, build notes, reflection
```

## Building

```bash
cd devEnv/code/scnd
make ctl64          # gcc -m64 (default)
make ctl32          # gcc -m32 (requires multilib headers/libs)
make clean
```

- Linux/WSL users typically need `sudo apt install gcc-multilib g++-multilib` before `make ctl32`; otherwise the build fails with missing `bits/wordsize.h`/`bits/libc-header-start.h`, exactly as seen on this host.
- On Raspberry Pi OS, install the matching cross-compiler or run `make ctl64` natively on a 64-bit image.

## Running & logging

```bash
./build/ctl64 [threshold] [iterations]
```

- `threshold` defaults to 65.0, `iterations` defaults to 200 samples.
- Sensor data comes from `data/sensor_feed.csv` unless the `SENSOR_FEED` env var overrides it; a random fallback is used when the CSV is missing.
- Example log (captured in `tests/sample_run.log`):

```
[14.209505] sensor=65.40 led=ON buzzer=ON
[14.610921] sensor=70.90 led=ON buzzer=ON
[15.111860] sensor=78.90 led=ON buzzer=ON
```

## Binary inspection

```
$ file build/ctl64
build/ctl64: ELF 64-bit LSB pie executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=5948c5b56e71a8e0994114f0a39f616f308199ba, for GNU/Linux 3.2.0, not stripped

$ readelf -h build/ctl64
  Class:   ELF64
  Type:    DYN (PIE)
  Machine: Advanced Micro Devices X86-64
  Entry:   0x12e0
```

Full outputs are saved under `tests/ctl64.file.txt` and `tests/ctl64.readelf.txt`.

## AI usage & reflections

- Interaction history is logged in `docs/ai_log.md`.
- Design notes about compile/link hygiene, wrapper design, timing, and multilib requirements live in `docs/reflection.md` and `docs/build_notes.md`.
