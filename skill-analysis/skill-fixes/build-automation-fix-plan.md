# Fix Plan: build-automation

## Summary
- Critical fixes: 8
- Extraction operations: 5
- Total estimated changes: 22
- Estimated time: 85 minutes

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Fix YAML Frontmatter
**Location:** Lines 3-24
**Operation:** REPLACE
**Before:**
```yaml
---

skill: build-automation
description: Orchestrates plugin builds using centralized build script with comprehensive failure handling
category: core
invoked_by:

- plugin-workflow (Stages 2-6)
- plugin-improve (Phase 5)
- plugin-lifecycle (via build verification)
  invokes:
- scripts/build-and-install.sh
- troubleshooter agent (on failure, if user chooses "Investigate")
  tools:
- Bash
- Read
- Task
  output:
- Build success/failure status
- Log file path
- Decision menu on failure

---
```
**After:**
```yaml
---
skill: build-automation
description: Orchestrates plugin builds using centralized build script with comprehensive failure handling. Invoked during compilation, build verification, installation. Handles both success and failure paths with structured decision menus.
category: core
invoked_by:
  - plugin-workflow
  - plugin-improve
  - plugin-lifecycle
invokes:
  - scripts/build-and-install.sh
  - troubleshooter
tools:
  - Bash
  - Read
  - Task
output:
  - build_status
  - log_file_path
  - user_decision
preconditions:
  - scripts/build-and-install.sh exists and is executable
  - Plugin directory exists at plugins/$PLUGIN_NAME
  - CMakeLists.txt exists in plugin directory
  - CMake and Ninja installed (validated by build script)
---
```
**Verification:** YAML parser should validate successfully

### Fix 1.2: Add Success Criteria Section
**Location:** Line 26 (after frontmatter)
**Operation:** INSERT
**Change:**
```markdown
## Success Criteria

This skill succeeds when:
1. Build script executes and returns exit code
2. Build result communicated to user (success or failure message)
3. Log file path displayed
4. Context-appropriate decision menu presented
5. User choice captured and executed
6. Control returned to invoking skill

This skill fails when:
- Plugin directory doesn't exist
- Build script not found or not executable
- User chooses "Wait" and doesn't resume (partial success - workflow paused intentionally)
```
**Verification:** Check that section exists after frontmatter, before "Purpose"

### Fix 1.3: Wrap Build Workflow in Critical Sequence
**Location:** Lines 44-96
**Operation:** WRAP
**Before:**
```markdown
## Build Workflow Sequence

When invoked, execute these steps in exact order:

1. **Input Validation**
   - Verify plugin name provided
   - Check plugin directory exists: `test -d "plugins/$PLUGIN_NAME"`
   - Validate CMakeLists.txt present

2. **Determine Build Flags**
   - Stage 2 → `--no-install`
   - Stages 3-6 → Full build
   - plugin-improve → Full build

3. **Invoke Build Script**
   - Execute: `./scripts/build-and-install.sh [PluginName] [flags]`
   - Display progress in real-time

4. **Monitor Build Output**
   - Capture stdout/stderr
   - Watch for error indicators

5. **Capture Exit Code**
   - Exit 0 → Success Protocol (line 209)
   - Exit 1 → Failure Protocol (line 98)

6. **Display Log Location**
   - Format: `logs/[PluginName]/build_TIMESTAMP.log`
```
**After:**
```markdown
<critical_sequence name="build_workflow" enforce_order="strict">

## Build Workflow Sequence

When invoked, execute these steps in exact order:

1. **Input Validation**
   - Verify plugin name provided
   - Check plugin directory exists: `test -d "plugins/$PLUGIN_NAME"`
   - Validate CMakeLists.txt present

2. **Determine Build Flags**
   - Stage 2 → `--no-install`
   - Stages 3-6 → Full build
   - plugin-improve → Full build

3. **Invoke Build Script**
   - Execute: `./scripts/build-and-install.sh [PluginName] [flags]`
   - Display progress in real-time

4. **Monitor Build Output**
   - Capture stdout/stderr
   - Watch for error indicators

5. **Capture Exit Code**
   - Exit 0 → Success Protocol (line 209)
   - Exit 1 → Failure Protocol (line 98)

6. **Display Log Location**
   - Format: `logs/[PluginName]/build_TIMESTAMP.log`

</critical_sequence>
```
**Verification:** Confirm XML tags present at lines 44 and 96

