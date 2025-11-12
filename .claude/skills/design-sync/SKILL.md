---
name: design-sync
description: Validate mockup ↔ creative brief consistency, catch design drift
allowed-tools:
  - Read # Read contracts (brief, parameter-spec, mockup YAML)
  - Edit # Update brief's Evolution section
preconditions:
  - creative-brief.md exists
  - Mockup has been finalized (parameter-spec.md generated)
extended-thinking: true
thinking-budget: 8000 # Moderate creative reasoning
---

# design-sync Skill

**Purpose:** Validate mockup ↔ creative brief consistency and catch design drift before implementation begins.

## Overview

This skill compares the finalized UI mockup against the original creative brief to detect drift—where the implemented design diverges from the original vision. Catches misalignments before Stage 5 (GUI implementation) starts, preventing wasted work.

**Why this matters:** Creative briefs capture intent; mockups capture reality. During iteration these can diverge (parameter mismatches, visual style drift, missing features). Detecting drift early enables course correction before implementation.

**Key innovation:** Dual validation (quantitative + semantic) with categorized drift levels and appropriate decision menus.

---

## Entry Points

**Invoked by:**

1. **Auto-invoked by ui-mockup** after Phase 4.5 finalization (before C++ generation)
2. **Manual:** `/sync-design [PluginName]`
3. **Stage 1 Planning:** Optional pre-check before implementation starts

**Entry parameters:**

- **Plugin name**: Which plugin to validate
- **Mockup version** (optional): Specific version to check (defaults to latest)

---

## Workflow

<critical_sequence>
<sequence_name>design-sync-validation</sequence_name>
<enforcement>MUST execute all 7 steps in order. MUST NOT skip any step. MUST NOT auto-proceed past decision gates. MUST NOT continue until user responds to gates.</enforcement>

### Step 1: Load Contracts
<step_requirement>MUST complete before Step 2. If files missing, BLOCK with error menu.</step_requirement>

Read three files:

- `plugins/[PluginName]/.ideas/creative-brief.md` - Original vision
- `plugins/[PluginName]/.ideas/parameter-spec.md` - Finalized parameters from mockup
- `plugins/[PluginName]/.ideas/mockups/vN-ui.yaml` - Finalized mockup spec

**Error handling:** If any missing, BLOCK with clear message:

```
❌ Cannot validate design sync

Missing required files:
- creative-brief.md: [EXISTS/MISSING]
- parameter-spec.md: [EXISTS/MISSING]
- mockups/v[N]-ui.yaml: [EXISTS/MISSING]

design-sync requires finalized mockup (parameter-spec.md generated).

Actions:
1. Generate parameter-spec.md - Finalize mockup first
2. Create creative brief - Document vision before mockup
3. Skip validation - Proceed without sync (not recommended)
4. Other
```

### Step 2: Quantitative Checks
<step_requirement>MUST complete before Step 3. Extract counts first, then compare.</step_requirement>

**Extract parameter counts and feature lists from contracts using the patterns in `references/extraction-logic.md`:**

- Count parameters mentioned in creative-brief.md
- Count parameters in parameter-spec.md
- Count controls in mockup render-spec.yaml
- Detect feature keywords (preset, bypass, meter, tabs, etc.)
- Compare for mismatches

**Parameter count thresholds:**

- Match (0-1 difference): No drift
- Small increase (2-4): Acceptable evolution
- Large increase (5+): Drift requiring attention
- Massive (2x or more): Critical drift
- Any decrease: Drift requiring attention (scope reduction)

**Feature detection:**
Keywords to search in brief:

- "preset", "presets"
- "bypass"
- "meter", "visualization"
- "undo/redo"
- "A/B compare"

Check if mockup YAML contains corresponding components.

### Step 3: Semantic Validation (Extended Thinking)
<step_requirement>MUST use extended thinking. MUST NOT skip semantic analysis.</step_requirement>

**Use extended thinking to answer:**

1. **Visual style alignment:**

   - Brief aesthetic: [extract quotes from "UI Vision" section]
   - Mockup aesthetic: [analyze YAML design system: colors, typography, layout style]
   - Match? Yes/No + reasoning

2. **Feature completeness:**

   - Brief promises: [list all mentioned features]
   - Mockup delivers: [list all implemented features]
   - Missing: [list gaps]
   - Assessment: Complete / Partial / Missing core features

3. **Scope assessment:**
   - Additions justified? (evolution vs creep)
   - Reductions justified? (simplification vs missing)
   - Core concept preserved?

**MUST use this exact extended thinking prompt for semantic validation:**

