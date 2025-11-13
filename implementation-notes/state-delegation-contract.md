# State Delegation Contract - Subagent State Management

**Purpose:** Define the exact contract for how subagents update state files, reducing orchestrator token overhead by 57.5k tokens per workflow.

**Status:** Contract definition (implementation in prompts 047-049)

**Last Updated:** 2025-11-13

---

## 1. State File Formats

### 1.1 .continue-here.md Format

**Location:** `plugins/[PluginName]/.continue-here.md`

**Format:** YAML frontmatter + Markdown sections

**Required YAML fields:**
```yaml
---
plugin: [PluginName]              # String - exact plugin name (required)
stage: [0-6]                      # Integer - current stage number (required)
phase: [null or N.M]              # String/null - phase number if phased (e.g., "4.2")
status: [status_value]            # String - workflow status (required)
last_updated: [YYYY-MM-DD]        # String - date only, no time (required)
complexity_score: [N.N or null]   # Float/null - from plan.md (required after Stage 0)
phased_implementation: [bool/null] # Boolean/null - from plan.md (required after Stage 0)
orchestration_mode: true          # Boolean - always true (enables dispatcher pattern)
next_action: [action or null]     # String/null - which subagent to invoke next (optional)
next_phase: [phase or null]       # String/null - next phase number if phased (optional)
contract_checksums:               # Object - SHA256 checksums for tamper detection
  creative_brief: [sha256:hash]   # String - checksum of creative-brief.md
  parameter_spec: [sha256:hash]   # String - checksum of parameter-spec.md
  architecture: [sha256:hash]     # String - checksum of architecture.md
  plan: [sha256:hash or null]     # String/null - checksum of plan.md (null if doesn't exist)
---
```

**Valid status values:**
- `in_progress` - Stage/phase currently being worked on
- `complete` - Stage/phase finished, waiting for decision
- `workflow_complete` - All stages finished (Stage 6 complete)

**Required Markdown sections:**
```markdown
# [Current State Description]

## Current State: [One-line summary]
[What stage/phase just completed or is in progress]

## Completed So Far
[Bulleted list of completed stages/phases with brief descriptions]

## Next Steps
[Prioritized list of remaining work]

## Build Artifacts
[Paths to compiled binaries - updated after successful builds]

## Testing Checklist
[Manual testing items - updated as stages complete]

## Context to Preserve
[Key decisions, implementation notes, file locations]
```

**Validation rules:**
1. YAML frontmatter must be valid YAML (no syntax errors)
2. `plugin` field must match directory name
3. `stage` must be integer 0-6
4. `phase` must be null or format "N.M" (e.g., "4.2")
5. `last_updated` must be valid ISO date (YYYY-MM-DD)
6. `complexity_score` must be float 1.0-5.0 or null (null only before Stage 0 complete)
7. `phased_implementation` must be boolean or null (null only before Stage 0 complete)
8. Contract checksums must be valid sha256 format or "null" string
9. All required Markdown sections must be present

### 1.2 PLUGINS.md Format

**Location:** Root `PLUGINS.md`

**Two update locations (must stay synchronized):**

**Location 1: Registry table**
```markdown
| Plugin Name | Status | Version | Last Updated |
|-------------|--------|---------|--------------|
| PluginName  | 🚧 Stage 2 | 1.0.0 | 2025-11-13 |
```

**Location 2: Full entry section**
```markdown
### PluginName
**Status:** 🚧 Stage 2
**Version:** 1.0.0
**Created:** 2025-11-10
**Type:** Audio Effect

**Description:**
[Plugin description]

**Lifecycle Timeline:**
- **2025-11-10:** Creative brief created
- **2025-11-13 (Stage 2):** Foundation complete - build system operational

**Last Updated:** 2025-11-13
```

**Valid status emoji values:**
- `💡 Ideated` - Creative brief exists, no implementation
- `💡 Ideated (Draft Params)` - Creative brief + draft parameters captured
- `🚧 Stage 0` - Research & planning in progress
- `🚧 Stage 2` - Build system implementation
- `🚧 Stage 3` - Audio engine implementation
- `🚧 Stage 4` - UI integration
- `🚧 Stage 5` - Validation in progress
- `✅ Working` - Stage 6 complete, not installed
- `📦 Installed` - Deployed to system folders

**Validation rules:**
1. Registry table status must match full entry status (character-for-character after normalization)
2. Both locations must be updated atomically (all or nothing)
3. Last Updated date must match in both locations
4. Timeline must be in chronological order
5. Timeline entries must use format: `- **YYYY-MM-DD (Stage N):** [description]`

---

## 2. Subagent Responsibilities

### 2.1 Stage 0 (Research & Planning Agent)

**Subagent:** `research-planning-agent`
**Owns stages:** 0 only
**Trigger:** `/plan` command or `/implement` with no planning

**State updates required:**

**PLUGINS.md updates:**
- **Status change:** `💡 Ideated (Draft Params)` → `🚧 Stage 0`
- **Timeline append:** `- **[date] (Stage 0):** Research & Planning complete - Complexity [score]`
- **Last Updated:** Current date

