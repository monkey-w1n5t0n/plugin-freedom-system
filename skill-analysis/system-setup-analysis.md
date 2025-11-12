# Skill Analysis: system-setup

## Executive Summary
- Overall health: **Yellow**
- Critical issues: **3**
- Optimization opportunities: **8**
- Estimated context savings: **~4,200 tokens / 25%**

## Dimension Scores (1-5)
1. Structure Compliance: **4**/5
2. Content Extraction: **2**/5
3. Instruction Clarity: **3**/5
4. XML Organization: **2**/5
5. Context Efficiency: **2**/5
6. Claude-Optimized Language: **3**/5
7. System Integration: **4**/5
8. Critical Sequence Enforcement: **1**/5

## Critical Issues (blockers for Claude comprehension)

### Issue 1: No critical sequence enforcement (Lines 76-455)
The dependency validation workflow is described in natural language across ~380 lines, but there's no XML structure to enforce the order. Claude could easily skip steps or validate out of sequence.

**Location:** Lines 139-455 (entire dependency validation section)
**Impact:** High - Without explicit enforcement, Claude may:
- Skip version checks
- Install without checking first
- Proceed when dependencies are missing
- Not follow checkpoint protocol consistently

### Issue 2: Ambiguous pronoun references throughout
Lines 144-153 use "If found", "If not found" repeatedly without clear antecedents when multiple conditions nest.

**Example (Lines 144-152):**
```
2. **If found:**
   - Display version and path
   - Validate it meets minimum requirements
   - Save to config
   - Continue to next dependency
3. **If not found:**
   - **Automated mode:** Offer to install automatically
```

**Issue:** When nested 3+ levels deep (mode → dependency → found/not-found → user choice), "it" becomes ambiguous.

### Issue 3: Test mode handling described but not enforced (Lines 77-114)
Test mode logic is explained in prose, but there's no guaranteed sequence to ensure TEST_MODE is checked before every system-check.sh call.

**Location:** Lines 77-114
**Impact:** Claude could forget to append `--test=$SCENARIO` to some calls, breaking test mode.

## Optimization Opportunities

### Content to Extract to references/

#### Extract 1: Dependency validation steps (Lines 154-455)
**Lines:** 154-455 (~300 lines)
**Description:** All five dependency validation workflows (Python, Build Tools, CMake, JUCE, pluginval)
**Target file:** `references/dependency-validation-flows.md`
**Token savings:** ~3,000 tokens

These are highly repetitive patterns that could be extracted and referenced:
```markdown
For each dependency, see references/dependency-validation-flows.md section:
- Python: ${DEPENDENCY_VALIDATION.python}
- Build Tools: ${DEPENDENCY_VALIDATION.build_tools}
- CMake: ${DEPENDENCY_VALIDATION.cmake}
- JUCE: ${DEPENDENCY_VALIDATION.juce}
- pluginval: ${DEPENDENCY_VALIDATION.pluginval}
```

#### Extract 2: Error handling scenarios (Lines 534-606)
**Lines:** 534-606 (~70 lines)
**Description:** All error handling patterns with examples
**Target file:** `references/error-handling-patterns.md`
**Token savings:** ~700 tokens

The error handling is consistent across dependencies - extract the pattern once.

#### Extract 3: Success criteria and notes (Lines 653-706)
**Lines:** 653-706 (~50 lines)
**Description:** Verbose notes and common pitfalls
**Target file:** `references/implementation-notes.md`
**Token savings:** ~500 tokens

### Content to Extract to assets/

No additional assets needed - system-check.sh already exists and is appropriately placed.

### XML Tag Opportunities

#### XML Opportunity 1: Wrap critical sequence (Lines 76-114)
**Lines:** 76-114
**Purpose:** Enforce test mode detection happens first, before any system interaction

**BEFORE:**
```markdown
When invoked via `/setup` command:

**Check for test mode first:**
- If user provided `--test=SCENARIO` argument, set TEST_MODE variable
- Pass test mode to all system-check.sh invocations via `--test=$SCENARIO`
```

