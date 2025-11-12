# Skill Analysis: ui-mockup

## Executive Summary
- Overall health: **Yellow** (Good structure, needs enforcement improvements)
- Critical issues: **3**
- Optimization opportunities: **8**
- Estimated context savings: **~4,200 tokens / 13% reduction**

## Dimension Scores (1-5)

1. Structure Compliance: **5/5**
   - Perfect YAML frontmatter format
   - Description includes trigger terms ("WebView UI mockups", "design iteration", "implementation scaffolding")
   - allowed-tools appropriate (Read, Write only)
   - preconditions clear (None - can work standalone)

2. Content Extraction: **3/5**
   - Good use of references/ and assets/ directories
   - SKILL.md still contains 987 lines (could be more aggressive)
   - Some large procedural blocks should be extracted
   - Template code examples embedded inline (should be in assets/)

3. Instruction Clarity: **4/5**
   - Sequential steps use numbered lists consistently
   - Requirements marked explicitly (CRITICAL, MUST, ALWAYS)
   - Conditionals structured well (IF X THEN Y)
   - Success criteria defined but buried at end
   - Examples follow complex instructions

4. XML Organization: **2/5**
   - Minimal XML tagging (only in embedded code examples)
   - Critical sequences not wrapped in XML tags
   - Decision gates not explicitly tagged
   - No `<critical_sequence>`, `<decision_gate>`, or `<delegation_rule>` tags
   - Phase sections could use semantic XML for enforcement

5. Context Efficiency: **3/5**
   - No obvious duplication within skill
   - Some verbosity in phase descriptions
   - Large code blocks inline (should reference assets/)
   - Progressive disclosure partially implemented

6. Claude-Optimized Language: **4/5**
   - Clear antecedents throughout
   - Explicit instructions (not implicit)
   - Direct, unambiguous phrasing
   - Concrete examples present but sometimes verbose

7. System Integration: **5/5**
   - Perfect checkpoint protocol adherence
   - Inline numbered decision menus (not AskUserQuestion)
   - Clear handoff protocols for ui-template-library and design-sync skills
   - State management (.continue-here.md) integration complete

8. Critical Sequence Enforcement: **2/5**
   - Two-phase workflow critical but not enforced with XML
   - Phase A → Phase B gate exists but relies on prose
   - No XML wrappers to prevent Claude from skipping Phase 5.5 decision menu
   - Validation requirements (Phase 5.3) could be stronger

## Critical Issues (blockers for Claude comprehension)

### Issue 1: Phase A/B Split Not Enforced (Lines 15-34)
**Problem:** The two-phase workflow (design iteration vs implementation scaffolding) is described in prose but lacks XML enforcement. Claude could generate all 7 files prematurely.

**Impact:** High - Defeats core workflow optimization (iterating before committing to C++ boilerplate).

**Solution:** Wrap in `<critical_sequence>` and `<decision_gate>` tags.

### Issue 2: Phase 5.5 Decision Menu Bypassing Risk (Lines 457-510)
**Problem:** The "CRITICAL STOP POINT" warning relies on bold text and emoji. No structural gate prevents Claude from proceeding to Phase 6.

**Impact:** High - User loses chance to iterate on design before implementation files are generated.

**Solution:** Wrap Phase 6-10 in `<conditional_execution>` that requires explicit Phase 5.5 approval.

### Issue 3: Validation Requirements Not Enforced (Lines 376-400)
**Problem:** WebView constraint validation (Phase 5.3) is described as "CRITICAL" but has no enforcement mechanism. Claude might skip validation and present invalid HTML.

**Impact:** Medium-High - Invalid mockups cause blank screens in JUCE WebView.

**Solution:** Wrap validation in `<requirement>` tag with explicit pass/fail gate.

## Optimization Opportunities

### Content to Extract to references/

#### 1. Lines 581-664: C++ Boilerplate Generation Logic (~84 lines)
**Extract to:** `references/cpp-boilerplate-generation.md`
**Estimated savings:** ~1,200 tokens

**Rationale:** These are implementation details for generating PluginEditor.h and .cpp. Can be referenced when needed in Phase 7.

