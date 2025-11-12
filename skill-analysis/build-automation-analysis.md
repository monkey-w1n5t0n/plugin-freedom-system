# Skill Analysis: build-automation

## Executive Summary
- Overall health: **Red**
- Critical issues: **4**
- Optimization opportunities: **8**
- Estimated context savings: **~6000 tokens / 55%**

## Dimension Scores (1-5)
1. Structure Compliance: 2/5
2. Content Extraction: 1/5
3. Instruction Clarity: 3/5
4. XML Organization: 1/5
5. Context Efficiency: 1/5
6. Claude-Optimized Language: 4/5
7. System Integration: 3/5
8. Critical Sequence Enforcement: 1/5

**Average: 2.0/5**

## Critical Issues (blockers for Claude comprehension)

1. **Line 456-467: No XML enforcement for NEVER auto-retry rule** - This is marked CRITICAL but not wrapped in `<critical_rule>` tags. Claude may miss this invariant and autonomously retry builds.

2. **Lines 44-96: Build workflow not wrapped in `<critical_sequence>`** - Six sequential steps that MUST happen in order lack enforcement. Claude might skip steps or execute out of order.

3. **Lines 3-24: Malformed YAML frontmatter** - Uses markdown list syntax instead of proper YAML arrays for `invoked_by`, `invokes`, `tools`, `output`. Will fail YAML parsers.

4. **No preconditions section** - Skill doesn't explicitly state required environment (scripts/build-and-install.sh must exist, CMake installed, etc.). Claude may invoke skill in invalid state.

## Optimization Opportunities

### Content to Extract to references/

**Lines 98-207 (110 lines) → references/failure-protocol.md**
- Section: "Failure Protocol"
- Contains: 5 failure handling options with detailed implementation steps
- Why extract: This is reference documentation that Claude needs selectively based on user choice, not upfront
- Estimated savings: ~2000 tokens

**Lines 320-453 (133 lines) → references/integration-examples.md**
- Section: "Integration Examples"
- Contains: 3 detailed scenario walkthroughs (Stage 2, Stages 3-6, plugin-improve)
- Why extract: Examples are learning aids, not execution instructions. Reference when needed.
- Estimated savings: ~2500 tokens

**Lines 525-558 (33 lines) → references/testing-guide.md**
- Section: "Testing & Debugging"
- Contains: Manual invocation, simulated failures, integration testing procedures
- Why extract: Developer documentation, not runtime instructions
- Estimated savings: ~600 tokens

**Lines 560-595 (35 lines) → references/troubleshooting.md**
- Section: "Common Issues"
- Contains: 5 common problems with solutions
- Why extract: Reference material for specific error scenarios
- Estimated savings: ~650 tokens

**Total extraction savings: ~5750 tokens (52% reduction)**

### Content to Extract to assets/

**Lines 246-307 (61 lines) → assets/success-menus.md**
- Section: "Context-Aware Decision Menu"
- Contains: 6 menu templates (Stage 2, 3, 4, 5, 6, plugin-improve)
- Why extract: Template data that can be referenced by context
- Format: Markdown template with variables {{STAGE}} {{OPTIONS}}
- Estimated savings: ~1100 tokens (templates compress well)

### XML Tag Opportunities

**Critical Priority: Wrap the "Never Auto-Retry" Rule**

Lines 456-467:

```
BEFORE:
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

AFTER:
<critical_rule name="never_auto_retry" enforcement="blocking">
**NEVER automatically retry a failed build without explicit user decision.**

When build fails:
1. MUST present failure menu (lines 99-112)
2. MUST await user choice
3. MUST NOT execute any retry logic autonomously

Violation consequences: User loses control of workflow, unexpected builds consume resources, debugging becomes impossible.
</critical_rule>
```

**Lines 44-96: Wrap Build Workflow in Critical Sequence**

