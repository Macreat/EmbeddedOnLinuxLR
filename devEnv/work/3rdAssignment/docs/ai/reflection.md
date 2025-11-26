# Reflection on AI-Assisted Development

I used AI assistance to accelerate the design, documentation, and modularization of the `assignment-sensor` project, which implements a systemd-managed mock sensor sampler in C. The goal was not to delegate responsibility but to leverage AI for structural guidance, code refinement, and validation of design decisions.

My first interaction focused on interpreting the assignment requirements. The AI provided a structured breakdown of the expected components: a compiled C binary, a systemd service starting at `multi-user.target`, fallback logging logic, a reproducible Makefile, and an `ai/` evidence directory. I accepted this plan because it aligned directly with the assignment specification and helped ensure early compliance.

For implementation details, the AI suggested using `/dev/urandom` as a mock sensor given its non-blocking nature. It also recommended `sigaction` for signal handling and `clock_gettime` combined with `gmtime_r` for generating ISO-8601 timestamps. These recommendations were sound and aligned with best practices in embedded Linux development, so I incorporated them. However, I added explicit error checks on all I/O operations—something the AI did not initially emphasize—to ensure correct exit codes and robustness in failure cases.

The fallback logging policy required careful design. While the AI recommended defaulting to `/tmp` and falling back to `/var/tmp`, I strengthened the semantics by preventing silent fallback when the user explicitly provides a `--logfile` argument. This mirrors real-world expectations: explicit operator intent should override automatic recovery policies.

The AI proposed using `getopt_long` for parsing CLI arguments, but I chose a simpler manual parser to minimize dependencies and maintain portability across embedded toolchains. I verified that invalid or incomplete options are handled cleanly, and that numeric values are validated using `strtod`.

The modularization step—splitting the program into `assignment_sensor.h`, `assignment_sensor.c`, and a minimal `main.c`—was significantly accelerated using AI. The generated layout removed unnecessary coupling and improved testability.

Systemd troubleshooting was another area where AI was particularly helpful. When the service failed with `203/EXEC`, the AI recognized the root cause: the binary had been compiled on the wrong architecture. This insight saved time by redirecting debugging away from the program logic. The solution—compiling natively on the Raspberry Pi—resolved the issue.

Throughout the process, I manually validated every AI suggestion. I tested the binary both outside and inside systemd, inspected logs, checked permissions, and ensured consistency with assignment requirements. The AI provided speed and structure, but correctness and verification remained my responsibility.

Overall, AI support helped streamline development while reinforcing core embedded Linux concepts. The integration of AI was deliberate, critical, and aligned with academic integrity.

```

```
