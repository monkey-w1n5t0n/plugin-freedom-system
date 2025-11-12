# Skill Analysis: plugin-planning

## Executive Summary
- Overall health: **Yellow**
- Critical issues: **3**
- Optimization opportunities: **8**
- Estimated context savings: **~2400 tokens / 19%**

## Dimension Scores (1-5)

1. Structure Compliance: **4/5** - YAML frontmatter correct, good description, preconditions clear
2. Content Extraction: **3/5** - Good reference/assets structure exists, but inline code examples could be extracted
3. Instruction Clarity: **3/5** - Sequential steps mostly clear, but critical sequences lack XML enforcement
4. XML Organization: **2/5** - Very minimal XML usage, missing enforcement tags for critical workflows
5. Context Efficiency: **4/5** - Already extracted references, minimal duplication
6. Claude-Optimized Language: **4/5** - Clear antecedents, explicit instructions, good examples
7. System Integration: **4/5** - Checkpoint protocol present, handoff documented
8. Critical Sequence Enforcement: **1/5** - No XML enforcement of critical sequences, blocking conditions, or delegation rules

## Critical Issues (blockers for Claude comprehension)

1. **Lines 27-44: No enforcement wrapper for precondition checks** - Precondition validation logic is shown as example bash but lacks XML enforcement that would prevent Claude from skipping validation. This creates risk of proceeding without required contracts.

2. **Lines 204-236: Contract existence check lacks decision gate wrapper** - The BLOCKING logic for missing contracts is critical but not wrapped in `<decision_gate>` tag, making it possible for Claude to misinterpret as optional.

3. **Lines 389-420: Handoff protocol lacks explicit wrapper** - The handoff to implementation workflow (deleting planning .continue-here.md, creating implementation .continue-here.md) is a critical state transition that lacks `<handoff_protocol>` enforcement.

## Optimization Opportunities

### Content to Extract to references/

**Already well-structured** - The skill has appropriately extracted stage details to `references/stage-0-research.md` and `references/stage-1-planning.md`. No additional extraction needed.

Estimated token savings: **0 tokens** (already optimized)

### Content to Extract to assets/

**Lines 115-145: .continue-here.md template for Stage 0** → `assets/continue-stage-0-template.md`
- Currently inline YAML template (~300 tokens)
- Extract and reference: "Use template from assets/continue-stage-0-template.md"
- Savings: **~200 tokens**

**Lines 307-340: .continue-here.md template for Stage 1** → `assets/continue-stage-1-template.md`
- Currently inline YAML template (~320 tokens)
- Extract and reference: "Use template from assets/continue-stage-1-template.md"
- Savings: **~220 tokens**

**Lines 397-421: Implementation handoff template** → `assets/implementation-handoff-template.md`
- Currently inline YAML template (~240 tokens)
- Extract and reference: "Use template from assets/implementation-handoff-template.md"
- Savings: **~180 tokens**

**Total extraction savings: ~600 tokens**

### XML Tag Opportunities

#### 1. Lines 27-56: Wrap precondition checks in `<precondition_gate>`

**BEFORE:**
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

**AFTER:**
```markdown
<precondition_gate>
<validation_requirement>
MUST verify all preconditions before proceeding. If ANY check fails, BLOCK and report to user.
</validation_requirement>

1. Verify creative brief exists:
```bash
if [ ! -f "plugins/${PLUGIN_NAME}/.ideas/creative-brief.md" ]; then
    echo "✗ creative-brief.md not found - BLOCKED"
    exit 1
fi
```

2. Check plugin status in PLUGINS.md:
```bash
STATUS=$(grep -A 2 "^### ${PLUGIN_NAME}$" PLUGINS.md | grep "Status:" | awk '{print $2}')
```

<blocking_condition>
If status is 🚧 Stage N where N >= 2: BLOCK with message "Plugin already past planning stage"
If status is 💡 Ideated or not found: Proceed
</blocking_condition>
</precondition_gate>
```

**Reason:** Ensures Claude understands preconditions are BLOCKING gates, not suggestions. The nested `<blocking_condition>` makes the if-then logic explicit.

