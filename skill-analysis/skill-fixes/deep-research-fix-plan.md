# Fix Plan: deep-research

## Summary
- Critical fixes: **8**
- Extraction operations: **5**
- Total estimated changes: **13**
- Estimated time: **70 minutes**

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Wrap read-only protocol in `<invariant>` tag
**Location:** Lines 32-58
**Operation:** WRAP
**Before:**
```markdown
## Read-Only Protocol

**CRITICAL:** This skill is **read-only** and **advisory only**. It NEVER:

- Edits code files
- Runs builds
- Modifies contracts or configurations
- Implements solutions
- Uses Write, Edit, or NotebookEdit tools
- Runs npm, CMake, or compiler commands

It ONLY:

- Searches for information (Grep, Glob, Read, WebSearch, WebFetch)
- Analyzes existing code and documentation
- Generates reports and recommendations
- Presents findings with decision menus
```
**After:**
```markdown
<invariant type="read_only_protocol" severity="critical">
This skill is READ-ONLY and ADVISORY ONLY.

NEVER:
- Edit code files via Edit/Write/NotebookEdit tools
- Run build commands (npm, CMake, compiler)
- Modify contracts or configurations
- Implement any solutions

ONLY:
- Search for information (Grep, Glob, Read, WebSearch, WebFetch)
- Analyze existing code and documentation
- Generate reports and recommendations
- Present findings with decision menus

ALWAYS delegate implementation to plugin-improve skill via handoff protocol.
Violation of this invariant breaks the system architecture.
</invariant>
```
**Verification:** Check that `<invariant type="read_only_protocol">` tag wraps the protocol at line 32

---

### Fix 1.2: Wrap handoff protocol in `<handoff_protocol>` tag
**Location:** Lines 44-50
**Operation:** WRAP
**Before:**
```markdown
**Workflow handoff:**

1. Research completes and presents findings with decision menu
2. User selects "Apply solution"
3. Output routing instruction: "User selected: Apply solution. Next step: Invoke plugin-improve skill."
4. Main conversation (orchestrator) sees routing instruction and invokes plugin-improve skill
5. plugin-improve reads research findings from conversation history
6. plugin-improve handles all implementation (with versioning, backups, testing)
```
**After:**
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
plugin-improve skips Phase 0.5 investigation (already completed by deep-research).
</context_passing>

<enforcement>
deep-research NEVER implements. Only plugin-improve implements.
</enforcement>
</handoff_protocol>
```
**Verification:** Check that `<handoff_protocol target_skill="plugin-improve">` tag exists at line 44

---

### Fix 1.3: Wrap Level 1 process in `<critical_sequence>` tag
**Location:** Lines 78-139
**Operation:** WRAP
**Before:**
```markdown
## Level 1: Quick Check (5-10 minutes, Sonnet)

**When to use:** All research requests start here

**Goal:** Find quick answer from local knowledge base or JUCE API docs

**Process:**

1. **Parse research topic:**
   [... details ...]

2. **Search local troubleshooting docs:**
   [... details ...]

3. **Check Context7 JUCE docs:**
   [... details ...]

4. **Assess confidence:**
   - **HIGH:** Exact match in local docs OR clear API documentation
   - **MEDIUM:** Similar issue found but needs adaptation
   - **LOW:** No relevant matches or unclear documentation

5. **Decision point:**

   **If HIGH confidence:**
   Present findings with decision menu:
   [... menu details ...]

   **If MEDIUM/LOW confidence:**
   Auto-escalate to Level 2 with notification:
   [... escalation details ...]
```
**After:**
```markdown
## Level 1: Quick Check (5-10 minutes, Sonnet)

<critical_sequence name="level1_quick_check" enforce_order="strict">
<step number="1" required="true">
**Parse research topic:**
- Extract keywords (e.g., "WebView", "parameter", "build error")
- Identify JUCE components involved (e.g., APVTS, AudioProcessor, Component)
- Classify problem type (build, runtime, API usage, concept)
</step>

<step number="2" required="true">
**Search local troubleshooting docs:**
- Use Grep tool to search troubleshooting/ directory
- Search patterns: keywords, error messages, JUCE component names
- Check by-plugin/ and by-symptom/ indices
- Look for exact or similar problem matches
</step>

