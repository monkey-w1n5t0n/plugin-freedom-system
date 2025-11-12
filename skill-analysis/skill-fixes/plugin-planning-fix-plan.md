# Fix Plan: plugin-planning

## Summary
- Critical fixes: **3**
- Extraction operations: **7**
- Total estimated changes: **18**
- Estimated time: **50** minutes

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Add decision gate to Stage 1 contract validation
**Location:** Lines 193-236
**Operation:** WRAP
**Before:**
```markdown
**Preconditions:**

Check for required contracts:
```bash
test -f "plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md" && echo "✓ parameter-spec.md" || echo "✗ parameter-spec.md MISSING"
test -f "plugins/${PLUGIN_NAME}/.ideas/architecture.md" && echo "✓ architecture.md" || echo "✗ architecture.md MISSING"
```

**If parameter-spec.md OR architecture.md is missing:**

STOP IMMEDIATELY and BLOCK with this message:
[...long error message...]
```

**After:**
```markdown
**Preconditions:**

<decision_gate id="stage-1-contract-validation">
<validation_requirement>
MUST verify parameter-spec.md AND architecture.md exist before proceeding. This is a BLOCKING gate.
</validation_requirement>

Check for required contracts:
```bash
PARAM_SPEC_EXISTS=$(test -f "plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md" && echo "true" || echo "false")
ARCH_EXISTS=$(test -f "plugins/${PLUGIN_NAME}/.ideas/architecture.md" && echo "true" || echo "false")

echo "Parameter spec: ${PARAM_SPEC_EXISTS}"
echo "Architecture: ${ARCH_EXISTS}"
```

<blocking_condition>
IF PARAM_SPEC_EXISTS == "false" OR ARCH_EXISTS == "false":
  THEN display error message (see assets/stage-1-blocked.md)
  AND exit skill
ELSE:
  Proceed to complexity calculation
</blocking_condition>
</decision_gate>
```

**Verification:** Check that `<decision_gate>` wrapper exists at line 193 and `</decision_gate>` closes at line ~236

---

### Fix 1.2: Wrap precondition checks in enforcement gate
**Location:** Lines 27-56
**Operation:** WRAP
**Before:**
```markdown
**Check preconditions first:**

1. Verify creative brief exists:
```bash
if [ ! -f "plugins/${PLUGIN_NAME}/.ideas/creative-brief.md" ]; then
    echo "✗ creative-brief.md not found"
    exit 1
fi
```

2. Check plugin status in PLUGINS.md:
```bash
STATUS=$(grep -A 2 "^### ${PLUGIN_NAME}$" PLUGINS.md | grep "Status:" | awk '{print $2}')
```

- If status is 🚧 Stage N where N >= 2: **BLOCK** - Plugin already past planning
- If status is 💡 Ideated or not found: OK to proceed
```

**After:**
```markdown
**Check preconditions first:**

<precondition_gate>
<validation_requirement>
MUST verify all preconditions before proceeding. If ANY check fails, BLOCK and report to user.
</validation_requirement>

1. Verify creative brief exists:
```bash
if [ ! -f "plugins/${PLUGIN_NAME}/.ideas/creative-brief.md" ]; then
    echo "✗ creative-brief.md not found - SKILL BLOCKED"
    exit 1
fi
```

2. Check plugin status in PLUGINS.md:
```bash
STATUS=$(grep -A 2 "^### ${PLUGIN_NAME}$" PLUGINS.md | grep "Status:" | awk '{print $2}')
```

<blocking_condition>
IF status is 🚧 Stage N where N >= 2:
  THEN BLOCK with message "Plugin already past planning stage"
ELSE IF status is 💡 Ideated or not found:
  THEN Proceed
</blocking_condition>
</precondition_gate>
```

**Verification:** Confirm `<precondition_gate>` wrapper present and `<blocking_condition>` uses explicit IF-THEN logic

---

