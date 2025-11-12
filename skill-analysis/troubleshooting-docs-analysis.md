# Skill Analysis: troubleshooting-docs

## Executive Summary
- Overall health: **Yellow** (Functional but has critical comprehension blockers)
- Critical issues: **3**
- Optimization opportunities: **7**
- Estimated context savings: **~2700 tokens (18.3% reduction)**

## Dimension Scores (1-5)
1. Structure Compliance: **4/5** - Proper frontmatter, good tool selection, clear preconditions
2. Content Extraction: **2/5** - Extensive duplication (~100 lines), schema repeated 3x
3. Instruction Clarity: **3/5** - Good structure but conflicting instructions about dual-indexing
4. XML Organization: **1/5** - No XML tags used; critical sequences unenforced
5. Context Efficiency: **2/5** - ~18% bloat from duplication and verbosity
6. Claude-Optimized Language: **3/5** - Mostly clear but some ambiguous pronouns
7. System Integration: **4/5** - Good checkpoint protocol adherence, inline menus
8. Critical Sequence Enforcement: **1/5** - No sequence enforcement despite critical ordering requirements

**Average Score: 2.5/5**

## Critical Issues (blockers for Claude comprehension)

### 1. Conflicting Dual-Indexing Instructions (Lines 203-252, 349-363, 563-584)
**Impact: HIGH - Creates ambiguity about fundamental behavior**

The skill contains contradictory instructions about file organization:
- **Lines 203-216:** "Single file in category directory (no symlinks)"
- **Lines 220-252:** Instructions for creating both real file AND symlink
- **Lines 349-363:** Decision menu shows both paths: "Real: ... / Symlink: ..."
- **Lines 563-584:** Example scenario demonstrates dual-indexing structure

Claude will be confused about which approach to implement. Based on context, the single-file approach (lines 203-216) appears to be the intended design, but the majority of the skill describes dual-indexing.

**Resolution Required:** Choose ONE approach and remove all references to the other.

### 2. Missing Critical Sequence Enforcement (Lines 33-341)
**Impact: HIGH - Steps could execute out of order or be skipped**

The 7-step documentation process MUST execute sequentially:
1. Detect confirmation → 2. Gather context → 3. Check existing → 4. Generate filename → 5. Validate YAML → 6. Create documentation → 7. Cross-reference

However, no XML tags enforce this ordering. Claude could:
- Skip validation (Step 5) and proceed to file creation (Step 6)
- Create documentation before checking for duplicates
- Bypass the YAML validation gate

**Critical dependency:** Step 6 MUST NOT execute until Step 5 validation passes.

### 3. Content Duplication (~100 lines across 4 locations)
**Impact: MEDIUM - Wastes 18% of context, maintenance burden**

Schema information duplicated in three places:
- **Lines 136-184 (49 lines):** Complete enum descriptions inline
- **schema.yaml (137 lines):** Full schema specification
- **references/yaml-schema.md (62 lines):** Simplified reference

Additional duplication:
- **Lines 203-216:** Category mapping (also in references/yaml-schema.md lines 49-61)
- **Lines 254-267:** Template structure (redundant with assets/resolution-template.md)

## Optimization Opportunities

### Content to Extract to references/
**No new reference files needed** - Existing references are adequate

### Content to Extract to assets/

#### Extract: Critical Pattern Template (Lines 317-341, 25 lines → 1 line reference)
**Estimated savings: 24 lines**

Extract the template for adding patterns to juce8-critical-patterns.md:

**Create:** `assets/critical-pattern-template.md`
```markdown
## N. [Pattern Name] (ALWAYS REQUIRED)

### ❌ WRONG ([Will cause X error])
```[language]
[code showing wrong approach]
```

### ✅ CORRECT
```[language]
[code showing correct approach]
```

**Why:** [Technical explanation of why this is required]

**Placement/Context:** [When this applies]

**Documented in:** `troubleshooting/[category]/[filename].md`
```

**Replace lines 317-341 with:**
```markdown
**Template for critical pattern addition:**

When user selects Option 2 (Add to Required Reading), use the template from `assets/critical-pattern-template.md`.
```

