# Skill Analysis: design-sync

## Executive Summary
- Overall health: **Yellow** (Functional but has critical comprehension risks)
- Critical issues: **3** (all related to missing XML enforcement)
- Optimization opportunities: **6**
- Estimated context savings: **1150 tokens / 15%**

## Dimension Scores (1-5)
1. Structure Compliance: 5/5
2. Content Extraction: 2/5
3. Instruction Clarity: 4/5
4. XML Organization: 1/5
5. Context Efficiency: 3/5
6. Claude-Optimized Language: 4/5
7. System Integration: 4/5
8. Critical Sequence Enforcement: 1/5

**Average: 3.0/5**

## Critical Issues (blockers for Claude comprehension)

### 1. Missing `<critical_sequence>` wrapper (Lines 50-389)
**Impact:** HIGH - Claude may skip validation steps or execute out of order

The 7-step workflow (Load Contracts → Quantitative Checks → Semantic Validation → Categorize Drift → Present Findings → Execute User Choice → Route Back) is a mandatory sequence but lacks XML enforcement. Without this wrapper, Claude may:
- Skip quantitative checks and jump to semantic validation
- Present findings before categorizing drift
- Execute user choice before waiting for response
- Not route back to ui-mockup properly

### 2. Missing `<decision_gate>` wrappers (Lines 204-294, and throughout)
**Impact:** HIGH - Claude may auto-proceed past decision menus without waiting for user

Multiple decision menus throughout the skill (No Drift, Acceptable Evolution, Drift Requiring Attention, Critical Drift) lack explicit gate enforcement. While checkpoint protocol implies waiting, without XML tags Claude may:
- Auto-select options (especially "recommended" options)
- Proceed without user confirmation
- Skip decision menus entirely

### 3. Missing `<handoff_protocol>` wrapper (Lines 355-389)
**Impact:** MEDIUM - Claude may not correctly route back to ui-mockup

The routing back to ui-mockup Phase 5.5 is critical for workflow continuity but lacks protocol enforcement. Without this, Claude may:
- Present wrong decision menu
- Not invoke ui-mockup correctly
- Skip routing entirely and end execution

## Optimization Opportunities

### Content to Extract to references/

#### Extract: Example Scenarios (Lines 593-773, 180 lines)
**Estimated savings:** ~900 tokens

Move four detailed example scenarios to `references/examples.md`:
- Scenario 1: No Drift (Quick Validation) - lines 593-629
- Scenario 2: Acceptable Evolution - lines 631-674
- Scenario 3: Drift Requiring Attention - lines 676-724
- Scenario 4: Critical Drift (Blocked) - lines 725-773

**Rationale:** These are supporting examples, not core instructions. They're helpful for understanding but don't need to be loaded every time the skill runs. Reference them with: "See references/examples.md for detailed scenarios."

#### Extract: Extraction Logic Pseudocode (Lines 82-98, 16 lines)
**Estimated savings:** ~100 tokens

Move the TypeScript-like pseudocode showing how to extract data to `references/extraction-logic.md`. Replace with: "Extract parameter counts and features using patterns in references/extraction-logic.md"

### Content to Extract to assets/

#### Extract: Evolution Template (Lines 302-330, 29 lines)
**Estimated savings:** ~150 tokens

Move the markdown template for documenting design evolution to `assets/evolution-template.md`. This template is used when updating the brief, so it belongs in assets for easy insertion. Replace inline template with: "Use template from assets/evolution-template.md"

### XML Tag Opportunities

#### Wrap Workflow in `<critical_sequence>` (Lines 50-389)

**BEFORE:**
```markdown
## Workflow

### Step 1: Load Contracts

Read three files:
[...]

### Step 7: Route Back to ui-mockup
[...]
```