### Fix 1.4: Wrap Failure Protocol in Decision Gate
**Location:** Lines 98-207
**Operation:** WRAP and REPLACE
**Before:**
```markdown
## Failure Protocol

When build fails (exit code 1):

1. Extract error from log (last 50 lines or first error indicator)
2. Present structured menu:
[... full 110-line section ...]
```
**After:**
```markdown
<decision_gate name="build_failure_handling" blocking="true">

## Failure Protocol

When build fails (exit code 1):

1. Extract error from log (last 50 lines or first error indicator)
2. Present structured menu (see references/failure-protocol.md for option details)
3. WAIT for user choice (NEVER auto-proceed)
4. Execute chosen option
5. For options 1-3: Re-present menu after completion (iterative debugging)

Menu structure:
```
⚠️ Build failed

What would you like to do?
1. Investigate - Run troubleshooter agent (recommended)
2. Show me the build log - Display full build output
3. Show me the code - Open source files with error
4. Wait - I'll fix manually
5. Other
```

Option implementations: See references/failure-protocol.md

</decision_gate>
```
**Verification:** Confirm section is wrapped in decision_gate tags and reduced from 110 lines to ~20 lines

### Fix 1.5: Wrap Never Auto-Retry Rule
**Location:** Lines 456-467
**Operation:** WRAP and REPLACE
**Before:**
```markdown
### Never Auto-Retry

**CRITICAL**: build-automation must NEVER automatically retry a failed build without explicit user decision. Always present failure menu and await user choice.

Bad (DON'T DO):

```
Build failed. Retrying with different flags...
```

Good (DO THIS):

```
⚠️ Build failed

What would you like to do?
1. Investigate...
```
```
**After:**
```markdown
<critical_rule name="never_auto_retry" enforcement="blocking">

**NEVER automatically retry a failed build without explicit user decision.**

When build fails:
1. MUST present failure menu (lines 99-112)
2. MUST await user choice
3. MUST NOT execute any retry logic autonomously

Violation consequences: User loses control of workflow, unexpected builds consume resources, debugging becomes impossible.

</critical_rule>
```
**Verification:** Confirm critical_rule tags present and blocking enforcement attribute set

### Fix 1.6: Wrap State Preservation in State Requirement
**Location:** Lines 475-483
**Operation:** WRAP and REPLACE
**Before:**
```markdown
### Preserve Context Between Retries

When user says "retry build" after manual fixes:

1. Use same flags as original build attempt
2. Preserve same invoking context (Stage 2, plugin-improve, etc.)
3. Return to same decision point after success/failure
```
**After:**
```markdown
<state_requirement name="retry_context_preservation">

When user requests build retry after manual fix:

MUST preserve from original invocation:
- Build flags (`--no-install`, `--dry-run`, or none)
- Invoking skill (plugin-workflow, plugin-improve, manual)
- Invoking stage (Stage 2, 3, 4, 5, 6, or N/A)
- Last decision point (for return navigation)

Store in skill-local variables at skill entry (line 48).
Reuse on retry without re-prompting user.

</state_requirement>
```
**Verification:** Confirm state_requirement tags present

### Fix 1.7: Wrap Handoff Protocol
**Location:** Lines 308-317
**Operation:** WRAP and REPLACE
**Before:**
```markdown
### 4. Return to Invoking Skill

After user makes decision, return control to the skill that invoked build-automation:

- **plugin-workflow**: Returns to stage dispatcher to continue or pause
- **plugin-improve**: Returns to improvement workflow to continue or finalize
- **Manual invocation**: Exits skill, displays final status

**IMPORTANT**: Always return control to invoking skill after success. Don't continue workflow autonomously.
```
**After:**
```markdown
<handoff_protocol name="return_to_invoker">

After user makes decision from success menu:

1. Identify invoking skill (stored at entry)
2. Return control using appropriate mechanism:
   - **plugin-workflow**: Exit with status SUCCESS, await stage dispatcher
   - **plugin-improve**: Exit with status SUCCESS, await improvement workflow
   - **Manual invocation**: Exit with final status message

NEVER continue workflow autonomously after success.
NEVER invoke next stage directly.
ALWAYS exit and let invoking skill/workflow orchestrate next action.

</handoff_protocol>
```
**Verification:** Confirm handoff_protocol tags present and explicit "NEVER" directives included