### Content to Remove (Duplication)

#### Remove: Inline Schema Duplication (Lines 136-184, 49 lines → 2 lines)
**Estimated savings: 47 lines**

BEFORE:
```markdown
**Load schema:**
```bash
cat .claude/skills/troubleshooting-docs/schema.yaml
```

**Classify the problem:**
- Which `problem_type` enum value? (build_error, runtime_error, ui_layout, etc.)
- Which `component` enum value? (cmake, juce_gui_basics, juce_dsp, etc.)
- What are specific `symptoms`? (exact error messages, observable behavior)
- What's the `root_cause` enum value? (missing_module, missing_constraint, etc.)
- What `resolution_type`? (code_fix, config_change, etc.)
- What `severity`? (critical, moderate, minor)
- What searchable `tags`? (lowercase, hyphenated keywords)

**Required fields:**
- ✅ `plugin` - String (plugin name or "JUCE")
- ✅ `date` - String (YYYY-MM-DD format)
[... 42 more lines of enum descriptions ...]
```

AFTER:
```markdown
**Validate against schema:**
Load `schema.yaml` and classify the problem against the enum values defined in `references/yaml-schema.md`.
```

#### Remove: Category Mapping Duplication (Lines 203-216, 14 lines → 1 line)
**Estimated savings: 13 lines**

BEFORE:
```markdown
**Determine category from problem_type enum:**

Category mapping (based on validated `problem_type` field):

- `build_error` → troubleshooting/build-failures/
- `runtime_error` → troubleshooting/runtime-issues/
- `ui_layout` → troubleshooting/gui-issues/
- `api_misuse` → troubleshooting/api-usage/
- `dsp_issue` → troubleshooting/dsp-issues/
- `state_management` → troubleshooting/parameter-issues/
- `performance` → troubleshooting/runtime-issues/
- `thread_violation` → troubleshooting/runtime-issues/
- `validation_failure` → troubleshooting/validation-problems/
```

AFTER:
```markdown
**Determine category from problem_type:** Use the category mapping defined in `references/yaml-schema.md`.
```

#### Remove: Template Structure Redundancy (Lines 254-267, 14 lines → 1 line)
**Estimated savings: 13 lines**

BEFORE:
```markdown
**Documentation template:**

Use template from `assets/resolution-template.md` with populated values:

- YAML frontmatter with validated fields
- Problem title (descriptive)
- Symptom section (exact errors, observable behavior)
- Context section (plugin, stage, JUCE version, OS)
- Investigation steps tried (what didn't work and why)
- Root cause (technical explanation)
- Solution (step-by-step fix with code examples)
- Prevention (how to avoid)
- Related issues (cross-references if found in Step 3)
- References (JUCE docs, forum threads, external resources)
```

AFTER:
```markdown
**Create documentation:** Populate `assets/resolution-template.md` with the gathered context and validated YAML frontmatter.
```

### XML Tag Opportunities

#### Wrap: Critical Sequence (Lines 33-341)
**Reason: Enforce strict ordering of documentation steps**

BEFORE:
```markdown
## 7-Step Process

### Step 1: Detect Confirmation
[instructions]

### Step 2: Gather Context
[instructions]
```

AFTER:
```markdown
<critical_sequence name="documentation-capture" enforce_order="strict">

## 7-Step Process

<step number="1" required="true">
### Step 1: Detect Confirmation

**Auto-invoke after phrases:**
- "that worked"
- "it's fixed"
[...]
</step>

<step number="2" required="true" depends_on="1">
### Step 2: Gather Context

Extract from conversation history:
[...]
</step>

<step number="5" required="true" depends_on="4">
### Step 5: Validate YAML Schema

<validation_gate name="yaml-schema" blocking="true">
**CRITICAL:** All docs require validated YAML frontmatter with enum validation.

[validation instructions]

**BLOCK if validation fails:**

❌ YAML validation failed

Errors:
[...]

**REQUIRED:** Present retry with corrected values. Do NOT proceed to Step 6 until YAML is valid.
</validation_gate>
</step>

[... remaining steps ...]

</critical_sequence>
```

