---
name: plugin-workflow
description: Complete 7-stage JUCE plugin development workflow
allowed-tools:
  - Task # For subagents (Phases 2-5)
  - Bash # For git commits
  - Read # For contracts
  - Write # For documentation
  - Edit # For state updates
preconditions:
  - Plugin must not exist OR status must be 💡 (ideated)
  - Plugin must NOT be 🚧 (use /continue instead)
---

# plugin-workflow Skill

**Purpose:** Orchestrate complete 7-stage JUCE plugin development from research to validated, production-ready plugin.

## Precondition Checking

**Before starting, verify:**

1. Read PLUGINS.md:

```bash
grep "^### $PLUGIN_NAME$" PLUGINS.md
```

2. Check status:

   - If not found → OK to proceed (new plugin)
   - If status = 💡 Ideated → OK to proceed
   - If status = 🚧 Stage N → BLOCK with message:
     ```
     [PluginName] is already in development (Stage [N]).
     Use /continue [PluginName] to resume the workflow.
     ```
   - If status = ✅ Working or 📦 Installed → BLOCK with message:
     ```
     [PluginName] is already complete.
     Use /improve [PluginName] to make changes.
     ```

3. Check for creative brief:

```bash
test -f "plugins/$PLUGIN_NAME/.ideas/creative-brief.md"
```

If missing, offer:

```
No creative brief found for [PluginName].

Would you like to:
1. Create one now (/dream [PluginName]) (recommended)
2. Continue without brief (not recommended)

Choose (1-2): _
```

If user chooses 1, exit and instruct them to run `/dream [PluginName]`.

---

## Stage Dispatcher

**Purpose:** Route to correct stage implementation based on current state.

**Entry point:** Called by /implement command or when resuming workflow.

**Implementation:**

1. Determine current stage:

```bash
# Check if handoff file exists (resuming)
if [ -f "plugins/${PLUGIN_NAME}/.continue-here.md" ]; then
    # Parse stage from handoff YAML frontmatter
    CURRENT_STAGE=$(grep "^stage:" plugins/${PLUGIN_NAME}/.continue-here.md | awk '{print $2}')
    echo "Resuming from Stage ${CURRENT_STAGE}"
else
    # Starting new workflow
    CURRENT_STAGE=0
    echo "Starting workflow at Stage 0"
fi
```

2. Verify preconditions for target stage:

```javascript
function checkStagePreconditions(pluginName, stage) {
  // Stage 0: Just needs creative brief (already checked)
  if (stage === 0) {
    return { allowed: true }
  }

  // Stage 1: Requires research.md + contracts
  if (stage === 1) {
    const researchExists = fileExists(`plugins/${pluginName}/.ideas/research.md`)
    const paramSpecExists = fileExists(`plugins/${pluginName}/.ideas/parameter-spec.md`)
    const archSpecExists = fileExists(`plugins/${pluginName}/.ideas/architecture.md`)

    if (!researchExists) {
      return {
        allowed: false,
        reason: "Stage 0 research must complete before Stage 1",
        action: "Complete Stage 0 first"
      }
    }

    if (!paramSpecExists || !archSpecExists) {
      return {
        allowed: false,
        reason: "Cannot proceed to Stage 1 - missing implementation contracts",
        action: "Complete ui-mockup workflow and create architecture.md",
        contracts: {
          "parameter-spec.md": paramSpecExists,
          "architecture.md": archSpecExists
        }
      }
    }

    return { allowed: true }
  }

  // Stages 2-6: Require previous stage complete
  if (stage >= 2 && stage <= 6) {
    const status = getPluginStatus(pluginName)
    const expectedPrevious = stage - 1

    // Check PLUGINS.md shows previous stage complete
    if (!status.includes(`Stage ${expectedPrevious}`) &&
        !status.includes('complete')) {
      return {
        allowed: false,
        reason: `Stage ${expectedPrevious} must complete before Stage ${stage}`,
        action: `Complete Stage ${expectedPrevious} or use /continue to resume`
      }
    }

    return { allowed: true }
  }

  return { allowed: false, reason: "Invalid stage number" }
}
```

3. Route to stage implementation:

```javascript
function dispatchStage(pluginName, stageNumber) {
  // Check preconditions
  const preconditionCheck = checkStagePreconditions(pluginName, stageNumber)

  if (!preconditionCheck.allowed) {
    // Display blocking message
    console.log(`✗ BLOCKED: ${preconditionCheck.reason}`)
    console.log(`Action: ${preconditionCheck.action}`)

    if (preconditionCheck.contracts) {
      console.log("\nContract status:")
      for (const [name, exists] of Object.entries(preconditionCheck.contracts)) {
        console.log(`${exists ? '✓' : '✗'} ${name}`)
      }
    }

    return { status: 'blocked', reason: preconditionCheck.reason }
  }

  // Route to stage
  switch(stageNumber) {
    case 0:
      return executeStage0Research(pluginName)
    case 1:
      return executeStage1Planning(pluginName)
    case 2:
      return executeStage2FoundationStub(pluginName) // Phase 2b: stub
    case 3:
      return executeStage3ShellStub(pluginName) // Phase 2b: stub
    case 4:
      return executeStage4DSPStub(pluginName) // Phase 2b: stub
    case 5:
      return executeStage5GUIStub(pluginName) // Phase 2b: stub
    case 6:
      return executeStage6Validation(pluginName) // Phase 2c
    default:
      return { status: 'error', reason: `Invalid stage: ${stageNumber}` }
  }
}
```

4. Stage loop for continuous execution:

```javascript
function runWorkflow(pluginName, startStage = 0) {
  let currentStage = startStage
  let shouldContinue = true

  while (shouldContinue && currentStage <= 6) {
    console.log(`\n━━━ Stage ${currentStage} ━━━\n`)

    // Execute stage
    const result = dispatchStage(pluginName, currentStage)

    if (result.status === 'blocked') {
      console.log("\nWorkflow blocked. Resolve issues and resume with /continue.")
      return result
    }

    if (result.status === 'error') {
      console.log(`\nError: ${result.reason}`)
      return result
    }

    // Present decision menu
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
      // Re-present menu after handling
    }
  }

  if (currentStage > 6) {
    console.log("\n✓ All stages complete!")
  }
}
```

