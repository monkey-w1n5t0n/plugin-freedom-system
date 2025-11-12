# Fix Plan: ui-template-library

## Summary
- Critical fixes: **7**
- Extraction operations: **2**
- Total estimated changes: **15**
- Estimated time: **95 minutes**

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Add Critical Sequence Enforcement to Save Operation
**Location:** Lines 63-278
**Operation:** WRAP
**Before:**
```markdown
## Operation 1: Save Aesthetic

**Purpose:** Analyze completed mockup HTML and generate structured prose aesthetic.md that describes the visual design.
```
**After:**
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

## Operation 1: Save Aesthetic

**Purpose:** Analyze completed mockup HTML and generate structured prose aesthetic.md that describes the visual design.
```
**Verification:**
- Confirm `<operation name="save_aesthetic">` tag present at line 63
- Confirm `<critical_sequence>` contains 8 steps with dependencies
- Confirm `<state_requirement>` present with verification checklist

### Fix 1.2: Add Decision Gate to Save Operation Confirmation
**Location:** Lines 264-277
**Operation:** REPLACE
**Before:**
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
**After:**
```markdown
### Step 8: Present Decision Menu

<decision_gate wait_required="true">
  Present numbered decision menu (inline format, NOT AskUserQuestion tool).
  MUST WAIT for user response. DO NOT auto-proceed.
</decision_gate>

**Format:**
```
✅ Aesthetic saved: "[NAME]" ([ID])

Files created:
- .claude/aesthetics/[ID]/aesthetic.md
- .claude/aesthetics/[ID]/preview.html
- .claude/aesthetics/[ID]/metadata.json

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
**Verification:**
- Confirm `<decision_gate wait_required="true">` tag present
- Confirm decision menu uses inline numbered format
- Confirm explicit "WAIT" instruction present

### Fix 1.3: Add Critical Sequence Enforcement to Apply Operation
**Location:** Lines 280-596
**Operation:** WRAP
**Before:**
```markdown
## Operation 2: Apply Aesthetic

**Purpose:** Generate a new mockup for a target plugin by interpreting an aesthetic's prose description and adapting it to the plugin's specific requirements.
```
**After:**
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

## Operation 2: Apply Aesthetic

**Purpose:** Generate a new mockup for a target plugin by interpreting an aesthetic's prose description and adapting it to the plugin's specific requirements.
```
**Verification:**
- Confirm `<operation name="apply_aesthetic">` tag present at line 280
- Confirm `<critical_sequence>` contains 8 steps with dependencies
- Confirm `<decision_gate>` present with wait_required="true"

### Fix 1.4: Add Decision Gate to Apply Operation Confirmation
**Location:** Lines 576-595
**Operation:** REPLACE
**Before:**
```markdown
### Step 8: Confirmation

```
✅ Mockup generated: plugins/Dreamscape/mockups/mockup-dreamscape.html

Applied aesthetic: "Vintage Hardware" (vintage-hardware-001)
- Adapted color scheme to plugin parameters
- Generated [X] parameter controls ([Y] knobs, [Z] sliders)
- Applied layout pattern: [pattern name]

Next steps:
- Preview mockup in browser
- If approved: Continue to ui-mockup Phase 2 (detailed design)
- If adjustments needed: Modify aesthetic.md or request different aesthetic
```
```
**After:**
```markdown
### Step 8: Present Decision Menu

<decision_gate wait_required="true">
  Present numbered decision menu (inline format, NOT AskUserQuestion tool).
  MUST WAIT for user response. DO NOT auto-proceed.
</decision_gate>

**Format:**
```
✅ Mockup generated: plugins/[Plugin]/mockups/mockup-[plugin].html

Applied aesthetic: "[NAME]" ([ID])

What's next?
1. Preview mockup in browser (recommended)
2. Continue to ui-mockup Phase 2 (detailed design iteration)
3. Apply different aesthetic to same plugin
4. Adjust current aesthetic and re-apply
5. Other

Choose (1-5): _
```

**WAIT for user input. DO NOT execute any option automatically.**
```
**Verification:**
- Confirm `<decision_gate wait_required="true">` tag present
- Confirm decision menu format matches checkpoint protocol

