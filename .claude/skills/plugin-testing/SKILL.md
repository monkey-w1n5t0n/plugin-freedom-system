---
name: plugin-testing
description: Run automated stability tests, pluginval validation, and DAW testing for audio plugins. Invoke when user mentions test, validate, validation, pluginval, stability, automated tests, run tests, check plugin, or quality assurance
allowed-tools:
  - Read
  - Bash
  - Task # For deep-research on failures
preconditions:
  - Plugin must exist
  - Plugin status must NOT be 💡 (must have implementation)
---

# plugin-testing Skill

**Purpose:** Catch crashes, parameter issues, and state bugs in 2 minutes with automated tests.

## Workflow Overview

This skill provides three test modes:

1. **Automated Testing** (~2 min) - Quick C++ unit tests for stability
2. **Build + Pluginval** (~5-10 min) - Industry-standard validation (50+ tests) ⭐ **RECOMMENDED**
3. **Manual DAW Testing** (~30-60 min) - Real-world testing with guided checklist

## Phase 1: Detect Plugin and Mode Selection

<decision_gate enforcement="strict">
**You MUST complete ALL steps before presenting mode selection:**

1. Parse plugin name from user input (extract from `/test [Name]` or natural language)
2. Read `PLUGINS.md` and verify plugin exists
3. Verify plugin status is NOT 💡 (implementation must exist)
4. Check for Tests/ directory existence: `test -d "plugins/$PLUGIN_NAME/Tests/"`

**Once validation complete, you MUST present mode selection menu** (see `assets/decision-menu-templates.md#mode-selection`)

**You MUST NOT proceed to Phase 2 until user selects a mode (1-4)**
</decision_gate>

**Parse shorthand commands:**
- `/test [PluginName] build` → Jump to Mode 2
- `/test [PluginName] quick` → Jump to Mode 1
- `/test [PluginName] manual` → Jump to Mode 3

## Phase 2: Execute Test Mode

### Mode 1: Automated Testing

<critical_sequence enforcement="strict">
**You MUST execute these steps in exact order:**

<step id="read_spec">
**Step 1: Read Test Specifications**

You MUST read `references/test-specifications.md` for detailed test implementations before proceeding.
</step>

<step id="check_tests" depends_on="read_spec">
**Step 2: Check for Tests/ Directory**

You MUST verify Tests/ directory exists:
```bash
test -d "plugins/$PLUGIN_NAME/Tests/"
```

If missing: You MUST inform user that automated tests require test infrastructure and suggest Mode 2 (pluginval) as recommended alternative. Present decision menu from `assets/decision-menu-templates.md#missing-tests`.
</step>

<step id="build" depends_on="check_tests">
**Step 3: Build and Execute Tests**

You MUST build and run tests (see `references/test-specifications.md#execution`).
</step>

<step id="parse" depends_on="build">
**Step 4: Parse Test Results**

You MUST parse test output and generate report using template from `assets/report-templates.md#mode1-results`.
</step>

<step id="present" depends_on="parse">
**Step 5: Present Results**

You MUST present test results and post-test decision menu (see `assets/decision-menu-templates.md#post-test-mode1`). WAIT for user selection.
</step>
</critical_sequence>

### Mode 2: Build + Pluginval

<critical_sequence enforcement="strict">
**You MUST execute these steps in exact order:**

<step id="prereq_check">
**Step 1: Prerequisites Check**

You MUST read `references/pluginval-guide.md#installation-check` for implementation details.

Check for pluginval installation using script from reference file.

**If pluginval not found:** You MUST present installation decision menu (see `assets/decision-menu-templates.md#pluginval-install`) and WAIT for user response. BLOCK execution until resolved.
</step>

<step id="build" depends_on="prereq_check">
**Step 2: Build Release Binaries**

You MUST build in Release mode (see `references/pluginval-guide.md#build-process`):

Locate binaries:
```bash
VST3_PATH="build/plugins/$PLUGIN_NAME/${PLUGIN_NAME}_artefacts/Release/VST3/${PRODUCT_NAME}.vst3"
AU_PATH="build/plugins/$PLUGIN_NAME/${PLUGIN_NAME}_artefacts/Release/AU/${PRODUCT_NAME}.component"
```
</step>

<step id="validate" depends_on="build">
**Step 3: Run Pluginval**

You MUST validate both formats at strictness level 10 (see `references/pluginval-guide.md#execution`).
</step>

<step id="parse" depends_on="validate">
**Step 4: Parse Results**

You MUST parse pluginval output (see `references/pluginval-guide.md#parsing-output`) and generate report using template from `assets/report-templates.md#mode2-results`.
</step>

<step id="present" depends_on="parse">
**Step 5: Present Results and Next Steps**

You MUST present post-test decision menu (see `assets/decision-menu-templates.md#post-test-mode2`) and WAIT for user selection.
</step>
</critical_sequence>

### Mode 3: Manual DAW Testing

<critical_sequence enforcement="strict">
**You MUST execute these steps in exact order:**