**Usage:**

```javascript
// From /implement command:
runWorkflow(pluginName, 0)

// From /continue command (via context-resume):
const handoff = readHandoffFile(pluginName)
const resumeStage = handoff.stage
runWorkflow(pluginName, resumeStage)
```

---

## Stage 0: Research

**Goal:** Understand what we're building before writing code

**Duration:** 5-10 minutes

**Model Configuration:**
- Model: Opus (complex reasoning for algorithm comparison)
- Extended thinking: ENABLED
- Budget: 10000 tokens

**Actions:**

1. Read creative brief:

```bash
cat plugins/[PluginName]/.ideas/creative-brief.md
```

2. Identify plugin type and technical approach:

   - Audio effect, MIDI effect, synthesizer, or utility?
   - Input/output configuration (mono, stereo, sidechain)
   - Processing type (time-domain, frequency-domain, granular)

3. **Context7 JUCE Documentation Lookup** (CRITICAL):

   a. Resolve JUCE library ID:
   ```
   Use: mcp__context7__resolve-library-id("JUCE")
   Returns: Context7-compatible library ID (e.g., "/juce-framework/JUCE")
   ```

   b. Get JUCE DSP documentation:
   ```
   Use: mcp__context7__get-library-docs(libraryID, topic="dsp modules", tokens=5000)
   Extract: Relevant juce::dsp classes for identified plugin type
   ```

   c. Document JUCE modules found:
   - List specific juce::dsp classes (e.g., juce::dsp::Gain, juce::dsp::IIR::Filter)
   - Note Context7 library references
   - Identify any missing algorithms (need custom DSP)

4. Research professional plugin examples:

   - Search web for industry leaders (FabFilter, Waves, UAD, etc.)
   - Document 3-5 similar plugins
   - Note sonic characteristics
   - Extract typical parameter ranges used

5. Research parameter ranges:

   - Industry-standard ranges for plugin type
   - Typical defaults (reference professional plugins)
   - Skew factors for nonlinear ranges

6. Check design sync (if mockup exists):
   - Look for `plugins/[Name]/.ideas/mockups/v*-ui.yaml`
   - If exists: Compare mockup parameters with creative brief
   - If conflicts: Invoke design-sync skill
   - Document sync results

**Output:** Create `plugins/[PluginName]/.ideas/research.md`

**Format:**

```markdown
# [PluginName] - Research

**Date:** [YYYY-MM-DD]
**Plugin Type:** [Effect/Synth/Utility]

## Similar Plugins

- [Plugin 1]: [Key features, parameter ranges]
- [Plugin 2]: [Key features, parameter ranges]
- [Plugin 3]: [Key features, parameter ranges]

## Technical Approach

**Processing Type:** [Time-domain/Frequency-domain/etc.]
**JUCE Modules Identified** (from Context7):
- `juce::dsp::[Module1]` - [Purpose]
- `juce::dsp::[Module2]` - [Purpose]
- (List all identified modules with Context7 library reference)

**Custom DSP Required:** [Any algorithms not available in JUCE]

## Parameter Research

| Parameter | Typical Range | Default | Skew | Notes (Industry Standard) |
| --------- | ------------- | ------- | ---- | ------------------------- |
| [Name]    | [Min-Max]     | [Value] | [X]  | [Professional plugin ranges] |

## Technical Feasibility

**JUCE Support:** [All modules available? / Custom implementation needed?]
**Complexity Factors:** [Feedback loops? FFT? Multiband? Modulation?]
**Implementation Challenges:** [Known gotchas from research]

## Design Sync Results

[If mockup exists]
- Brief ↔ Mockup consistency: [Pass/Conflicts found]
- Conflicts: [List if any]
- Resolution: [How resolved]

[If no mockup]
- No mockup found (OK - will create during workflow)

## References

- Context7 JUCE Library: [Library ID used]
- Professional Plugins: [URLs to researched plugins]
- JUCE Documentation: [Specific doc pages]
- Papers/Resources: [DSP papers if applicable]
```

**Create handoff file:** `plugins/[PluginName]/.continue-here.md`

**Format:**

```yaml
---
plugin: [PluginName]
stage: 0
status: in_progress
last_updated: [YYYY-MM-DD HH:MM:SS]
---

# Resume Point

## Current State: Stage 0 - Research

Research phase complete. Ready to proceed to planning.

## Completed So Far

**Stage 0:** ✓ Complete
- Plugin type defined
- Professional examples researched
- DSP feasibility verified
- Parameter ranges researched

## Next Steps

1. Stage 1: Planning (calculate complexity, create implementation plan)
2. Review research findings
3. Pause here

## Context to Preserve

**Plugin Type:** [Effect/Synth/Utility]
**Processing:** [Approach]
**JUCE Modules:** [List]

**Files Created:**
- plugins/[PluginName]/.ideas/research.md
```

**Update PLUGINS.md:**

1. Check if entry exists:
```bash
grep "^### ${PLUGIN_NAME}$" PLUGINS.md
```

2. If NOT found, create initial entry:

Use Edit tool to add to PLUGINS.md:
```markdown
### [PluginName]
**Status:** 💡 Ideated
**Type:** [Audio Effect | MIDI Instrument | Synth]
**Created:** [YYYY-MM-DD]

[Brief description from creative-brief.md]

**Lifecycle Timeline:**
- **[YYYY-MM-DD]:** Creative brief created

**Last Updated:** [YYYY-MM-DD]
```

3. Update status to Stage 0:

Use Edit tool to change:
```markdown
**Status:** 💡 Ideated → **Status:** 🚧 Stage 0
```

4. Add timeline entry:

Use Edit tool to append to Lifecycle Timeline:
```markdown
- **[YYYY-MM-DD] (Stage 0):** Research completed
```

5. Update Last Updated field

**Git commit:**

```bash
git add plugins/[PluginName]/.ideas/research.md plugins/[PluginName]/.continue-here.md PLUGINS.md
git commit -m "$(cat <<'EOF'
feat: [PluginName] Stage 0 - research complete

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
```

Display commit hash:
```bash
git log -1 --format='✓ Committed: %h - Stage 0 complete'
```

**Decision menu:**

Present inline numbered list (NOT AskUserQuestion):

```
✓ Stage 0 complete: research documented

What's next?
1. Continue to Stage 1 (recommended)
2. Review research findings
3. Improve creative brief based on research
4. Run deeper investigation (deep-research skill) ← Discover JUCE troubleshooting
5. Pause here
6. Other

Choose (1-6): _
```

Wait for user response. Handle:
- Number (1-6): Execute corresponding option
- "continue" keyword: Execute option 1
- "pause" keyword: Execute option 5
- "review" keyword: Execute option 2
- "other": Ask "What would you like to do?" then re-present menu

---

## Stage 1: Planning

**Goal:** Analyze complexity and create implementation plan

**Duration:** 2-5 minutes

**Model Configuration:**
- Model: Sonnet (deterministic planning)
- Extended thinking: DISABLED

**Preconditions:**

1. Stage 0 must be complete (research.md exists)
2. PLUGINS.md shows `🚧 Stage 0` or similar

**Contract Prerequisites (CRITICAL - BLOCKING):**

Check for required contract files:

```bash
test -f "plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md" && echo "✓ parameter-spec.md" || echo "✗ parameter-spec.md MISSING"
test -f "plugins/${PLUGIN_NAME}/.ideas/architecture.md" && echo "✓ architecture.md" || echo "✗ architecture.md MISSING"
test -f "plugins/${PLUGIN_NAME}/.ideas/research.md" && echo "✓ research.md" || echo "✗ research.md MISSING"
```

**If parameter-spec.md OR architecture.md is missing:**

STOP IMMEDIATELY and BLOCK with this exact message:

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ BLOCKED: Cannot proceed to Stage 1
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Missing implementation contracts:

Required contracts:
✓ creative-brief.md - exists
[✓/✗] parameter-spec.md - [exists/MISSING (required)]
[✓/✗] architecture.md - [exists/MISSING (required)]
✓ research.md - exists

WHY BLOCKED:
Stage 1 planning requires complete specifications to prevent implementation
drift. These contracts are the single source of truth.

HOW TO UNBLOCK:
1. parameter-spec.md: Complete ui-mockup two-phase workflow
   - Run: /dream [PluginName]
   - Choose: "Create UI mockup"
   - Design UI and finalize (Phase 4.5)
   - Finalization generates parameter-spec.md

2. architecture.md: Create DSP architecture specification
   - Use research.md findings (JUCE modules identified)
   - Document DSP components and processing chain
   - Map parameters to DSP components
   - Save to plugins/[PluginName]/.ideas/architecture.md

Once both contracts exist, Stage 1 will proceed.
```

DO NOT PROCEED PAST THIS POINT if contracts are missing.

Exit skill and wait for user to create contracts.

**Actions (contracts confirmed present):**

1. Read all contracts:

```bash
# Read parameter specification
cat plugins/[PluginName]/.ideas/parameter-spec.md

# Read DSP architecture
cat plugins/[PluginName]/.ideas/architecture.md

# Read research findings
cat plugins/[PluginName]/.ideas/research.md
```

2. Calculate complexity score:

**Formula:**
```
score = min(param_count / 5, 2.0) + algorithm_count + feature_count
Cap at 5.0
```

**Extract metrics:**

From parameter-spec.md:
- Count parameters (each parameter definition = 1)
- param_score = min(param_count / 5, 2.0)

From architecture.md:
- Count distinct DSP algorithms/components
- algorithm_count = number of juce::dsp classes or custom algorithms

From architecture.md (identify features):
- Feedback loops present? (+1)
- FFT/frequency domain processing? (+1)
- Multiband processing? (+1)
- Modulation systems (LFO, envelope)? (+1)
- External MIDI control? (+1)
- feature_count = sum of above

**Calculate:**
```
total_score = param_score + algorithm_count + feature_count
final_score = min(total_score, 5.0)
```

**Show breakdown:**
```
Complexity Calculation:
- Parameters: [N] parameters ([N/5] points, capped at 2.0) = [X.X]
- Algorithms: [N] DSP components = [N]
- Features: [List] = [N]
Total: [X.X] / 5.0
```

3. Determine implementation strategy:

   - **Simple (score ≤ 2.0):** Single-pass implementation
   - **Complex (score ≥ 3.0):** Phase-based implementation with staged commits

4. For complex plugins (score ≥ 3), create phases:

**Stage 4 (DSP) phases:**

- Phase 4.1: Core processing (essential audio path)
- Phase 4.2: Parameter modulation (APVTS integration)
- Phase 4.3: Advanced features (if applicable)

**Stage 5 (GUI) phases:**

- Phase 5.1: Layout and basic controls
- Phase 5.2: Advanced UI elements
- Phase 5.3: Polish and styling (if applicable)

Each phase needs:

- Description of what gets implemented
- Test criteria to verify completion
- Estimated duration

**Output:** Create `plugins/[PluginName]/.ideas/plan.md`

**Format for simple plugins:**

```markdown
# [PluginName] - Implementation Plan

**Date:** [YYYY-MM-DD]
**Complexity Score:** [X.X] (Simple)
**Strategy:** Single-pass implementation

## Stages

- Stage 0: Research ✓
- Stage 1: Planning ← Current
- Stage 2: Foundation
- Stage 3: Shell
- Stage 4: DSP
- Stage 5: GUI
- Stage 6: Validation

## Estimated Duration

