# Skill Analysis: plugin-testing

## Executive Summary
- Overall health: **Yellow** (functional but has critical deficiencies in enforcement)
- Critical issues: **2** (XML organization missing, critical sequence enforcement absent)
- Optimization opportunities: **6** major opportunities
- Estimated context savings: **1,120 tokens (29% reduction)**

## Dimension Scores (1-5)
1. Structure Compliance: **4**/5 (YAML good, description could include trigger terms)
2. Content Extraction: **3**/5 (good extractions done, but templates/menus remain inline)
3. Instruction Clarity: **4**/5 (clear structure, but lacks explicit MUST/SHOULD markers)
4. XML Organization: **1**/5 ⚠️ **CRITICAL** (zero XML tags for critical sections)
5. Context Efficiency: **3**/5 (significant duplication of decision menus and examples)
6. Claude-Optimized Language: **3**/5 (generally clear but uses implicit instructions)
7. System Integration: **3**/5 (follows patterns but lacks enforcement)
8. Critical Sequence Enforcement: **1**/5 ⚠️ **CRITICAL** (no XML wrappers to enforce workflows)

## Critical Issues (blockers for Claude comprehension)

### Issue 1: Zero XML enforcement of critical sequences (Lines 1-473)
**Severity:** Critical
**Location:** Entire SKILL.md
**Problem:** The skill contains multiple critical sequences that MUST execute in order, decision gates that MUST block execution, and delegation rules that MUST be followed, but NONE are wrapped in XML enforcement tags.

**Specific sequences lacking enforcement:**
- Lines 29-71: Phase 1 detection/mode selection (decision gate)
- Lines 74-142: Mode 1 execution (critical sequence)
- Lines 144-248: Mode 2 execution (critical sequence with prerequisites)
- Lines 252-286: Mode 3 execution (critical sequence)
- Lines 336-378: Failure investigation handoff (delegation rule)
- Lines 383-414: Log creation and state updates (state requirement)

**Risk:** Claude may skip steps, execute out of order, or fail to update state files.

### Issue 2: No XML organization for semantic sections (Lines 1-473)
**Severity:** Critical
**Location:** Entire SKILL.md
**Problem:** Related to Issue 1 - the skill lacks XML tags like `<instructions>`, `<decision_gate>`, `<critical_sequence>`, `<delegation_rule>`, `<state_requirement>` that would provide semantic structure and enforcement.

**Risk:** Reduces Claude's ability to understand workflow structure and enforce invariants.

## Optimization Opportunities

### Content to Extract to references/

#### 1. Failure Investigation Procedures
**Lines:** 336-378 (~42 lines)
**Description:** Phase 4 investigation details with example output
**Target:** `references/failure-investigation-guide.md`
**Estimated token savings:** ~340 tokens

**Rationale:** Detailed investigation examples with code snippets are reference material, not core workflow instructions.

### Content to Extract to assets/

#### 2. Decision Menu Templates
**Lines:** 38-58, 132-140, 239-247, 289-333 (~80 lines total when deduplicated)
**Description:** Repeated decision menu formats for mode selection and post-test actions
**Target:** `assets/decision-menu-templates.md`
**Estimated token savings:** ~640 tokens

**Example structure:**
```markdown
# Decision Menu Templates

## Mode Selection Menu
[Template with {PLUGIN_NAME} placeholder]

## Post-Test Menu (Mode 1 Success)
[Template]

## Post-Test Menu (Mode 2 Success)
[Template]

## Failure Investigation Menu
[Template]
```

#### 3. Test Report Format Templates
**Lines:** 107-140, 212-247 (~60 lines)
**Description:** Detailed pass/fail report formats with examples
**Target:** `assets/report-templates.md`
**Estimated token savings:** ~480 tokens

**Rationale:** These are output templates that Claude can reference when generating reports, not workflow instructions.

### XML Tag Opportunities

