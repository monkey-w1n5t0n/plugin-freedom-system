# Fix Plan: plugin-testing

## Summary
- Critical fixes: **6** (XML enforcement for all critical workflows)
- Extraction operations: **4** (decision menus, report templates, investigation guide, bash snippets)
- Total estimated changes: **38** discrete operations
- Estimated time: **95 minutes** (~1.5 hours)

---

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Wrap Phase 1 Detection in `<decision_gate>`
**Location:** Lines 29-71
**Operation:** WRAP
**Before:**
```markdown
## Phase 1: Detect Plugin and Mode Selection

**Input:** User invokes `/test [PluginName]` or asks to test a plugin

**Steps:**

1. Parse plugin name from user input
2. Verify plugin exists in `PLUGINS.md`
3. Check plugin status - must NOT be 💡 (must have implementation)
4. Check for Tests/ directory existence

**If mode not specified, present decision menu:**
```

**After:**
```markdown
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
```

**Verification:** Confirm `<decision_gate>` tags present at Phase 1 start and end

---

### Fix 1.2: Wrap Mode 1 Execution in `<critical_sequence>`
**Location:** Lines 74-142
**Operation:** WRAP
**Before:**
```markdown
### Mode 1: Automated Stability Tests

**See:** `references/test-specifications.md` for detailed test implementations

**Check if Tests/ directory exists:**
```

**After:**
```markdown
### Mode 1: Automated Stability Tests

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
```

**Verification:** Confirm `<critical_sequence>` with 5 `<step>` tags, each with `depends_on` attribute

---

### Fix 1.3: Wrap Mode 2 Execution in `<critical_sequence>`
**Location:** Lines 144-248
**Operation:** WRAP
**Before:**
```markdown
### Mode 2: Build + Pluginval

**See:** `references/pluginval-guide.md` for detailed implementation

**Prerequisites check:**

```bash
# Check for pluginval.app first (standard macOS install), then fall back to PATH
if [ -x "/Applications/pluginval.app/Contents/MacOS/pluginval" ]; then
    PLUGINVAL_PATH="/Applications/pluginval.app/Contents/MacOS/pluginval"
```

**After:**
```markdown
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
```

**Verification:** Confirm `<critical_sequence>` with 5 `<step>` tags, prerequisite check blocks when pluginval missing

---

### Fix 1.4: Wrap Mode 3 Execution in `<critical_sequence>`
**Location:** Lines 252-286
**Operation:** WRAP
**Before:**
```markdown
### Mode 3: Manual DAW Testing

**See:** `references/daw-testing-guide.md` for detailed checklist

**Generate customized checklist from:**
```

**After:**
```markdown
### Mode 3: Manual DAW Testing

<critical_sequence enforcement="strict">
**You MUST execute these steps in exact order:**

<step id="read_guide">
**Step 1: Read DAW Testing Guide**

You MUST read `references/daw-testing-guide.md` for complete manual testing procedures.
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
```

**Verification:** Confirm `<critical_sequence>` with 5 `<step>` tags, step 4 includes WAIT directive

---

### Fix 1.5: Wrap Phase 4 Investigation in `<delegation_rule>`
**Location:** Lines 336-378
**Operation:** WRAP
**Before:**
```markdown
## Phase 4: Failure Investigation (Option 1)

**When user chooses "Investigate failures":**

**For each failed test:**

1. Read detailed explanation from `references/troubleshooting.md`
2. Provide specific fix recommendations
3. Offer to show relevant code sections
4. If available (Phase 7), launch `Task` agent with `deep-research` to find root cause
```

**After:**
```markdown
## Phase 4: Failure Investigation (Option 1)

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
```

**Verification:** Confirm `<delegation_rule>` tags with `<handoff_protocol>` nested inside, includes WAIT directive

---

### Fix 1.6: Wrap Phase 5 State Updates in `<state_requirement>`
**Location:** Lines 383-414
**Operation:** WRAP
**Before:**
```markdown
## Phase 5: Log Test Results

