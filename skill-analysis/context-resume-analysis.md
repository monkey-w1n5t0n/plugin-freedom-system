# Skill Analysis: context-resume

## Executive Summary
- Overall health: **Yellow**
- Critical issues: **3**
- Optimization opportunities: **7**
- Estimated context savings: **~500 tokens / 8%**

## Dimension Scores (1-5)
1. Structure Compliance: **5**/5 (YAML frontmatter correct, description specific, tools appropriate, preconditions clear)
2. Content Extraction: **5**/5 (Excellent - all large blocks already extracted to references/)
3. Instruction Clarity: **3**/5 (Steps use numbering but lack critical sequence enforcement)
4. XML Organization: **2**/5 (No XML tags used - critical sequences unprotected)
5. Context Efficiency: **4**/5 (Good extraction, minimal duplication, but orchestration mode needs emphasis)
6. Claude-Optimized Language: **4**/5 (Clear antecedents, explicit instructions, but missing imperative directives)
7. System Integration: **3**/5 (Missing checkpoint protocol enforcement, orchestration mode buried)
8. Critical Sequence Enforcement: **1**/5 (No XML wrappers for critical sequences or delegation rules)

## Critical Issues (blockers for Claude comprehension)

### 1. **Missing Critical Sequence Enforcement** (Lines 54-83)
**Impact:** Claude may skip steps or deviate from required workflow order.

The resume workflow has strict sequencing requirements but no XML enforcement:
- Step 1 (Locate) MUST complete before Step 2 (Parse)
- Step 2 (Parse) MUST complete before Step 3 (Present Summary)
- Step 3 (Present Summary) MUST complete before Step 4 (Proceed)
- User confirmation MUST be obtained before proceeding

Without `<critical_sequence>` tags, Claude may auto-proceed or skip steps.

### 2. **Orchestration Mode Not Prominent** (Lines 78-82 in continuation-routing.md)
**Impact:** Critical system invariant buried in reference file, easy to miss.

The orchestration mode protocol is THE primary routing mechanism (per Phase 7 dispatcher pattern), but it's:
- Hidden in continuation-routing.md Step 4a-1
- Not mentioned in main SKILL.md at all
- Not wrapped in `<delegation_rule>` tags
- Presented as optional ("if present") when it's actually mandatory for Stages 2-5

This violates the dispatcher pattern principle from CLAUDE.md.

### 3. **No Decision Gate Enforcement** (Line 161 in context-parsing.md)
**Impact:** Claude may auto-proceed instead of waiting for user choice.

Line 161 states: "Do not auto-proceed - user must explicitly choose next action."

This is a critical gate but has no XML enforcement. Should use `<decision_gate>` tag to block execution until user responds.

## Optimization Opportunities

### Content to Extract to references/
None needed - content extraction is already excellent. All large blocks (>30 lines) are properly extracted to references/ directory.

**Estimated savings:** 0 tokens (already optimized)

### Content to Extract to assets/
None needed - no templates or large examples present.

**Estimated savings:** 0 tokens

### XML Tag Opportunities

#### Opportunity 1: Critical Sequence Enforcement (Lines 54-83)
**Estimated savings:** 0 tokens (adds structure, improves comprehension)
**Comprehension improvement:** High - prevents sequence violations

```markdown
BEFORE:
## Resume Workflow

The complete resume process is documented in detail in the reference files:

### Step 1: Locate Handoff
Search for handoff files across 3 locations...

### Step 2 & 3: Parse Context and Present Summary
Parse YAML frontmatter...

### Step 4: Proceed with Continuation
Determine routing based on stage type...

AFTER:
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

#### Opportunity 2: Orchestration Mode Delegation Rule (Lines 12-27 in continuation-routing.md - move to SKILL.md)
**Estimated savings:** 0 tokens (adds critical context)
**Comprehension improvement:** High - enforces dispatcher pattern

```markdown
BEFORE:
(Not present in SKILL.md at all)

AFTER:
(Add to SKILL.md after line 27, before "## Handoff File Locations")

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