### Fix 1.8: Clarify Return Mechanism
**Location:** Line 308
**Operation:** REPLACE
**Before:**
```markdown
After user makes decision, return control to the skill that invoked build-automation
```
**After:**
```markdown
After user makes decision, exit the skill cleanly:
1. Do NOT invoke any other skills or agents
2. Do NOT continue to next stage
3. Simply complete skill execution
4. The invoking skill/workflow will detect completion and proceed according to its own logic
```
**Verification:** Text now explicitly states "Do NOT invoke" rather than vague "return control"

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Extract Failure Protocol Details
**Location:** Lines 98-207
**Operation:** EXTRACT
**Create file:** `references/failure-protocol.md`
**File content:**
```markdown
# Failure Protocol - Option Details

## Option 1: Investigate (Recommended)

Invoke troubleshooter agent via Task tool:

```
Task: Deep investigation of build failure for [PluginName]

Context:
- Build log: logs/[PluginName]/build_TIMESTAMP.log
- Error summary: [extracted error from log]
- Build flags used: [--no-install | none]
- Invoking stage: [Stage 2-6 | plugin-improve | manual]

Instructions:
1. Analyze build log for root cause
2. Search codebase for similar issues
3. Check troubleshooting knowledge base
4. Provide fix recommendations with exact file/line changes

Return findings as structured report with:
- Root cause analysis
- Recommended fixes (ranked by confidence)
- Example code snippets
```

After troubleshooter returns:
- Display findings to user
- Ask if they want to apply suggested fix or choose another option
- Re-present failure menu

## Option 2: Show me the build log

Display full build output:

```
=== Build Log: logs/[PluginName]/build_TIMESTAMP.log ===

[full log contents]

=== End of Log ===

The error appears to be:
[highlighted error section]
```

Re-present failure menu after display.

## Option 3: Show me the code

Extract file/line references from error, display relevant code:

```
Error in: Source/PluginProcessor.cpp:45

=== Source/PluginProcessor.cpp (lines 40-50) ===
40: void PluginProcessor::processBlock(AudioBuffer<float>& buffer,
41:                                      MidiBuffer& midiMessages)
42: {
43:     ScopedNoDenormals noDenormals;
44:     auto totalNumInputChannels  = getTotalNumInputChannels();
45:     auto totalNumOutputChannels = getTotalNumOutputChannels();  // ERROR HERE
46:
47:     for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
48:         buffer.clear (i, 0, buffer.getNumSamples());
49: }
=== End of Code ===
```

Re-present failure menu after display.

## Option 4: Wait - I'll fix manually

```
Workflow paused. When ready:
- Make your changes to the source files
- Say "retry build" to rebuild with same flags
- Or say "continue [PluginName]" to resume workflow
```

Exit skill. Do NOT re-present menu. User will resume when ready.

## Option 5: Other

Capture user's custom action:
- If user says "retry": Re-invoke build with same flags
- If user says "skip": Continue to next stage (plugin-workflow only)
- If user says "abort": Exit workflow completely
- Otherwise: Execute user's specific instruction
```
**Replace with:**
```markdown
See detailed option implementations in `references/failure-protocol.md`
```
**Verification:** File exists at references/failure-protocol.md and SKILL.md reduced by ~110 lines

