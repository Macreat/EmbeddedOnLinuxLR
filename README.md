# EmbeddedOnLinuxLR

## Learning route for my Embedded Systems on Linux course (UNAL 2025-2)

_Course taught by Prof. Juan Bernardo Gomez Mendoza – Universidad Nacional de Colombia, Sede Manizales_

![status](https://img.shields.io/badge/status-active-brightgreen)
![made-with](https://img.shields.io/badge/Made%20with-C%20%26%20Python-blue)
![platform](https://img.shields.io/badge/platform-Ubuntu%20%7C%20Arch-lightgrey)

---

## Course Overview

This repository documents the complete learning route for the course **Programación de Sistemas Linux Embebidos (Embedded Linux Programming 4201152)**.  
Students explore embedded systems development through Linux, from kernel space to user space, combining **C, Python, and shell scripting**.

**Course Goals**

- Understand embedded Linux architecture and low-power SoC platforms.
- Develop simple user- and kernel-space applications.
- Work with drivers, APIs, and system-level software.
- Integrate machine–machine (M2M) and human–machine (HMI) interfaces.

**Methodology**

- Combination of **lectures + hands-on labs**.
- Use of **virtual machines, WSL, Docker**, and **SBCs** (Raspberry Pi 4, BeaglePlay, Lichee RV, etc. (rapspberry pi used on this case)).
- Tasks include writing **C programs, shell scripts, kernel modules**, and **Python utilities**.

---

## Branch Strategy

To keep the work organized, this repo uses a **branching model**:

- **`main`**  
  Main branch. Contains **stable, reviewed, and clean code/documentation**.  
  Always reflects the state of the project that is ready to be delivered or referenced.

- **`dev`**  
  Development branch. Contains **ongoing work** and integrates features before merging into `main`.  
  All new features or experiments should be branched off `dev`.

---

## Repository Structure

```
EmbeddedOnLinuxLR/
├── content/
├── work/
│   ├── work/
│   ├── code/
│   ├── PYDevEnv/
│       ├── Include/ | Lib/ | nbs/ | Scripts/
└── README.md
```

---

## Practical Workflow

```bash
sudo apt install build-essential git libncurses5-dev
make
./cpuInfo
python3 -m venv work/embeddedOnLinuxPYDevEnv
source work/embeddedOnLinuxPYDevEnv/bin/activate
```

---

## Key Concepts Summary

- Linux services = daemons managed by systemd.
- `/proc` = virtual filesystem for kernel info.
- Embedded Linux = multi-layered abstraction system.
- Debugging tools: `valgrind`, `ddd`, `kcachegrind`.
- ncurses enables TUI interaction for resource monitoring.

---

## Notes and Resources

### all course contente inside content dir

- **Class notes** → [`content/lectures/`](./content/lectures/)
- **Presentations** → [`content/classContent/`](./content/classContent/)
- **interest papers** → [`/papers/`](./content/papers/)
- **code and courses from class** → [`code`](./devEnv/code/)
- **work and projects** → [`work space`](./devEnv/work/)

### work in progress inside dev Env dir

- **work progress and deliverys** → [`workspace/`](./devEnv/work/)
- **simulations** → [`simulations/`](./devEnv/code/)
- **environments** → [`local development environment/`](./devEnv/)

- **Reference** →
- **Tools** →

---

## Roadmap

- [x] Repository structure and environment setup
- [ ] Import lecture notes and materials
- [ ] task as :
      -Kernel module exercise (heartbeat service)
      -Raspberry Pi deployment, conection and SSH access
      -Call-graph visualization in Python and C
- [ ] project advances

---

## Author

**Mateo Almeida (Macreat)**  
Course: Embedded Systems on Linux @ UNAL 2025-2  
GitHub: [@Macreat](https://github.com/Macreat)

---

## License

**x**  
For academic and learning purposes only.