**AFTER:**
```markdown
## Workflow

<critical_sequence>
<sequence_name>design-sync-validation</sequence_name>
<enforcement>MUST execute all steps in order, MUST NOT skip steps, MUST NOT auto-proceed past decision gates</enforcement>

### Step 1: Load Contracts
<step_requirement>MUST complete before Step 2. If files missing, BLOCK with error menu.</step_requirement>

Read three files:
[...]

### Step 2: Quantitative Checks
<step_requirement>MUST complete before Step 3. Extract counts first, then compare.</step_requirement>
[...]

### Step 3: Semantic Validation (Extended Thinking)
<step_requirement>MUST use extended thinking. MUST NOT skip semantic analysis.</step_requirement>
[...]

### Step 4: Categorize Drift
<step_requirement>MUST categorize based on Step 2+3 findings before presenting.</step_requirement>
[...]

### Step 5: Present Findings
<step_requirement>MUST present appropriate decision menu based on drift category.</step_requirement>

<decision_gate>
<gate_name>drift-resolution</gate_name>
<blocking>true</blocking>
<wait_for_user>REQUIRED - MUST NOT auto-proceed, MUST NOT auto-select options</wait_for_user>
[Decision menus here]
</decision_gate>
[...]

### Step 6: Execute User Choice
<step_requirement>MUST execute choice from Step 5 gate before proceeding to Step 7.</step_requirement>
[...]

### Step 7: Route Back to ui-mockup
<step_requirement>MUST present ui-mockup Phase 5.5 decision menu after completing user's choice.</step_requirement>

<handoff_protocol>
<target_skill>ui-mockup</target_skill>
<target_phase>5.5</target_phase>
<handoff_type>return_to_workflow</handoff_type>
<required_menu>Phase 5.5 decision menu (without "Check alignment" option)</required_menu>
</handoff_protocol>

[...]
</critical_sequence>
```

#### Wrap Each Decision Menu in `<decision_gate>` (Lines 204-294, and throughout)

**BEFORE:**
```markdown
#### If No Drift:

```
✓ Design-brief alignment verified
[...]
What's next?
1. Continue implementation (recommended)
[...]
```
```

**AFTER:**
```markdown
#### If No Drift:

<decision_gate>
<gate_name>no-drift-confirmation</gate_name>
<blocking>true</blocking>
<wait_for_user>REQUIRED - Present menu and WAIT. MUST NOT auto-select option 1.</wait_for_user>

```
✓ Design-brief alignment verified
[...]
What's next?
1. Continue implementation (recommended)
[...]
```

</decision_gate>
```

**Apply to all decision menus:**
- No Drift (lines 204-216)
- Acceptable Evolution (lines 218-240)
- Drift Requiring Attention (lines 243-267)
- Critical Drift (lines 270-294)
- Error handling menus (lines 56-78, 471-524)
- Route back menu (lines 362-373)

#### Add `<anti-patterns>` wrapper (Lines 560-567)

**BEFORE:**
```markdown
**Common pitfalls:**

- Forgetting to use extended thinking for semantic validation (critical for accuracy)
[...]
```

**AFTER:**
```markdown
<anti-patterns>
**Common pitfalls (AVOID THESE):**

- Forgetting to use extended thinking for semantic validation (critical for accuracy)
- Auto-categorizing as "acceptable" without checking (be objective)
- Not presenting "Continue anyway" for non-critical drift (user should have option)
- Providing "override" for critical drift (should be blocked)
- Updating mockup instead of brief (mockup is source of truth, brief gets updated)
- Not logging overrides (audit trail required)
</anti-patterns>
```

#### Add `<requirements>` wrapper (Lines 456-466)

**BEFORE:**
```markdown
## Success Criteria

Validation is successful when:

- ✅ Both contracts loaded (brief + parameter-spec + mockup YAML)
[...]
```

**AFTER:**
```markdown
<requirements>
<section>Success Criteria</section>

Validation is successful when ALL of these are met:

- ✅ Both contracts loaded (brief + parameter-spec + mockup YAML)
- ✅ Quantitative checks completed (parameter count, feature detection)
- ✅ Semantic validation performed (extended thinking analysis)
- ✅ Drift category assigned (none / acceptable / attention / critical)
- ✅ Appropriate decision menu presented
- ✅ User action executed (update brief / update mockup / override)

MUST NOT mark validation complete until all requirements met.
</requirements>
```

### Instruction Clarity Improvements

#### Line 82: Make extraction instruction explicit
**BEFORE:**
```markdown
**Extract data from contracts:**

```typescript
// From creative-brief.md
briefParamCount = extractParameterCount(brief);
```

**AFTER:**
```markdown
**Extract parameter counts and feature lists from contracts using these patterns:**

See references/extraction-logic.md for implementation details:
- Count parameters mentioned in brief
- Count parameters in parameter-spec.md
- Detect feature keywords (preset, bypass, meter, etc.)
```

#### Line 141: Make extended thinking prompt instruction explicit
**BEFORE:**
```markdown
**Extended thinking prompt:**

