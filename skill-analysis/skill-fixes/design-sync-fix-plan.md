# Fix Plan: design-sync

## Summary
- Critical fixes: 7
- Extraction operations: 3
- Clarity improvements: 4
- Total estimated changes: 14
- Estimated time: 25-30 minutes

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Wrap entire workflow in `<critical_sequence>`
**Location:** Lines 50-389
**Operation:** WRAP
**Before:**
```markdown
## Workflow

### Step 1: Load Contracts

Read three files:
```

**After:**
```markdown
## Workflow

<critical_sequence>
<sequence_name>design-sync-validation</sequence_name>
<enforcement>MUST execute all 7 steps in order. MUST NOT skip any step. MUST NOT auto-proceed past decision gates. MUST NOT continue until user responds to gates.</enforcement>

### Step 1: Load Contracts
<step_requirement>MUST complete before Step 2. If files missing, BLOCK with error menu.</step_requirement>

Read three files:
```

**Closing tag location:** After Step 7 (line 389)
```markdown
[... end of Step 7 content ...]

</critical_sequence>
```

**Verification:** Check that `<critical_sequence>` opens at line 52 and closes after line 389

### Fix 1.2: Add step requirements throughout workflow
**Location:** Multiple locations within Lines 50-389
**Operation:** INSERT

**Step 2 (after "### Step 2: Quantitative Checks"):**
```markdown
<step_requirement>MUST complete before Step 3. Extract counts first, then compare.</step_requirement>
```

**Step 3 (after "### Step 3: Semantic Validation (Extended Thinking)"):**
```markdown
<step_requirement>MUST use extended thinking. MUST NOT skip semantic analysis.</step_requirement>
```

**Step 4 (after "### Step 4: Categorize Drift"):**
```markdown
<step_requirement>MUST categorize based on Step 2+3 findings before presenting.</step_requirement>
```

**Step 5 (after "### Step 5: Present Findings"):**
```markdown
<step_requirement>MUST present appropriate decision menu based on drift category.</step_requirement>
```

**Step 6 (after "### Step 6: Execute User Choice"):**
```markdown
<step_requirement>MUST execute choice from Step 5 gate before proceeding to Step 7.</step_requirement>
```

**Step 7 (after "### Step 7: Route Back to ui-mockup"):**
```markdown
<step_requirement>MUST present ui-mockup Phase 5.5 decision menu after completing user's choice.</step_requirement>
```

**Verification:** Each step has a `<step_requirement>` tag immediately after its heading

### Fix 1.3: Wrap "No Drift" decision menu in `<decision_gate>`
**Location:** Lines 204-216
**Operation:** WRAP
**Before:**
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

**After:**
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

**Verification:** Verify `<decision_gate>` tags surround the decision menu at lines 204-216

### Fix 1.4: Wrap "Acceptable Evolution" decision menu in `<decision_gate>`
**Location:** Lines 218-240
**Operation:** WRAP
**Before:**
```markdown
#### If Acceptable Evolution:

```
⚠️  Design evolution detected
```

**After:**
```markdown
#### If Acceptable Evolution:

<decision_gate>
<gate_name>acceptable-evolution-choice</gate_name>
<blocking>true</blocking>
<wait_for_user>REQUIRED - Present menu and WAIT. MUST NOT auto-proceed.</wait_for_user>

```
⚠️  Design evolution detected
```

**Closing tag:** Insert `</decision_gate>` after the decision menu ends (line 240)

**Verification:** Verify `<decision_gate>` tags surround lines 218-240

### Fix 1.5: Wrap "Drift Requiring Attention" decision menu in `<decision_gate>`
**Location:** Lines 243-267
**Operation:** WRAP
**Before:**
```markdown
#### If Drift Requiring Attention:

```
❌ Design drift detected
```

**After:**
```markdown
#### If Drift Requiring Attention:

<decision_gate>
<gate_name>drift-attention-choice</gate_name>
<blocking>true</blocking>
<wait_for_user>REQUIRED - Present menu and WAIT. MUST NOT auto-proceed.</wait_for_user>

