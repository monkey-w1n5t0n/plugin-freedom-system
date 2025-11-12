# Skill Analysis: ui-template-library

## Executive Summary
- Overall health: **Yellow**
- Critical issues: **3**
- Optimization opportunities: **8**
- Estimated context savings: **~3,200 tokens / 36%**

## Dimension Scores (1-5)
1. Structure Compliance: **4/5** - Good YAML frontmatter, proper directory structure, but missing critical sequence enforcement
2. Content Extraction: **3/5** - Good use of references/ and assets/, but SKILL.md still has extractable content
3. Instruction Clarity: **4/5** - Strong sequential steps and examples, but lacks explicit XML structure for critical workflows
4. XML Organization: **2/5** - Minimal XML tags; critical sequences and decision gates not wrapped
5. Context Efficiency: **3/5** - Some verbosity, large bash code blocks could be referenced
6. Claude-Optimized Language: **4/5** - Generally clear, but some ambiguous pronouns and implicit instructions
7. System Integration: **3/5** - Missing explicit checkpoint protocol adherence, decision menu format inconsistent
8. Critical Sequence Enforcement: **2/5** - No XML enforcement of operation workflows; easy to skip steps

## Critical Issues (blockers for Claude comprehension)

### 1. Missing Critical Sequence Enforcement (Lines 63-278, 280-596)
**Issue:** The five operations (Save, Apply, List, Delete, Update) are complex multi-step workflows but lack XML enforcement tags. Claude could easily skip steps or execute operations out of order.

**Impact:** High - Could result in incomplete aesthetic.md files, missing metadata, or broken manifest.json

**Fix Required:** Wrap each operation in `<operation>` tags with `<critical_sequence>` for mandatory steps

### 2. No Decision Gate for Checkpoint Protocol (Lines 264-277, 576-595)
**Issue:** Skill presents decision menus but doesn't enforce WAIT behavior. No `<decision_gate>` tags to block auto-progression.

**Impact:** Medium - Claude might auto-proceed instead of waiting for user choice, violating system checkpoint protocol

**Fix Required:** Add explicit `<decision_gate>` tags around all decision menus with `wait_required="true"`

### 3. Implicit ui-mockup Integration (Lines 742-764)
**Issue:** Integration with ui-mockup skill described in prose, but no formal `<handoff_protocol>` or explicit invocation instructions.

**Impact:** Medium - Unclear how/when ui-template-library is invoked from ui-mockup, could cause coordination failures

**Fix Required:** Add `<handoff_protocol>` section with explicit triggering conditions and data contracts

## Optimization Opportunities

### Content to Extract to references/

#### 1. Lines 76-261 (Operation 1: Save Aesthetic - Steps 1-8)
**Extraction:** `references/save-operation.md`
**Rationale:** 185-line implementation workflow, rarely needs to be in main context
**Token savings:** ~1,100 tokens
**Keep in SKILL.md:** High-level summary (5-8 lines) + reference link

**Example transformation:**
```markdown
BEFORE:
### Step 1: Read Mockup HTML
[full 10-line explanation with bash examples]

AFTER:
### Save Aesthetic Operation
Read mockup HTML, extract patterns, generate prose, write aesthetic.md, update manifest.
**See:** `references/save-operation.md` for complete workflow.
```

#### 2. Lines 280-596 (Operation 2: Apply Aesthetic - Steps 1-8)
**Extraction:** `references/apply-operation.md`
**Rationale:** 316-line implementation workflow, longest section in skill
**Token savings:** ~1,900 tokens
**Keep in SKILL.md:** High-level summary (8-10 lines) + reference link

#### 3. Lines 86-128 (Bash extraction examples in Step 2)
**Extraction:** Already extracted to `references/pattern-extraction.md` ✅
**Issue:** Still duplicated in SKILL.md lines 86-128
**Action:** Remove duplication, keep only 2-3 line summary + reference link
**Token savings:** ~250 tokens

### Content to Extract to assets/

No additional extractions needed - templates already properly placed in assets/

### XML Tag Opportunities