#### 4. Wrap Phase 1 Detection in `<decision_gate>`
**Lines:** 29-71
**Example transformation:**

**BEFORE:**
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

**AFTER:**
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

**Rationale:** Ensures Claude completes all detection steps before offering choices, and blocks execution until user responds.

#### 5. Wrap Mode 2 Execution in `<critical_sequence>`
**Lines:** 144-248
**Example transformation:**

**BEFORE:**
```markdown
### Mode 2: Build + Pluginval

**See:** `references/pluginval-guide.md` for detailed implementation

**Prerequisites check:**

```bash
# Check for pluginval.app first (standard macOS install), then fall back to PATH
if [ -x "/Applications/pluginval.app/Contents/MacOS/pluginval" ]; then
    PLUGINVAL_PATH="/Applications/pluginval.app/Contents/MacOS/pluginval"
...
```

**AFTER:**
```markdown
### Mode 2: Build + Pluginval

<critical_sequence enforcement="strict">
**You MUST execute these steps in exact order:**

<step id="prereq_check">
**Step 1: Prerequisites Check**

You MUST read `references/pluginval-guide.md` for implementation details.

Check for pluginval installation:
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

**If pluginval not found:** You MUST present installation decision menu (see `assets/decision-menu-templates.md#pluginval-install`) and WAIT for user response. BLOCK execution until resolved.
</step>

<step id="build" depends_on="prereq_check">
**Step 2: Build Release Binaries**

You MUST build in Release mode:
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
```

Locate binaries:
```bash
VST3_PATH="build/plugins/$PLUGIN_NAME/${PLUGIN_NAME}_artefacts/Release/VST3/${PRODUCT_NAME}.vst3"
AU_PATH="build/plugins/$PLUGIN_NAME/${PLUGIN_NAME}_artefacts/Release/AU/${PRODUCT_NAME}.component"
```
</step>

<step id="validate" depends_on="build">
**Step 3: Run Pluginval**

You MUST validate both formats at strictness level 10:
```bash
"${PLUGINVAL_PATH}" --validate "$VST3_PATH" --strictness-level 10 --timeout-ms 30000 --verbose
"${PLUGINVAL_PATH}" --validate "$AU_PATH" --strictness-level 10 --timeout-ms 30000 --verbose
```
</step>

<step id="parse" depends_on="validate">
**Step 4: Parse Results**

You MUST parse pluginval output (see `references/pluginval-guide.md#parsing-output`) and generate report using template from `assets/report-templates.md#pluginval-results`.
</step>

<step id="present" depends_on="parse">
**Step 5: Present Results and Next Steps**

You MUST present post-test decision menu (see `assets/decision-menu-templates.md#post-test-mode2`) and WAIT for user selection.
</step>
</critical_sequence>
```

**Rationale:** Enforces strict step ordering with dependencies, prevents Claude from skipping prerequisite checks or build steps.

#### 6. Wrap State Updates in `<state_requirement>`
**Lines:** 383-414
**Example transformation:**

**BEFORE:**
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

**AFTER:**
```markdown
## Phase 5: Log Test Results

<state_requirement enforcement="strict">
**After completing ANY test mode (1, 2, or 3), you MUST:**

<requirement id="save_log">
Save detailed test log to: `logs/[PluginName]/test_[timestamp].log`

Use format from `assets/report-templates.md#test-log-format`
</requirement>

<requirement id="update_continue">
Update `.continue-here.md`:
- Mark current stage as complete
- Set next_step based on test results
- Record test date and mode used
</requirement>

<requirement id="update_plugins">
Update `PLUGINS.md`:
- Set test status (✅ passed / ❌ failed)
- Record last tested date
- Update test mode used (Mode 1/2/3)
</requirement>