```
❌ Design drift detected
```

**Closing tag:** Insert `</decision_gate>` after the decision menu ends (line 267)

**Verification:** Verify `<decision_gate>` tags surround lines 243-267

### Fix 1.6: Wrap "Critical Drift" decision menu in `<decision_gate>`
**Location:** Lines 270-294
**Operation:** WRAP
**Before:**
```markdown
#### If Critical Drift:

```
🚫 Critical design drift - blocked
```

**After:**
```markdown
#### If Critical Drift:

<decision_gate>
<gate_name>critical-drift-blocked</gate_name>
<blocking>true</blocking>
<wait_for_user>REQUIRED - Present menu and WAIT. MUST NOT provide override option for critical drift.</wait_for_user>

```
🚫 Critical design drift - blocked
```

**Closing tag:** Insert `</decision_gate>` after the decision menu ends (line 294)

**Verification:** Verify `<decision_gate>` tags surround lines 270-294

### Fix 1.7: Add `<handoff_protocol>` wrapper for routing back to ui-mockup
**Location:** Lines 355-389 (within Step 7)
**Operation:** WRAP
**Before:**
```markdown
Present ui-mockup's Phase 5.5 decision menu (but remove the "Check alignment" option since we just did that):
```

**After:**
```markdown
<handoff_protocol>
<target_skill>ui-mockup</target_skill>
<target_phase>5.5</target_phase>
<handoff_type>return_to_workflow</handoff_type>
<required_menu>Phase 5.5 decision menu (without "Check alignment" option)</required_menu>

Present ui-mockup's Phase 5.5 decision menu (but remove the "Check alignment" option since we just did that):
```

**Closing tag:** Insert `</handoff_protocol>` after the decision menu in Step 7 (line 389)

**Verification:** Verify `<handoff_protocol>` tags surround handoff instructions at lines 355-389

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Extract example scenarios to references/
**Location:** Lines 593-773 (180 lines)
**Operation:** EXTRACT
**Create file:** `references/examples.md`
**File content:**
```markdown
# Design-Sync Validation Examples

These scenarios demonstrate design-sync validation across different drift categories.

## Scenario 1: No Drift (Quick Validation)

**Plugin:** SimpleGain
**Brief:** 1 parameter (GAIN), minimal UI, slider only
**Mockup:** 1 parameter (GAIN), single vertical slider, dark background

**Validation:**

```
Step 1: Load contracts ✓
Step 2: Quantitative checks
  - Brief parameter count: 1 (GAIN)
  - Parameter-spec count: 1 (GAIN)
  - Mockup parameter count: 1 (GAIN)
  - ✓ Parameter counts match

Step 3: Semantic validation (extended thinking)
  Prompt: "Compare creative brief with mockup..."
  Analysis:
    - Visual style: Brief says "minimal", mockup is minimal ✓
    - Feature completeness: Brief mentions slider only, mockup shows slider only ✓
    - Core concept: Simple gain control, mockup matches ✓
    - Missing features: None
    - Extra features: None

Step 4: Categorize drift
  - Parameter match: 100%
  - Visual alignment: High
  - Feature parity: 100%
  - Category: NO DRIFT

Step 5: Present findings
  ✓ Design-brief alignment verified

  No issues detected:
  - ✓ Parameter count matches (1)
  - ✓ Visual style aligns (minimal → minimal)
  - ✓ All features present

  What's next?
  1. Continue implementation (recommended)
  2. Review details
  3. Other

Step 6: User selects option 1

Step 7: Route back
  [Present ui-mockup Phase 5.5 decision menu]
```

**Duration:** ~90 seconds (minimal validation)

---

## Scenario 2: Acceptable Evolution

**Plugin:** VintageEQ
**Brief:** 3-band EQ, vintage warmth, 6 parameters (LOW_FREQ, LOW_GAIN, MID_FREQ, MID_GAIN, HIGH_FREQ, HIGH_GAIN)
**Mockup:** 3-band EQ with 8 parameters (added OUTPUT_GAIN, MIX for parallel processing)

**Validation:**

```
Step 1: Load contracts ✓
Step 2: Quantitative checks
  - Brief parameter count: 6
  - Parameter-spec count: 8
  - Mockup parameter count: 8
  - ❌ Parameter mismatch (+2 parameters)