**.continue-here.md updates:**
- **Create file** (first time)
- **stage:** 0
- **phase:** null
- **status:** "complete"
- **last_updated:** Current date (YYYY-MM-DD)
- **complexity_score:** From analysis (e.g., 4.2)
- **phased_implementation:** true/false based on complexity ≥3
- **contract_checksums:** Calculate all four checksums
- **Markdown sections:** Fill all sections with Stage 0 context

**Example before:**
```yaml
# File doesn't exist yet
```

**Example after:**
```yaml
---
plugin: MinimalKick
stage: 0
phase: null
status: complete
last_updated: 2025-11-13
complexity_score: 5.0
phased_implementation: true
orchestration_mode: true
next_action: invoke_foundation_shell_agent
next_phase: null
contract_checksums:
  creative_brief: sha256:abc123...
  parameter_spec: sha256:def456...
  architecture: sha256:ghi789...
  plan: sha256:jkl012...
---

# Stage 0 Complete - Research & Planning

## Current State: Research & Planning Complete

Complexity score: 5.0 (Complex - phased implementation)
Implementation strategy: 6 phases (3 DSP + 3 GUI)

## Completed So Far
- **Stage 0:** Research complete - DSP architecture documented

## Next Steps
1. Invoke foundation-shell-agent for Stage 2
2. Implement build system and parameters
3. Build and verify compilation

## Context to Preserve
- Complexity: 5.0 (maximum)
- Phased: true (6 phases total)
- Architecture: plugins/MinimalKick/.ideas/architecture.md
- Plan: plugins/MinimalKick/.ideas/plan.md
```

### 2.2 Stage 2 (Foundation-Shell Agent)

**Subagent:** `foundation-shell-agent`
**Owns stages:** 2 only
**Trigger:** After Stage 0 complete, orchestrator invokes via Task tool

**State updates required:**

**PLUGINS.md updates:**
- **Status change:** `🚧 Stage 0` → `🚧 Stage 2`
- **Timeline append:** `- **[date] (Stage 2):** Foundation complete - Build system operational`
- **Last Updated:** Current date

**.continue-here.md updates:**
- **stage:** 2
- **phase:** null (always single-pass, never phased)
- **status:** "complete"
- **last_updated:** Current date (YYYY-MM-DD)
- **next_action:** "invoke_dsp_agent"
- **next_phase:** null (or "3.1" if complexity ≥3)
- **Append to "Completed So Far":** `- **Stage 2:** Foundation complete - Build system operational, N parameters implemented`
- **Update "Next Steps":** Remove Stage 2 items, add Stage 3 items
- **Update "Build Artifacts":** Add paths to compiled binaries (after build succeeds)

**Example before:**
```yaml
---
plugin: MinimalKick
stage: 0
status: complete
last_updated: 2025-11-13
---
```

**Example after:**
```yaml
---
plugin: MinimalKick
stage: 2
phase: null
status: complete
last_updated: 2025-11-13
complexity_score: 5.0
phased_implementation: true
next_action: invoke_dsp_agent
next_phase: 3.1
contract_checksums:
  creative_brief: sha256:abc123...
  parameter_spec: sha256:def456...
  architecture: sha256:ghi789...
  plan: sha256:jkl012...
---

# Stage 2 Complete - Build System Ready

## Current State: Foundation Complete

Build system operational, 5 parameters implemented.

## Completed So Far
- **Stage 0:** Research complete - DSP architecture documented
- **Stage 2:** Foundation complete - Build system operational, 5 parameters implemented

## Next Steps
1. Invoke dsp-agent for Stage 3 Phase 3.1
2. Implement core synthesis (oscillator + MIDI)
3. Build and verify audio output

## Build Artifacts
- VST3: build/MinimalKick/plugins/MinimalKick/MinimalKick_artefacts/VST3/MinimalKick.vst3
- AU: build/MinimalKick/plugins/MinimalKick/MinimalKick_artefacts/AU/MinimalKick.component
- Standalone: build/MinimalKick/plugins/MinimalKick/MinimalKick_artefacts/Standalone/MinimalKick.app
```

### 2.3 Stage 3 (DSP Agent)

**Subagent:** `dsp-agent`
**Owns stages:** 3 only (may run in multiple phases if complex)
**Trigger:** After Stage 2 complete, orchestrator invokes via Task tool

**Complexity handling:**
- **Simple (≤2):** Single-pass implementation (phase: null)
- **Complex (≥3):** Phased implementation (phase: "3.1", "3.2", etc.)

**State updates required:**

**For each phase completion (phased) OR single completion (simple):**

**PLUGINS.md updates:**
- **Status change:**
  - Simple: `🚧 Stage 2` → `🚧 Stage 3`
  - Phased first: `🚧 Stage 2` → `🚧 Stage 3.1`
  - Phased subsequent: `🚧 Stage 3.N` → `🚧 Stage 3.N+1`
  - Phased final: `🚧 Stage 3.N` → `🚧 Stage 3` (complete)