### Fix 1.3: Wrap handoff protocol with verification
**Location:** Lines 385-442
**Operation:** WRAP + MODIFY
**Before:**
```markdown
## Handoff to Implementation

**When user chooses to proceed to Stage 2:**

1. **Delete planning handoff** to prevent confusion (two `.continue-here.md` files):
```bash
rm plugins/${PLUGIN_NAME}/.ideas/.continue-here.md
```

2. **Create implementation handoff** at root that plugin-workflow skill expects:

```bash
# Create at plugins/[PluginName]/.continue-here.md (NOT in .ideas/)
```
```

**After:**
```markdown
## Handoff to Implementation

<handoff_protocol id="planning-to-implementation">
<state_requirement>
CRITICAL: Handoff modifies .continue-here.md location. Execute steps EXACTLY in sequence. File path precision is critical.
</state_requirement>

**When user chooses to proceed to Stage 2:**

<critical_sequence>
1. Delete planning handoff (prevents dual state files):
```bash
rm plugins/${PLUGIN_NAME}/.ideas/.continue-here.md
if [ $? -ne 0 ]; then
    echo "✗ Failed to delete planning handoff"
    exit 1
fi
```

2. Create implementation handoff at plugin root:
```bash
# IMPORTANT: Create at plugin root, NOT in .ideas/
# Use template from assets/implementation-handoff-template.md
cat > plugins/${PLUGIN_NAME}/.continue-here.md <<'EOF'
[template content from assets/implementation-handoff-template.md]
EOF
```

3. Verify handoff:
```bash
test ! -f "plugins/${PLUGIN_NAME}/.ideas/.continue-here.md" || { echo "✗ Old handoff still exists"; exit 1; }
test -f "plugins/${PLUGIN_NAME}/.continue-here.md" || { echo "✗ New handoff not created"; exit 1; }
echo "✓ Handoff verified"
```
</critical_sequence>

<verification_step>
After handoff, verify:
- plugins/[PluginName]/.ideas/.continue-here.md does NOT exist
- plugins/[PluginName]/.continue-here.md DOES exist
- PLUGINS.md status updated to 🚧 Stage 2
</verification_step>
</handoff_protocol>
```

**Verification:** Confirm handoff protocol includes verification bash commands and explicit state requirements

---

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Extract Stage 0 .continue-here.md template
**Location:** Lines 115-145
**Operation:** EXTRACT
**Create file:** `assets/continue-stage-0-template.md`
**File content:**
```yaml
skill: plugin-planning
plugin: ${PLUGIN_NAME}
stage: 0
status: completed
last_updated: ${TIMESTAMP}

progress:
  research_complete: true
  architecture_spec: "plugins/${PLUGIN_NAME}/.ideas/architecture.md"

contracts_created:
  - architecture.md

next_actions:
  - Continue to Stage 1 (Planning)
  - Invoke plugin-planning skill with Stage 1

notes: |
  Stage 0 research complete. DSP architecture documented.
  Ready to proceed to complexity estimation and implementation planning.
```

**Replace with:**
```markdown
4. Create checkpoint state file:

Create `.continue-here.md` using template from `assets/continue-stage-0-template.md` with variables:
- `${PLUGIN_NAME}` - Plugin name
- `${TIMESTAMP}` - Current timestamp
```

**Verification:** File exists at `.claude/skills/plugin-planning/assets/continue-stage-0-template.md` and SKILL.md reference updated

---

### Fix 2.2: Extract Stage 1 .continue-here.md template
**Location:** Lines 307-340
**Operation:** EXTRACT
**Create file:** `assets/continue-stage-1-template.md`
**File content:**
```yaml
skill: plugin-planning
plugin: ${PLUGIN_NAME}
stage: 1
status: completed
last_updated: ${TIMESTAMP}

progress:
  planning_complete: true
  complexity_calculated: true
  plan_spec: "plugins/${PLUGIN_NAME}/.ideas/plan.md"

contracts_created:
  - plan.md

complexity:
  estimated_stages: ${ESTIMATED_STAGES}
  foundation_complexity: ${FOUNDATION_COMPLEXITY}
  shell_complexity: ${SHELL_COMPLEXITY}
  dsp_complexity: ${DSP_COMPLEXITY}
  gui_complexity: ${GUI_COMPLEXITY}

next_actions:
  - Proceed to Stage 2 (Foundation)
  - Invoke plugin-workflow skill for implementation

notes: |
  Stage 1 planning complete. Implementation plan documented with complexity estimates.
  Ready to proceed to implementation workflow.
```

