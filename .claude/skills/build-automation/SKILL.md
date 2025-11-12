# build-automation

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

## Purpose

Orchestrates plugin builds via `scripts/build-and-install.sh` with comprehensive failure handling.

**Invokers:** plugin-workflow (Stages 2-6), plugin-improve (Phase 5), plugin-lifecycle (verification)
**Invokes:** build script, troubleshooter agent (on failure, user option 1)

**Key behaviors:**
- Context-aware build flags (Stage 2 uses `--no-install`, others full build)
- Structured 5-option failure menu (never auto-retry)
- Context-specific success menus (different per stage)
- Always returns control to invoker (never continues autonomously)

<critical_sequence name="build_workflow" enforce_order="strict">

## Build Workflow

When invoked, the build-automation skill follows this workflow:

### 1. Input Validation

- Verify plugin name provided
- Check plugin directory exists: `test -d "plugins/$PLUGIN_NAME"`
- Validate CMakeLists.txt present: `test -f "plugins/$PLUGIN_NAME/CMakeLists.txt"`

### 2. Determine Build Flags

Context-aware flag selection:

- **Stage 2 (Foundation)**: Always use `--no-install` flag (verify compilation only, no installation)
- **Stages 3-6 (Shell/DSP/GUI/Validation)**: Full build with installation (no flags)
- **plugin-improve**: Full build with installation (no flags)
- **User manual invocation**: Ask if they want `--dry-run` to preview commands

### 3. Invoke Build Script

Execute build script with appropriate flags:

```bash
./scripts/build-and-install.sh [PluginName] [flags]
```

Display build progress in real-time using Bash tool.

### 4. Monitor Build Output

- Capture stdout and stderr
- Display progress messages to user
- Watch for error indicators
- Track build duration

### 5. Capture Exit Code

Check build script exit code:

- **Exit 0**: Build succeeded → proceed to Success Protocol
- **Exit 1**: Build failed → proceed to Failure Protocol

### 6. Log File Location

Always show log file path after build attempt:

```
Build log: logs/[PluginName]/build_TIMESTAMP.log
```

User can review full build output from log file if needed.

</critical_sequence>

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

## Success Protocol

When build succeeds (exit code 0), follow this workflow:

### 1. Verify Success

- Confirm build script exit code 0
- Check log contains "Phase 7: Verification" success message
- Extract installed plugin locations from script output

### 2. Display Success Message

Use this template:

```
✓ Build successful

Built and installed:
- VST3: ~/Library/Audio/Plug-Ins/VST3/[ProductName].vst3
- AU: ~/Library/Audio/Plug-Ins/Components/[ProductName].component

Build time: [duration]
Log: logs/[PluginName]/build_TIMESTAMP.log
```

For `--no-install` builds (Stage 2):

```
✓ Build successful (compilation verified, not installed)

Built artifacts:
- VST3: plugins/[PluginName]/build/[PluginName]_artefacts/Release/VST3/[ProductName].vst3
- AU: plugins/[PluginName]/build/[PluginName]_artefacts/Release/AU/[ProductName].component

Build time: [duration]
Log: logs/[PluginName]/build_TIMESTAMP.log
```

### 3. Context-Aware Decision Menu

Load context-appropriate menu from `assets/success-menus.md` based on invoking stage

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

Exit the skill cleanly:
1. Do NOT invoke any other skills or agents
2. Do NOT continue to next stage
3. Simply complete skill execution
4. The invoking skill/workflow will detect completion and proceed according to its own logic

</handoff_protocol>

## Integration Examples

See complete scenario walkthroughs in `references/integration-examples.md`

## Error Handling Rules

<critical_rule name="never_auto_retry" enforcement="blocking">

**NEVER automatically retry a failed build without explicit user decision.**

When build fails:
1. MUST present failure menu (lines 99-112)
2. MUST await user choice
3. MUST NOT execute any retry logic autonomously

Violation consequences: User loses control of workflow, unexpected builds consume resources, debugging becomes impossible.

</critical_rule>

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

### Handle Missing Dependencies

If build script fails with dependency errors (CMake, Ninja, JUCE not found):

1. Display specific missing dependency
2. Provide installation command (e.g., "Install with: brew install ninja")
3. After user installs, offer: "Retry build now?"

### Parse Errors Intelligently

Extract meaningful error information for troubleshooter:

- **CMake errors**: Configuration issues, missing modules, path problems
- **Compilation errors**: Syntax, type mismatches, missing declarations
- **Linker errors**: Missing symbols, library issues
- **Installation errors**: Permission issues, path conflicts

Pass full context to troubleshooter, not just error message.

## Performance Notes

- **Duration tracking:** Display "Build time: Xm Ys" in all success/failure messages
- **Log management:** Logs accumulate in `logs/[PluginName]/build_TIMESTAMP.log` (no auto-cleanup)
- **Parallel builds:** Handled by build script (Ninja), skill only monitors output

## Testing & Debugging

See `references/testing-guide.md` for manual testing procedures

## Common Issues

See `references/troubleshooting.md` for common issues and solutions

## Future Enhancements

Potential additions for later phases:

1. **Build caching**: Incremental builds for faster iteration
2. **Parallel plugin builds**: Build multiple plugins simultaneously
3. **Build profiles**: Debug, Release, RelWithDebInfo
4. **Build notifications**: macOS notifications for long builds
5. **Build analytics**: Track success rates, common failures, build times
6. **Custom build targets**: Support non-standard JUCE targets
7. **Windows/Linux support**: Extend build system beyond macOS

---

**Skill Status**: Ready for integration
**Last Updated**: 2025-01-10
**Dependencies**: scripts/build-and-install.sh, .claude/agents/troubleshooter.md