**Save detailed logs for every test run:**

**Location:** `logs/[PluginName]/test_[timestamp].log`

**Log format:**
[...]

**Update state files:**

- `.continue-here.md` → Mark testing complete, update next step
- `PLUGINS.md` → Update test status (last tested date, results)
```

**After:**
```markdown
## Phase 5: Log Test Results

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
```

**Verification:** Confirm `<state_requirement>` tags with 3 nested `<requirement>` tags, includes VERIFY directive

---

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Extract Decision Menus to Assets
**Location:** Lines 38-58, 132-140, 239-247, 289-333
**Operation:** EXTRACT
**Create file:** `assets/decision-menu-templates.md`
**File content:**
```markdown
# Decision Menu Templates

## Mode Selection Menu {#mode-selection}

```
How would you like to test {PLUGIN_NAME}?

1. Automated stability tests (~2 min)
   → Quick C++ unit tests (requires Tests/ directory)
   → Validates core functionality and edge cases
   → Best for: plugins with test infrastructure

2. Build + pluginval (~5-10 min) ⭐ RECOMMENDED
   → Industry-standard validation at strictness level 10
   → Tests VST3 and AU formats
   → Catches 99% of issues before DAW testing
   → Required before distribution

3. Manual DAW testing (~30-60 min)
   → Hands-on testing in your DAW
   → Custom checklist based on plugin features
   → Best for: final validation and sound quality checks

4. Skip testing (NOT RECOMMENDED)
   → Proceed to installation without validation
   → Use only if you've already tested externally

Choose (1-4): _
```

## Missing Tests Directory Menu {#missing-tests}

```
Tests/ directory not found for {PLUGIN_NAME}.

Mode 1 requires C++ test infrastructure (GTest/Catch2).

Recommended: Use Mode 2 (pluginval) instead - it's faster and more comprehensive.

Would you like to:
1. Switch to Mode 2 (Build + Pluginval) ⭐ RECOMMENDED
2. Set up test infrastructure first
3. Cancel testing

Choose (1-3): _
```

## Pluginval Installation Menu {#pluginval-install}

```
Pluginval not found on system.

Pluginval is required for Mode 2 testing. Install via:
- Download: https://github.com/Tracktion/pluginval/releases
- Homebrew: brew install pluginval

Would you like to:
1. Install via Homebrew (recommended)
2. Download manually
3. Switch to Mode 1 (Automated Tests)
4. Switch to Mode 3 (Manual DAW Testing)
5. Cancel testing

Choose (1-5): _
```

## Post-Test Menu (Mode 1 Success) {#post-test-mode1}

```
✓ All automated tests passed ({PASS_COUNT}/{TOTAL_COUNT})

What would you like to do?

1. Investigate results (view detailed logs)
2. Run Mode 2 (pluginval) for comprehensive validation ⭐ RECOMMENDED
3. Continue to next stage (Stage 6: Installation)
4. Return to main workflow

Choose (1-4): _
```

## Post-Test Menu (Mode 1 Failure) {#post-test-mode1-fail}

```
✗ {FAIL_COUNT}/{TOTAL_COUNT} tests failed

What would you like to do?

1. Investigate failures (detailed analysis) ⭐ RECOMMENDED
2. View failed test logs
3. Re-run tests after manual fixes
4. Skip failures and continue (NOT RECOMMENDED)

Choose (1-4): _
```

## Post-Test Menu (Mode 2 Success) {#post-test-mode2}

```
✓ Pluginval validation passed (strictness level 10)

VST3: {VST3_RESULT}
AU: {AU_RESULT}

What's next?

1. Continue to Stage 6 (Installation) ⭐ RECOMMENDED
2. Run Mode 1 (Automated Tests) for additional coverage
3. Run Mode 3 (Manual DAW Testing)
4. View detailed pluginval logs
5. Return to main workflow

Choose (1-5): _
```