**You MUST complete ALL three requirements before proceeding to decision menu.**
</state_requirement>
```

**Rationale:** Prevents Claude from forgetting to update state files, which would break workflow continuity.

### Instruction Clarity Improvements

#### 7. Convert Implicit "See:" to Explicit "Read" Commands
**Multiple locations:** Lines 76, 146, 253, 465-468

**BEFORE:**
```markdown
**See:** `references/test-specifications.md` for detailed test implementations
```

**AFTER:**
```markdown
**You MUST read** `references/test-specifications.md` for detailed test implementations before proceeding.
```

**Rationale:** "See" is passive/optional, "You MUST read" is explicit/required.

#### 8. Add Explicit Requirement Markers Throughout
**Multiple locations:** Lines 88, 205, 262

**Examples:**

Line 88:
- BEFORE: `Inform user that automated tests require test infrastructure`
- AFTER: `You MUST inform user that automated tests require test infrastructure`

Line 205:
- BEFORE: `Parse pluginval output`
- AFTER: `You MUST parse pluginval output`

Line 262:
- BEFORE: `Generate customized checklist`
- AFTER: `You MUST generate customized checklist from parameter-spec.md`

**Rationale:** Removes ambiguity about whether actions are required or optional.

### Verbosity Reductions

#### 9. Extract Bash Code Snippets to References
**Lines:** 80-86, 92-97, 151-159, 181-204

**Current:** Inline bash code in SKILL.md (duplicated across skill and references)
**Proposed:** Reference bash snippets from `references/` files only

**Example:**

**BEFORE (Lines 151-159):**
```markdown
**Prerequisites check:**

```bash
# Check for pluginval.app first (standard macOS install), then fall back to PATH
if [ -x "/Applications/pluginval.app/Contents/MacOS/pluginval" ]; then
    PLUGINVAL_PATH="/Applications/pluginval.app/Contents/MacOS/pluginval"
elif command -v pluginval >/dev/null 2>&1; then
    PLUGINVAL_PATH="pluginval"
else
    echo "Pluginval not found"
    PLUGINVAL_PATH=""
fi
```
```

**AFTER:**
```markdown
**Prerequisites check:**

You MUST check for pluginval installation using the script from `references/pluginval-guide.md#installation-check`.

If not found, present installation menu (see `assets/decision-menu-templates.md#pluginval-install`).
```

**Token savings:** ~100 tokens per instance (4 instances = ~400 tokens)

### Critical Sequence Enforcement

#### 10. Add Delegation Rule for Deep-Research Handoff
**Lines:** 336-378

**BEFORE:**
```markdown
## Phase 4: Failure Investigation (Option 1)

**When user chooses "Investigate failures":**

**For each failed test:**

1. Read detailed explanation from `references/troubleshooting.md`
2. Provide specific fix recommendations
3. Offer to show relevant code sections
4. If available (Phase 7), launch `Task` agent with `deep-research` to find root cause
```

**AFTER:**
```markdown
## Phase 4: Failure Investigation (Option 1)

<delegation_rule enforcement="strict">
**When user chooses "Investigate failures":**

<sequence>
1. You MUST read `references/troubleshooting.md` for each failed test
2. Provide initial fix recommendations from troubleshooting docs
3. Offer code inspection option

4. **If issue is non-trivial (not in troubleshooting.md):**

   <handoff_protocol>
   You MUST delegate to `deep-research` skill via Task tool:

   ```
   Task: "Investigate [test_name] failure in [PluginName]

   Context:
   - Test failed: [specific failure message]
   - Plugin type: [from creative-brief.md]
   - Relevant code: [file paths]

   Goal: Find root cause and provide specific fix"
   ```

   WAIT for deep-research completion before presenting results to user.
   </handoff_protocol>