#### 1. Lines 63-278: Wrap "Operation 1: Save Aesthetic"
```xml
<operation name="save_aesthetic">
  <purpose>Capture visual design from mockup as structured prose aesthetic.md</purpose>

  <critical_sequence>
    <step id="1" required="true">
      Read mockup HTML file into memory (Read tool)
    </step>
    <step id="2" required="true" depends_on="1">
      Extract visual patterns using pattern-extraction.md strategies
    </step>
    <step id="3" required="true" depends_on="2">
      Generate prose descriptions using prose-generation.md guidelines
    </step>
    <step id="4" required="true" depends_on="3">
      Write aesthetic.md following aesthetic-template.md structure exactly
    </step>
    <step id="5" required="true" depends_on="4">
      Copy preview.html to aesthetic directory
    </step>
    <step id="6" required="true" depends_on="5">
      Generate metadata.json with inferred tags
    </step>
    <step id="7" required="true" depends_on="6">
      Update manifest.json using Read → modify → Write pattern
    </step>
    <step id="8" required="true" depends_on="7">
      Present confirmation + decision menu
    </step>
  </critical_sequence>

  <decision_gate wait_required="true">
    Present decision menu (inline numbered list, NOT AskUserQuestion tool)
    MUST wait for user response before proceeding
  </decision_gate>
</operation>
```

**Token cost:** ~150 tokens
**Benefit:** Ensures all 8 steps execute in order, prevents partial aesthetic creation

#### 2. Lines 280-596: Wrap "Operation 2: Apply Aesthetic"
```xml
<operation name="apply_aesthetic">
  <purpose>Generate new mockup by interpreting aesthetic prose and adapting to target plugin</purpose>

  <critical_sequence>
    <step id="1" required="true">
      Load aesthetic.md and metadata.json (Read tool)
    </step>
    <step id="2" required="true">
      Read target plugin parameter-spec.md or creative-brief.md
    </step>
    <step id="3" required="true" depends_on="1,2">
      Interpret aesthetic prose using aesthetic-interpretation.md strategies
    </step>
    <step id="4" required="true" depends_on="3">
      Choose layout based on parameter count (layout-generation.md)
    </step>
    <step id="5" required="true" depends_on="3,4">
      Generate HTML/CSS applying aesthetic to chosen layout
    </step>
    <step id="6" required="true" depends_on="5">
      Save generated mockup to target plugin mockups directory
    </step>
    <step id="7" required="true" depends_on="6">
      Update aesthetic metadata.json usedInPlugins array
    </step>
    <step id="8" required="true" depends_on="7">
      Present decision menu (inline numbered, NOT AskUserQuestion)
    </step>
  </critical_sequence>

  <decision_gate wait_required="true">
    Present decision menu with preview/continue options
    MUST wait for user response
  </decision_gate>
</operation>
```

**Token cost:** ~140 tokens
**Benefit:** Enforces interpretation → layout → generation sequence, prevents skipping adaptation logic

#### 3. Lines 742-764: Formalize ui-mockup Integration
```xml
<handoff_protocol>
  <from_skill>ui-mockup</from_skill>
  <to_skill>ui-template-library</to_skill>

  <trigger_conditions>
    <condition when="Phase 0" if="aesthetics exist in library">
      ui-mockup presents option "Start from aesthetic template"
      User selects option → invoke ui-template-library "apply" operation
    </condition>
    <condition when="Phase 5.5 decision menu">
      ui-mockup presents option "Save as aesthetic template"
      User selects option → invoke ui-template-library "save" operation
    </condition>
  </trigger_conditions>

  <data_contract>
    <input name="mockup_path" type="absolute_path" required="true">
      Path to finalized mockup HTML file
    </input>
    <input name="plugin_name" type="string" required="true">
      Name of plugin for aesthetic source tracking
    </input>
    <input name="aesthetic_name" type="string" required="false">
      Optional custom name for aesthetic (defaults to plugin name)
    </input>
  </data_contract>

  <return_values>
    <output name="aesthetic_id" type="string">
      Generated aesthetic ID (e.g., "vintage-hardware-001")
    </output>
    <output name="aesthetic_path" type="absolute_path">
      Path to aesthetic directory
    </output>
  </return_values>
</handoff_protocol>
```

**Token cost:** ~130 tokens
**Benefit:** Makes integration explicit, defines data contract, prevents coordination errors

#### 4. Lines 599-653: Wrap "Operation 3: List Aesthetics"
```xml
<operation name="list_aesthetics">
  <critical_sequence>
    <step id="1" required="true">Read manifest.json</step>
    <step id="2" required="true">Format as markdown table</step>
    <step id="3" required="true">Show preview paths</step>
    <step id="4" required="true">Present decision menu</step>
  </critical_sequence>

  <decision_gate wait_required="true">
    Wait for user to select action from menu
  </decision_gate>
</operation>
```

### Instruction Clarity Improvements