- **Timeline append:**
  - Simple: `- **[date] (Stage 3):** Audio engine complete - [description]`
  - Phased: `- **[date] (Stage 3.N):** Phase 3.N complete - [phase description]`
- **Last Updated:** Current date

**.continue-here.md updates:**
- **stage:** 3
- **phase:** null (simple) or "3.N" (phased)
- **status:** "complete" (phase or stage complete)
- **last_updated:** Current date
- **next_action:**
  - If phase complete: "continue_dsp_phase_3.N+1"
  - If stage complete: "invoke_gui_agent"
- **next_phase:**
  - If phase complete: "3.N+1"
  - If stage complete: "4.1" (if GUI phased) or null
- **Append to "Completed So Far":** `- **Stage 3.N:** Phase description` or `- **Stage 3:** Complete description`
- **Update "Next Steps":** Update for next phase or next stage

**Example before (simple):**
```yaml
---
plugin: GainKnob
stage: 2
phase: null
status: complete
complexity_score: 1.2
phased_implementation: false
---
```

**Example after (simple):**
```yaml
---
plugin: GainKnob
stage: 3
phase: null
status: complete
last_updated: 2025-11-13
complexity_score: 1.2
phased_implementation: false
next_action: invoke_gui_agent
next_phase: null
---
```

**Example before (phased - moving to phase 3.2):**
```yaml
---
plugin: MinimalKick
stage: 3
phase: 3.1
status: complete
complexity_score: 5.0
phased_implementation: true
next_phase: 3.2
---
```

**Example after (phased - phase 3.2 complete):**
```yaml
---
plugin: MinimalKick
stage: 3
phase: 3.2
status: complete
last_updated: 2025-11-13
complexity_score: 5.0
phased_implementation: true
next_action: continue_dsp_phase_3.3
next_phase: 3.3
---
```

**Example before (phased - final phase):**
```yaml
---
plugin: MinimalKick
stage: 3
phase: 3.3
status: complete
next_phase: null
---
```

**Example after (phased - stage complete):**
```yaml
---
plugin: MinimalKick
stage: 3
phase: null
status: complete
last_updated: 2025-11-13
complexity_score: 5.0
phased_implementation: true
next_action: invoke_gui_agent
next_phase: 4.1
---
```

### 2.4 Stage 4 (GUI Agent)

**Subagent:** `gui-agent`
**Owns stages:** 4 only (may run in multiple phases if complex)
**Trigger:** After Stage 3 complete, orchestrator invokes via Task tool

**Complexity handling:**
- **Simple (≤2):** Single-pass implementation (phase: null)
- **Complex (≥3):** Phased implementation (phase: "4.1", "4.2", "4.3")

**State updates required:**

**For each phase completion (phased) OR single completion (simple):**

**PLUGINS.md updates:**
- **Status change:**
  - Simple: `🚧 Stage 3` → `🚧 Stage 4`
  - Phased first: `🚧 Stage 3` → `🚧 Stage 4.1`
  - Phased subsequent: `🚧 Stage 4.N` → `🚧 Stage 4.N+1`
  - Phased final: `🚧 Stage 4.N` → `🚧 Stage 4` (complete)
- **Timeline append:**
  - Simple: `- **[date] (Stage 4):** UI complete - [description]`
  - Phased: `- **[date] (Stage 4.N):** Phase 4.N complete - [phase description]`
- **Last Updated:** Current date

**.continue-here.md updates:**
- **stage:** 4
- **phase:** null (simple) or "4.N" (phased)
- **status:** "complete" (phase or stage complete)
- **last_updated:** Current date
- **next_action:**
  - If phase complete: "continue_gui_phase_4.N+1"
  - If stage complete: "invoke_validation" or "begin_stage_5"
- **next_phase:**
  - If phase complete: "4.N+1"
  - If stage complete: null
- **Append to "Completed So Far":** Phase or stage completion entry
- **Update "Next Steps":** Update for next phase or Stage 5

**Example before (simple):**
```yaml
---
plugin: GainKnob
stage: 3
phase: null
status: complete
next_action: invoke_gui_agent
---
```

**Example after (simple):**
```yaml
---
plugin: GainKnob
stage: 4
phase: null
status: complete
last_updated: 2025-11-13
next_action: begin_stage_5
next_phase: null
---
```

**Example before (phased - phase 4.2 complete):**
```yaml
---
plugin: MinimalKick
stage: 4
phase: 4.2
status: complete
next_phase: 4.3
---
```

**Example after (phased - stage complete, moving to 5):**
```yaml
---
plugin: MinimalKick
stage: 4
phase: null
status: complete
last_updated: 2025-11-13
next_action: begin_stage_5
next_phase: null
---
```

### 2.5 Stage 5 (Validation - No Subagent)

**Subagent:** None (handled directly by orchestrator)
**Owns stages:** 5 only
**Trigger:** After Stage 4 complete, orchestrator runs validation directly

**State updates required:**