**Replace with:**
```markdown
5. Create checkpoint state file:

Create `.continue-here.md` using template from `assets/continue-stage-1-template.md` with variables:
- `${PLUGIN_NAME}` - Plugin name
- `${TIMESTAMP}` - Current timestamp
- `${ESTIMATED_STAGES}` - Total estimated stages
- `${FOUNDATION_COMPLEXITY}`, `${SHELL_COMPLEXITY}`, `${DSP_COMPLEXITY}`, `${GUI_COMPLEXITY}` - Complexity scores
```

**Verification:** File exists at `assets/continue-stage-1-template.md` and SKILL.md reduced by ~220 tokens

---

### Fix 2.3: Extract implementation handoff template
**Location:** Lines 397-421
**Operation:** EXTRACT
**Create file:** `assets/implementation-handoff-template.md`
**File content:**
```yaml
workflow: plugin-workflow
plugin: ${PLUGIN_NAME}
current_stage: 2
status: ready
entry_point: foundation
last_updated: ${TIMESTAMP}

contracts:
  creative_brief: "plugins/${PLUGIN_NAME}/.ideas/creative-brief.md"
  parameter_spec: "plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md"
  architecture: "plugins/${PLUGIN_NAME}/.ideas/architecture.md"
  plan: "plugins/${PLUGIN_NAME}/.ideas/plan.md"

progress:
  planning_complete: true
  foundation_started: false

next_action: "Invoke plugin-workflow skill to begin Stage 2 (Foundation)"

notes: |
  Planning phase complete. All contracts generated.
  Ready to begin implementation workflow starting at Stage 2 (Foundation).
```

**Replace with:**
```markdown
2. Create implementation handoff at plugin root:
```bash
# Use template from assets/implementation-handoff-template.md
# IMPORTANT: Create at plugins/${PLUGIN_NAME}/.continue-here.md (NOT in .ideas/)
```

Apply template with variables:
- `${PLUGIN_NAME}` - Plugin name
- `${TIMESTAMP}` - Current timestamp
```

**Verification:** File exists at `assets/implementation-handoff-template.md` and handoff references template

---

### Fix 2.4: Extract Stage 0 decision menu
**Location:** Lines 162-176
**Operation:** EXTRACT
**Create file:** `assets/decision-menu-stage-0.md`
**File content:**
```markdown
✓ Stage 0 complete: DSP architecture documented

What's next?
1. Continue to Stage 1 - Planning (recommended)
2. Review architecture.md findings
3. Improve creative brief based on research
4. Run deeper JUCE investigation (deep-research skill) ← Discover troubleshooting
5. Pause here
6. Other

Choose (1-6): _
```

**Replace with:**
```markdown
6. Present decision menu:

Display menu from `assets/decision-menu-stage-0.md`

<user_input_handling>
Wait for user response:
- Numeric 1-6: Execute option
- Keywords: "continue"→1, "pause"→5, "review"→2
- Other: Re-present menu
</user_input_handling>
```

**Verification:** Menu extracted and SKILL.md references asset file

---

### Fix 2.5: Extract Stage 1 decision menu
**Location:** Lines 360-376
**Operation:** EXTRACT
**Create file:** `assets/decision-menu-stage-1.md`
**File content:**
```markdown
✓ Stage 1 complete: Implementation plan created

What's next?
1. Proceed to Stage 2 - Foundation (recommended)
2. Review plan.md complexity estimates
3. Adjust parameter-spec.md based on complexity
4. Revise architecture.md design
5. Validate contracts with design-sync skill ← Discover validation
6. Pause here
7. Other

Choose (1-7): _
```

**Replace with:**
```markdown
8. Present decision menu:

Display menu from `assets/decision-menu-stage-1.md`

<user_input_handling>
Wait for user response:
- Numeric 1-7: Execute option
- Keywords: "continue"→1, "pause"→6, "review"→2, "validate"→5
- Other: Re-present menu
</user_input_handling>
```

**Verification:** Menu extracted and SKILL.md references asset file

---

