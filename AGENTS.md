# waveshare_boards

[简体中文](AGENTS_ZH.md)

## Code Review Rules

- Cross-check changed pins, clocks, bus addresses, active levels, and coordinate orientation against the schematic or official example for the same model and hardware revision. For an omitted YAML field that affects behavior, follow the default in the relevant driver version. Cite the lines and explain which device is affected and under what conditions.
- Trace first initialization, reuse of existing configuration, and verification after a write. Check that any condition for skipping an update covers the behavior the change requires. Where possible, give one register value, input, or small test with its expected and observed results. Identify whether it ran on hardware.
- Write each finding and the merge recommendation in both languages. Use separate paragraphs labeled `中文：` and `English:`, each covering the trigger, impact, evidence, and suggested change. Preserve code identifiers and evidence links. Check relevant CI jobs and logs for ignored settings. Distinguish successful builds from recorded hardware tests. If you find no reproducible issue, list the paths you actually checked. For any needed follow-up test, give the procedure and pass condition.