**PLUGINS.md updates:**
- **Status change:** `🚧 Stage 4` → `✅ Working`
- **Timeline append:** `- **[date] (Stage 5):** Validation complete - ready for installation`
- **Last Updated:** Current date

**.continue-here.md updates:**
- **stage:** 5
- **phase:** null
- **status:** "workflow_complete"
- **last_updated:** Current date
- **next_action:** "install" or "improve" or null
- **next_phase:** null
- **Append to "Completed So Far":** `- **Stage 5:** Validation complete - CHANGELOG created, ready to install`
- **Update "Next Steps":** Installation or improvement options

**Example before:**
```yaml
---
plugin: MinimalKick
stage: 4
phase: null
status: complete
---
```

**Example after:**
```yaml
---
plugin: MinimalKick
stage: 5
phase: null
status: workflow_complete
last_updated: 2025-11-13
next_action: null
next_phase: null
---

# Stage 5 Complete - Ready for Installation

## Current State: Validation Complete

All stages complete. Plugin built successfully. CHANGELOG created. Ready to install.

## Completed So Far
- **Stage 0:** Research complete
- **Stage 2:** Foundation complete
- **Stage 3:** Audio engine complete
- **Stage 4:** UI complete
- **Stage 5:** Validation complete
```

**Note:** After installation (status → `📦 Installed`), orchestrator deletes `.continue-here.md` (no longer needed).

---

## 3. Update Protocol

**Step-by-step process each subagent MUST follow:**

### 3.1 Read Current State
```bash
# Read existing .continue-here.md
read_file("plugins/[PluginName]/.continue-here.md")

# Parse YAML frontmatter
parse_yaml(frontmatter)

# Verify current stage matches expected stage
assert current_stage == expected_stage
```

### 3.2 Calculate Contract Checksums
```bash
# Calculate SHA256 checksums for tamper detection
BRIEF_SHA=$(shasum -a 256 plugins/$PLUGIN_NAME/.ideas/creative-brief.md | awk '{print $1}')
PARAM_SHA=$(shasum -a 256 plugins/$PLUGIN_NAME/.ideas/parameter-spec.md | awk '{print $1}')
ARCH_SHA=$(shasum -a 256 plugins/$PLUGIN_NAME/.ideas/architecture.md | awk '{print $1}')
PLAN_SHA=$(shasum -a 256 plugins/$PLUGIN_NAME/.ideas/plan.md | awk '{print $1}')

# Format as "sha256:hash"
checksums = {
  creative_brief: "sha256:${BRIEF_SHA}",
  parameter_spec: "sha256:${PARAM_SHA}",
  architecture: "sha256:${ARCH_SHA}",
  plan: "sha256:${PLAN_SHA}"
}
```

### 3.3 Update .continue-here.md Fields

**Update YAML frontmatter:**
```yaml
stage: [new_stage_number]         # Increment or set to current stage
phase: [new_phase or null]        # Set phase if phased, else null
status: "complete"                # Always "complete" after subagent finishes
last_updated: [YYYY-MM-DD]        # Current date
next_action: [next_subagent]      # Which subagent to invoke next
next_phase: [next_phase or null]  # Next phase number if phased
contract_checksums:               # Recalculated checksums
  creative_brief: [sha256:hash]
  parameter_spec: [sha256:hash]
  architecture: [sha256:hash]
  plan: [sha256:hash]
```

**Update Markdown sections:**
```markdown
## Completed So Far
[Append new entry: "- **Stage N:** Description"]

## Next Steps
[Remove completed items, add next stage/phase items]

## Build Artifacts
[Update paths if build succeeded]

## Testing Checklist
[Update checkmarks for completed tests]
```

### 3.4 Update PLUGINS.md Status

**Two atomic updates (must both succeed or both fail):**

**Update 1: Registry table**
```markdown
| PluginName | 🚧 Stage N | 1.0.0 | 2025-11-13 |
```

**Update 2: Full entry**
```markdown
### PluginName
**Status:** 🚧 Stage N
...
**Lifecycle Timeline:**
- **2025-11-13 (Stage N):** Stage description

**Last Updated:** 2025-11-13
```

**Validation after update:**
```bash
# Verify both locations match
grep "^| ${PLUGIN_NAME} |" PLUGINS.md | grep "Stage ${STAGE_NUM}"
grep -A1 "^### ${PLUGIN_NAME}$" PLUGINS.md | grep "Stage ${STAGE_NUM}"
```

### 3.5 Report Success in JSON

**Return subagent report:**
```json
{
  "agent": "[subagent-name]",
  "status": "success",
  "outputs": {
    "stage_completed": 3,
    "phase_completed": "3.2",
    "files_modified": [
      "plugins/PluginName/Source/PluginProcessor.cpp"
    ],
    "state_files_updated": [
      "plugins/PluginName/.continue-here.md",
      "PLUGINS.md"
    ]
  },
  "issues": [],
  "ready_for_next_stage": true
}
```

---

## 4. Error Handling

### 4.1 Possible Errors