Step 3: Semantic validation (extended thinking)
  Prompt: "Compare creative brief with mockup..."
  Analysis:
    - Visual style: Brief says "vintage warmth", mockup has warm colors, analog-style knobs ✓
    - Core concept: 3-band EQ preserved ✓
    - Extra features detected:
      * OUTPUT_GAIN (standard mixing utility)
      * MIX (parallel processing - common EQ feature)
    - Assessment: Additions are natural extensions, don't contradict core concept
    - Core identity: Still a vintage 3-band EQ

Step 4: Categorize drift
  - Parameter match: 75% (6/8 match, 2 additions)
  - Visual alignment: High
  - Feature additions: Minor utility features
  - Contradictions: None
  - Category: ACCEPTABLE EVOLUTION

Step 5: Present findings
  ⚠️  Design evolution detected

  The mockup evolved beyond the original brief:

  **Added features:**
  - OUTPUT_GAIN parameter (mixing utility)
  - MIX parameter (parallel processing)

  **Assessment:**
  These additions are natural extensions that don't contradict the core concept.
  The plugin remains a vintage 3-band EQ at its core.

  What's next?
  1. Document evolution and continue (recommended)
  2. Revert mockup to original spec
  3. Update brief to match mockup
  4. Other

Step 6: User selects option 1 (document evolution)

  You're asked: "Document this evolution in the creative-brief's Evolution section"

  [Using assets/evolution-template.md:]

  ### Evolution Log

  #### 2025-11-12: Added mixing controls

  **Changes:**
  - Added OUTPUT_GAIN parameter for level control
  - Added MIX parameter for parallel processing

  **Rationale:**
  These are standard mixing utilities that complement the EQ without changing its core identity.
  Users expect output control and parallel processing in modern EQ plugins.

  **Impact:**
  - Parameter count: 6 → 8
  - Scope: Minor expansion (mixing utilities)
  - Core concept: Unchanged (still vintage 3-band EQ)

Step 7: Route back
  [Present ui-mockup Phase 5.5 decision menu]
```

**Duration:** ~3 minutes (includes documentation)

---

## Scenario 3: Drift Requiring Attention

**Plugin:** LushChorus
**Brief:** Simple stereo chorus, 4 parameters (RATE, DEPTH, MIX, WIDTH), "lush and spacious" sound
**Mockup:** Multi-mode modulation unit with 12 parameters including FEEDBACK, DELAY_TIME, FLUTTER, FILTER, multiple LFO shapes

**Validation:**

```
Step 1: Load contracts ✓
Step 2: Quantitative checks
  - Brief parameter count: 4
  - Parameter-spec count: 12
  - Mockup parameter count: 12
  - ❌ Major parameter expansion (+8 parameters, 300% increase)

Step 3: Semantic validation (extended thinking)
  Prompt: "Compare creative brief with mockup..."
  Analysis:
    - Core concept: Brief says "simple chorus", mockup is complex modulation unit
    - Visual style: Brief implies simplicity, mockup shows multi-tab interface
    - Feature additions:
      * FEEDBACK (moves toward flanger territory)
      * DELAY_TIME (manual delay control)
      * FLUTTER (tape modulation)
      * FILTER (additional processing)
      * Multiple LFO shapes (beyond basic chorus)
    - Assessment: Significant scope expansion
    - Identity shift: From "simple chorus" to "modulation workstation"

Step 4: Categorize drift
  - Parameter match: 33% (4/12, massive expansion)
  - Visual complexity: High (multi-tab vs. single panel)
  - Feature expansion: Major (simple → complex)
  - Scope change: Significant
  - Category: DRIFT REQUIRING ATTENTION

