# Fix Plan: context-resume

## Summary
- Critical fixes: **7**
- Extraction operations: **0** (already optimal)
- Total estimated changes: **7 operations**
- Estimated time: **45 minutes**

---

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Add Orchestration Mode Delegation Rule
**Location:** After line 27 in SKILL.md
**Operation:** INSERT
**Change:**
```markdown

## Orchestration Protocol

<delegation_rule type="mandatory" enforcement="strict">

**CRITICAL:** This skill MUST NOT implement workflow stages directly.

When resuming workflow (Stages 0-6), this skill:
1. Locates handoff file
2. Parses context
3. Presents summary to user
4. Checks for `orchestration_mode: true` in handoff YAML
5. If orchestration_mode enabled → Invokes plugin-workflow skill via Skill tool
6. If orchestration_mode disabled → Uses legacy direct routing

**NEVER bypass orchestration_mode.** This enforces the dispatcher pattern:
- plugin-workflow orchestrates
- Subagents implement
- context-resume just loads and routes

See **[references/continuation-routing.md](references/continuation-routing.md)** Step 4a-1 for complete protocol.

</delegation_rule>
```
**Verification:** Check that `<delegation_rule>` tag appears after line 27 and before "## Handoff File Locations"

---

### Fix 1.2: Wrap Resume Workflow in Critical Sequence Tags
**Location:** Lines 54-83 in SKILL.md
**Operation:** REPLACE
**Before:**
```markdown
## Resume Workflow

The complete resume process is documented in detail in the reference files:

### Step 1: Locate Handoff
Search for handoff files across 3 locations (priority order). Handle interactive plugin selection if no name provided. Disambiguate when multiple handoffs exist for same plugin.

See **[references/handoff-location.md](references/handoff-location.md)** for complete location logic.

### Step 2 & 3: Parse Context and Present Summary
Parse YAML frontmatter (plugin, stage, status, last_updated, etc.) and markdown body (current state, completed work, next steps, key decisions). Calculate "time ago" and build user-facing summary.

See **[references/context-parsing.md](references/context-parsing.md)** for parsing and presentation logic.

### Step 4: Proceed with Continuation
Determine routing based on stage type (workflow, ideation, mockup, improvement). Load relevant context files (contracts, source code, git history). Invoke appropriate continuation skill.

See **[references/continuation-routing.md](references/continuation-routing.md)** for routing logic.
```
**After:**
```markdown
## Resume Workflow

<critical_sequence enforcement="strict" allow_backtrack="false">

<sequence_step number="1" name="locate_handoff" blocking="true">
Search for handoff files across 3 locations, handle interactive plugin selection if no name provided, and disambiguate when multiple handoffs exist for same plugin.

See **[references/handoff-location.md](references/handoff-location.md)** for complete location logic.

MUST complete before Step 2. If no handoff found, see error recovery.
</sequence_step>

<sequence_step number="2" name="parse_context" blocking="true" depends_on="1">
Parse YAML frontmatter (plugin, stage, status, last_updated, etc.) and markdown body (current state, completed work, next steps, key decisions).

See **[references/context-parsing.md](references/context-parsing.md)** for parsing logic.

MUST complete before Step 3.
</sequence_step>

<sequence_step number="3" name="present_summary" blocking="true" depends_on="2">
Calculate "time ago" and build user-facing summary showing where we are in workflow, what's completed, what's next, build/test status, time since last session.

See **[references/context-parsing.md](references/context-parsing.md)** for presentation logic.

<decision_gate>
MUST wait for user confirmation. DO NOT auto-proceed.
Present numbered decision menu following checkpoint protocol.
</decision_gate>
</sequence_step>

<sequence_step number="4" name="proceed_continuation" blocking="true" depends_on="3" requires_user_confirmation="true">
Determine routing based on stage type (workflow, ideation, mockup, improvement), load relevant context files (contracts, source code, git history), and invoke appropriate continuation skill.

See **[references/continuation-routing.md](references/continuation-routing.md)** for routing logic.
</sequence_step>

</critical_sequence>
```
**Verification:** Confirm `<critical_sequence>` wrapper with 4 `<sequence_step>` elements, each with explicit dependencies and blocking attributes. Verify `<decision_gate>` appears in Step 3.

---