**File write errors:**
- `.continue-here.md` write fails (permissions, disk full)
- `PLUGINS.md` write fails (file locked, corrupted)

**Format errors:**
- Invalid YAML syntax in frontmatter
- Missing required fields
- Invalid field values (e.g., stage: "foo")

**State mismatch errors:**
- Current stage doesn't match expected stage
- Phase number inconsistent with plan.md
- Contract checksums changed (tampering detected)

**PLUGINS.md drift:**
- Registry table doesn't match full entry
- Status emoji mismatch between locations

### 4.2 Subagent Error Reporting

**If state update fails, report in JSON:**
```json
{
  "agent": "[subagent-name]",
  "status": "failure",
  "outputs": {
    "error_type": "state_update_failure",
    "error_message": "Failed to write .continue-here.md: Permission denied",
    "failed_operation": "write_continue_here",
    "implementation_status": "complete",
    "state_status": "not_updated"
  },
  "issues": [
    "Implementation succeeded but state update failed",
    "Manual state correction required",
    "Code changes completed: [list files]"
  ],
  "ready_for_next_stage": false
}
```

**Critical:** Subagent reports implementation vs. state status separately. Implementation may succeed while state update fails.

### 4.3 Orchestrator Error Handling

**When subagent reports state update failure:**

1. **Verify implementation succeeded:**
   - Check reported files_modified
   - Verify code changes present

2. **Attempt manual state update:**
   - Read current state
   - Calculate checksums
   - Update .continue-here.md
   - Update PLUGINS.md (atomic)
   - Validate consistency

3. **If manual update succeeds:**
   - Log warning: "Subagent state update failed, corrected by orchestrator"
   - Continue workflow normally
   - Consider subagent fix

4. **If manual update fails:**
   - Present error menu to user:
     ```
     State update failed for Stage N.

     What would you like to do?
     1. Show me the error details
     2. Manually fix state files (I'll guide you)
     3. Skip state update and continue (NOT RECOMMENDED)
     4. Abort workflow
     5. Other
     ```

### 4.4 Recovery Procedures

**For corrupted .continue-here.md:**
```bash
# Restore from git history
git checkout HEAD~1 -- plugins/[PluginName]/.continue-here.md

# Or regenerate from current state
# (orchestrator function: regenerateContinueHere)
```

**For PLUGINS.md drift:**
```bash
# Sync registry table to match full entry (full entry is canonical)
# (orchestrator function: syncRegistryTable)
```

**For contract tampering:**
```bash
# Option 1: Restore contracts from git
git checkout HEAD -- plugins/[PluginName]/.ideas/

# Option 2: Update checksums if changes intentional
# (orchestrator function: updateContractChecksums)
```

---

## 5. Verification Contract

### 5.1 Orchestrator Verification After Subagent Completes

**Mandatory checks:**

1. **Verify JSON report structure:**
   ```javascript
   assert report.agent == expected_agent_name
   assert report.status in ["success", "failure"]
   assert report.outputs exists
   assert report.ready_for_next_stage is boolean
   ```

2. **Verify .continue-here.md updated:**
   ```bash
   # Check file modified timestamp
   stat -f "%m" plugins/[PluginName]/.continue-here.md

   # Verify stage field updated
   grep "^stage: ${EXPECTED_STAGE}$" plugins/[PluginName]/.continue-here.md

   # Verify status is "complete"
   grep "^status: complete$" plugins/[PluginName]/.continue-here.md
   ```

3. **Verify PLUGINS.md updated:**
   ```bash
   # Check both locations match
   TABLE_STATUS=$(grep "^| ${PLUGIN_NAME} |" PLUGINS.md | awk -F'|' '{print $3}')
   ENTRY_STATUS=$(grep -A1 "^### ${PLUGIN_NAME}$" PLUGINS.md | grep "^**Status:**")

   # Normalize and compare
   assert normalize($TABLE_STATUS) == normalize($ENTRY_STATUS)
   ```

4. **Verify contract checksums present:**
   ```bash
   grep "creative_brief: sha256:" plugins/[PluginName]/.continue-here.md
   grep "parameter_spec: sha256:" plugins/[PluginName]/.continue-here.md
   grep "architecture: sha256:" plugins/[PluginName]/.continue-here.md
   grep "plan: sha256:" plugins/[PluginName]/.continue-here.md
   ```

5. **Verify timeline updated:**
   ```bash
   # Check for new timeline entry
   grep "- \*\*$(date +%Y-%m-%d) (Stage ${STAGE}):\*\*" PLUGINS.md
   ```

### 5.2 Fallback Actions

**If verification fails, orchestrator takes over:**

**Missing .continue-here.md update:**
```javascript
if (!verifyContinueHereUpdated(pluginName, expectedStage)) {
  logWarning("Subagent did not update .continue-here.md")

  // Orchestrator updates state
  updateHandoff(pluginName, expectedStage, completed, nextSteps)

  // Continue workflow
  proceedToCheckpoint()
}
```