**Impact:** Prevents Claude from skipping validation or executing steps out of order.

#### Wrap: Decision Gate (Lines 349-407)
**Reason: Mark explicit decision point with user wait requirement**

BEFORE:
```markdown
## Decision Menu After Capture

After successful documentation:

```
✓ Solution documented
[menu options]
```

**Handle responses:**
[...]
```

AFTER:
```markdown
<decision_gate name="post-documentation" wait_for_user="true">

## Decision Menu After Capture

After successful documentation, present options and WAIT for user response:

```
✓ Solution documented
[menu options]
```

**Handle responses:**
[...]

</decision_gate>
```

#### Wrap: Integration Protocol (Lines 410-441)
**Reason: Clarify skill boundaries and handoff requirements**

BEFORE:
```markdown
## Integration Points

**Invoked by:**
[...]

**Invokes:**
[...]
```

AFTER:
```markdown
<integration_protocol>

## Integration Points

**Invoked by:**
[...]

**Invokes:**
- None (terminal skill - does not delegate to other skills)

[...]

</integration_protocol>
```

#### Wrap: Success Criteria (Lines 444-456)
**Reason: Make validation checklist explicit and scannable**

BEFORE:
```markdown
## Success Criteria

Documentation is successful when:

- ✅ YAML frontmatter validated (all required fields, correct formats)
[...]
```

AFTER:
```markdown
<success_criteria>

Documentation is successful when ALL of the following are true:

- ✅ YAML frontmatter validated (all required fields, correct formats)
- ✅ Real file created in troubleshooting/by-plugin/[Plugin]/[category]/
[...]

</success_criteria>
```

### Instruction Clarity Improvements

#### Line 82: Make blocking requirement explicit
BEFORE:
```markdown
**Ask user if missing critical context:**
```

AFTER:
```markdown
**BLOCKING REQUIREMENT:** If critical context is missing, ask user and WAIT for response before proceeding:
```

#### Lines 104-116: Add explicit IF-THEN-ELSE structure
BEFORE:
```markdown
**If similar issue found:**

Present options:
```

AFTER:
```markdown
**IF similar issue found:**

THEN present decision options:

```
Found similar issue: troubleshooting/[path]

What's next?
1. Create new doc with cross-reference (recommended)
[...]
```

ELSE proceed to Step 4 (no user interaction needed).
```

#### Line 254: Remove ambiguous "the documentation"
BEFORE:
```markdown
Use template from `assets/resolution-template.md` with populated values:
```

AFTER:
```markdown
Create the documentation file using `assets/resolution-template.md` as the structure, populating each section with context from Step 2:
```

### Verbosity Reductions

#### Lines 22-29: Condense dual-indexing explanation (8 lines → 3 lines)
**Estimated savings: 5 lines**

BEFORE:
```markdown
**Why dual-indexing matters:**

When researching problems, you might know the plugin name OR the symptom, but not both:

- "What build issues has DelayPlugin had?" → Search by-plugin/DelayPlugin/
- "What causes parameter validation failures?" → Search by-symptom/validation-problems/

Both paths lead to the same documentation (via symlinks).
```

AFTER (if keeping dual-indexing):
```markdown
**Dual-indexing:** Documentation is accessible by plugin name (`by-plugin/`) or symptom category (`by-symptom/`) via symlinks for flexible lookup.
```

OR (if removing dual-indexing per lines 203-216):
```markdown
**Organization:** Documentation is filed by symptom category (determined from `problem_type` enum) for consistent lookup.
```

#### Lines 491-511: Condense "Notes for Claude" (21 lines → 12 lines)
**Estimated savings: 9 lines**

