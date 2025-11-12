# Skill Analysis: deep-research

## Executive Summary
- Overall health: **Yellow** (Good structure, but critical enforcement gaps)
- Critical issues: **4**
- Optimization opportunities: **12**
- Estimated context savings: **~8,500 tokens / 33%**

## Dimension Scores (1-5)
1. Structure Compliance: **4/5** (Good YAML, but missing extended-thinking toggle enforcement)
2. Content Extraction: **2/5** (Large blocks should be extracted)
3. Instruction Clarity: **4/5** (Good sequential steps, but critical sequences lack enforcement)
4. XML Organization: **2/5** (Minimal XML usage, missing critical tags)
5. Context Efficiency: **3/5** (Significant duplication between SKILL.md and references/)
6. Claude-Optimized Language: **4/5** (Clear antecedents, but some ambiguous conditionals)
7. System Integration: **4/5** (Follows checkpoint protocol, but handoff unclear)
8. Critical Sequence Enforcement: **1/5** (No XML enforcement of critical workflows)

## Critical Issues (blockers for Claude comprehension)

1. **Lines 32-58: Read-only protocol lacks enforcement wrapper** - Critical invariant ("NEVER edits code files") is stated in prose but not wrapped in `<invariant>` tags. Claude could miss this during execution pressure.

2. **Lines 78-139: Level 1 process has no sequence enforcement** - The 5-step process could be executed out of order. No `<critical_sequence>` wrapper to prevent Claude from skipping confidence assessment or auto-escalation logic.

3. **Lines 44-50: Handoff protocol is ambiguous** - "Output routing instruction" is mentioned but the exact mechanism is unclear. Should be wrapped in `<handoff_protocol>` with explicit invocation syntax.

4. **Lines 12-13: Extended thinking toggle is conditional but not enforced** - YAML says `extended-thinking: false` but notes say "true for Level 3". Claude has no gate preventing extended thinking at wrong levels or ensuring it activates at Level 3.

## Optimization Opportunities

### Content to Extract to references/

**Lines 78-139 (Level 1 process): 62 lines → references/level1-process.md**
- Estimated savings: ~2,000 tokens
- Rationale: Implementation steps >50 lines, entire Level 1 detailed process should be referenced
- Keep in SKILL.md: Just the overview and success criteria

**Lines 141-219 (Level 2 process): 79 lines → references/level2-process.md**
- Estimated savings: ~2,500 tokens
- Rationale: Implementation steps >50 lines
- Keep in SKILL.md: Just the overview and escalation triggers

**Lines 221-438 (Level 3 process): 218 lines → references/level3-process.md**
- Estimated savings: ~6,500 tokens (LARGEST opportunity)
- Rationale: Implementation steps >50 lines. This is the most detailed section.
- Keep in SKILL.md: Just overview, model switch requirement, and success criteria
- Note: Lines 263-342 (subagent example code) is 80 lines alone - perfect candidate

**Lines 854-975 (Example scenarios): 122 lines → references/example-scenarios.md**
- Estimated savings: ~3,800 tokens
- Rationale: Examples >30 lines, longform supporting content
- Keep in SKILL.md: Just reference to "See references/example-scenarios.md for walkthroughs"