**Missing PLUGINS.md update:**
```javascript
if (!verifyPluginsMdUpdated(pluginName, expectedStage)) {
  logWarning("Subagent did not update PLUGINS.md")

  // Orchestrator updates status
  updatePluginStatus(pluginName, newStatus)
  updatePluginTimeline(pluginName, expectedStage, description)

  // Continue workflow
  proceedToCheckpoint()
}
```

**PLUGINS.md drift detected:**
```javascript
if (!validateRegistryConsistency(pluginName)) {
  logError("Registry drift detected")

  // Present auto-fix menu
  presentDriftMenu(pluginName)
}
```

### 5.3 Detection of Missing Updates

**How to detect "subagent didn't update state":**

1. **Check file modification time:**
   ```bash
   # Get last modified timestamp
   BEFORE_TIMESTAMP=$(date +%s)
   # ... invoke subagent ...
   AFTER_TIMESTAMP=$(stat -f "%m" plugins/[PluginName]/.continue-here.md)

   if [ $AFTER_TIMESTAMP -le $BEFORE_TIMESTAMP ]; then
     echo "WARNING: .continue-here.md not modified by subagent"
   fi
   ```

2. **Check stage field value:**
   ```bash
   # Extract stage from .continue-here.md
   ACTUAL_STAGE=$(grep "^stage:" plugins/[PluginName]/.continue-here.md | awk '{print $2}')

   if [ "$ACTUAL_STAGE" != "$EXPECTED_STAGE" ]; then
     echo "WARNING: Stage field not updated (expected: $EXPECTED_STAGE, actual: $ACTUAL_STAGE)"
   fi
   ```

3. **Check JSON report:**
   ```javascript
   if (report.outputs.state_files_updated) {
     // Subagent claims it updated state
     if (!verifyStateUpdated()) {
       logError("Subagent reported state update but verification failed")
     }
   } else {
     // Subagent explicitly states it didn't update
     logWarning("Subagent did not update state files, orchestrator will handle")
     updateStateFiles()
   }
   ```

---

## 6. Migration Strategy

### 6.1 Existing Plugin Compatibility

**Old state format (.continue-here.md before this contract):**
- May be missing `next_action`, `next_phase` fields
- May be missing `contract_checksums` section
- May have incomplete Markdown sections

**Migration logic:**

```javascript
function migrateOldHandoff(pluginName) {
  const handoffPath = `plugins/${pluginName}/.continue-here.md`

  // Read existing file
  const content = readFile(handoffPath)
  const [frontmatter, markdown] = parseFrontmatter(content)

  // Check if migration needed
  if (frontmatter.contract_checksums) {
    return // Already migrated
  }

  // Add missing fields
  frontmatter.orchestration_mode = true
  frontmatter.next_action = determineNextAction(frontmatter.stage, frontmatter.phase)
  frontmatter.next_phase = determineNextPhase(frontmatter.stage, frontmatter.phase, pluginName)

  // Calculate checksums
  frontmatter.contract_checksums = {
    creative_brief: calculateChecksum(`plugins/${pluginName}/.ideas/creative-brief.md`),
    parameter_spec: calculateChecksum(`plugins/${pluginName}/.ideas/parameter-spec.md`),
    architecture: calculateChecksum(`plugins/${pluginName}/.ideas/architecture.md`),
    plan: calculateChecksum(`plugins/${pluginName}/.ideas/plan.md`)
  }

  // Write back
  writeFile(handoffPath, formatYamlMarkdown(frontmatter, markdown))

  logInfo(`Migrated ${handoffPath} to new format`)
}
```

### 6.2 One-Time Migration

**When to migrate:**
- On first `/continue [PluginName]` command after contract implementation
- Automatically detect old format and migrate

**Migration script:**
```bash
#!/bin/bash
# migrate-handoffs.sh - One-time migration of all .continue-here.md files

for HANDOFF in plugins/*/.continue-here.md; do
  PLUGIN_NAME=$(basename $(dirname "$HANDOFF"))

  # Check if already migrated
  if grep -q "contract_checksums:" "$HANDOFF"; then
    echo "✓ $PLUGIN_NAME already migrated"
    continue
  fi

  echo "Migrating $PLUGIN_NAME..."

  # Calculate checksums
  BRIEF=$(shasum -a 256 "plugins/$PLUGIN_NAME/.ideas/creative-brief.md" | awk '{print $1}')
  PARAM=$(shasum -a 256 "plugins/$PLUGIN_NAME/.ideas/parameter-spec.md" | awk '{print $1}')
  ARCH=$(shasum -a 256 "plugins/$PLUGIN_NAME/.ideas/architecture.md" | awk '{print $1}')
  PLAN=$(shasum -a 256 "plugins/$PLUGIN_NAME/.ideas/plan.md" | awk '{print $1}' 2>/dev/null || echo "null")

  # Append checksums section before first ---
  sed -i '' "/^---$/a\\
contract_checksums:\\
  creative_brief: sha256:$BRIEF\\
  parameter_spec: sha256:$PARAM\\
  architecture: sha256:$ARCH\\
  plan: sha256:$PLAN\\
orchestration_mode: true
" "$HANDOFF"

  echo "✓ $PLUGIN_NAME migrated"
done

echo "Migration complete"
```