</sequence>
</delegation_rule>
```

**Rationale:** Ensures Claude delegates complex investigations rather than guessing, and waits for results before proceeding.

### YAML Frontmatter Enhancement

#### 11. Improve Description for Autonomous Activation
**Lines:** 1-11

**BEFORE:**
```yaml
description: Automated stability tests and validation suite
```

**AFTER:**
```yaml
description: Run automated stability tests, pluginval validation, and DAW testing for audio plugins. Invoke when user mentions: test, validate, validation, pluginval, stability, automated tests, run tests, check plugin
```

**Rationale:** Includes trigger keywords for autonomous skill activation when users request testing in natural language.

## Examples of Issues Found

### Example 1: Critical Sequence Not Enforced (Most Critical)

**Issue:** Mode 2 execution allows Claude to skip prerequisite checks

**BEFORE (Lines 144-204):**
```markdown
### Mode 2: Build + Pluginval

**See:** `references/pluginval-guide.md` for detailed implementation

**Prerequisites check:**

```bash
[bash code for checking pluginval]
```

**If missing:**
[decision menu]

**Build release binaries:**

```bash
[build commands]
```
```

**Problem:** No enforcement that prerequisites MUST be checked before build, no guarantee Claude won't skip to build step.

**AFTER:**
```markdown
### Mode 2: Build + Pluginval

<critical_sequence enforcement="strict">
<step id="prereq" blocking="true">
**Prerequisites Check (MUST complete before build):**

You MUST read `references/pluginval-guide.md#installation-check` and verify pluginval installation.

If not found: BLOCK and present installation menu. WAIT for resolution.
</step>

<step id="build" depends_on="prereq">
**Build Release Binaries:**

You MUST build in Release mode (see `references/pluginval-guide.md#build-process`).
</step>

<step id="validate" depends_on="build">
**Run Validation:**
[validation steps]
</step>
</critical_sequence>
```

**Impact:** Prevents build failures from missing dependencies, ensures proper workflow order.

### Example 2: Decision Menu Duplication

**Issue:** Decision menu format repeated 4 times with slight variations

**BEFORE (Lines 38-58, 132-140, 239-247, 289-333):**
```markdown
[Lines 38-58]
```
How would you like to test [PluginName]?

1. Automated stability tests (~2 min)
   → Quick C++ unit tests...
[20 lines of menu]
```

[Lines 132-140]
What would you like to do?
1. Investigate failures...
[8 lines of menu]
```

[Lines 239-247]
What would you like to do?
1. Investigate failures...
[8 lines of menu, nearly identical to 132-140]
```

[Lines 289-333]
✓ All automated tests passed (5/5)

What's next?
1. Continue to next stage...
[44 lines across 3 different menus]
```

**Token cost:** ~80 lines total = ~640 tokens

**AFTER:**

Create `assets/decision-menu-templates.md`:
```markdown
# Decision Menu Templates

## Mode Selection
\`\`\`
How would you like to test {PLUGIN_NAME}?

1. Automated stability tests (~2 min)
2. Build + pluginval (~5-10 min) ⭐ RECOMMENDED
3. Manual DAW testing (~30-60 min)
4. Skip testing (NOT RECOMMENDED)

Choose (1-4): _
\`\`\`

## Post-Test Mode 1 Success
[template with placeholders]

## Post-Test Mode 2 Success
[template with placeholders]

## Failure Investigation
[template with placeholders]
```

In SKILL.md (Line 36):
```markdown
<decision_gate>
You MUST present mode selection menu (use template from `assets/decision-menu-templates.md#mode-selection`, substitute {PLUGIN_NAME}).

WAIT for user response. BLOCK execution until choice made.
</decision_gate>
```

**Token savings:** ~640 tokens (inline) - ~50 tokens (reference) = **~590 net savings**

### Example 3: Implicit vs Explicit Instructions

**Issue:** Unclear whether actions are required or optional

**BEFORE (Lines 76, 88, 205):**
```markdown
**See:** `references/test-specifications.md` for detailed test implementations

Inform user that automated tests require test infrastructure

Parse test output:
```

**AFTER:**
```markdown
You MUST read `references/test-specifications.md` for detailed test implementations before executing Mode 1.