```
AFTER:
<critical_sequence name="build_workflow" enforce_order="strict">

## Build Workflow Sequence

When invoked, execute these steps in exact order:

1. Input Validation
   - Verify plugin name provided
   - Check plugin directory exists: `test -d "plugins/$PLUGIN_NAME"`
   - Validate CMakeLists.txt present

2. Determine Build Flags
   - Stage 2 → `--no-install`
   - Stages 3-6 → Full build
   - plugin-improve → Full build

3. Invoke Build Script
   - Execute: `./scripts/build-and-install.sh [PluginName] [flags]`
   - Display progress in real-time

4. Monitor Build Output
   - Capture stdout/stderr
   - Watch for error indicators

5. Capture Exit Code
   - Exit 0 → Success Protocol (line 209)
   - Exit 1 → Failure Protocol (line 98)

6. Display Log Location
   - Format: `logs/[PluginName]/build_TIMESTAMP.log`

</critical_sequence>
```

**Lines 98-207: Wrap Failure Protocol in Decision Gate**

```
AFTER:
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

**Lines 475-483: Wrap State Preservation in State Requirement**

```
BEFORE:
### Preserve Context Between Retries

When user says "retry build" after manual fixes:

1. Use same flags as original build attempt
2. Preserve same invoking context (Stage 2, plugin-improve, etc.)
3. Return to same decision point after success/failure

AFTER:
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

**Lines 308-317: Wrap Handoff Protocol**

```
BEFORE:
### 4. Return to Invoking Skill

After user makes decision, return control to the skill that invoked build-automation:

- **plugin-workflow**: Returns to stage dispatcher to continue or pause
- **plugin-improve**: Returns to improvement workflow to continue or finalize
- **Manual invocation**: Exits skill, displays final status

**IMPORTANT**: Always return control to invoking skill after success. Don't continue workflow autonomously.

AFTER:
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

### Instruction Clarity Improvements

**Line 3-24: Fix YAML Frontmatter**

```
BEFORE:
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

AFTER:
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
  - troubleshooter (via Task tool)
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

**Add Success Criteria Section After Frontmatter**

```
INSERT at line 26 (after frontmatter):

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

**Line 308: Clarify Return Mechanism**

```
BEFORE:
After user makes decision, return control to the skill that invoked build-automation

AFTER:
After user makes decision, exit the skill cleanly:
1. Do NOT invoke any other skills or agents
2. Do NOT continue to next stage
3. Simply complete skill execution
4. The invoking skill/workflow will detect completion and proceed according to its own logic
```

### Verbosity Reductions

**Lines 28-42 (15 lines → 8 lines): Condense Purpose and Integration**

```
BEFORE:
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

AFTER:
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

**Lines 502-523 (22 lines → 8 lines): Condense Performance Considerations**

```
BEFORE:
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

AFTER:
## Performance Notes

- **Duration tracking:** Display "Build time: Xm Ys" in all success/failure messages
- **Log management:** Logs accumulate in `logs/[PluginName]/build_TIMESTAMP.log` (no auto-cleanup)
- **Parallel builds:** Handled by build script (Ninja), skill only monitors output
```

### Critical Sequence Enforcement

The most critical improvement is wrapping the build workflow in XML enforcement tags. Claude needs clear signals about:

1. **What MUST NOT happen:** Auto-retry without user consent
2. **What MUST happen in order:** 6-step build workflow
3. **Where to stop and wait:** Decision gates at failure and success
4. **How to delegate:** Troubleshooter invocation protocol
5. **What state to preserve:** Build flags and invoking context across retries

Current structure relies on prose ("CRITICAL", "IMPORTANT", "NEVER") which Claude may miss or misinterpret under token pressure. XML tags with explicit `enforcement="blocking"` attributes create parse-able constraints.

## Examples of Issues Found

### Example 1: Missing Critical Sequence Enforcement

**Location:** Lines 44-96 (Build Workflow)

**Issue:** Six sequential steps lack `<critical_sequence>` wrapper. Claude might skip validation (step 1) or display log location before capturing exit code.

**Impact:** Build failures misdiagnosed, error handling broken, user confusion.

**Fix:** Wrap in `<critical_sequence name="build_workflow" enforce_order="strict">` with numbered steps.

---

### Example 2: Massive Content Duplication

**Location:** Lines 246-307 (Context-Aware Decision Menus)