### Fix 2.2: Extract Integration Examples
**Location:** Lines 320-453
**Operation:** EXTRACT
**Create file:** `references/integration-examples.md`
**File content:**
```markdown
# Integration Examples

## Scenario 1: From Stage 2 (Foundation)

**Context:** plugin-workflow invoked build-automation after foundation-agent completed

**Invocation:**
```
Invoking build-automation for LushVerb (Stage 2: Foundation)
Flags: --no-install
```

**Workflow:**
1. Validate plugin directory exists
2. Execute: `./scripts/build-and-install.sh LushVerb --no-install`
3. Build succeeds (foundation code compiles)
4. Present success menu:

```
Foundation verified. What's next?
1. Continue to Stage 3 (Shell/Parameters) (recommended)
2. Review generated code
3. Pause workflow
```

5. User chooses 1
6. Exit skill, return to plugin-workflow
7. plugin-workflow invokes shell-agent

## Scenario 2: From Stages 3-6

**Context:** plugin-workflow invoked build-automation after shell-agent/dsp-agent/gui-agent completed

**Invocation:**
```
Invoking build-automation for LushVerb (Stage 4: DSP)
Flags: none (full build + install)
```

**Workflow:**
1. Validate plugin directory exists
2. Execute: `./scripts/build-and-install.sh LushVerb`
3. Build succeeds, binaries installed to system folders
4. Present success menu:

```
DSP implementation complete. What's next?
1. Continue to Stage 5 (GUI) (recommended)
2. Test in DAW now
3. Review DSP code
4. Pause workflow
```

5. User chooses 2 (test in DAW)
6. User tests plugin, finds issue
7. User returns: "continue"
8. Present menu again, user chooses 1
9. Exit skill, return to plugin-workflow
10. plugin-workflow invokes gui-agent

## Scenario 3: From plugin-improve

**Context:** plugin-improve invoked build-automation after applying fixes

**Invocation:**
```
Invoking build-automation for LushVerb (plugin-improve Phase 5)
Flags: none (full build + install)
```

**Workflow:**
1. Validate plugin directory exists
2. Execute: `./scripts/build-and-install.sh LushVerb`
3. Build fails (introduced regression)
4. Present failure menu:

```
⚠️ Build failed

What would you like to do?
1. Investigate - Run troubleshooter agent (recommended)
2. Show me the build log
3. Show me the code
4. Wait - I'll fix manually
5. Other
```

5. User chooses 1 (investigate)
6. Invoke troubleshooter agent
7. Troubleshooter finds: "Missing include for <memory> in PluginProcessor.cpp"
8. Display findings, re-present failure menu
9. User chooses 4 (wait, I'll fix manually)
10. Exit skill, return to plugin-improve
11. plugin-improve restores backup, asks user what to do next
```
**Replace with:**
```markdown
See complete scenario walkthroughs in `references/integration-examples.md`
```
**Verification:** File exists and SKILL.md reduced by ~133 lines

### Fix 2.3: Extract Testing Guide
**Location:** Lines 525-558
**Operation:** EXTRACT
**Create file:** `references/testing-guide.md`
**File content:**
```markdown
# Testing & Debugging Guide

## Manual Invocation

Test build-automation directly without workflow:

```
Invoke build-automation skill for [PluginName]
```

Claude will:
1. Execute build workflow
2. Present appropriate menu on success/failure
3. Await your decision

Use this to test:
- Success menu variations (different stages)
- Failure handling options
- State preservation across retries

## Simulated Failures

To test failure protocol without breaking code:

1. Temporarily rename CMakeLists.txt → CMakeLists.txt.bak
2. Invoke build-automation
3. Build will fail immediately (missing CMakeLists.txt)
4. Test failure menu options
5. Restore CMakeLists.txt

Or inject syntax error:

1. Add `#error "Test failure"` to PluginProcessor.cpp
2. Invoke build-automation
3. Build will fail with clear error
4. Test troubleshooter invocation
5. Remove error directive

## Integration Testing

Test within workflow:

1. Start `/implement [PluginName]`
2. Let workflow reach Stage 2 completion
3. Observe build-automation invocation
4. Test success menu choices
5. Continue workflow through Stage 6

Verify:
- Correct flags used per stage (--no-install for Stage 2, none for others)
- Context-appropriate menus displayed
- State preserved when pausing/resuming
```
**Replace with:**
```markdown
See `references/testing-guide.md` for manual testing procedures
```
**Verification:** File exists and SKILL.md reduced by ~33 lines

### Fix 2.4: Extract Troubleshooting
**Location:** Lines 560-595
**Operation:** EXTRACT
**Create file:** `references/troubleshooting.md`
**File content:**
```markdown
# Common Issues

## Build script not found

**Symptom:** `./scripts/build-and-install.sh: No such file or directory`

**Cause:** Skill invoked from wrong directory or script missing

**Fix:**
1. Verify working directory is plugin-freedom-system root
2. Check script exists: `ls -la scripts/build-and-install.sh`
3. Ensure script executable: `chmod +x scripts/build-and-install.sh`

## Plugin directory doesn't exist

**Symptom:** `plugins/[PluginName]: No such file or directory`

**Cause:** Skill invoked before plugin created or wrong name used

**Fix:**
1. Verify plugin exists: `ls plugins/`
2. Check exact name (case-sensitive)
3. If missing, run `/dream` and `/implement` first

## CMakeLists.txt missing