Step 5: Present findings
  ❌ Design drift detected

  The mockup significantly diverges from the original brief:

  **Mismatches:**
  - Parameter count: 4 → 12 (300% increase)
  - Complexity: "Simple chorus" → Multi-mode modulation unit
  - Visual layout: Single panel → Multi-tab interface
  - Feature scope: Basic chorus → Flanger, delay, tape flutter, filtering

  **Assessment:**
  The plugin has evolved from a simple, focused chorus into a complex modulation workstation.
  This contradicts the "simple" and "focused" goals in the brief.

  **Recommendation:**
  Either update the brief to embrace the expanded scope, or simplify the mockup to match the original vision.

  What's next?
  1. Update brief to match expanded scope
  2. Simplify mockup to match original brief
  3. Continue anyway (document override)
  4. Other

Step 6: User must decide (no auto-recommendation)

  If user selects "Update brief":
    [Skill updates creative-brief.md with expanded scope, new parameter list, updated vision]

  If user selects "Simplify mockup":
    [Skill returns error - cannot modify mockup, must invoke ui-mockup skill]
    Menu:
    1. Invoke ui-mockup to simplify design
    2. Switch to updating brief instead
    3. Cancel

  If user selects "Continue anyway":
    [Log override to .validator-overrides.yaml]

Step 7: Route back
  [Present ui-mockup Phase 5.5 decision menu]
```

**Duration:** ~5 minutes (requires user decision and document updates)

---

## Scenario 4: Critical Drift (Blocked)

**Plugin:** MinimalDelay
**Brief:** "Ultra-minimal delay - time, feedback, mix only. Clean, simple, no frills."
**Mockup:** Feature-rich delay with 20+ parameters including modulation, filtering, distortion, reverb, preset browser, spectrum analyzer

**Validation:**

```
Step 1: Load contracts ✓
Step 2: Quantitative checks
  - Brief parameter count: 3 (TIME, FEEDBACK, MIX)
  - Parameter-spec count: 24
  - Mockup parameter count: 24
  - 🚫 Massive parameter expansion (+21 parameters, 800% increase)

Step 3: Semantic validation (extended thinking)
  Prompt: "Compare creative brief with mockup..."
  Analysis:
    - Core concept: Brief explicitly says "ultra-minimal", "no frills"
    - Mockup reality: Feature-rich, complex, many frills
    - Direct contradictions:
      * Brief: "time, feedback, mix only" → Mockup: 24 parameters
      * Brief: "clean" → Mockup: distortion, modulation
      * Brief: "simple, no frills" → Mockup: spectrum analyzer, preset browser
    - Assessment: Complete contradiction of stated goals
    - Identity: "Minimal" → "Maximal"

Step 4: Categorize drift
  - Parameter match: 12.5% (3/24, catastrophic expansion)
  - Core concept: Directly contradicted
  - Stated goals: Violated ("no frills" → many frills)
  - Scope change: Fundamental transformation
  - Category: CRITICAL DRIFT

Step 5: Present findings
  🚫 Critical design drift - blocked

  The mockup fundamentally contradicts the creative brief:

  **Critical issues:**
  - Brief explicitly says "ultra-minimal" → Mockup is feature-rich
  - Brief says "time, feedback, mix only" → Mockup has 24 parameters
  - Brief says "no frills" → Mockup has modulation, filtering, distortion, reverb, analyzer, presets
  - Parameter count: 3 → 24 (800% expansion)

  **Assessment:**
  This is not evolution - it's a complete contradiction of the stated vision.
  Either the brief or the mockup needs to be completely redone.

  **Required action:**
  You must choose one:
  1. Completely rewrite brief to embrace complex delay concept
  2. Start over with new mockup matching minimal vision
  3. Abandon MinimalDelay and start new plugin concept

  What's next?
  1. Rewrite creative brief (new plugin concept)
  2. Restart mockup process (match minimal vision)
  3. Abandon plugin and start over
  4. Other

  Note: No "continue anyway" option - this is a blocking issue.