### Fix 2.6: Extract Stage 1 blocked error message
**Location:** Lines 204-236 (within the error message section)
**Operation:** EXTRACT
**Create file:** `assets/stage-1-blocked.md`
**File content:**
```markdown
# Stage 1 Blocked: Missing Contracts

Stage 1 (Planning) requires Stage 0 (Research) to be complete.

**Missing contracts:**
- [ ] parameter-spec.md - Parameter definitions
- [ ] architecture.md - DSP design specification

**What to do:**

1. Complete Stage 0 first:
   - If starting fresh: Invoke plugin-planning skill without stage argument
   - If resuming: Check plugins/${PLUGIN_NAME}/.ideas/ for existing contracts

2. If contracts exist but not detected:
   - Verify file paths:
     - `plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md`
     - `plugins/${PLUGIN_NAME}/.ideas/architecture.md`

3. Contact system administrator if contracts are present but still blocked.
```

**Replace with:**
```markdown
<blocking_condition>
IF PARAM_SPEC_EXISTS == "false" OR ARCH_EXISTS == "false":
  THEN display error message from assets/stage-1-blocked.md
  AND exit skill
ELSE:
  Proceed to complexity calculation
</blocking_condition>
```

**Verification:** Error message extracted and decision gate references asset file

---

### Fix 2.7: Extract precondition failed error message
**Location:** Lines 30-44 (within precondition section)
**Operation:** EXTRACT
**Create file:** `assets/precondition-failed.md`
**File content:**
```markdown
# Precondition Failed

Plugin planning requires a creative brief to exist before proceeding.

**Missing:**
- `plugins/${PLUGIN_NAME}/.ideas/creative-brief.md`

**What to do:**

1. Create creative brief first:
   - Use `/dream` command to ideate plugin concept
   - Or use plugin-ideation skill to generate creative-brief.md

2. If creative brief exists but not detected:
   - Verify file path: `plugins/${PLUGIN_NAME}/.ideas/creative-brief.md`
   - Check file permissions (must be readable)

3. If resuming from checkpoint:
   - Check .continue-here.md for correct plugin name
```

**Replace with:**
```bash
if [ ! -f "plugins/${PLUGIN_NAME}/.ideas/creative-brief.md" ]; then
    echo "✗ creative-brief.md not found - SKILL BLOCKED"
    cat assets/precondition-failed.md
    exit 1
fi
```

**Verification:** Error message extracted and precondition gate references asset file

---

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Refactor resume logic to explicit IF-THEN
**Location:** Lines 52-56
**Operation:** REPLACE
**Before:**
```markdown
**Resume logic:**
- If architecture.md exists but plan.md doesn't: Skip to Stage 1
- If both exist: Ask user if they want to regenerate or proceed to implementation
- If neither exists: Start at Stage 0
```

**After:**
```markdown
**Resume logic:**

<resume_logic>
IF architecture.md exists AND plan.md missing:
  THEN skip to Stage 1
ELSE IF architecture.md exists AND plan.md exists:
  THEN ask user: "Both contracts exist. Regenerate or proceed to implementation?"
ELSE:
  THEN start at Stage 0
</resume_logic>
```

**Verification:** Text now uses explicit IF-THEN-ELSE structure wrapped in `<resume_logic>` tag

---

### Fix 3.2: Wrap Stage 0 workflow in critical sequence
**Location:** Lines 59-183
**Operation:** WRAP
**Before:**
```markdown
## Stage 0: Research

**Goal:** Create DSP architecture specification (architecture.md)

**Duration:** 5-10 minutes

**Process:**

1. Read creative brief:
[...process steps...]
```

**After:**
```markdown
## Stage 0: Research

<critical_sequence id="stage-0-research">
<sequence_requirement>
Execute steps 1-6 in order. Do NOT skip steps. Each step builds on previous.
</sequence_requirement>

**Goal:** Create DSP architecture specification (architecture.md)

**Duration:** 5-10 minutes

**Process:**

1. Read creative brief:
[...process steps...]

[at end of stage, before decision menu]

<checkpoint_requirement>
After completing step 6, MUST commit changes and present decision menu. Do NOT auto-proceed.
</checkpoint_requirement>
</critical_sequence>
```