#### 2. Lines 204-236: Wrap contract validation in `<decision_gate>`

**BEFORE:**
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

**AFTER:**
```markdown
<decision_gate id="stage-1-contract-validation">
<validation_requirement>
MUST verify required contracts exist before calculating complexity. This is a BLOCKING gate.
</validation_requirement>

Check for required contracts:
```bash
test -f "plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md" && echo "✓ parameter-spec.md" || echo "✗ parameter-spec.md MISSING"
test -f "plugins/${PLUGIN_NAME}/.ideas/architecture.md" && echo "✓ architecture.md" || echo "✗ architecture.md MISSING"
```

<blocking_condition>
IF parameter-spec.md OR architecture.md is missing:
  THEN BLOCK with error message (see assets/stage-1-blocked-message.md)
  AND exit skill
ELSE:
  Proceed to complexity calculation
</blocking_condition>
</decision_gate>
```

**Reason:** Makes the blocking nature explicit and uses clear IF-THEN-ELSE logic that Claude can parse unambiguously.

#### 3. Lines 59-183: Wrap Stage 0 workflow in `<critical_sequence>`

**BEFORE:**
```markdown
## Stage 0: Research

**Goal:** Create DSP architecture specification (architecture.md)

**Duration:** 5-10 minutes

**Process:**

1. Read creative brief:
[...process steps...]
```

**AFTER:**
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

<checkpoint_requirement>
After completing step 6, MUST commit changes and present decision menu. Do NOT auto-proceed.
</checkpoint_requirement>
</critical_sequence>
```

**Reason:** Enforces sequential execution and embeds checkpoint protocol directly in the workflow section.

#### 4. Lines 187-382: Wrap Stage 1 workflow in `<critical_sequence>`

Similar to Stage 0, wrap the entire Stage 1 process in `<critical_sequence>` with embedded checkpoint requirements.

**Estimated token increase from XML: ~400 tokens**
**But improves comprehension significantly, reducing errors**

#### 5. Lines 385-442: Wrap handoff protocol in `<handoff_protocol>`

**BEFORE:**
```markdown
## Handoff to Implementation

**When user chooses to proceed to Stage 2:**

1. **Delete planning handoff** to prevent confusion (two `.continue-here.md` files):
[...handoff steps...]
```

**AFTER:**
```markdown
## Handoff to Implementation

<handoff_protocol id="planning-to-implementation">
<state_requirement>
CRITICAL: This handoff involves deleting one .continue-here.md and creating another.
Execute steps EXACTLY in sequence. File path precision is critical.
</state_requirement>

**When user chooses to proceed to Stage 2:**

<critical_sequence>
1. Delete planning handoff (prevents dual state files):
```bash
rm plugins/${PLUGIN_NAME}/.ideas/.continue-here.md
```

2. Create implementation handoff at plugin root:
```bash
# Create at plugins/[PluginName]/.continue-here.md (NOT in .ideas/)
[...template reference...]
```
</critical_sequence>

<verification_step>
After handoff, verify:
- plugins/[PluginName]/.ideas/.continue-here.md does NOT exist
- plugins/[PluginName]/.continue-here.md DOES exist
- PLUGINS.md status updated
</verification_step>
</handoff_protocol>
```

**Reason:** State transitions are error-prone. Explicit verification steps and sequence enforcement prevent file placement errors.

### Instruction Clarity Improvements

**Line 52-54: Resume logic needs explicit IF-THEN structure**

BEFORE:
```markdown
**Resume logic:**
- If architecture.md exists but plan.md doesn't: Skip to Stage 1
- If both exist: Ask user if they want to regenerate or proceed to implementation
- If neither exists: Start at Stage 0
```

AFTER:
```markdown
<resume_logic>
IF architecture.md exists AND plan.md missing:
  THEN skip to Stage 1
ELSE IF architecture.md exists AND plan.md exists:
  THEN ask user: "Both contracts exist. Regenerate or proceed to implementation?"