<step number="3" required="true">
**Check Context7 JUCE docs:**
- Query JUCE API documentation via Context7 MCP
- Focus on: API reference, tutorials, migration guides
- Look for: class documentation, usage examples, best practices
</step>

<step number="4" required="true">
**Assess confidence:**
- **HIGH:** Exact match in local docs OR clear API documentation
- **MEDIUM:** Similar issue found but needs adaptation
- **LOW:** No relevant matches or unclear documentation
</step>

<decision_gate name="level1_outcome" enforce="strict">
IF confidence = HIGH:
  THEN present decision menu with solution
  WAIT for user selection

ELSE IF confidence IN [MEDIUM, LOW]:
  THEN auto-escalate to Level 2
  OUTPUT notification: "Level 1: No confident solution found. Escalating to Level 2..."
  PROCEED to Level 2 process
</decision_gate>
</critical_sequence>
```
**Verification:** Check that Level 1 process is wrapped in `<critical_sequence name="level1_quick_check">` tag

---

### Fix 1.4: Wrap Level 2 escalation logic in `<decision_gate>` tag
**Location:** Lines 189-219
**Operation:** WRAP
**Before:**
```markdown
5. **Decision point:**

   **If HIGH/MEDIUM confidence:**
   Present findings with decision menu:
   [... menu options ...]

   **If LOW confidence OR novel problem:**
   Auto-escalate to Level 3:
   [... escalation details ...]
```
**After:**
```markdown
<decision_gate name="level2_outcome" enforce="strict">
IF confidence IN [HIGH, MEDIUM]:
  THEN present decision menu with solution options
  OPTIONS: Apply solution | Review findings | Escalate to Level 3 | Other
  WAIT for user selection

ELSE IF confidence = LOW:
  THEN auto-escalate to Level 3
  OUTPUT notification: "Level 2: Low confidence. Escalating to Level 3 (deep research)..."
  PROCEED to Level 3 process

ELSE IF problem_type = "novel_implementation":
  THEN auto-escalate to Level 3
  OUTPUT notification: "Level 2: Novel problem requires parallel investigation. Escalating to Level 3..."
  PROCEED to Level 3 process
</decision_gate>
```
**Verification:** Check that Level 2 decision logic uses explicit `<decision_gate>` with IF/THEN/ELSE structure

---

### Fix 1.5: Wrap Level 3 model switch in `<delegation_rule>` tag
**Location:** Lines 221-235
**Operation:** WRAP
**Before:**
```markdown
## Level 3: Deep Research (30-60 minutes, Opus + Extended Thinking)

**When to use:** Low confidence at Level 2 OR novel/complex problems requiring parallel investigation

**Goal:** Comprehensive research via parallel subagents with extended thinking synthesis

**Requirements:**
- **MUST** switch to claude-opus-4-1-20250805
- **MUST** enable extended-thinking with 15,000 token budget
- **MUST** use parallel subagent spawning (not serial)
```
**After:**
```markdown
## Level 3: Deep Research (30-60 minutes, Opus + Extended Thinking)

<delegation_rule level="3" name="level3_requirements" enforce="strict">
<model_switch>
MUST switch to: claude-opus-4-1-20250805
MUST enable: extended-thinking with 15,000 token budget
MUST set timeout: 3600 seconds (60 min)
NEVER use: Sonnet at Level 3 (insufficient capacity for synthesis)
</model_switch>

<subagent_requirement>
MUST spawn 2-3 parallel research subagents via Task tool.
MUST invoke ALL subagents in PARALLEL (single response with multiple Task calls).
NEVER invoke subagents sequentially (defeats 60-min → 20-min optimization).

Each subagent runs in fresh context with focused research goal.
</subagent_requirement>

