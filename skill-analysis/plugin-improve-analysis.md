# Skill Analysis: plugin-improve

## Executive Summary
- Overall health: **Yellow** (functional but needs optimization)
- Critical issues: **3**
- Optimization opportunities: **12**
- Estimated context savings: **~8,000 tokens / 33%**

## Dimension Scores (1-5)
1. Structure Compliance: **4**/5 (YAML correct, description good, tools appropriate)
2. Content Extraction: **2**/5 (large procedural blocks not extracted, changelog templates embedded)
3. Instruction Clarity: **3**/5 (mixed formats, some ambiguous conditionals)
4. XML Organization: **1**/5 (almost no XML tags for critical sequences)
5. Context Efficiency: **2**/5 (significant verbosity, duplication, no progressive disclosure)
6. Claude-Optimized Language: **3**/5 (mostly clear but some ambiguous pronouns)
7. System Integration: **4**/5 (checkpoint protocol present but inconsistent format)
8. Critical Sequence Enforcement: **1**/5 (NO enforcement tags for critical workflows)

## Critical Issues (blockers for Claude comprehension)

### 1. Missing Critical Sequence Enforcement (Lines 34-951)
**Severity: CRITICAL**

The entire skill lacks `<critical_sequence>` tags around phases that MUST execute in order. This is the skill's primary function - versioned improvements with backups - yet there's no enforcement preventing Claude from skipping backup creation or verification steps.

**Example violation:** Phase 0.9 (Backup Verification) → Phase 1 (Pre-Implementation) → Phase 2 (Backup Creation) sequence can be violated.

**Impact:** Claude could skip backup creation and proceed directly to implementation, violating the core safety guarantee.

### 2. Inconsistent Decision Menu Format (Lines 82-169, 773-794)
**Severity: CRITICAL**

Phase 0 uses `AskUserQuestion` tool (lines 82-169) while Phase 8 uses inline numbered menus (lines 773-794). The CLAUDE.md explicitly states: "Do NOT use AskUserQuestion tool for decision menus - use inline numbered lists."

**Locations:**
- Line 82: Phase 0 Specificity Detection - uses AskUserQuestion
- Line 99: Phase 0.3 Clarification Questions - uses AskUserQuestion (4 questions)
- Line 154: Phase 0.4 Decision Gate - uses AskUserQuestion
- Line 773: Phase 8 Completion - uses inline numbered list ✓

**Impact:** Violates system-wide checkpoint protocol, creates inconsistent UX.

### 3. Missing Decision Gates (Lines 213-259, 407-422)
**Severity: HIGH**

Phase 0.5 (Investigation) Tier 3 delegation and Phase 3 (Implementation) both lack `<decision_gate>` tags and explicit user approval checkpoints before expensive operations.

**Example:** Line 248-252 invokes deep-research skill without explicit approval gate wrapping.

## Optimization Opportunities

### Content to Extract to references/

#### 1. Lines 430-540: Enhanced CHANGELOG Template → references/changelog-format.md
**Estimated savings: ~2,500 tokens**

BEFORE (lines 430-540):
```markdown
**Update CHANGELOG.md with enhanced format:**

Add new version entry at top with technical details:

```markdown
# Changelog
[...110 lines of template...]
```

**Sections to use:**
[...60 lines of explanation...]
```

AFTER (in SKILL.md):
```markdown
**Update CHANGELOG.md with enhanced format:**

See `references/changelog-format.md` for complete template structure.

**Quick reference:**
- PATCH: Use "Fixed" section
- MINOR: Use "Added"/"Changed" sections
- MAJOR: Include "Migration Notes" section
```

Create `references/changelog-format.md`:
```markdown
# Enhanced CHANGELOG Format

[Full template from lines 430-540]
```

#### 2. Lines 633-644: RegressionReport Interface → references/regression-testing.md
**Estimated savings: ~400 tokens**

Extract TypeScript interface and testing protocol details.

#### 3. Lines 799-823: Breaking Change Detection Logic → references/breaking-changes.md
**Estimated savings: ~600 tokens**

Move detection criteria and warning templates to reference file.

#### 4. Lines 846-871: Version Bump Logic → references/versioning.md
**Estimated savings: ~700 tokens**

Extract semantic versioning decision tree to dedicated reference.

### Content to Extract to assets/

#### 5. Lines 313-317: Backup Script Template → assets/backup-template.sh
**Estimated savings: ~150 tokens**

```bash
mkdir -p "backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/"
cp -r "plugins/${PLUGIN_NAME}/" "backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/"
```