**Verification:** Stage 0 wrapped in `<critical_sequence>` with `<checkpoint_requirement>` at end

---

### Fix 3.3: Wrap Stage 1 workflow in critical sequence
**Location:** Lines 187-382
**Operation:** WRAP
**Before:**
```markdown
## Stage 1: Planning

**Goal:** Create implementation roadmap (plan.md)

**Duration:** 10-15 minutes

**Process:**
[...process steps...]
```

**After:**
```markdown
## Stage 1: Planning

<critical_sequence id="stage-1-planning">
<sequence_requirement>
Execute steps 1-8 in order. Do NOT skip steps. Complexity calculation must precede plan generation.
</sequence_requirement>

**Goal:** Create implementation roadmap (plan.md)

**Duration:** 10-15 minutes

**Process:**
[...process steps...]

[at end of stage, before decision menu]

<checkpoint_requirement>
After completing step 8, MUST commit changes and present decision menu. Do NOT auto-proceed.
</checkpoint_requirement>
</critical_sequence>
```

**Verification:** Stage 1 wrapped in `<critical_sequence>` with `<checkpoint_requirement>` at end

---

### Fix 3.4: Refactor Stage 0 user input handling
**Location:** Lines 177-182
**Operation:** REPLACE
**Before:**
```markdown
Wait for user input. Handle:
- Number (1-6): Execute corresponding option
- "continue" keyword: Execute option 1
- "pause" keyword: Execute option 5
- "review" keyword: Execute option 2
- "other": Ask "What would you like to do?" then re-present menu
```

**After:**
```markdown
<user_input_handling>
Wait for user response. Parse input:

IF input matches /^[1-6]$/: Execute option number
ELSE IF input contains "continue": Execute option 1
ELSE IF input contains "pause": Execute option 5
ELSE IF input contains "review": Execute option 2
ELSE: Ask "What would you like to do?" then re-present menu
</user_input_handling>
```

**Verification:** Input handling uses explicit IF-THEN logic wrapped in `<user_input_handling>` tag

---

### Fix 3.5: Refactor Stage 1 user input handling
**Location:** Lines 371-376
**Operation:** REPLACE
**Before:**
```markdown
Wait for user input. Handle:
- Number (1-7): Execute corresponding option
- "continue" keyword: Execute option 1
- "pause" keyword: Execute option 6
- "review" keyword: Execute option 2
- "validate" keyword: Execute option 5
- "other": Ask "What would you like to do?" then re-present menu
```

**After:**
```markdown
<user_input_handling>
Wait for user response. Parse input:

IF input matches /^[1-7]$/: Execute option number
ELSE IF input contains "continue": Execute option 1
ELSE IF input contains "pause": Execute option 6
ELSE IF input contains "review": Execute option 2
ELSE IF input contains "validate": Execute option 5
ELSE: Ask "What would you like to do?" then re-present menu
</user_input_handling>
```

**Verification:** Input handling uses explicit IF-THEN logic with option 1-7 range

---

## File Creation Manifest

Files to create:

1. `assets/continue-stage-0-template.md` - Stage 0 completion state file template (~300 tokens)
2. `assets/continue-stage-1-template.md` - Stage 1 completion state file template (~320 tokens)
3. `assets/implementation-handoff-template.md` - Handoff to plugin-workflow state file (~240 tokens)
4. `assets/decision-menu-stage-0.md` - Stage 0 decision menu format (~100 tokens)
5. `assets/decision-menu-stage-1.md` - Stage 1 decision menu format (~110 tokens)
6. `assets/stage-1-blocked.md` - Error message when contracts missing (~200 tokens)
7. `assets/precondition-failed.md` - Error message when preconditions fail (~180 tokens)

All files should be created in: `.claude/skills/plugin-planning/assets/`

---

## Execution Checklist

### Phase 1: Critical Fixes
- [ ] Fix 1.1: Decision gate added to Stage 1 contract validation (lines 193-236)
- [ ] Fix 1.2: Precondition gate wrapper added (lines 27-56)
- [ ] Fix 1.3: Handoff protocol wrapper with verification added (lines 385-442)