<step id="read_guide">
**Step 1: Read DAW Testing Guide**

You MUST read `references/manual-testing-guide.md` for complete manual testing procedures.
</step>

<step id="generate_checklist" depends_on="read_guide">
**Step 2: Generate Customized Checklist**

You MUST generate customized checklist from `parameter-spec.md` tailored to plugin's specific parameters and features.
</step>

<step id="present_checklist" depends_on="generate_checklist">
**Step 3: Present Checklist**

You MUST present checklist to user with instructions for manual testing in their DAW.

Inform user to report back with results (pass/fail per item).
</step>

<step id="collect_results" depends_on="present_checklist">
**Step 4: Collect Results**

WAIT for user to complete manual testing and provide results.

You MUST parse user's feedback and generate completion report.
</step>

<step id="present_menu" depends_on="collect_results">
**Step 5: Present Next Steps**

You MUST present post-test decision menu (see `assets/decision-menu-templates.md#post-test-mode3`) and WAIT for user selection.
</step>
</critical_sequence>

## Phase 3: Failure Investigation (Option 1)

<delegation_rule enforcement="strict">
**When user chooses "Investigate failures":**

<sequence>
1. You MUST read `references/troubleshooting.md` for each failed test
2. Provide initial fix recommendations from troubleshooting docs
3. Offer code inspection option

4. **For non-trivial issues (not documented in troubleshooting.md):**

   <handoff_protocol>
   You MUST delegate to `deep-research` skill via Task tool:

   ```
   Task: "Investigate [test_name] failure in [PluginName]

   Context:
   - Test failed: [specific failure message]
   - Plugin type: [from creative-brief.md]
   - Relevant code: [file paths from investigation]

   Goal: Find root cause and provide specific fix"
   ```

   WAIT for deep-research completion before presenting results to user.

   **NEVER attempt to fix complex issues without delegation to deep-research.**
   </handoff_protocol>
</sequence>
</delegation_rule>

## Phase 4: Log Test Results

<state_requirement enforcement="strict">
**After completing ANY test mode (1, 2, or 3), you MUST:**

<requirement id="save_log">
**Requirement 1: Save Test Log**

You MUST save detailed test log to: `logs/[PluginName]/test_[timestamp].log`

Use format from `assets/report-templates.md#test-log-format`
</requirement>

<requirement id="update_continue">
**Requirement 2: Update .continue-here.md**

You MUST update `.continue-here.md`:
- Set current_stage: "testing_complete"
- Set next_step based on test results (Stage 6 if passed, investigation if failed)
- Record test_date: [timestamp]
- Record test_mode: [1/2/3]
</requirement>

<requirement id="update_plugins">
**Requirement 3: Update PLUGINS.md**

You MUST update `PLUGINS.md` for {PLUGIN_NAME}:
- Set test_status: "✅ passed" or "❌ failed"
- Record last_tested: [date]
- Record test_mode_used: [Mode 1/2/3]
</requirement>

**You MUST complete ALL three requirements before proceeding to decision menu.**

VERIFY both state files updated before presenting next steps.
</state_requirement>

## Success Criteria

Testing is successful when:

- ✅ Tests run without crashes (even if some fail, process completes)
- ✅ All tests pass OR failures are documented with clear explanations
- ✅ User understands what failed and why (no mystery errors)
- ✅ Logs saved for future reference (`logs/[PluginName]/`)
- ✅ User knows next step (install, fix issues, continue workflow)
- ✅ Test results stored in PLUGINS.md (test date, pass/fail, mode used)

**NOT required for success:**

- 100% pass rate (failures are learning opportunities)
- Fixing all issues immediately (user can defer fixes)
- Running all 3 test modes (one mode is sufficient for validation)

## Integration Points

**Invoked by:**

- `/test [PluginName]` command
- `/test [PluginName] build` → Direct to mode 2
- `/test [PluginName] quick` → Direct to mode 1
- `/test [PluginName] manual` → Direct to mode 3
- `plugin-workflow` skill → After Stages 4, 5, 6
- `plugin-improve` skill → After implementing changes
- Natural language: "Test [PluginName]", "Run validation on [PluginName]"

**Invokes (future):**

- `deep-research` skill (Phase 7) → When user chooses "Investigate failures"

**Creates:**

- Test logs in `logs/[PluginName]/test_[timestamp].log`
- Build artifacts in `build/plugins/[PluginName]/`

**Updates:**

- `.continue-here.md` → Testing checkpoint
- `PLUGINS.md` → Test status

**Blocks:**

- Installation (`/install-plugin`) → Recommends testing first if not done recently

## Reference Documentation

- **Test specifications:** `references/test-specifications.md` - Detailed implementation of 5 automated tests
- **Pluginval guide:** `references/pluginval-guide.md` - Industry-standard validation details
- **Manual testing guide:** `references/manual-testing-guide.md` - DAW testing methodology
- **Troubleshooting:** `references/troubleshooting.md` - Common issues and fixes

## Template Assets

- **Manual testing checklist:** `assets/manual-testing-checklist.md` - DAW testing template
