# Fix Plan: troubleshooting-docs

## Summary
- Critical fixes: **7**
- Extraction operations: **4**
- Total estimated changes: **14 operations**
- Estimated time: **50 minutes**

---

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Resolve Dual-Indexing Conflict (Choose Single-File Approach)
**Location:** Lines 3, 22-29, 203-252, 349-363, 563-584
**Operation:** REPLACE + DELETE
**Rationale:** Lines 203-216 explicitly state "Single file in category directory (no symlinks)" but other sections describe dual-indexing. Based on the explicit statement, single-file is the authoritative design.

**Change 1.1a - Update description (Line 3):**
**Before:**
```markdown
description: Capture solved problems as dual-indexed documentation (by-plugin + by-symptom) with YAML frontmatter for fast lookup
```
**After:**
```markdown
description: Capture solved problems as categorized documentation with YAML frontmatter for fast lookup
```
**Verification:** Line 3 no longer mentions "dual-indexed"

**Change 1.1b - Simplify explanation (Lines 22-29):**
**Before:**
```markdown
**Why dual-indexing matters:**

When researching problems, you might know the plugin name OR the symptom, but not both:

- "What build issues has DelayPlugin had?" → Search by-plugin/DelayPlugin/
- "What causes parameter validation failures?" → Search by-symptom/validation-problems/

Both paths lead to the same documentation (via symlinks).
```
**After:**
```markdown
**Organization:** Documentation is filed by symptom category (determined from `problem_type` enum) for consistent lookup and searchability.
```
**Verification:** Section reduced from 8 lines to 1 line, mentions only symptom category

**Change 1.1c - Remove symlink instructions (Lines 220-252):**
**Operation:** DELETE lines 220-252 entirely
**Replace with:**
```markdown
**Create documentation file:**

```bash
PROBLEM_TYPE="[from validated YAML]"
CATEGORY="[mapped from problem_type]"
FILENAME="[generated-filename].md"
DOC_PATH="troubleshooting/${CATEGORY}/${FILENAME}"

# Create directory if needed
mkdir -p "troubleshooting/${CATEGORY}"

# Write documentation using template from assets/resolution-template.md
# (Content populated with Step 2 context and validated YAML frontmatter)
```

**Result:**
- Single file in category directory
- Enum validation ensures consistent categorization
```
**Verification:** No references to symlinks, by-plugin/, or by-symptom/ paths

**Change 1.1d - Update decision menu (Lines 349-363):**
**Before:**
```markdown
✓ Solution documented

File created:
- Real: troubleshooting/by-plugin/[Plugin]/[category]/[filename].md
- Symlink: troubleshooting/by-symptom/[category]/[filename].md

What's next?
```
**After:**
```markdown
✓ Solution documented

File created:
- troubleshooting/[category]/[filename].md

What's next?
```
**Verification:** Decision menu shows single file path only

**Change 1.1e - Update example scenario (Lines 563-584):**
**Before:**
```markdown
**Resulting structure:**
```
troubleshooting/
├── by-plugin/
│   └── LushVerb/
│       └── runtime-issues/
│           └── frozen-webview-knobs-es6-module-loading.md
└── by-symptom/
    └── runtime-issues/
        └── frozen-webview-knobs-es6-module-loading.md (symlink)
```
```
**After:**
```markdown
**Resulting structure:**
```
troubleshooting/
└── runtime-issues/
    └── frozen-webview-knobs-es6-module-loading.md
```
```
**Verification:** Example shows single-file structure

---

### Fix 1.2: Add Critical Sequence Enforcement (Wrap 7-Step Process)
**Location:** Lines 33-341
**Operation:** WRAP
**Rationale:** Prevent steps from executing out of order or being skipped, especially the critical validation gate in Step 5

**Before:**
```markdown
## 7-Step Process

### Step 1: Detect Confirmation

**Auto-invoke after phrases:**
[...]
```

**After:**
```markdown
<critical_sequence name="documentation-capture" enforce_order="strict">

## 7-Step Process

<step number="1" required="true">
### Step 1: Detect Confirmation

**Auto-invoke after phrases:**
[...]
</step>

<step number="2" required="true" depends_on="1">
### Step 2: Gather Context

Extract from conversation history:
[...]
</step>

<step number="3" required="false" depends_on="2">
### Step 3: Check Existing Docs

**Search for duplicates:**
[...]
</step>

<step number="4" required="true" depends_on="2">
### Step 4: Generate Filename

**Naming convention:**
[...]
</step>

<step number="5" required="true" depends_on="4" blocking="true">
### Step 5: Validate YAML Schema

<validation_gate name="yaml-schema" blocking="true">

**Validate against schema:**
Load `schema.yaml` and classify the problem against the enum values defined in `references/yaml-schema.md`. Ensure all required fields are present and match allowed values exactly.

**BLOCK if validation fails:**

```
❌ YAML validation failed