Total: ~[X] minutes

- Stage 2: 5 min
- Stage 3: 5 min
- Stage 4: [X] min
- Stage 5: [X] min
- Stage 6: 15 min
```

**Format for complex plugins:**

```markdown
# [PluginName] - Implementation Plan

**Date:** [YYYY-MM-DD]
**Complexity Score:** [X.X] (Complex)
**Strategy:** Phase-based implementation

## Complexity Factors

- Parameters: [N] ([N/5] points)
- Algorithms: [N] ([N] points)
- Features: [List] ([N] points)

## Stage 4: DSP Phases

### Phase 4.1: Core Processing

**Goal:** [Description]
**Test Criteria:**

- [ ] [Criterion 1]
- [ ] [Criterion 2]
      **Duration:** [X] min

### Phase 4.2: Parameter Modulation

**Goal:** [Description]
**Test Criteria:**

- [ ] [Criterion 1]
- [ ] [Criterion 2]
      **Duration:** [X] min

### Phase 4.3: Advanced Features

**Goal:** [Description]
**Test Criteria:**

- [ ] [Criterion 1]
- [ ] [Criterion 2]
      **Duration:** [X] min

## Stage 5: GUI Phases

### Phase 5.1: Layout and Basic Controls

**Goal:** [Description]
**Test Criteria:**

- [ ] [Criterion 1]
- [ ] [Criterion 2]
      **Duration:** [X] min

### Phase 5.2: Advanced UI Elements

**Goal:** [Description]
**Test Criteria:**

- [ ] [Criterion 1]
- [ ] [Criterion 2]
      **Duration:** [X] min

## Estimated Duration

Total: ~[X] hours

- Stage 2: 5 min
- Stage 3: 5 min
- Stage 4: [X] min (phased)
- Stage 5: [X] min (phased)
- Stage 6: 20 min
```

**Update .continue-here.md:**

Use Edit tool to update YAML frontmatter:

```yaml
---
plugin: [PluginName]
stage: 1
status: complete
last_updated: [YYYY-MM-DD HH:MM:SS]
complexity_score: [X.X]
phased_implementation: [true/false]
---
```

Update markdown sections:
- Current State: "Stage 1 - Planning complete"
- Completed So Far: Add Stage 1 details
- Next Steps: List Stage 2 actions
- Context to Preserve: Add complexity score, strategy

**Update PLUGINS.md:**

1. Update status:

Use Edit tool to change:
```markdown
**Status:** 🚧 Stage 0 → **Status:** 🚧 Stage 1
```

2. Add timeline entry:

Use Edit tool to append to Lifecycle Timeline:
```markdown
- **[YYYY-MM-DD] (Stage 1):** Planning - Complexity [X.X]
```

3. Update Last Updated field

**Git commit:**

```bash
git add plugins/[PluginName]/.ideas/plan.md plugins/[PluginName]/.continue-here.md PLUGINS.md
git commit -m "$(cat <<'EOF'
feat: [PluginName] Stage 1 - planning complete

Complexity: [X.X]
Strategy: [Single-pass | Phased implementation]

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
```

Display commit hash:
```bash
git log -1 --format='✓ Committed: %h - Stage 1 complete'
```

**Decision menu:**

Present inline numbered list (NOT AskUserQuestion):

```
✓ Stage 1 complete: plan created (Complexity [X.X], [single-pass/phased])

What's next?
1. Continue to Stage 2 (recommended)
2. Review plan details
3. Adjust complexity assessment
4. Review contracts (parameter-spec, architecture)
5. Pause here
6. Other

Choose (1-6): _
```

Wait for user response. Handle:
- Number (1-6): Execute corresponding option
- "continue" keyword: Execute option 1
- "pause" keyword: Execute option 5
- "review" keyword: Execute option 2
- "other": Ask "What would you like to do?" then re-present menu

---

## Stage 2: Foundation

**Goal:** Create plugin structure that compiles

**Duration:** 5 minutes

**NOTE:** Stage 2 requires foundation-agent (Phase 3). Creating placeholder.

**Current Implementation (Manual):**

Since foundation-agent doesn't exist yet, provide manual instructions:

```
Stage 2 requires the foundation-agent subagent (Phase 3).

For now, I'll create a minimal foundation manually:

1. Create plugins/[PluginName]/CMakeLists.txt
2. Create plugins/[PluginName]/Source/PluginProcessor.h
3. Create plugins/[PluginName]/Source/PluginProcessor.cpp
4. Create plugins/[PluginName]/Source/PluginEditor.h
5. Create plugins/[PluginName]/Source/PluginEditor.cpp

These will be minimal stubs that compile but don't do anything yet.
```

Create minimal files manually following JUCE patterns.

**Git commit:**

```bash
git add plugins/[PluginName]/
# Stage but don't commit
```

**Update .continue-here.md:**

```yaml
stage: 2
```

**Update PLUGINS.md:**

```markdown
**Status:** 🚧 Stage 2
```

**Decision menu:**

```
✓ Stage 2 complete: foundation created (manual)

Note: Stage 2 will use foundation-agent in Phase 3 for automated setup.

What's next?
1. Continue to Stage 3 (recommended)
2. Review foundation code
3. Pause here
4. Other

Choose (1-4): _
```

---

## Stage 3: Shell

**Goal:** Plugin loads in DAW, does nothing yet

**Duration:** 5 minutes

**NOTE:** Stage 3 requires shell-agent (Phase 3). Creating placeholder.

**Current Implementation (Manual):**

```
Stage 3 requires the shell-agent subagent (Phase 3).

For now, I'll create a basic shell manually:

1. Implement basic AudioProcessor methods (prepareToPlay, processBlock, releaseResources)
2. Create empty APVTS (no parameters yet)
3. Implement minimal PluginEditor with placeholder text
4. Build and verify it loads in DAW
```

Create basic shell manually.

**Git commit:**

```bash
git add plugins/[PluginName]/Source/
# Stage but don't commit
```

**Update .continue-here.md:**

```yaml
stage: 3
```

**Update PLUGINS.md:**

```markdown
**Status:** 🚧 Stage 3
```

**Decision menu:**

```
✓ Stage 3 complete: shell loads in DAW (manual)

