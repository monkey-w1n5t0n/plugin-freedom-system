# Fix Plan: plugin-workflow

## Summary
- Critical fixes: **8**
- Extraction operations: **3**
- Total estimated changes: **11**
- Estimated time: **100 minutes**

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Wrap orchestration rules in XML enforcement structure (Lines 32-51)
**Location:** Lines 32-51 (SKILL.md)
**Operation:** WRAP
**Before:**
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
**After:**
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
```
**Verification:**
- Check that XML tags `<orchestration_rules>`, `<delegation_rule>`, `<checkpoint_protocol>`, `<handoff_protocol>`, `<state_requirement>` are present
- Verify `violation="IMMEDIATE_STOP"` attribute on delegation_rule
- Verify `blocking="true"` attribute on checkpoint step 6
- Confirm nested `<enforcement>` and `<critical_sequence>` tags are properly structured

### Fix 1.2: Wrap checkpoint enforcement in state-management.md (Lines 224-273)
**Location:** Lines 224-273 (SKILL.md)
**Operation:** WRAP
**Before:**
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
**After:**
```xml
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
```
**Verification:**
- Check that `<checkpoint_enforcement enforce_order="true">` wrapper exists
- Verify 5 steps with `order` and `required` attributes
- Verify step 5 has `blocking="true"`
- Confirm `<decision_gate blocking="true">` wrapper exists

### Fix 1.3: Wrap stage boundary protocol in state-management.md (Lines 513-598)
**Location:** references/state-management.md lines 513-598
**Operation:** WRAP
**Change:**
Wrap the stage boundary protocol section in structured XML:
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
**Verification:** Confirm XML wrapper exists with ordered actions and actor responsibilities

### Fix 1.4: Replace repeated delegation rule with reference structure (Lines 144-151, 386, 404)
**Location:** Lines 144-151, 386, 404 (SKILL.md)
**Operation:** REPLACE (multiple instances)
**Before (Line 144-151):**
```markdown
### CRITICAL RULE: Never Implement Directly

**This skill is a PURE ORCHESTRATOR:**
- Stages 2-5 MUST be delegated to subagents
- NEVER write plugin code directly in this skill
- ALWAYS use Task tool to invoke subagents
- Only Stage 6 validation can optionally run directly
```
**After (Line 144-151):**
```xml
<enforcement_reminder ref="subagent-dispatch-only" severity="CRITICAL"/>
```

**Before (Lines 386-387):**
```markdown
1. **NEVER implement stages 2-5 directly** - You MUST use Task tool to invoke subagents
```
**After (Lines 386-387):**
```xml
<critical_reminders>
  <reminder ref="subagent-dispatch-only" priority="CRITICAL">
    NEVER implement stages 2-5 directly - MUST use Task tool to invoke subagents
  </reminder>
</critical_reminders>
```

**Before (Lines 404-405):**
```markdown
- ❌ Implementing stage logic directly in orchestrator (ALWAYS use Task tool)
```
**After (Lines 404-405):**
```xml
<anti_pattern ref="subagent-dispatch-only" severity="CRITICAL">
  ❌ Implementing stage logic directly in orchestrator
  ✓ ALWAYS use Task tool to invoke appropriate subagent
</anti_pattern>
```
**Verification:** Search for all instances of delegation rule text - should only find XML references, not duplicated prose

### Fix 1.5: Wrap precondition checking in decision gates (Lines 54-99)
**Location:** Lines 54-99 (SKILL.md)
**Operation:** WRAP
**Before:**
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
**After:**
```xml
<preconditions blocking="true">
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
```
**Verification:** Confirm `<preconditions>`, `<contract_verification>`, `<status_verification>`, `<allowed_state>`, `<blocked_state>` tags exist

### Fix 1.6: Wrap resume/fresh-start logic in decision gate (Lines 108-135)
**Location:** Lines 108-135 (SKILL.md)
**Operation:** WRAP
**Before:**
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
**Verification:** Confirm `<decision_gate>` wrapper with `<condition>` and `<routing_logic>` tags

### Fix 1.7: Restructure "Notes for Claude" section with XML guidance (Lines 384-411)
**Location:** Lines 384-411 (SKILL.md)
**Operation:** REPLACE
**Before:**
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
```
**Verification:** Confirm `<execution_guidance>`, `<critical_reminders>`, `<execution_checklist>`, `<anti_patterns>` structure