```
Compare creative brief with mockup to assess alignment:
```

**AFTER:**
```markdown
**Use this exact extended thinking prompt:**

```
Compare creative brief with mockup to assess alignment:
[prompt text]
```

**MUST use extended thinking for semantic validation. This is not optional.**
```

#### Lines 547-558: Add explicit WAIT instruction after decision menus
**BEFORE:**
```markdown
**When executing this skill:**

1. Always load all three contracts first (brief, parameter-spec, mockup YAML)
[...]
8. Never auto-override (user must explicitly choose)
```

**AFTER:**
```markdown
**When executing design-sync, MUST follow these rules:**

1. ALWAYS load all three contracts first (brief, parameter-spec, mockup YAML)
2. ALWAYS run quantitative checks before semantic validation (faster)
3. MUST use extended thinking for semantic validation (8k budget) - not optional
4. MUST categorize drift objectively (use thresholds from references/drift-detection.md)
5. MUST present appropriate decision menu (based on drift category)
6. After presenting decision menu, MUST WAIT for user response - NEVER auto-proceed
7. If user approves evolution, update brief's Evolution section
8. ALWAYS log overrides to .validator-overrides.yaml
9. NEVER auto-override (user must explicitly choose)
10. After user choice executed, MUST route back to ui-mockup Phase 5.5
```

### Verbosity Reductions

#### Lines 22-30: Condense "Why this matters" section
**Current:** 9 lines, ~100 words
**Target:** 3 lines, ~40 words

**BEFORE:**
```markdown
**Why this matters:**

Creative briefs capture intent. Mockups capture reality. During design iteration, these can diverge:

- Brief says 8 parameters → Mockup has 12
- Brief says "vintage warmth" → Mockup is "modern minimal"
- Brief mentions preset browser → Mockup lacks it

Detecting drift early allows course correction before implementation locks in the design.
```

**AFTER:**
```markdown
**Why this matters:** Creative briefs capture intent; mockups capture reality. During design iteration these can diverge (parameter mismatches, visual style drift, missing features). Detecting drift early enables course correction before implementation.
```

**Savings:** ~50 tokens

#### Lines 56-78: Remove duplicate error handling
Lines 56-78 show error handling that duplicates Step 1's error handling. Remove this section and keep only the Step 1 version.

**Savings:** ~150 tokens

### Critical Sequence Enforcement

The workflow (Steps 1-7) is the core critical sequence that MUST be enforced. Here's the enforcement structure:

```xml
<critical_sequence>
  <sequence_name>design-sync-validation</sequence_name>
  <enforcement>
    - MUST execute all 7 steps in order
    - MUST NOT skip any step
    - MUST NOT auto-proceed past decision gates
    - MUST NOT continue until user responds to gates
  </enforcement>

  Step 1: Load Contracts (blocks if missing)
  Step 2: Quantitative Checks (depends on Step 1)
  Step 3: Semantic Validation (depends on Step 2, MUST use extended thinking)
  Step 4: Categorize Drift (depends on Step 2+3)
  Step 5: Present Findings (decision gate - blocks until user responds)
  Step 6: Execute User Choice (depends on Step 5 gate)
  Step 7: Route Back (handoff protocol to ui-mockup)
</critical_sequence>
```

**Why this matters:**
- Without enforcement, Claude may skip quantitative checks and jump to semantic validation
- Without decision gates, Claude may auto-select "recommended" options
- Without handoff protocol, Claude may not route back to ui-mockup correctly

## Examples of Issues Found

### Example 1: Missing Critical Sequence Enforcement (HIGH IMPACT)

**Issue:** The 7-step workflow lacks XML wrapper to enforce execution order.

**BEFORE (Lines 50-52):**
```markdown
## Workflow

### Step 1: Load Contracts
```

**AFTER:**
```markdown
## Workflow

<critical_sequence>
<sequence_name>design-sync-validation</sequence_name>
<enforcement>MUST execute all steps in order. MUST NOT skip steps. MUST NOT auto-proceed past decision gates.</enforcement>

### Step 1: Load Contracts
<step_requirement>MUST complete before Step 2. If files missing, BLOCK with error menu.</step_requirement>
```

**Impact:** Prevents Claude from skipping validation steps or executing out of order. Ensures reliable workflow execution.

### Example 2: Missing Decision Gate (HIGH IMPACT)