Step 6: User must resolve contradiction (blocking)

  If user selects "Rewrite brief":
    [Skill cannot rewrite brief - too fundamental]
    Error: "Critical drift requires manual intervention. Please update creative-brief.md yourself, then re-run design-sync."

  If user selects "Restart mockup":
    [Skill returns to ui-mockup skill]
    Menu:
    1. Invoke ui-mockup to start fresh design
    2. Cancel and resolve manually

  If user selects "Abandon plugin":
    [Skill exits, suggests /dream for new concept]

Step 7: Route back
  [Only if user resolves contradiction]
  [Present ui-mockup Phase 5.5 decision menu]
```

**Duration:** ~10 minutes (requires fundamental decisions and rework)

---

## Usage Notes

- **Scenario 1** (No Drift): Fast validation, typical for simple plugins or early iterations
- **Scenario 2** (Acceptable Evolution): Natural feature additions that complement core concept
- **Scenario 3** (Drift Requiring Attention): Scope expansion that needs explicit approval
- **Scenario 4** (Critical Drift): Fundamental contradictions that block implementation

The validation skill should aim for Scenario 1 (quick pass) or Scenario 2 (document evolution).
Scenarios 3-4 indicate planning issues that need resolution before implementation continues.
```

**Replace with (in SKILL.md at lines 593-773):**
```markdown
## Example Scenarios

See `references/examples.md` for four detailed validation scenarios:
1. **No Drift (Quick Validation)** - Parameter match, visual alignment (~90 seconds)
2. **Acceptable Evolution** - Minor additions, core concept intact (~3 minutes)
3. **Drift Requiring Attention** - Missing features, style mismatch (~5 minutes)
4. **Critical Drift (Blocked)** - Contradicts core concept, massive scope change (~10 minutes)
```

**Verification:**
- File `references/examples.md` exists with 180 lines of content
- SKILL.md reduced from 785 lines to ~610 lines
- Reference to examples.md present at original location (line 593)

### Fix 2.2: Extract evolution template to assets/
**Location:** Lines 302-330 (29 lines)
**Operation:** EXTRACT
**Create file:** `assets/evolution-template.md`
**File content:**
```markdown
### Evolution Log

#### [DATE]: [Brief description of change]

**Changes:**
- [Specific change 1]
- [Specific change 2]

**Rationale:**
[Why these changes were made. What problem do they solve? How do they improve the plugin?]

**Impact:**
- Parameter count: [before] → [after]
- Scope: [Minor/Moderate/Major expansion]
- Core concept: [Unchanged/Enhanced/Shifted]
```

**Replace with (in SKILL.md at lines 302-330):**
```markdown
Use the template from `assets/evolution-template.md` to document the evolution:

[Insert populated evolution log entry into creative-brief.md]
```

**Verification:**
- File `assets/evolution-template.md` exists
- SKILL.md reference updated at line 302
- Template is ~29 lines shorter in SKILL.md

### Fix 2.3: Extract extraction logic pseudocode to references/
**Location:** Lines 82-98 (16 lines)
**Operation:** EXTRACT
**Create file:** `references/extraction-logic.md`
**File content:**
```markdown
# Parameter Extraction Logic

This pseudocode shows how to extract parameter counts and feature lists from contracts.

## Extraction Patterns

```typescript
// From creative-brief.md
briefParamCount = extractParameterCount(brief);
briefFeatures = extractFeatureKeywords(brief); // ["preset", "bypass", "meter", etc.]

// From parameter-spec.md
specParamCount = countParameters(parameterSpec);
specParamList = extractParameterNames(parameterSpec);

// From mockup YAML (render-spec.yaml)
mockupParamCount = countControlsInLayout(mockup.layout);
mockupFeaturesList = detectUIFeatures(mockup); // ["preset_browser", "meter", etc.]

