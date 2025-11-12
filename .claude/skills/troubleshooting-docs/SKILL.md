---
name: troubleshooting-docs
description: Capture solved problems as categorized documentation with YAML frontmatter for fast lookup
allowed-tools:
  - Read # Parse conversation context
  - Write # Create resolution docs
  - Bash # Create directories
  - Grep # Search existing docs
preconditions:
  - Problem has been solved (not in-progress)
  - Solution has been verified working
---

# troubleshooting-docs Skill

**Purpose:** Automatically document solved problems to build searchable institutional knowledge with category-based organization (enum-validated problem types).

## Overview

This skill captures problem solutions immediately after confirmation, creating structured documentation that serves as a searchable knowledge base for future sessions.

**Organization:** Single-file architecture - each problem documented as one markdown file in its symptom category directory (e.g., `troubleshooting/build-failures/cmake-version-mismatch.md`). Files use YAML frontmatter for metadata and searchability.

---

<critical_sequence name="documentation-capture" enforce_order="strict">

## 7-Step Process

<step number="1" required="true">
### Step 1: Detect Confirmation

**Auto-invoke after phrases:**

- "that worked"
- "it's fixed"
- "working now"
- "problem solved"
- "that did it"

**OR manual:** `/doc-fix` command

**Non-trivial problems only:**

- Multiple investigation attempts needed
- Tricky debugging that took time
- Non-obvious solution
- Future sessions would benefit

**Skip documentation for:**

- Simple typos
- Obvious syntax errors
- Trivial fixes immediately corrected
</step>

<step number="2" required="true" depends_on="1">
### Step 2: Gather Context

Extract from conversation history:

**Required information:**

- **Plugin name**: Which plugin had the problem
- **Symptom**: Observable error/behavior (exact error messages)
- **Investigation attempts**: What didn't work and why
- **Root cause**: Technical explanation of actual problem
- **Solution**: What fixed it (code/config changes)
- **Prevention**: How to avoid in future

**Environment details:**

- JUCE version
- Stage (0-6 or post-implementation)
- OS version
- File/line references

**BLOCKING REQUIREMENT:** If critical context is missing (plugin name, exact error, stage, or resolution steps), ask user and WAIT for response before proceeding to Step 3:

```
I need a few details to document this properly:

1. Which plugin had this issue? [PluginName]
2. What was the exact error message or symptom?
3. What stage were you in? (0-6 or post-implementation)

[Continue after user provides details]
```
</step>

<step number="3" required="false" depends_on="2">
### Step 3: Check Existing Docs

Search troubleshooting/ for similar issues:

```bash
# Search by error message keywords
grep -r "exact error phrase" troubleshooting/

# Search by symptom category
ls troubleshooting/[category]/
```

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
</step>

<step number="4" required="true" depends_on="2">
### Step 4: Generate Filename

Format: `[sanitized-symptom]-[plugin]-[YYYYMMDD].md`

**Sanitization rules:**

- Lowercase
- Replace spaces with hyphens
- Remove special characters except hyphens
- Truncate to reasonable length (< 80 chars)

**Examples:**

- `missing-juce-dsp-module-DelayPlugin-20251110.md`
- `parameter-not-saving-state-ReverbPlugin-20251110.md`
- `webview-crash-on-resize-TapeAgePlugin-20251110.md`
</step>

<step number="5" required="true" depends_on="4" blocking="true">
### Step 5: Validate YAML Schema

**CRITICAL:** All docs require validated YAML frontmatter with enum validation.

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

**Determine category from problem_type:** Use the category mapping defined in `references/yaml-schema.md` (lines 49-61).

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

**Create documentation:** Populate the structure from `assets/resolution-template.md` with context gathered in Step 2 and validated YAML frontmatter from Step 5.
</step>

<step number="7" required="false" depends_on="6">
### Step 7: Cross-Reference & Critical Pattern Detection

If similar issues found in Step 3:

**Update existing doc:**

```bash
# Add Related Issues link to similar doc
echo "- See also: [$FILENAME]($REAL_FILE)" >> [similar-doc.md]
```

**Update new doc:**
Already includes cross-reference from Step 6.

**Update patterns if applicable:**

If this represents a common pattern (3+ similar issues):

```bash
# Add to troubleshooting/patterns/common-solutions.md
cat >> troubleshooting/patterns/common-solutions.md << 'EOF'

## [Pattern Name]

**Common symptom:** [Description]
**Root cause:** [Technical explanation]
**Solution pattern:** [General approach]

**Examples:**
- [Link to doc 1]
- [Link to doc 2]
- [Link to doc 3]
EOF
```

**Critical Pattern Detection (Optional Proactive Suggestion):**

If this issue has automatic indicators suggesting it might be critical:
- Severity: `critical` in YAML
- Affects multiple plugins OR foundational stage (Stage 2 or 3)
- Non-obvious solution

Then in the decision menu (Step 8), add a note:
```
💡 This might be worth adding to Required Reading (Option 2)
```

But **NEVER auto-promote**. User decides via decision menu (Option 2).

**Template for critical pattern addition:**