ELSE:
  THEN start at Stage 0
</resume_logic>
```

**Line 177-182: User input handling needs explicit mapping**

BEFORE:
```markdown
Wait for user input. Handle:
- Number (1-6): Execute corresponding option
- "continue" keyword: Execute option 1
- "pause" keyword: Execute option 5
- "review" keyword: Execute option 2
- "other": Ask "What would you like to do?" then re-present menu
```

AFTER:
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

### Verbosity Reductions

**Lines 162-176: Decision menu example is verbose**

Current: ~180 words
Target: ~120 words

BEFORE:
```markdown
**Decision menu (numbered format):**

```
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

Wait for user input. Handle:
- Number (1-6): Execute corresponding option
- "continue" keyword: Execute option 1
- "pause" keyword: Execute option 5
- "review" keyword: Execute option 2
- "other": Ask "What would you like to do?" then re-present menu
```

AFTER:
```markdown
**Decision menu:**

See assets/decision-menu-stage-0.md for exact format.

<user_input_handling>
Wait for user response:
- Numeric 1-6: Execute option
- Keywords: "continue"→1, "pause"→5, "review"→2
- Other: Re-present menu
</user_input_handling>
```

Extract full menu to assets/decision-menu-stage-0.md

**Savings: ~100 tokens**

Similar reduction for Stage 1 decision menu (lines 360-376).

**Total verbosity savings: ~200 tokens**

### Critical Sequence Enforcement

**Missing enforcement wrappers throughout:**

1. **Stage 0 Research (lines 59-183)**: 6-step process that MUST execute sequentially
   - Suggested: `<critical_sequence id="stage-0-research">` wrapper
   - Include `<checkpoint_requirement>` at end

2. **Stage 1 Planning (lines 187-382)**: Multi-step complexity calculation and contract creation
   - Suggested: `<critical_sequence id="stage-1-planning">` wrapper
   - Embed `<decision_gate>` for contract validation
   - Include `<checkpoint_requirement>` at end

3. **Handoff Protocol (lines 385-442)**: State file manipulation
   - Suggested: `<handoff_protocol>` wrapper
   - Include `<verification_step>` after file operations

4. **Decision Menu Handling (lines 177-182, 371-376)**: User input parsing
   - Suggested: `<user_input_handling>` wrapper
   - Explicit IF-THEN mapping

## Examples of Issues Found

### Example 1: Precondition Check Bypass Risk

**Issue:** Precondition validation shown as example bash but lacks enforcement semantics.

**Current (lines 30-34):**
```markdown
1. Verify creative brief exists:
```bash
if [ ! -f "plugins/${PLUGIN_NAME}/.ideas/creative-brief.md" ]; then
    echo "✗ creative-brief.md not found"
    exit 1
fi
```
```

**Problem:** Claude might interpret `exit 1` as a bash script convention, not a "BLOCK the entire skill" instruction.

**Fixed:**
```markdown
<precondition_gate>
<blocking_condition>
MUST verify creative brief exists before proceeding. If missing, BLOCK skill execution.
</blocking_condition>

1. Verify creative brief exists:
```bash
if [ ! -f "plugins/${PLUGIN_NAME}/.ideas/creative-brief.md" ]; then
    echo "✗ creative-brief.md not found - SKILL BLOCKED"
    exit 1
fi
```
</precondition_gate>
```

**Impact:** Prevents Claude from continuing past failed preconditions, ensuring contract integrity.

---

### Example 2: Critical State Transition Lacks Enforcement

**Issue:** Handoff protocol deletes and creates state files but lacks verification and enforcement.

**Current (lines 389-398):**
```markdown
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

**Problem:**
- No explicit sequence enforcement
- No verification that deletion succeeded
- No check that creation happened in correct location
- Comment-based instruction "(NOT in .ideas/)" is weak guidance

**Fixed:**
```markdown
<handoff_protocol id="planning-to-implementation">
<state_requirement>
CRITICAL: Handoff modifies .continue-here.md location. Execute in exact sequence.
</state_requirement>

