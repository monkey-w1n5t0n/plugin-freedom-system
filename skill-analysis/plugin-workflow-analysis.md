# Skill Analysis: plugin-workflow

## Executive Summary
- Overall health: **Yellow** (Functional but lacks critical enforcement structure)
- Critical issues: **4** (XML enforcement gaps, no structural validation of invariants)
- Optimization opportunities: **6** (Content extraction, XML wrapping, verbosity reduction)
- Estimated context savings: **1920 tokens (12% reduction)**

## Dimension Scores (1-5)
1. Structure Compliance: **4/5** (Good YAML frontmatter, could improve description with trigger terms)
2. Content Extraction: **3/5** (Stage files extracted well, pseudocode and examples need extraction)
3. Instruction Clarity: **4/5** (Clear numbered steps, needs XML wrapping for critical rules)
4. XML Organization: **2/5** (Minimal XML usage, missing all enforcement tags)
5. Context Efficiency: **3/5** (Moderate duplication, pseudocode verbose)
6. Claude-Optimized Language: **4/5** (Clear and explicit, minor pronoun improvements)
7. System Integration: **5/5** (Excellent documentation of checkpoints, handoffs, integration points)
8. Critical Sequence Enforcement: **1/5** (CRITICAL DEFICIENCY - relies on natural language emphasis instead of XML structure)

## Critical Issues (blockers for Claude comprehension)

### Issue #1: Orchestration Rules Lack XML Enforcement (Lines 32-51)
**Severity:** CRITICAL
**Impact:** Claude may skip delegation rules under token pressure

The most important rules in the skill are buried in markdown prose:
```markdown
**CRITICAL ORCHESTRATION RULES:**
1. Stages 2-5 MUST use Task tool to invoke subagents - NEVER implement directly
2. After EVERY subagent return (whether full stage or phase completion), orchestrator MUST:
   - Commit all changes with git
   - Update .continue-here.md with current state
   ...
```

**Problem:** Natural language emphasis (MUST, NEVER, ALWAYS) is not structurally enforced. Claude can miss these during context compression.

**Recommended Fix:** Wrap in nested XML structure with violation attributes:
```xml
<orchestration_rules>
  <delegation_rule stages="2-5" tool="Task" violation="BLOCKS_EXECUTION">
    Stages 2-5 MUST invoke subagents via Task tool. NEVER implement directly in orchestrator context.
  </delegation_rule>

  <checkpoint_protocol frequency="after_every_subagent" auto_proceed="NEVER">
    After EVERY subagent return, orchestrator MUST complete all checkpoint steps before waiting for user.
  </checkpoint_protocol>

  <critical_sequence>
    1. Commit changes → 2. Update .continue-here.md → 3. Update PLUGINS.md → 4. Present menu → 5. WAIT
  </critical_sequence>
</orchestration_rules>
```

### Issue #2: Checkpoint Protocol Not Wrapped (Lines 224-273, state-management.md lines 513-598)
**Severity:** CRITICAL
**Impact:** Claude may skip checkpoint steps, breaking workflow resumability

The 5-step checkpoint sequence is described in prose but not structurally enforced:
```javascript
// Current approach (lines 242-256):
await commitStage(pluginName, currentStage, result.description)
await updateHandoff(pluginName, currentStage + 1, result.completed, result.nextSteps)
await updatePluginStatus(pluginName, `🚧 Stage ${currentStage}`)
await updatePluginTimeline(pluginName, currentStage, result.description)

const choice = presentDecisionMenu({...})
```

**Problem:** No structural enforcement that these steps must happen in order and cannot be skipped.

**Recommended Fix:** Wrap in `<checkpoint_enforcement>` with step ordering:
```xml
<checkpoint_enforcement>
  <step order="1" required="true" function="commitStage">
    Auto-commit all changes from subagent completion
  </step>
  <step order="2" required="true" function="updateHandoff">
    Update .continue-here.md with new stage, timestamp, next_action
  </step>
  <step order="3" required="true" function="updatePluginStatus">
    Update PLUGINS.md status emoji
  </step>
  <step order="4" required="true" function="updatePluginTimeline">
    Append timeline entry to PLUGINS.md
  </step>
  <step order="5" required="true" blocking="true" function="presentDecisionMenu">
    Present numbered decision menu and WAIT for user response
  </step>
</checkpoint_enforcement>
```

### Issue #3: Delegation Rule Repeated Without Structure (Lines 144-151, 386, 404)
**Severity:** HIGH
**Impact:** Maintenance burden, inconsistent enforcement

The "NEVER implement directly" rule appears 3+ times in different phrasings:
- Line 144: "CRITICAL RULE: Never Implement Directly"
- Line 386: "NEVER implement stages 2-5 directly"
- Line 404: "❌ Implementing stage logic directly in orchestrator (ALWAYS use Task tool)"

**Problem:** Repetition suggests uncertainty about whether Claude will follow the rule. Creates maintenance burden.

**Recommended Fix:** Single `<delegation_rule>` referenced throughout:
```xml
<delegation_rule id="stages-2-5-subagent-only" violation="IMMEDIATE_STOP">
  Stages 2-5 MUST be delegated to subagents via Task tool.
  This skill is a PURE ORCHESTRATOR - it NEVER implements plugin code directly.

  Valid delegation targets:
  - Stage 2: foundation-agent
  - Stage 3: shell-agent
  - Stage 4: dsp-agent
  - Stage 5: gui-agent
</delegation_rule>

<!-- Later references -->
<enforcement_reminder ref="stages-2-5-subagent-only"/>
```

### Issue #4: Orchestrator vs Subagent Prompt Content Unclear (Lines 176-223, 293-300)
**Severity:** MEDIUM
**Impact:** Claude confusion about whether to execute or delegate

Lines 176-223 show JavaScript pseudocode for dispatcher logic:
```javascript
async function dispatchStage(pluginName, stageNumber) {
  // Check preconditions
  const preconditionCheck = checkStagePreconditions(pluginName, stageNumber)
  ...
```

But line 293-300 notes: "Stage reference files contain subagent prompts, not direct implementation instructions."

**Problem:** The pseudocode might confuse Claude about whether to execute dispatcher logic directly or recognize it as orchestration pattern guidance.

**Recommended Fix:** Clear separation with tags:
```xml
<orchestrator_logic>
  The orchestrator follows this pattern (DO NOT implement, this is guidance):
  1. Check preconditions
  2. Invoke appropriate subagent via Task tool
  3. Wait for completion
  4. Run checkpoint protocol
</orchestrator_logic>

<subagent_prompt_location>
  Subagent prompts are stored in references/stage-N-*.md files.
  Read these files to construct Task tool invocations.
</subagent_prompt_location>
```

## Optimization Opportunities

### Content to Extract to references/