## Post-Test Menu (Mode 2 Failure) {#post-test-mode2-fail}

```
✗ Pluginval validation failed

VST3: {VST3_RESULT}
AU: {AU_RESULT}

Common failures: {FAILURE_SUMMARY}

What would you like to do?

1. Investigate failures (deep analysis) ⭐ RECOMMENDED
2. View full pluginval output
3. Re-run after manual fixes
4. Skip failures and continue (NOT RECOMMENDED)

Choose (1-4): _
```

## Post-Test Menu (Mode 3 Complete) {#post-test-mode3}

```
✓ Manual DAW testing complete

Results: {USER_REPORTED_RESULTS}

What's next?

1. Continue to Stage 6 (Installation) ⭐ RECOMMENDED
2. Run Mode 2 (pluginval) for validation
3. Document issues found
4. Return to main workflow

Choose (1-4): _
```

## Failure Investigation Menu {#failure-investigation}

```
Investigating {TEST_NAME} failure...

Analysis: {BRIEF_SUMMARY}

What would you like to do?

1. See detailed explanation and fix recommendations
2. View relevant code sections
3. Delegate to deep-research for root cause analysis
4. Apply suggested fix
5. Skip this failure

Choose (1-5): _
```
```

**Replace in SKILL.md (Line 38):**
```markdown
You MUST present mode selection menu (use template from `assets/decision-menu-templates.md#mode-selection`, substitute {PLUGIN_NAME}).

WAIT for user response. BLOCK execution until choice made.
```

**Replace in SKILL.md (Line 88):**
```markdown
If Tests/ directory missing, you MUST present menu from `assets/decision-menu-templates.md#missing-tests` and WAIT for user response.
```

**Replace in SKILL.md (Line 132):**
```markdown
Present post-test menu from `assets/decision-menu-templates.md#post-test-mode1` (use success or fail variant based on results). WAIT for user selection.
```

**Replace in SKILL.md (Line 239):**
```markdown
Present post-test menu from `assets/decision-menu-templates.md#post-test-mode2` (use success or fail variant based on results). WAIT for user selection.
```

**Replace in SKILL.md (Line 289):**
```markdown
Present post-test menu from `assets/decision-menu-templates.md#post-test-mode3`. WAIT for user selection.
```

**Verification:** File exists with 9 menu templates, SKILL.md reduced by ~80 lines, all menu references point to correct anchors

---

### Fix 2.2: Extract Report Templates to Assets
**Location:** Lines 107-140, 212-247
**Operation:** EXTRACT
**Create file:** `assets/report-templates.md`
**File content:**
```markdown
# Report Templates

## Mode 1 Test Results (Pass) {#mode1-results-pass}

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  TEST RESULTS: {PLUGIN_NAME}
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

✓ All tests passed ({PASS_COUNT}/{TOTAL_COUNT})

Executed tests:
{TEST_LIST}

Duration: {DURATION}
Timestamp: {TIMESTAMP}

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## Mode 1 Test Results (Fail) {#mode1-results-fail}

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  TEST RESULTS: {PLUGIN_NAME}
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

✗ {FAIL_COUNT}/{TOTAL_COUNT} tests failed

Passed tests:
{PASSED_TEST_LIST}

Failed tests:
{FAILED_TEST_LIST}

Duration: {DURATION}
Timestamp: {TIMESTAMP}

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## Mode 2 Pluginval Results (Pass) {#mode2-results-pass}

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  PLUGINVAL VALIDATION: {PLUGIN_NAME}
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

✓ All formats passed (strictness level 10)

VST3: ✅ PASSED
  Location: {VST3_PATH}
  Tests run: {VST3_TEST_COUNT}
  Duration: {VST3_DURATION}

AU: ✅ PASSED
  Location: {AU_PATH}
  Tests run: {AU_TEST_COUNT}
  Duration: {AU_DURATION}