**AFTER:**
```markdown
<critical_sequence name="skill_entry">
  <step number="1" required="true">
    <description>Check for test mode argument</description>
    <condition>User provided --test=SCENARIO</condition>
    <action>Set TEST_MODE variable</action>
    <enforcement>MUST check before any system interaction</enforcement>
  </step>

  <step number="2" required="true">
    <description>Display welcome message</description>
    <action>Show setup mode selection menu (automated/guided/check-only)</action>
    <checkpoint>Wait for user choice (1-4)</checkpoint>
  </step>

  <step number="3" required="true">
    <description>Store user mode choice</description>
    <action>Save to MODE variable for use in validation workflow</action>
  </step>
</critical_sequence>
```

#### XML Opportunity 2: Enforce dependency validation order (Lines 139-455)
**Lines:** 139-455
**Purpose:** Guarantee dependencies are validated in correct order with proper gates

**BEFORE:**
```markdown
## Dependency Validation Workflow

For each dependency (in order):

1. **Check if already installed and functional**
2. **If found:**
   - Display version and path
   ...
```

**AFTER:**
```markdown
<critical_sequence name="dependency_validation">
  <sequence_requirement>
    Dependencies MUST be validated in this exact order.
    NEVER skip to later dependencies if earlier ones fail in check-only mode.
  </sequence_requirement>

  <step number="1" dependency="python" required="true">
    <validation>
      <check_command>bash .claude/skills/system-setup/assets/system-check.sh --check-python ${TEST_MODE:+--test=$TEST_MODE}</check_command>
      <success_criteria>found=true AND meets_minimum=true</success_criteria>
    </validation>

    <decision_gate condition="not_found OR version_too_old">
      <mode name="automated">
        <action>Present installation menu (lines 176-186)</action>
        <checkpoint>Wait for user choice</checkpoint>
      </mode>
      <mode name="guided">
        <action>Display manual instructions from references/platform-requirements.md</action>
        <checkpoint>Wait for user confirmation</checkpoint>
      </mode>
      <mode name="check-only">
        <action>Report as missing, continue to next dependency</action>
      </mode>
    </decision_gate>

    <on_success>
      <action>Save to config: python_path, python_version</action>
      <action>Display: ✓ Python {version} found at {path}</action>
      <action>Continue to step 2</action>
    </on_success>
  </step>

  <step number="2" dependency="build_tools" required="true">
    <!-- Similar structure for build tools -->
  </step>

  <step number="3" dependency="cmake" required="true">
    <!-- Similar structure for cmake -->
  </step>

  <step number="4" dependency="juce" required="true">
    <!-- Similar structure for JUCE -->
  </step>

  <step number="5" dependency="pluginval" required="false">
    <!-- Similar structure for pluginval -->
  </step>
</critical_sequence>
```

#### XML Opportunity 3: Wrap checkpoint protocol enforcement (Lines 610-627)
**Lines:** 610-627
**Purpose:** Make checkpoint requirements explicit and enforceable

**BEFORE:**
```markdown
## Checkpoint Protocol Integration

This skill follows the checkpoint protocol:

**After each major validation step:**
1. Display what was found/installed
2. Present numbered decision menu
3. Wait for user response
4. Execute chosen action
```

**AFTER:**
```markdown
<checkpoint_protocol>
  <requirement type="mandatory">
    After each dependency validation completes (success OR user-skipped), execute checkpoint:

    <checkpoint_steps>
      <step>Display validation result (✓ found or ✗ missing/skipped)</step>
      <step>Present numbered decision menu</step>
      <step>WAIT for user response (NEVER auto-proceed)</step>
      <step>Execute user's chosen action</step>
    </checkpoint_steps>
  </requirement>

  <checkpoint_moments>
    <moment>After platform detection (line 126-135)</moment>
    <moment>After each dependency validation (Python, Build Tools, CMake, JUCE, pluginval)</moment>
    <moment>After configuration saved (line 461-486)</moment>
    <moment>At final system report (line 492-528)</moment>
  </checkpoint_moments>

  <anti_pattern>
    NEVER use AskUserQuestion tool - ALWAYS use inline numbered menus as shown in examples.
  </anti_pattern>
</checkpoint_protocol>
```

#### XML Opportunity 4: Enforce configuration persistence requirements (Lines 459-487)
**Lines:** 459-487
**Purpose:** Guarantee config is saved with all required fields

**BEFORE:**
```markdown
## Configuration Persistence

After all dependencies are validated, create `.claude/system-config.json`:

```bash
# Generate config file
cat > .claude/system-config.json <<EOF
{
  "platform": "darwin",
  ...
}
EOF
```
```