#### Extraction #1: Lines 176-223 → references/dispatcher-pattern.md
**Description:** JavaScript pseudocode showing dispatcher logic flow
**Reason:** 48 lines of pseudocode in main SKILL.md increases cognitive load
**Token savings:** ~750 tokens

**Current:**
```javascript
async function dispatchStage(pluginName, stageNumber) {
  const preconditionCheck = checkStagePreconditions(pluginName, stageNumber)

  if (!preconditionCheck.allowed) {
    console.log(`✗ BLOCKED: ${preconditionCheck.reason}`)
    ...
  }

  switch(stageNumber) {
    case 2:
      return await invokeSubagent('foundation-agent', {...})
    ...
  }
}
```

**After extraction:**
```markdown
## Stage Dispatcher

The orchestrator uses the dispatcher pattern to route stages to subagents.
See [references/dispatcher-pattern.md](references/dispatcher-pattern.md) for implementation pseudocode.

**Key principle:** ALWAYS invoke subagents via Task tool. NEVER implement directly.
```

#### Extraction #2: Lines 60-77 → references/precondition-checks.md
**Description:** Bash examples for checking contract files
**Reason:** Implementation details that can be referenced when needed
**Token savings:** ~270 tokens

**Current:**
```bash
test -f "plugins/$PLUGIN_NAME/.ideas/architecture.md"
test -f "plugins/$PLUGIN_NAME/.ideas/plan.md"
test -f "plugins/$PLUGIN_NAME/.ideas/creative-brief.md"
```

**After extraction:**
```markdown
## Precondition Checking

Before starting, verify contracts exist. See [references/precondition-checks.md](references/precondition-checks.md) for bash implementation.

If any missing, BLOCK with message guiding user to run `/plan [PluginName]`.
```

### Content to Extract to assets/

#### Extraction #3: state-management.md lines 180-257 → assets/decision-menus.json
**Description:** Context-specific decision menu options for each stage
**Reason:** 78 lines of examples that are only needed when presenting menus
**Token savings:** ~1100 tokens (progressive disclosure)

**Current (state-management.md):**
```javascript
**After Stage 2 (Foundation):**
[
  { label: "Continue to Stage 3", recommended: true },
  { label: "Review foundation code" },
  { label: "Test build manually" },
  ...
]

**After Stage 3 (Shell):**
[
  { label: "Continue to Stage 4", recommended: true },
  ...
]
```

**After extraction (assets/decision-menus.json):**
```json
{
  "stage_2_complete": {
    "options": [
      { "label": "Continue to Stage 3", "recommended": true },
      { "label": "Review foundation code" },
      { "label": "Test build manually" },
      { "label": "Review CMakeLists.txt" },
      { "label": "Pause here" }
    ]
  },
  "stage_3_complete": {
    "options": [
      { "label": "Continue to Stage 4", "recommended": true },
      ...
    ]
  }
}
```

**Updated state-management.md:**
```markdown
### generateContextualOptions(context)

**Purpose:** Generate situation-specific menu options after subagent completion.

Load menu configurations from `assets/decision-menus.json` and customize based on context.
```

### XML Tag Opportunities

#### Opportunity #1: Lines 32-51 needs nested enforcement structure
**Current:**
```markdown
**CRITICAL ORCHESTRATION RULES:**
1. Stages 2-5 MUST use Task tool to invoke subagents - NEVER implement directly
2. After EVERY subagent return (whether full stage or phase completion), orchestrator MUST:
   - Commit all changes with git
   - Update .continue-here.md with current state
   ...
```

**After:**
```xml
<orchestration_rules>
  <delegation_rule stages="2-5" tool="Task" violation="BLOCKS_EXECUTION">
    Stages 2-5 MUST invoke subagents via Task tool. NEVER implement directly in orchestrator context.

    Violation handling: STOP execution and present error to user.
  </delegation_rule>

  <checkpoint_protocol frequency="after_every_subagent" auto_proceed="NEVER">
    After EVERY subagent return (whether full stage or phase completion), orchestrator MUST:
    1. Commit all changes with git
    2. Update .continue-here.md with current state
    3. Update PLUGINS.md status
    4. Update plan.md if phased implementation
    5. Present numbered decision menu
    6. WAIT for user response

    This applies to:
    - Simple plugins (complexity ≤2): After stages 2, 3, 4, 5, 6
    - Complex plugins (complexity ≥3): After stages 2, 3 AND after EACH DSP/GUI phase
  </checkpoint_protocol>

  <state_requirement>
    All subagents receive Required Reading (juce8-critical-patterns.md) to prevent repeat mistakes.
  </state_requirement>

  <handoff_protocol>
    Subagents NEVER commit - they only implement and return JSON report.
    Orchestrator handles all state management and git operations.
  </handoff_protocol>
</orchestration_rules>
```

#### Opportunity #2: Lines 224-273 needs checkpoint enforcement wrapper
**Current:**
```javascript
// CHECKPOINT: Commit, update state, present menu
await commitStage(pluginName, currentStage, result.description)
await updateHandoff(pluginName, currentStage + 1, result.completed, result.nextSteps)
await updatePluginStatus(pluginName, `🚧 Stage ${currentStage}`)
await updatePluginTimeline(pluginName, currentStage, result.description)

const choice = presentDecisionMenu({...})
```

**After:**
```xml
<checkpoint_enforcement>
  <step order="1" required="true" function="commitStage(pluginName, currentStage, result.description)">
    Auto-commit all changes from subagent completion with standardized message format.
    If git fails, warn user but continue (non-blocking).
  </step>

  <step order="2" required="true" function="updateHandoff(pluginName, currentStage + 1, result.completed, result.nextSteps)">
    Update .continue-here.md with:
    - New stage number
    - Timestamp
    - Next action (which subagent to invoke)
    - Completion summary
  </step>

  <step order="3" required="true" function="updatePluginStatus(pluginName, `🚧 Stage ${currentStage}`)">
    Update PLUGINS.md status emoji to reflect current stage.
    Validate state transition is legal before updating.
  </step>

  <step order="4" required="true" function="updatePluginTimeline(pluginName, currentStage, result.description)">
    Append timeline entry to PLUGINS.md lifecycle history.
  </step>

  <step order="5" required="true" blocking="true" function="presentDecisionMenu({...})">
    Present numbered decision menu with context-appropriate options.
    WAIT for user response - NEVER auto-proceed to next stage.
  </step>
</checkpoint_enforcement>

<!-- Pseudocode showing enforcement: -->
async function runCheckpoint(pluginName, stage, result) {
  for (const step of checkpointSteps) {
    if (step.required) {
      await executeStep(step);
      if (step.blocking) {
        // Must wait for user input before proceeding
        return await waitForUserChoice();
      }
    }
  }
}
```