#### 6. Lines 693-698: Rollback Script → assets/rollback-template.sh
**Estimated savings: ~200 tokens**

Extract complete rollback procedure.

### XML Tag Opportunities

#### 7. Lines 34-61: Wrap Precondition Checking in `<gate_preconditions>`
**Reason:** Block execution if plugin status invalid

BEFORE:
```markdown
## Precondition Checking

**Before starting, verify:**

1. Read PLUGINS.md:
[...]
```

AFTER:
```markdown
<gate_preconditions>
## Precondition Checking

**MUST execute before any other phase. BLOCK if conditions not met.**

1. Read PLUGINS.md:
[...]
</gate_preconditions>
```

#### 8. Lines 298-336: Wrap Backup Verification in `<critical_sequence>`
**Reason:** Ensure rollback capability before any code changes

BEFORE:
```markdown
## Phase 0.9: Backup Verification

**Goal:** Ensure rollback is possible if improvement fails
```

AFTER:
```markdown
<critical_sequence phase="backup-verification" enforcement="strict">
## Phase 0.9: Backup Verification

**Goal:** Ensure rollback is possible if improvement fails

**CRITICAL INVARIANT:** No implementation changes until backup verified.
**VIOLATION CONSEQUENCE:** Data loss, no rollback path.

[...content...]
</critical_sequence>
```

#### 9. Lines 598-699: Wrap Regression Testing in `<validation_gate>`
**Reason:** Block deployment if regressions detected without user override

BEFORE:
```markdown
## Phase 5.5: Regression Testing

**Check:** Does `.claude/skills/plugin-testing/SKILL.md` exist?
```

AFTER:
```markdown
<validation_gate gate="regression-tests" required="conditional">
## Phase 5.5: Regression Testing

**GATE CONDITION:** If plugin-testing skill exists AND baseline backup available
**GATE FAILURE:** Present rollback options, require user decision

[...content...]
</validation_gate>
```

#### 10. Lines 171-211: Wrap Research Detection in `<handoff_protocol>`
**Reason:** Explicit detection of deep-research handoff

BEFORE:
```markdown
## Phase 0.45: Research Detection

**BEFORE starting investigation, check conversation history for deep-research findings:**
```

AFTER:
```markdown
<handoff_protocol source="deep-research" target="plugin-improve">
## Phase 0.45: Research Detection

**HANDOFF DETECTION:** Scan for routing directive "Invoke plugin-improve skill"

**Signal patterns:**
- Routing directive: "Invoke plugin-improve skill" (signals deep-research handoff)
- Messages from deep-research skill
[...]

**If handoff detected:**
- Extract research findings (problem, root cause, solution, steps)
- Skip Phase 0.5 investigation
- Proceed to implementation approval

**If no handoff:**
- Proceed to Phase 0.5 (fresh investigation)
</handoff_protocol>
```

### Instruction Clarity Improvements

#### 11. Lines 19-32: Ambiguous Integration Description → Explicit Handoff Protocol
**Issue:** Verbose prose explanation instead of structured protocol

BEFORE (lines 19-32):
```markdown
**Integration with deep-research:**

This skill can receive pre-computed research findings from the deep-research skill. When user runs `/research` to investigate a problem and then chooses "Apply solution", deep-research outputs a directive that the main conversation (orchestrator) recognizes and acts on by invoking this skill. Phase 0.45 detects the research findings in conversation history and skips investigation, proceeding directly to implementation approval.

**Workflow:**
1. User: `/research [problem]` → deep-research investigates
[...7 more steps...]
```

AFTER:
```markdown
**Integration with deep-research:**

<handoff_protocol>
**Trigger:** deep-research outputs "Invoke plugin-improve skill"
**Detection:** Phase 0.45 scans conversation history
**Action:** Extract research findings, skip investigation (Phase 0.5)
**Benefits:** Preserve expensive research context (Opus + extended thinking)
</handoff_protocol>

See `references/handoff-protocols.md` for complete workflow.
```

#### 12. Lines 213-259: Tier Selection Criteria Need Explicit Decision Tree
**Issue:** Narrative description instead of clear criteria

BEFORE:
```markdown
**Tier Selection:**

- **Tier 1 (5-10 min):** Cosmetic changes, simple fixes, obvious issues
- **Tier 2 (15-30 min):** Logic errors, parameter issues, integration bugs
- **Tier 3 (30-60 min):** Complex bugs, performance issues, architectural problems
```