**Issue:** Decision menus lack explicit gate enforcement to prevent auto-proceeding.

**BEFORE (Lines 204-216):**
```markdown
#### If No Drift:

```
✓ Design-brief alignment verified

What's next?
1. Continue implementation (recommended)
2. Review details
3. Other
```
```

**AFTER:**
```markdown
#### If No Drift:

<decision_gate>
<gate_name>no-drift-confirmation</gate_name>
<blocking>true</blocking>
<wait_for_user>REQUIRED - Present menu and WAIT. MUST NOT auto-select option 1 even though marked "(recommended)".</wait_for_user>

```
✓ Design-brief alignment verified

What's next?
1. Continue implementation (recommended)
2. Review details
3. Other
```

</decision_gate>
```

**Impact:** Ensures Claude waits for user response instead of auto-selecting the "recommended" option. Critical for checkpoint protocol compliance.

### Example 3: Extract Example Scenarios (MEDIUM IMPACT)

**Issue:** 180 lines of example scenarios bloat the core instruction file.

**BEFORE (Lines 593-773):**
```markdown
## Example Scenarios

### Scenario 1: No Drift (Quick Validation)

**Plugin:** SimpleGain
**Brief:** 1 parameter (GAIN), minimal UI, slider only
**Mockup:** 1 parameter (GAIN), single vertical slider, dark background
[... 36 lines of detailed example ...]

### Scenario 2: Acceptable Evolution
[... 43 lines ...]

### Scenario 3: Drift Requiring Attention
[... 48 lines ...]

### Scenario 4: Critical Drift (Blocked)
[... 48 lines ...]
```

**AFTER (In SKILL.md):**
```markdown
## Example Scenarios

See `references/examples.md` for four detailed validation scenarios:
1. No Drift (Quick Validation) - Parameter match, visual alignment
2. Acceptable Evolution - Minor additions, core concept intact
3. Drift Requiring Attention - Missing features, style mismatch
4. Critical Drift (Blocked) - Contradicts core concept, massive scope change
```

**AFTER (Create references/examples.md):**
```markdown
# Design-Sync Validation Examples

[Move all 180 lines here with same formatting]
```

**Impact:** Reduces SKILL.md from 785 lines to ~610 lines (~900 token savings). Examples still available for reference but don't bloat core instructions.

### Example 4: Unclear Extended Thinking Instruction (LOW IMPACT)

**Issue:** Extended thinking prompt is shown but not explicitly instructed to be used.

**BEFORE (Line 141):**
```markdown
**Extended thinking prompt:**

```
Compare creative brief with mockup to assess alignment:
[...]
```
```

**AFTER:**
```markdown
**MUST use this exact extended thinking prompt for semantic validation:**

```
Compare creative brief with mockup to assess alignment:
[...]
```

**You MUST use extended thinking for Step 3. This is not optional. Budget: 8000 tokens.**
```

**Impact:** Makes it explicit that extended thinking is mandatory, not optional. Reduces chance Claude skips semantic validation.

### Example 5: Verbose "Why this matters" Section (LOW IMPACT)

**Issue:** 9 lines explaining importance when 2-3 would suffice.

**BEFORE (Lines 22-30):**
```markdown
**Why this matters:**

Creative briefs capture intent. Mockups capture reality. During design iteration, these can diverge:

- Brief says 8 parameters → Mockup has 12
- Brief says "vintage warmth" → Mockup is "modern minimal"
- Brief mentions preset browser → Mockup lacks it

Detecting drift early allows course correction before implementation locks in the design.
```

**AFTER:**
```markdown
**Why this matters:** Creative briefs capture intent; mockups capture reality. During iteration these can diverge (parameter mismatches, visual style drift, missing features). Detecting drift early enables course correction before implementation.
```

**Impact:** Reduces from 9 lines to 2 lines, saves ~50 tokens while preserving meaning.

## Estimated Impact

### Context Reduction
**Total potential savings: ~1150 tokens (15% reduction)**

Breakdown:
- Extract example scenarios: ~900 tokens
- Extract evolution template: ~150 tokens
- Extract pseudocode: ~100 tokens
- Condense "Why this matters": ~50 tokens
- Remove duplicate error handling: ~150 tokens
- **Total extraction: 1350 tokens**
- **XML additions: +200 tokens** (worth the overhead for enforcement)
- **Net savings: ~1150 tokens**