<timing_expectation>
3 parallel subagents × 20 min each = 20 min total wall time
NOT 60 min serial time
</timing_expectation>
</delegation_rule>
```
**Verification:** Check that Level 3 requirements are wrapped in `<delegation_rule>` with explicit model switch and parallel subagent enforcement

---

### Fix 1.6: Add `<critical_sequence>` wrapper for graduated protocol
**Location:** Lines 78 (before Level 1 section)
**Operation:** INSERT
**Change:**
```markdown
<critical_sequence name="graduated_research_protocol" skip_prevention="strict">
<level number="1" name="quick_check" max_time_minutes="10">
  <goal>Find quick answer from local knowledge base or JUCE API docs</goal>

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

  <goal>Deep-dive JUCE docs, forums, GitHub for authoritative answers</goal>

  <required_steps>
    1. Context7 deep-dive (advanced queries, cross-references)
    2. JUCE forum search via WebSearch
    3. GitHub issue search (juce-framework/JUCE)
    4. Synthesize findings from multiple sources
  </required_steps>

  <exit_conditions>
    IF confidence IN [HIGH, MEDIUM]: Present decision menu
    ELSE IF confidence = LOW OR novel_problem = true: MUST escalate to Level 3
  </exit_conditions>
</level>

<level number="3" name="deep_research" max_time_minutes="60">
  <prerequisite>Level 2 completed OR user manually started at Level 3</prerequisite>

  <goal>Parallel subagent investigation for novel/complex problems</goal>

  <model_requirements>
    MUST use: claude-opus-4-1-20250805
    MUST enable: extended-thinking with 15k budget
    NEVER use: Sonnet (insufficient synthesis capacity)
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
- NEVER implement solutions (always delegate to plugin-improve)
</enforcement_rules>
</critical_sequence>
```
**Verification:** Check that entire graduated protocol is wrapped in top-level `<critical_sequence>` tag starting at line 78

---

### Fix 1.7: Wrap decision menu handling in `<state_requirement>` tag
**Location:** Lines 579-643
**Operation:** WRAP
**Before:**
```markdown
## Decision Menu Protocol

At the end of each level (when presenting findings), follow checkpoint protocol:

1. **Present numbered decision menu** (NOT AskUserQuestion tool):
   [... menu details ...]

2. **Wait for user response**

3. **Handle responses:**
   - **Option 1 ("Apply solution"):**
     Output: "User selected: Apply solution. Next step: Invoke plugin-improve skill."
   [... other options ...]
```
**After:**
```markdown
<state_requirement name="checkpoint_protocol">
At the end of each level (when presenting findings), MUST:

1. Present decision menu (numbered list format, NOT AskUserQuestion tool)
2. WAIT for user response (NEVER auto-proceed)
3. Route based on selection:

<response_handler option="1" action="apply_solution">
<condition>User selects option 1 ("Apply solution")</condition>
<action>
1. Output handoff directive: "User selected: Apply solution. Next step: Invoke plugin-improve skill."
2. STOP execution (do not implement)
3. Orchestrator will invoke plugin-improve skill
</action>
</response_handler>

<response_handler option="2" action="review_findings">
<condition>User selects option 2 ("Review full findings")</condition>
<action>
1. Display complete research report
2. Re-present decision menu
3. WAIT for new selection
</action>
</response_handler>

<response_handler option="3" action="escalate">
<condition>User selects option 3 ("Escalate to next level")</condition>
<action>
1. Proceed to next level (Level 1 → Level 2 → Level 3)
2. If already at Level 3, inform user no further escalation available
3. Continue with next level's process
</action>
</response_handler>

<response_handler option="other" action="clarify">
<condition>User provides custom response</condition>
<action>
1. Ask for clarification
2. Re-present decision menu with context
3. WAIT for selection
</action>
</response_handler>

<enforcement>
NEVER auto-proceed to implementation.
NEVER skip decision menu.
ALWAYS wait for user response before continuing.
</enforcement>
</state_requirement>
```
**Verification:** Check that decision menu handling is wrapped in `<state_requirement name="checkpoint_protocol">` tag

---

### Fix 1.8: Add extended thinking enforcement gate
**Location:** Lines 12-13 (YAML frontmatter)
**Operation:** INSERT (after Line 13)
**Change:**
```markdown
extended-thinking: false # true for Level 3 only

<extended_thinking_gate>
IF level = 3:
  MUST enable extended-thinking with 15,000 token budget
  MUST verify activation before spawning subagents
ELSE IF level IN [1, 2]:
  MUST NOT enable extended-thinking
  Use standard reasoning for quick/moderate research