### Fix 1.5: Add Explicit Operation Selection Instruction
**Location:** Lines 36-37
**Operation:** INSERT
**Before:**
```markdown
## Operations

1. **Save aesthetic** - Analyze mockup HTML and generate structured prose aesthetic.md
```
**After:**
```markdown
## Operations

**You MUST determine which operation the user is requesting, then execute ONLY that operation.**

1. **Save aesthetic** - Analyze mockup HTML and generate structured prose aesthetic.md
```
**Verification:** Confirm explicit instruction prevents multi-operation execution

### Fix 1.6: Strengthen Sequencing Language in Save Operation
**Location:** Lines 76-78
**Operation:** REPLACE
**Before:**
```markdown
**High-level process:**
1. Read and analyze mockup HTML using string/pattern matching
```
**After:**
```markdown
**CRITICAL: Execute these steps in exact order. Do NOT skip steps.**

1. Read and analyze mockup HTML using string/pattern matching
```
**Verification:** Confirm "CRITICAL" instruction present before step list

### Fix 1.7: Formalize ui-mockup Integration with Handoff Protocol
**Location:** Lines 742-764
**Operation:** REPLACE
**Before:**
```markdown
## Integration with ui-mockup Skill

This skill integrates with the ui-mockup skill at two points:

**Phase 0 (Template Selection):**
- If aesthetics exist in library, ui-mockup presents option "Start from aesthetic template"
- User selects option → invoke ui-template-library "apply" operation

**Phase 5.5 (Save as Template):**
- After user approves finalized mockup, ui-mockup presents decision menu
- Option: "Save as aesthetic template for future use"
- User selects option → invoke ui-template-library "save" operation
```
**After:**
```xml
## Integration with ui-mockup Skill

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
**Verification:**
- Confirm `<handoff_protocol>` tag present with trigger_conditions
- Confirm `<data_contract>` defines inputs/outputs
- Confirm `<delegation_rule>` specifies inline_invocation

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Extract Save Operation Workflow to References
**Location:** Lines 76-261
**Operation:** EXTRACT
**Create file:** `references/save-operation.md`
**File content:**
```markdown
# Save Aesthetic Operation - Complete Workflow

## Purpose
Analyze completed mockup HTML and generate structured prose aesthetic.md that describes the visual design.

## Step-by-Step Process

### Step 1: Read Mockup HTML

**Use Read tool to load the mockup HTML file:**
```bash
# Example path
MOCKUP_PATH="plugins/LushVerb/mockups/mockup-lushverb.html"
```

**Store the entire HTML content in memory for pattern extraction in Step 2.**

### Step 2: Extract Visual Patterns

**See `references/pattern-extraction.md` for complete extraction strategies.**

Use Read tool + bash pattern matching to extract:
- Colors (hex codes, rgba values, CSS variables)
- Fonts (family names, sizes, weights)
- Control types (knobs, sliders, buttons, switches)
- Spacing values (gap, padding, margin)
- Visual effects (box-shadow, border-radius, textures)

### Step 3: Generate Prose Descriptions

**See `references/prose-generation.md` for detailed generation guidelines.**

Transform extracted patterns into interpretable prose for each aesthetic.md section:
- Color Palette → descriptive prose
- Typography → font descriptions
- Control Design → style descriptions
- Layout Patterns → spatial descriptions
- Visual Effects → appearance descriptions

### Step 4: Write aesthetic.md

**Load template using Read tool:**
- Path: `.claude/skills/ui-template-library/assets/aesthetic-template.md`
- This template defines the exact structure for aesthetic.md
- CRITICAL: Follow template section order exactly - same sections, same sequence

**Fill template sections with generated prose from Step 3:**

1. **Core Identity** - Name, essence, mood
2. **Color Palette** - Prose + example codes
3. **Typography** - Font descriptions
4. **Control Design** - Knobs, sliders, buttons
5. **Layout Patterns** - Grid, spacing, hierarchy
6. **Visual Effects** - Shadows, depth, texture
7. **Interpretation Guidelines** - Adaptation rules