#### Opportunity #3: Lines 108-135 needs decision gate wrapper
**Current:**
```markdown
**When invoked via context-resume skill or /continue command:**

1. **Check if handoff file exists:**
2. **Parse handoff metadata:**
3. **Determine resume behavior:**
4. **Always use orchestration pattern:**
```

**After:**
```xml
<decision_gate type="resume_or_fresh_start">
  <condition check="handoff_file_exists">
    IF plugins/${PLUGIN_NAME}/.continue-here.md exists:
      - Parse YAML frontmatter
      - Extract current_stage, next_action, next_phase
      - Resume at specified stage/phase
    ELSE:
      - Start fresh at Stage 2
      - No handoff context available
  </condition>

  <routing_logic>
    Based on current_stage value:
    - Stage 2 → invoke foundation-agent
    - Stage 3 → invoke shell-agent
    - Stage 4 → invoke dsp-agent
    - Stage 5 → invoke gui-agent
    - Stage 6 → execute validation

    If next_phase is set: Resume phased implementation at specified phase.
  </routing_logic>
</decision_gate>
```

### Instruction Clarity Improvements

#### Line 54-77: Precondition checking logic could be more structured
**Current:**
```markdown
**Before starting, verify contracts from plugin-planning:**

1. Check for required contract files:

```bash
test -f "plugins/$PLUGIN_NAME/.ideas/architecture.md"
test -f "plugins/$PLUGIN_NAME/.ideas/plan.md"
test -f "plugins/$PLUGIN_NAME/.ideas/creative-brief.md"
```

If any missing, BLOCK with message:
```

**After:**
```xml
<preconditions>
  <contract_verification required="true" blocking="true">
    Before starting Stage 2, verify these contract files exist:

    <required_file path="plugins/$PLUGIN_NAME/.ideas/architecture.md" created_by="Stage 0"/>
    <required_file path="plugins/$PLUGIN_NAME/.ideas/plan.md" created_by="Stage 1"/>
    <required_file path="plugins/$PLUGIN_NAME/.ideas/creative-brief.md" created_by="ideation"/>

    <on_missing_files action="BLOCK">
      Display error message:
      "[PluginName] is missing required planning documents.

      Missing files will be listed here:
      - architecture.md (from Stage 0)
      - plan.md (from Stage 1)
      - creative-brief.md (from ideation)

      Run /plan [PluginName] to complete planning stages 0-1."
    </on_missing_files>
  </contract_verification>

  <status_verification required="true" blocking="true">
    Read PLUGINS.md and verify status is appropriate:
    - ✓ 🚧 Stage 1 → OK to proceed (just finished planning)
    - ✓ 🚧 Stage N (N ≥ 2) → OK to proceed (resuming implementation)
    - ✗ 💡 Ideated → BLOCK: "Run /plan [PluginName]"
    - ✗ ✅ Working → BLOCK: "Use /improve [PluginName]"
    - ✗ 📦 Installed → BLOCK: "Use /improve [PluginName]"
  </status_verification>
</preconditions>
```

#### Lines 384-411: "Notes for Claude" section needs restructuring
**Current:**
```markdown
## Notes for Claude

**CRITICAL ORCHESTRATION REQUIREMENTS:**

1. **NEVER implement stages 2-5 directly** - You MUST use Task tool to invoke subagents
2. **ALWAYS present decision menu after subagent completes** - User must confirm next action
...

**When executing this skill:**
1. Read contracts (architecture.md, plan.md) before starting
2. Verify preconditions block if contracts missing
...

**Common pitfalls to AVOID:**
- ❌ Implementing stage logic directly in orchestrator (ALWAYS use Task tool)
- ❌ Auto-proceeding without user confirmation (ALWAYS wait for menu choice)
...
```

**After:**
```xml
<execution_guidance>
  <critical_reminders>
    <reminder priority="CRITICAL" ref="delegation_rule">
      NEVER implement stages 2-5 directly - MUST use Task tool to invoke subagents
    </reminder>

    <reminder priority="CRITICAL" ref="checkpoint_protocol">
      ALWAYS present decision menu after subagent completes - user MUST confirm next action
    </reminder>

    <reminder priority="HIGH">
      ALWAYS commit after each stage using commitStage() from state-management.md
    </reminder>

    <reminder priority="HIGH">
      ALWAYS update state files (.continue-here.md and PLUGINS.md) after every stage
    </reminder>

    <reminder priority="HIGH">
      ALWAYS inject Required Reading (juce8-critical-patterns.md) to all subagents
    </reminder>
  </critical_reminders>

  <execution_checklist>
    When executing this skill:
    1. Read contracts (architecture.md, plan.md) before starting
    2. Verify preconditions - block if contracts missing
    3. Use Task tool for ALL stages 2-5 - no exceptions
    4. Stage reference files contain subagent prompts, not direct implementation instructions
    5. Decision menus use inline numbered lists, not AskUserQuestion tool
    6. Handoff files preserve orchestration state across sessions
    7. Build failures bubble up from subagents to orchestrator for menu presentation
  </execution_checklist>

  <anti_patterns>
    Common pitfalls to AVOID:

    <anti_pattern severity="CRITICAL">
      ❌ Implementing stage logic directly in orchestrator
      ✓ ALWAYS use Task tool to invoke appropriate subagent
    </anti_pattern>

    <anti_pattern severity="CRITICAL">
      ❌ Auto-proceeding without user confirmation
      ✓ ALWAYS wait for menu choice after presenting options
    </anti_pattern>

    <anti_pattern severity="HIGH">
      ❌ Not updating handoff file after stage completes
      ✓ Update .continue-here.md immediately after subagent returns
    </anti_pattern>

    <anti_pattern severity="HIGH">
      ❌ Skipping decision menu after subagent returns
      ✓ Present context-appropriate menu at every checkpoint
    </anti_pattern>

    <anti_pattern severity="MEDIUM">
      ❌ Proceeding to next stage when tests fail
      ✓ Present investigation menu and wait for user decision
    </anti_pattern>

    <anti_pattern severity="MEDIUM">
      ❌ Not injecting Required Reading to subagents
      ✓ Always pass juce8-critical-patterns.md to prevent repeat mistakes
    </anti_pattern>
  </anti_patterns>
</execution_guidance>
```

### Verbosity Reductions

#### Lines 176-223: Dispatcher pseudocode (48 lines → 8 lines summary)
**Current wordcount:** ~650 words
**Target wordcount:** ~100 words