Note: Stage 3 will use shell-agent in Phase 3 for automated setup.

What's next?
1. Continue to Stage 4 (recommended)
2. Review shell code
3. Test loading in different DAW
4. Pause here
5. Other

Choose (1-5): _
```

---

## Stage 4: DSP

**Goal:** Audio processing works, parameters functional

**Duration:** 15-45 minutes (depending on complexity)

**NOTE:** Stage 4 requires dsp-agent (Phase 3). Creating placeholder.

**Current Implementation (Manual):**

```
Stage 4 requires the dsp-agent subagent (Phase 3).

For now, provide implementation guidance:

Simple plugins (score ≤ 2):
- Single-pass implementation
- Read parameter-spec.md for all parameters
- Read architecture.md for DSP design
- Implement processBlock with audio processing
- Create APVTS with all parameters
- Test with automated tests

Complex plugins (score ≥ 3):
- Phase-based implementation
- Each phase has specific goals from plan.md
- Commit after each phase
- Update plan.md with completion checkmarks
- Test after each phase
```

For now, implement DSP manually or provide detailed guidance.

**Testing:**

After Stage 4 completion, invoke plugin-testing skill (Phase 1b Task 8):

```
Running automated stability tests...
```

If tests fail, STOP and present menu:

```
❌ Tests failed

What would you like to do?
1. Investigate failures
2. Show me the test output
3. I'll fix it manually
4. Other

Choose (1-4): _
```

**Git commit:**

Simple:

```bash
git add plugins/[PluginName]/Source/
# Message: feat: [Plugin] Stage 4 - DSP
```

Complex (per phase):

```bash
git add plugins/[PluginName]/Source/
git add plugins/[PluginName]/.ideas/plan.md
# Message: feat: [Plugin] Stage 4.1 - [phase description]
```

**Update .continue-here.md:**

```yaml
stage: 4
phase: [N if complex]
```

**Update PLUGINS.md:**

```markdown
**Status:** 🚧 Stage 4
```

**Decision menu:**

```
✓ Stage 4 complete: DSP implementation finished

What's next?
1. Continue to Stage 5 (recommended)
2. Review DSP code
3. Test audio processing manually
4. Pause here
5. Other

Choose (1-5): _
```

---

## Stage 5: GUI

**Goal:** Professional UI with working controls

**Duration:** 20-60 minutes (depending on complexity)

**NOTE:** Stage 5 requires gui-agent (Phase 3). Creating placeholder.

**Current Implementation (Manual):**

```
Stage 5 requires the gui-agent subagent (Phase 3).

For now, provide implementation guidance:

Simple plugins (score ≤ 2):
- Single-pass implementation
- Read mockup files if they exist (v*-ui.yaml, v*-ui.html)
- Create FlexBox layout (never manual setBounds)
- Add rotary sliders, buttons, labels
- Attach controls to APVTS parameters
- Apply styling

Complex plugins (score ≥ 3):
- Phase-based implementation
- Each phase has specific goals from plan.md
- Commit after each phase
- Update plan.md with completion checkmarks
```

For now, implement GUI manually or provide detailed guidance.

**Testing:**

After Stage 5 completion, invoke plugin-testing skill:

```
Running automated stability tests...
```

**Git commit:**

Simple:

```bash
git add plugins/[PluginName]/Source/
# Message: feat: [Plugin] Stage 5 - GUI
```

Complex (per phase):

```bash
git add plugins/[PluginName]/Source/
git add plugins/[PluginName]/.ideas/plan.md
# Message: feat: [Plugin] Stage 5.1 - [phase description]
```

**Update .continue-here.md:**

```yaml
stage: 5
phase: [N if complex]
```

**Update PLUGINS.md:**

```markdown
**Status:** 🚧 Stage 5
```

**Decision menu:**

```
✓ Stage 5 complete: GUI implementation finished

What's next?
1. Continue to Stage 6 (recommended)
2. Review GUI code
3. Test UI in standalone app (/show-standalone)
4. Pause here
5. Other

Choose (1-5): _
```

---

## Stage 6: Validation

**Goal:** Ready to install and use

**Duration:** 10-20 minutes

**Preconditions:**

- Stages 0-5 complete
- Plugin compiles successfully
- Automated tests pass (if run)

**Actions:**

1. Create factory presets:

```bash
mkdir -p plugins/[PluginName]/Presets/
```

Create 3-5 preset files showcasing plugin capabilities.

**Preset format (.preset or .xml):**

```xml
<?xml version="1.0" encoding="UTF-8"?>
<preset name="[PresetName]">
  <param id="[paramID]" value="[value]"/>
  <param id="[paramID]" value="[value]"/>
  ...
</preset>
```

2. Invoke plugin-testing skill:

Present test method choice:

```
How would you like to test [PluginName]?

1. Automated stability tests (if Tests/ directory exists)
2. Build and run pluginval (recommended)
3. Manual DAW testing checklist
4. Skip testing (not recommended)

Choose (1-4): _
```

If tests fail, STOP and wait for fixes.

3. Generate CHANGELOG.md:

**Format:**

```markdown
# Changelog

All notable changes to [PluginName] will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [1.0.0] - [YYYY-MM-DD]

### Added

- Initial release
- [Feature 1]
- [Feature 2]
- [Parameter 1]: [Description]
- [Parameter 2]: [Description]

### Audio Processing

- [DSP description]

### User Interface

- [UI description]

### Validation

- Passed pluginval strict validation
- Tested in [DAW names]
```

4. Update PLUGINS.md:

Call `updatePluginStatus(pluginName, "✅ Working")`.

Add final fields to entry:
```markdown
**Version:** 1.0.0
**Completed:** [YYYY-MM-DD]