**Write using Write tool:**
```bash
OUTPUT_PATH=".claude/aesthetics/$AESTHETIC_ID/aesthetic.md"
```

### Step 5: Copy Preview HTML

**Copy mockup HTML to aesthetic directory for visual reference:**
```bash
SOURCE="plugins/LushVerb/mockups/mockup-lushverb.html"
DEST=".claude/aesthetics/$AESTHETIC_ID/preview.html"
```

Use Read (source) + Write (destination) pattern.

### Step 6: Generate metadata.json

**Create metadata file with inferred tags:**
```json
{
  "id": "vintage-hardware-001",
  "name": "Vintage Hardware",
  "sourcePlugin": "LushVerb",
  "createdDate": "2025-11-12",
  "tags": ["warm", "vintage", "analog", "hardware"],
  "usedInPlugins": [],
  "version": "1.0.0"
}
```

**Tag inference logic:**
- Color palette → color tags (warm, cool, monochrome)
- Control design → style tags (minimal, detailed, skeuomorphic)
- Visual effects → appearance tags (flat, dimensional, textured)

**Write using Write tool:**
```bash
METADATA_PATH=".claude/aesthetics/$AESTHETIC_ID/metadata.json"
```

### Step 7: Update manifest.json

**CRITICAL: Use Read → modify → Write pattern to prevent corruption:**

1. **Read current manifest:**
```bash
MANIFEST_PATH=".claude/aesthetics/manifest.json"
```

2. **Parse JSON and add new entry:**
```json
{
  "aesthetics": [
    {
      "id": "vintage-hardware-001",
      "name": "Vintage Hardware",
      "path": ".claude/aesthetics/vintage-hardware-001",
      "createdDate": "2025-11-12",
      "tags": ["warm", "vintage", "analog", "hardware"]
    }
  ]
}
```

3. **Write updated manifest using Write tool**

**NEVER append directly - always read full manifest, modify in memory, write complete file.**

### Step 8: Present Decision Menu

(See main SKILL.md for decision gate and menu format)
```

**Replace in SKILL.md with:**
```markdown
## Operation 1: Save Aesthetic

**High-level summary:**
Read mockup HTML, extract visual patterns, generate prose descriptions, write aesthetic.md following template structure, copy preview HTML, generate metadata, update manifest, present decision menu.

**See:** `references/save-operation.md` for complete 8-step workflow.
```

**Verification:**
- File exists at `.claude/skills/ui-template-library/references/save-operation.md`
- SKILL.md reduced by ~185 lines
- Summary + reference link present in SKILL.md

### Fix 2.2: Extract Apply Operation Workflow to References
**Location:** Lines 298-596
**Operation:** EXTRACT
**Create file:** `references/apply-operation.md`
**File content:**
```markdown
# Apply Aesthetic Operation - Complete Workflow

## Purpose
Generate a new mockup for a target plugin by interpreting an aesthetic's prose description and adapting it to the plugin's specific requirements.

## Step-by-Step Process

### Step 1: Load Aesthetic

**Verify aesthetic directory exists, then load files:**

```bash
AESTHETIC_DIR=".claude/aesthetics/$AESTHETIC_ID"
AESTHETIC_MD="$AESTHETIC_DIR/aesthetic.md"
METADATA_JSON="$AESTHETIC_DIR/metadata.json"
```

**Use Read tool to load:**
1. aesthetic.md - Complete prose description
2. metadata.json - Tags and usage tracking

**Store both in memory for Steps 3-5.**

**Validation:**
- BEFORE reading: Verify aesthetic directory exists
- IF NOT: Report error and halt operation

**Success criteria:**
- aesthetic.md loaded and contains all required sections
- metadata.json loaded and is valid JSON
- Both stored for subsequent interpretation

### Step 2: Load Plugin Requirements

**Read target plugin contracts to understand parameter requirements:**

**Try in order:**
1. `plugins/$PLUGIN_NAME/.ideas/parameter-spec.md` (preferred)
2. `plugins/$PLUGIN_NAME/.ideas/creative-brief.md` (fallback)