**Before:**
```javascript
async function dispatchStage(pluginName, stageNumber) {
  // Check preconditions
  const preconditionCheck = checkStagePreconditions(pluginName, stageNumber)

  if (!preconditionCheck.allowed) {
    console.log(`✗ BLOCKED: ${preconditionCheck.reason}`)
    console.log(`Action: ${preconditionCheck.action}`)
    return { status: 'blocked', reason: preconditionCheck.reason }
  }

  // ALWAYS invoke subagents via Task tool for stages 2-5
  switch(stageNumber) {
    case 2:
      // Invoke foundation-agent subagent
      return await invokeSubagent('foundation-agent', {
        pluginName,
        contracts: loadContracts(pluginName),
        requiredReading: 'juce8-critical-patterns.md'
      })
    case 3:
      // Invoke shell-agent subagent
      return await invokeSubagent('shell-agent', {
        pluginName,
        contracts: loadContracts(pluginName),
        requiredReading: 'juce8-critical-patterns.md'
      })
    case 4:
      // Invoke dsp-agent subagent
      return await invokeSubagent('dsp-agent', {
        pluginName,
        contracts: loadContracts(pluginName),
        requiredReading: 'juce8-critical-patterns.md'
      })
    case 5:
      // Invoke gui-agent subagent
      return await invokeSubagent('gui-agent', {
        pluginName,
        contracts: loadContracts(pluginName),
        requiredReading: 'juce8-critical-patterns.md'
      })
    case 6:
      // Can run directly or invoke validator subagent
      return executeStage6Validation(pluginName)  // See references/stage-6-validation.md
    default:
      return { status: 'error', reason: `Invalid stage: ${stageNumber}` }
  }
}
```

**After (condensed):**
```xml
<dispatcher_pattern>
  The orchestrator routes stages to subagents using this logic:

  1. Check preconditions → If failed, BLOCK with reason
  2. Route to subagent based on stage number:
     - Stage 2 → foundation-agent
     - Stage 3 → shell-agent
     - Stage 4 → dsp-agent
     - Stage 5 → gui-agent
     - Stage 6 → validator (or direct execution)
  3. Pass contracts and Required Reading to subagent
  4. Wait for subagent completion

  See [references/dispatcher-pattern.md](references/dispatcher-pattern.md) for full pseudocode.
</dispatcher_pattern>
```

#### Lines 287-300: Stage reference files explanation (14 lines → 5 lines)
**Current wordcount:** ~120 words
**Target wordcount:** ~40 words

**Before:**
```markdown
## Stage Reference Files

Implementation stages reference files (Stages 0-1 removed, now in plugin-planning skill):

- **[Stage 2: Foundation](references/stage-2-foundation.md)** - Create build system, verify compilation (foundation-agent)
- **[Stage 3: Shell](references/stage-3-shell.md)** - Implement APVTS with all parameters (shell-agent)
- **[Stage 4: DSP](references/stage-4-dsp.md)** - Implement audio processing (dsp-agent)
- **[Stage 5: GUI](references/stage-5-gui.md)** - Integrate WebView UI with parameter bindings (gui-agent)
- **[Stage 6: Validation](references/stage-6-validation.md)** - Factory presets, pluginval, CHANGELOG (direct or validator)
- **[State Management](references/state-management.md)** - State machine, git commits, handoffs, decision menus

**Note:** Stage reference files contain subagent prompts and context. The orchestrator reads these files to construct Task tool invocations but never implements stage logic directly.
```

**After:**
```xml
<reference_files>
  Each stage has a reference file containing subagent prompt templates:

  - [stage-2-foundation.md](references/stage-2-foundation.md) - foundation-agent
  - [stage-3-shell.md](references/stage-3-shell.md) - shell-agent
  - [stage-4-dsp.md](references/stage-4-dsp.md) - dsp-agent
  - [stage-5-gui.md](references/stage-5-gui.md) - gui-agent
  - [stage-6-validation.md](references/stage-6-validation.md) - validator
  - [state-management.md](references/state-management.md) - State machine functions

  <usage_pattern>
    Orchestrator reads reference files → constructs Task invocation → NEVER implements directly
  </usage_pattern>
</reference_files>
```

### Critical Sequence Enforcement

#### Lines 32-51: Orchestration rules need enforcement attributes
**Current:** Natural language emphasis (MUST, NEVER, ALWAYS)
**Suggested:** XML with violation handling

```xml
<orchestration_rules enforcement_level="STRICT">
  <delegation_rule
    id="subagent-dispatch-only"
    stages="2-5"
    tool="Task"
    violation="IMMEDIATE_STOP">

    Stages 2-5 MUST be delegated to subagents via Task tool.
    This skill is a PURE ORCHESTRATOR - it NEVER implements plugin code directly.

    <enforcement>
      IF stage in [2,3,4,5] AND action != "invoke_subagent_via_Task":
        STOP execution
        DISPLAY error: "Stage {stage} requires subagent delegation. Use Task tool to invoke {subagent_name}."
    </enforcement>

    <valid_delegations>
      - Stage 2: foundation-agent
      - Stage 3: shell-agent
      - Stage 4: dsp-agent
      - Stage 5: gui-agent
    </valid_delegations>
  </delegation_rule>

  <checkpoint_protocol
    id="stage-completion-checkpoint"
    frequency="after_every_subagent"
    auto_proceed="NEVER"
    violation="WORKFLOW_CORRUPTION">

    After EVERY subagent return (whether full stage or phase completion), orchestrator MUST execute checkpoint sequence.

    <critical_sequence enforce_order="true">
      <step order="1" required="true">Commit all changes with git</step>
      <step order="2" required="true">Update .continue-here.md with current state</step>
      <step order="3" required="true">Update PLUGINS.md status</step>
      <step order="4" required="true">Update plan.md if phased implementation</step>
      <step order="5" required="true">Present numbered decision menu</step>
      <step order="6" required="true" blocking="true">WAIT for user response</step>
    </critical_sequence>

    <enforcement>
      All 6 steps must complete in order before proceeding.
      Step 6 is blocking - NEVER auto-proceed to next stage.
    </enforcement>

    <applies_to>
      - Simple plugins (complexity ≤2): After stages 2, 3, 4, 5, 6
      - Complex plugins (complexity ≥3): After stages 2, 3 AND after EACH DSP/GUI phase (4.1, 4.2, 4.3+, 5.1, 5.2, 5.3+), then 6
    </applies_to>
  </checkpoint_protocol>

  <handoff_protocol id="subagent-orchestrator-handoff">
    Subagents NEVER commit - they only implement and return JSON report.
    Orchestrator handles all state management and git operations.

    <handoff_format>
      Subagent returns JSON:
      {
        "status": "success" | "error",
        "stage": 2-6,
        "completionStatement": "...",
        "filesCreated": [...],
        "nextSteps": [...]
      }
    </handoff_format>
  </handoff_protocol>

  <state_requirement id="required-reading-injection">
    All subagents (stages 2-5) MUST receive Required Reading file to prevent repeat mistakes.

    <enforcement>
      BEFORE invoking subagent via Task tool:
      1. Read troubleshooting/patterns/juce8-critical-patterns.md
      2. Inject content into subagent prompt
      3. Verify injection succeeded
    </enforcement>
  </state_requirement>
</orchestration_rules>
```