**Validation:**
- ✓ Factory presets: [N] presets created
- ✓ CHANGELOG.md: Generated in Keep a Changelog format
- ✓ Pluginval: [Passed | Skipped (no build)]

**Formats:** VST3, AU (if built)
```

Add timeline entry: `updatePluginTimeline(pluginName, 6, "Validation complete")`.

5. Delete .continue-here.md:

Call `deleteHandoff(pluginName)`:

```bash
rm plugins/[PluginName]/.continue-here.md
```

Workflow is complete, no need for handoff file.

**Git commit:**

Call `commitStage(pluginName, 6, "validation complete")`.

This commits:
- `plugins/[PluginName]/Presets/`
- `plugins/[PluginName]/CHANGELOG.md`
- `PLUGINS.md`

Note: `.continue-here.md` is deleted and NOT committed (workflow complete).

**Decision menu:**

```
✓ Stage 6 complete: [PluginName] is ready!

What's next?
1. Install plugin to system folders (recommended)
2. Test in DAW from build folder first
3. Create another plugin
4. Document this plugin
5. Share plugin (export build)
6. Other

Choose (1-6): _
```

**Handle responses:**

- Option 1 → Invoke `plugin-lifecycle` skill (Phase 1b Task 9)
- Option 2 → Provide instructions for manual DAW testing
- Option 3 → Exit, suggest `/dream` or `/implement`
- Option 4 → Suggest creating README or documentation
- Option 5 → Provide instructions for exporting builds
- Option 6 → Ask what they'd like to do

---

## State Machine Functions

### updatePluginStatus(pluginName, newStatus)

**Purpose:** Update plugin status emoji in PLUGINS.md.

**Valid statuses:**
- `💡 Ideated` - Creative brief exists, no Source/
- `🚧 Stage N` - In development (with stage number)
- `🚧 Stage N.M` - In development (with stage and phase)
- `✅ Working` - Stage 6 complete, not installed
- `📦 Installed` - Deployed to system folders

**Implementation:**
1. Read PLUGINS.md
2. Find section: `### [pluginName]`
3. Update `**Status:**` line with new emoji and text
4. Validate transition is legal (see validateTransition below)
5. Enforce single 🚧 constraint (only ONE plugin can be 🚧)
6. Write back to PLUGINS.md

**Example:**
```markdown
### TapeDelay
**Status:** 🚧 Stage 4 → **Status:** 🚧 Stage 5
```

### createPluginEntry(pluginName, type, brief)

**Purpose:** Create initial PLUGINS.md entry when starting new plugin.

**Implementation:**
1. Read PLUGINS.md
2. Check if entry already exists (search for `### [pluginName]`)
3. If not exists, append new entry:
   ```markdown
   ### [pluginName]
   **Status:** 💡 Ideated
   **Type:** [Audio Effect | MIDI Instrument | Synth]
   **Created:** [YYYY-MM-DD]

   [Brief description from creative-brief.md]

   **Lifecycle Timeline:**
   - **[YYYY-MM-DD]:** Creative brief created

   **Last Updated:** [YYYY-MM-DD]
   ```
4. Write back to PLUGINS.md

### updatePluginTimeline(pluginName, stage, description)

**Purpose:** Add timeline entry to PLUGINS.md when stage completes.

**Implementation:**
1. Read PLUGINS.md
2. Find plugin entry
3. Find `**Lifecycle Timeline:**` section
4. Append new entry:
   ```markdown
   - **[YYYY-MM-DD] (Stage N):** [description]
   ```
5. Update `**Last Updated:**` field
6. Write back to PLUGINS.md

### getPluginStatus(pluginName)

**Purpose:** Return current status emoji for routing logic.

**Implementation:**
1. Read PLUGINS.md
2. Find `### [pluginName]` section
3. Extract `**Status:**` line
4. Parse emoji: 💡, 🚧, ✅, or 📦
5. If 🚧, extract stage number (e.g., "🚧 Stage 4" → 4)
6. Return object: `{ emoji: "🚧", stage: 4, text: "Stage 4" }`

### validateTransition(currentStatus, newStatus)

**Purpose:** Enforce legal state machine transitions.

**Legal transitions:**
```
💡 Ideated → 🚧 Stage 0 (start workflow)
🚧 Stage N → 🚧 Stage N+1 (sequential stages)
🚧 Stage 6 → ✅ Working (validation complete)
✅ Working → 📦 Installed (install plugin)
📦 Installed → 🚧 Improving (start improvement)
🚧 Improving → 📦 Installed (improvement complete)
```

**Illegal transitions:**
```
💡 → ✅ (can't skip implementation)
🚧 Stage 2 → 🚧 Stage 5 (can't skip stages)
✅ Working → 💡 (can't go backward)
```

**Implementation:**
1. Parse current and new status
2. Check transition against rules
3. Return: `{ allowed: true }` or `{ allowed: false, reason: "..." }`

### enforceSingleInProgress()

**Purpose:** Ensure only ONE plugin is 🚧 at a time.

**Implementation:**
1. Read PLUGINS.md
2. Search for all `**Status:** 🚧` entries
3. Count occurrences
4. If starting new 🚧 and another exists:
   ```
   ✗ Cannot start [PluginA] - [PluginB] is already in progress.

   Only one plugin can be in development (🚧) at a time.

   Options:
   1. Complete [PluginB] first (recommended)
   2. Pause [PluginB] (/continue will resume it later)
   3. View [PluginB] status

   Choose (1-3): _
   ```
5. Return: `{ allowed: true }` or `{ allowed: false, blockedBy: "PluginB" }`

## Interactive Decision Menu System

### presentDecisionMenu(context)

**Purpose:** Present context-aware decision menu at every checkpoint.

**Context parameters:**
- `stage`: Current stage number (0-6)
- `completionStatement`: What was just accomplished
- `pluginName`: Plugin being worked on
- `errors`: Any errors/failures (optional)
- `options`: Custom options (optional)

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