**Extract critical information:**
- Parameter count (determines layout complexity)
- Parameter types (knobs, sliders, switches, buttons)
- Plugin purpose/category (informs aesthetic interpretation)
- Any explicit UI requirements from creative brief

### Step 3: Interpret Aesthetic Prose

**See `references/aesthetic-interpretation.md` for complete interpretation strategies.**

**Transform prose descriptions into specific values:**

**Color Palette section → CSS variables:**
```css
/* Prose: "Warm vintage palette with burnt orange accent" */
--color-primary: #ff6b35;
--color-background: #2b2015;
--color-text: #f5e6d3;
```

**Typography section → CSS font properties:**
```css
/* Prose: "Industrial sans-serif headings with condensed proportions" */
--font-heading: "Roboto Condensed", sans-serif;
--font-heading-weight: 700;
```

**Control Design section → Component styles:**
```css
/* Prose: "Large knobs with engraved tick marks, worn metal texture" */
.knob {
  width: 80px;
  background: metallic-gradient;
  box-shadow: inset 0 2px 4px rgba(0,0,0,0.3);
}
```

**Key principle:** Aesthetic prose is interpretive, not prescriptive. Adapt values to fit target plugin's parameter count and layout needs.

### Step 4: Choose Layout Pattern

**See `references/layout-generation.md` for layout selection logic.**

**Select layout based on parameter count:**
- 1-6 parameters: Single-row horizontal layout
- 7-12 parameters: Two-row grid layout
- 13-24 parameters: Multi-section grouped layout
- 25+ parameters: Tabbed or scrolling layout

**Apply aesthetic's "Layout Patterns" prose to chosen structure:**
- Spacing values from aesthetic
- Hierarchy principles from aesthetic
- Grouping strategies adapted to plugin parameters

### Step 5: Generate HTML with Aesthetic

**Load base template structure from skill assets, then inject aesthetic-specific values.**

**Template locations:**
- Base HTML structure: Common mockup pattern (see ui-mockup skill)
- CSS variable insertion: Extract from aesthetic.md "Example Color Codes" section
- Control styling: Generate using `references/control-generation.md` strategies

**Complete HTML structure:**
```html
<!DOCTYPE html>
<html>
<head>
  <style>
    :root {
      /* Inject aesthetic color variables */
    }
    /* Inject aesthetic typography styles */
    /* Inject aesthetic control styles */
    /* Inject aesthetic layout patterns */
  </style>
</head>
<body>
  <!-- Generate parameter controls using aesthetic interpretation -->
</body>
</html>
```

**See `references/control-generation.md` for complete HTML/CSS generation logic.**

### Step 6: Save Generated Mockup

**Write mockup to target plugin's mockups directory:**

```bash
OUTPUT_PATH="plugins/$PLUGIN_NAME/mockups/mockup-$PLUGIN_NAME_LOWER.html"
```

**Use Write tool to save complete HTML.**

**Ensure mockups directory exists:**
```bash
# If directory doesn't exist, create it first
mkdir -p "plugins/$PLUGIN_NAME/mockups"
```

### Step 7: Update Aesthetic Metadata

**Track which plugins use this aesthetic:**

1. **Read aesthetic metadata.json** (loaded in Step 1)
2. **Add plugin to usedInPlugins array:**
```json
{
  "usedInPlugins": [
    {
      "pluginName": "Dreamscape",
      "appliedDate": "2025-11-12",
      "mockupPath": "plugins/Dreamscape/mockups/mockup-dreamscape.html"
    }
  ]
}
```
3. **Write updated metadata using Write tool**

### Step 8: Present Decision Menu

(See main SKILL.md for decision gate and menu format)
```

**Replace in SKILL.md with:**
```markdown
## Operation 2: Apply Aesthetic

**High-level summary:**
Load aesthetic prose and plugin requirements, interpret aesthetic into specific CSS values, choose layout based on parameter count, generate HTML applying aesthetic to layout, save to plugin mockups directory, update aesthetic metadata, present decision menu.