#### state-management.md lines 502-598: Stage boundary protocol needs enforcement wrapper

**Current:** Procedural description
**Suggested:** Structured enforcement with validation hooks

```xml
<stage_boundary_protocol>
  <trigger>When subagent completes and returns to orchestrator</trigger>

  <sequence enforce_order="true">
    <action order="1" actor="orchestrator">
      Display completion statement:
      "✓ Stage [N] complete: [description]"
    </action>

    <action order="2" actor="orchestrator" conditional="stages_4_5_6_only">
      Run automated tests:
      - Invoke plugin-testing skill
      - IF tests fail: STOP, show results, present investigation menu
      - IF tests pass: Continue to next action
    </action>

    <action order="3" actor="orchestrator" required="true">
      Auto-commit changes:
      ```bash
      git add plugins/[PluginName]/Source/
      git add plugins/[PluginName]/.ideas/
      git add plugins/[PluginName]/.continue-here.md
      git add PLUGINS.md
      git commit -m "feat: [Plugin] Stage [N] - [description]"
      ```
      IF commit fails: Warn user, continue anyway (non-blocking)
    </action>

    <action order="4" actor="orchestrator" required="true">
      Update .continue-here.md:
      - stage: [new stage number]
      - phase: [phase number if complex]
      - last_updated: [timestamp]
      - next_action: [which subagent to invoke next]
      - next_phase: [phase number if phased implementation]
    </action>

    <action order="5" actor="orchestrator" required="true">
      Update PLUGINS.md:
      - Status: 🚧 Stage [N]
      - Last Updated: [date]
      - Lifecycle Timeline: Append new entry
    </action>

    <action order="6" actor="orchestrator" required="true" blocking="true">
      Present decision menu with context-appropriate options
    </action>

    <action order="7" actor="orchestrator" required="true" blocking="true">
      WAIT for user response - NEVER auto-proceed
    </action>
  </sequence>

  <responsibilities>
    <actor name="subagent">
      - Complete stage work
      - Report completion status to orchestrator
      - Return control to orchestrator for checkpoint
      - NEVER commit changes
    </actor>

    <actor name="orchestrator">
      - Commit changes
      - Update state files (.continue-here.md, PLUGINS.md)
      - Present decision menu
      - Wait for user input
      - Invoke next subagent based on user choice
    </actor>
  </responsibilities>

  <critical_invariant>
    Orchestrator NEVER auto-proceeds to next stage without user confirmation.
  </critical_invariant>
</stage_boundary_protocol>
```

## Examples of Issues Found

### Example 1: Orchestration Rules Buried in Prose

**BEFORE (Lines 32-51):**
```markdown
**CRITICAL ORCHESTRATION RULES:**
1. Stages 2-5 MUST use Task tool to invoke subagents - NEVER implement directly
2. After EVERY subagent return (whether full stage or phase completion), orchestrator MUST:
   - Commit all changes with git
   - Update .continue-here.md with current state
   - Update PLUGINS.md status
   - Update plan.md if phased implementation
   - Present numbered decision menu
   - WAIT for user response

   This applies to:
   - Simple plugins (complexity ≤2): After each stage completion (2, 3, 4, 5, 6)
   - Complex plugins (complexity ≥3): After stages 2, 3 AND after EACH DSP/GUI phase (4.1, 4.2, 4.3+, 5.1, 5.2, 5.3+), then 6

   Note: Number of phases determined by plan.md - could be 4.1-4.2, or 4.1-4.3, or more depending on complexity

3. Stage 6 can run directly in orchestrator or via validator subagent
4. All subagents receive Required Reading (juce8-critical-patterns.md) to prevent repeat mistakes
5. Subagents NEVER commit - they only implement and return JSON report
```

**AFTER:**
```xml
<orchestration_rules enforcement_level="STRICT">
  <delegation_rule id="subagent-dispatch-only" stages="2-5" tool="Task" violation="IMMEDIATE_STOP">
    Stages 2-5 MUST be delegated to subagents via Task tool.
    This skill is a PURE ORCHESTRATOR - it NEVER implements plugin code directly.

    Valid delegations:
    - Stage 2: foundation-agent
    - Stage 3: shell-agent
    - Stage 4: dsp-agent
    - Stage 5: gui-agent

    Stage 6 can run directly or via validator subagent (optional delegation).
  </delegation_rule>

  <checkpoint_protocol id="stage-completion-checkpoint" frequency="after_every_subagent" auto_proceed="NEVER">
    After EVERY subagent return, orchestrator MUST execute checkpoint sequence:

    <critical_sequence enforce_order="true">
      <step>Commit all changes with git</step>
      <step>Update .continue-here.md with current state</step>
      <step>Update PLUGINS.md status</step>
      <step>Update plan.md if phased implementation</step>
      <step>Present numbered decision menu</step>
      <step blocking="true">WAIT for user response</step>
    </critical_sequence>

    Applies to:
    - Simple plugins (complexity ≤2): After stages 2, 3, 4, 5, 6
    - Complex plugins (complexity ≥3): After stages 2, 3 AND after EACH DSP/GUI phase

    Note: Phase count determined by plan.md (varies by complexity)
  </checkpoint_protocol>

  <handoff_protocol>
    Subagents NEVER commit - they only implement and return JSON report.
    Orchestrator handles all state management.
  </handoff_protocol>

  <state_requirement>
    All subagents receive Required Reading (juce8-critical-patterns.md) to prevent repeat mistakes.
  </state_requirement>
</orchestration_rules>
```

**Impact:** Structural enforcement makes rules machine-parseable, reduces risk of Claude missing critical requirements during context compression.

### Example 2: Repeated Delegation Rule Without Reference Structure

**BEFORE (Lines 144-151, 386, 404):**

Lines 144-151:
```markdown
### CRITICAL RULE: Never Implement Directly

**This skill is a PURE ORCHESTRATOR:**
- Stages 2-5 MUST be delegated to subagents
- NEVER write plugin code directly in this skill
- ALWAYS use Task tool to invoke subagents
- Only Stage 6 validation can optionally run directly
```

Lines 386-387:
```markdown
1. **NEVER implement stages 2-5 directly** - You MUST use Task tool to invoke subagents
```

Lines 404-405:
```markdown
- ❌ Implementing stage logic directly in orchestrator (ALWAYS use Task tool)
```

**AFTER:**