**Before:**
```
### File 1: v[N]-PluginEditor.h (C++ Header)
[84 lines of template replacement logic]
```

**After:**
```
### File 1: v[N]-PluginEditor.h (C++ Header)
Generate using template replacement strategy from `references/cpp-boilerplate-generation.md#header-generation`.
```

#### 2. Lines 699-738: CMake Configuration Details (~40 lines)
**Extract to:** `references/cmake-generation.md`
**Estimated savings:** ~600 tokens

**Rationale:** CMake snippet generation rules are technical details, not workflow logic.

#### 3. Lines 115-150: Context Extraction Examples (~36 lines)
**Extract to:** `references/context-extraction.md` (append to existing)
**Estimated savings:** ~500 tokens

**Rationale:** Already have context-extraction.md reference, these examples should live there.

#### 4. Lines 246-296: Question Batch Examples (~51 lines)
**Extract to:** `references/design-questions.md` (append to existing)
**Estimated savings:** ~700 tokens

**Rationale:** Concrete question examples belong in reference, not SKILL.md.

### Content to Extract to assets/

#### 5. Lines 325-355: YAML Structure Example (~31 lines)
**Already extracted** to `assets/ui-yaml-template.yaml` but still shown inline.
**Action:** Replace with reference link only.
**Estimated savings:** ~450 tokens

**Before:**
```yaml
window:
  width: 600
  height: 400
  resizable: false
[...28 more lines...]
```

**After:**
```
**Structure:** See `assets/ui-yaml-template.yaml` for complete template.
```

#### 6. Lines 379-402: JavaScript Binding Example (~24 lines)
**Extract to:** `assets/parameter-binding-example.js`
**Estimated savings:** ~350 tokens

**Rationale:** Code examples should be in assets/, not SKILL.md.

### XML Tag Opportunities

#### 7. Lines 15-34: Two-Phase Workflow Enforcement
**Wrap in:** `<critical_sequence>` with phase gate

**Example transformation:**

**BEFORE:**
```markdown
## Workflow Overview

**TWO-PHASE WORKFLOW:**

### Phase A: Design Iteration (Fast)
Generate 2 design files for rapid iteration:
[...]

**STOP HERE** - Present decision menu for user to iterate or finalize.

### Phase B: Implementation Scaffolding (After Finalization)
Generate 5 implementation files only after user approves design:
[...]
```

**AFTER:**
```markdown
## Workflow Overview

<critical_sequence phases="A,B" enforcement="gate">
<phase id="A" name="Design Iteration">
**Purpose:** Generate 2 design files for rapid iteration.

**Outputs:**
1. v[N]-ui.yaml - Machine-readable design specification
2. v[N]-ui-test.html - Browser-testable mockup

<decision_gate id="design_approval" required="true">
**STOP:** Do NOT proceed to Phase B until user approves design via Phase 5.5 decision menu.

**Gate criteria:**
- User selected option 3 or 5 from Phase 5.5 menu ("Finalize...")
- Design validated against WebView constraints
</decision_gate>
</phase>

<phase id="B" name="Implementation Scaffolding" requires_gate="design_approval">
**Purpose:** Generate 5 implementation files ONLY after Phase A approval.

**Outputs:**
3. v[N]-ui.html - Production HTML
4. v[N]-PluginEditor.h - C++ header boilerplate
5. v[N]-PluginEditor.cpp - C++ implementation
6. v[N]-CMakeLists.txt - Build configuration
7. v[N]-integration-checklist.md - Integration steps
</phase>
</critical_sequence>
```

#### 8. Lines 376-400: WebView Validation Gate
**Wrap in:** `<requirement>` with validation checklist

**Example transformation:**

**BEFORE:**
```markdown
## Phase 5.3: Validate WebView Constraints (Before Decision Menu)

**CRITICAL:** Validate generated HTML against WebView constraints before presenting to user.

**Validation checklist:**
[checklist items]

**If validation fails:**
- ❌ REJECT: Regenerate mockup with corrections
- Do NOT present to user until constraints are satisfied
```

**AFTER:**
```markdown
## Phase 5.3: Validate WebView Constraints (Before Decision Menu)

<requirement type="validation" blocking="true">
<validation_checklist>
Validate generated HTML against WebView constraints before presenting to user.

