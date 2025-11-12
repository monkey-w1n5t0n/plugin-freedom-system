---
name: plugin-workflow
description: Implementation orchestrator for stages 2-6 (Foundation through Validation)
allowed-tools:
  - Task # REQUIRED - All stages 2-5 MUST invoke subagents
  - Bash # For git commits
  - Read # For contracts
  - Write # For documentation
  - Edit # For state updates
preconditions:
  - architecture.md must exist (from /plan)
  - plan.md must exist (from /plan)
  - Status must be 🚧 Stage 1 OR resuming from 🚧 Stage 2+
  - Plugin must NOT be ✅ Working or 📦 Installed (use /improve instead)
---

# plugin-workflow Skill

**Purpose:** Pure orchestrator for stages 2-6 of JUCE plugin implementation. This skill NEVER implements directly - it always delegates to specialized subagents and presents decision menus after each stage completes.

## Overview

This skill orchestrates plugin implementation stages 2-6. Stages 0-1 (Research & Planning) are handled by the `plugin-planning` skill.

**Implementation Stages:**
- **Stage 2:** Foundation - Create build system, verify compilation (foundation-agent)
- **Stage 3:** Shell - Implement APVTS with all parameters (shell-agent)
- **Stage 4:** DSP - Implement audio processing (dsp-agent)
- **Stage 5:** GUI - Integrate WebView UI with parameter bindings (gui-agent)
- **Stage 6:** Validation - Factory presets, pluginval, CHANGELOG (direct or validator)

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

    Stage 6 can optionally run directly in orchestrator or via validator subagent.
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

      Note: Phase count determined by plan.md (varies by complexity)
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

Each stage is fully documented in its own reference file in `references/` subdirectory.

<preconditions blocking="true">
  <contract_verification required="true" blocking="true">
    Before starting Stage 2, verify these contract files exist:

    <required_file path="plugins/$PLUGIN_NAME/.ideas/architecture.md" created_by="Stage 0" />
    <required_file path="plugins/$PLUGIN_NAME/.ideas/plan.md" created_by="Stage 1" />
    <required_file path="plugins/$PLUGIN_NAME/.ideas/creative-brief.md" created_by="ideation" />

    <on_missing_files action="BLOCK">
      Display error message:
      "[PluginName] is missing required planning documents.

      Missing files will be listed here:
      - architecture.md (from Stage 0)
      - plan.md (from Stage 1)
      - creative-brief.md (from ideation)

      Run /plan [PluginName] to complete planning stages 0-1."
    </on_missing_files>

    See [references/precondition-checks.md](references/precondition-checks.md) for bash implementation.
  </contract_verification>

  <status_verification required="true" blocking="true">
    Read PLUGINS.md and verify status is appropriate:

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
  </status_verification>
</preconditions>

---

## Resume Entry Point

**Purpose:** Handle workflow resume from .continue-here.md handoff file.

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

---

## Stage Dispatcher

**Purpose:** Pure orchestration dispatcher that ONLY invokes subagents via Task tool.

**Entry point:** Called by /implement command or /continue command after plugin-planning completes.

<enforcement_reminder ref="subagent-dispatch-only" severity="CRITICAL">
This orchestrator MUST NEVER implement plugin code directly. ALL stages 2-5 MUST be delegated via Task tool.
</enforcement_reminder>

### Implementation

1. **Determine current stage:**

```bash
# Check if handoff file exists (resuming)
if [ -f "plugins/${PLUGIN_NAME}/.continue-here.md" ]; then
    # Parse stage from handoff YAML frontmatter
    CURRENT_STAGE=$(grep "^stage:" plugins/${PLUGIN_NAME}/.continue-here.md | awk '{print $2}')
    echo "Resuming from Stage ${CURRENT_STAGE}"
else
    # Starting fresh after planning
    CURRENT_STAGE=2
    echo "Starting implementation at Stage 2"
fi
```

2. **Verify preconditions for target stage:**

See `references/state-management.md` for `checkStagePreconditions()` function.

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

4. **Checkpoint enforcement after EVERY subagent:**