Timestamp: {TIMESTAMP}

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## Mode 2 Pluginval Results (Fail) {#mode2-results-fail}

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  PLUGINVAL VALIDATION: {PLUGIN_NAME}
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

✗ Validation failed (strictness level 10)

VST3: {VST3_STATUS}
  Location: {VST3_PATH}
  {VST3_FAILURES}

AU: {AU_STATUS}
  Location: {AU_PATH}
  {AU_FAILURES}

Common issues detected:
{FAILURE_SUMMARY}

Timestamp: {TIMESTAMP}

Full logs: logs/{PLUGIN_NAME}/pluginval_{TIMESTAMP}.log

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## Mode 3 Completion Message {#mode3-completion}

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  MANUAL DAW TESTING: {PLUGIN_NAME}
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Checklist generated based on:
- Parameter spec: {PARAM_COUNT} parameters
- Plugin type: {PLUGIN_TYPE}
- Features: {FEATURE_LIST}

Testing checklist:
{CHECKLIST_ITEMS}

Instructions:
1. Load plugin in your DAW
2. Test each item in the checklist
3. Report back with results (pass/fail per item)

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

## Test Log Format {#test-log-format}

```
TEST LOG: {PLUGIN_NAME}
═══════════════════════════════════════

Timestamp: {TIMESTAMP}
Mode: {TEST_MODE}
Plugin Status: {STATUS}

Test Configuration:
- Build type: {BUILD_TYPE}
- Formats tested: {FORMATS}
- Strictness level: {STRICTNESS}

Results:
{DETAILED_RESULTS}

Next Steps:
{NEXT_STEPS}

═══════════════════════════════════════
```
```

**Replace in SKILL.md (Line 107):**
```markdown
Generate test results report using template from `assets/report-templates.md#mode1-results-pass` or `#mode1-results-fail` based on outcome.
```

**Replace in SKILL.md (Line 212):**
```markdown
Generate pluginval results report using template from `assets/report-templates.md#mode2-results-pass` or `#mode2-results-fail` based on outcome.
```

**Verification:** File exists with 7 report templates, SKILL.md reduced by ~60 lines

---

### Fix 2.3: Extract Failure Investigation Guide to References
**Location:** Lines 336-378
**Operation:** EXTRACT
**Create file:** `references/failure-investigation-guide.md`
**File content:**
```markdown
# Failure Investigation Guide

## Overview

When tests fail, systematic investigation is required to identify root causes and apply fixes.

## Investigation Procedure

### Step 1: Read Troubleshooting Documentation

Check `references/troubleshooting.md` for known issues matching the failure:

1. Search by test name (e.g., "ParameterChangeTest")
2. Search by error message (e.g., "assertion failed")
3. Search by symptom (e.g., "crash on parameter change")

If documented solution exists:
- Present fix recommendation to user
- Offer to apply fix or show relevant code
- Link to troubleshooting doc for context

### Step 2: Code Inspection

If issue not documented, offer to inspect relevant code:

Common locations by failure type:
- **Parameter failures:** `PluginProcessor.cpp` → `getStateInformation`, `setStateInformation`, APVTS setup
- **DSP failures:** `PluginProcessor.cpp` → `processBlock`, `prepareToPlay`
- **GUI failures:** `PluginEditor.cpp` → constructor, `paint`, `resized`
- **Initialization failures:** `PluginProcessor.cpp` → constructor, destructor

Present code snippets to user with identified issues.

### Step 3: Delegate to Deep Research (Non-Trivial Issues)

If issue is complex or root cause unclear, delegate to `deep-research` skill.

**When to delegate:**
- Error message not in troubleshooting docs
- Multiple interconnected failures
- Requires JUCE API expertise
- Needs cross-file analysis