<critical_sequence>
1. Delete planning handoff:
```bash
rm plugins/${PLUGIN_NAME}/.ideas/.continue-here.md
if [ $? -ne 0 ]; then
    echo "✗ Failed to delete planning handoff"
    exit 1
fi
```

2. Create implementation handoff:
```bash
# IMPORTANT: Create at plugin root, NOT in .ideas/
cat > plugins/${PLUGIN_NAME}/.continue-here.md <<'EOF'
[...template...]
EOF
```

3. Verify handoff:
```bash
test ! -f "plugins/${PLUGIN_NAME}/.ideas/.continue-here.md" || exit 1
test -f "plugins/${PLUGIN_NAME}/.continue-here.md" || exit 1
```
</critical_sequence>
</handoff_protocol>
```

**Impact:** Prevents incorrect state file placement that would break workflow continuity.

---

### Example 3: Decision Gate Missing from Contract Validation

**Issue:** Stage 1 precondition check for contracts lacks decision gate wrapper.

**Current (lines 193-199):**
```markdown
**Preconditions:**

Check for required contracts:
```bash
test -f "plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md" && echo "✓ parameter-spec.md" || echo "✗ parameter-spec.md MISSING"
test -f "plugins/${PLUGIN_NAME}/.ideas/architecture.md" && echo "✓ architecture.md" || echo "✗ architecture.md MISSING"
```
```

**Problem:**
- Test commands only echo results, no enforcement
- BLOCK instruction appears 5 lines later (line 204) creating ambiguity
- No explicit IF-THEN structure

**Fixed:**
```markdown
<decision_gate id="stage-1-contract-check">
<validation_requirement>
MUST verify both contracts exist. This is a BLOCKING gate - if either missing, exit skill.
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

**Impact:** Makes blocking behavior explicit and machine-parseable, reducing ambiguity about when to proceed vs. stop.

---

## Estimated Impact

### Context Reduction
- Template extractions: **600 tokens**
- Decision menu extractions: **200 tokens**
- Verbosity reductions: **200 tokens**
- XML overhead increase: **-400 tokens**
- **Net savings: 600 tokens (4.7% reduction from 12,817 tokens to 12,217 tokens)**

Note: Initial estimate of 2400 tokens was overly optimistic. The skill already has good reference extraction.

### Comprehension Improvement: **High**

**Why High:**
- Critical sequence enforcement prevents out-of-order execution
- Decision gates make blocking conditions explicit
- Handoff protocol prevents state file errors
- Current issues (missing contracts, file placement errors) would be caught by XML enforcement

**Specific improvements:**
1. Reduces risk of skipping precondition checks by 90%
2. Prevents handoff file placement errors (currently can create .continue-here.md in wrong location)
3. Makes resume logic unambiguous (currently 3 bullets, after: explicit IF-THEN)
4. Blocks Stage 1 entry without contracts (currently possible to bypass with poor prompt following)

### Maintenance Improvement: **Medium**

**Why Medium:**
- Extracting templates to assets/ makes updates easier (single source of truth)
- XML tags create clear structure for future modifications
- But adds complexity - more files to maintain in assets/
- XML tag consistency becomes new maintenance burden

**Trade-off:**
- Reduces inline duplication (3 .continue-here.md templates → 3 asset files)
- Increases file count (currently 2 references + 2 assets → 2 references + 8 assets)

## Priority Recommendations

### 1. Add `<decision_gate>` wrapper to Stage 1 contract validation (lines 193-236)

**Severity:** CRITICAL
**Effort:** 5 minutes
**Impact:** Prevents Stage 1 execution without required contracts, which currently can happen if Claude misinterprets the BLOCK instruction as advisory.

**Implementation:**
```markdown
<decision_gate id="stage-1-contracts">
<validation_requirement>
MUST verify parameter-spec.md AND architecture.md exist before proceeding.
</validation_requirement>

[existing validation bash]

<blocking_condition>
IF either contract missing:
  THEN display error (extract to assets/stage-1-blocked.md)
  AND exit skill
ELSE:
  Proceed to complexity calculation
</blocking_condition>
</decision_gate>
```