</extended_thinking_gate>
```
**Verification:** Check that extended thinking toggle has enforcement gate preventing misuse at wrong levels

---

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Remove duplication between SKILL.md and references/research-protocol.md
**Location:** Lines 78-310
**Operation:** EXTRACT + REPLACE
**Analysis:**
- SKILL.md lines 78-310 contain full Level 1-3 process (233 lines)
- references/research-protocol.md contains same content (310 lines)
- Total duplication: ~17,000 tokens

**Replace with:**
```markdown
## Level 1: Quick Check (5-10 minutes, Sonnet)

Search local docs + Context7 for known solutions.

See `references/research-protocol.md#level-1-quick-check` for detailed process.

<success_criteria level="1">
- Found exact match in local docs OR clear API documentation
- HIGH confidence (80%+)
- Solution directly applicable without modification
</success_criteria>

---

## Level 2: Moderate Investigation (15-30 minutes, Sonnet)

Deep-dive Context7, JUCE forums, GitHub for authoritative answers.

See `references/research-protocol.md#level-2-moderate-investigation` for detailed process.

<success_criteria level="2">
- Found authoritative answer verified by multiple sources
- MEDIUM-HIGH confidence (60%+)
- Solution adaptable with minor modifications
</success_criteria>

---

## Level 3: Deep Research (30-60 minutes, Opus + Extended Thinking)

Parallel subagent investigation for novel/complex problems.

See `references/research-protocol.md#level-3-deep-research` for detailed process.

<delegation_rule level="3">
MUST switch to claude-opus-4-1-20250805 with extended thinking (15k budget).
MUST spawn 2-3 parallel subagents via Task tool (NOT serial).
MUST use extended thinking for synthesis phase.
</delegation_rule>

<success_criteria level="3">
- Comprehensive investigation from multiple angles
- Novel insights or authoritative precedent found
- HIGH confidence after synthesis
</success_criteria>
```
**Verification:**
- SKILL.md reduced from 233 lines to ~50 lines for Level 1-3 sections
- No detailed process steps remain in SKILL.md
- All references point to research-protocol.md
- Estimated savings: ~6,000 tokens (35%)

---

### Fix 2.2: Extract report templates to assets/
**Location:** Lines 440-576
**Operation:** EXTRACT

**Create file:** `assets/level1-report-template.md`
**File content:**
```markdown
# Level 1 Research Report: [Topic]

## Search Results

### Local Troubleshooting Docs
- **Files searched:** [list]
- **Matches found:** [count]
- **Relevant entries:**
  1. [entry 1]
  2. [entry 2]

### Context7 JUCE Docs
- **API classes checked:** [list]
- **Documentation found:**
  - [class/method]: [summary]

## Confidence Assessment
- **Level:** HIGH/MEDIUM/LOW
- **Reasoning:** [why this confidence level]

## Solution
[Proposed solution based on findings]

## References
- Local docs: [file paths]
- JUCE docs: [URLs or class names]
```

**Create file:** `assets/level2-report-template.md`
**File content:**
```markdown
# Level 2 Research Report: [Topic]

## Investigation Summary
- **Time spent:** [X] minutes
- **Sources consulted:** [count]
- **Key findings:** [summary]

## Detailed Findings

### Context7 Deep-Dive
- **Advanced queries used:** [list]
- **Cross-references found:** [list]
- **Key insights:**
  1. [insight 1]
  2. [insight 2]

### JUCE Forum Search
- **Search terms:** [list]
- **Relevant threads:** [URLs]
- **Community solutions:**
  1. [solution 1]
  2. [solution 2]

### GitHub Issue Search
- **Repository:** juce-framework/JUCE
- **Issues found:** [URLs]
- **Resolutions:**
  1. [resolution 1]
  2. [resolution 2]

## Synthesis
[Synthesized understanding from all sources]

## Confidence Assessment
- **Level:** HIGH/MEDIUM/LOW
- **Reasoning:** [why this confidence level]

## Recommended Solution
[Proposed solution with source attribution]