When user selects Option 2 (Add to Required Reading), use the template from `assets/critical-pattern-template.md` to structure the pattern entry. Number it sequentially based on existing patterns in `troubleshooting/patterns/juce8-critical-patterns.md`.
</step>

</critical_sequence>

---

<decision_gate name="post-documentation" wait_for_user="true">

## Decision Menu After Capture

After successful documentation, present options and WAIT for user response:

```
✓ Solution documented

File created:
- troubleshooting/[category]/[filename].md

What's next?
1. Continue workflow (recommended)
2. Add to Required Reading - Promote to critical patterns (juce8-critical-patterns.md)
3. Link related issues - Connect to similar problems
4. Update common patterns - Add to pattern library
5. View documentation - See what was captured
6. Other
```

**Handle responses:**

**Option 1: Continue workflow**

- Return to calling skill/workflow
- Documentation is complete

**Option 2: Add to Required Reading** ⭐ PRIMARY PATH FOR CRITICAL PATTERNS

User selects this when:
- System made this mistake multiple times across different plugins
- Solution is non-obvious but must be followed every time
- Foundational requirement (CMake, JUCE API, threading, etc.)

Action:
1. Extract pattern from the documentation
2. Format as ❌ WRONG vs ✅ CORRECT with code examples
3. Add to `troubleshooting/patterns/juce8-critical-patterns.md`
4. Add cross-reference back to this doc
5. Confirm: "✓ Added to Required Reading. All subagents will see this pattern before code generation."

**Option 3: Link related issues**

- Prompt: "Which doc to link? (provide filename or describe)"
- Search troubleshooting/ for the doc
- Add cross-reference to both docs
- Confirm: "✓ Cross-reference added"

**Option 4: Update common patterns**

- Check if 3+ similar issues exist
- If yes: Add pattern to troubleshooting/patterns/common-solutions.md
- If no: "Need 3+ similar issues to establish pattern (currently N)"

**Option 5: View documentation**

- Display the created documentation
- Present decision menu again

**Option 6: Other**

- Ask what they'd like to do

</decision_gate>

---

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

---

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

---

## Error Handling

**Missing context:**

- Ask user for missing details
- Don't proceed until critical info provided

**YAML validation failure:**

- Show specific errors
- Present retry with corrected values
- BLOCK until valid

**Similar issue ambiguity:**

- Present multiple matches
- Let user choose: new doc, update existing, or link as duplicate

**Plugin not in PLUGINS.md:**

- Warn but don't block
- Proceed with documentation
- Suggest: "Add [Plugin] to PLUGINS.md if not there"

---

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

---

## Quality Guidelines

**Good documentation has:**

- ✅ Exact error messages (copy-paste from output)
- ✅ Specific file:line references
- ✅ Observable symptoms (what you saw, not interpretations)
- ✅ Failed attempts documented (helps avoid wrong paths)
- ✅ Technical explanation (not just "what" but "why")
- ✅ Code examples (before/after if applicable)
- ✅ Prevention guidance (how to catch early)
- ✅ Cross-references (related issues)

**Avoid:**

- ❌ Vague descriptions ("something was wrong")
- ❌ Missing technical details ("fixed the code")
- ❌ No context (which version? which file?)
- ❌ Just code dumps (explain why it works)
- ❌ No prevention guidance
- ❌ No cross-references

---

## Example Scenario

**User:** "That worked! The parameter is saving correctly now."

**Skill activates:**

1. **Detect confirmation:** "That worked!" triggers auto-invoke
2. **Gather context:**
   - Plugin: ReverbPlugin
   - Symptom: DECAY parameter not persisting after DAW reload
   - Failed attempts: Checked APVTS creation, verified parameter ID
   - Solution: Added parameterValueChanged() to update processor state
   - Root cause: State only saved from APVTS, not reflecting UI changes
3. **Check existing:** No similar issue found
4. **Generate filename:** `parameter-not-saving-decay-ReverbPlugin-20251110.md`
5. **Validate YAML:**
   ```yaml
   plugin: ReverbPlugin
   date: 2025-11-10
   symptom: DECAY parameter not persisting after DAW reload
   severity: medium
   tags: [parameters, runtime]
   ```
   ✅ Valid
6. **Create documentation:**
   - `troubleshooting/parameter-issues/parameter-not-saving-decay-ReverbPlugin-20251110.md`
7. **Cross-reference:** None needed (no similar issues)

**Output:**

```
✓ Solution documented

File created:
- troubleshooting/parameter-issues/parameter-not-saving-decay-ReverbPlugin-20251110.md

What's next?
1. Continue workflow (recommended)
2. Add to Required Reading - Promote to critical patterns (juce8-critical-patterns.md)
3. Link related issues - Connect to similar problems
4. Update common patterns - Add to pattern library
5. View documentation - See what was captured
6. Other
```

---

## Future Enhancements

**Not in Phase 7 scope, but potential:**

- Search by date range
- Filter by severity
- Tag-based search interface
- Metrics (most common issues, resolution time)
- Export to shareable format (community knowledge sharing)
- Import community solutions
