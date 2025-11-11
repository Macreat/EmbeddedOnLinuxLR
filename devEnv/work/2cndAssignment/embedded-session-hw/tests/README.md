# Testing & Verification Notes

Use this folder to capture evidence that the controller works as required.

## Planned artifacts
- `sample_run.log` – streaming output from `./build/ctl64 [threshold] [iterations]`.
- `ctl64.file.txt` / `ctl64.readelf.txt` – copies of `file` and `readelf -h` output for documentation.
- Additional CSVs or scripts for regression tests.

## Workflow
1. After implementing code, run unit benches (if added) plus an end-to-end session with a deterministic sensor feed.
2. Save terminal output to log files and reference them from the top-level README.
3. For 32-bit builds, document any toolchain/setup steps and attach inspection logs alongside the 64-bit ones.
4. Keep this README updated with the testing matrix so reviewers can reconstruct the validation process quickly.