## References
- Forum threads: [URLs]
- GitHub issues: [URLs]
- JUCE docs: [links]
```

**Create file:** `assets/level3-report-template.md`
**File content:**
```markdown
# Level 3 Deep Research Report: [Topic]

## Executive Summary
- **Research duration:** [X] minutes
- **Subagents spawned:** [count]
- **Model used:** claude-opus-4-1-20250805
- **Extended thinking:** Enabled (15k budget)
- **Confidence level:** HIGH/MEDIUM/LOW

## Research Approaches

### Approach 1: [Name]
**Subagent focus:** [description]
**Key findings:**
- [finding 1]
- [finding 2]
**Sources:** [list]

### Approach 2: [Name]
**Subagent focus:** [description]
**Key findings:**
- [finding 1]
- [finding 2]
**Sources:** [list]

### Approach 3: [Name]
**Subagent focus:** [description]
**Key findings:**
- [finding 1]
- [finding 2]
**Sources:** [list]

## Extended Thinking Synthesis
[Comprehensive synthesis using extended thinking to connect findings]

## Novel Insights
1. [insight 1]
2. [insight 2]
3. [insight 3]

## Recommended Solution
[Detailed solution with implementation considerations]

### Trade-offs
- [trade-off 1]
- [trade-off 2]

### Implementation Steps
1. [step 1]
2. [step 2]
3. [step 3]

## Confidence Assessment
- **Level:** [HIGH/MEDIUM/LOW]
- **Reasoning:** [detailed justification]

## References
### Primary Sources
- [source 1]
- [source 2]

### Supporting Sources
- [source 3]
- [source 4]

### JUCE-Specific
- API docs: [links]
- Forum threads: [URLs]
- GitHub issues: [URLs]
```

**Replace lines 440-576 in SKILL.md with:**
```markdown
## Report Generation

Each level generates a structured report using templates in `assets/`:
- Level 1: `assets/level1-report-template.md`
- Level 2: `assets/level2-report-template.md`
- Level 3: `assets/level3-report-template.md`

Reports include: findings summary, confidence assessment, recommended solution, and source references.
```
**Verification:**
- Three template files created in assets/
- SKILL.md reduced by ~137 lines
- Reference to templates exists in SKILL.md
- Estimated savings: ~4,200 tokens

---

### Fix 2.3: Extract example scenarios to references/
**Location:** Lines 854-975
**Operation:** EXTRACT

**Create file:** `references/example-scenarios.md`
**File content:**
```markdown
# Example Research Scenarios

## Scenario 1: WebView JavaScript Communication Freeze

**Problem:** User reports knobs freezing after WebView UI loads. Suspicion: race condition in message passing.

**Level 1 (Quick Check):**
1. Parse: "WebView", "JavaScript", "message passing", "race condition"
2. Search local: `Grep pattern:"WebView.*race|message.*freeze" path:troubleshooting/`
3. Result: Found `troubleshooting/gui-issues/webview-message-timing.md`
4. Confidence: HIGH (exact match with solution)
5. Decision: Present solution menu

**Outcome:** Solved in 3 minutes at Level 1.

---

## Scenario 2: APVTS Parameter Not Appearing in DAW

**Problem:** Parameter defined in code but not visible in DAW automation menu.

**Level 1 (Quick Check):**
1. Parse: "APVTS", "parameter", "automation", "DAW"
2. Search local: No exact match
3. Context7: Found `AudioProcessorValueTreeState` docs but unclear
4. Confidence: MEDIUM (API docs exist but problem not addressed)
5. Decision: Auto-escalate to Level 2

**Level 2 (Moderate Investigation):**
1. Context7 deep-dive: Cross-reference APVTS, PluginHostType, AudioProcessor
2. Forum search: Found thread "Parameters not automatable in Logic Pro"
3. GitHub: Found issue #542 "APVTS parameter visibility"
4. Synthesis: Parameter category metadata required for DAW visibility
5. Confidence: HIGH (multiple authoritative sources)
6. Decision: Present solution menu

**Outcome:** Solved in 18 minutes at Level 2.

---

## Scenario 3: Novel DSP Algorithm Implementation (IIR Filter Cascade)

**Problem:** Need to implement cascaded IIR filters with minimal phase distortion. No existing examples in codebase.