**Handoff protocol:**
```
Task: "Investigate [test_name] failure in [PluginName]

Test output:
[exact error message or failure log]

Plugin info:
- Type: [from creative-brief.md - e.g., delay effect, reverb, etc.]
- DSP features: [list from parameter-spec.md]
- Parameters: [count and types]

Relevant files:
- plugins/{PluginName}/Source/PluginProcessor.{h,cpp}
- plugins/{PluginName}/Source/PluginEditor.{h,cpp}
[add other relevant files]

Goal: Find root cause of test failure and provide actionable fix with code changes"
```

**IMPORTANT:** Wait for deep-research to complete before presenting results to user. Do NOT attempt to fix complex issues without delegation.

### Step 4: Present Findings

After investigation (manual or delegated), present:

1. **Root cause analysis:**
   - What is failing
   - Why it's failing
   - Where in the code

2. **Fix recommendations:**
   - Specific code changes required
   - Files to modify
   - Testing approach to verify fix

3. **Decision menu:**
   - Apply fix automatically (if straightforward)
   - Show code and let user fix manually
   - Document issue for later
   - Skip and continue (not recommended)

## Common Failure Patterns

### Pattern: Assertion Failures

**Symptom:** Test fails with "assertion failed" message

**Investigation:**
1. Check which assertion failed (line number in test file)
2. Inspect preconditions: what state is required?
3. Check if plugin meets preconditions

**Common causes:**
- APVTS not initialized properly
- Parameters out of range
- State information incomplete

### Pattern: Segmentation Faults

**Symptom:** Test crashes with segfault

**Investigation:**
1. Identify crash location (use debugger or stack trace)
2. Check for null pointer dereferences
3. Check for out-of-bounds array access

**Common causes:**
- Uninitialized pointers
- Deleted objects still referenced
- Buffer size mismatches

### Pattern: Timeout Failures

**Symptom:** Test exceeds time limit

**Investigation:**
1. Check if plugin is blocking (mutex deadlock)
2. Check if processing is too slow
3. Check for infinite loops

**Common causes:**
- Thread synchronization issues
- Inefficient DSP algorithms
- Resource contention

## Example Investigation

**Scenario:** Pluginval fails with "getStateInformation returned empty data"

**Step 1 - Check troubleshooting.md:**
Found entry: "Empty state data usually means APVTS not saved"

**Step 2 - Inspect code:**
```cpp
void PluginProcessor::getStateInformation(MemoryBlock& destData)
{
    // BUG: Not saving APVTS state!
}
```

**Step 3 - Fix recommendation:**
```cpp
void PluginProcessor::getStateInformation(MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}
```

**Step 4 - Present to user:**
"Found issue: `getStateInformation` is empty, not saving APVTS state. This is required for pluginval to pass. Would you like me to apply the fix?"
```

**Replace in SKILL.md (Lines 336-378):**
```markdown
## Phase 4: Failure Investigation (Option 1)

<delegation_rule enforcement="strict">
**When user chooses "Investigate failures":**

You MUST follow the investigation procedure from `references/failure-investigation-guide.md`:

1. Check troubleshooting.md for known issues
2. Inspect relevant code sections
3. For non-trivial issues: Delegate to deep-research (see guide for handoff protocol)
4. Present findings with decision menu

**See `references/failure-investigation-guide.md` for detailed procedures and examples.**
</delegation_rule>
```

**Verification:** File exists at `references/failure-investigation-guide.md`, SKILL.md reduced by ~40 lines

---

### Fix 2.4: Deduplicate Bash Snippets (Reference Only)
**Location:** Lines 80-86, 92-97, 151-159, 181-204
**Operation:** REPLACE (remove inline bash, reference existing files)

**Line 80-86 (Mode 1 Tests check):**
**Before:**
```markdown
```bash
if [ ! -d "plugins/$PLUGIN_NAME/Tests/" ]; then
    echo "No Tests/ directory found"
    # Inform user about Mode 2 alternative
fi
```
```

**After:**
```markdown
You MUST check for Tests/ directory (see `references/test-specifications.md#prerequisites`).