You MUST inform user that automated tests require test infrastructure and suggest Mode 2 (pluginval) as alternative.

You MUST parse test output according to these rules:
```

**Impact:** Eliminates ambiguity, Claude knows these are required actions not suggestions.

### Example 4: State Requirements Not Enforced

**Issue:** Claude might skip updating state files

**BEFORE (Lines 411-414):**
```markdown
**Update state files:**

- `.continue-here.md` → Mark testing complete, update next step
- `PLUGINS.md` → Update test status (last tested date, results)
```

**AFTER:**
```markdown
<state_requirement enforcement="strict">
After ANY test completion, you MUST update both state files before presenting decision menu:

<requirement id="continue">
Update `.continue-here.md`:
- current_stage: "testing_complete"
- next_step: [based on test results]
- test_date: [timestamp]
- test_mode: [1/2/3]
</requirement>

<requirement id="plugins">
Update `PLUGINS.md` for {PLUGIN_NAME}:
- test_status: "✅ passed" or "❌ failed"
- last_tested: [date]
- test_mode_used: [Mode 1/2/3]
</requirement>

VERIFY both files updated before proceeding.
</state_requirement>
```

**Impact:** Guarantees workflow continuity, prevents "lost state" bugs.

### Example 5: Delegation Rule Missing

**Issue:** No enforcement of deep-research handoff for complex failures

**BEFORE (Lines 343-344):**
```markdown
4. If available (Phase 7), launch `Task` agent with `deep-research` to find root cause
```

**AFTER:**
```markdown
<delegation_rule enforcement="strict">
**For non-trivial failures (not documented in `references/troubleshooting.md`):**

You MUST delegate investigation to deep-research skill:

<handoff_protocol>
1. Prepare context:
   - Failed test name and error message
   - Plugin name and type (from creative-brief.md)
   - Relevant source files (PluginProcessor.cpp/h)

2. Invoke via Task tool:
   \`\`\`
   Task: "Investigate {test_name} failure in {plugin_name}

   Test output: {error_message}
   Plugin type: {type}
   Files: {file_list}

   Goal: Find root cause and actionable fix"
   \`\`\`

3. WAIT for deep-research completion (do NOT guess at fixes)

4. Present deep-research findings to user with decision menu
</handoff_protocol>

**NEVER attempt to fix complex issues without delegation to deep-research.**
</delegation_rule>
```

**Impact:** Ensures proper handoff protocol, prevents Claude from providing incorrect fixes.

## Estimated Impact

### Context reduction
- **Decision menu extraction:** 640 tokens
- **Report template extraction:** 480 tokens
- **Bash snippet deduplication:** 400 tokens
- **Investigation examples extraction:** 340 tokens
- **Total extracted:** 1,860 tokens

- **XML tags added:** ~740 tokens (40 lines of wrapper tags)

- **Net reduction:** 1,860 - 740 = **1,120 tokens (29% reduction)**

### Comprehension improvement
**HIGH** - XML enforcement eliminates ambiguity about:
- Which steps are required vs optional
- Order of execution (dependencies explicit)
- When to block and wait for user input
- When to delegate vs handle internally
- Which state files must be updated

The skill currently "suggests" workflows but doesn't "enforce" them. XML tags transform suggestions into requirements.

### Maintenance improvement
**HIGH** - Three key benefits:

1. **Single source of truth for templates:** Decision menus and report formats stored in assets/, referenced from SKILL.md. Changes to formats require updating only one file.

2. **Clearer structure for debugging:** When plugin-testing behavior is unexpected, XML tags make it obvious where enforcement should happen. Current structure makes it hard to distinguish "Claude skipped a step" from "step wasn't clear."

3. **Easier to extend:** Adding new test modes requires:
   - Add `<critical_sequence>` wrapper to new mode
   - Add decision menu template to assets/
   - Add report template to assets/
   - Reference both from SKILL.md

   Current approach requires inline changes across multiple locations.