**Symptom:** `CMake Error: The source directory does not appear to contain CMakeLists.txt`

**Cause:** Foundation stage not completed or file deleted

**Fix:**
1. Check if file exists: `ls plugins/[PluginName]/CMakeLists.txt`
2. If missing, re-run Stage 2 (foundation-agent)
3. Or restore from backup in `plugins/[PluginName]/.backup/`

## Build succeeds but menu not displayed

**Symptom:** Build completes but no "What's next?" menu appears

**Cause:** Success protocol not executed (line 209+ not reached)

**Fix:**
1. Check if skill exited early
2. Verify exit code capture logic (line 87-91)
3. Ensure Success Protocol section executed

## Troubleshooter invocation fails

**Symptom:** "Failed to invoke troubleshooter agent"

**Cause:** Task tool error or agent not found

**Fix:**
1. Verify troubleshooter agent exists in `.claude/agents/`
2. Check Task tool permissions
3. Try manual troubleshooting (option 2 or 3) instead
```
**Replace with:**
```markdown
See `references/troubleshooting.md` for common issues and solutions
```
**Verification:** File exists and SKILL.md reduced by ~35 lines

### Fix 2.5: Extract Success Menu Templates to Assets
**Location:** Lines 246-307
**Operation:** EXTRACT
**Create file:** `assets/success-menus.md`
**File content:**
```markdown
# Success Menu Templates

## Template Structure

```
{{COMPLETION_MESSAGE}}. What's next?
{{#OPTIONS}}
{{INDEX}}. {{LABEL}}{{#RECOMMENDED}} (recommended){{/RECOMMENDED}}
{{/OPTIONS}}
```

## Menu Definitions

### Stage 2 (Foundation)
**Completion message:** "Foundation verified"
**Options:**
1. Continue to Stage 3 (Shell/Parameters) [recommended]
2. Review generated code
3. Pause workflow

### Stage 3 (Shell)
**Completion message:** "Shell built successfully"
**Options:**
1. Continue to Stage 4 (DSP) [recommended]
2. Test in DAW now
3. Review parameter code
4. Pause workflow

### Stage 4 (DSP)
**Completion message:** "DSP implementation complete"
**Options:**
1. Continue to Stage 5 (GUI) [recommended]
2. Test in DAW now
3. Review DSP code
4. Pause workflow

### Stage 5 (GUI)
**Completion message:** "GUI implementation complete"
**Options:**
1. Continue to Stage 6 (Validation) [recommended]
2. Test in DAW now
3. Review GUI code
4. Pause workflow

### Stage 6 (Validation)
**Completion message:** "Plugin complete and validated"
**Options:**
1. Test in DAW now [recommended]
2. Review final code
3. Package for distribution
4. Start new plugin

### plugin-improve
**Completion message:** "Improvements applied successfully"
**Options:**
1. Test in DAW now [recommended]
2. Review changes
3. Apply more improvements
4. Finalize version

## Usage

Load appropriate template based on invoking context:
- Extract stage from invocation parameters
- Select matching template
- Substitute {{COMPLETION_MESSAGE}} and {{OPTIONS}}
- Present to user
```
**Replace with:**
```markdown
Load context-appropriate menu from `assets/success-menus.md` based on invoking stage
```
**Verification:** File exists and SKILL.md reduced by ~61 lines

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Condense Purpose and Integration
**Location:** Lines 28-42
**Operation:** REPLACE
**Before:**
```markdown
## Purpose

The build-automation skill orchestrates plugin builds by invoking the centralized build script (`scripts/build-and-install.sh`) and providing comprehensive failure handling through a structured 4-option protocol. It bridges the gap between workflow stages and the build system, handling both success and failure paths gracefully.

## Integration Points

This skill is invoked by:

- **plugin-workflow**: After each stage completion (Stages 2-6) to build and verify changes
- **plugin-improve**: After implementation changes (Phase 5) to rebuild with modifications
- **plugin-lifecycle**: During installation verification to ensure plugins are buildable

This skill invokes:

- **scripts/build-and-install.sh**: Centralized 7-phase build pipeline
- **troubleshooter agent**: Deep research agent (via Task tool) when user chooses "Investigate" option
```
**After:**
```markdown
## Purpose

Orchestrates plugin builds via `scripts/build-and-install.sh` with comprehensive failure handling.