### 6.3 Backward Compatibility

**Requirements:**
- Old workflows (without state delegation) must continue to work
- Orchestrator can still update state if subagent doesn't
- Gradual rollout: Some subagents updated, others not

**Compatibility checks:**
```javascript
function isSubagentStateDelegationEnabled(subagentName) {
  // Check if subagent supports state delegation
  const supportedAgents = [
    "research-planning-agent",
    "foundation-shell-agent",
    "dsp-agent",
    "gui-agent"
  ]

  return supportedAgents.includes(subagentName)
}

function handleSubagentReturn(report, pluginName, expectedStage) {
  if (isSubagentStateDelegationEnabled(report.agent)) {
    // New behavior: Verify subagent updated state
    if (!verifyStateUpdated(pluginName, expectedStage)) {
      logWarning("State delegation enabled but update missing, falling back to orchestrator")
      updateStateFiles(pluginName, expectedStage) // Fallback
    }
  } else {
    // Old behavior: Orchestrator always updates state
    updateStateFiles(pluginName, expectedStage)
  }

  // Continue with checkpoint protocol
  proceedToCheckpoint(pluginName, expectedStage)
}
```

---

## 7. Success Criteria

### 7.1 How to Verify State Update Succeeded

**Per-update verification:**

1. **File exists and is valid YAML:**
   ```bash
   test -f plugins/[PluginName]/.continue-here.md
   python3 -c "import yaml; yaml.safe_load(open('plugins/[PluginName]/.continue-here.md'))"
   ```

2. **Required fields present:**
   ```bash
   grep -q "^plugin: " plugins/[PluginName]/.continue-here.md
   grep -q "^stage: " plugins/[PluginName]/.continue-here.md
   grep -q "^status: " plugins/[PluginName]/.continue-here.md
   grep -q "^last_updated: " plugins/[PluginName]/.continue-here.md
   ```

3. **Stage field matches expected:**
   ```bash
   ACTUAL=$(grep "^stage:" plugins/[PluginName]/.continue-here.md | awk '{print $2}')
   test "$ACTUAL" = "$EXPECTED_STAGE"
   ```

4. **PLUGINS.md consistency:**
   ```bash
   # Verify both locations match
   TABLE=$(grep "^| ${PLUGIN_NAME} |" PLUGINS.md | awk -F'|' '{print $3}' | xargs)
   ENTRY=$(grep -A1 "^### ${PLUGIN_NAME}$" PLUGINS.md | grep "^**Status:**" | sed 's/\*\*//g; s/Status://g' | xargs)
   test "$TABLE" = "$ENTRY"
   ```

5. **Timeline entry added:**
   ```bash
   grep -q "- \*\*$(date +%Y-%m-%d) (Stage ${STAGE}):\*\*" PLUGINS.md
   ```

### 7.2 What Makes an Update "Valid"

**An update is valid if ALL of these are true:**

1. ✓ `.continue-here.md` exists and is parseable YAML
2. ✓ All required YAML fields present and correct types
3. ✓ `stage` field matches expected stage number
4. ✓ `status` is "complete" or "in_progress"
5. ✓ `last_updated` is current date (YYYY-MM-DD)
6. ✓ `contract_checksums` section present with 4 checksums
7. ✓ All required Markdown sections present
8. ✓ PLUGINS.md registry table updated
9. ✓ PLUGINS.md full entry updated
10. ✓ Registry table matches full entry (no drift)
11. ✓ Timeline entry added to PLUGINS.md
12. ✓ "Completed So Far" section updated with new entry

**Validation function:**
```javascript
function validateStateUpdate(pluginName, expectedStage) {
  const checks = [
    checkFileExists(pluginName),
    checkValidYaml(pluginName),
    checkRequiredFields(pluginName),
    checkStageMatches(pluginName, expectedStage),
    checkStatusValid(pluginName),
    checkDateCurrent(pluginName),
    checkChecksumsPresent(pluginName),
    checkMarkdownSectionsPresent(pluginName),
    checkPluginsMdTableUpdated(pluginName, expectedStage),
    checkPluginsMdEntryUpdated(pluginName, expectedStage),
    checkPluginsMdConsistency(pluginName),
    checkTimelineUpdated(pluginName, expectedStage),
    checkCompletedSectionUpdated(pluginName, expectedStage)
  ]

  const failed = checks.filter(c => !c.passed)

  if (failed.length > 0) {
    logError(`State update validation failed: ${failed.map(c => c.name).join(", ")}`)
    return { valid: false, failures: failed }
  }

  return { valid: true }
}
```

### 7.3 Token Reduction Measurement

**Baseline (before delegation):**
- Orchestrator updates state: ~57,500 tokens per workflow
- Breakdown:
  - Stage 0: 15,000 tokens (state updates)
  - Stage 2: 12,000 tokens (state updates)
  - Stage 3: 15,000 tokens (state updates, phases)
  - Stage 4: 12,000 tokens (state updates, phases)
  - Stage 5: 3,500 tokens (state updates)