**Level 1 (Quick Check):**
1. Parse: "IIR filter", "cascade", "phase distortion", "DSP"
2. Search local: Generic DSP patterns but not cascaded IIR
3. Context7: Found `IIRFilter` class but no cascade examples
4. Confidence: LOW (API exists but novel use case)
5. Decision: Auto-escalate to Level 2

**Level 2 (Moderate Investigation):**
1. Context7: Deep-dive IIRFilter, ProcessorChain, dsp::ProcessSpec
2. Forum: Found discussions on filter design but not cascading
3. GitHub: No direct examples of cascaded IIR
4. Synthesis: API supports it but no precedent for minimal phase approach
5. Confidence: LOW (no clear precedent for novel requirement)
6. Decision: Auto-escalate to Level 3

**Level 3 (Deep Research):**
1. Model switch: Opus + extended thinking (15k budget)
2. Spawn 3 parallel subagents:
   - **Subagent A:** JUCE DSP architecture patterns (ProcessorChain, cascading)
   - **Subagent B:** DSP theory (IIR cascade phase distortion, correction techniques)
   - **Subagent C:** JUCE forum + GitHub deep-dive (related implementations)
3. Synthesis with extended thinking:
   - Subagent A: ProcessorChain supports cascading via template composition
   - Subagent B: Allpass filters for phase correction, minimum-phase design theory
   - Subagent C: Found forum post on filter cascade with phase compensation
4. Novel insight: Combine ProcessorChain with allpass phase compensator
5. Confidence: HIGH (novel solution synthesized from multiple authoritative sources)
6. Decision: Present comprehensive solution with implementation steps

**Outcome:** Solved in 42 minutes at Level 3 (20 min parallel subagents + 22 min synthesis).

---

## Scenario 4: Build Failure After JUCE 8 Migration

**Problem:** Project won't compile after upgrading from JUCE 7 to JUCE 8. Error: "AudioParameterFloat constructor ambiguous".

**Level 1 (Quick Check):**
1. Parse: "build failure", "JUCE 8 migration", "AudioParameterFloat", "constructor"
2. Search local: `Grep pattern:"AudioParameterFloat|JUCE 8" path:troubleshooting/build-failures/`
3. Result: Found `troubleshooting/api-usage/juce8-parameter-migration.md`
4. Content: JUCE 8 changed AudioParameterFloat constructor (breaking change)
5. Solution: Use new constructor signature with NormalisableRange
6. Confidence: HIGH (exact match in Required Reading)
7. Decision: Present solution menu

**Outcome:** Solved in 2 minutes at Level 1.

---

## Scenario 5: Unknown JUCE Component Behavior

**Problem:** User asks "How does LookAndFeel customization propagation work in JUCE?"

**Level 1 (Quick Check):**
1. Parse: "LookAndFeel", "customization", "propagation"
2. Search local: No specific docs on propagation mechanism
3. Context7: Found `LookAndFeel` class docs and examples
4. Confidence: HIGH (clear API documentation with examples)
5. Decision: Present explanation with API reference

**Outcome:** Answered in 4 minutes at Level 1.

---

## Key Patterns

### When Level 1 Succeeds:
- Exact match in local troubleshooting docs
- Clear API documentation in Context7
- Common problem with known solution
- **Time:** 2-10 minutes

### When Level 2 Needed:
- Local docs have partial/similar info
- API exists but usage unclear
- Problem needs cross-referencing multiple sources
- **Time:** 15-30 minutes

### When Level 3 Needed:
- Novel implementation (no precedent)
- Complex problem requiring multiple research angles
- Low confidence after Level 2
- Synthesis required from disparate sources
- **Time:** 30-60 minutes (20 min parallel + synthesis)

### Escalation Indicators:
- Confidence MEDIUM/LOW at current level
- "Novel" or "custom" in problem description
- Multiple failed searches at lower level
- Problem requires theoretical understanding + practical implementation
```

**Replace lines 854-975 in SKILL.md with:**
```markdown
## Example Scenarios