```
Compare creative brief with mockup to assess alignment:

Creative Brief:
- Concept: [extract]
- Use Cases: [extract]
- UI Vision: [extract]
- Parameters: [list]

Mockup:
- Layout: [from YAML]
- Components: [from YAML]
- Visual style: [colors, typography from YAML]
- Parameters: [from parameter-spec.md]

Answer:
1. Does mockup visual style match brief aesthetic intent? (vintage warmth vs modern, minimal vs complex, etc.)
2. Are all brief-mentioned features present?
3. Are mockup additions reasonable evolution or scope creep?
4. Does mockup support the use cases in brief?

Assess confidence: HIGH / MEDIUM / LOW
```

**You MUST use extended thinking for Step 3. This is not optional. Budget: 8000 tokens.**

### Step 4: Categorize Drift
<step_requirement>MUST categorize based on Step 2+3 findings before presenting.</step_requirement>

Based on findings:

**No drift detected:**

- Parameter counts match (±1)
- All features present
- Style aligned (semantic validation: YES)
- Mockup delivers on brief promise

**Acceptable evolution:**

- Parameter count increased slightly (2-4)
- Visual polish added
- Layout refined for UX
- Additions improve design (justified)
- Core concept intact

**Attention needed:**

- Missing features from brief
- Style mismatch (different direction)
- Significant scope change (±5 parameters)
- Brief and mockup tell different stories

**Critical drift:**

- Mockup contradicts brief's core concept
- Missing essential features
- Massive scope change (2x parameters or more)
- Completely opposite visual style

### Step 5: Present Findings
<step_requirement>MUST present appropriate decision menu based on drift category.</step_requirement>

#### If No Drift:

<decision_gate>
<gate_name>no-drift-confirmation</gate_name>
<blocking>true</blocking>
<wait_for_user>REQUIRED - Present menu and WAIT. MUST NOT auto-select option 1 even though marked "(recommended)".</wait_for_user>

```
✓ Design-brief alignment verified

- Parameter count: 12 (matches brief)
- All features present: preset browser, bypass, meter
- Visual style aligned: Modern minimal with analog warmth

What's next?
1. Continue implementation (recommended) - Alignment confirmed
2. Review details - See full comparison
3. Other
```

</decision_gate>

#### If Acceptable Evolution:

<decision_gate>
<gate_name>acceptable-evolution-choice</gate_name>
<blocking>true</blocking>
<wait_for_user>REQUIRED - Present menu and WAIT. MUST NOT auto-proceed.</wait_for_user>

```
⚠️ Design evolution detected (acceptable)

**Changes from brief:**
- Parameter count: 12 (brief: 8) +4 parameters
  - Added: TONE, DRIVE, MIX, WIDTH
- Added features: Visual feedback meter (not in brief)
- Visual refinements: Animation polish, gradient backgrounds

**Assessment:** Reasonable evolution based on design iteration

**Reasoning:**
Added parameters (TONE, DRIVE, MIX, WIDTH) provide necessary tonal shaping not
anticipated in original brief. Visual meter improves usability during performance.
Core concept ("vintage tape delay") preserved.

What's next?
1. Update brief and continue (recommended) - Document evolution
2. Review changes - See detailed comparison
3. Revert to original - Simplify mockup to match brief
4. Other
```

</decision_gate>

#### If Drift Requiring Attention:

<decision_gate>
<gate_name>drift-attention-choice</gate_name>
<blocking>true</blocking>
<wait_for_user>REQUIRED - Present menu and WAIT. MUST NOT auto-proceed.</wait_for_user>

```
⚠️ Design drift detected

**Issues found:**
1. Missing feature: "Preset browser" mentioned in brief, absent in mockup
   - Brief line 42: "Include preset system for patch recall"
   - Mockup: No preset-related components
2. Visual style mismatch:
   - Brief: "Vintage warmth with analog aesthetics"
   - Mockup: Stark modern minimal with flat colors
3. Parameter count: 5 (brief: 12) - significant reduction
   - Removed: FEEDBACK_TONE, MOD_DEPTH, SPREAD, MIX, WIDTH, DRY, WET

**Recommendation:** Address drift before implementation

**Confidence:** HIGH (clear quantitative + semantic misalignment)

What's next?
1. Update mockup - Add missing features, adjust style to match brief
2. Update brief - Brief overpromised, mockup is realistic scope
3. Continue anyway (override) - Accept drift, proceed with mockup as-is
4. Review detailed comparison - See side-by-side analysis
5. Other
```

</decision_gate>

#### If Critical Drift:

<decision_gate>
<gate_name>critical-drift-blocked</gate_name>
<blocking>true</blocking>
<wait_for_user>REQUIRED - Present menu and WAIT. MUST NOT provide override option for critical drift.</wait_for_user>

```
❌ Critical design drift - Implementation BLOCKED

**Critical issues:**
1. Brief core concept: "Tempo-synced rhythmic delay with modulation"
   Mockup delivers: Basic feedback delay (no tempo sync, no modulation)
2. Parameter count: 25 (brief: 5) - 5x scope creep
   - Brief: DELAY_TIME, FEEDBACK, MIX, TONE, DRIVE
   - Mockup: 20 additional parameters for features not mentioned in brief

**Action required:** Resolve drift before implementation can proceed

**Why blocking:**
Mockup contradicts core concept. Implementation would not match user's vision.
High risk of complete rework.

What's next?
1. Update mockup - Align with brief's core concept (tempo sync + modulation)
2. Update brief - Revise concept to match mockup's approach (basic delay)
3. Start over - Create new mockup from brief
4. Other

(Option to override not provided - critical drift must be resolved)
```

</decision_gate>

### Step 6: Execute User Choice
<step_requirement>MUST execute choice from Step 5 gate before proceeding to Step 7.</step_requirement>

**Option 1: Update brief and continue**

Use the template from `assets/evolution-template.md` to document the evolution:

[Insert populated evolution log entry into creative-brief.md]

Update brief's "UI Vision" section to reflect current mockup (preserve original in "Evolution" section).

**Option 2: Update mockup**

- Return user to ui-mockup skill
- Present drift findings
- Iterate design to align with brief
- Re-run design-sync after changes

**Option 3: Continue anyway (override)**

- Log override to `.validator-overrides.yaml`:
  ```yaml
  - date: 2025-11-10
    validator: design-sync
    finding: parameter-count-mismatch
    severity: attention
    override-reason: User confirmed evolution is intentional
    mockup-version: v3
  ```
- Return success (allow implementation)
- Warn: "Implementation may not match original vision"

### Step 7: Route Back to ui-mockup
<step_requirement>MUST present ui-mockup Phase 5.5 decision menu after completing user's choice.</step_requirement>

<handoff_protocol>
<target_skill>ui-mockup</target_skill>
<target_phase>5.5</target_phase>
<handoff_type>return_to_workflow</handoff_type>
<required_menu>Phase 5.5 decision menu (without "Check alignment" option)</required_menu>

**After Step 6 actions complete (brief updated, mockup changed, or override logged), return to ui-mockup Phase 5.5 decision menu.**

**Present this menu:**

```
✓ Design-brief alignment complete

What's next?
1. Finalize and create implementation files (if satisfied and aligned)
2. Provide more refinements (iterate on design) ← Creates v[N+1]
3. Test in browser (open v[N]-ui-test.html)
4. Save as aesthetic template (add to library for reuse)
5. Finalize AND save aesthetic (do both operations)
6. Other

Choose (1-6): _
```

**This is the same decision point as ui-mockup Phase 5.5, minus the "Check alignment" option (already done).**

**Option handling:**
- **Option 1**: Proceed to ui-mockup Phase 6-9 (generate remaining 5 files)
- **Option 2**: Return to ui-mockup Phase 2 with new version number (iterate design)
- **Option 3**: Open test HTML in browser for review
- **Option 4**: Invoke ui-template-library "save" operation
- **Option 5**: Save aesthetic, then proceed to Phase 6-9
- **Option 6**: Other

**Why route back:**
- Validates the design is aligned before generating implementation files
- Prevents generating C++ boilerplate for misaligned mockups
- Maintains checkpoint protocol (user decides next action after validation)

</handoff_protocol>

</critical_sequence>

---

## Integration with ui-mockup

**ui-mockup Phase 4.5** (after finalization, before C++ generation):

```markdown
✓ Mockup finalized: v3

parameter-spec.md generated (12 parameters)

What's next?

1. Check alignment - Run design-sync validation (recommended)
2. Generate implementation files - Proceed to Phase 5
3. Iterate design - Make more changes
4. Other
```

If user chooses "Check alignment" → invoke design-sync skill

**Flow:**

1. ui-mockup finishes Phase 4 (generates YAML + test HTML + parameter-spec.md)
2. Presents decision menu with "Check alignment" option
3. If chosen: Invokes design-sync skill
4. design-sync validates, presents findings
5. User resolves any drift
6. Returns to ui-mockup to continue Phase 5 (or iterate)

---

## Integration with plugin-workflow Stage 1

**Optional pre-check before planning:**

Stage 1 Planning detects finalized mockup:

```markdown
Starting Stage 1: Planning

Contracts loaded:

- creative-brief.md: EXISTS
- parameter-spec.md: EXISTS (from mockup v3)

Mockup finalized: Yes (v3)
design-sync validation: Not yet run

Recommendation: Validate brief ↔ mockup alignment before planning

What's next?

1. Run design-sync - Validate alignment (recommended)
2. Skip validation - Proceed with planning (trust mockup)
3. Other
```

If user chooses "Run design-sync" → invoke this skill

**Why pre-check matters:**

Stage 1 generates plan.md based on parameter-spec.md. If parameter-spec doesn't match brief vision, plan will be misaligned. Better to catch drift before planning.

---

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

---

## Error Handling

**Missing contracts:**

```
❌ Cannot validate: creative-brief.md not found

design-sync requires creative brief to validate against mockup.

Actions:
1. Create brief - Document vision before validation
2. Skip validation - Proceed without sync (not recommended)
3. Other
```

**No mockup finalized:**

```
❌ Cannot validate: No finalized mockup found

design-sync requires:
- mockups/v[N]-ui.yaml (finalized mockup spec)
- parameter-spec.md (generated from mockup)

Current state:
- Mockups: v1-ui.yaml, v2-ui.yaml (no parameter-spec.md)

Actions:
1. Finalize mockup - Generate parameter-spec.md via ui-mockup
2. Skip validation - Proceed without sync (not recommended)
3. Other
```

**Ambiguous findings:**

```
⚠️ Drift assessment uncertain

Quantitative checks:
- Parameter count: 10 (brief: 8) +2 parameters ← Minor difference
- Features: All present

Semantic validation:
- Visual style: MEDIUM confidence alignment
  - Brief aesthetic: "Vintage warmth"
  - Mockup aesthetic: "Brushed metal with warm colors"
  - Ambiguous: Is brushed metal "vintage"?

**Asking for user input:**
Is this mockup style aligned with your "vintage warmth" vision?

1. Yes - Style matches intent (acceptable evolution)
2. No - Style misses the mark (drift requiring attention)
3. Review side-by-side - See comparison
4. Other
```

**Override tracking:**

All overrides logged to `.validator-overrides.yaml`:

```yaml
overrides:
  - timestamp: 2025-11-10T14:32:00Z
    validator: design-sync
    plugin: DelayPlugin
    mockup-version: v3
    finding: parameter-count-increase
    severity: attention
    details: "Brief: 8 parameters, Mockup: 12 parameters (+4)"
    override-reason: "User confirmed: added parameters are intentional (TONE, DRIVE, MIX, WIDTH)"
    approved-by: User
```

Enables audit trail: "Why did we proceed with drift?"

---

## Notes for Claude

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

<anti-patterns>
**Common pitfalls (AVOID THESE):**

- Forgetting to use extended thinking for semantic validation (critical for accuracy)
- Auto-categorizing as "acceptable" without checking (be objective)
- Not presenting "Continue anyway" for non-critical drift (user should have option)
- Providing "override" for critical drift (should be blocked)
- Updating mockup instead of brief (mockup is source of truth, brief gets updated)
- Not logging overrides (audit trail required)
</anti-patterns>

---

## Quality Guidelines

**Good drift detection:**

- ✅ Specific findings ("Missing feature: preset browser from brief line 42")
- ✅ Quantitative evidence (parameter counts, feature lists)
- ✅ Semantic reasoning (why style does/doesn't match aesthetic)
- ✅ Confidence levels stated (HIGH/MEDIUM/LOW)
- ✅ Actionable recommendations

**Avoid:**

- ❌ Vague assessments ("Mockup looks different")
- ❌ Missing quantitative data (no parameter counts)
- ❌ No semantic analysis (just counting, no reasoning)
- ❌ Unclear confidence (is this certain or uncertain?)
- ❌ No clear recommendations (what should user do?)

---

## Example Scenarios

See `references/examples.md` for four detailed validation scenarios:
1. **No Drift (Quick Validation)** - Parameter match, visual alignment (~90 seconds)
2. **Acceptable Evolution** - Minor additions, core concept intact (~3 minutes)
3. **Drift Requiring Attention** - Missing features, style mismatch (~5 minutes)
4. **Critical Drift (Blocked)** - Contradicts core concept, massive scope change (~10 minutes)

---

## Future Enhancements

**Not in Phase 7 scope, but potential:**

- Visual diff tool (side-by-side brief vs mockup comparison)
- Automated brief updating (AI-generated Evolution sections)
- Historical drift tracking (drift patterns over time)
- Confidence calibration (track prediction accuracy)
- Integration with version control (git diff for brief changes)