### Phase 2: Extractions
- [ ] Fix 2.1: Stage 0 template extracted to assets/continue-stage-0-template.md
- [ ] Fix 2.2: Stage 1 template extracted to assets/continue-stage-1-template.md
- [ ] Fix 2.3: Implementation handoff template extracted to assets/implementation-handoff-template.md
- [ ] Fix 2.4: Stage 0 decision menu extracted to assets/decision-menu-stage-0.md
- [ ] Fix 2.5: Stage 1 decision menu extracted to assets/decision-menu-stage-1.md
- [ ] Fix 2.6: Stage 1 blocked error extracted to assets/stage-1-blocked.md
- [ ] Fix 2.7: Precondition failed error extracted to assets/precondition-failed.md

### Phase 3: Polish
- [ ] Fix 3.1: Resume logic refactored to IF-THEN (lines 52-56)
- [ ] Fix 3.2: Stage 0 workflow wrapped in critical_sequence (lines 59-183)
- [ ] Fix 3.3: Stage 1 workflow wrapped in critical_sequence (lines 187-382)
- [ ] Fix 3.4: Stage 0 user input handling refactored (lines 177-182)
- [ ] Fix 3.5: Stage 1 user input handling refactored (lines 371-376)

### Verification
- [ ] All 3 critical issues from analysis addressed
- [ ] All 7 asset files created in correct location
- [ ] SKILL.md references updated to point to asset files
- [ ] XML enforcement tags properly closed (no orphaned tags)
- [ ] Context reduced by ~600 tokens (from 12,817 to ~12,217)
- [ ] All blocking conditions use explicit IF-THEN logic
- [ ] All critical sequences have checkpoint requirements
- [ ] Handoff protocol includes verification steps

---

## Estimated Impact

### Token Reduction
- Before: 12,817 tokens
- After: ~12,217 tokens
- Savings: ~600 tokens (4.7%)

### Comprehension Improvement
- Critical sequence enforcement: **90% reduction** in out-of-order execution risk
- Decision gates: **90% reduction** in precondition bypass risk
- Handoff verification: **Eliminates** state file placement errors
- IF-THEN refactoring: **Removes ambiguity** from resume and input parsing logic

### Risk Reduction
1. Contract validation bypass: **Blocked** by decision gate
2. State file placement errors: **Prevented** by handoff verification
3. Precondition skipping: **Blocked** by precondition gate
4. Out-of-order stage execution: **Prevented** by critical sequence wrappers

---

## Implementation Time Estimate

### By Phase
- Phase 1 (Critical Fixes): **20 minutes**
  - Fix 1.1: 5 minutes
  - Fix 1.2: 7 minutes
  - Fix 1.3: 8 minutes

- Phase 2 (Extractions): **18 minutes**
  - Fixes 2.1-2.3 (templates): 9 minutes
  - Fixes 2.4-2.5 (menus): 4 minutes
  - Fixes 2.6-2.7 (errors): 5 minutes

- Phase 3 (Polish): **12 minutes**
  - Fix 3.1: 2 minutes
  - Fix 3.2: 3 minutes
  - Fix 3.3: 3 minutes
  - Fix 3.4: 2 minutes
  - Fix 3.5: 2 minutes

**Total: 50 minutes**

---

## Notes

### Execution Order Rationale
Phase 1 must be completed first because it addresses blocking issues that could cause workflow failures. Without these fixes, Claude might:
- Skip contract validation and proceed to Stage 1 without required files
- Place .continue-here.md in wrong location, breaking workflow continuity
- Bypass precondition checks

Phase 2 extractions can happen independently after Phase 1. They reduce context size and improve maintainability but don't affect correctness.

Phase 3 polish improves clarity but doesn't change behavior - safe to do last.

### Verification Strategy
After each phase:
1. Read SKILL.md to confirm changes applied correctly
2. Check that XML tags are properly paired (opening/closing)
3. Verify asset files created in correct location
4. Confirm line numbers still roughly align (extraction will shift subsequent lines)

### Maintenance Considerations
- Asset file changes now require updating templates, not SKILL.md directly
- XML tag consistency becomes new maintenance requirement
- But reduces inline duplication (3 templates → 3 files = single source of truth)