**Must pass ALL checks:**
- [ ] No forbidden CSS viewport units (`100vh`, `100vw`, `100dvh`, `100svh`)
- [ ] Required `html, body { height: 100%; }` present
- [ ] Native feel CSS present (`user-select: none`)
- [ ] Context menu disabled in JavaScript

**Failure handling:**
IF any check fails THEN:
  1. Log specific violation
  2. Regenerate mockup with corrections
  3. Re-run validation
  4. Do NOT proceed to Phase 5.5 until ALL checks pass
</validation_checklist>
</requirement>
```

### Instruction Clarity Improvements

#### 9. Line 108: Ambiguous Conditional
**Issue:** "CRITICAL: Always read creative-brief.md before starting." but preconditions say "None (can work standalone)".

**Fix:**
```markdown
**Read creative-brief.md if it exists:**
```bash
if [ -f "plugins/$PLUGIN_NAME/.ideas/creative-brief.md" ]; then
    # Extract context (see references/context-extraction.md)
else
    # Skip to Phase 1.5 with generic prompt
fi
```

#### 10. Line 299: Decision Gate Options Not Numbered Consistently
**Issue:** AskUserQuestion example uses labels, but system protocol requires inline numbered menus.

**Fix:** Add note that this is internal routing, final presentation is inline numbered.

### Verbosity Reductions

#### 11. Lines 899-926: Versioning Strategy Section
**Current:** 28 lines explaining version numbering
**Target:** 12 lines with reference link

**Condensed version:**
```markdown
## Versioning Strategy

**Pattern:** v1, v2, v3... Each UI version is saved separately.

**Purpose:**
- Explore different layouts without losing previous work
- A/B test designs before committing
- Keep design history for rollback

**File naming:** All 7 files prefixed with version (e.g., `v2-ui.html`, `v2-PluginEditor.h`)

**Implementation:** Latest version used for Stage 5 unless user specifies different version.

**See:** `references/versioning.md` for file management details.
```

### Critical Sequence Enforcement

#### 12. Lines 832-875: Phase 10.5 Finalization Commit
**Wrap in:** `<state_requirement>` to ensure workflow state is updated

**Example:**
```markdown
<state_requirement>
<commit_protocol phase="finalization">
**MUST commit all implementation files and update workflow state.**

**Files to commit:**
- All 7 mockup files (v[N]-*.{html,yaml,h,cpp,txt,md})
- parameter-spec.md (if v1 only)
- .continue-here.md (workflow state)

**State updates required:**
```bash
# Update .continue-here.md
sed -i '' "s/mockup_finalized: .*/mockup_finalized: true/" .continue-here.md
sed -i '' "s/finalized_version: .*/finalized_version: [N]/" .continue-here.md
sed -i '' "s/stage_0_status: .*/stage_0_status: ui_design_complete/" .continue-here.md
```

**Verification:**
- [ ] Git commit succeeded
- [ ] .continue-here.md updated
- [ ] mockup_finalized: true
- [ ] stage_0_status: ui_design_complete

ONLY proceed to "After Completing All Phases" menu if verification passes.
</commit_protocol>
</state_requirement>
```

## Examples of Issues Found

### Example 1: Phase Gate Bypassing

**CURRENT BEHAVIOR (Line 457-510):**
```markdown
## ⚠️ CRITICAL STOP POINT - Phase 5.5: Design Decision Menu

**DO NOT PROCEED TO PHASE 6 WITHOUT USER CONFIRMATION**

After generating YAML + test HTML, present this decision menu:
[menu options]

**WAIT for user response before continuing.**
```

**PROBLEM:** Claude might interpret "present menu" as showing it once, then auto-proceeding if user doesn't respond immediately. No structural gate.