Define once at top:
```xml
<orchestration_rules>
  <delegation_rule id="stages-2-5-subagent-only" violation="IMMEDIATE_STOP">
    Stages 2-5 MUST be delegated to subagents via Task tool.
    This skill is a PURE ORCHESTRATOR - it NEVER implements plugin code directly.

    Valid delegation targets:
    - Stage 2: foundation-agent
    - Stage 3: shell-agent
    - Stage 4: dsp-agent
    - Stage 5: gui-agent

    Stage 6 can optionally run directly in orchestrator.
  </delegation_rule>
</orchestration_rules>
```

Reference throughout:
```xml
<!-- Line 144 replacement -->
<enforcement_reminder ref="stages-2-5-subagent-only" severity="CRITICAL"/>

<!-- Line 386 replacement -->
<execution_guidance>
  <critical_reminders>
    <reminder ref="stages-2-5-subagent-only" priority="CRITICAL">
      NEVER implement stages 2-5 directly - MUST use Task tool to invoke subagents
    </reminder>
  </critical_reminders>
</execution_guidance>

<!-- Line 404 replacement -->
<anti_patterns>
  <anti_pattern ref="stages-2-5-subagent-only" severity="CRITICAL">
    ❌ Implementing stage logic directly in orchestrator
    ✓ ALWAYS use Task tool to invoke appropriate subagent
  </anti_pattern>
</anti_patterns>
```

**Impact:** Single source of truth, easier maintenance, referential structure makes rule more salient.

### Example 3: Checkpoint Sequence Without Enforcement

**BEFORE (Lines 224-273):**
```javascript
async function runWorkflow(pluginName, startStage = 2) {
  let currentStage = startStage
  let shouldContinue = true

  while (shouldContinue && currentStage <= 6) {
    console.log(`\n━━━ Stage ${currentStage} ━━━\n`)

    // ALWAYS invoke subagent (never implement directly)
    const result = await dispatchStage(pluginName, currentStage)

    if (result.status === 'blocked' || result.status === 'error') {
      console.log(`\nWorkflow blocked: ${result.reason}`)
      return result
    }

    // CHECKPOINT: Commit, update state, present menu
    await commitStage(pluginName, currentStage, result.description)
    await updateHandoff(pluginName, currentStage + 1, result.completed, result.nextSteps)
    await updatePluginStatus(pluginName, `🚧 Stage ${currentStage}`)
    await updatePluginTimeline(pluginName, currentStage, result.description)

    // Present decision menu and WAIT for user
    const choice = presentDecisionMenu({
      stage: currentStage,
      completionStatement: result.completionStatement,
      pluginName: pluginName
    })

    // Handle user choice
    if (choice === 'continue' || choice === 1) {
      currentStage++
    } else if (choice === 'pause') {
      console.log("\n✓ Workflow paused. Resume anytime with /continue")
      shouldContinue = false
    } else {
      // Handle other menu options (review, test, etc.)
      handleMenuChoice(choice, pluginName, currentStage)
    }
  }

  if (currentStage > 6) {
    console.log("\n✓ All stages complete!")
    await updatePluginStatus(pluginName, '✅ Working')
  }
}
```

**AFTER:**
```xml
<workflow_loop>
  <stage_iteration from="2" to="6">
    <dispatch_phase>
      Display: "━━━ Stage ${currentStage} ━━━"

      <delegation_enforcement ref="stages-2-5-subagent-only">
        Invoke subagent via Task tool (NEVER implement directly)
      </delegation_enforcement>

      IF result.status == 'blocked' OR 'error':
        Display error and return (workflow blocked)
    </dispatch_phase>

    <checkpoint_phase>
      <checkpoint_enforcement enforce_order="true">
        <step order="1">commitStage(pluginName, currentStage, result.description)</step>
        <step order="2">updateHandoff(pluginName, currentStage + 1, result.completed, result.nextSteps)</step>
        <step order="3">updatePluginStatus(pluginName, `🚧 Stage ${currentStage}`)</step>
        <step order="4">updatePluginTimeline(pluginName, currentStage, result.description)</step>
        <step order="5" blocking="true">
          presentDecisionMenu({ stage, completionStatement, pluginName })
        </step>
      </checkpoint_enforcement>
    </checkpoint_phase>

    <decision_gate blocking="true">
      WAIT for user response (NEVER auto-proceed)

      <routing>
        IF choice == 'continue' OR choice == 1:
          currentStage++
        ELSE IF choice == 'pause':
          Display: "✓ Workflow paused. Resume anytime with /continue"
          Exit workflow loop
        ELSE:
          handleMenuChoice(choice, pluginName, currentStage)
      </routing>
    </decision_gate>
  </stage_iteration>

  <completion_check>
    IF currentStage > 6:
      Display: "✓ All stages complete!"
      updatePluginStatus(pluginName, '✅ Working')
  </completion_check>
</workflow_loop>
```

**Impact:** Structured enforcement makes checkpoint sequence machine-parseable, validates step ordering, ensures blocking behavior at decision gates.

### Example 4: Decision Menu Format Not Structurally Enforced

**BEFORE (state-management.md lines 137-173):**
```markdown
**Format - Inline Numbered List (NOT AskUserQuestion):**

```
✓ [Completion statement]

What's next?
1. [Primary action] (recommended)
2. [Secondary action]
3. [Discovery feature] ← User discovers [capability]
4. [Alternative path]
5. [Escape hatch]
6. Other

Choose (1-6): _
```

**Implementation:**
1. Generate context-appropriate options (see generateContextualOptions below)
2. Format as inline numbered list
3. Display to user
4. Wait for response (number, keyword shortcut, or "Other")
5. Parse response
6. Execute chosen action or re-present menu if invalid
```

**AFTER:**
```xml
<decision_menu_format>
  <display_format type="inline_numbered_list" forbidden_tool="AskUserQuestion">
    Structure:
    ```
    ✓ [Completion statement]

    What's next?
    1. [Primary action] (recommended)
    2. [Secondary action]
    3. [Discovery option] ← User discovers [feature]
    4. [Alternative path]
    5. [Escape hatch]
    6. Other

    Choose (1-6): _
    ```
  </display_format>

  <rendering_sequence enforce_order="true">
    <step order="1">Load context-appropriate options from assets/decision-menus.json</step>
    <step order="2">Format as inline numbered list (see format above)</step>
    <step order="3">Display to user</step>
    <step order="4" blocking="true">Wait for response (number, keyword, or "Other")</step>
    <step order="5">Parse response and validate</step>
    <step order="6">
      IF valid: Execute chosen action
      ELSE: Display error and re-present menu
    </step>
  </rendering_sequence>

  <keyword_shortcuts>
    - "continue" → Option 1 (primary action)
    - "pause" → Pause option (creates checkpoint)
    - "review" → Review option (show code/context)
  </keyword_shortcuts>

  <other_handling>
    When user selects "Other":
    1. Prompt: "What would you like to do?"
    2. Accept free-form text
    3. Process custom request
    4. Re-present decision menu after completing request
  </other_handling>

  <progressive_disclosure>
    Use discovery markers to surface hidden features:
    - "Save as template ← Add to UI template library"
    - "Design sync ← Validate brief matches mockup"
    - "/research ← Deep investigation for complex problems"
  </progressive_disclosure>
</decision_menu_format>
```