**Issue:** Six nearly-identical menu structures repeated verbatim (only option text differs).

**Current implementation:**
```
#### From Stage 2 (Foundation)
```
Foundation verified. What's next?
1. Continue to Stage 3 (Shell/Parameters)
2. Review generated code
3. Pause workflow
```

#### From Stage 3 (Shell)
```
Shell built successfully. What's next?
1. Continue to Stage 4 (DSP)
2. Test in DAW now
3. Review parameter code
4. Pause workflow
```
[...4 more similar blocks]
```

**Better approach (extract to assets/success-menus.md):**
```
# Success Menu Template

## Template Structure
```
{{COMPLETION_MESSAGE}}. What's next?
{{#OPTIONS}}
{{INDEX}}. {{LABEL}}{{#RECOMMENDED}} (recommended){{/RECOMMENDED}}
{{/OPTIONS}}
```

## Menu Definitions
[Define menus as structured data that skill references by stage]
```

**Estimated savings:** 1100 tokens

---

### Example 3: Never Auto-Retry Rule Not Enforced

**Location:** Lines 456-467

**Issue:** Marked "CRITICAL" in prose but not wrapped in enforcement XML.

**Current:**
```
**CRITICAL**: build-automation must NEVER automatically retry a failed build without explicit user decision.
```

**Risk:** Under token pressure or complex context, Claude might miss this prose constraint and implement auto-retry logic like:
```
Build failed. Analyzing error... Retrying with --verbose flag...
```

**Fix:**
```xml
<critical_rule name="never_auto_retry" enforcement="blocking">
NEVER automatically retry a failed build without explicit user decision.

When build fails:
1. MUST present failure menu (lines 99-112)
2. MUST await user choice
3. MUST NOT execute any retry logic autonomously
</critical_rule>
```

**Impact of fix:** Parser-level enforcement ensures Claude cannot proceed past build failure without user decision.

---

### Example 4: Unclear Return Mechanism

**Location:** Line 308-317 (Return to Invoking Skill)

**Issue:** Says "return control" but doesn't specify HOW.

**Current:**
```
After user makes decision, return control to the skill that invoked build-automation:
- **plugin-workflow**: Returns to stage dispatcher to continue or pause
```

**Ambiguity:** Does "return control" mean:
- Call a specific tool?
- Exit the skill?
- Set a variable?
- Invoke another skill?

**Fix:**
```xml
<handoff_protocol name="return_to_invoker">
After user makes decision from success menu:

1. Do NOT invoke any other skills or agents
2. Do NOT continue to next stage
3. Exit skill execution cleanly (no explicit return call needed)
4. Invoking skill/workflow will detect completion and proceed

Implementation: Simply stop executing. No special return mechanism required.
</handoff_protocol>
```

---

### Example 5: Malformed YAML Breaks Parsers

**Location:** Lines 3-24

**Issue:** Uses markdown list syntax in YAML frontmatter.

**Current:**
```yaml
invoked_by:

- plugin-workflow (Stages 2-6)
- plugin-improve (Phase 5)
```

**Problem:** Blank line after `invoked_by:` breaks YAML array parsing. Comments in parentheses not standard YAML.

**Fix:**
```yaml
invoked_by:
  - plugin-workflow
  - plugin-improve
  - plugin-lifecycle
```

**Impact:** Skill metadata is now machine-parseable for tooling, skill dashboards, dependency graphs.

## Estimated Impact

### Context Reduction
- **Extract to references/:** 5750 tokens (failure-protocol, integration-examples, testing-guide, troubleshooting)
- **Extract to assets/:** 1100 tokens (success menu templates)
- **Verbosity reduction:** ~500 tokens (condense Purpose, Integration, Performance sections)
- **Total savings:** ~6000 tokens (55% reduction from 11,000 → 5,000 tokens)

### Comprehension Improvement
**HIGH** - Critical sequence enforcement with XML tags directly addresses the primary failure modes:
1. Auto-retry violations (blocked by `<critical_rule>`)
2. Out-of-order execution (blocked by `<critical_sequence>`)
3. Missing decision gates (enforced by `<decision_gate>`)
4. Unclear handoffs (clarified by `<handoff_protocol>`)

These are structural improvements that make the skill machine-verifiable, not just human-readable.

### Maintenance Improvement
**HIGH** - Extracted content enables:
1. **Parallel evolution:** Failure protocol can be updated in references/ without touching main SKILL.md
2. **Template reuse:** Success menu templates in assets/ can be used by other skills
3. **Targeted reference:** Integration examples available when needed, not cluttering main execution path
4. **Clear structure:** XML tags create explicit semantic boundaries for future editors

## Priority Recommendations

### 1. Wrap Critical Sequences and Rules in XML Enforcement Tags
**Priority:** CRITICAL
**Lines affected:** 44-96 (build workflow), 98-207 (failure protocol), 456-467 (never auto-retry), 475-483 (state preservation), 308-317 (handoff)
**Estimated time:** 30 minutes
**Impact:** Prevents Claude from deviating from critical workflows, auto-retrying builds, or skipping decision gates

**Tags to add:**
- `<critical_sequence name="build_workflow" enforce_order="strict">` (lines 44-96)
- `<critical_rule name="never_auto_retry" enforcement="blocking">` (lines 456-467)
- `<decision_gate name="build_failure_handling" blocking="true">` (lines 98-207)
- `<state_requirement name="retry_context_preservation">` (lines 475-483)
- `<handoff_protocol name="return_to_invoker">` (lines 308-317)

### 2. Extract Large Content Blocks to references/ and assets/
**Priority:** HIGH
**Lines affected:** 98-207, 246-307, 320-453, 525-595
**Estimated time:** 45 minutes
**Impact:** 55% context reduction (6000 tokens saved), enables progressive disclosure

**Files to create:**
- `references/failure-protocol.md` (110 lines, detailed option implementations)
- `references/integration-examples.md` (133 lines, 3 scenario walkthroughs)
- `references/testing-guide.md` (33 lines, manual testing procedures)
- `references/troubleshooting.md` (35 lines, common issues)
- `assets/success-menus.md` (61 lines as templates with variables)

**SKILL.md changes:**
- Replace extracted sections with references: "See references/failure-protocol.md for option details"
- Replace menu blocks with template references: "Load menu from assets/success-menus.md using {{STAGE}} context"

### 3. Fix YAML Frontmatter and Add Preconditions
**Priority:** HIGH
**Lines affected:** 3-24, insert at 26
**Estimated time:** 10 minutes
**Impact:** Enables machine parsing of skill metadata, explicit environment requirements prevent invalid invocations

**Changes:**
1. Fix YAML array syntax (remove blank lines, use proper indentation)
2. Remove comments from YAML values (move to description field)
3. Add `preconditions:` section with explicit requirements
4. Add Success Criteria section after frontmatter

## Implementation Notes

### Files that would be created in references/
1. `failure-protocol.md` - Four failure handling options with detailed implementation steps
2. `integration-examples.md` - Three complete scenario walkthroughs (Stage 2, Stages 3-6, plugin-improve)
3. `testing-guide.md` - Manual invocation, simulated failures, integration testing procedures
4. `troubleshooting.md` - Five common issues with solutions

### Files that would be created in assets/
1. `success-menus.md` - Six context-aware menu templates with variable substitution

### Estimated time to implement all fixes
**Total: 85 minutes**
- XML enforcement tags: 30 min
- Content extraction: 45 min
- YAML + preconditions: 10 min

### Implementation Order
1. **First:** Fix YAML frontmatter (breaks skill loading if malformed)
2. **Second:** Add XML enforcement tags (prevents critical failures)
3. **Third:** Extract content to references/ and assets/ (improves performance)

### Verification Steps
After implementation:
1. Parse YAML frontmatter with validator (should pass)
2. Search for keywords "CRITICAL", "IMPORTANT", "NEVER" in SKILL.md (should all be wrapped in XML tags)
3. Check SKILL.md line count (should be ~250 lines, down from 613)
4. Verify all extracted content has "See references/X" pointers in SKILL.md
5. Test skill invocation manually to confirm references load correctly