### Fix 1.3: Add Handoff Protocol Wrapper
**Location:** Lines 29-51 in SKILL.md
**Operation:** WRAP
**Before:**
```markdown
## Handoff File Locations

### 1. Main workflow handoff
**Location:** `plugins/[PluginName]/.continue-here.md`
**Meaning:** Plugin is in active development (Stages 0-6)
**Created by:** plugin-workflow skill
**Contains:** Stage number, phase (if complex), completed work, next steps

### 2. Ideation handoff
**Location:** `plugins/[PluginName]/.ideas/.continue-here.md`
**Meaning:** Plugin is in planning/ideation phase
**Created by:** plugin-ideation skill
**Contains:** Creative brief status, mockup status, ready-to-implement flag

### 3. Mockup handoff
**Location:** `plugins/[PluginName]/.ideas/mockups/.continue-here.md`
**Meaning:** UI mockup iteration is in progress
**Created by:** ui-mockup skill
**Contains:** Mockup version, iteration notes, finalization status
```
**After:**
```markdown
<handoff_protocol>

## Handoff File Locations

The system uses 3 handoff locations, checked in priority order:

<handoff_location priority="1" type="workflow">
**Path:** `plugins/[PluginName]/.continue-here.md`
**Meaning:** Plugin in active development (Stages 0-6)
**Creator:** plugin-workflow skill
**Contains:** stage, phase, orchestration_mode, next_action, completed work, next steps
</handoff_location>

<handoff_location priority="2" type="ideation">
**Path:** `plugins/[PluginName]/.ideas/.continue-here.md`
**Meaning:** Plugin in planning/ideation phase
**Creator:** plugin-ideation skill
**Contains:** Creative brief status, mockup status, ready-to-implement flag
</handoff_location>

<handoff_location priority="3" type="mockup">
**Path:** `plugins/[PluginName]/.ideas/mockups/.continue-here.md`
**Meaning:** UI mockup iteration in progress
**Creator:** ui-mockup skill
**Contains:** mockup_version, iteration notes, finalization status
</handoff_location>

**Search order:** Priority 1 → 2 → 3. If multiple found, disambiguate (see references/handoff-location.md).

</handoff_protocol>
```
**Verification:** Confirm `<handoff_protocol>` wrapper and 3 `<handoff_location>` elements with priority attributes. Check search order note added.

---

### Fix 1.4: Convert Informal Notes to Formal Requirements
**Location:** Lines 144-164 in SKILL.md
**Operation:** REPLACE
**Before:**
```markdown
## Notes for Claude

**When executing this skill:**

1. Always search all 3 handoff locations before declaring "not found"
2. Parse YAML carefully - handle missing optional fields gracefully
3. Present time-ago in human-readable format (not raw timestamps)
4. Show enough context that user remembers where they were
5. Do not auto-proceed - wait for explicit user choice
6. Load contract files before invoking continuation skill (provides context)
7. Use git log as backup if handoff is stale/corrupt
8. Preserve user's mental model - summary should match how they think about plugin
```
**After:**
```markdown
## Execution Requirements

<requirements enforcement="strict">

**MUST do when executing this skill:**

1. **ALWAYS** search all 3 handoff locations before declaring "not found"
2. **MUST** parse YAML carefully - handle missing optional fields gracefully
3. **MUST** present time-ago in human-readable format (not raw timestamps)
4. **MUST** show enough context that user remembers where they were
5. **NEVER** auto-proceed - wait for explicit user choice
6. **MUST** load contract files BEFORE invoking continuation skill (provides context)
7. **MUST** use git log as backup if handoff is stale/corrupt
8. **MUST** preserve user's mental model - summary should match how they think about plugin

</requirements>

<anti_patterns>

**NEVER do these common pitfalls:**

- Forgetting to check all 3 locations
- Auto-proceeding without user confirmation
- Not loading contract files before continuation
- Showing raw YAML instead of human summary
- Missing disambiguation when multiple handoffs exist

</anti_patterns>
```
**Verification:** Check section renamed to "Execution Requirements" with `<requirements>` and `<anti_patterns>` tags. Confirm all items use imperative directives (MUST/NEVER/ALWAYS).

---