AFTER:
```markdown
**Tier Selection Decision Tree:**

```
Request analysis:
├─ Known pattern match? → Tier 1
├─ Single component issue? → Tier 2
├─ Multi-component / Performance / Architecture? → Tier 3
└─ Unclear? → Start Tier 1, escalate if needed
```

**Tier 1:** File read + pattern match (5-10 min)
**Tier 2:** Logic trace + integration check (15-30 min)
**Tier 3:** Invoke deep-research skill (30-60 min, uses Opus)
```

### Verbosity Reductions

#### 13. Lines 872-920: Duplicate Information → Consolidate with Phase 7 Section
**Current wordcount: ~450 words | Target: ~100 words**

Lines 872-920 ("Phase 7 Enhancements") duplicate information already present in earlier phases. This is commentary on the skill itself, not instructions.

CONDENSED VERSION:
```markdown
## Version History

**Phase 7 enhancements (2025-11):**
- Regression testing (Phase 5.5)
- Enhanced changelogs (Phase 4)
- Backup verification (Phase 0.9)
- Rollback mechanism (one-command restore)

**See:** `architecture/17-testing-strategy.md`, `scripts/verify-backup.sh`
```

SAVINGS: ~350 words (~1,400 tokens)

#### 14. Lines 1-13: Redundant Frontmatter Description → Use Trigger Terms Only
**Current: 65 words | Target: 20 words**

BEFORE:
```yaml
description: Fix bugs and add features with versioning, regression testing, and backups
```

AFTER:
```yaml
description: Fix bugs, add features to completed plugins. Includes versioning, backups, regression testing, changelog automation. Trigger terms - improve, fix, add feature, modify plugin, version bump, rollback
```

Adding trigger terms helps autonomous skill activation.

### Critical Sequence Enforcement

#### 15. Phase Sequence Enforcement (Lines 34-951)

The entire skill needs wrapped in nested `<critical_sequence>` tags:

```markdown
<critical_sequence name="plugin-improve-workflow" enforcement="strict">

<gate_preconditions>
[Lines 34-61: Precondition Checking]
</gate_preconditions>

<phase_sequence order="strict">

<phase id="0" name="specificity-detection">
[Lines 62-96: Phase 0]
</phase>

<phase id="0.3" name="clarification" condition="if_specific">
[Lines 97-151: Phase 0.3]
</phase>

<phase id="0.4" name="decision-gate" condition="after_clarification">
[Lines 152-169: Phase 0.4]
</phase>

<handoff_protocol source="deep-research" target="plugin-improve">
[Lines 171-211: Phase 0.45]
</handoff_protocol>

<decision_gate id="investigation" optional="true" condition="no_research_found">
[Lines 213-295: Phase 0.5]
</decision_gate>

<critical_sequence phase="backup-verification" enforcement="strict">
[Lines 298-336: Phase 0.9 - MUST complete before Phase 1]
**VIOLATION CONSEQUENCE:** Data loss, no rollback path
</critical_sequence>

<phase id="1" name="pre-implementation">
[Lines 338-387: Phase 1]
</phase>

<critical_sequence phase="backup-creation" enforcement="strict">
[Lines 389-404: Phase 2 - MUST complete before Phase 3]
**VIOLATION CONSEQUENCE:** No recovery if Phase 3 fails
</critical_sequence>

<phase id="3" name="implementation">
[Lines 407-422: Phase 3]
</phase>

<phase id="4" name="changelog">
[Lines 424-541: Phase 4]
</phase>

<delegation_rule target="build-automation" required="true">
[Lines 543-577: Phase 5 Build]
</delegation_rule>

<validation_gate gate="regression-tests" required="conditional">
[Lines 598-699: Phase 5.5 - Block if regressions found]
</validation_gate>

<phase id="6" name="git-workflow">
[Lines 700-737: Phase 6]
</phase>

<delegation_rule target="plugin-lifecycle" required="false">
[Lines 739-769: Phase 7 Installation]
</delegation_rule>

<checkpoint_protocol>
[Lines 773-794: Phase 8 Completion]
**MUST present numbered decision menu**
</checkpoint_protocol>

</phase_sequence>

</critical_sequence>
```

**Estimated effort:** 2 hours to restructure with proper XML tags.

## Examples of Issues Found

### Example 1: Critical Sequence Violation Risk

**BEFORE (Lines 298-387):**
```markdown
## Phase 0.9: Backup Verification

**Goal:** Ensure rollback is possible if improvement fails

**Check if backup exists:**

[...instructions...]

## Phase 1: Pre-Implementation Checks

**Load current state:**

1. Read CHANGELOG.md:
```

**Issue:** Nothing prevents Claude from reading Phase 1 instructions and executing them before Phase 0.9 completes. The phases are narrative sections, not enforced gates.

