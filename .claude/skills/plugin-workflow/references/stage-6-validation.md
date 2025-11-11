# Stage 6: Validation

**Context:** This file is part of the plugin-workflow skill.
**Invoked by:** Main workflow dispatcher after Stage 5 completion
**Purpose:** Final validation, ready to install and use

---

**Goal:** Ready to install and use

**Duration:** 10-20 minutes

**Preconditions:**

- Stages 0-5 complete
- Plugin compiles successfully
- Automated tests pass (if run)

**Actions:**

1. Create factory presets:

```bash
mkdir -p plugins/[PluginName]/Presets/
```

Create 3-5 preset files showcasing plugin capabilities.

**Preset format (.preset or .xml):**

```xml
<?xml version="1.0" encoding="UTF-8"?>
<preset name="[PresetName]">
  <param id="[paramID]" value="[value]"/>
  <param id="[paramID]" value="[value]"/>
  ...
</preset>
```

2. Invoke plugin-testing skill:

Present test method choice:

```
How would you like to test [PluginName]?

1. Automated stability tests (if Tests/ directory exists)
2. Build and run pluginval (recommended)
3. Manual DAW testing checklist
4. Skip testing (not recommended)

Choose (1-4): _
```

If tests fail, STOP and wait for fixes.

3. Generate CHANGELOG.md:

**Format:**

```markdown
# Changelog

All notable changes to [PluginName] will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [1.0.0] - [YYYY-MM-DD]

### Added

- Initial release
- [Feature 1]
- [Feature 2]
- [Parameter 1]: [Description]
- [Parameter 2]: [Description]

### Audio Processing

- [DSP description]

### User Interface

- [UI description]

### Validation

- Passed pluginval strict validation
- Tested in [DAW names]
```

4. Invoke validator subagent:

Call validator to verify Stage 6 completion:

```typescript
const validation = Task({
  subagent_type: "validator",
  description: `Validate ${pluginName} Stage 6`,
  prompt: `
Validate Stage 6 completion for ${pluginName}.

**Stage:** 6
**Plugin:** ${pluginName}
**Contracts:**
- parameter-spec.md: [paste content or "not applicable"]
- architecture.md: [paste content or "not applicable"]
- plan.md: [paste content]

**Expected outputs for Stage 6:**
- CHANGELOG.md exists in Keep a Changelog format
- Version 1.0.0 for initial release
- Presets/ directory has 3+ preset files
- pluginval passed (or skipped with reason)
- PLUGINS.md updated to ✅ Working

Return JSON validation report with status, checks, and recommendation.
  `
})

const report = JSON.parse(validation)

if (report.status === "FAIL") {
  // Present validation failure menu
  presentValidationFailure(report)
  // Options: Fix issues / Continue anyway / Pause
  // Wait for user choice before proceeding
}
```

5. Update PLUGINS.md:

Call `updatePluginStatus(pluginName, "✅ Working")`.

Add final fields to entry:
```markdown
**Version:** 1.0.0
**Completed:** [YYYY-MM-DD]

**Validation:**
- ✓ Factory presets: [N] presets created
- ✓ CHANGELOG.md: Generated in Keep a Changelog format
- ✓ Pluginval: [Passed | Skipped (no build)]

**Formats:** VST3, AU (if built)
```

Add timeline entry: `updatePluginTimeline(pluginName, 6, "Validation complete")`.

6. Delete .continue-here.md:

Call `deleteHandoff(pluginName)`:

```bash
rm plugins/[PluginName]/.continue-here.md
```

Workflow is complete, no need for handoff file.

**Git commit:**

Call `commitStage(pluginName, 6, "validation complete")`.

This commits:
- `plugins/[PluginName]/Presets/`
- `plugins/[PluginName]/CHANGELOG.md`
- `PLUGINS.md`

Note: `.continue-here.md` is deleted and NOT committed (workflow complete).

**Decision menu:**

```
✓ Stage 6 complete: [PluginName] is ready!

What's next?
1. Install plugin to system folders (recommended)
2. Test in DAW from build folder first
3. Create another plugin
4. Document this plugin
5. Share plugin (export build)
6. Other

Choose (1-6): _
```

**Handle responses:**

- Option 1 → Invoke `plugin-lifecycle` skill (Phase 1b Task 9)
- Option 2 → Provide instructions for manual DAW testing
- Option 3 → Exit, suggest `/dream` or `/implement`
- Option 4 → Suggest creating README or documentation
- Option 5 → Provide instructions for exporting builds
- Option 6 → Ask what they'd like to do

---

**Return to:** Main plugin-workflow orchestration in `SKILL.md`