### Fix 1.5: Add State Requirement Wrapper
**Location:** Lines 114-125 in SKILL.md
**Operation:** REPLACE
**Before:**
```markdown
**Updates:**

- Nothing directly (just reads and routes)
- Continuation skills will update handoff files as they proceed
```
**After:**
```markdown
<state_requirement type="read_only" enforcement="strict">

**This skill is READ-ONLY for state files.**

MUST read:
- `.continue-here.md` files (all 3 locations)
- PLUGINS.md (status verification)
- Git log (commit history for inference)
- Contract files (creative-brief.md, parameter-spec.md, architecture.md, plan.md)
- Source files (if mentioned in handoff)
- CHANGELOG.md (for improvements)

MUST NOT write:
- Any `.continue-here.md` files
- PLUGINS.md
- Any source code or contract files

Continuation skills invoked by this skill will handle all state updates.

</state_requirement>
```
**Verification:** Confirm `<state_requirement>` tag with type="read_only". Check MUST read and MUST NOT write lists present.

---

### Fix 1.6: Restructure Integration Points
**Location:** Lines 100-112 in SKILL.md
**Operation:** REPLACE
**Before:**
```markdown
**Invoked by:**

- `/continue` command (no args → interactive plugin selection)
- `/continue [PluginName]` command (specific plugin)
- Natural language: "resume [PluginName]", "continue working on [PluginName]"

**Invokes:**

- `plugin-workflow` (workflow resume at specific stage)
- `plugin-ideation` (ideation resume for improvements)
- `ui-mockup` (mockup iteration resume)
- `plugin-improve` (improvement implementation resume)
```
**After:**
```markdown
## Integration Points

<integration type="inbound" trigger="command">

**Invoked by:**

1. `/continue` command (no args) → Triggers interactive plugin selection
2. `/continue [PluginName]` command → Triggers specific plugin resume
3. Natural language: "resume [PluginName]", "continue working on [PluginName]"

</integration>

<integration type="outbound" action="delegation">

**Invokes (via Skill tool):**

1. `plugin-workflow` - For workflow resume at specific stage (Stages 0-6)
2. `plugin-ideation` - For ideation resume (improvements or refinement)
3. `ui-mockup` - For mockup iteration resume
4. `plugin-improve` - For improvement implementation resume

MUST use Skill tool for invocation, NEVER implement directly.

</integration>
```
**Verification:** Check `<integration>` tags with type/trigger/action attributes. Confirm numbered lists and delegation requirement emphasized.

---

### Fix 1.7: Reduce Overview Verbosity
**Location:** Lines 14-27 in SKILL.md
**Operation:** REPLACE
**Before:**
```markdown
**Purpose:** Load plugin development context from `.continue-here.md` handoff files to enable pause/resume across sessions. Creates continuity between work sessions by preserving and restoring complete plugin development state.

## Overview

The handoff system allows plugins to be paused at any point and resumed later with full context preservation. This skill is the universal entry point for resuming any type of work: implementation, ideation, UI mockup iteration, or improvements.

**Key capabilities:**

- Locates handoff files across 3 possible locations
- Parses YAML frontmatter and markdown body for structured context
- Summarizes current state and progress
- Loads relevant contract files and source code
- Routes to appropriate continuation skill
- Handles missing or corrupted handoff files gracefully
```
**After:**
```markdown
**Purpose:** Universal entry point for resuming plugin work from `.continue-here.md` handoff files. Handles workflow, ideation, mockup, and improvement resume scenarios.

**Capabilities:**
- Locates handoff files (3 possible locations)
- Parses YAML frontmatter and markdown context
- Presents state summary with time-ago calculation
- Routes to appropriate continuation skill (via Skill tool)
- Graceful error recovery for missing/corrupt handoffs
```
**Verification:** Confirm "Overview" section removed and Purpose condensed to 2 paragraphs. Check token count reduction (~60 tokens).

---

## Phase 2: Content Extraction (Context Reduction)

**No extractions needed.** Content extraction is already optimal. All large blocks (>30 lines) properly extracted to references/ directory.

---

## Phase 3: Polish (Clarity & Verbosity)

**All polish items completed in Phase 1.**

The Phase 1 fixes address both critical enforcement issues AND clarity/verbosity improvements simultaneously:
- Fix 1.4 (Requirements) improves clarity
- Fix 1.7 (Overview) reduces verbosity
- Fixes 1.2, 1.3, 1.5, 1.6 improve both enforcement AND clarity