BEFORE:
```markdown
## Notes for Claude

**When executing this skill:**

1. Always validate YAML frontmatter - BLOCK if invalid
2. Use relative symlinks (../../by-plugin/...) not absolute paths
3. Extract exact error messages from conversation
4. Include code examples in solution section
5. Cross-reference similar issues automatically
6. Category detection is automatic from tags
7. Ask user if critical context missing
8. Be specific in documentation (exact file:line, versions)

**Common pitfalls:**

- Forgetting to create directories before writing files
- Using absolute symlink paths (breaks portability)
- Missing YAML validation (creates invalid docs)
- Vague descriptions (not searchable)
- No code examples (harder to understand solution)
- No cross-references (knowledge stays siloed)
```

AFTER:
```markdown
## Execution Guidelines

**MUST do:**
- Validate YAML frontmatter (BLOCK if invalid)
- Extract exact error messages from conversation
- Include code examples in solution section
- Create directories before writing files
- Ask user if critical context missing

**MUST NOT do:**
- Skip YAML validation
- Use vague descriptions
- Omit code examples or cross-references
- Use absolute paths for symlinks (if applicable)
```

### Critical Sequence Enforcement

#### Primary: Wrap 7-Step Process (Lines 33-341)
**Lines to wrap: 308 lines**

The entire 7-step process needs `<critical_sequence>` wrapper with individual `<step>` tags:

```xml
<critical_sequence name="documentation-capture" enforce_order="strict">

<step number="1" required="true">
### Step 1: Detect Confirmation
[content]
</step>

<step number="2" required="true" depends_on="1">
### Step 2: Gather Context
[content]
</step>

<step number="3" required="false" depends_on="2">
### Step 3: Check Existing Docs
[content]
</step>

<step number="4" required="true" depends_on="2">
### Step 4: Generate Filename
[content]
</step>

<step number="5" required="true" depends_on="4" blocking="true">
### Step 5: Validate YAML Schema

<validation_gate name="yaml-schema" blocking="true">
[YAML validation instructions]

**REQUIRED:** Do NOT proceed to Step 6 until validation passes.
</validation_gate>
</step>

<step number="6" required="true" depends_on="5">
### Step 6: Create Documentation
[content]
</step>

<step number="7" required="false" depends_on="6">
### Step 7: Cross-Reference & Critical Pattern Detection
[content]
</step>

</critical_sequence>
```

**Enforcement attributes:**
- `enforce_order="strict"`: Steps must execute 1→2→3→4→5→6→7
- `depends_on`: Explicit dependencies
- `blocking="true"`: Step 5 validation MUST complete before Step 6

## Examples of Issues Found

### Example 1: Conflicting Instructions Create Ambiguity

**Location:** Lines 203-252 vs 220-252, 349-363

**Issue:** The skill says both "no symlinks" AND provides instructions for creating symlinks.

**BEFORE (Lines 203-216):**
```markdown
**Determine category from problem_type enum:**

Category mapping (based on validated `problem_type` field):
[mapping]

**Create documentation file:**
[...]
**Result:**
- Single file in category directory (no symlinks)
- Enum validation ensures consistent categorization
```

**ALSO IN SAME SKILL (Lines 220-252):**
```markdown
**Create documentation file:**

```bash
PROBLEM_TYPE="[from validated YAML]"
CATEGORY="[mapped from problem_type]"
FILENAME="[generated-filename].md"
DOC_PATH="troubleshooting/${CATEGORY}/${FILENAME}"

# Create directory if needed
mkdir -p "troubleshooting/${CATEGORY}"

# Write documentation using template
[...]
```

**Result:**
- Single file in category directory (no symlinks)
```

**BUT ALSO (Lines 349-363 - Decision Menu):**
```markdown
✓ Solution documented

File created:
- Real: troubleshooting/by-plugin/[Plugin]/[category]/[filename].md
- Symlink: troubleshooting/by-symptom/[category]/[filename].md
```

**Impact:** Claude receives contradictory instructions about fundamental file structure.

**FIX:** Remove ALL references to either dual-indexing OR single-file approach. Be consistent throughout.

### Example 2: Missing Validation Gate Allows Invalid Docs

**Location:** Lines 186-200

**Issue:** YAML validation is described as blocking, but no XML gate enforces it.