**Keyword Shortcuts:**
- "continue" → Option 1 (primary action)
- "pause" → Pause option (creates checkpoint)
- "review" → Review option (show code/context)

**Handle "Other" responses:**
```
User: Other
System: "What would you like to do?"
User: [Free-form request]
System: [Process request]
System: [Re-present decision menu afterward]
```

### generateContextualOptions(context)

**Purpose:** Generate situation-specific menu options.

**After Stage 0 (Research):**
```javascript
[
  { label: "Continue to Stage 1", recommended: true },
  { label: "Review research findings" },
  { label: "Improve creative brief based on research" },
  { label: "Run deeper investigation (deep-research skill)" },
  { label: "Pause here" },
  { label: "Other" }
]
```

**After Stage 1 (Planning):**
```javascript
[
  { label: "Continue to Stage 2", recommended: true },
  { label: "Review plan details" },
  { label: "Adjust complexity assessment" },
  { label: "Review contracts" },
  { label: "Pause here" },
  { label: "Other" }
]
```

**After Stage 6 (Validation):**
```javascript
[
  { label: "Install plugin to system folders", recommended: true },
  { label: "Test in DAW first" },
  { label: "Create another plugin" },
  { label: "Review complete plugin code" },
  { label: "Document this plugin" },
  { label: "Other" }
]
```

**Build Failure:**
```javascript
[
  { label: "Investigate", discovery: "Run deep-research to find root cause" },
  { label: "Show me the code" },
  { label: "Show full build output" },
  { label: "I'll fix it manually (say \"resume automation\" when ready)" },
  { label: "Other" }
]
```

**Validation Failure:**
```javascript
[
  { label: "Fix and re-validate", recommended: true },
  { label: "Re-run stage" },
  { label: "Override (not recommended)" },
  { label: "Other" }
]
```

### formatDecisionMenu(completionStatement, options)

**Purpose:** Format options as inline numbered list.

**Implementation:**
```
output = `✓ ${completionStatement}\n\n`
output += `What's next?\n`

options.forEach((opt, i) => {
  output += `${i+1}. ${opt.label}`

  if (opt.recommended) {
    output += ` (recommended)`
  }

  if (opt.discovery) {
    output += ` ← ${opt.discovery}`
  }

  output += `\n`
})

output += `\nChoose (1-${options.length}): _`

return output
```

**Progressive Disclosure:**
Use discovery markers (`← User discovers [feature]`) to surface hidden capabilities:
- "Save as template ← Add to UI template library"
- "Design sync ← Validate brief matches mockup"
- "/troubleshoot-juce ← Document problems for knowledge base"

### handleMenuChoice(choice, options, context)

**Purpose:** Parse user response and execute chosen action.

**Implementation:**
```javascript
// Parse response
if (isNumber(choice)) {
  const index = parseInt(choice) - 1
  if (index >= 0 && index < options.length) {
    return executeOption(options[index], context)
  } else {
    return { error: "Invalid choice", reprompt: true }
  }
}

// Handle keyword shortcuts
if (choice.toLowerCase() === "continue") {
  return executeOption(options[0], context) // First option
}

if (choice.toLowerCase() === "pause") {
  const pauseOption = options.find(o => o.label.includes("Pause"))
  return executeOption(pauseOption, context)
}

if (choice.toLowerCase() === "review") {
  const reviewOption = options.find(o => o.label.includes("Review"))
  return executeOption(reviewOption, context)
}

// Handle "Other"
if (choice.toLowerCase() === "other" || options[choice - 1].label === "Other") {
  return { action: "ask_freeform", reprompt: true }
}
```

**After executing action:**
- Re-present menu if action was exploratory (review, show code)
- Continue workflow if action was directive (continue, pause)

## Git Commit Functions

### commitStage(pluginName, stage, description)

**Purpose:** Create standardized git commit after stage completion.

**Commit message format:**
```
feat: [PluginName] Stage [N] - [description]

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

**For phased stages:**
```
feat: [PluginName] Stage [N.M] - [phase description]

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

**Implementation:**
1. Stage changes atomically in single commit:
   ```bash
   git add plugins/[PluginName]/Source/ (if exists)
   git add plugins/[PluginName]/.ideas/ (contracts)
   git add plugins/[PluginName]/.continue-here.md (handoff)
   git add PLUGINS.md (state)
   ```

2. Commit with standardized message using heredoc:
   ```bash
   git commit -m "$(cat <<'EOF'
   feat: [PluginName] Stage [N] - [description]

   🤖 Generated with Claude Code

   Co-Authored-By: Claude <noreply@anthropic.com>
   EOF
   )"
   ```

3. Verify commit succeeded:
   ```bash
   git log -1 --format='%h'
   ```

4. Display commit hash to user:
   ```
   ✓ Committed: abc1234 - Stage [N] complete
   ```

5. If commit fails:
   - Warn user
   - Suggest manual commit
   - Continue workflow (don't block)

**Atomic state transitions:**
- PLUGINS.md update + handoff update + code changes = Single commit
- If commit fails → Rollback state changes (or warn user about inconsistency)

**Commit variations by stage:**
- Stage 0: `feat: [Plugin] Stage 0 - research complete`
- Stage 1: `feat: [Plugin] Stage 1 - planning complete`
- Stage 2: `feat: [Plugin] Stage 2 - foundation compiles`
- Stage 3: `feat: [Plugin] Stage 3 - shell loads in DAW`
- Stage 4: `feat: [Plugin] Stage 4 - DSP complete`
- Stage 4.1: `feat: [Plugin] Stage 4.1 - core processing`
- Stage 4.2: `feat: [Plugin] Stage 4.2 - parameter modulation`
- Stage 5: `feat: [Plugin] Stage 5 - GUI complete`
- Stage 6: `feat: [Plugin] Stage 6 - validation complete`

### verifyGitAvailable()

**Purpose:** Check git is available before workflow starts.

**Implementation:**
```bash
if ! command -v git &> /dev/null; then
    echo "⚠️ Warning: git not found. Commits will be skipped."
    echo "Install git to enable automatic commit workflow."
    return false