---

## File Creation Manifest

**No new files created.**

All changes are modifications to existing files:
- `.claude/skills/context-resume/SKILL.md` (primary changes)
- `.claude/skills/context-resume/references/continuation-routing.md` (minor forward reference addition)

---

## Execution Checklist

### Phase 1 Critical Fixes
- [ ] Fix 1.1: Orchestration delegation rule added after line 27
- [ ] Fix 1.2: Resume workflow wrapped in `<critical_sequence>` tags
- [ ] Fix 1.3: Handoff locations wrapped in `<handoff_protocol>` tags
- [ ] Fix 1.4: "Notes for Claude" converted to `<requirements>` and `<anti_patterns>`
- [ ] Fix 1.5: State updates wrapped in `<state_requirement>` tag
- [ ] Fix 1.6: Integration points restructured with `<integration>` tags
- [ ] Fix 1.7: Overview section condensed for verbosity reduction

### Verification Tests
- [ ] All 4 sequence steps have `blocking="true"` attribute
- [ ] Step 3 contains `<decision_gate>` preventing auto-proceed
- [ ] Step 4 has `requires_user_confirmation="true"` attribute
- [ ] Orchestration delegation rule visible in main SKILL.md (not buried)
- [ ] All requirements use imperative directives (MUST/NEVER/ALWAYS)
- [ ] State requirement clearly separates MUST read vs. MUST NOT write
- [ ] Integration points emphasize Skill tool requirement
- [ ] Token count reduced by ~60 tokens in overview section
- [ ] XML tags properly closed and nested
- [ ] Line numbers in references match after insertions

### Final Validation
- [ ] Skill health score improved from Yellow to Green
- [ ] Critical sequence enforcement score: 1/5 → 5/5
- [ ] XML organization score: 2/5 → 5/5
- [ ] System integration score: 3/5 → 5/5
- [ ] No broken markdown formatting
- [ ] All internal references still valid
- [ ] YAML frontmatter unchanged

---

## Post-Fix Reference Update

### Minor Change: references/continuation-routing.md
**Location:** Step 4a-1 (around line 78-82)
**Operation:** INSERT
**Add forward reference:**
```markdown
**NOTE:** The orchestration mode protocol is defined as a top-level delegation rule in the main SKILL.md. See "Orchestration Protocol" section for the primary specification.
```
**Verification:** Check forward reference added without disrupting step logic.

---

## Estimated Impact Summary

### Before Fix
- Overall health: **Yellow**
- Critical sequence enforcement: **1/5**
- XML organization: **2/5**
- System integration: **3/5**
- Critical issues: **3**

### After Fix
- Overall health: **Green** (estimated)
- Critical sequence enforcement: **5/5**
- XML organization: **5/5**
- System integration: **5/5**
- Critical issues: **0**

### Time Breakdown
- Fix 1.1 (Orchestration rule): 10 min
- Fix 1.2 (Critical sequence): 15 min
- Fix 1.3 (Handoff protocol): 5 min
- Fix 1.4 (Requirements): 8 min
- Fix 1.5 (State requirement): 5 min
- Fix 1.6 (Integration points): 5 min
- Fix 1.7 (Overview verbosity): 2 min
- Reference update: 2 min
- **Total: 52 minutes** (includes buffer for testing)

### Token Impact
- Verbosity reduction: -60 tokens
- XML tag additions: +500 tokens
- **Net change: +440 tokens**

**Note:** Token count increases slightly, but error prevention and comprehension clarity far outweigh the cost. This aligns with prompt engineering principle: reliability > minimal token count.

---

## Anti-Pattern Prevention

These fixes prevent the following common failure modes:

1. **Sequence violations** - Claude skipping steps or executing out of order
2. **Auto-proceed bugs** - Claude continuing without user confirmation
3. **Direct implementation** - Claude implementing instead of delegating to subagents
4. **State mutations** - Claude writing to read-only state files
5. **Dispatcher bypass** - Claude ignoring orchestration_mode flag
6. **Missing disambiguation** - Claude not handling multiple handoffs properly
7. **Integration confusion** - Claude unclear about tool invocation requirements

All 7 anti-patterns are now blocked by XML enforcement tags and imperative directives.