**AFTER:**
```markdown
<state_requirement name="configuration_persistence">
  <trigger>After all dependencies validated (or user completes check-only mode)</trigger>

  <required_fields>
    <field name="platform" source="detect_platform" required="always"/>
    <field name="platform_version" source="detect_platform" required="always"/>
    <field name="arch" source="detect_platform" required="always"/>
    <field name="python_path" source="check_python" required="if_found"/>
    <field name="python_version" source="check_python" required="if_found"/>
    <field name="xcode_path" source="check_xcode" required="if_found_and_macos"/>
    <field name="cmake_path" source="check_cmake" required="if_found"/>
    <field name="cmake_version" source="check_cmake" required="if_found"/>
    <field name="juce_path" source="check_juce" required="if_found"/>
    <field name="juce_version" source="check_juce" required="if_found"/>
    <field name="pluginval_path" source="check_pluginval" required="if_found"/>
    <field name="pluginval_version" source="check_pluginval" required="if_found"/>
    <field name="validated_at" source="timestamp" required="always"/>
  </required_fields>

  <validation>
    MUST include all fields where dependency was found.
    MUST NOT include fields for dependencies not found.
    MUST use absolute paths (no relative paths, expand ~).
  </validation>

  <example_generation>
    See lines 462-479 for bash implementation.
  </example_generation>

  <gitignore_requirement>
    MUST add .claude/system-config.json to .gitignore if not present (line 484-486).
  </gitignore_requirement>
</state_requirement>
```

### Instruction Clarity Improvements

#### Improvement 1: Ambiguous nested conditionals (Lines 144-153)
**Issue:** Multiple "If found" / "If not found" branches without clear scoping

**BEFORE:**
```markdown
For each dependency (in order):

1. **Check if already installed and functional**
2. **If found:**
   - Display version and path
   - Validate it meets minimum requirements
   - Save to config
   - Continue to next dependency
3. **If not found:**
   - **Automated mode:** Offer to install automatically
   - **Guided mode:** Show manual installation instructions
```

**AFTER:**
```markdown
For each dependency, execute this validation pattern:

<dependency_validation_pattern>
1. FIRST: Run check command via system-check.sh
2. THEN: Parse JSON response to determine state
3. BRANCH on result:

   IF dependency found AND version meets minimum:
     - Display success: "✓ {name} {version} found at {path}"
     - Save to config: {name}_path, {name}_version
     - CONTINUE to next dependency

   ELSE IF dependency found BUT version too old:
     - Display warning: "✗ {name} {found_version} found, but {minimum} required"
     - BRANCH on user's setup mode:
       - Automated mode: Offer upgrade menu
       - Guided mode: Show manual upgrade instructions
     - WAIT for user action

   ELSE IF dependency not found:
     - Display error: "✗ {name} not found"
     - BRANCH on user's setup mode:
       - Automated mode: Offer installation menu
       - Guided mode: Show manual installation instructions
       - Check-only mode: Report as missing, continue
     - WAIT for user action (except check-only mode)
</dependency_validation_pattern>
```

#### Improvement 2: Mode handling scattered (Lines 86-99, 149-152, 176-186)
**Issue:** User mode (automated/guided/check-only) is referenced throughout but never centrally defined

**Add at line 101 (after mode selection):**
```markdown
<mode_definitions>
After user selects mode, ALL subsequent dependency validations MUST respect this mode:

MODE: "automated"
  - Attempt installations with user confirmation
  - Fall back to guided instructions if automation fails
  - Present installation menus for missing dependencies
  - Wait for user choice at each menu

MODE: "guided"
  - NEVER attempt automated installation
  - Display manual instructions from references/
  - Wait for user to complete manual steps
  - Verify installation after user confirms completion

MODE: "check-only"
  - NEVER offer installation (automated or guided)
  - Report each dependency status (found/missing/wrong-version)
  - Continue through all dependencies regardless of failures
  - Generate report at end showing what needs attention
</mode_definitions>
```

#### Improvement 3: JUCE validation is most complex but least structured (Lines 293-400)
**Issue:** JUCE validation has multiple nested conditionals (standard paths → custom path → validation → retry) without clear structure