fi

if ! git rev-parse --git-dir &> /dev/null; then
    echo "⚠️ Warning: Not a git repository. Commits will be skipped."
    echo "Run 'git init' to enable automatic commit workflow."
    return false
fi

return true
```

Call at beginning of Stage 0.

## Handoff Management Functions

### createHandoff(pluginName, stage, context)

**Purpose:** Create initial handoff file after Stage 0 completion.

**Implementation:**
1. Read handoff template from `.claude/skills/plugin-workflow/assets/continue-here-template.md`
2. Fill in YAML frontmatter:
   - plugin: [pluginName]
   - stage: [stage number]
   - phase: null (only for complex plugins)
   - status: "in_progress"
   - last_updated: [current timestamp]
   - complexity_score: null (filled in Stage 1)
   - phased_implementation: null (filled in Stage 1)
3. Fill in markdown sections with context:
   - Current State: "Stage [N] - [description]"
   - Completed So Far: [what's done]
   - Next Steps: [prioritized actions]
   - Context to Preserve: [key decisions, files, build status]
4. Write to `plugins/[pluginName]/.continue-here.md`

### updateHandoff(pluginName, stage, completed, nextSteps, complexityScore, phased)

**Purpose:** Update handoff file after each stage/phase completion.

**Implementation:**
1. Read existing `plugins/[pluginName]/.continue-here.md`
2. Update YAML frontmatter:
   - stage: [new stage number]
   - phase: [phase number if complex]
   - status: [in_progress | complete]
   - last_updated: [current timestamp]
   - complexity_score: [score if known]
   - phased_implementation: [true/false if known]
3. Append to "Completed So Far" section
4. Update "Next Steps" with new actions
5. Update "Context to Preserve" with latest context
6. Write back to file

### deleteHandoff(pluginName)

**Purpose:** Remove handoff file when plugin reaches ✅ Working or 📦 Installed.

**Implementation:**
1. Check if `plugins/[pluginName]/.continue-here.md` exists
2. Delete file
3. Log deletion (workflow complete)

**When to call:**
- After Stage 6 complete (status → ✅ Working)
- After plugin installation (status → 📦 Installed)

## Checkpoint Types

### Hard Checkpoints (MUST pause for user decision)

**Stages:**
- Stage 0: Research complete
- Stage 1: Planning complete
- Stage 6: Validation complete

**Behavior:**
1. Complete stage work
2. Auto-commit changes
3. Update handoff file
4. Update PLUGINS.md
5. Present decision menu
6. **WAIT for user response** - do NOT auto-continue
7. Execute user choice

### Soft Checkpoints (can auto-continue)

**Phases within complex stages (complexity ≥3):**
- Stage 4.1, 4.2, 4.3: DSP phases
- Stage 5.1, 5.2: GUI phases

**Behavior:**
1. Complete phase work
2. Auto-commit changes
3. Update handoff file
4. Present decision menu with "Continue to next phase" as recommended option
5. If user chooses continue: proceed to next phase
6. If user chooses pause: stop and preserve state

### Decision Checkpoints

**Occur before significant choices:**
- Build failures (show 4-option menu)
- Validation failures (show 3-option menu)
- Manual pause requests

**Behavior:**
1. Update handoff with current context
2. Present situation-specific menu
3. Wait for user choice
4. Execute chosen path

## Resume Handling

**Support "resume automation" command:**

If user paused and says "resume automation" or chooses to continue:

1. Read `.continue-here.md` to determine current stage/phase
2. Parse YAML frontmatter for stage, phase, complexity_score, phased_implementation
3. Continue from documented "Next Steps"
4. Load relevant context (contracts, research, plan)

---

## Stage Boundary Protocol

**At every stage completion:**

1. Show completion statement:

```
✓ Stage [N] complete: [description]
```

2. Run automated tests (Stages 4, 5 only):

   - Invoke plugin-testing skill
   - If fail: STOP, show results, wait for fixes
   - If pass: Continue

3. Auto-commit:

```bash
git add [files]
# Message format: feat: [Plugin] Stage [N] - [description]
# For complex: feat: [Plugin] Stage [N.M] - [phase description]
```

4. Update `.continue-here.md` with new stage, timestamp, context

5. Update PLUGINS.md with new status

6. Present decision menu with context-appropriate options

7. Wait for user response

**Do NOT auto-proceed without user confirmation.**

---

## Integration Points

**Invoked by:**

- `/implement` command
- `plugin-ideation` skill (after creative brief)
- `context-resume` skill (when resuming)

**Invokes:**

- `plugin-testing` skill (Stages 4, 5, 6)
- `plugin-lifecycle` skill (after Stage 6, if user chooses install)

**Creates:**

- `.continue-here.md` (handoff file)
- `research.md` (Stage 0)
- `plan.md` (Stage 1)
- `CHANGELOG.md` (Stage 6)
- `Presets/` directory (Stage 6)

**Updates:**

- PLUGINS.md (status changes throughout)

---

## Error Handling

**If contract files missing at Stage 1:**
Block and guide to create UI mockup first.

**If build fails at any stage:**
Present menu:

```
Build error at [stage]:
[Error context]

What would you like to do?
1. Investigate (triggers deep-research)
2. Show me the code
3. Show full build output
4. I'll fix it manually (say "resume automation" when ready)
5. Other

Choose (1-5): _
```

**If tests fail:**
Present menu with investigation options.

**If git staging fails:**
Continue anyway, log warning.

---

## Success Criteria

Workflow is successful when:

- Plugin compiles without errors
- All stages completed in sequence
- Tests pass (if run)
- PLUGINS.md updated to ✅ Working
- Handoff file deleted (workflow complete)
- Git history shows all stage commits
- Ready for installation or improvement