If missing, present menu from `assets/decision-menu-templates.md#missing-tests`.
```

**Line 151-159 (Pluginval check):**
**Before:**
```markdown
```bash
# Check for pluginval.app first (standard macOS install), then fall back to PATH
if [ -x "/Applications/pluginval.app/Contents/MacOS/pluginval" ]; then
    PLUGINVAL_PATH="/Applications/pluginval.app/Contents/MacOS/pluginval"
elif command -v pluginval >/dev/null 2>&1; then
    PLUGINVAL_PATH="pluginval"
else
    PLUGINVAL_PATH=""
fi
```
```

**After:**
```markdown
You MUST check for pluginval installation using script from `references/pluginval-guide.md#installation-check`.
```

**Line 181-204 (Pluginval execution):**
**Before:**
```markdown
```bash
"${PLUGINVAL_PATH}" --validate "$VST3_PATH" --strictness-level 10 --timeout-ms 30000 --verbose
```

```bash
"${PLUGINVAL_PATH}" --validate "$AU_PATH" --strictness-level 10 --timeout-ms 30000 --verbose
```
```

**After:**
```markdown
You MUST run pluginval validation using commands from `references/pluginval-guide.md#execution`.
```

**Verification:** Inline bash snippets removed, all execution details referenced from existing reference files, SKILL.md reduced by ~30 lines

---

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Convert "See:" to Explicit "Read" Commands
**Location:** Lines 76, 146, 253
**Operation:** REPLACE

**Line 76:**
**Before:**
```markdown
**See:** `references/test-specifications.md` for detailed test implementations
```

**After:**
```markdown
You MUST read `references/test-specifications.md` for detailed test implementations before executing Mode 1.
```

**Line 146:**
**Before:**
```markdown
**See:** `references/pluginval-guide.md` for detailed implementation
```

**After:**
```markdown
You MUST read `references/pluginval-guide.md` for detailed implementation before executing Mode 2.
```

**Line 253:**
**Before:**
```markdown
**See:** `references/daw-testing-guide.md` for detailed checklist
```

**After:**
```markdown
You MUST read `references/daw-testing-guide.md` for detailed checklist before executing Mode 3.
```

**Verification:** All "See:" references replaced with "You MUST read"

---

### Fix 3.2: Add Explicit Requirement Markers (MUST/SHOULD)
**Location:** Lines 88, 205, 262, 390, 397, 411-414
**Operation:** REPLACE

**Line 88:**
**Before:**
```markdown
Inform user that automated tests require test infrastructure
```

**After:**
```markdown
You MUST inform user that automated tests require test infrastructure and suggest Mode 2 (pluginval) as recommended alternative.
```

**Line 205:**
**Before:**
```markdown
Parse pluginval output
```

**After:**
```markdown
You MUST parse pluginval output according to rules in `references/pluginval-guide.md#parsing-output`.
```

**Line 262:**
**Before:**
```markdown
Generate customized checklist
```

**After:**
```markdown
You MUST generate customized checklist from `parameter-spec.md` tailored to plugin's specific parameters and features.
```

**Line 390:**
**Before:**
```markdown
Save detailed logs for every test run
```

**After:**
```markdown
You MUST save detailed logs for every test run to `logs/[PluginName]/test_[timestamp].log`.
```

**Line 397:**
**Before:**
```markdown
Log format:
```

**After:**
```markdown
You MUST use log format from `assets/report-templates.md#test-log-format`:
```

**Line 411-414:**
**Before:**
```markdown
**Update state files:**

- `.continue-here.md` → Mark testing complete, update next step
- `PLUGINS.md` → Update test status (last tested date, results)
```

**After:**
```markdown
You MUST update both state files before presenting decision menu:

- `.continue-here.md` → Mark testing complete, update next step
- `PLUGINS.md` → Update test status (last tested date, results)