**AFTER:**
```markdown
<critical_sequence phase="backup-verification" enforcement="strict">
## Phase 0.9: Backup Verification

**CRITICAL INVARIANT:** Phase 1 MUST NOT execute until backup verified.
**ENFORCEMENT:** Block execution, halt workflow if backup fails.

[...instructions...]

**VERIFICATION CHECKPOINT:**
- ✓ Backup exists at backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/
- ✓ Backup integrity verified (verify-backup.sh passed)
- ✓ Rollback path confirmed

**ONLY AFTER VERIFICATION:** Proceed to Phase 1
</critical_sequence>

<phase id="1" name="pre-implementation" dependencies="backup-verification">
## Phase 1: Pre-Implementation Checks
```

**Impact:** High - Prevents data loss from failed improvements with no recovery path.

### Example 2: Decision Menu Format Inconsistency

**BEFORE (Lines 82-95):**
```markdown
**Assess specificity:**

- **Specific enough (1-2 clarification questions max):** Proceed to Phase 0.3 (4-question clarification batch)
- **Vague:** Present choice using AskUserQuestion:

```
Question:
  question: "Your request needs more detail. How should I proceed?"
  header: "Approach"
  options:
    - label: "Brainstorm approaches together", description: "I'll ask questions to explore options"
```

**Issue:** Uses AskUserQuestion tool, violates checkpoint protocol.

**AFTER:**
```markdown
**If request is vague, present inline decision menu:**

```
Your request needs more detail. How should I proceed?

1. Brainstorm approaches together - I'll ask questions to explore options
2. Implement something reasonable - I'll investigate and propose a solution
3. Other

Choose (1-3): _
```

**Handle responses:**
- Option 1 → Invoke plugin-ideation skill in improvement mode
- Option 2 → Proceed to Phase 0.45 (Research Detection)
- Option 3 → Collect free-form text, reassess
```

**Impact:** Medium - Creates inconsistent UX, violates system-wide pattern.

### Example 3: Missing Handoff Protocol Structure

**BEFORE (Lines 171-211):**
```markdown
## Phase 0.45: Research Detection

**BEFORE starting investigation, check conversation history for deep-research findings:**

**Scan recent messages for:**

- Routing directive: "Invoke plugin-improve skill" (signals deep-research handoff)
- Messages from deep-research skill
- Research reports (Level 1/2/3)
[...]

**If deep-research findings found:**

[...show findings...]

Wait for user approval, then proceed to Phase 0.9 (Backup Verification).

**If NO research findings found:**

Proceed to Phase 0.5 (Investigation) - perform fresh root cause analysis.
```

**Issue:** No XML structure to make handoff protocol explicit and scannable.

**AFTER:**
```markdown
<handoff_protocol source="deep-research" target="plugin-improve">
## Phase 0.45: Research Detection

**PROTOCOL:** Check for pre-computed research before starting investigation

<detection_signals>
- Routing directive: "Invoke plugin-improve skill"
- Skill attribution: Messages from deep-research skill
- Research artifacts: Level 1/2/3 reports
- Problem analysis: Root cause + recommended solution
</detection_signals>

<if_detected>
**Extract research findings:**
- Problem: [from research]
- Root Cause: [from research]
- Solution: [from research]
- Steps: [from research]

**Skip Phase 0.5 investigation** (research already complete)

**Present findings:**
```
✓ Research already completed (deep-research Level N)
[...extracted findings...]
Ready to implement? (y/n): _
```

**Wait for user approval → Proceed to Phase 0.9**
</if_detected>

<if_not_detected>
**Proceed to Phase 0.5** (Investigation) - perform fresh root cause analysis
</if_not_detected>