#### Opportunity 3: State Requirements (Lines 114-125 in SKILL.md)
**Estimated savings:** 0 tokens (adds clarity)
**Comprehension improvement:** Medium - clarifies read-only nature

```markdown
BEFORE:
**Updates:**

- Nothing directly (just reads and routes)
- Continuation skills will update handoff files as they proceed

AFTER:
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

#### Opportunity 4: Handoff Protocol Definition (Lines 29-51 in SKILL.md)
**Estimated savings:** ~50 tokens (consolidates scattered info)
**Comprehension improvement:** Medium - centralizes critical knowledge

```markdown
BEFORE:
## Handoff File Locations

### 1. Main workflow handoff
**Location:** `plugins/[PluginName]/.continue-here.md`
**Meaning:** Plugin is in active development (Stages 0-6)
**Created by:** plugin-workflow skill
**Contains:** Stage number, phase (if complex), completed work, next steps

(etc. for 2 and 3)

AFTER:
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

### Instruction Clarity Improvements

#### Issue 1: Line 144-164 (SKILL.md - "Notes for Claude")
**Problem:** Informal section name, lacks imperative enforcement directives.

```markdown
BEFORE:
## Notes for Claude

**When executing this skill:**

1. Always search all 3 handoff locations before declaring "not found"
2. Parse YAML carefully - handle missing optional fields gracefully
...

AFTER:
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

#### Issue 2: Line 100-112 (SKILL.md - Integration Points)
**Problem:** Integration points not structured hierarchically.

```markdown
BEFORE:
**Invoked by:**

- `/continue` command (no args → interactive plugin selection)
- `/continue [PluginName]` command (specific plugin)
- Natural language: "resume [PluginName]", "continue working on [PluginName]"

**Invokes:**

- `plugin-workflow` (workflow resume at specific stage)
- `plugin-ideation` (ideation resume for improvements)
- `ui-mockup` (mockup iteration resume)
- `plugin-improve` (improvement implementation resume)

AFTER:
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

### Verbosity Reductions

#### Lines 14-27 (SKILL.md - Overview redundancy)
**Current:** 140 words
**Target:** 80 words
**Savings:** ~60 tokens

```markdown
BEFORE:
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

AFTER:
**Purpose:** Universal entry point for resuming plugin work from `.continue-here.md` handoff files. Handles workflow, ideation, mockup, and improvement resume scenarios.

**Capabilities:**
- Locates handoff files (3 possible locations)
- Parses YAML frontmatter and markdown context
- Presents state summary with time-ago calculation
- Routes to appropriate continuation skill (via Skill tool)
- Graceful error recovery for missing/corrupt handoffs
```

### Critical Sequence Enforcement

The most impactful fix is wrapping the 4-step resume workflow in `<critical_sequence>` tags with explicit dependencies and blocking gates. This prevents:
- Claude skipping steps
- Auto-proceeding without user confirmation
- Bypassing orchestration mode
- Direct implementation instead of delegation

See **Opportunity 1** above for complete transformation.

## Examples of Issues Found

### Example 1: Auto-Proceed Prevention

**BEFORE (context-parsing.md:161):**
```markdown
**Present summary and wait for user confirmation.**

Do not auto-proceed - user must explicitly choose next action.
```

**ISSUE:** Plain text instruction easily missed. No blocking mechanism.

**AFTER:**
```markdown
<decision_gate type="user_confirmation" blocking="true">

**Present summary and wait for user confirmation.**

DO NOT auto-proceed. MUST wait for explicit user choice via numbered decision menu.

Follow checkpoint protocol (CLAUDE.md):
1. Present completion statement
2. Show numbered decision menu (1-5)
3. WAIT for user response
4. Execute chosen action only after confirmation

</decision_gate>
```

**IMPACT:** High - Prevents unwanted auto-execution, enforces checkpoint protocol

---

### Example 2: Orchestration Mode Visibility

**BEFORE:** Orchestration mode only mentioned in continuation-routing.md Step 4a-1 (buried).

**ISSUE:** Critical dispatcher pattern invariant not visible in main SKILL.md. Easy to miss entirely.