**Restructure lines 293-400:**
```markdown
### 4. JUCE Validation (Complex Multi-Stage)

<juce_validation_sequence>
JUCE validation requires multiple attempts due to custom installation paths.

STAGE 1: Search standard locations
  <action>bash .claude/skills/system-setup/assets/system-check.sh --check-juce</action>

  IF found in standard location:
    - Display: "✓ JUCE {version} found at {path}"
    - VALIDATE version >= 8.0.0
    - IF version valid: Save to config and CONTINUE to pluginval
    - IF version old: GOTO version_too_old_handler (line 578)
    - SKIP stages 2-4

  IF not found in standard locations:
    - CONTINUE to STAGE 2

STAGE 2: Ask about custom location
  Present menu:
  ```
  ✗ JUCE not found in standard locations

  Do you have JUCE 8.0+ installed in a custom location?
  1. Yes, let me provide the path
  2. No, install it for me (automated mode only)
  3. No, show me how to install it manually

  Choose (1-3): _
  ```

  User choice 1: GOTO STAGE 3 (custom path validation)
  User choice 2: GOTO STAGE 4 (automated install) - only if MODE=automated
  User choice 3: Display references/juce-setup-guide.md, WAIT for user

STAGE 3: Validate custom path
  Prompt user: "Enter the full path to your JUCE installation:"
  READ user input → $USER_PROVIDED_PATH

  <validation_command>
  bash .claude/skills/system-setup/assets/system-check.sh --validate-juce-path "$USER_PROVIDED_PATH"
  </validation_command>

  IF valid:
    - Display: "✓ JUCE {version} found at {path}"
    - Save custom path to config
    - CONTINUE to pluginval

  IF invalid:
    Present menu:
    ```
    ✗ Invalid JUCE installation at provided path

    The path must contain JUCE modules (modules/juce_core/juce_core.h).

    Would you like to:
    1. Try a different path (return to STAGE 3)
    2. Install JUCE to default location (GOTO STAGE 4)
    3. Show manual installation instructions

    Choose (1-3): _
    ```

STAGE 4: Automated JUCE installation (MODE=automated only)
  Display: "Installing JUCE 8.0.3 to ~/JUCE..."
  Display: "⏳ Downloading JUCE from juce.com..."

  Execute installation (lines 382-396)

  IF installation succeeds:
    - Display: "✓ JUCE 8.0.3 installed to ~/JUCE"
    - Save to config
    - CONTINUE to pluginval

  IF installation fails:
    - Display error message (line 398)
    - Fall back to guided mode: Display references/juce-setup-guide.md
    - WAIT for user to complete manual installation
    - After user confirms, return to STAGE 1 to re-validate
</juce_validation_sequence>
```

### Verbosity Reductions

#### Reduction 1: Remove redundant bash examples (Lines 154-455)
**Current:** Every dependency validation includes inline bash commands (~100 lines total)
**Optimized:** Extract to references/dependency-validation-flows.md, reference by name

**Token savings:** ~1,000 tokens

#### Reduction 2: Consolidate repetitive menu examples (Lines 176-240)
**Current:** Each dependency shows full menu text with slight variations
**Optimized:** Define menu template once, reference with variables

**BEFORE (repeated 5 times):**
```markdown
✗ Python 3.8+ not found

Would you like me to install Python 3 via Homebrew?
1. Yes, install automatically
2. No, show me manual instructions
3. Skip Python (not recommended)

Choose (1-3): _
```

**AFTER (define once):**
```markdown
<menu_template name="missing_dependency">
✗ {dependency_name} {version_requirement} not found

Would you like me to install {dependency_name} {install_method}?
1. Yes, install automatically
2. No, show me manual instructions
3. Skip {dependency_name} {skip_warning}

Choose (1-3): _
</menu_template>

Usage: Display menu_template.missing_dependency with:
  - Python: {install_method: "via Homebrew", skip_warning: "(not recommended)"}
  - CMake: {install_method: "via Homebrew", skip_warning: "(build will fail)"}
  - JUCE: {install_method: "from juce.com", skip_warning: "(cannot build plugins)"}
```

**Token savings:** ~400 tokens

#### Reduction 3: Shorten "Notes for Claude" section (Lines 667-706)
**Current:** 40 lines of verbose warnings
**Optimized:** Convert to bulleted requirements list

**BEFORE:**
```markdown
**CRITICAL REQUIREMENTS:**

1. **ALWAYS check before installing** - Never install if dependency already exists
2. **ALWAYS validate versions** - Don't assume found dependency meets minimum
3. **ALWAYS test functionality** - Run version check to ensure executable works
...
```

