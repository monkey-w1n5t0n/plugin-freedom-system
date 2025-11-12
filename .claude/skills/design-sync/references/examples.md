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