**File size reduction:** 785 lines → ~610 lines (22% smaller)

### Comprehension Improvement
**HIGH** - Adding XML enforcement tags would significantly improve Claude's ability to:
- Execute workflow steps in correct order
- Wait at decision gates instead of auto-proceeding
- Use extended thinking when required
- Route back to ui-mockup correctly

The current skill works in practice but lacks structural guarantees. Adding XML enforcement would prevent execution errors.

### Maintenance Improvement
**MEDIUM** - Extracting examples and templates to separate files would:
- Make SKILL.md more focused on instructions
- Allow examples to be updated independently
- Enable template reuse across skills
- Reduce cognitive load when reading core instructions

## Priority Recommendations

### 1. Add XML enforcement tags (CRITICAL)
**Priority:** HIGHEST
**Estimated time:** 15 minutes
**Impact:** Prevents Claude from skipping steps or auto-proceeding

Add these XML wrappers:
- `<critical_sequence>` around entire workflow (lines 50-389)
- `<decision_gate>` around each decision menu (lines 204-294 and throughout)
- `<handoff_protocol>` around routing back to ui-mockup (lines 355-389)
- `<requirements>` around success criteria (lines 456-466)
- `<anti-patterns>` around common pitfalls (lines 560-567)

**Why this is #1:** Without XML enforcement, Claude may deviate from mandatory workflow, skip validation steps, or auto-proceed past decision gates. This is the biggest risk to reliable execution.

### 2. Extract example scenarios to references/ (HIGH)
**Priority:** HIGH
**Estimated time:** 5 minutes
**Impact:** Saves ~900 tokens while keeping examples accessible

Move lines 593-773 (180 lines) to `references/examples.md`. Replace with brief summary and reference link. Examples are helpful but don't need to be loaded every execution.

### 3. Extract templates to assets/ (MEDIUM)
**Priority:** MEDIUM
**Estimated time:** 5 minutes
**Impact:** Saves ~250 tokens, improves reusability

Move:
- Evolution template (lines 302-330) → `assets/evolution-template.md`
- Extraction pseudocode (lines 82-98) → `references/extraction-logic.md`

Templates belong in assets for easy insertion into files.

## Implementation Notes

### Files that would be created in references/
1. **examples.md** (180 lines from lines 593-773)
   - Four detailed validation scenarios
   - Shows expected input/output for each drift category
   - Includes timing estimates

2. **extraction-logic.md** (16 lines from lines 82-98)
   - Pseudocode showing how to extract parameter counts
   - Feature detection patterns
   - Comparison logic

### Files that would be created in assets/
1. **evolution-template.md** (29 lines from lines 302-330)
   - Markdown template for documenting design evolution
   - Used when updating creative-brief.md
   - Includes placeholders for date, changes, rationale

### Files that already exist
1. **references/drift-detection.md** ✓
   - Already extracted
   - Contains drift categories and thresholds
   - Well-structured

### Estimated time to implement all fixes
**Total: 25-30 minutes**

Breakdown:
- Add XML tags: 15 minutes (most critical, needs careful placement)
- Extract examples: 5 minutes (copy-paste and update references)
- Extract templates: 5 minutes (copy-paste and update references)
- Update instructions: 5 minutes (explicit WAIT instructions, clear prompts)

**Recommended order:**
1. Add XML enforcement tags (prevents execution errors)
2. Extract examples (reduces context immediately)
3. Extract templates (cleanup and organization)
4. Polish instructions (clarity improvements)

## Health Assessment Justification

**Overall health: YELLOW (Functional but has critical comprehension risks)**

**Rationale:**
- The skill works in practice and follows many best practices
- Strong structure compliance, good system integration, clear language
- BUT: Missing XML enforcement creates comprehension risks
- Without `<critical_sequence>` and `<decision_gate>` tags, Claude may deviate from mandatory workflow
- Score of 3.0/5 reflects "works but needs structural improvements"

**Would be GREEN after fixes:**
- Adding XML enforcement → Score 4.0/5
- Adding XML + extractions → Score 4.5/5

**Not RED because:**
- Instructions are clear and detailed
- Decision menus follow checkpoint protocol format
- Integration with ui-mockup and plugin-workflow is documented
- Extended thinking is configured correctly
- The skill currently functions in production

The yellow rating indicates "needs attention" rather than "fundamentally broken."