Errors:
- problem_type: must be one of schema enums, got "compilation_error"
- severity: must be one of [critical, moderate, minor], got "high"
- symptoms: must be array with 1-5 items, got string

Please provide corrected values.
```

**GATE ENFORCEMENT:** Do NOT proceed to Step 6 (Create Documentation) until YAML frontmatter passes all validation rules defined in `schema.yaml`.

</validation_gate>
</step>

<step number="6" required="true" depends_on="5">
### Step 6: Create Documentation

[content...]
</step>

<step number="7" required="false" depends_on="6">
### Step 7: Cross-Reference & Critical Pattern Detection

[content...]
</step>

</critical_sequence>
```

**Verification:**
- Opening `<critical_sequence>` tag at start of 7-step process
- Each step wrapped in `<step>` tags with number attribute
- Step 5 contains `<validation_gate>` with blocking attribute
- Closing `</critical_sequence>` tag after Step 7
- Dependencies explicit: Step 6 depends_on Step 5

---

### Fix 1.3: Wrap Decision Gate (Post-Documentation Menu)
**Location:** Lines 349-407
**Operation:** WRAP
**Rationale:** Make explicit that Claude must WAIT for user response at this decision point

**Before:**
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

**After:**
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

**Verification:**
- Opening `<decision_gate>` tag with wait_for_user attribute
- Closing `</decision_gate>` tag after response handling
- Instruction explicitly says "WAIT for user response"

---

### Fix 1.4: Make Blocking Requirement Explicit (Step 2)
**Location:** Line 82
**Operation:** REPLACE
**Rationale:** Clarify that Claude must STOP and wait for user if critical context is missing

**Before:**
```markdown
**Ask user if missing critical context:**
```

**After:**
```markdown
**BLOCKING REQUIREMENT:** If critical context is missing (plugin name, exact error, stage, or resolution steps), ask user and WAIT for response before proceeding to Step 3:
```

**Verification:** Line 82 now uses "BLOCKING REQUIREMENT" keyword and explicit WAIT instruction

---

### Fix 1.5: Add IF-THEN-ELSE Structure (Step 3)
**Location:** Lines 104-116
**Operation:** REPLACE
**Rationale:** Make conditional logic explicit to prevent Claude from always presenting decision menu

**Before:**
```markdown
**If similar issue found:**

Present options:
```

**After:**
```markdown
**IF similar issue found:**

THEN present decision options:

```
Found similar issue: troubleshooting/[path]

What's next?
1. Create new doc with cross-reference (recommended)
2. Update existing doc (only if same root cause)
3. Other

Choose (1-3): _
```

WAIT for user response, then execute chosen action.

**ELSE** (no similar issue found):

Proceed directly to Step 4 (no user interaction needed).
```

**Verification:**
- Uses IF-THEN-ELSE keywords
- ELSE clause explicit (proceed to Step 4)
- Decision menu only shown if similar issue found

---

### Fix 1.6: Wrap Integration Protocol
**Location:** Lines 410-441
**Operation:** WRAP
**Rationale:** Clarify skill boundaries and handoff requirements

**Before:**
```markdown
## Integration Points

**Invoked by:**
[...]

**Invokes:**
[...]
```

**After:**
```markdown
<integration_protocol>

## Integration Points

**Invoked by:**
- deep-research skill (after solution found)
- plugin-improve skill (after fix validated)
- Manual invocation via /doc-fix command

**Invokes:**
- None (terminal skill - does not delegate to other skills)

**Handoff expectations:**
All context needed for documentation should be present in conversation history before invocation.

</integration_protocol>
```

**Verification:**
- Opening `<integration_protocol>` tag
- Closing `</integration_protocol>` tag
- Explicit statement that this is a terminal skill

---

### Fix 1.7: Wrap Success Criteria
**Location:** Lines 444-456
**Operation:** WRAP
**Rationale:** Make validation checklist explicit and scannable

**Before:**
```markdown
## Success Criteria

Documentation is successful when:

- ✅ YAML frontmatter validated (all required fields, correct formats)
[...]
```