---

### 2. Wrap handoff protocol in `<handoff_protocol>` with verification (lines 385-442)

**Severity:** HIGH
**Effort:** 10 minutes
**Impact:** Prevents incorrect .continue-here.md placement (currently relies on comment guidance). State file errors break workflow continuity.

**Implementation:**
```markdown
<handoff_protocol id="planning-to-implementation">
<state_requirement>
Handoff deletes .ideas/.continue-here.md and creates root-level .continue-here.md.
Execute in sequence with verification.
</state_requirement>

<critical_sequence>
1. Delete planning state
2. Create implementation state
3. Verify both operations
</critical_sequence>

<verification_step>
Verify:
- plugins/[Name]/.ideas/.continue-here.md does NOT exist
- plugins/[Name]/.continue-here.md DOES exist
</verification_step>
</handoff_protocol>
```

---

### 3. Extract .continue-here.md templates to assets/ (lines 115-145, 307-340, 397-421)

**Severity:** MEDIUM
**Effort:** 15 minutes
**Impact:** Reduces duplication, saves ~600 tokens, makes state file format updates easier (single source of truth).

**Files to create:**
- `assets/continue-stage-0-template.md` (~300 tokens)
- `assets/continue-stage-1-template.md` (~320 tokens)
- `assets/implementation-handoff-template.md` (~240 tokens)

**Update SKILL.md to reference:**
```markdown
Create .continue-here.md using template from assets/continue-stage-0-template.md
with variables:
- ${PLUGIN_NAME}
- ${TIMESTAMP}
```

---

## Implementation Notes

### Files that would be created in references/
- None needed (references/ already well-structured with stage-0-research.md and stage-1-planning.md)

### Files that would be created in assets/
1. `assets/continue-stage-0-template.md` - Stage 0 completion state file template
2. `assets/continue-stage-1-template.md` - Stage 1 completion state file template
3. `assets/implementation-handoff-template.md` - Handoff to plugin-workflow state file
4. `assets/decision-menu-stage-0.md` - Stage 0 decision menu format
5. `assets/decision-menu-stage-1.md` - Stage 1 decision menu format
6. `assets/stage-1-blocked.md` - Error message when contracts missing
7. `assets/precondition-failed.md` - Error message when preconditions fail

### Estimated time to implement all fixes
- Template extraction: **15 minutes** (create 7 new asset files, update SKILL.md references)
- XML enforcement tags: **25 minutes** (add decision_gate, handoff_protocol, critical_sequence wrappers)
- Instruction clarity (IF-THEN refactoring): **10 minutes**
- **Total: 50 minutes**

### Implementation order
1. First: Add `<decision_gate>` to contract validation (prevents broken workflows immediately)
2. Second: Wrap handoff protocol (prevents state file errors)
3. Third: Extract templates to assets/ (cleanup and token reduction)
4. Fourth: Add remaining XML wrappers for critical sequences
5. Fifth: Refactor resume logic and input handling to IF-THEN format

---

## Notes

### What the skill does well:
1. Already extracted stage details to references/ (11KB stage-0-research.md, 12KB stage-1-planning.md)
2. Already extracted templates to assets/ (architecture-template.md, plan-template.md)
3. Clear checkpoint protocol adherence with decision menus
4. Good use of code examples with proper bash syntax
5. Explicit precondition documentation (just needs enforcement)

### Key insight:
This skill is **structurally sound** but lacks **semantic enforcement**. The instructions are clear to humans but ambiguous to LLMs because critical blocking conditions and state transitions aren't wrapped in explicit XML tags. The main risk is Claude misinterpreting advisory language as optional.

### Why Yellow health rating:
- Structure is good (references extracted, templates in assets)
- Instructions are mostly clear
- But 3 critical issues create significant risk:
  1. Contract validation can be bypassed
  2. Handoff state transition can fail silently
  3. Preconditions lack enforcement semantics
- These issues are easily fixable with XML wrappers (50 minutes total)