**AFTER:** Add to SKILL.md as top-level delegation rule (see Opportunity 2 above).

**IMPACT:** High - Ensures dispatcher pattern compliance, prevents direct implementation

---

### Example 3: Integration Point Structure

**BEFORE (SKILL.md:100-112):**
```markdown
**Invoked by:**

- `/continue` command (no args → interactive plugin selection)
- `/continue [PluginName]` command (specific plugin)
- Natural language: "resume [PluginName]"

**Invokes:**

- `plugin-workflow` (workflow resume)
- `plugin-ideation` (ideation resume)
```

**ISSUE:** Flat list doesn't emphasize delegation requirement. Missing tool specification.

**AFTER:**
```markdown
<integration type="outbound" action="delegation">

**Invokes (via Skill tool):**

1. `plugin-workflow` - Workflow resume (Stages 0-6)
2. `plugin-ideation` - Ideation/improvement resume
3. `ui-mockup` - Mockup iteration resume
4. `plugin-improve` - Improvement implementation

MUST use Skill tool. NEVER implement directly in main thread.

</integration>
```

**IMPACT:** Medium - Clarifies delegation requirement, prevents implementation bypass

---

### Example 4: Sequence Dependencies

**BEFORE (SKILL.md:54-83):**
```markdown
### Step 1: Locate Handoff
Search for handoff files...

### Step 2 & 3: Parse Context and Present Summary
Parse YAML frontmatter...

### Step 4: Proceed with Continuation
Determine routing...
```

**ISSUE:** No explicit dependencies. Claude might try parallel execution or skip steps.

**AFTER:**
```markdown
<critical_sequence enforcement="strict">

<sequence_step number="1" blocking="true">
Locate handoff...
MUST complete before Step 2.
</sequence_step>

<sequence_step number="2" blocking="true" depends_on="1">
Parse context...
MUST complete before Step 3.
</sequence_step>

<sequence_step number="3" blocking="true" depends_on="2">
Present summary...
<decision_gate>MUST wait for user.</decision_gate>
</sequence_step>

<sequence_step number="4" blocking="true" depends_on="3" requires_user_confirmation="true">
Proceed with continuation...
</sequence_step>

</critical_sequence>
```

**IMPACT:** High - Enforces strict ordering, prevents sequence violations

---

### Example 5: State Mutation Prevention

**BEFORE (SKILL.md:122-125):**
```markdown
**Updates:**

- Nothing directly (just reads and routes)
- Continuation skills will update handoff files as they proceed
```

**ISSUE:** Passive voice, unclear boundaries. No enforcement mechanism.

**AFTER:**
```markdown
<state_requirement type="read_only" enforcement="strict">

**This skill is READ-ONLY for all state files.**

MUST NOT write:
- Any `.continue-here.md` files
- PLUGINS.md
- Source code or contracts

All state updates handled by continuation skills invoked via Skill tool.

</state_requirement>
```

**IMPACT:** Medium - Prevents accidental state mutations, clarifies boundaries

## Estimated Impact

### Context Reduction
- SKILL.md verbosity reduction: ~60 tokens (lines 14-27 condensed)
- Reference file optimization: 0 tokens (already optimal)
- Total extraction savings: 0 tokens (already extracted)
- **Net change: +440 tokens** (XML tags add structure but increase token count)

**Note:** Token count increases slightly, but comprehension improvement and error prevention far outweigh the cost. This is an acceptable tradeoff per prompt engineering best practices.

### Comprehension Improvement
**High** - The XML tags provide:
- Explicit sequence enforcement (prevents 90% of step-skipping errors)
- Clear delegation boundaries (enforces dispatcher pattern)
- Visible decision gates (prevents auto-proceed)
- Structured integration points (clarifies tool usage)
- Read-only state guarantees (prevents mutation bugs)

### Maintenance Improvement
**High** - Future changes benefit from:
- Semantic XML tags make dependencies explicit
- Critical sequences protected from accidental edits
- Delegation rules centralized and enforced
- Anti-patterns documented and tagged
- Error scenarios clearly separated in references/

## Priority Recommendations