**See:** `references/apply-operation.md` for complete 8-step workflow.
```

**Verification:**
- File exists at `.claude/skills/ui-template-library/references/apply-operation.md`
- SKILL.md reduced by ~316 lines
- Summary + reference link present in SKILL.md

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Remove Redundant Bash Examples (Already in Reference Doc)
**Location:** Lines 86-128
**Operation:** REPLACE
**Before:**
```markdown
### Step 2: Extract Visual Patterns

[42 lines of bash extraction examples]
```
**After:**
```markdown
(This fix is superseded by extraction to references/save-operation.md in Fix 2.1)
```
**Verification:** Content moved to reference doc, no duplication in SKILL.md

### Fix 3.2: Remove Template Variable Syntax Confusion
**Location:** Lines 166-169
**Operation:** REPLACE
**Before:**
```markdown
**Load template:**
```bash
TEMPLATE=".claude/skills/ui-template-library/assets/aesthetic-template.md"
```

**Fill in all sections using generated prose:**
```
**After:**
```markdown
(This fix is superseded by extraction to references/save-operation.md in Fix 2.1)
```
**Verification:** Content moved to reference doc with explicit Read tool usage

### Fix 3.3: Condense Prose Generation Examples
**Location:** Lines 133-161
**Operation:** REPLACE
**Before:**
```markdown
### Step 3: Generate Prose Descriptions

[28 lines of prose generation examples]
```
**After:**
```markdown
(This fix is superseded by extraction to references/save-operation.md in Fix 2.1)
```
**Verification:** Content moved to reference doc

### Fix 3.4: Remove Duplicated HTML Template Structure
**Location:** Lines 405-453
**Operation:** REPLACE
**Before:**
```markdown
[48 lines showing HTML template structure]
```
**After:**
```markdown
(This fix is superseded by extraction to references/apply-operation.md in Fix 2.2)
```
**Verification:** Content moved to reference doc with control-generation.md reference

### Fix 3.5: Add Critical Sequence Enforcement to List Operation
**Location:** Lines 599-653
**Operation:** WRAP
**Before:**
```markdown
## Operation 3: List Aesthetics
```
**After:**
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

## Operation 3: List Aesthetics
```
**Verification:** Confirm XML tags present around Operation 3

## File Creation Manifest

Files to create:
1. `references/save-operation.md` - Complete save workflow (8 steps, ~200 lines)
2. `references/apply-operation.md` - Complete apply workflow (8 steps, ~330 lines)

## Execution Checklist

**Phase 1: Critical Fixes**
- [ ] Fix 1.1: Save operation wrapped in `<operation>` + `<critical_sequence>` tags
- [ ] Fix 1.2: Save decision gate has `<decision_gate wait_required="true">`
- [ ] Fix 1.3: Apply operation wrapped in `<operation>` + `<critical_sequence>` tags
- [ ] Fix 1.4: Apply decision gate has `<decision_gate wait_required="true">`
- [ ] Fix 1.5: Explicit operation selection instruction added
- [ ] Fix 1.6: "CRITICAL: Execute in order" instruction added to Save
- [ ] Fix 1.7: ui-mockup integration has `<handoff_protocol>` with data contract

**Phase 2: Extractions**
- [ ] Fix 2.1: `references/save-operation.md` created with complete workflow
- [ ] Fix 2.1: SKILL.md Save section reduced to summary + reference
- [ ] Fix 2.2: `references/apply-operation.md` created with complete workflow
- [ ] Fix 2.2: SKILL.md Apply section reduced to summary + reference
- [ ] Verify: SKILL.md reduced by ~500 lines (36% reduction)

**Phase 3: Polish**
- [ ] Fix 3.5: List operation has `<critical_sequence>` enforcement
- [ ] Verify: All operations (Save, Apply, List, Delete, Update) have XML enforcement
- [ ] Verify: All decision menus have `<decision_gate wait_required="true">`
- [ ] Verify: No redundant content between SKILL.md and reference docs

**Final Verification**
- [ ] All 3 critical issues from analysis addressed
- [ ] All 8 optimization opportunities implemented
- [ ] SKILL.md context reduced by ~3,000 tokens (29% net after XML additions)
- [ ] Every operation has explicit sequencing and blocking gates
- [ ] ui-mockup integration formally specified with data contract
- [ ] Checkpoint protocol enforced at all decision points