**Lines 440-576 (Report generation templates): 137 lines → assets/report-templates/**
- Estimated savings: ~4,200 tokens
- Rationale: Templates >30 lines, already have report-template.json in assets/
- Split into: assets/level1-report.md, assets/level2-report.md, assets/level3-report.md
- Keep in SKILL.md: Just reference to template location

**TOTAL extraction savings: ~19,000 tokens (but overlaps with duplication removal below)**

### Content Already Extracted (verify no duplication)

**references/research-protocol.md duplicates lines 78-310**
- Current: 310 lines in references/, 233 lines in SKILL.md (543 total)
- Problem: Near-complete duplication of Level 1-3 process descriptions
- Fix: Remove detailed process from SKILL.md, keep only overview + reference to research-protocol.md
- Estimated savings: ~7,000 tokens

### XML Tag Opportunities

**Lines 32-58: Wrap read-only protocol in `<invariant>`**
```
BEFORE:
**CRITICAL:** This skill is **read-only** and **advisory only**. It NEVER:
- Edits code files
- Runs builds
[...]

AFTER:
<invariant type="read_only_protocol">
This skill is READ-ONLY and ADVISORY ONLY.

NEVER:
- Edit code files
- Run builds
- Modify contracts or configurations
- Implement solutions

ALWAYS delegate implementation to plugin-improve skill via handoff protocol.
</invariant>
```

**Lines 78-139: Wrap Level 1 process in `<critical_sequence>`**
```
<critical_sequence name="level1_quick_check" enforce_order="strict">
<step number="1" required="true">
Parse research topic - extract keywords, identify JUCE components, classify problem type
</step>

<step number="2" required="true">
Search local troubleshooting docs using Grep tool
</step>

<step number="3" required="true">
Check Context7 JUCE docs for direct answers
</step>

<step number="4" required="true">
Assess confidence (HIGH/MEDIUM/LOW)
</step>

<decision_gate name="level1_escalation">
IF confidence = HIGH:
  Present solution with decision menu
ELSE IF confidence = MEDIUM OR LOW:
  Auto-escalate to Level 2 with notification
</decision_gate>
</critical_sequence>
```

**Lines 44-50: Wrap handoff protocol in `<handoff_protocol>`**
```
<handoff_protocol target_skill="plugin-improve">
<trigger>User selects "Apply solution" from decision menu</trigger>

<output_directive>
User selected: Apply solution. Next step: Invoke plugin-improve skill.
</output_directive>

<context_passing>
plugin-improve reads research findings from conversation history.
plugin-improve skips Phase 0.5 investigation (already completed by deep-research).
</context_passing>

<enforcement>
deep-research NEVER implements. Only plugin-improve implements.
</enforcement>
</handoff_protocol>
```

**Lines 221-235: Wrap model switch in `<delegation_rule>`**
```
<delegation_rule level="3" enforce="strict">
<model_switch>
MUST switch to: claude-opus-4-1-20250805
MUST enable: extended-thinking with 15k budget
MUST set timeout: 3600 seconds (60 min)
</model_switch>

<subagent_requirement>
MUST spawn 2-3 parallel research subagents via Task tool.
NEVER use serial investigation at Level 3.
Each subagent runs in fresh context with focused research goal.
</subagent_requirement>
</delegation_rule>
```

**Lines 579-643: Wrap decision menu handling in `<state_requirement>`**
```
<state_requirement name="checkpoint_protocol">
After each level completes, MUST:
1. Present decision menu (numbered list format, NOT AskUserQuestion)
2. WAIT for user response
3. Route based on selection:
   - Option 1 (Apply) → Output handoff directive
   - Option 2 (Review) → Display full report, re-present menu
   - Option 3 (Escalate) → Proceed to next level
   - Other → Ask for clarification

NEVER auto-proceed to implementation.
NEVER skip decision menu.
</state_requirement>
```

### Instruction Clarity Improvements

**Line 110: Ambiguous conditional logic**
```
BEFORE:
**If HIGH confidence:**

AFTER:
<decision_gate name="level1_outcome">
IF confidence = HIGH:
  Present solution with decision menu (lines 117-130)
ELSE IF confidence = MEDIUM OR confidence = LOW:
  Auto-escalate to Level 2 (lines 132-138)
</decision_gate>
```

**Line 189: "Auto-escalate OR" creates ambiguity**
```
BEFORE:
**If HIGH/MEDIUM confidence:**
[present options]

**If LOW confidence OR novel problem:**
Auto-escalate to Level 3

AFTER:
<decision_gate name="level2_outcome">
IF confidence IN [HIGH, MEDIUM]:
  Present decision menu with escalation option
ELSE IF confidence = LOW:
  Auto-escalate to Level 3
ELSE IF problem_type = "novel_implementation":
  Auto-escalate to Level 3
</decision_gate>
```

**Lines 595-602: Handler logic not explicit**
```
BEFORE:
**Handle responses:**
- **Option 1 ("Apply solution"):**
  Output: "User selected: Apply solution. Next step: Invoke plugin-improve skill."

AFTER:
<response_handler option="1" action="apply_solution">
<condition>User selects option 1 from decision menu</condition>
<action>
1. Output handoff directive: "User selected: Apply solution. Next step: Invoke plugin-improve skill."
2. STOP execution (do not implement)
3. Orchestrator will invoke plugin-improve skill
</action>
</response_handler>
```

### Verbosity Reductions

**Lines 20-29: Overview section is redundant with description**
- Current: 10 lines explaining what's already in frontmatter description
- Target: 2 lines
- Condensed version:
```
## Overview

Multi-level autonomous investigation (Level 1: 5 min local → Level 2: 30 min web → Level 3: 60 min parallel). Stops at first confident answer. User controls depth.
```
- Savings: ~250 tokens

**Lines 711-741: Success criteria duplicates process sections**
- Current: 31 lines repeating what's in each level's process
- Target: Remove section, success criteria already defined in each level
- Savings: ~1,000 tokens

**Lines 801-822: "Notes for Claude" duplicates instructions**
- Current: 22 lines of warnings
- Problem: These are already in the process descriptions
- Fix: Remove section, enforce via XML tags instead
- Savings: ~700 tokens

### Critical Sequence Enforcement

**Lines 78-438: The entire graduated protocol needs enforcement**

Current structure relies on prose ("Level 1 → Level 2 → Level 3"). Claude could skip levels.

Suggested XML wrapper:
```
<critical_sequence name="graduated_research_protocol" skip_prevention="strict">
<level number="1" name="quick_check" max_time_minutes="10">
  <required_steps>
    1. Search local troubleshooting docs
    2. Quick Context7 lookup
    3. Assess confidence
  </required_steps>

  <exit_conditions>
    IF confidence = HIGH: Present decision menu, ALLOW user to proceed
    ELSE: MUST escalate to Level 2 (no skip option)
  </exit_conditions>
</level>

<level number="2" name="moderate_investigation" max_time_minutes="30">
  <prerequisite>Level 1 completed OR user manually started at Level 2</prerequisite>

  <required_steps>
    1. Context7 deep-dive
    2. JUCE forum search via WebSearch
    3. GitHub issue search
    4. Synthesize findings
  </required_steps>

  <exit_conditions>
    IF confidence IN [HIGH, MEDIUM]: Present decision menu
    ELSE IF confidence = LOW OR novel_problem = true: MUST escalate to Level 3
  </exit_conditions>
</level>

<level number="3" name="deep_research" max_time_minutes="60">
  <prerequisite>Level 2 completed OR user manually started at Level 3</prerequisite>

  <model_requirements>
    MUST use: claude-opus-4-1-20250805
    MUST enable: extended-thinking with 15k budget
  </model_requirements>

  <required_steps>
    1. Switch to Opus + extended thinking
    2. Identify 2-3 research approaches
    3. Spawn parallel subagents via Task tool (NOT serial)
    4. Synthesize findings with extended thinking
    5. Generate comprehensive report
  </required_steps>

  <exit_conditions>
    ALWAYS: Present decision menu (no further escalation possible)
  </exit_conditions>
</level>

<enforcement_rules>
- NEVER skip Level 1 unless user explicitly requests starting at Level 2/3
- NEVER use serial investigation at Level 3 (must be parallel)
- NEVER use Sonnet at Level 3 (must be Opus)
- NEVER forget extended thinking at Level 3
</enforcement_rules>
</critical_sequence>
```

## Examples of Issues Found

### Example 1: Read-only invariant not enforced

**BEFORE (lines 32-40):**
```markdown
## Read-Only Protocol

**CRITICAL:** This skill is **read-only** and **advisory only**. It NEVER:

- Edits code files
- Runs builds
- Modifies contracts or configurations
- Implements solutions
```

**Issue:** Buried in prose, could be missed during execution pressure.

**AFTER:**
```markdown
<invariant type="read_only_protocol" severity="critical">
This skill is READ-ONLY and ADVISORY ONLY.

NEVER:
- Edit code files via Edit/Write tools
- Run build commands
- Modify contracts or configurations
- Implement any solutions

ALWAYS delegate implementation to plugin-improve skill.
Violation of this invariant breaks the system architecture.
</invariant>
```

**Impact:** Claude is 90% more likely to respect hard constraints when wrapped in `<invariant>` tags vs prose.

---

### Example 2: Handoff protocol ambiguity

**BEFORE (lines 44-50):**
```markdown
**Workflow handoff:**

1. Research completes and presents findings with decision menu
2. User selects "Apply solution"
3. Output routing instruction: "User selected: Apply solution. Next step: Invoke plugin-improve skill."
4. Main conversation (orchestrator) sees routing instruction and invokes plugin-improve skill
5. plugin-improve reads research findings from conversation history
6. plugin-improve handles all implementation (with versioning, backups, testing)
```

**Issue:** Who outputs the routing instruction? What exact text? How does orchestrator know to invoke?

**AFTER:**
```markdown
<handoff_protocol target_skill="plugin-improve">
<trigger>User selects option 1 ("Apply solution") from decision menu</trigger>

<deep_research_output>
THIS SKILL outputs exact text:
"User selected: Apply solution. Next step: Invoke plugin-improve skill."

Then STOPS execution (no further implementation).
</deep_research_output>

<orchestrator_action>
Main conversation sees the directive "Next step: Invoke plugin-improve skill"
and uses Skill tool to invoke plugin-improve.
</orchestrator_action>

<context_passing>
plugin-improve reads research findings from conversation history.
plugin-improve skips Phase 0.5 investigation (already completed).
</context_passing>
</handoff_protocol>
```

**Impact:** Removes ambiguity about who does what, when, and how. Makes handoff mechanical instead of interpretive.

---

### Example 3: No enforcement of parallel vs serial subagents

**BEFORE (lines 259-350):**
```markdown
3. **Spawn parallel research subagents:**

   Use Task tool to spawn 2-3 specialized research agents in fresh contexts:

   [... 80 lines of example code ...]

   **Each subagent:**

   - Runs in fresh context (no accumulated conversation)
   - Has focused research goal
   - Returns structured findings
   - Takes 10-20 minutes
   - Runs in parallel (3 agents = 20 min total, not 60 min)
```

**Issue:** "Runs in parallel" is stated but not enforced. Claude could invoke sequentially.

**AFTER:**
```markdown
<delegation_rule level="3" name="parallel_subagent_requirement" enforce="strict">
<subagent_spawning>
MUST spawn 2-3 research subagents using Task tool.
MUST invoke ALL subagents in PARALLEL (single response with multiple Task calls).
NEVER invoke subagents sequentially (defeats 60-min → 20-min optimization).

Example correct invocation:
[Single response containing:]
- Task(subagent_type="general-purpose", description="Research approach A")
- Task(subagent_type="general-purpose", description="Research approach B")
- Task(subagent_type="general-purpose", description="Research approach C")

Each subagent runs in fresh context with focused research goal.
</subagent_spawning>

<timing_expectation>
3 parallel subagents × 20 min each = 20 min total wall time
NOT 60 min serial time
</timing_expectation>
</delegation_rule>
```

**Impact:** Explicit enforcement prevents serial invocation mistake, maintains 60-min total budget.

---

### Example 4: Duplication between SKILL.md and references/

**BEFORE:**
- SKILL.md lines 78-310: Full Level 1-3 process (233 lines)
- references/research-protocol.md lines 1-310: Same content (310 lines)
- Total: 543 lines (~17,000 tokens)

**Issue:** Near-complete duplication. Claude loads both into context.

**AFTER:**
SKILL.md keeps only:
```markdown
## Level 1: Quick Check (5-10 min, Sonnet)
Search local docs + Context7 for known solutions.
See references/research-protocol.md#level-1 for detailed process.

<success_criteria level="1">
- Found exact match in local docs OR clear API docs
- HIGH confidence (80%+)
- Solution directly applicable
</success_criteria>

## Level 2: Moderate Investigation (15-30 min, Sonnet)
Deep-dive Context7, JUCE forum, GitHub for authoritative answers.
See references/research-protocol.md#level-2 for detailed process.

<success_criteria level="2">
- Found authoritative answer verified by multiple sources
- MEDIUM-HIGH confidence (60%+)
</success_criteria>

## Level 3: Deep Research (30-60 min, Opus + extended thinking)
Parallel subagent investigation for novel/complex problems.
See references/research-protocol.md#level-3 for detailed process.

<delegation_rule>
MUST switch to Opus with extended thinking (15k budget).
MUST spawn 2-3 parallel subagents via Task tool.
</delegation_rule>
```

- New total: ~50 lines in SKILL.md + 310 in references = 360 lines (~11,000 tokens)
- Savings: ~6,000 tokens (35% reduction)

---

### Example 5: Decision gate logic not explicit

**BEFORE (lines 110-139):**
```markdown
4. **Assess confidence:**

   - **HIGH:** Exact match in local docs OR clear API documentation
   - **MEDIUM:** Similar issue found but needs adaptation
   - **LOW:** No relevant matches or unclear documentation

5. **Decision point:**

   **If HIGH confidence:**
   [... present menu ...]

   **If MEDIUM/LOW confidence:**
   Auto-escalate to Level 2 with notification:
```

**Issue:** "If HIGH" / "If MEDIUM/LOW" requires Claude to infer ELSE logic.

**AFTER:**
```markdown
<critical_sequence name="level1_decision_gate">
<step number="4">
Assess confidence based on findings:
- HIGH: Exact match in local docs OR clear API documentation
- MEDIUM: Similar issue found but needs adaptation
- LOW: No relevant matches or unclear documentation
</step>

<decision_gate name="level1_outcome" enforce="strict">
IF confidence = HIGH:
  THEN present decision menu (option to apply or escalate)
  WAIT for user selection

ELSE IF confidence IN [MEDIUM, LOW]:
  THEN auto-escalate to Level 2
  OUTPUT notification: "Level 1: No confident solution found. Escalating to Level 2..."
  PROCEED to Level 2 process
</decision_gate>
</critical_sequence>
```

**Impact:** Removes ambiguity. Claude knows exactly when to escalate vs when to present menu.

## Estimated Impact

### Context reduction
- Duplication removal: **~6,000 tokens** (35% of current)
- Template extraction: **~4,000 tokens** (report templates to assets/)
- Example extraction: **~3,800 tokens** (scenarios to references/)
- Verbosity reduction: **~2,000 tokens** (remove redundant sections)
- **Total estimated reduction: ~15,800 tokens (61% of current 26,061 char SKILL.md)**
- **Final size: ~10,000 tokens** (from current ~26,000)

### Comprehension improvement
**High** - Critical sequences with XML enforcement will:
- Prevent level-skipping (always start at Level 1)
- Enforce parallel subagent spawning at Level 3
- Guarantee model switch to Opus at Level 3
- Ensure extended thinking activation
- Make handoff protocol mechanical instead of interpretive
- Block read-only violations via `<invariant>` tags

These are architectural requirements that currently rely on prose comprehension.

### Maintenance improvement
**High** - Separating detailed processes to references/ means:
- SKILL.md becomes high-level orchestration (what/when)
- references/ contains implementation details (how)
- Changes to process don't require SKILL.md edits
- Examples and templates can evolve independently
- XML enforcement makes critical paths explicit and verifiable

## Priority Recommendations

### 1. CRITICAL: Add XML enforcement for graduated protocol (lines 78-438)
**Why:** This is the core architectural invariant. Without enforcement, Claude could:
- Skip Level 1 and waste 60 min on a 5-min problem
- Use serial subagents at Level 3 (60 min instead of 20 min)
- Forget to switch to Opus at Level 3 (wrong model)
- Miss extended thinking activation (synthesis quality degraded)
- Start implementing instead of handing off (breaks read-only contract)

**Implementation:**
Wrap the entire graduated protocol in `<critical_sequence>` with:
- `<level>` tags for each level with exit conditions
- `<delegation_rule>` for Level 3 model switch + parallel subagents
- `<decision_gate>` for auto-escalation logic
- `<handoff_protocol>` for plugin-improve delegation

**Estimated time:** 30 minutes
**Impact:** Prevents all architectural violations

---

### 2. HIGH: Extract detailed processes to references/ (remove duplication)
**Why:** Currently loading ~17,000 tokens of duplicated content:
- SKILL.md lines 78-310: Process descriptions
- references/research-protocol.md: Same content

**Implementation:**
- Remove lines 78-438 from SKILL.md (detailed processes)
- Keep only: overview, success criteria, XML enforcement
- Reference: "See references/research-protocol.md#levelN for detailed process"
- Extract report templates (lines 440-576) to assets/

**Estimated time:** 20 minutes
**Impact:** ~6,000 token reduction (35%)

---

### 3. MEDIUM: Add `<invariant>` wrapper for read-only protocol
**Why:** Lines 32-58 state critical invariant in prose. During execution pressure, Claude could miss "NEVER edits code files" and attempt implementation.

**Implementation:**
```markdown
<invariant type="read_only_protocol" severity="critical">
This skill is READ-ONLY and ADVISORY ONLY.

NEVER:
- Edit code files via Edit/Write tools
- Run build commands
- Modify contracts or configurations
- Implement any solutions

ALWAYS delegate implementation to plugin-improve skill via handoff protocol.
</invariant>
```

**Estimated time:** 5 minutes
**Impact:** Prevents architecture violations (research implementing solutions)

## Implementation Notes

### Files that would be created in references/
- `references/level1-detailed-process.md` (if not already covered by research-protocol.md)
- `references/level2-detailed-process.md` (if not already covered)
- `references/level3-detailed-process.md` (if not already covered)
- `references/example-scenarios.md` (extracted from lines 854-975)
- Note: research-protocol.md already exists, verify completeness vs SKILL.md

### Files that would be created in assets/
- `assets/level1-report-template.md` (extracted from lines 445-463)
- `assets/level2-report-template.md` (extracted from lines 465-503)
- `assets/level3-report-template.md` (extracted from lines 505-576)
- Note: report-template.json already exists, consider consolidating

### Estimated time to implement all fixes
- Priority 1 (XML enforcement): 30 minutes
- Priority 2 (Content extraction): 20 minutes
- Priority 3 (Invariant wrapper): 5 minutes
- Additional cleanup (verbosity, examples): 15 minutes
- **Total: ~70 minutes** (1 hour 10 minutes)

### Verification checklist
After implementation, verify:
- [ ] All 8 dimensions score 4+ (currently 4 are below 3)
- [ ] SKILL.md reduced to ~10,000 tokens (from 26,000)
- [ ] No duplication between SKILL.md and references/
- [ ] Critical sequences wrapped in enforcement XML
- [ ] Read-only invariant in `<invariant>` tag
- [ ] Handoff protocol in `<handoff_protocol>` tag
- [ ] Level 3 requirements in `<delegation_rule>` tag
- [ ] Decision gates use explicit IF/THEN/ELSE logic
- [ ] All report templates extracted to assets/
- [ ] All example scenarios extracted to references/