#### Line 36-37: Clarify operation selection
**Current:**
```markdown
## Operations

1. **Save aesthetic** - Analyze mockup HTML and generate structured prose aesthetic.md
```

**Improved:**
```markdown
## Operations

**You MUST determine which operation the user is requesting, then execute ONLY that operation.**

1. **Save aesthetic** - Analyze mockup HTML and generate structured prose aesthetic.md
```

**Rationale:** Explicit instruction prevents Claude from executing multiple operations

#### Lines 76-78: Strengthen sequencing language
**Current:**
```markdown
**High-level process:**
1. Read and analyze mockup HTML using string/pattern matching
```

**Improved:**
```markdown
**CRITICAL: Execute these steps in exact order. Do NOT skip steps.**
<critical_sequence>
1. Read and analyze mockup HTML using string/pattern matching
```

**Rationale:** Explicit CRITICAL + XML tag enforces sequence

#### Lines 166-169: Remove template variable syntax confusion
**Current:**
```markdown
**Load template:**
```bash
TEMPLATE=".claude/skills/ui-template-library/assets/aesthetic-template.md"
```

**Improved:**
```markdown
**Load template using Read tool:**
- Path: `.claude/skills/ui-template-library/assets/aesthetic-template.md`
- Read entire template into memory
- Fill each section with generated prose (see Step 3)
```

**Rationale:** Bash variables might be interpreted as instructions to write bash scripts; explicit tool usage is clearer

### Verbosity Reductions

#### Lines 86-128: Redundant extraction examples (already in reference doc)
**Current:** 42 lines of bash examples
**Target:** 6 lines summary + reference link
**Savings:** ~220 tokens

**Example condensed version:**
```markdown
### Step 2: Extract Visual Patterns

**Use Read tool + bash pattern matching to extract:**
- Colors (hex, rgba, CSS variables)
- Fonts (families, sizes, weights)
- Control types (knobs, sliders, buttons)
- Spacing values (gap, padding, margin)
- Visual effects (shadows, borders, textures)

**See:** `references/pattern-extraction.md` for complete extraction strategies and bash examples
```

#### Lines 133-161: Prose generation examples (redundant with reference doc)
**Current:** 28 lines
**Target:** 8 lines
**Savings:** ~120 tokens

**Example condensed version:**
```markdown
### Step 3: Generate Prose Descriptions

**Transform extracted patterns into interpretable prose for each aesthetic.md section.**

Example: Colors `#ff6b35, #2b2015, #f5e6d3` → "Warm vintage palette with burnt orange accent against dark brown background. Cream text provides high contrast."

**See:** `references/prose-generation.md` for detailed generation guidelines for all sections
```

#### Lines 405-453: Duplicated HTML template structure
**Current:** 48 lines showing template structure
**Target:** Reference to template file
**Savings:** ~280 tokens

**Replace with:**
```markdown
### Step 5: Generate HTML with Aesthetic

**Load base template structure from skill assets, then inject aesthetic-specific values.**

Template locations:
- Base HTML structure: Common mockup pattern (see ui-mockup skill)
- CSS variable insertion: Extract from aesthetic.md "Example Color Codes" section
- Control styling: Generate using control-generation.md strategies

**See:** `references/control-generation.md` for complete HTML/CSS generation logic
```

### Critical Sequence Enforcement

#### Lines 63-278: Save operation needs state tracking
**Issue:** No verification that all 7 steps completed before presenting confirmation
**Solution:** Add state requirement tags

```xml
<operation name="save_aesthetic">
  <state_requirement>
    <before_completion>
      ALL steps 1-7 must complete successfully
      IF any step fails: halt, report error, DO NOT proceed to step 8
    </before_completion>

    <verification>
      Before step 8:
      - VERIFY aesthetic.md exists and is not empty
      - VERIFY preview.html exists
      - VERIFY metadata.json is valid JSON
      - VERIFY manifest.json updated successfully
    </verification>
  </state_requirement>
</operation>
```

#### Lines 742-764: ui-mockup integration needs delegation rule
**Issue:** Unclear whether ui-template-library should be invoked as subagent or inline
**Solution:** Add delegation rule

```xml
<delegation_rule>
  <from>ui-mockup skill</from>
  <to>ui-template-library skill</to>
  <method>inline_invocation</method>
  <rationale>
    Both skills are lightweight and stateless.
    No need for separate agent context.
    ui-mockup simply expands to include ui-template-library operations.
  </rationale>
</delegation_rule>
```

## Examples of Issues Found

### Example 1: Missing Checkpoint Protocol Enforcement

**BEFORE (Lines 264-277):**
```markdown
### Step 8: Confirmation