**AFTER:**
```markdown
<critical_requirements>
  <requirement>Check before installing (NEVER install if exists)</requirement>
  <requirement>Validate versions meet minimums (don't assume)</requirement>
  <requirement>Test functionality (not just presence)</requirement>
  <requirement>Get confirmation before automated installation</requirement>
  <requirement>Provide fallback if automation fails</requirement>
  <requirement>Use absolute paths (expand ~, validate existence)</requirement>
  <requirement>Mode-specific behavior (respect user's choice)</requirement>
</critical_requirements>

<anti_patterns>
  <anti_pattern>Installing without checking first</anti_pattern>
  <anti_pattern>Not validating version requirements</anti_pattern>
  <anti_pattern>Proceeding when critical dependencies missing</anti_pattern>
  <anti_pattern>Using relative paths in config</anti_pattern>
  <anti_pattern>Not testing executables actually run</anti_pattern>
  <anti_pattern>Auto-proceeding without user confirmation</anti_pattern>
  <anti_pattern>Using AskUserQuestion tool instead of inline menus</anti_pattern>
</anti_patterns>
```

**Token savings:** ~300 tokens

### Critical Sequence Enforcement

Currently **NONE**. This is the biggest issue.

The entire skill is procedural instructions in natural language. There's no enforcement that:
- Test mode is checked before system interaction
- Dependencies are validated in order
- Checkpoints occur after each step
- Config is saved with required fields
- User mode is respected throughout

**Recommended structure:**
```markdown
<skill_execution_flow>
  <phase name="initialization">
    <critical_sequence>
      <!-- Lines 76-114: Test mode detection, welcome, mode selection -->
    </critical_sequence>
  </phase>

  <phase name="platform_detection">
    <critical_sequence>
      <!-- Lines 106-136: Detect platform, confirm with user -->
    </critical_sequence>
  </phase>

  <phase name="dependency_validation">
    <critical_sequence>
      <!-- Lines 139-455: All five dependencies in order -->
      <enforcement>MUST validate in order: Python → Build Tools → CMake → JUCE → pluginval</enforcement>
      <enforcement>MUST respect user's mode (automated/guided/check-only)</enforcement>
      <enforcement>MUST checkpoint after each dependency</enforcement>
    </critical_sequence>
  </phase>

  <phase name="configuration_persistence">
    <state_requirement>
      <!-- Lines 459-487: Save config, update gitignore -->
    </state_requirement>
  </phase>

  <phase name="completion">
    <checkpoint_protocol>
      <!-- Lines 490-528: System report, final menu -->
    </checkpoint_protocol>
  </phase>
</skill_execution_flow>
```

## Examples of Issues Found

### Example 1: Test mode can be forgotten mid-execution

**BEFORE (Line 158):**
```markdown
**Check command:**
```bash
bash .claude/skills/system-setup/assets/system-check.sh --check-python
```
```

**ISSUE:** No mention of appending test mode. Claude could forget to add `${TEST_MODE:+--test=$TEST_MODE}`.

**AFTER:**
```markdown
<validation>
  <check_command required="true">
    bash .claude/skills/system-setup/assets/system-check.sh --check-python ${TEST_MODE:+--test=$TEST_MODE}
  </check_command>
  <enforcement>MUST append test mode if TEST_MODE variable is set</enforcement>
</validation>
```

### Example 2: Ambiguous checkpoint timing

**BEFORE (Lines 144-152):**
```markdown
2. **If found:**
   - Display version and path
   - Validate it meets minimum requirements
   - Save to config
   - Continue to next dependency
```

**ISSUE:** Does "continue to next dependency" mean checkpoint first, or skip straight to next check?

**AFTER:**
```markdown
<on_success>
  <step>Display success message: "✓ {dependency} {version} found at {path}"</step>
  <step>Save to config: {dependency}_path, {dependency}_version</step>
  <step>
    <checkpoint>
      Present menu:
      1. Continue to next dependency (recommended)
      2. Re-check this dependency
      3. Skip remaining dependencies
      4. Exit setup

      Choose (1-4): _
    </checkpoint>
  </step>
  <step>Execute user choice</step>
</on_success>
```

### Example 3: No enforcement that config has all required fields

**BEFORE (Lines 462-479):**
```bash
cat > .claude/system-config.json <<EOF
{
  "platform": "darwin",
  "platform_version": "14.0",
  ...
}
EOF
```