**BEFORE:**
```markdown
**BLOCK if validation fails:**

```
❌ YAML validation failed

Errors:
- problem_type: must be one of schema enums, got "compilation_error"
- severity: must be one of [critical, moderate, minor], got "high"
- symptoms: must be array with 1-5 items, got string

Please provide corrected values.
```

Present retry with corrected values, don't proceed until valid.
```

**Problem:** Nothing prevents Claude from continuing to Step 6 if validation logic fails.

**AFTER:**
```markdown
<validation_gate name="yaml-schema" blocking="true">

**BLOCK if validation fails:**

```
❌ YAML validation failed

Errors:
- problem_type: must be one of schema enums, got "compilation_error"
- severity: must be one of [critical, moderate, minor], got "high"
- symptoms: must be array with 1-5 items, got string
```

**REQUIRED:** Present retry with corrected values.

**GATE ENFORCEMENT:** Do NOT proceed to Step 6 (Create Documentation) until YAML frontmatter passes all validation rules defined in `schema.yaml`.

</validation_gate>
```

**Impact:** Explicit gate makes the blocking requirement enforceable, not just advisory.

### Example 3: Schema Duplication Wastes Context

**Location:** Lines 136-184 (49 lines) duplicating schema.yaml and references/yaml-schema.md

**Issue:** Complete enum descriptions are inlined when a reference would suffice.

**BEFORE (49 lines):**
```markdown
**Load schema:**
```bash
cat .claude/skills/troubleshooting-docs/schema.yaml
```

**Classify the problem:**
- Which `problem_type` enum value? (build_error, runtime_error, ui_layout, etc.)
- Which `component` enum value? (cmake, juce_gui_basics, juce_dsp, etc.)
- What are specific `symptoms`? (exact error messages, observable behavior)
- What's the `root_cause` enum value? (missing_module, missing_constraint, etc.)
- What `resolution_type`? (code_fix, config_change, etc.)
- What `severity`? (critical, moderate, minor)
- What searchable `tags`? (lowercase, hyphenated keywords)

**Required fields:**
- ✅ `plugin` - String (plugin name or "JUCE")
- ✅ `date` - String (YYYY-MM-DD format)
- ✅ `problem_type` - Enum from schema
- ✅ `component` - Enum from schema
- ✅ `symptoms` - Array (1-5 items)
- ✅ `root_cause` - Enum from schema
- ✅ `resolution_type` - Enum from schema
- ✅ `severity` - Enum from schema

**Optional fields:**
- `juce_version` - String (X.Y.Z format)
- `tags` - Array of strings

**Validation process:**

```bash
# Verify enum values against schema
# problem_type must be in: build_error, runtime_error, api_misuse, validation_failure,
#   ui_layout, dsp_issue, state_management, performance, thread_violation

# component must be in: cmake, juce_processor, juce_editor, juce_dsp, juce_gui_basics,
#   juce_audio_utils, apvts, pluginval, xcode, system, webview

# root_cause must be in: missing_framework, missing_module, wrong_api, thread_violation,
#   missing_constraint, state_sync, memory_issue, config_error, version_incompatibility,
#   logic_error, event_timing, url_protocol

# resolution_type must be in: code_fix, config_change, environment_setup, api_migration

# severity must be in: critical, moderate, minor
```
```

**AFTER (2 lines):**
```markdown
**Validate against schema:**
Load `schema.yaml` and classify the problem against the enum values defined in `references/yaml-schema.md`. Ensure all required fields are present and match allowed values exactly.
```

**Token Savings:** 47 lines (~1175 tokens)

## Estimated Impact

### Context Reduction
- Schema duplication removal: **~1175 tokens**
- Category mapping removal: **~325 tokens**
- Template structure reference: **~250 tokens**
- Critical pattern extraction: **~625 tokens**
- Verbosity reductions: **~350 tokens**

**Total: ~2725 tokens (18.3% reduction)**

Current SKILL.md: ~597 lines (~14,925 tokens @ 25 tokens/line average)
After optimization: ~488 lines (~12,200 tokens)