**Impact:** Structured format makes it impossible to accidentally use AskUserQuestion tool, enforces keyword shortcut handling, documents progressive disclosure pattern.

### Example 5: Precondition Checking Without Decision Gate Structure

**BEFORE (Lines 60-99):**
```markdown
**Before starting, verify contracts from plugin-planning:**

1. Check for required contract files:

```bash
test -f "plugins/$PLUGIN_NAME/.ideas/architecture.md"
test -f "plugins/$PLUGIN_NAME/.ideas/plan.md"
test -f "plugins/$PLUGIN_NAME/.ideas/creative-brief.md"
```

If any missing, BLOCK with message:

```
[PluginName] is missing required planning documents.

Missing files will be listed here:
- architecture.md (from Stage 0)
- plan.md (from Stage 1)
- creative-brief.md (from ideation)

Run /plan [PluginName] to complete planning stages 0-1.
```

2. Read PLUGINS.md to verify status:

```bash
grep "^### $PLUGIN_NAME$" PLUGINS.md
```

3. Verify status is appropriate:

   - If status = 🚧 Stage 1 → OK to proceed (just finished planning)
   - If status = 🚧 Stage N (N ≥ 2) → OK to proceed (resuming implementation)
   - If status = 💡 Ideated → BLOCK with message:
     ```
     [PluginName] needs planning before implementation.
     Run /plan [PluginName] to complete stages 0-1.
     ```
   - If status = ✅ Working or 📦 Installed → BLOCK with message:
     ```
     [PluginName] is already complete.
     Use /improve [PluginName] to make changes.
     ```
```

**AFTER:**
```xml
<preconditions blocking="true">
  <decision_gate type="contract_verification">
    <required_contracts>
      <contract path="plugins/$PLUGIN_NAME/.ideas/architecture.md" created_by="Stage 0"/>
      <contract path="plugins/$PLUGIN_NAME/.ideas/plan.md" created_by="Stage 1"/>
      <contract path="plugins/$PLUGIN_NAME/.ideas/creative-brief.md" created_by="ideation"/>
    </required_contracts>

    <validation_sequence>
      <step>Check existence of all required contract files</step>
      <step>
        IF any missing:
          BLOCK execution
          Display error:
          "[PluginName] is missing required planning documents.

          Missing files will be listed here:
          - architecture.md (from Stage 0)
          - plan.md (from Stage 1)
          - creative-brief.md (from ideation)

          Run /plan [PluginName] to complete planning stages 0-1."
      </step>
    </validation_sequence>
  </decision_gate>

  <decision_gate type="status_verification">
    <validation_sequence>
      <step>Read PLUGINS.md and extract current status for plugin</step>
      <step>
        Evaluate status against allowed states:

        <allowed_state status="🚧 Stage 1">
          OK to proceed (just finished planning)
        </allowed_state>

        <allowed_state status="🚧 Stage N" condition="N >= 2">
          OK to proceed (resuming implementation)
        </allowed_state>

        <blocked_state status="💡 Ideated">
          BLOCK with message:
          "[PluginName] needs planning before implementation.
          Run /plan [PluginName] to complete stages 0-1."
        </blocked_state>

        <blocked_state status="✅ Working">
          BLOCK with message:
          "[PluginName] is already complete.
          Use /improve [PluginName] to make changes."
        </blocked_state>

        <blocked_state status="📦 Installed">
          BLOCK with message:
          "[PluginName] is already complete.
          Use /improve [PluginName] to make changes."
        </blocked_state>
      </step>
    </validation_sequence>
  </decision_gate>
</preconditions>
```

**Impact:** Decision gates make precondition validation structural rather than procedural, enforce blocking behavior, make allowed/blocked states explicit.

## Estimated Impact

### Context Reduction
- **Dispatcher pseudocode extraction:** 750 tokens
- **Precondition bash extraction:** 270 tokens
- **Decision menu examples to JSON:** 1100 tokens
- **XML structure overhead:** +200 tokens (but provides structural enforcement)
- **Net savings:** ~1920 tokens (12% reduction)

**Baseline:** SKILL.md (411 lines) + state-management.md (680 lines) ≈ 16,000 tokens
**After optimization:** ~14,080 tokens (12% reduction)

### Comprehension Improvement: **HIGH**

**Reasons:**
1. **XML enforcement tags make critical sequences machine-parseable**
   - Claude can't accidentally skip checkpoint steps if wrapped in `<checkpoint_enforcement>`
   - Delegation rules become structural constraints, not advisory guidelines
   - Decision gates enforce blocking behavior at validation points

2. **Context compression is less likely to drop critical invariants**
   - XML tags act as semantic anchors during summarization
   - `<critical_sequence>` wrapper protects ordering requirements
   - `violation="IMMEDIATE_STOP"` attributes make consequences explicit

3. **Clear separation between orchestrator logic and subagent prompts**
   - `<orchestrator_logic>` vs `<subagent_prompt_template>` tags
   - Reduces confusion about whether to execute or delegate
   - Makes dispatcher pattern more obvious

4. **Single source of truth for repeated rules**
   - `<delegation_rule id="...">` defined once, referenced throughout
   - Eliminates inconsistency risk from maintaining 3+ copies of same rule
   - Makes updates safer (change once, applies everywhere)

**Risk mitigation:**
- Without XML enforcement: Claude might skip checkpoint steps under token pressure → workflow state corruption
- With XML enforcement: Structural tags survive context compression → workflow invariants preserved

### Maintenance Improvement: **HIGH**

**Reasons:**
1. **Referential structure eliminates duplication**
   - Delegation rule appears once, referenced via `ref="stages-2-5-subagent-only"`
   - Change propagates automatically to all references
   - No hunt-and-replace across multiple sections

2. **Data-driven decision menus**
   - Menu options stored in `assets/decision-menus.json`
   - Easy to add new stage-specific menus without touching skill logic
   - Version control tracks menu changes separately