See `references/example-scenarios.md` for detailed walkthroughs of:
- Scenario 1: WebView freeze (solved at Level 1)
- Scenario 2: APVTS visibility (escalated to Level 2)
- Scenario 3: Novel DSP implementation (escalated to Level 3)
- Scenario 4: JUCE 8 migration (solved at Level 1 via Required Reading)
- Scenario 5: LookAndFeel propagation (answered at Level 1)

Each scenario shows escalation logic and time estimates.
```
**Verification:**
- File created at references/example-scenarios.md
- SKILL.md reduced by ~122 lines
- Reference exists in SKILL.md
- Estimated savings: ~3,800 tokens

---

### Fix 2.4: Remove redundant "Overview" section
**Location:** Lines 20-29
**Operation:** REPLACE
**Before:**
```markdown
## Overview

This skill provides graduated research capabilities:

- **Level 1 (Quick Check):** 5-10 minutes, local docs + Context7
- **Level 2 (Moderate Investigation):** 15-30 minutes, web research + synthesis
- **Level 3 (Deep Research):** 30-60 minutes, parallel subagents + extended thinking

The skill automatically escalates when confidence is low or problem is novel.
It stops at the first level that provides a confident answer.
```
**After:**
```markdown
## Overview

Multi-level autonomous investigation (Level 1: 5 min local → Level 2: 30 min web → Level 3: 60 min parallel). Stops at first confident answer. User controls depth.
```
**Verification:** Overview reduced from 10 lines to 2 lines, savings ~250 tokens

---

### Fix 2.5: Remove redundant "Notes for Claude" section
**Location:** Lines 801-822
**Operation:** DELETE
**Rationale:** These warnings are now enforced via XML tags in Phase 1 fixes:
- Read-only protocol → `<invariant>` tag (Fix 1.1)
- Model switch → `<delegation_rule>` tag (Fix 1.5)
- Parallel subagents → `<delegation_rule>` tag (Fix 1.5)
- Decision menu → `<state_requirement>` tag (Fix 1.7)

**Change:** Delete lines 801-822
**Verification:** Section removed, warnings now enforced via XML, savings ~700 tokens

---

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Remove redundant "Success Criteria" section
**Location:** Lines 711-741
**Operation:** DELETE
**Rationale:** Success criteria are already defined in each level's section after Fix 2.1.

**Change:** Delete lines 711-741
**Verification:** Section removed, criteria remain in level sections, savings ~1,000 tokens

---

### Fix 3.2: Clarify extended thinking toggle comment
**Location:** Line 13 (YAML frontmatter)
**Operation:** REPLACE
**Before:**
```yaml
extended-thinking: false # true for Level 3 only
```
**After:**
```yaml
extended-thinking: false # MUST be true for Level 3 (enforced by delegation_rule), false for Level 1-2
```
**Verification:** Comment clarifies enforcement mechanism

---

### Fix 3.3: Make decision menu format explicit
**Location:** Lines 595-602
**Operation:** REPLACE (enhanced by Fix 1.7)
**Before:**
```markdown
**Present numbered decision menu:**
```
**After:**
```markdown
**Present numbered decision menu (inline format, NOT AskUserQuestion tool):**
```
**Verification:** Clarifies NOT to use AskUserQuestion tool (already in Fix 1.7 but reinforced)

---

## File Creation Manifest

### Files to create:
1. `assets/level1-report-template.md` - Quick check report template
2. `assets/level2-report-template.md` - Moderate investigation report template
3. `assets/level3-report-template.md` - Deep research report template
4. `references/example-scenarios.md` - Detailed walkthrough of 5 research scenarios with escalation logic

### Files to verify (no creation needed):
- `references/research-protocol.md` - Already exists, verify completeness vs SKILL.md after Fix 2.1

---

## Execution Checklist

### Phase 1: Critical Fixes (30 minutes)
- [ ] Fix 1.1: Read-only protocol wrapped in `<invariant>` tag
- [ ] Fix 1.2: Handoff protocol wrapped in `<handoff_protocol>` tag
- [ ] Fix 1.3: Level 1 process wrapped in `<critical_sequence>` tag
- [ ] Fix 1.4: Level 2 escalation logic wrapped in `<decision_gate>` tag
- [ ] Fix 1.5: Level 3 model switch wrapped in `<delegation_rule>` tag
- [ ] Fix 1.6: Entire graduated protocol wrapped in top-level `<critical_sequence>` tag
- [ ] Fix 1.7: Decision menu handling wrapped in `<state_requirement>` tag
- [ ] Fix 1.8: Extended thinking enforcement gate added

### Phase 2: Content Extraction (20 minutes)
- [ ] Fix 2.1: Level 1-3 process details removed from SKILL.md, kept in references/research-protocol.md
- [ ] Fix 2.2: Report templates extracted to assets/ (3 files)
- [ ] Fix 2.3: Example scenarios extracted to references/example-scenarios.md
- [ ] Fix 2.4: Overview section condensed from 10 lines to 2 lines
- [ ] Fix 2.5: "Notes for Claude" section removed (enforced via XML)

### Phase 3: Polish (15 minutes)
- [ ] Fix 3.1: Redundant "Success Criteria" section removed
- [ ] Fix 3.2: Extended thinking toggle comment clarified
- [ ] Fix 3.3: Decision menu format made explicit

### Verification: Dimension Improvements (5 minutes)
- [ ] Structure Compliance: 4/5 → 5/5 (extended thinking enforcement added)
- [ ] Content Extraction: 2/5 → 5/5 (duplication removed, templates extracted)
- [ ] Instruction Clarity: 4/5 → 5/5 (decision gates explicit)
- [ ] XML Organization: 2/5 → 5/5 (all critical sequences wrapped)
- [ ] Context Efficiency: 3/5 → 5/5 (15,800 tokens saved)
- [ ] Claude-Optimized Language: 4/5 → 5/5 (all conditionals explicit)
- [ ] System Integration: 4/5 → 5/5 (handoff protocol explicit)
- [ ] Critical Sequence Enforcement: 1/5 → 5/5 (all workflows wrapped in XML)

### Final Verification:
- [ ] SKILL.md reduced to ~10,000 tokens (from ~26,000)
- [ ] All 4 critical issues resolved
- [ ] All 8 dimensions score 4+ (target: 7 at 5/5)
- [ ] No duplication between SKILL.md and references/
- [ ] All report templates in assets/
- [ ] All examples in references/
- [ ] All critical sequences have XML enforcement
- [ ] Read-only invariant cannot be violated
- [ ] Handoff protocol is mechanical, not interpretive
- [ ] Level 3 requirements cannot be skipped (model switch, parallel subagents, extended thinking)
- [ ] Decision gates use explicit IF/THEN/ELSE logic

---

## Estimated Impact

### Before:
- Context size: ~26,000 tokens
- Critical issues: 4
- Dimension scores below 3: 4 (XML, Content Extraction, Critical Sequence, Context Efficiency)
- Risk: Claude could skip levels, use wrong model, implement instead of research, invoke serial subagents

### After:
- Context size: ~10,000 tokens (61% reduction)
- Critical issues: 0
- All dimension scores: 4-5
- Risk: Minimal - all critical paths enforced via XML, architectural invariants protected

### Key Improvements:
1. **Architectural safety:** Read-only invariant enforced via `<invariant>` tag
2. **Workflow integrity:** Graduated protocol enforced via nested `<critical_sequence>` tags
3. **Model correctness:** Level 3 requires Opus (enforced via `<delegation_rule>`)
4. **Performance:** Parallel subagents enforced (not serial)
5. **Handoff clarity:** plugin-improve delegation is mechanical (not interpretive)
6. **Context efficiency:** 61% reduction enables better comprehension
7. **Decision clarity:** All gates use explicit IF/THEN/ELSE logic
8. **Maintenance:** Separated concerns (SKILL.md = orchestration, references/ = details)

---

## Notes

- All line numbers reference the CURRENT state of SKILL.md from the analysis report
- After Phase 1, line numbers will shift - use content matching for subsequent phases
- Phase 1 fixes are highest priority (architectural integrity)
- Phase 2 can be done incrementally (each extraction is independent)
- Phase 3 is polish (lowest priority but improves readability)
- Verify no duplication after Fix 2.1 by comparing SKILL.md lines 78-310 with references/research-protocol.md
- Report templates (Fix 2.2) should match existing `assets/report-template.json` structure if possible
