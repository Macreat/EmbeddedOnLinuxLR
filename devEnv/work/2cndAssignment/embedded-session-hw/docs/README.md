# Documentation Deliverables

This folder will gather every non-source artifact referenced in the assignment brief.

## Expected files
- `ai_log.md` – chronological list of prompts, assistant replies, and short rationales explaining how each interaction improved the solution.
- `build_notes.md` – optional space to document toolchain setup (e.g., installing `gcc-multilib` on WSL or Pi OS).
- `reflection.md` – notes on compile vs. link errors, wrapper design rationale, and lessons learned.

## Workflow
1. **Before coding:** summarize the plan and assumptions here so the README at the repo root can point back to detailed documentation.
2. **During development:** after each assistant interaction or major change, append to `ai_log.md` with timestamps and justifications (per assignment requirement).
3. **After testing:** capture command outputs (`file`, `readelf`, sample runs) under `../tests/` and reference them in `reflection.md`.
4. **Final check:** ensure every deliverable listed in `docs/Compilation linking interfaces homework.md` is linked either from this directory or the root README.