<workflow_loop>
  <stage_iteration from="2" to="6">
    <dispatch_phase>
      Display: "━━━ Stage ${currentStage} ━━━"

      <delegation_enforcement ref="subagent-dispatch-only">
        Invoke subagent via Task tool (NEVER implement directly)
      </delegation_enforcement>

      IF result.status == 'blocked' OR 'error':
        Display error and return (workflow blocked)
    </dispatch_phase>

    <checkpoint_phase>
      <checkpoint_enforcement enforce_order="true">
        <step order="1" required="true" function="commitStage">
          commitStage(pluginName, currentStage, result.description)
          Auto-commit all changes from subagent completion
        </step>

        <step order="2" required="true" function="updateHandoff">
          updateHandoff(pluginName, currentStage + 1, result.completed, result.nextSteps)
          Update .continue-here.md with new stage, timestamp, next_action
        </step>

        <step order="3" required="true" function="updatePluginStatus">
          updatePluginStatus(pluginName, `🚧 Stage ${currentStage}`)
          Update PLUGINS.md status emoji
        </step>

        <step order="4" required="true" function="updatePluginTimeline">
          updatePluginTimeline(pluginName, currentStage, result.description)
          Append timeline entry to PLUGINS.md
        </step>

        <step order="5" required="true" blocking="true" function="presentDecisionMenu">
          presentDecisionMenu({ stage, completionStatement, pluginName })
          Present numbered decision menu and WAIT for user response
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

**Usage:**

```javascript
// From /implement command (after planning complete):
runWorkflow(pluginName, 2)

// From /continue command:
const handoff = readHandoffFile(pluginName)
const resumeStage = handoff.stage
runWorkflow(pluginName, resumeStage)
```

---

<reference_files>
  Each stage has a reference file containing subagent prompt templates:

  - [stage-2-foundation.md](references/stage-2-foundation.md) - foundation-agent
  - [stage-3-shell.md](references/stage-3-shell.md) - shell-agent
  - [stage-4-dsp.md](references/stage-4-dsp.md) - dsp-agent
  - [stage-5-gui.md](references/stage-5-gui.md) - gui-agent
  - [stage-6-validation.md](references/stage-6-validation.md) - validator
  - [state-management.md](references/state-management.md) - State machine functions
  - [dispatcher-pattern.md](references/dispatcher-pattern.md) - Routing logic
  - [precondition-checks.md](references/precondition-checks.md) - Contract validation

  <usage_pattern>
    Orchestrator reads reference files → constructs Task invocation → NEVER implements directly
  </usage_pattern>
</reference_files>

---

## Integration Points

**Invoked by:**

- `/implement` command (after plugin-planning completes)
- `context-resume` skill (when resuming implementation stages)
- `/continue` command (for stages 2-6)

**ALWAYS invokes (via Task tool):**

- `foundation-agent` subagent (Stage 2) - REQUIRED, never implement directly
- `shell-agent` subagent (Stage 3) - REQUIRED, never implement directly
- `dsp-agent` subagent (Stage 4) - REQUIRED, never implement directly
- `gui-agent` subagent (Stage 5) - REQUIRED, never implement directly
- `validator` subagent (Stage 6) - Optional, can run directly

**Also invokes:**

- `build-automation` skill (build coordination across stages)
- `plugin-testing` skill (validation after stages 4, 5, 6)
- `plugin-lifecycle` skill (if user chooses to install after Stage 6)

**Reads (contracts from plugin-planning):**

- `architecture.md` (DSP specification from Stage 0)
- `plan.md` (implementation strategy from Stage 1)
- `creative-brief.md` (vision from ideation)
- `parameter-spec.md` (parameter definitions)

**Creates:**

- `.continue-here.md` (handoff file for checkpoints)
- `CHANGELOG.md` (Stage 6)
- `Presets/` directory (Stage 6)

**Updates:**

- PLUGINS.md (status changes after each stage)
- `.continue-here.md` (after each stage completes)

---

## Error Handling

**If contract files missing before Stage 2:**
Block and instruct user to run `/plan [PluginName]` to complete stages 0-1.

**If build fails during subagent execution:**
Subagent returns error. Orchestrator presents 4-option menu:
1. Investigate (deep-research)
2. Show me the code
3. Show full build output
4. Manual fix (resume with /continue)

**If tests fail:**
Present menu with investigation options. Do NOT auto-proceed to next stage.

**If subagent fails to complete:**
Present menu allowing retry, manual intervention, or workflow pause.

**If git staging fails:**
Continue anyway, log warning.

---

## Success Criteria

Workflow is successful when:

- All subagents (stages 2-5) invoked successfully via Task tool
- Plugin compiles without errors at each stage
- All stages completed in sequence (2 → 3 → 4 → 5 → 6)
- Decision menus presented after EVERY stage
- Tests pass (if run)
- PLUGINS.md updated to ✅ Working after Stage 6
- Handoff file updated after each stage
- Git history shows atomic commits for each stage

---

<execution_guidance>
  <critical_reminders>
    <reminder priority="CRITICAL" ref="subagent-dispatch-only">
      NEVER implement stages 2-5 directly - MUST use Task tool to invoke subagents
    </reminder>

    <reminder priority="CRITICAL" ref="stage-completion-checkpoint">
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

    <anti_pattern severity="CRITICAL" ref="subagent-dispatch-only">
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