**IMPROVED VERSION:**
```markdown
<decision_gate id="phase_5_5_approval" blocking="true">
## Phase 5.5: Design Decision Menu

**Gate enforcement:** Phases 6-10 are CONDITIONALLY EXECUTED based on user choice.

<menu_presentation>
Present this decision menu:

✓ Mockup v[N] design created (2 files)
[...menu options...]

Choose (1-8): _
</menu_presentation>

<conditional_execution requires="user_choice">
**Execution routing:**
- IF user chose option 3 or 5 (finalize) THEN proceed to Phase 6
- IF user chose option 2 (refine) THEN return to Phase 2 with v[N+1]
- IF user chose option 1 (check alignment) THEN invoke design-sync skill
- ELSE handle custom options

**DO NOT proceed to Phase 6 unless user explicitly chose finalization option.**
</conditional_execution>
</decision_gate>
```

**IMPACT:** Prevents Claude from generating unnecessary implementation files prematurely.

### Example 2: Validation Not Enforced

**CURRENT BEHAVIOR (Line 382-393):**
```markdown
**Validation checklist:**

```bash
# Check for forbidden CSS viewport units
! grep -q "100vh\|100vw\|100dvh\|100svh" v[N]-ui-test.html

# Check for required html/body height
grep -q "html, body.*height: 100%" v[N]-ui-test.html
```

**If validation fails:**
- ❌ REJECT: Regenerate mockup with corrections
- Do NOT present to user until constraints are satisfied
```

**PROBLEM:** No actual execution of validation. Claude generates HTML then skips to decision menu without running checks.

**IMPROVED VERSION:**
```markdown
<validation_gate id="webview_constraints" required="true">
**Execute validation before Phase 5.5:**

```bash
# Store validation results
VALIDATION_PASSED=true

# Check 1: No viewport units
if grep -q "100vh\|100vw\|100dvh\|100svh" v[N]-ui-test.html; then
    echo "❌ FAIL: Forbidden viewport units found"
    VALIDATION_PASSED=false
fi

# Check 2: Required html/body height
if ! grep -q "html, body.*height: 100%" v[N]-ui-test.html; then
    echo "❌ FAIL: Missing required html/body height: 100%"
    VALIDATION_PASSED=false
fi

# Check 3: Native feel CSS
if ! grep -q "user-select: none" v[N]-ui-test.html; then
    echo "❌ FAIL: Missing user-select: none"
    VALIDATION_PASSED=false
fi

# Check 4: Context menu disabled
if ! grep -q 'contextmenu.*preventDefault' v[N]-ui-test.html; then
    echo "❌ FAIL: Context menu not disabled"
    VALIDATION_PASSED=false
fi

# Gate decision
if [ "$VALIDATION_PASSED" = false ]; then
    echo "Regenerating mockup with corrections..."
    # Return to Phase 4 with fixes
else
    echo "✅ All WebView constraints validated"
    # Proceed to Phase 5.5
fi
```
</validation_gate>
```

**IMPACT:** Ensures mockups meet WebView requirements before user sees them.

### Example 3: Context Bloat from Inline Examples

**CURRENT (Lines 200-238):**
```markdown
**Example question batch (via AskUserQuestion):**

```
Question 1:
  question: "Layout structure for the three knobs?"
  header: "Layout"
  options:
    - label: "Horizontal row of knobs", description: "Classic single-row layout"
    - label: "Vertical stack", description: "Narrow, tall layout"
    - label: "Triangle arrangement", description: "Two bottom, one top"
    - label: "Other", description: "Custom arrangement"

Question 2:
[...37 more lines of example questions...]
```
```

**Token cost:** ~700 tokens for examples

**IMPROVED:**
```markdown
**Question batch generation:**

Generate 4 questions using AskUserQuestion based on identified gaps.

**Question structure pattern:**
- question: Clear, specific question about the gap
- header: Short category label (max 12 chars)
- options: 2-4 distinct choices + "Other"

**See:** `references/design-questions.md` for question templates and examples.

**Tier priority:**
1. Critical gaps (layout, control types) - ask first
2. Visual gaps (style, key elements) - ask second
3. Polish gaps (colors, animations) - ask if needed
```

**Token savings:** ~650 tokens (examples moved to reference)

## Estimated Impact

### Context Reduction
- **Extraction to references/:** 3,000 tokens (9%)
- **Extraction to assets/:** 800 tokens (2.5%)
- **Verbosity reduction:** 400 tokens (1.2%)
- **Total reduction:** ~4,200 tokens (13% of current 987-line SKILL.md)