// Compare
parameterMatch = (briefParamCount == specParamCount == mockupParamCount);
featureMatch = compareFeatureLists(briefFeatures, mockupFeaturesList);
```

## Feature Detection Keywords

**Brief keywords to look for:**
- "preset" / "preset browser" / "preset management"
- "bypass" / "wet/dry" / "mix"
- "meter" / "visualization" / "spectrum"
- "tabs" / "pages" / "sections"
- Parameter names in quotes or caps (e.g., "GAIN", "FREQUENCY")

**Mockup YAML features:**
- `preset_browser` component
- `meter` / `spectrum_analyzer` components
- `tabs` / `multi_page_layout`
- Count of `knob`, `slider`, `button` elements

## Comparison Logic

```typescript
if (parameterMatch && featureMatch) {
  return "NO_DRIFT";
} else if (minorAdditions && coreConceptIntact) {
  return "ACCEPTABLE_EVOLUTION";
} else if (majorExpansion || missingFeatures) {
  return "DRIFT_REQUIRING_ATTENTION";
} else if (contradictsCoreGoals) {
  return "CRITICAL_DRIFT";
}
```
```

**Replace with (in SKILL.md at lines 82-98):**
```markdown
**Extract parameter counts and feature lists from contracts using the patterns in `references/extraction-logic.md`:**

- Count parameters mentioned in creative-brief.md
- Count parameters in parameter-spec.md
- Count controls in mockup render-spec.yaml
- Detect feature keywords (preset, bypass, meter, tabs, etc.)
- Compare for mismatches
```

**Verification:**
- File `references/extraction-logic.md` exists with pseudocode
- SKILL.md reference updated at line 82
- Context reduced by ~100 tokens

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Condense "Why this matters" section
**Location:** Lines 22-30
**Operation:** REPLACE
**Before:**
```markdown
**Why this matters:**

Creative briefs capture intent. Mockups capture reality. During design iteration, these can diverge:

- Brief says 8 parameters → Mockup has 12
- Brief says "vintage warmth" → Mockup is "modern minimal"
- Brief mentions preset browser → Mockup lacks it

Detecting drift early allows course correction before implementation locks in the design.
```

**After:**
```markdown
**Why this matters:** Creative briefs capture intent; mockups capture reality. During iteration these can diverge (parameter mismatches, visual style drift, missing features). Detecting drift early enables course correction before implementation.
```

**Verification:** Section reduced from 9 lines to 2 lines while preserving meaning

### Fix 3.2: Make extended thinking instruction explicit
**Location:** Line 141
**Operation:** REPLACE
**Before:**
```markdown
**Extended thinking prompt:**

```
Compare creative brief with mockup to assess alignment:
```

**After:**
```markdown
**MUST use this exact extended thinking prompt for semantic validation:**

```
Compare creative brief with mockup to assess alignment:
[prompt text continues...]
```

**You MUST use extended thinking for Step 3. This is not optional. Budget: 8000 tokens.**
```

**Verification:** Instruction now explicitly states "MUST use" and "not optional"

### Fix 3.3: Add explicit WAIT instructions to execution rules
**Location:** Lines 547-558
**Operation:** REPLACE
**Before:**
```markdown
**When executing this skill:**

1. Always load all three contracts first (brief, parameter-spec, mockup YAML)
2. Always run quantitative checks before semantic validation (faster)
3. Use extended thinking for semantic validation (8k budget)
4. Categorize drift objectively (use thresholds from references/drift-detection.md)
5. Present appropriate decision menu (based on drift category)
6. If user approves evolution, update brief's Evolution section
7. Log overrides to .validator-overrides.yaml
8. Never auto-override (user must explicitly choose)
```

**After:**
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

**Verification:** Rules now use MUST/ALWAYS/NEVER and include explicit WAIT instruction (rule #6)

### Fix 3.4: Wrap anti-patterns in XML
**Location:** Lines 560-567
**Operation:** WRAP
**Before:**
```markdown
**Common pitfalls:**

- Forgetting to use extended thinking for semantic validation (critical for accuracy)
- Auto-categorizing as "acceptable" without checking (be objective)
- Not presenting "Continue anyway" for non-critical drift (user should have option)
- Providing "override" for critical drift (should be blocked)
- Updating mockup instead of brief (mockup is source of truth, brief gets updated)
- Not logging overrides (audit trail required)
```

**After:**
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

**Verification:** Anti-patterns section wrapped in XML tags with "AVOID THESE" emphasis

### Fix 3.5: Wrap success criteria in `<requirements>`
**Location:** Lines 456-466
**Operation:** WRAP
**Before:**
```markdown
## Success Criteria