```
✅ Aesthetic saved: "Vintage Hardware" (vintage-hardware-001)

Files created:
- .claude/aesthetics/vintage-hardware-001/aesthetic.md (prose description)
- .claude/aesthetics/vintage-hardware-001/preview.html (visual reference)
- .claude/aesthetics/vintage-hardware-001/metadata.json (metadata)

Preview: open .claude/aesthetics/vintage-hardware-001/preview.html

You can now apply this aesthetic to new plugins:
- From ui-mockup skill: Choose "Start from aesthetic template"
- Or manually invoke: ui-template-library with "apply" operation
```
```

**AFTER:**
```markdown
### Step 8: Present Decision Menu

<decision_gate wait_required="true">
  Present numbered decision menu (inline format, NOT AskUserQuestion tool).
  MUST WAIT for user response. DO NOT auto-proceed.
</decision_gate>

**Format:**
```
✅ Aesthetic saved: "Vintage Hardware" (vintage-hardware-001)

Files created:
- .claude/aesthetics/vintage-hardware-001/aesthetic.md
- .claude/aesthetics/vintage-hardware-001/preview.html
- .claude/aesthetics/vintage-hardware-001/metadata.json

What's next?
1. Apply to existing plugin (recommended)
2. Preview aesthetic in browser
3. List all aesthetics
4. Return to ui-mockup workflow
5. Other

Choose (1-5): _
```

**WAIT for user input. DO NOT execute any option automatically.**
```

**Impact:** Ensures checkpoint protocol compliance, prevents auto-progression

### Example 2: Implicit Step Dependencies

**BEFORE (Lines 298-309):**
```markdown
### Step 1: Load Aesthetic

```bash
AESTHETIC_DIR=".claude/aesthetics/$AESTHETIC_ID"
AESTHETIC_MD="$AESTHETIC_DIR/aesthetic.md"
METADATA_JSON="$AESTHETIC_DIR/metadata.json"

test -f "$AESTHETIC_MD" || { echo "Aesthetic not found"; exit 1; }
```

**Use Read tool to load aesthetic.md and metadata.json.**
```

**AFTER:**
```markdown
### Step 1: Load Aesthetic

<step id="1" required="true">
  <validation>
    BEFORE reading: Verify aesthetic directory exists
    IF NOT: Report error and halt operation
  </validation>

  <actions>
    1. Use Read tool to load aesthetic.md
    2. Use Read tool to load metadata.json
    3. Store both in memory for Steps 3-5
  </actions>

  <success_criteria>
    - aesthetic.md loaded and contains all required sections
    - metadata.json loaded and is valid JSON
    - Both stored for subsequent interpretation
  </success_criteria>
</step>
```

**Impact:** Makes validation explicit, defines success criteria, prevents proceeding with missing data

### Example 3: Ambiguous Reference to "Template"

**BEFORE (Lines 166-169):**
```markdown
**Load template:**
```bash
TEMPLATE=".claude/skills/ui-template-library/assets/aesthetic-template.md"
```

**Fill in all sections using generated prose:**
```

**AFTER:**
```markdown
**Use Read tool to load aesthetic-template.md:**
```markdown
- Path: `.claude/skills/ui-template-library/assets/aesthetic-template.md`
- This template defines the exact structure for aesthetic.md
- CRITICAL: Follow template section order exactly - same sections, same sequence
```