## Priority Recommendations

### 1. Add XML Critical Sequence Enforcement (CRITICAL)
**Estimated time:** 45 minutes

**Actions:**
- Wrap Phase 1 (lines 29-71) in `<decision_gate>` with strict enforcement
- Wrap Mode 1 execution (lines 74-142) in `<critical_sequence>` with step dependencies
- Wrap Mode 2 execution (lines 144-248) in `<critical_sequence>` with step dependencies
- Wrap Mode 3 execution (lines 252-286) in `<critical_sequence>`
- Wrap Phase 4 investigation (lines 336-378) in `<delegation_rule>` with handoff protocol
- Wrap Phase 5 state updates (lines 383-414) in `<state_requirement>` with verification

**Why first:** Without this, Claude can skip critical steps, execute out of order, or forget to update state files. This is the foundational issue blocking reliable workflow execution.

**Success criteria:** All critical workflows have XML enforcement with explicit blocking, dependencies, and verification requirements.

### 2. Extract Decision Menus and Report Templates to Assets (HIGH IMPACT)
**Estimated time:** 30 minutes

**Actions:**
- Create `assets/decision-menu-templates.md` with all menu formats:
  - Mode selection menu
  - Post-test menus (for each mode × pass/fail combinations)
  - Failure investigation menu
  - Installation decision menu
- Create `assets/report-templates.md` with all output formats:
  - Mode 1 test results (pass/fail variations)
  - Mode 2 pluginval results (pass/fail variations)
  - Mode 3 completion message
  - Test log format
- Replace inline menus/reports with references to templates
- Use placeholder syntax: `{PLUGIN_NAME}`, `{TEST_COUNT}`, `{PASS_COUNT}`, etc.

**Why second:** Largest token savings (1,120 tokens / 29% reduction). Makes SKILL.md much more readable and maintainable.

**Success criteria:** All decision menus and report formats use templates from assets/ with placeholder substitution.

### 3. Convert Implicit to Explicit Instructions (MEDIUM IMPACT)
**Estimated time:** 20 minutes

**Actions:**
- Replace all "See: `file.md`" with "You MUST read `file.md` before proceeding"
- Add explicit requirement markers (MUST/SHOULD/NEVER) to all imperative statements
- Convert passive voice ("Parse output") to active with requirement ("You MUST parse output")
- Add "WAIT for X" markers where blocking is required
- Add "VERIFY X before Y" markers where validation needed

**Pattern to find and replace:**
- `See:` → `You MUST read`
- `Parse` → `You MUST parse`
- `Check` → `You MUST check`
- `Verify` → `You MUST verify`
- `Update` → `You MUST update`
- `Present decision menu` → `Present decision menu and WAIT for user response`

**Why third:** Complements XML enforcement by making individual instructions unambiguous. Lower priority than XML because XML provides structural enforcement even with implicit instructions.

**Success criteria:** All action statements use explicit requirement markers, no ambiguous imperatives remain.

## Implementation Notes

### Files to Create in references/
- `references/failure-investigation-guide.md` - Detailed investigation procedures with examples

### Files to Create in assets/
- `assets/decision-menu-templates.md` - All decision menu formats with placeholders
- `assets/report-templates.md` - All test result report formats with placeholders

### Files to Modify
- `SKILL.md` - Add XML tags, replace inline content with references, explicit instructions

### Estimated Time to Implement All Fixes
**Total:** 95 minutes (~1.5 hours)

**Breakdown:**
- XML critical sequence enforcement: 45 min (priority 1)
- Extract templates to assets/: 30 min (priority 2)
- Explicit instruction markers: 20 min (priority 3)

**Recommended approach:** Implement priority 1 first (XML enforcement) and validate skill behavior before proceeding to optimizations (priorities 2-3). The XML enforcement is correctness-critical, while template extraction is optimization.