### Comprehension Improvement
**High** - XML enforcement tags will significantly improve Claude's ability to:
1. Stop at Phase 5.5 decision menu (prevents premature Phase B execution)
2. Validate WebView constraints before presentation (prevents invalid mockups)
3. Follow two-phase workflow without deviation (design first, implement second)

### Maintenance Improvement
**Medium-High** - Changes to:
- C++ boilerplate generation → Edit `references/cpp-boilerplate-generation.md` only
- Question templates → Edit `references/design-questions.md` only
- WebView validation rules → Edit validation gate in SKILL.md (centralized)
- Code examples → Edit `assets/` files only

## Priority Recommendations

### 1. Add XML Enforcement for Phase A/B Split (CRITICAL)
**Why:** Core workflow optimization depends on stopping at Phase 5.5. Without structural enforcement, Claude may generate all 7 files prematurely, wasting user time on unwanted C++ boilerplate.

**Action:**
- Wrap lines 15-34 (workflow overview) in `<critical_sequence>` tags
- Wrap Phase 5.5 (lines 457-510) in `<decision_gate>` with blocking=true
- Wrap Phases 6-10 in `<conditional_execution requires="design_approval">`

**Estimated effort:** 30 minutes

**Impact:** Prevents most common workflow deviation (skipping iteration phase)

### 2. Extract C++ Generation Logic to References (HIGH VALUE)
**Why:** Largest token reduction opportunity (1,800 tokens) with minimal comprehension loss. C++ template replacement logic is implementation detail, not workflow logic.

**Action:**
- Create `references/cpp-boilerplate-generation.md`
- Move lines 581-664 (PluginEditor.h/cpp generation)
- Move lines 699-738 (CMake generation)
- Replace with reference links

**Estimated effort:** 20 minutes

**Impact:** 13% context reduction, easier maintenance of C++ templates

### 3. Add Validation Gate with Executable Checks (CRITICAL)
**Why:** WebView constraint violations cause blank screens. Current "CRITICAL" warning is prose-only. Need structural gate with actual bash execution.

**Action:**
- Wrap Phase 5.3 (lines 376-400) in `<validation_gate>` tags
- Add executable bash validation script
- Add failure handling (regenerate with corrections)
- Block Phase 5.5 until all checks pass

**Estimated effort:** 25 minutes

**Impact:** Eliminates invalid mockup generation, improves first-time success rate

## Implementation Notes

### Files that would be created in references/
1. `references/cpp-boilerplate-generation.md` - C++ PluginEditor generation rules (new)
2. `references/cmake-generation.md` - CMake snippet generation (new)
3. `references/versioning.md` - Version management details (new)
4. Append to existing `references/context-extraction.md` - Context extraction examples
5. Append to existing `references/design-questions.md` - Question batch examples

### Files that would be created in assets/
1. `assets/parameter-binding-example.js` - JavaScript binding example (new)

### Changes to SKILL.md structure
1. Add `<critical_sequence>` wrapper around workflow overview (lines 15-34)
2. Add `<decision_gate>` wrapper around Phase 5.5 (lines 457-510)
3. Add `<validation_gate>` wrapper around Phase 5.3 (lines 376-400)
4. Add `<state_requirement>` wrapper around Phase 10.5 (lines 832-875)
5. Replace inline examples with reference links (9 locations)
6. Condense versioning section (lines 899-926)

### Estimated time to implement all fixes
**Total:** ~2.5 hours
- XML enforcement tags: 1 hour
- Content extraction: 1 hour
- Validation gate implementation: 30 minutes

### Recommended implementation order
1. **Phase 1** (30 min): Add XML enforcement tags (highest impact, prevents workflow deviation)
2. **Phase 2** (30 min): Add validation gate (prevents invalid mockups)
3. **Phase 3** (60 min): Extract C++ generation to references (largest token savings)
4. **Phase 4** (30 min): Extract examples and condense verbosity (polish)

### Testing verification after changes
1. Test two-phase workflow: Confirm Claude stops at Phase 5.5 decision menu
2. Test validation: Generate mockup with intentional `100vh` violation, verify rejection
3. Test context reduction: Measure token usage before/after extraction
4. Test maintenance: Update C++ template in reference, verify skill still works