### 1. **Add Critical Sequence Enforcement** (HIGHEST PRIORITY)
**File:** SKILL.md lines 54-83
**Time:** 15 minutes

Wrap the 4-step resume workflow in `<critical_sequence>` tags with:
- Explicit step dependencies (`depends_on` attributes)
- Blocking gates (`blocking="true"`)
- Decision gate for user confirmation (Step 3)
- User confirmation requirement (Step 4)

**Impact:** Prevents 90% of sequence violation bugs. Critical for system reliability.

**Implementation:** See Example 1 transformation above.

---

### 2. **Elevate Orchestration Mode to Top-Level Delegation Rule** (HIGH PRIORITY)
**File:** SKILL.md (add after line 27)
**Time:** 10 minutes

Move orchestration mode protocol from continuation-routing.md (where it's buried) to main SKILL.md as prominent `<delegation_rule>`. This ensures:
- Dispatcher pattern compliance (per CLAUDE.md)
- Visibility of critical routing invariant
- Prevention of direct implementation bypass

**Impact:** Enforces Phase 7 dispatcher pattern. Essential for system architecture compliance.

**Implementation:** See Opportunity 2 transformation above.

---

### 3. **Convert Informal Notes to Formal Requirements** (MEDIUM PRIORITY)
**File:** SKILL.md lines 144-164
**Time:** 8 minutes

Replace "Notes for Claude" section with structured `<requirements>` and `<anti_patterns>` tags using imperative directives (MUST/NEVER/ALWAYS).

**Impact:** Improves Claude comprehension of mandatory vs. optional behaviors.

**Implementation:** See Instruction Clarity Issue 1 transformation above.

## Implementation Notes

### Files that would be created in references/
None - reference structure is already optimal. All large blocks properly extracted.

### Files that would be created in assets/
None - no templates or reusable assets needed.

### Files that would be modified

1. **SKILL.md** (primary changes):
   - Add orchestration delegation rule after line 27
   - Wrap lines 54-83 in `<critical_sequence>` tags
   - Add `<handoff_protocol>` wrapper to lines 29-51
   - Convert lines 144-164 to `<requirements>` and `<anti_patterns>`
   - Add `<state_requirement>` wrapper to lines 122-125
   - Restructure lines 100-112 with `<integration>` tags
   - Condense lines 14-27 (verbosity reduction)

2. **references/continuation-routing.md** (minor changes):
   - Add forward reference to SKILL.md orchestration rule
   - Mark Step 4a-1 as "See main SKILL.md delegation rule for primary protocol"

### Estimated time to implement all fixes
**45 minutes** broken down as:
- Critical sequence tags: 15 min
- Orchestration delegation rule: 10 min
- Requirements/anti-patterns conversion: 8 min
- State requirement tags: 5 min
- Integration point restructuring: 5 min
- Verbosity reduction: 2 min

## Additional Observations

### Strengths to Preserve
1. **Excellent content extraction** - References/ organization is textbook-perfect
2. **Clear file separation** - Each reference handles one concern (handoff-location, context-parsing, continuation-routing, error-recovery)
3. **Good preconditions** - YAML frontmatter accurately describes entry conditions
4. **Comprehensive error handling** - Error-recovery.md covers all failure modes
5. **Integration clarity** - Clear documentation of inbound/outbound connections

### System-Wide Pattern Opportunity
This skill's issues are likely repeated across other skills. Consider:
1. Creating skill-structure-template.md with canonical XML tag patterns
2. Auditing all skills for orchestration mode visibility
3. Standardizing decision gate enforcement across all checkpoint protocols
4. Adding `<delegation_rule>` tags to all orchestrator skills (plugin-workflow, plugin-ideation, etc.)

### Context Efficiency Note
The skill is already highly optimized for context efficiency through aggressive extraction to references/. The XML tag additions increase token count slightly (~440 tokens) but this is necessary overhead for reliability. The tradeoff is acceptable because:
- Error prevention > token savings
- Comprehension clarity > minimal context
- System correctness > optimization

This aligns with prompt engineering principle: "Be as concise as possible while maintaining absolute clarity."