</handoff_protocol>
```

**Impact:** Medium - Improves clarity of handoff protocol, makes conditional logic explicit.

## Estimated Impact

### Context Reduction: ~8,000 tokens (33% reduction)

**Breakdown:**
- Changelog template extraction: 2,500 tokens
- Version bump logic extraction: 700 tokens
- Breaking change detection extraction: 600 tokens
- Regression testing interface extraction: 400 tokens
- Backup/rollback scripts extraction: 350 tokens
- Phase 7 commentary removal: 1,400 tokens
- Verbosity reductions: 800 tokens
- XML tag overhead: +300 tokens (but improves clarity)
- Miscellaneous: 1,250 tokens

**Remaining size:** ~16,500 tokens (still large but focused on orchestration)

### Comprehension Improvement: **High**

**Reasons:**
1. Critical sequences explicitly enforced (prevents out-of-order execution)
2. Handoff protocols structured (clear detection and routing)
3. Decision gates explicit (no ambiguity on blocking conditions)
4. Templates extracted (SKILL.md becomes pure orchestration logic)
5. Consistent checkpoint format (inline numbered menus throughout)

**Failure modes prevented:**
- Skipping backup verification before implementation
- Skipping backup creation before code changes
- Proceeding past regression failures without user decision
- Inconsistent decision menu UX

### Maintenance Improvement: **High**

**Reasons:**
1. Templates in references/ can be updated independently
2. Versioning logic externalized (easy to modify semver rules)
3. Changelog format versioned separately
4. XML tags make phase dependencies explicit
5. Reduced duplication (single source of truth for templates)

**Benefits:**
- Update changelog format without touching orchestration
- Modify regression testing protocol in one place
- Add new breaking change detection rules easily
- Clear separation: orchestration vs. templates vs. documentation

## Priority Recommendations

### 1. ADD CRITICAL SEQUENCE ENFORCEMENT (HIGHEST PRIORITY)
**Estimated time: 2 hours**

Wrap all phases in XML tags with explicit enforcement:
- `<gate_preconditions>` for status checking (lines 34-61)
- `<critical_sequence>` for backup verification (lines 298-336)
- `<critical_sequence>` for backup creation (lines 389-404)
- `<validation_gate>` for regression testing (lines 598-699)
- `<handoff_protocol>` for deep-research integration (lines 171-211)
- `<checkpoint_protocol>` for completion menu (lines 773-794)

**Why critical:** The skill's core value is safe versioned improvements. Without enforcement, Claude can skip backup creation/verification, violating the safety guarantee.

**Files to create:** None (inline XML tags)

**Verification:** Test that Claude cannot proceed past backup verification without passing the gate.

### 2. EXTRACT LARGE TEMPLATES TO REFERENCES/ (HIGH PRIORITY)
**Estimated time: 1 hour**

Create four new reference files:
- `references/changelog-format.md` (lines 430-540 → extract)
- `references/versioning.md` (lines 846-871 → extract)
- `references/breaking-changes.md` (lines 799-823 → extract)
- `references/regression-testing.md` (lines 633-644 → extract)

**Why important:** Reduces context by 33%, makes SKILL.md scannable, allows independent template updates.

**Token savings:** ~4,200 tokens

**Files to create:**
- `references/changelog-format.md`
- `references/versioning.md`
- `references/breaking-changes.md`
- `references/regression-testing.md`

**Update SKILL.md:** Replace extracted sections with references (e.g., "See references/changelog-format.md")

### 3. FIX DECISION MENU INCONSISTENCY (MEDIUM PRIORITY)
**Estimated time: 30 minutes**

Convert all `AskUserQuestion` calls to inline numbered menus:
- Lines 82-95: Phase 0 Specificity Detection
- Lines 99-169: Phase 0.3 Clarification Questions (4 questions)
- Lines 154-169: Phase 0.4 Decision Gate

**Why important:** Violates system-wide checkpoint protocol documented in CLAUDE.md. Creates inconsistent UX across all skills.

**Format:**
```markdown
[Question text]

1. [Option 1] - [Description]
2. [Option 2] - [Description]
3. Other

Choose (1-3): _
```

**Affected lines:** 82-169

**Verification:** Ensure all user decision points use inline numbered format, no AskUserQuestion tool calls remain.

## Implementation Notes

### Files that would be created in references/:
1. `changelog-format.md` - Enhanced changelog template structure (110 lines)
2. `versioning.md` - Semantic versioning decision tree (30 lines)
3. `breaking-changes.md` - Breaking change detection criteria (25 lines)
4. `regression-testing.md` - RegressionReport interface and comparison protocol (40 lines)
5. `handoff-protocols.md` - Complete deep-research → plugin-improve workflow (25 lines)

### Files that would be created in assets/:
1. `backup-template.sh` - Backup creation script (10 lines)
2. `rollback-template.sh` - Complete rollback procedure (15 lines)

### Estimated time to implement all fixes:
**3.5 hours total:**
- Critical sequence enforcement: 2 hours (most complex, requires careful XML nesting)
- Template extraction: 1 hour (create 5 reference files, update SKILL.md references)
- Decision menu conversion: 30 minutes (straightforward find-replace with format change)

### Implementation order:
1. **First:** Extract templates (safer, independent changes)
2. **Second:** Fix decision menu format (affects UX, no logic changes)
3. **Third:** Add XML enforcement tags (most impactful, requires testing)

This order minimizes risk: Templates and menus can be fixed without affecting orchestration logic, then XML tags added last to ensure proper enforcement.