### Comprehension Improvement
**HIGH** - Resolving the dual-indexing conflict removes a major ambiguity that could cause Claude to implement the wrong file structure. Adding critical sequence enforcement ensures steps execute in the correct order, particularly preventing invalid YAML from reaching file creation.

### Maintenance Improvement
**HIGH** - Extracting duplicated schema content to references means updates only need to happen in one place (schema.yaml). Currently, enum changes require updating 3 files. XML tags make critical sequences explicit for future maintainers.

## Priority Recommendations

### 1. CRITICAL: Resolve Dual-Indexing Conflict (Lines 203-252, 349-363, 563-584)
**Impact: High - Blocks Claude comprehension of fundamental behavior**

Choose ONE file organization approach:

**Option A: Single-file (matches lines 203-216)**
- Remove all references to "by-plugin/" and "by-symptom/" paths
- Remove symlink creation instructions (lines 220-252)
- Update decision menu (lines 349-363) to show single path
- Update example scenario (lines 563-584) to show single path
- Update description (line 3) to remove "dual-indexed"

**Option B: Dual-indexing (matches lines 220-252, 349-363)**
- Remove "no symlinks" statement (lines 248-250)
- Ensure symlink instructions are clear and correct
- Keep dual paths in decision menu and examples

**Recommendation:** Choose Option A (single-file) based on the explicit statement "Single file in category directory (no symlinks)" at lines 248-250, which appears to be the authoritative design decision.

**Estimated time:** 15 minutes

### 2. CRITICAL: Add Critical Sequence Enforcement (Lines 33-341)
**Impact: High - Ensures steps execute in correct order, prevents invalid docs**

Wrap the 7-step process in `<critical_sequence>` with individual `<step>` tags and explicit dependencies:

```xml
<critical_sequence name="documentation-capture" enforce_order="strict">
  <step number="1" required="true">...</step>
  <step number="2" required="true" depends_on="1">...</step>
  <step number="5" required="true" depends_on="4" blocking="true">
    <validation_gate name="yaml-schema" blocking="true">
      [YAML validation with explicit gate]
    </validation_gate>
  </step>
  <step number="6" required="true" depends_on="5">...</step>
</critical_sequence>
```

**Key additions:**
- Wrap entire 7-step process (lines 33-341)
- Add `<validation_gate>` inside Step 5 (lines 186-200)
- Add `depends_on` attributes to enforce Step 5 → Step 6 dependency
- Add explicit "Do NOT proceed to Step 6 until validation passes" instruction

**Estimated time:** 20 minutes

### 3. HIGH: Extract Duplicated Schema Content (Lines 136-184, 203-216, 254-267, 317-341)
**Impact: Medium - 18% context reduction, improved maintainability**

**Actions:**
1. Replace lines 136-184 (49 lines) with 2-line reference to schema.yaml
2. Replace lines 203-216 (14 lines) with 1-line reference to yaml-schema.md
3. Replace lines 254-267 (14 lines) with 1-line reference to resolution-template.md
4. Extract lines 317-341 (25 lines) to assets/critical-pattern-template.md

**Files to create:**
- `assets/critical-pattern-template.md` (new file with critical pattern structure)

**Total reduction:** ~97 lines → ~5 lines (92 line savings, ~2300 tokens)

**Estimated time:** 15 minutes

## Implementation Notes

### Files that would be created in references/
- None (existing references are adequate)

### Files that would be created in assets/
- `assets/critical-pattern-template.md` - Template for adding patterns to juce8-critical-patterns.md (extracted from lines 317-341)

### Estimated time to implement all fixes
- Priority 1 (Dual-indexing resolution): 15 minutes
- Priority 2 (Critical sequence enforcement): 20 minutes
- Priority 3 (Content extraction): 15 minutes

**Total: ~50 minutes**

### Implementation Order
1. **First:** Fix dual-indexing conflict (establishes correct file structure)
2. **Second:** Add XML enforcement tags (improves execution reliability)
3. **Third:** Extract duplicated content (reduces context usage)

This order ensures structural correctness before optimization.