**ISSUE:** What if a dependency was found but Claude forgot to add its path to config? No validation.

**AFTER:**
```markdown
<state_requirement name="configuration_persistence">
  <generation>
    Generate system-config.json with all fields from validation results.
  </generation>

  <validation_before_write>
    BEFORE writing config file, verify:
    - All "found" dependencies have corresponding path/version fields
    - All paths are absolute (no ~, no relative paths)
    - Platform fields are always present
    - validated_at timestamp is current ISO8601 format
  </validation_before_write>

  <verification_after_write>
    AFTER writing config:
    - Read file back and parse JSON
    - Verify parseable and all expected fields present
    - Display success message with field count
  </verification_after_write>
</state_requirement>
```

### Example 4: Mode-specific behavior not enforced

**BEFORE (Line 150):**
```markdown
- **Automated mode:** Offer to install automatically
```

**ISSUE:** Nothing prevents Claude from offering automated install in guided mode.

**AFTER:**
```markdown
<decision_gate condition="dependency_not_found">
  <mode_check>
    IF current MODE is "automated":
      <action>Present installation menu with automated option</action>
    ELSE IF current MODE is "guided":
      <action>Display manual instructions from references/, NEVER offer automated install</action>
    ELSE IF current MODE is "check-only":
      <action>Report as missing, continue to next dependency without prompting</action>
  </mode_check>
</decision_gate>
```

### Example 5: Unclear when to fall back from automated to guided

**BEFORE (Lines 536-553):**
```markdown
### Automated Installation Failures

If any automated installation fails:

1. **Capture error output**
2. **Display error message**
3. **Fall back to guided mode**
```

**ISSUE:** Does "fall back to guided mode" mean for just this dependency, or for all remaining dependencies?

**AFTER:**
```markdown
<error_handling name="automated_installation_failure">
  <trigger>Automated installation command exits with non-zero status</trigger>

  <recovery_sequence>
    <step>Capture stderr output from failed command</step>
    <step>Display error message:
      "✗ Failed to install {dependency} via {method}"
      "Error: {error_output}"
      "Falling back to manual instructions..."
    </step>
    <step>Display manual instructions from references/platform-requirements.md</step>
    <step>Present menu:
      1. I've installed it manually, re-check now
      2. Skip this dependency (continue with others)
      3. Exit setup

      Choose (1-3): _
    </step>
  </recovery_sequence>

  <scope>Fallback applies ONLY to current dependency, not all remaining</scope>
  <enforcement>After recovery, MODE remains "automated" for subsequent dependencies</enforcement>
</error_handling>
```

## Estimated Impact

### Context reduction
- **Current size:** ~17,000 tokens (SKILL.md + references already extracted)
- **After optimization:** ~12,800 tokens
- **Savings:** 4,200 tokens (25% reduction)

### Breakdown:
- Extract dependency flows to references: -3,000 tokens
- Extract error patterns: -700 tokens
- Consolidate menu templates: -400 tokens
- Shorten notes section: -300 tokens
- Add XML enforcement: +200 tokens (but worth it for clarity)
- **Net savings:** 4,200 tokens

### Comprehension improvement
**High** - The skill has good structure but lacks enforcement mechanisms. Adding XML tags will:
- Eliminate ambiguity about execution order
- Prevent Claude from skipping steps
- Make mode-specific behavior explicit
- Enforce checkpoint protocol consistently
- Validate configuration completeness

The repetitive dependency validation flows are the main comprehension blocker - extracting them will make the core logic much clearer.

### Maintenance improvement
**High** - Currently, changing validation behavior requires editing 5 different sections (one per dependency). After extraction:
- Edit pattern once in references/dependency-validation-flows.md
- All dependencies inherit the change
- XML enforcement makes invariants explicit and testable
- Future debugging becomes: "Check which critical_sequence step failed"

## Priority Recommendations

### 1. Add critical sequence enforcement for dependency validation workflow
**Why:** This is the core of the skill (80% of lines), and without enforcement, Claude can deviate.
**Impact:** Prevents skipped checks, ensures checkpoint consistency, respects user mode
**Effort:** 2-3 hours to wrap lines 139-455 in proper XML structure
**Files to create:** None (in-place modification of SKILL.md)