**After:**
```markdown
<success_criteria>

## Success Criteria

Documentation is successful when ALL of the following are true:

- ✅ YAML frontmatter validated (all required fields, correct formats)
- ✅ File created in troubleshooting/[category]/[filename].md
- ✅ Enum values match schema.yaml exactly
- ✅ Code examples included in solution section
- ✅ Cross-references added if related issues found
- ✅ User presented with decision menu and action confirmed

</success_criteria>
```

**Verification:**
- Opening `<success_criteria>` tag
- Closing `</success_criteria>` tag
- Phrase "ALL of the following are true" added

---

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Extract Critical Pattern Template to Assets
**Location:** Lines 317-341
**Operation:** EXTRACT
**Estimated savings:** 24 lines (~600 tokens)

**Create file:** `assets/critical-pattern-template.md`
**File content:**
```markdown
# Critical Pattern Template

Use this template when adding a pattern to `troubleshooting/patterns/juce8-critical-patterns.md`:

---

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

---

**Instructions:**
1. Replace N with the next pattern number
2. Replace [Pattern Name] with descriptive title
3. Fill in WRONG example with code that causes the problem
4. Fill in CORRECT example with the solution
5. Explain the technical reason in "Why"
6. Clarify when this pattern applies in "Placement/Context"
7. Link to the full troubleshooting doc where this was originally solved
```

**Replace lines 317-341 with:**
```markdown
**Template for critical pattern addition:**

When user selects Option 2 (Add to Required Reading), use the template from `assets/critical-pattern-template.md` to structure the pattern entry. Number it sequentially based on existing patterns in `troubleshooting/patterns/juce8-critical-patterns.md`.
```

**Verification:**
- File exists at `.claude/skills/troubleshooting-docs/assets/critical-pattern-template.md`
- SKILL.md reduced by ~24 lines
- Reference to assets file present at lines 317-341

---

### Fix 2.2: Replace Inline Schema Duplication with Reference
**Location:** Lines 136-184
**Operation:** REPLACE
**Estimated savings:** 47 lines (~1175 tokens)

**Before (49 lines):**
```markdown
**Load schema:**
```bash
cat .claude/skills/troubleshooting-docs/schema.yaml
```

**Classify the problem:**
- Which `problem_type` enum value? (build_error, runtime_error, ui_layout, etc.)
- Which `component` enum value? (cmake, juce_gui_basics, juce_dsp, etc.)
[... 42 more lines of enum descriptions ...]
```

**After (2 lines):**
```markdown
**Validate against schema:**
Load `schema.yaml` and classify the problem against the enum values defined in `references/yaml-schema.md`. Ensure all required fields are present and match allowed values exactly.
```

**Verification:**
- Lines 136-184 replaced with 2-line reference
- Reference points to both schema.yaml and references/yaml-schema.md
- No enum values listed inline

---

### Fix 2.3: Replace Category Mapping Duplication with Reference
**Location:** Lines 203-216
**Operation:** REPLACE
**Estimated savings:** 13 lines (~325 tokens)

**Before (14 lines):**
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

**After (1 line):**
```markdown
**Determine category from problem_type:** Use the category mapping defined in `references/yaml-schema.md` (lines 49-61).
```

**Verification:**
- Lines 203-216 replaced with 1-line reference
- Reference specifies exact line range in yaml-schema.md
- No category mappings listed inline

---

### Fix 2.4: Replace Template Structure Redundancy with Reference
**Location:** Lines 254-267
**Operation:** REPLACE
**Estimated savings:** 13 lines (~250 tokens)

**Before (14 lines):**
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

**After (1 line):**
```markdown
**Create documentation:** Populate the structure from `assets/resolution-template.md` with context gathered in Step 2 and validated YAML frontmatter from Step 5.
```

**Verification:**
- Lines 254-267 replaced with 1-line reference
- Reference mentions both context source (Step 2) and YAML source (Step 5)
- No template structure listed inline

---

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Remove Ambiguous Pronoun (Line 254)
**Location:** Line 254
**Operation:** REPLACE (already covered in Fix 2.4)
**Rationale:** "the documentation" is vague - specify what is being created

**Verification:** Covered by Fix 2.4 - new text says "Create the documentation file"

---

### Fix 3.2: Condense "Notes for Claude" Section
**Location:** Lines 491-511
**Operation:** REPLACE
**Estimated savings:** 9 lines (~225 tokens)