### Fix 1.8: Wrap decision menu format in state-management.md (Lines 137-173)
**Location:** references/state-management.md lines 137-173
**Operation:** WRAP
**Before:**
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
**After:**
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
**Verification:** Confirm `forbidden_tool="AskUserQuestion"` attribute exists and rendering sequence is structured

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Extract dispatcher pseudocode to references/dispatcher-pattern.md
**Location:** Lines 176-223 (SKILL.md)
**Operation:** EXTRACT
**Create file:** `references/dispatcher-pattern.md`
**File content:**
```markdown
# Dispatcher Pattern

## Overview
The orchestrator uses the dispatcher pattern to route implementation stages to specialized subagents. This ensures separation of concerns - the orchestrator handles workflow state and checkpoints, while subagents handle implementation.

## Dispatcher Logic (Pseudocode)

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

## Key Principles

1. **Always delegate stages 2-5** - Use Task tool to invoke subagents
2. **Pass contracts and Required Reading** - Every subagent receives architecture.md, plan.md, and juce8-critical-patterns.md
3. **Check preconditions first** - Validate contracts exist before dispatching
4. **Stage 6 flexibility** - Can run directly in orchestrator or via validator subagent

## Integration

The dispatcher is called from the main workflow loop:
```javascript
const result = await dispatchStage(pluginName, currentStage)
```

After dispatch completes, the orchestrator runs the checkpoint protocol (see state-management.md).
```
**Replace with:**
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
**Verification:** File exists at references/dispatcher-pattern.md and SKILL.md reduced by ~48 lines

### Fix 2.2: Extract precondition checks to references/precondition-checks.md
**Location:** Lines 60-77 (SKILL.md)
**Operation:** EXTRACT
**Create file:** `references/precondition-checks.md`
**File content:**
```markdown
# Precondition Checks

## Contract Verification

Before starting Stage 2, verify all required planning documents exist:

```bash
# Check for architecture document (Stage 0 output)
test -f "plugins/$PLUGIN_NAME/.ideas/architecture.md"

# Check for implementation plan (Stage 1 output)
test -f "plugins/$PLUGIN_NAME/.ideas/plan.md"

# Check for creative brief (ideation output)
test -f "plugins/$PLUGIN_NAME/.ideas/creative-brief.md"
```

## Status Verification

Read PLUGINS.md to verify current plugin status:

```bash
# Extract plugin section
grep "^### $PLUGIN_NAME$" PLUGINS.md
```

Parse the Status line to determine current state:
- 🚧 Stage 1 → Planning just completed, OK to proceed
- 🚧 Stage N (N ≥ 2) → Implementation in progress, OK to resume
- 💡 Ideated → Planning not started, BLOCK
- ✅ Working → Plugin complete, BLOCK (suggest /improve)
- 📦 Installed → Plugin complete and installed, BLOCK (suggest /improve)

## Block Messages

### Missing Contracts
```
[PluginName] is missing required planning documents.

Missing files will be listed here:
- architecture.md (from Stage 0)
- plan.md (from Stage 1)
- creative-brief.md (from ideation)

Run /plan [PluginName] to complete planning stages 0-1.
```

### Wrong Status
```
[PluginName] needs planning before implementation.
Run /plan [PluginName] to complete stages 0-1.
```

OR

```
[PluginName] is already complete.
Use /improve [PluginName] to make changes.
```
```
**Replace with:** Reference to extracted file (already included in Fix 1.5)
**Verification:** File exists at references/precondition-checks.md

