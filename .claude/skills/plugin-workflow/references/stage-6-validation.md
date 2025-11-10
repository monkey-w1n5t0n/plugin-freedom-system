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
- Automated tests pass

## Actions

### 1. Create Factory Presets

```bash
mkdir -p plugins/[PluginName]/Presets/
```

Create 3-5 preset files showcasing plugin capabilities in .preset or .xml format.

**Preset format example:**

```xml
<?xml version="1.0" encoding="UTF-8"?>
<preset name="[PresetName]">
  <param id="[paramID]" value="[value]"/>
  <param id="[paramID]" value="[value]"/>
  ...
</preset>
```

### 2. Test Method Selection

Present test method choice:

```
How would you like to test [PluginName]?

1. Automated stability tests (if Tests/ directory exists)
2. Build and run pluginval (recommended)
3. Manual DAW testing checklist
4. Skip testing (not recommended)

Choose (1-4): _
```

**If tests fail:** STOP and wait for fixes.

### 3. Generate CHANGELOG.md

Create in Keep a Changelog format:

```markdown
# Changelog

All notable changes to [PluginName] will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [1.0.0] - [YYYY-MM-DD]

### Added

- Initial release
- [Feature descriptions]
- [Parameter descriptions]

### Audio Processing

- [DSP description]

### User Interface

- [UI description]

### Validation

- Passed pluginval strict validation
- Tested in [DAW names]
```

### 4. Invoke Validator Subagent

Validate Stage 6 completion:
- CHANGELOG.md exists in Keep a Changelog format
- Version 1.0.0 for initial release
- Presets/ directory has 3+ preset files
- pluginval passed or skipped with reason
- PLUGINS.md updated to ✅ Working

### 5. Update PLUGINS.md

```typescript
updatePluginStatus(pluginName, "✅ Working")
```

Add final fields:
```markdown
**Version:** 1.0.0
**Completed:** [YYYY-MM-DD]

**Validation:**
- ✓ Factory presets: [N] presets created
- ✓ CHANGELOG.md: Generated in Keep a Changelog format
- ✓ Pluginval: [Passed | Skipped]

**Formats:** VST3, AU
```

Add timeline entry:
```markdown
- **[YYYY-MM-DD] (Stage 6):** Validation complete
```

### 6. Delete .continue-here.md

```bash
rm plugins/[PluginName]/.continue-here.md
```

Workflow is complete, no need for handoff file.

### 7. Git Commit

```bash
git add plugins/[PluginName]/Presets/
git add plugins/[PluginName]/CHANGELOG.md
git add PLUGINS.md

git commit -m "$(cat <<'EOF'
feat: [PluginName] Stage 6 - validation complete

Factory presets: [N] presets created
CHANGELOG.md: Keep a Changelog format
Pluginval: [Passed | Skipped]

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
```

**Note:** `.continue-here.md` is deleted and NOT committed (workflow complete).

## Decision Menu

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

- **Option 1:** Invoke `plugin-lifecycle` skill (installation to system folders)
- **Option 2:** Provide instructions for manual DAW testing from build folder
- **Option 3:** Exit, suggest `/dream` or `/implement`
- **Option 4:** Suggest creating README or user manual documentation
- **Option 5:** Provide instructions for exporting build artifacts
- **Option 6:** Ask what they'd like to do

---

**Return to:** Main plugin-workflow orchestration in `SKILL.md`