VERIFY both files updated.
```

**Verification:** All imperative statements use explicit requirement markers (MUST/SHOULD)

---

### Fix 3.3: Add WAIT/BLOCK Markers for User Input
**Location:** Lines 58, 140, 247, 333
**Operation:** INSERT (add after decision menu presentations)

**Line 58 (after mode selection menu):**
**Before:**
```markdown
Choose (1-4): _
```

**After:**
```markdown
Choose (1-4): _

**WAIT for user response. BLOCK execution until mode selected.**
```

**Line 140 (after Mode 1 result menu):**
**Before:**
```markdown
Choose (1-4): _
```

**After:**
```markdown
Choose (1-4): _

**WAIT for user selection before proceeding.**
```

**Line 247 (after Mode 2 result menu):**
**Before:**
```markdown
Choose (1-5): _
```

**After:**
```markdown
Choose (1-5): _

**WAIT for user selection before proceeding.**
```

**Line 333 (after Mode 3 result menu):**
**Before:**
```markdown
Choose (1-4): _
```

**After:**
```markdown
Choose (1-4): _

**WAIT for user selection before proceeding.**
```

**Verification:** All decision menus followed by explicit WAIT directive

---

### Fix 3.4: Improve YAML Description for Autonomous Activation
**Location:** Lines 1-11
**Operation:** REPLACE

**Before:**
```yaml
---
name: plugin-testing
description: Automated stability tests and validation suite
version: 1.0.0
category: validation
tags:
  - testing
  - validation
  - quality-assurance
  - automated-tests
---
```

**After:**
```yaml
---
name: plugin-testing
description: Run automated stability tests, pluginval validation, and DAW testing for audio plugins. Invoke when user mentions test, validate, validation, pluginval, stability, automated tests, run tests, check plugin, or quality assurance
version: 1.0.0
category: validation
tags:
  - testing
  - validation
  - quality-assurance
  - automated-tests