**Target (after delegation):**
- Subagents update state: ~5,000 tokens per workflow
- Breakdown:
  - Stage 0: 1,000 tokens (verification only)
  - Stage 2: 1,000 tokens (verification only)
  - Stage 3: 1,500 tokens (verification, phase handling)
  - Stage 4: 1,200 tokens (verification, phase handling)
  - Stage 5: 300 tokens (verification only)

**Measurement:**
- Track orchestrator token usage before/after implementation
- Measure per-stage token counts
- Calculate total reduction: 57.5k → 5k = 52.5k tokens saved (91% reduction)

**Success threshold:**
- ≥50k token reduction per workflow (87% reduction minimum)
- All state updates verified correctly
- No increase in errors or failures

---

## Appendix A: State Machine Diagram

```
💡 Ideated (Draft Params)
  ↓ [/plan command]
🚧 Stage 0 (research-planning-agent)
  ↓ [/implement command]
🚧 Stage 2 (foundation-shell-agent)
  ↓ [auto-continue]
🚧 Stage 3 (dsp-agent, may be phased)
  ↓ [auto-continue]
🚧 Stage 4 (gui-agent, may be phased)
  ↓ [auto-continue]
🚧 Stage 5 (validation, orchestrator direct)
  ↓ [validation complete]
✅ Working
  ↓ [/install-plugin command]
📦 Installed
```

**Phase notation:**
- Simple: Stage N (no phase)
- Complex: Stage N.1 → N.2 → N.3 → Stage N (complete)

---

## Appendix B: File Paths Reference

**State files:**
- Handoff: `plugins/[PluginName]/.continue-here.md`
- Registry: `PLUGINS.md` (root)

**Contract files:**
- Creative brief: `plugins/[PluginName]/.ideas/creative-brief.md`
- Parameter spec: `plugins/[PluginName]/.ideas/parameter-spec.md`
- Architecture: `plugins/[PluginName]/.ideas/architecture.md`
- Plan: `plugins/[PluginName]/.ideas/plan.md`

**Source files:**
- CMake: `plugins/[PluginName]/CMakeLists.txt`
- Processor: `plugins/[PluginName]/Source/PluginProcessor.h/cpp`
- Editor: `plugins/[PluginName]/Source/PluginEditor.h/cpp`
- WebView: `plugins/[PluginName]/Source/[PluginName]WebView.h/cpp` (if GUI)

**Build artifacts:**
- VST3: `build/[PluginName]/plugins/[PluginName]/[PluginName]_artefacts/VST3/[PluginName].vst3`
- AU: `build/[PluginName]/plugins/[PluginName]/[PluginName]_artefacts/AU/[PluginName].component`
- Standalone: `build/[PluginName]/plugins/[PluginName]/[PluginName]_artefacts/Standalone/[PluginName].app`

---

## Appendix C: Example Workflows

### Simple Plugin (Complexity 1.2)

**Stage flow:**
1. Stage 0: research-planning-agent updates state (complexity: 1.2, phased: false)
2. Stage 2: foundation-shell-agent updates state (phase: null)
3. Stage 3: dsp-agent updates state (phase: null, single-pass)
4. Stage 4: gui-agent updates state (phase: null, single-pass)
5. Stage 5: orchestrator updates state (status: workflow_complete)
6. Install: orchestrator deletes .continue-here.md

**State transitions:**
```
💡 Ideated (Draft Params)
→ 🚧 Stage 0
→ 🚧 Stage 2
→ 🚧 Stage 3
→ 🚧 Stage 4
→ ✅ Working
→ 📦 Installed
```

### Complex Plugin (Complexity 5.0)

**Stage flow:**
1. Stage 0: research-planning-agent updates state (complexity: 5.0, phased: true)
2. Stage 2: foundation-shell-agent updates state (phase: null)
3. Stage 3.1: dsp-agent updates state (phase: "3.1")
4. Stage 3.2: dsp-agent updates state (phase: "3.2")
5. Stage 3.3: dsp-agent updates state (phase: null, stage complete)
6. Stage 4.1: gui-agent updates state (phase: "4.1")
7. Stage 4.2: gui-agent updates state (phase: "4.2")
8. Stage 4.3: gui-agent updates state (phase: null, stage complete)
9. Stage 5: orchestrator updates state (status: workflow_complete)
10. Install: orchestrator deletes .continue-here.md

**State transitions:**
```
💡 Ideated (Draft Params)
→ 🚧 Stage 0
→ 🚧 Stage 2
→ 🚧 Stage 3.1
→ 🚧 Stage 3.2
→ 🚧 Stage 3.3
→ 🚧 Stage 3
→ 🚧 Stage 4.1
→ 🚧 Stage 4.2
→ 🚧 Stage 4.3
→ 🚧 Stage 4
→ ✅ Working
→ 📦 Installed
```

---

**END OF CONTRACT**