3. **Extracted pseudocode in references/**
   - Implementation details separated from orchestration rules
   - Can update dispatcher pattern without touching main SKILL.md
   - Easier to test pseudocode examples in isolation

4. **Structured anti-patterns**
   - `<anti_pattern ref="..." severity="CRITICAL">` makes severity explicit
   - Can programmatically validate skill doesn't violate its own rules
   - Easy to add new anti-patterns to checklist

5. **XML schema enables validation**
   - Could build linter to validate skill structure
   - Detect missing enforcement tags automatically
   - Verify all critical sequences have proper wrappers

**Time savings:**
- Current: ~20 minutes to safely update delegation rule (find all 3+ instances, ensure consistency)
- After: ~2 minutes (update once in `<delegation_rule>`, all references update automatically)

## Priority Recommendations

### 1. Add XML enforcement tags for orchestration rules and checkpoint protocol (CRITICAL)
**Priority:** CRITICAL
**Effort:** 30 minutes
**Impact:** Prevents catastrophic workflow failures

**What to do:**
- Wrap lines 32-51 (orchestration rules) in structured XML with nested `<delegation_rule>`, `<checkpoint_protocol>`, `<handoff_protocol>`, `<state_requirement>` tags
- Add `violation="IMMEDIATE_STOP"` attributes to delegation rule
- Wrap lines 224-273 (checkpoint enforcement) in `<checkpoint_enforcement>` with ordered steps
- Add `blocking="true"` attribute to decision menu step
- Wrap state-management.md lines 513-598 (stage boundary protocol) in `<stage_boundary_protocol>` with enforcement sequence

**Why this is critical:**
- Without structural enforcement, Claude might skip critical steps under token pressure
- Checkpoint protocol violations corrupt workflow state (missing .continue-here.md updates, PLUGINS.md drift)
- Delegation rule violations cause orchestrator to implement directly instead of invoking subagents
- XML structure survives context compression better than natural language emphasis

**Example transformation:**
See "Example 1: Orchestration Rules Buried in Prose" above for complete before/after.

### 2. Extract dispatcher pseudocode to references/dispatcher-pattern.md (HIGH)
**Priority:** HIGH
**Effort:** 15 minutes
**Impact:** Reduces main SKILL.md cognitive load, clarifies orchestrator vs subagent separation

**What to do:**
- Move lines 176-223 (JavaScript pseudocode) to new file `references/dispatcher-pattern.md`
- Replace with 8-line summary explaining dispatcher pattern conceptually
- Add note: "See references/dispatcher-pattern.md for full pseudocode implementation"
- Update line 300 note about reference files to mention dispatcher-pattern.md

**Why this is high priority:**
- 48 lines of pseudocode increases cognitive load when scanning SKILL.md
- Pseudocode might confuse Claude about whether to execute or use as guidance
- Better separation between "what" (orchestrate stages) and "how" (dispatcher implementation)
- Enables updating dispatcher logic without cluttering main skill file

**Token savings:** ~750 tokens

### 3. Extract decision menu examples to assets/decision-menus.json (MEDIUM)
**Priority:** MEDIUM
**Effort:** 20 minutes
**Impact:** Progressive disclosure, data-driven menus, easier maintenance

**What to do:**
- Move state-management.md lines 180-257 (decision menu examples by stage) to new file `assets/decision-menus.json`
- Structure as JSON object with keys for each context (stage_2_complete, stage_3_complete, build_failure, etc.)
- Update state-management.md to reference JSON asset: "Load menu configurations from assets/decision-menus.json"
- Add note about progressive disclosure pattern

**Why this is medium priority:**
- Progressive disclosure - only load menu options when actually presenting menu
- Makes menus data-driven - can add new contexts without touching skill logic
- Easier to maintain consistency across stage-specific menus
- Reduces token usage by ~1100 tokens (only loaded when needed)

**Example JSON structure:**
```json
{
  "stage_2_complete": {
    "completion_statement": "Foundation compiles successfully",
    "options": [
      { "label": "Continue to Stage 3", "recommended": true, "action": "next_stage" },
      { "label": "Review foundation code", "action": "review_code" },
      { "label": "Test build manually", "action": "manual_test" },
      { "label": "Review CMakeLists.txt", "action": "review_cmake" },
      { "label": "Pause here", "action": "pause" }
    ]
  },
  "build_failure": {
    "completion_statement": "Build failed at stage {stage}",
    "options": [
      { "label": "Investigate", "action": "deep_research", "discovery": "Run deep-research to find root cause" },
      { "label": "Show me the code", "action": "show_code" },
      { "label": "Show full build output", "action": "show_build_log" },
      { "label": "I'll fix it manually", "action": "manual_fix" }
    ]
  }
}
```

## Implementation Notes

### Files that would be created in references/
1. **dispatcher-pattern.md** (NEW)
   - Full JavaScript pseudocode from lines 176-223
   - Explanation of dispatcher pattern
   - Routing logic for stages 2-6
   - Precondition checking integration
   - ~50 lines

2. **precondition-checks.md** (NEW)
   - Bash scripts for contract verification
   - Status validation logic
   - Block message templates
   - ~40 lines

### Files that would be created in assets/
1. **decision-menus.json** (NEW)
   - Menu options for all stage completions (2, 3, 4, 5, 6)
   - Menu options for error conditions (build_failure, validation_failure)
   - Menu options for discovery features
   - Progressive disclosure markers
   - ~120 lines JSON

### Files that would be modified
1. **SKILL.md** (MAJOR UPDATE)
   - Add XML enforcement wrappers around lines 32-51 (orchestration rules)
   - Add XML enforcement around lines 224-273 (checkpoint protocol)
   - Extract pseudocode (lines 176-223) → replace with summary
   - Extract bash examples (lines 60-77) → replace with reference
   - Update precondition checking (lines 54-99) with decision gates
   - Update "Notes for Claude" section (lines 384-411) with structured guidance
   - Estimated new line count: ~350 lines (down from 411)

2. **references/state-management.md** (MODERATE UPDATE)
   - Wrap stage boundary protocol (lines 513-598) in `<stage_boundary_protocol>`
   - Extract decision menu examples (lines 180-257) → reference JSON asset
   - Add structured enforcement around checkpoint types (lines 502-598)
   - Update presentDecisionMenu documentation to reference JSON asset
   - Estimated new line count: ~600 lines (down from 680)

### Estimated time to implement all fixes
- **XML enforcement tags (CRITICAL):** 30 minutes
- **Extract dispatcher pseudocode (HIGH):** 15 minutes
- **Extract decision menus to JSON (MEDIUM):** 20 minutes
- **Extract precondition checks (MEDIUM):** 10 minutes
- **Update references in main SKILL.md:** 10 minutes
- **Testing and validation:** 15 minutes
- **Total:** ~100 minutes (1.7 hours)

### Implementation order
1. Add XML enforcement tags first (highest impact on comprehension)
2. Extract dispatcher pseudocode (clarifies orchestrator pattern)
3. Extract precondition checks (reduces clutter)
4. Extract decision menus to JSON (enables data-driven menus)
5. Update all references and verify consistency
6. Test by invoking skill and verifying behavior matches expectations