---
```

**Verification:** Description includes trigger keywords for autonomous activation

---

## File Creation Manifest

### Files to create:
1. `assets/decision-menu-templates.md` - 9 decision menu formats with placeholders for mode selection, post-test actions, and failure investigation
2. `assets/report-templates.md` - 7 report templates for test results, pluginval output, and log formatting
3. `references/failure-investigation-guide.md` - Detailed investigation procedures with examples and handoff protocols

### Files to modify:
1. `SKILL.md` - Add XML tags, replace inline content with references, add explicit requirement markers

---

## Execution Checklist

### Phase 1: Critical Fixes (45 minutes)
- [ ] Fix 1.1: Phase 1 wrapped in `<decision_gate>` with strict enforcement
- [ ] Fix 1.2: Mode 1 wrapped in `<critical_sequence>` with 5 steps and dependencies
- [ ] Fix 1.3: Mode 2 wrapped in `<critical_sequence>` with 5 steps and dependencies
- [ ] Fix 1.4: Mode 3 wrapped in `<critical_sequence>` with 5 steps and dependencies
- [ ] Fix 1.5: Phase 4 wrapped in `<delegation_rule>` with handoff protocol
- [ ] Fix 1.6: Phase 5 wrapped in `<state_requirement>` with 3 requirements
- [ ] Verify: All XML tags properly nested and closed
- [ ] Verify: All `<step>` tags have `depends_on` attributes (except first step)
- [ ] Verify: All blocking points have explicit WAIT or BLOCK directives

### Phase 2: Content Extraction (30 minutes)
- [ ] Fix 2.1: Created `assets/decision-menu-templates.md` with 9 menu templates
- [ ] Fix 2.1: All menu references in SKILL.md point to correct template anchors
- [ ] Fix 2.2: Created `assets/report-templates.md` with 7 report templates
- [ ] Fix 2.2: All report generation references template anchors
- [ ] Fix 2.3: Created `references/failure-investigation-guide.md` with procedures
- [ ] Fix 2.4: Removed inline bash snippets, references existing guide files
- [ ] Verify: SKILL.md reduced by ~210 lines (80 + 60 + 40 + 30)
- [ ] Verify: All extracted content has {PLACEHOLDER} syntax for substitution
- [ ] Verify: All references use correct file paths and anchor links

### Phase 3: Polish (20 minutes)
- [ ] Fix 3.1: All "See:" references converted to "You MUST read"
- [ ] Fix 3.2: All imperative statements have MUST/SHOULD markers
- [ ] Fix 3.3: All decision menus followed by WAIT/BLOCK directives
- [ ] Fix 3.4: YAML description includes trigger keywords
- [ ] Verify: No ambiguous imperatives remain
- [ ] Verify: All user interaction points have explicit WAIT directives
- [ ] Verify: Description supports autonomous skill activation

### Final Validation
- [ ] Run through each mode mentally: can Claude execute without ambiguity?
- [ ] Check all file paths are correct (assets/, references/)
- [ ] Check all anchor links work (#{anchor-name})
- [ ] Verify state requirements prevent "lost state" bugs
- [ ] Verify delegation rules enforce handoff protocol
- [ ] Test skill invocation with natural language trigger phrases
- [ ] Confirm estimated 1,120 token reduction achieved
- [ ] Document any deviations from plan in implementation notes

---

## Estimated Impact Summary

### Token Reduction
- Decision menus extracted: **640 tokens**
- Report templates extracted: **480 tokens**
- Investigation guide extracted: **340 tokens**
- Bash snippets deduplicated: **400 tokens**
- **Total extracted:** 1,860 tokens

- XML tags added: **~740 tokens**
- **Net reduction:** **1,120 tokens (29% reduction)**

### Comprehension Improvement
- **Before:** Skill "suggests" workflows with implicit instructions
- **After:** Skill "enforces" workflows with explicit XML structure
- **Impact:** Eliminates ambiguity about required vs optional steps, execution order, blocking points, and delegation rules

### Maintenance Improvement
- **Single source of truth:** Templates in assets/, not duplicated inline
- **Clear structure:** XML tags make enforcement points obvious
- **Easier to extend:** New modes follow same XML pattern

---

## Implementation Notes

### Execution Order
1. **Start with Phase 1 (XML enforcement)** - This is correctness-critical
2. **Validate behavior** - Test skill with /test command after Phase 1
3. **Proceed to Phase 2 (extraction)** - This is optimization
4. **Finish with Phase 3 (polish)** - Final clarity improvements

### Testing Strategy
After each phase:
1. Read SKILL.md to verify syntax (no broken XML tags)
2. Invoke skill with `/test [PluginName]` to verify execution
3. Verify Claude follows enforcement rules (blocks, waits, delegates)
4. Check state files updated correctly

### Rollback Plan
If issues arise:
- Phase 3 can be rolled back independently (just polish)
- Phase 2 can be rolled back by reverting extractions (restore inline content)
- Phase 1 should NOT be rolled back (enforcement is critical)

### Known Risks
- **XML tag nesting:** Carefully verify opening/closing tags match
- **Anchor links:** Test all `#anchor-name` links work after extraction
- **Placeholder substitution:** Ensure templates use consistent {PLACEHOLDER} syntax
- **File paths:** Verify all paths relative to .claude/skills/plugin-testing/

---

## Success Criteria

The fix plan successfully addresses:
- ✅ All 2 critical issues (XML organization and enforcement)
- ✅ All 6 optimization opportunities (extractions and clarity)
- ✅ 100% of analysis recommendations
- ✅ Provides mechanical, unambiguous instructions
- ✅ Requires zero additional analysis or decision-making
- ✅ Can be executed by someone unfamiliar with the skill
- ✅ Includes verification steps for every change
- ✅ Maintains exact line number references from analysis report