**Invokers:** plugin-workflow (Stages 2-6), plugin-improve (Phase 5), plugin-lifecycle (verification)
**Invokes:** build script, troubleshooter agent (on failure, user option 1)

**Key behaviors:**
- Context-aware build flags (Stage 2 uses `--no-install`, others full build)
- Structured 5-option failure menu (never auto-retry)
- Context-specific success menus (different per stage)
- Always returns control to invoker (never continues autonomously)
```
**Verification:** Section reduced from 15 lines to 8 lines, information preserved

### Fix 3.2: Condense Performance Considerations
**Location:** Lines 502-523
**Operation:** REPLACE
**Before:**
```markdown
## Performance Considerations

### Build Duration Tracking

Always display build duration in success/failure messages:

- Start timer before invoking build script
- Stop timer after exit
- Format as "Build time: 2m 34s" or "Build time: 45s"

### Log File Management

- Build logs accumulate in `logs/[PluginName]/`
- Each build creates timestamped log: `build_20250110_143022.log`
- No automatic cleanup (user can manually delete old logs)
- Always reference most recent log file

### Parallel Builds

Build script handles parallelization (Ninja builds VST3 + AU simultaneously). build-automation skill doesn't need to manage this - just invoke script and monitor output.
```
**After:**
```markdown
## Performance Notes

- **Duration tracking:** Display "Build time: Xm Ys" in all success/failure messages
- **Log management:** Logs accumulate in `logs/[PluginName]/build_TIMESTAMP.log` (no auto-cleanup)
- **Parallel builds:** Handled by build script (Ninja), skill only monitors output
```
**Verification:** Section reduced from 22 lines to 8 lines

## File Creation Manifest

Files to create:

1. `references/failure-protocol.md` - Detailed implementations of 5 failure handling options
2. `references/integration-examples.md` - 3 complete scenario walkthroughs (Stage 2, Stages 3-6, plugin-improve)
3. `references/testing-guide.md` - Manual invocation, simulated failures, integration testing procedures
4. `references/troubleshooting.md` - 5 common issues with solutions
5. `assets/success-menus.md` - 6 context-aware menu templates with variable substitution

All files should be created in `.claude/skills/build-automation/` directory.

## Execution Checklist

- [ ] Phase 1: YAML frontmatter is valid and includes preconditions
- [ ] Phase 1: Success Criteria section added after frontmatter
- [ ] Phase 1: Build workflow wrapped in `<critical_sequence>` (lines 44-96)
- [ ] Phase 1: Failure protocol wrapped in `<decision_gate>` (lines 98-207)
- [ ] Phase 1: Never auto-retry rule wrapped in `<critical_rule>` (lines 456-467)
- [ ] Phase 1: State preservation wrapped in `<state_requirement>` (lines 475-483)
- [ ] Phase 1: Handoff protocol wrapped in `<handoff_protocol>` (lines 308-317)
- [ ] Phase 1: Return mechanism clarified with explicit "Do NOT invoke" directives
- [ ] Phase 2: failure-protocol.md created with 5 option implementations
- [ ] Phase 2: integration-examples.md created with 3 scenarios
- [ ] Phase 2: testing-guide.md created with testing procedures
- [ ] Phase 2: troubleshooting.md created with 5 common issues
- [ ] Phase 2: success-menus.md created in assets/ with 6 templates
- [ ] Phase 2: All extracted sections replaced with references in SKILL.md
- [ ] Phase 3: Purpose and Integration section condensed (lines 28-42)
- [ ] Phase 3: Performance Considerations condensed (lines 502-523)
- [ ] Verification: SKILL.md reduced from ~613 lines to ~250 lines
- [ ] Verification: All XML tags have required attributes (enforcement, blocking, etc.)
- [ ] Verification: No prose "CRITICAL" or "IMPORTANT" outside XML tags
- [ ] Verification: All references/ and assets/ files exist and are complete

## Expected Outcomes

**Before implementation:**
- Health score: 2.0/5 (Red)
- Line count: ~613 lines
- Token count: ~11,000 tokens
- Critical issues: 4
- XML enforcement: Minimal

**After implementation:**
- Estimated health score: 4.0/5 (Green)
- Estimated line count: ~250 lines (59% reduction)
- Estimated token count: ~5,000 tokens (55% reduction)
- Critical issues: 0
- XML enforcement: Complete (5 wrapped sections)