**Before (21 lines):**
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

**After (12 lines):**
```markdown
## Execution Guidelines

**MUST do:**
- Validate YAML frontmatter (BLOCK if invalid per Step 5 validation gate)
- Extract exact error messages from conversation
- Include code examples in solution section
- Create directories before writing files (`mkdir -p`)
- Ask user and WAIT if critical context missing

**MUST NOT do:**
- Skip YAML validation (validation gate is blocking)
- Use vague descriptions (not searchable)
- Omit code examples or cross-references
```

**Verification:**
- Section renamed from "Notes for Claude" to "Execution Guidelines"
- Reduced from 21 lines to 12 lines
- Uses MUST/MUST NOT keywords for clarity
- Removes symlink reference (no longer applicable after single-file fix)

---

## File Creation Manifest

Files to create:
1. `assets/critical-pattern-template.md` - Template structure for adding patterns to juce8-critical-patterns.md (extracted from lines 317-341)

Files to modify:
1. `SKILL.md` - All fixes above applied

Files that already exist (no changes needed):
1. `references/yaml-schema.md` - Already contains schema enum descriptions and category mappings
2. `assets/resolution-template.md` - Already contains documentation template structure
3. `schema.yaml` - Already contains complete YAML schema

---

## Execution Checklist

### Phase 1: Critical Fixes
- [ ] 1.1: Dual-indexing conflict resolved (description, explanation, instructions, decision menu, example updated to single-file)
- [ ] 1.2: 7-step process wrapped in `<critical_sequence>` with individual `<step>` tags
- [ ] 1.2a: Step 5 contains `<validation_gate>` with blocking enforcement
- [ ] 1.2b: Step 6 has explicit `depends_on="5"` attribute
- [ ] 1.3: Decision menu wrapped in `<decision_gate>` with wait_for_user attribute
- [ ] 1.4: Step 2 uses "BLOCKING REQUIREMENT" for missing context
- [ ] 1.5: Step 3 uses IF-THEN-ELSE structure with explicit ELSE clause
- [ ] 1.6: Integration Points wrapped in `<integration_protocol>` tags
- [ ] 1.7: Success Criteria wrapped in `<success_criteria>` tags

### Phase 2: Content Extraction
- [ ] 2.1: Critical pattern template extracted to `assets/critical-pattern-template.md`
- [ ] 2.1a: SKILL.md lines 317-341 replaced with reference to assets file
- [ ] 2.2: Inline schema duplication (lines 136-184) replaced with 2-line reference
- [ ] 2.3: Category mapping (lines 203-216) replaced with 1-line reference
- [ ] 2.4: Template structure (lines 254-267) replaced with 1-line reference

### Phase 3: Polish
- [ ] 3.2: "Notes for Claude" section condensed and renamed to "Execution Guidelines"
- [ ] 3.2a: Section uses MUST/MUST NOT keywords

### Verification
- [ ] SKILL.md reduced by ~92 lines (from ~597 to ~505 lines)
- [ ] Context reduced by ~2700 tokens (18.3% reduction)
- [ ] All critical sequences have XML enforcement
- [ ] No references to dual-indexing, by-plugin/, by-symptom/, or symlinks remain
- [ ] All extraction references point to correct files
- [ ] Validation gate in Step 5 explicitly blocks Step 6

---

## Estimated Impact

### Before
- Lines: ~597
- Tokens: ~14,925
- Critical issues: 3
- XML enforcement: None
- Comprehension blockers: High (dual-indexing conflict)

### After
- Lines: ~505 (-92 lines, -15.4%)
- Tokens: ~12,200 (-2,725 tokens, -18.3%)
- Critical issues: 0
- XML enforcement: 4 critical sections
- Comprehension blockers: None

### Time Estimate by Phase
- Phase 1 (Critical Fixes): 30 minutes
  - Fix 1.1: 15 minutes (5 changes across multiple locations)
  - Fix 1.2: 20 minutes (wrap 308 lines with nested tags)
  - Fixes 1.3-1.7: 5 minutes (straightforward wraps and replacements)
- Phase 2 (Content Extraction): 15 minutes
  - Fix 2.1: 5 minutes (create template file + update reference)
  - Fixes 2.2-2.4: 10 minutes (replace duplications with references)
- Phase 3 (Polish): 5 minutes
  - Fix 3.2: 5 minutes (condense and rename section)

**Total: 50 minutes**