Validation is successful when:

- ✅ Both contracts loaded (brief + parameter-spec + mockup YAML)
- ✅ Quantitative checks completed (parameter count, feature detection)
- ✅ Semantic validation performed (extended thinking analysis)
- ✅ Drift category assigned (none / acceptable / attention / critical)
- ✅ Appropriate decision menu presented
- ✅ User action executed (update brief / update mockup / override)
```

**After:**
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

**Verification:** Success criteria wrapped in `<requirements>` with explicit "ALL" and "MUST NOT" enforcement

### Fix 3.6: Remove duplicate error handling section
**Location:** Lines 56-78
**Operation:** DELETE
**Before:**
```markdown
[Duplicate error handling section before Step 1]
```

**After:**
```markdown
[Section removed - keep only the error handling within Step 1]
```

**Verification:** Lines 56-78 deleted, error handling only appears in Step 1

## File Creation Manifest

Files to create:
1. `references/examples.md` - Four detailed validation scenarios (No Drift, Acceptable Evolution, Drift Requiring Attention, Critical Drift)
2. `references/extraction-logic.md` - Pseudocode for extracting parameter counts and features from contracts
3. `assets/evolution-template.md` - Markdown template for documenting design evolution in creative-brief.md

## Execution Checklist

### Phase 1: Critical Fixes
- [ ] Entire workflow (lines 50-389) wrapped in `<critical_sequence>` with enforcement tag
- [ ] Each step (Steps 1-7) has `<step_requirement>` tag after heading
- [ ] "No Drift" decision menu wrapped in `<decision_gate>` (lines 204-216)
- [ ] "Acceptable Evolution" decision menu wrapped in `<decision_gate>` (lines 218-240)
- [ ] "Drift Requiring Attention" decision menu wrapped in `<decision_gate>` (lines 243-267)
- [ ] "Critical Drift" decision menu wrapped in `<decision_gate>` (lines 270-294)
- [ ] Step 7 routing wrapped in `<handoff_protocol>` (lines 355-389)

### Phase 2: Content Extraction
- [ ] File `references/examples.md` created with 180 lines of scenarios
- [ ] SKILL.md lines 593-773 replaced with brief summary and reference
- [ ] File `assets/evolution-template.md` created with template
- [ ] SKILL.md lines 302-330 replaced with reference to assets file
- [ ] File `references/extraction-logic.md` created with pseudocode
- [ ] SKILL.md lines 82-98 replaced with brief instruction and reference
- [ ] Context reduced by ~1150 tokens (785 lines → 610 lines)

### Phase 3: Polish
- [ ] "Why this matters" section condensed (lines 22-30, 9 lines → 2 lines)
- [ ] Extended thinking instruction made explicit with "MUST use" (line 141)
- [ ] Execution rules updated with MUST/ALWAYS/NEVER and explicit WAIT (lines 547-558)
- [ ] Anti-patterns section wrapped in `<anti-patterns>` XML (lines 560-567)
- [ ] Success criteria wrapped in `<requirements>` XML (lines 456-466)
- [ ] Duplicate error handling section removed (lines 56-78)

### Verification
- [ ] All `<critical_sequence>`, `<decision_gate>`, and `<handoff_protocol>` tags properly opened and closed
- [ ] All extracted files exist in correct locations (references/ and assets/)
- [ ] All references in SKILL.md point to correct file paths
- [ ] SKILL.md line count reduced from 785 to ~610 lines
- [ ] Token count reduced by ~1150 tokens (15% reduction)
- [ ] All decision menus have explicit wait_for_user enforcement
- [ ] Extended thinking is marked as mandatory (not optional)
- [ ] Skill health score estimated to improve from 3.0/5 to 4.5/5