### Fix 2.3: Extract decision menu examples to assets/decision-menus.json
**Location:** references/state-management.md lines 180-257
**Operation:** EXTRACT
**Create file:** `assets/decision-menus.json`
**File content:**
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
  "stage_3_complete": {
    "completion_statement": "APVTS implemented with all parameters",
    "options": [
      { "label": "Continue to Stage 4", "recommended": true, "action": "next_stage" },
      { "label": "Review parameter setup", "action": "review_code" },
      { "label": "Test parameter values", "action": "manual_test" },
      { "label": "Pause here", "action": "pause" }
    ]
  },
  "stage_4_complete": {
    "completion_statement": "DSP implementation complete",
    "options": [
      { "label": "Continue to Stage 5", "recommended": true, "action": "next_stage" },
      { "label": "Review DSP code", "action": "review_code" },
      { "label": "Test audio processing", "action": "manual_test" },
      { "label": "Run validation suite", "action": "run_tests", "discovery": "Automated pluginval testing" },
      { "label": "Pause here", "action": "pause" }
    ]
  },
  "stage_5_complete": {
    "completion_statement": "GUI integrated with parameter bindings",
    "options": [
      { "label": "Continue to Stage 6", "recommended": true, "action": "next_stage" },
      { "label": "Review GUI code", "action": "review_code" },
      { "label": "Test UI manually", "action": "manual_test" },
      { "label": "Save as template", "action": "save_template", "discovery": "Add to UI template library" },
      { "label": "Pause here", "action": "pause" }
    ]
  },
  "stage_6_complete": {
    "completion_statement": "Validation complete - plugin ready for use",
    "options": [
      { "label": "Install plugin", "recommended": true, "action": "install" },
      { "label": "Review CHANGELOG", "action": "review_changelog" },
      { "label": "Test in DAW", "action": "manual_test" },
      { "label": "Create distribution package", "action": "package" },
      { "label": "Done", "action": "complete" }
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
  },
  "validation_failure": {
    "completion_statement": "pluginval failed with {count} errors",
    "options": [
      { "label": "Investigate", "action": "deep_research", "discovery": "Deep investigation for validation issues" },
      { "label": "Show failures", "action": "show_failures" },
      { "label": "Retry validation", "action": "retry" },
      { "label": "I'll fix it manually", "action": "manual_fix" }
    ]
  },
  "phase_complete": {
    "completion_statement": "Phase {phase} of stage {stage} complete",
    "options": [
      { "label": "Continue to next phase", "recommended": true, "action": "next_phase" },
      { "label": "Review phase code", "action": "review_code" },
      { "label": "Test this phase", "action": "manual_test" },
      { "label": "Pause here", "action": "pause" }
    ]
  }
}
```
**Replace with (in state-management.md):**
```markdown
### generateContextualOptions(context)

**Purpose:** Generate situation-specific menu options after subagent completion.

Load menu configurations from `assets/decision-menus.json` and customize based on context (stage number, success/failure, phase information).

**Progressive disclosure:** Menu options include discovery markers (←) to surface features like deep-research, UI template library, design sync validation.
```
**Verification:** File exists at assets/decision-menus.json and state-management.md reduced by ~78 lines

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Condense stage reference files section (Lines 287-300)
**Location:** Lines 287-300 (SKILL.md)
**Operation:** REPLACE
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
  - [dispatcher-pattern.md](references/dispatcher-pattern.md) - Routing logic
  - [precondition-checks.md](references/precondition-checks.md) - Contract validation

  <usage_pattern>
    Orchestrator reads reference files → constructs Task invocation → NEVER implements directly
  </usage_pattern>
</reference_files>
```
**Verification:** Section condensed from 14 lines to 11 lines, maintains all essential information

## File Creation Manifest

Files to create:
1. `references/dispatcher-pattern.md` - Dispatcher pseudocode and routing logic
2. `references/precondition-checks.md` - Contract and status verification bash scripts
3. `assets/decision-menus.json` - Context-specific menu options for all stages

## Execution Checklist

- [ ] Phase 1.1: Orchestration rules wrapped in XML with enforcement attributes
- [ ] Phase 1.2: Checkpoint protocol wrapped in XML with ordered steps
- [ ] Phase 1.3: Stage boundary protocol wrapped in XML (state-management.md)
- [ ] Phase 1.4: Delegation rule deduplicated - single definition, multiple references
- [ ] Phase 1.5: Precondition checking wrapped in decision gates
- [ ] Phase 1.6: Resume/fresh-start logic wrapped in decision gate
- [ ] Phase 1.7: "Notes for Claude" restructured with XML guidance
- [ ] Phase 1.8: Decision menu format wrapped in XML (state-management.md)
- [ ] Phase 2.1: Dispatcher pseudocode extracted to references/dispatcher-pattern.md
- [ ] Phase 2.2: Precondition checks extracted to references/precondition-checks.md
- [ ] Phase 2.3: Decision menus extracted to assets/decision-menus.json
- [ ] Phase 3.1: Stage reference files section condensed with XML structure
- [ ] Verification: All XML tags properly closed and nested
- [ ] Verification: All file references valid (dispatcher-pattern.md, precondition-checks.md, decision-menus.json)
- [ ] Verification: No delegation rule duplication - only references remain
- [ ] Verification: Context reduced by estimated ~1920 tokens (12%)
- [ ] Verification: Skill health improved from Yellow to Green (estimated)