**Specific actions:**
- Wrap lines 76-114 (entry point) in `<critical_sequence name="initialization">`
- Wrap lines 106-136 (platform detection) in `<critical_sequence name="platform_detection">`
- Wrap lines 139-455 (all dependencies) in `<critical_sequence name="dependency_validation">` with:
  - Each dependency as `<step>` with number, required flag, success_criteria
  - `<decision_gate>` for found vs not-found branching
  - `<mode_check>` for automated/guided/check-only branching
  - `<checkpoint>` tags at proper menu moments
- Wrap lines 459-487 (config save) in `<state_requirement name="configuration_persistence">`

### 2. Extract repetitive dependency validation flows to references/
**Why:** 5 dependencies × 60 lines each = 300 lines of repetitive patterns
**Impact:** Saves ~3,000 tokens, makes core logic visible
**Effort:** 1-2 hours to extract and refactor
**Files to create:**
  - `references/dependency-validation-flows.md` (new)

**Specific actions:**
- Create template structure for dependency validation pattern
- Extract Python validation (lines 154-203) as example
- Extract Build Tools validation (lines 205-250)
- Extract CMake validation (lines 252-292)
- Extract JUCE validation (lines 293-400) - most complex
- Extract pluginval validation (lines 402-455)
- Replace in SKILL.md with: "For each dependency, follow pattern in references/dependency-validation-flows.md section X"

### 3. Restructure JUCE validation into explicit stages
**Why:** Most complex validation with nested conditionals - currently hardest to follow
**Impact:** Prevents infinite loops, clarifies retry logic, makes custom path handling explicit
**Effort:** 1 hour to restructure lines 293-400
**Files to modify:** SKILL.md lines 293-400 in-place

**Specific actions:**
- Break JUCE validation into 4 labeled stages (as shown in Example 3 above)
- Add explicit GOTO instructions for stage transitions
- Make retry conditions explicit (return to which stage?)
- Add enforcement: "MUST complete all stages OR user explicitly skips"

## Implementation Notes

### Files that would be created in references/
1. `references/dependency-validation-flows.md` - Extracted validation patterns for all 5 dependencies
2. `references/error-handling-patterns.md` - Consolidated error handling with examples
3. `references/implementation-notes.md` - Success criteria and common pitfalls (extracted from current lines 653-706)

### Files that would be created in assets/
None - system-check.sh already exists and is appropriately placed.

### Changes to SKILL.md structure
**Phase 1: Critical sequences (priority 1)**
- Lines 76-114: Wrap in `<critical_sequence name="initialization">`
- Lines 106-136: Wrap in `<critical_sequence name="platform_detection">`
- Lines 139-455: Wrap in `<critical_sequence name="dependency_validation">`
- Lines 459-487: Wrap in `<state_requirement name="configuration_persistence">`
- Lines 610-627: Wrap in `<checkpoint_protocol>`

**Phase 2: Content extraction (priority 2)**
- Lines 154-455: Extract to references/dependency-validation-flows.md, replace with references
- Lines 534-606: Extract to references/error-handling-patterns.md, replace with reference
- Lines 653-706: Extract to references/implementation-notes.md, replace with reference

**Phase 3: Clarity improvements (priority 3)**
- Lines 144-152: Replace with explicit conditional structure
- Lines 293-400: Restructure JUCE validation into labeled stages
- Throughout: Replace ambiguous "it", "this", "the dependency" with explicit names

### Estimated time to implement all fixes
**Total: 6-8 hours**
- Priority 1 (critical sequences): 2-3 hours
- Priority 2 (content extraction): 2-3 hours
- Priority 3 (clarity improvements): 2 hours

### Testing recommendations
After implementing fixes:
1. Test with existing test-setup-skill.sh against all scenarios
2. Verify TEST_MODE properly propagates to all system-check.sh calls
3. Confirm checkpoint menus appear at correct moments
4. Validate config file has all required fields
5. Test mode-specific behavior (automated vs guided vs check-only)
6. Verify JUCE custom path validation with retry scenarios

### Risk assessment
**Low risk** - Changes are structural improvements that make existing logic more explicit:
- No behavior changes, only enforcement of existing requirements
- XML tags clarify intent without changing actions
- Content extraction moves text without altering meaning
- All changes are backward compatible with existing system-check.sh

**Validation:** Compare current behavior (run /setup in each test scenario) with post-fix behavior - should be identical except clearer checkpoints and better error recovery.