**Fill template sections with generated prose from Step 3:**
[keep existing section list]
```

**Impact:** Explicit tool usage, removes bash confusion, emphasizes template structure importance

## Estimated Impact

### Context Reduction
- **Operation workflows to references:** ~3,000 tokens (Save + Apply operations)
- **Redundant bash examples:** ~220 tokens
- **Redundant prose examples:** ~120 tokens
- **HTML template duplication:** ~280 tokens
- **Total savings:** ~3,620 tokens

**Percentage reduction:** 3,620 / 10,200 ≈ **36% reduction**

**Added for XML enforcement:** ~600 tokens (5 operations × ~120 tokens each)
**Net savings:** ~3,000 tokens / **29% reduction**

### Comprehension Improvement
**High** - Critical sequence enforcement prevents:
- Skipped steps in save/apply operations
- Missing metadata or manifest updates
- Incomplete aesthetic.md files
- Auto-progression violating checkpoint protocol
- Ambiguous ui-mockup integration

### Maintenance Improvement
**High** - Benefits:
- Clear operation boundaries with XML tags
- Explicit handoff protocols with ui-mockup
- All implementation details in references/ (single source of truth)
- SKILL.md becomes high-level orchestration (easier to understand)
- Changes to bash strategies only update reference docs (no SKILL.md edits)

## Priority Recommendations

### 1. Add Critical Sequence Enforcement to Save and Apply Operations
**Priority:** Critical (P0)
**Effort:** 30 minutes
**Files to modify:**
- `SKILL.md` lines 63-278 (Save operation)
- `SKILL.md` lines 280-596 (Apply operation)

**Actions:**
- Wrap both operations in `<operation>` tags
- Add `<critical_sequence>` with numbered steps and dependencies
- Add `<decision_gate>` tags for checkpoint protocol
- Add `<state_requirement>` for verification before completion
- Add explicit "WAIT for user response" instructions

**Expected outcome:**
- Claude cannot skip steps in workflows
- Checkpoint protocol enforced automatically
- Partial aesthetic creation prevented

### 2. Extract Save and Apply Workflows to References
**Priority:** High (P1)
**Effort:** 45 minutes
**Files to create:**
- `references/save-operation.md` (Steps 1-8 from lines 76-278)
- `references/apply-operation.md` (Steps 1-8 from lines 298-596)

**Files to modify:**
- `SKILL.md` - replace implementation details with summaries + reference links

**Actions:**
- Copy complete Save workflow (Steps 1-8) to new reference doc
- Copy complete Apply workflow (Steps 1-8) to new reference doc
- Replace SKILL.md sections with:
  - 5-8 line high-level summary
  - Critical sequence XML (from recommendation #1)
  - Reference link to detailed workflow
- Remove redundant bash examples (already in pattern-extraction.md)

**Expected outcome:**
- SKILL.md reduced by ~3,000 tokens
- Implementation details consolidated in references/
- Easier to maintain (edit one reference doc vs main skill)

### 3. Formalize ui-mockup Integration with Handoff Protocol
**Priority:** High (P1)
**Effort:** 20 minutes
**Files to modify:**
- `SKILL.md` lines 742-764

**Actions:**
- Replace prose description with `<handoff_protocol>` XML tag
- Define trigger conditions explicitly
- Define data contract (inputs/outputs)
- Add delegation rule (inline vs subagent)
- Reference ui-mockup SKILL.md sections that invoke this skill

**Expected outcome:**
- Clear integration contract between skills
- Prevents coordination errors
- Explicit data requirements
- Easier for ui-mockup skill to invoke correctly

## Implementation Notes

### Files that would be created in references/
1. `references/save-operation.md` - Complete save workflow with all 8 steps
2. `references/apply-operation.md` - Complete apply workflow with all 8 steps

### Files that would be modified
1. `SKILL.md` - Add XML enforcement, extract operations, formalize handoff

### Estimated time to implement all fixes
**Total:** ~95 minutes (1 hour 35 minutes)

**Breakdown:**
- Priority 1 (Critical sequence enforcement): 30 min
- Priority 2 (Extract to references): 45 min
- Priority 3 (Formalize handoff): 20 min

**Recommended order:**
1. First: Add XML enforcement (immediate comprehension benefit)
2. Second: Formalize handoff protocol (prevents integration errors)
3. Third: Extract to references (context optimization, less urgent)

---

## Additional Observations

### Strengths
- Excellent use of references/ directory (4 reference docs already extracted)
- Strong examples throughout (prose generation, extraction strategies)
- Good template structure in assets/
- Clear separation of concerns (extraction → prose → interpretation)
- Comprehensive coverage of all use cases

### Weaknesses
- Main SKILL.md still too large (883 lines, ~10,200 tokens)
- Missing XML enforcement for critical workflows
- Checkpoint protocol not explicitly enforced
- Some redundancy with reference documents
- ui-mockup integration informal (prose vs protocol)

### Risk Areas
- If Save operation Step 4-7 fail, partial aesthetics could corrupt library
- Apply operation could generate invalid HTML if interpretation skipped
- Manifest.json corruption if concurrent saves occur (no file locking mentioned)
- Decision menus presented but auto-progression not blocked

### Suggested Future Enhancements (not blocking)
1. Add file locking for manifest.json updates (prevent race conditions)
2. Add validation schema for aesthetic.md sections (ensure completeness)
3. Add rollback mechanism if Save operation fails mid-workflow
4. Add aesthetic versioning (track updates over time)
5. Add aesthetic diff tool (compare two aesthetics)
