# Fix Plan: system-setup

## Summary
- Critical fixes: **15**
- Extraction operations: **3**
- Total estimated changes: **18**
- Estimated time: **6-8 hours**

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Enforce test mode detection at skill entry
**Location:** Lines 76-114
**Operation:** WRAP
**Before:**
```markdown
When invoked via `/setup` command:

**Check for test mode first:**
- If user provided `--test=SCENARIO` argument, set TEST_MODE variable
- Pass test mode to all system-check.sh invocations via `--test=$SCENARIO`

**Display welcome message:**
```
Plugin Freedom System - Setup & Validation

This skill validates all dependencies required for plugin development:
- Python 3.8+ (for build scripts)
- Build tools (Xcode on macOS, MSVC on Windows, GCC on Linux)
- CMake 3.15+ (for JUCE project generation)
- JUCE 8.0+ (framework for plugin development)
- pluginval (optional validation tool)
```

**Prompt user to select mode:**

```
Setup mode:
1. Automated - Install missing dependencies automatically (recommended)
2. Guided - Show manual installation instructions
3. Check only - Report status without installing
4. Exit

Choose (1-4): _
```

**Wait for user choice and store in MODE variable.**
```

**After:**
```markdown
<critical_sequence name="skill_entry" enforcement="blocking">
  <step number="1" required="true">
    <description>Check for test mode argument</description>
    <condition>User provided --test=SCENARIO argument</condition>
    <action>Set TEST_MODE variable</action>
    <enforcement>MUST check before any system interaction</enforcement>
  </step>

  <step number="2" required="true">
    <description>Display welcome message</description>
    <message>
Plugin Freedom System - Setup & Validation

This skill validates all dependencies required for plugin development:
- Python 3.8+ (for build scripts)
- Build tools (Xcode on macOS, MSVC on Windows, GCC on Linux)
- CMake 3.15+ (for JUCE project generation)
- JUCE 8.0+ (framework for plugin development)
- pluginval (optional validation tool)
    </message>
  </step>

  <step number="3" required="true">
    <description>Prompt user to select mode</description>
    <menu>
Setup mode:
1. Automated - Install missing dependencies automatically (recommended)
2. Guided - Show manual installation instructions
3. Check only - Report status without installing
4. Exit

Choose (1-4): _
    </menu>
    <checkpoint>Wait for user choice (1-4)</checkpoint>
  </step>

  <step number="4" required="true">
    <description>Store user mode choice</description>
    <action>Save to MODE variable for use in validation workflow</action>
    <validation>MODE must be one of: automated, guided, check-only</validation>
  </step>
</critical_sequence>
```
**Verification:** Check that XML tags `<critical_sequence name="skill_entry">` are present at line 76 and closing tag at line 114

### Fix 1.2: Add mode definitions immediately after mode selection
**Location:** Line 101 (after mode selection)
**Operation:** INSERT
**Change:**
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
**Verification:** Mode definitions present immediately after mode selection, before platform detection

### Fix 1.3: Enforce platform detection sequence
**Location:** Lines 106-136
**Operation:** WRAP
**Before:**
```markdown
## Platform Detection

**Detect operating system and architecture:**

```bash
bash .claude/skills/system-setup/assets/system-check.sh --detect-platform
```

**Parse result and display confirmation:**

```
Detected platform: macOS (darwin) arm64
Platform version: 14.0
```

**Present checkpoint menu:**

```
Platform detected. What's next?
1. Begin dependency validation (recommended)
2. Exit setup

Choose (1-2): _
```

**Wait for user response.**
```

**After:**
```markdown
<critical_sequence name="platform_detection" enforcement="blocking">
  <step number="1" required="true">
    <description>Detect operating system and architecture</description>
    <command>bash .claude/skills/system-setup/assets/system-check.sh --detect-platform ${TEST_MODE:+--test=$TEST_MODE}</command>
    <enforcement>MUST append test mode if TEST_MODE variable is set</enforcement>
  </step>

  <step number="2" required="true">
    <description>Parse result and display confirmation</description>
    <display_format>
Detected platform: {os_name} ({os_type}) {arch}
Platform version: {version}
    </display_format>
  </step>

  <step number="3" required="true">
    <description>Present checkpoint menu</description>
    <checkpoint>
Platform detected. What's next?
1. Begin dependency validation (recommended)
2. Exit setup

Choose (1-2): _
    </checkpoint>
    <enforcement>WAIT for user response (NEVER auto-proceed)</enforcement>
  </step>
</critical_sequence>
```
**Verification:** Confirm XML tags present at lines 106 and 136

### Fix 1.4: Enforce dependency validation sequence
**Location:** Lines 139-455
**Operation:** WRAP
**Before:**
```markdown
## Dependency Validation Workflow

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
   - **Check-only mode:** Report as missing, continue

### 1. Python Validation
[... current content ...]
```

**After:**
```markdown
<critical_sequence name="dependency_validation" enforcement="blocking">
  <sequence_requirement>
    Dependencies MUST be validated in this exact order.
    NEVER skip to later dependencies if earlier ones fail in check-only mode.
  </sequence_requirement>

  <dependency_validation_pattern>
For each dependency, execute this validation pattern:

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

  <step number="1" dependency="python" required="true">
    <validation>
      <check_command>bash .claude/skills/system-setup/assets/system-check.sh --check-python ${TEST_MODE:+--test=$TEST_MODE}</check_command>
      <success_criteria>found=true AND version>=3.8</success_criteria>
      <enforcement>MUST append test mode if TEST_MODE variable is set</enforcement>
    </validation>

    <decision_gate condition="not_found OR version_too_old">
      <mode name="automated">
        <action>Present installation menu (see references/dependency-validation-flows.md#python-automated)</action>
        <checkpoint>Wait for user choice</checkpoint>
      </mode>
      <mode name="guided">
        <action>Display manual instructions from references/platform-requirements.md#python</action>
        <checkpoint>Wait for user confirmation</checkpoint>
      </mode>
      <mode name="check-only">
        <action>Report as missing, continue to next dependency</action>
      </mode>
    </decision_gate>

    <on_success>
      <action>Save to config: python_path, python_version</action>
      <action>Display: ✓ Python {version} found at {path}</action>
      <checkpoint>Present menu: 1. Continue to next dependency (recommended) / 2. Re-check / 3. Exit</checkpoint>
      <action>Continue to step 2</action>
    </on_success>
  </step>

  <step number="2" dependency="build_tools" required="true">
    <validation>
      <check_command>bash .claude/skills/system-setup/assets/system-check.sh --check-build-tools ${TEST_MODE:+--test=$TEST_MODE}</check_command>
      <success_criteria>found=true</success_criteria>
      <platform_specific>
        <macos>Xcode Command Line Tools</macos>
        <windows>MSVC Build Tools</windows>
        <linux>GCC/G++ toolchain</linux>
      </platform_specific>
      <enforcement>MUST append test mode if TEST_MODE variable is set</enforcement>
    </validation>

    <decision_gate condition="not_found">
      <mode name="automated">
        <action>Present installation menu (see references/dependency-validation-flows.md#build-tools-automated)</action>
        <checkpoint>Wait for user choice</checkpoint>
      </mode>
      <mode name="guided">
        <action>Display manual instructions from references/platform-requirements.md#build-tools</action>
        <checkpoint>Wait for user confirmation</checkpoint>
      </mode>
      <mode name="check-only">
        <action>Report as missing, continue to next dependency</action>
      </mode>
    </decision_gate>

    <on_success>
      <action>Save to config: build_tools_path, build_tools_type</action>
      <action>Display: ✓ {build_tools_type} found at {path}</action>
      <checkpoint>Present menu: 1. Continue to next dependency (recommended) / 2. Re-check / 3. Exit</checkpoint>
      <action>Continue to step 3</action>
    </on_success>
  </step>

  <step number="3" dependency="cmake" required="true">
    <validation>
      <check_command>bash .claude/skills/system-setup/assets/system-check.sh --check-cmake ${TEST_MODE:+--test=$TEST_MODE}</check_command>
      <success_criteria>found=true AND version>=3.15</success_criteria>
      <enforcement>MUST append test mode if TEST_MODE variable is set</enforcement>
    </validation>

    <decision_gate condition="not_found OR version_too_old">
      <mode name="automated">
        <action>Present installation menu (see references/dependency-validation-flows.md#cmake-automated)</action>
        <checkpoint>Wait for user choice</checkpoint>
      </mode>
      <mode name="guided">
        <action>Display manual instructions from references/platform-requirements.md#cmake</action>
        <checkpoint>Wait for user confirmation</checkpoint>
      </mode>
      <mode name="check-only">
        <action>Report as missing, continue to next dependency</action>
      </mode>
    </decision_gate>

    <on_success>
      <action>Save to config: cmake_path, cmake_version</action>
      <action>Display: ✓ CMake {version} found at {path}</action>
      <checkpoint>Present menu: 1. Continue to next dependency (recommended) / 2. Re-check / 3. Exit</checkpoint>
      <action>Continue to step 4</action>
    </on_success>
  </step>

  <step number="4" dependency="juce" required="true">
    <validation>
      <check_command>bash .claude/skills/system-setup/assets/system-check.sh --check-juce ${TEST_MODE:+--test=$TEST_MODE}</check_command>
      <success_criteria>found=true AND version>=8.0.0</success_criteria>
      <enforcement>MUST append test mode if TEST_MODE variable is set</enforcement>
    </validation>

    <juce_validation_sequence>
JUCE validation requires multiple attempts due to custom installation paths.

STAGE 1: Search standard locations
  <action>bash .claude/skills/system-setup/assets/system-check.sh --check-juce ${TEST_MODE:+--test=$TEST_MODE}</action>

  IF found in standard location:
    - Display: "✓ JUCE {version} found at {path}"
    - VALIDATE version >= 8.0.0
    - IF version valid: Save to config and CONTINUE to pluginval
    - IF version old: GOTO version_too_old_handler
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
  bash .claude/skills/system-setup/assets/system-check.sh --validate-juce-path "$USER_PROVIDED_PATH" ${TEST_MODE:+--test=$TEST_MODE}
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

  Execute installation (see references/dependency-validation-flows.md#juce-automated-install)

  IF installation succeeds:
    - Display: "✓ JUCE 8.0.3 installed to ~/JUCE"
    - Save to config
    - CONTINUE to pluginval

  IF installation fails:
    - Display error message
    - Fall back to guided mode: Display references/juce-setup-guide.md
    - WAIT for user to complete manual installation
    - After user confirms, return to STAGE 1 to re-validate
    </juce_validation_sequence>

    <on_success>
      <action>Save to config: juce_path, juce_version</action>
      <action>Display: ✓ JUCE {version} found at {path}</action>
      <checkpoint>Present menu: 1. Continue to next dependency (recommended) / 2. Re-check / 3. Exit</checkpoint>
      <action>Continue to step 5</action>
    </on_success>
  </step>

  <step number="5" dependency="pluginval" required="false">
    <validation>
      <check_command>bash .claude/skills/system-setup/assets/system-check.sh --check-pluginval ${TEST_MODE:+--test=$TEST_MODE}</check_command>
      <success_criteria>found=true</success_criteria>
      <enforcement>MUST append test mode if TEST_MODE variable is set</enforcement>
      <note>pluginval is optional but recommended for validation</note>
    </validation>

    <decision_gate condition="not_found">
      <mode name="automated">
        <action>Present installation menu (see references/dependency-validation-flows.md#pluginval-automated)</action>
        <checkpoint>Wait for user choice</checkpoint>
      </mode>
      <mode name="guided">
        <action>Display manual instructions from references/platform-requirements.md#pluginval</action>
        <checkpoint>Wait for user confirmation</checkpoint>
      </mode>
      <mode name="check-only">
        <action>Report as missing, continue to configuration</action>
      </mode>
    </decision_gate>

    <on_success>
      <action>Save to config: pluginval_path, pluginval_version</action>
      <action>Display: ✓ pluginval {version} found at {path}</action>
      <checkpoint>Present menu: 1. Save configuration (recommended) / 2. Re-check / 3. Exit</checkpoint>
    </on_success>

    <on_skip>
      <action>Display: ℹ pluginval not installed (optional)</action>
      <action>Continue to configuration persistence</action>
    </on_skip>
  </step>
</critical_sequence>
```
**Verification:** Confirm XML tags present at line 139 and closing tag at line 455, with all 5 dependencies wrapped in `<step>` tags

### Fix 1.5: Enforce configuration persistence requirements
**Location:** Lines 459-487
**Operation:** WRAP
**Before:**
```markdown
## Configuration Persistence

After all dependencies are validated, create `.claude/system-config.json`:

```bash
# Generate config file
cat > .claude/system-config.json <<EOF
{
  "platform": "darwin",
  "platform_version": "14.0",
  "arch": "arm64",
  "python_path": "/usr/local/bin/python3",
  "python_version": "3.11.5",
  "xcode_path": "/Applications/Xcode.app",
  "cmake_path": "/usr/local/bin/cmake",
  "cmake_version": "3.27.0",
  "juce_path": "/Users/username/JUCE",
  "juce_version": "8.0.3",
  "pluginval_path": "/usr/local/bin/pluginval",
  "pluginval_version": "1.0.3",
  "validated_at": "2025-01-15T10:30:00Z"
}
EOF
```

**Add to .gitignore:**

```bash
echo ".claude/system-config.json" >> .gitignore
```
```

**After:**
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

  <validation_before_write>
    BEFORE writing config file, verify:
    - All "found" dependencies have corresponding path/version fields
    - All paths are absolute (no ~, no relative paths - expand them)
    - Platform fields are always present
    - validated_at timestamp is current ISO8601 format
  </validation_before_write>

  <generation>
Generate config file:

```bash
# Generate config file
cat > .claude/system-config.json <<EOF
{
  "platform": "${DETECTED_PLATFORM}",
  "platform_version": "${PLATFORM_VERSION}",
  "arch": "${ARCH}",
  "python_path": "${PYTHON_PATH}",
  "python_version": "${PYTHON_VERSION}",
  "xcode_path": "${XCODE_PATH}",
  "cmake_path": "${CMAKE_PATH}",
  "cmake_version": "${CMAKE_VERSION}",
  "juce_path": "${JUCE_PATH}",
  "juce_version": "${JUCE_VERSION}",
  "pluginval_path": "${PLUGINVAL_PATH}",
  "pluginval_version": "${PLUGINVAL_VERSION}",
  "validated_at": "$(date -u +%Y-%m-%dT%H:%M:%SZ)"
}
EOF
```
  </generation>

  <verification_after_write>
    AFTER writing config:
    - Read file back and parse JSON
    - Verify parseable and all expected fields present
    - Display success message with field count
    - Example: "✓ Configuration saved to .claude/system-config.json (12 fields)"
  </verification_after_write>

  <gitignore_requirement>
    MUST add .claude/system-config.json to .gitignore if not present:

```bash
if ! grep -q ".claude/system-config.json" .gitignore 2>/dev/null; then
  echo ".claude/system-config.json" >> .gitignore
fi
```
  </gitignore_requirement>
</state_requirement>
```
**Verification:** Confirm XML tags present at line 459 and closing tag at line 487

### Fix 1.6: Enforce checkpoint protocol
**Location:** Lines 610-627
**Operation:** WRAP
**Before:**
```markdown
## Checkpoint Protocol Integration

This skill follows the checkpoint protocol:

**After each major validation step:**
1. Display what was found/installed
2. Present numbered decision menu
3. Wait for user response
4. Execute chosen action

**Key checkpoint moments:**
- After platform detection
- After each dependency validation (Python, Build Tools, CMake, JUCE, pluginval)
- After configuration saved
- At final system report
```

**After:**
```markdown
<checkpoint_protocol enforcement="mandatory">
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
    <moment>After Python validation</moment>
    <moment>After Build Tools validation</moment>
    <moment>After CMake validation</moment>
    <moment>After JUCE validation</moment>
    <moment>After pluginval validation (or skip)</moment>
    <moment>After configuration saved (line 461-486)</moment>
    <moment>At final system report (line 492-528)</moment>
  </checkpoint_moments>

  <anti_pattern>
    NEVER use AskUserQuestion tool - ALWAYS use inline numbered menus as shown in examples.
  </anti_pattern>
</checkpoint_protocol>
```
**Verification:** Confirm XML tags present at line 610 and closing tag at line 627

### Fix 1.7: Replace ambiguous pronouns in nested conditionals
**Location:** Lines 144-152
**Operation:** REPLACE (already covered in Fix 1.4 with dependency_validation_pattern)
**Verification:** Check that dependency_validation_pattern explicitly names the dependency in all branches

### Fix 1.8: Add critical requirements section
**Location:** Lines 667-706
**Operation:** REPLACE
**Before:**
```markdown
**CRITICAL REQUIREMENTS:**

1. **ALWAYS check before installing** - Never install if dependency already exists
2. **ALWAYS validate versions** - Don't assume found dependency meets minimum
3. **ALWAYS test functionality** - Run version check to ensure executable works
4. **ALWAYS get confirmation** - Never install without user approval
5. **ALWAYS provide fallback** - If automation fails, fall back to guided mode
6. **ALWAYS use absolute paths** - Expand ~ and validate paths exist
7. **ALWAYS respect mode** - Honor user's choice (automated/guided/check-only)
```

**After:**
```markdown
<critical_requirements>
  <requirement>Check before installing (NEVER install if exists)</requirement>
  <requirement>Validate versions meet minimums (don't assume)</requirement>
  <requirement>Test functionality (not just presence)</requirement>
  <requirement>Get confirmation before automated installation</requirement>
  <requirement>Provide fallback if automation fails</requirement>
  <requirement>Use absolute paths (expand ~, validate existence)</requirement>
  <requirement>Mode-specific behavior (respect user's choice)</requirement>
  <requirement>Append test mode to all system-check.sh calls if TEST_MODE set</requirement>
</critical_requirements>

<anti_patterns>
  <anti_pattern>Installing without checking first</anti_pattern>
  <anti_pattern>Not validating version requirements</anti_pattern>
  <anti_pattern>Proceeding when critical dependencies missing</anti_pattern>
  <anti_pattern>Using relative paths in config</anti_pattern>
  <anti_pattern>Not testing executables actually run</anti_pattern>
  <anti_pattern>Auto-proceeding without user confirmation</anti_pattern>
  <anti_pattern>Using AskUserQuestion tool instead of inline menus</anti_pattern>
  <anti_pattern>Forgetting to append test mode to system-check.sh calls</anti_pattern>
  <anti_pattern>Not respecting MODE throughout execution</anti_pattern>
</anti_patterns>
```
**Verification:** Confirm critical_requirements and anti_patterns sections replace verbose notes

### Fix 1.9: Enforce test mode propagation in all check commands
**Location:** Lines 158, 213, 260, 301, 410 (and all other system-check.sh calls)
**Operation:** REPLACE (already covered in Fix 1.4 within each step's validation)
**Verification:** Every system-check.sh command includes `${TEST_MODE:+--test=$TEST_MODE}`

### Fix 1.10: Add error handling enforcement
**Location:** Lines 534-606
**Operation:** WRAP
**Before:**
```markdown
## Error Handling

### Automated Installation Failures

If any automated installation fails:

1. **Capture error output**
2. **Display error message**
3. **Fall back to guided mode**
4. **Prompt user to try manual installation**
5. **After user confirms manual completion, re-check dependency**
```

**After:**
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

<error_handling name="version_too_old">
  <trigger>Dependency found but version < minimum required</trigger>

  <recovery_sequence>
    <step>Display warning:
      "✗ {dependency} {found_version} found, but {minimum_version} required"
    </step>
    <step>BRANCH on MODE:
      - Automated: Offer upgrade menu (install newer version)
      - Guided: Display upgrade instructions from references/
      - Check-only: Report issue, continue
    </step>
  </recovery_sequence>
</error_handling>

<error_handling name="custom_path_validation_failure">
  <trigger>User provides JUCE path that doesn't contain valid installation</trigger>

  <recovery_sequence>
    <step>Display error:
      "✗ Invalid JUCE installation at provided path"
      "The path must contain JUCE modules (modules/juce_core/juce_core.h)."
    </step>
    <step>Present retry menu (return to JUCE STAGE 3 or STAGE 4)</step>
  </recovery_sequence>
</error_handling>
```
**Verification:** Confirm error_handling tags with specific triggers and recovery sequences

### Fix 1.11: Add menu template to reduce repetition
**Location:** Line 170 (before first dependency validation)
**Operation:** INSERT
**Change:**
```markdown
<menu_templates>
  <template name="missing_dependency">
✗ {dependency_name} {version_requirement} not found

Would you like me to install {dependency_name} {install_method}?
1. Yes, install automatically
2. No, show me manual instructions
3. Skip {dependency_name} {skip_warning}

Choose (1-3): _
  </template>

  <template name="version_too_old">
✗ {dependency_name} {found_version} found, but {minimum_version} required

Would you like me to upgrade {dependency_name} {install_method}?
1. Yes, upgrade automatically
2. No, show me manual upgrade instructions
3. Skip {dependency_name} {skip_warning}

Choose (1-3): _
  </template>

  <template name="checkpoint_continue">
✓ {dependency_name} {version} found at {path}

What's next?
1. Continue to next dependency (recommended)
2. Re-check this dependency
3. Skip remaining dependencies
4. Exit setup

Choose (1-4): _
  </template>
</menu_templates>

<template_usage>
Use menu templates with these variables:

Python:
  - dependency_name: "Python"
  - version_requirement: "3.8+"
  - install_method: "via Homebrew"
  - skip_warning: "(not recommended - build scripts will fail)"

Build Tools:
  - dependency_name: "Xcode Command Line Tools" (macOS)
  - version_requirement: ""
  - install_method: "via xcode-select --install"
  - skip_warning: "(cannot build plugins)"

CMake:
  - dependency_name: "CMake"
  - version_requirement: "3.15+"
  - install_method: "via Homebrew"
  - skip_warning: "(build will fail)"

JUCE:
  - dependency_name: "JUCE"
  - version_requirement: "8.0+"
  - install_method: "from juce.com"
  - skip_warning: "(cannot build plugins)"

pluginval:
  - dependency_name: "pluginval"
  - version_requirement: ""
  - install_method: "from github.com/Tracktion/pluginval"
  - skip_warning: "(optional - validation tests won't run)"
</template_usage>
```
**Verification:** Menu template section present before first dependency validation

### Fix 1.12: Make final system report a checkpoint
**Location:** Lines 492-528
**Operation:** WRAP
**Before:**
```markdown
## Final System Report

After all validations complete:

**Display summary:**

```
Setup Complete!

Platform: macOS (darwin) arm64
Dependencies validated:
  ✓ Python 3.11.5
  ✓ Xcode Command Line Tools
  ✓ CMake 3.27.0
  ✓ JUCE 8.0.3
  ✓ pluginval 1.0.3

Configuration saved to .claude/system-config.json
```

**Present final menu:**

```
Setup complete. What's next?
1. Start new plugin project (/dream)
2. Resume existing plugin (/continue)
3. Exit

Choose (1-3): _
```
```

**After:**
```markdown
<checkpoint_protocol name="final_system_report">
  <trigger>After all validations complete</trigger>

  <display_summary>
Setup Complete!

Platform: {platform_name} ({platform_type}) {arch}
Dependencies validated:
  {dependency_results}

Configuration saved to .claude/system-config.json
  </display_summary>

  <dependency_results_format>
For each validated dependency:
  IF found: ✓ {name} {version}
  IF missing: ✗ {name} (not installed)
  IF skipped: - {name} (skipped)
  </dependency_results_format>

  <final_checkpoint>
Setup complete. What's next?
1. Start new plugin project (/dream)
2. Resume existing plugin (/continue)
3. Re-run setup to update configuration
4. Exit

Choose (1-4): _
  </final_checkpoint>

  <enforcement>WAIT for user response before completing skill</enforcement>
</checkpoint_protocol>
```
**Verification:** Confirm checkpoint_protocol tags at lines 492-528

### Fix 1.13: Add state file updates
**Location:** Line 530 (after final menu)
**Operation:** INSERT
**Change:**
```markdown
<state_updates>
After setup completes successfully:

1. NO state file updates needed (system-setup is standalone)
2. DO NOT update PLUGINS.md (no plugin created)
3. DO NOT update .continue-here.md (not part of workflow)
4. Configuration is stored in .claude/system-config.json only
</state_updates>
```
**Verification:** State update clarification present after final checkpoint

### Fix 1.14: Add delegation rules
**Location:** Line 60 (after description, before When to use)
**Operation:** INSERT
**Change:**
```markdown
<delegation_rules>
  <rule>This skill is STANDALONE - it does NOT delegate to other skills or subagents</rule>
  <rule>All validation logic is handled by system-check.sh bash script</rule>
  <rule>This skill is invoked BEFORE plugin workflows begin</rule>
  <rule>DO NOT invoke plugin-workflow, plugin-planning, or any other plugin skills from here</rule>
</delegation_rules>
```
**Verification:** Delegation rules present near top of skill, before "When to use"

### Fix 1.15: Add skill completion criteria
**Location:** Line 706 (at end of SKILL.md)
**Operation:** INSERT
**Change:**
```markdown
<completion_criteria>
This skill is complete when:

1. ✓ Platform detected and confirmed by user
2. ✓ All 5 dependencies validated (or user explicitly skipped)
3. ✓ Configuration saved to .claude/system-config.json
4. ✓ Configuration file added to .gitignore
5. ✓ Final system report displayed
6. ✓ User selected action from final menu (exit, start new plugin, resume existing)

DO NOT auto-proceed to next action after final menu - user decides what's next.
</completion_criteria>
```
**Verification:** Completion criteria section present at end of SKILL.md

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Extract dependency validation flows to references/
**Location:** Lines 154-455
**Operation:** EXTRACT
**Create file:** `references/dependency-validation-flows.md`
**File content:**
```markdown
# Dependency Validation Flows

This document contains detailed validation workflows for each dependency. The main SKILL.md references these patterns via the `<critical_sequence name="dependency_validation">` structure.

## Python Validation {#python-automated}

### Automated Mode - Installation Menu

When Python not found and MODE=automated:

```
✗ Python 3.8+ not found

Would you like me to install Python 3 via Homebrew?
1. Yes, install automatically
2. No, show me manual instructions
3. Skip Python (not recommended - build scripts will fail)

Choose (1-3): _
```

**User choice 1: Install automatically**

```bash
# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
  echo "Homebrew not found. Installing Homebrew first..."
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

# Install Python via Homebrew
brew install python3

# Verify installation
python3 --version
```

**User choice 2: Manual instructions**

Display from references/platform-requirements.md#python section.

**User choice 3: Skip**

Report Python as missing, continue to Build Tools validation.

### Guided Mode - Manual Instructions

Display:

```
Python 3.8+ Installation Instructions

macOS:
  Via Homebrew:
    brew install python3

  Via python.org:
    1. Download installer from https://www.python.org/downloads/
    2. Run the .pkg installer
    3. Verify: python3 --version

Linux (Ubuntu/Debian):
  sudo apt update
  sudo apt install python3 python3-pip

Linux (Fedora/RHEL):
  sudo dnf install python3 python3-pip

Windows:
  1. Download installer from https://www.python.org/downloads/
  2. Run the installer
  3. IMPORTANT: Check "Add Python to PATH"
  4. Verify: python --version

After installing manually:
1. I've installed it manually, re-check now
2. Exit

Choose (1-2): _
```

## Build Tools Validation {#build-tools-automated}

### Automated Mode - Installation Menu

When Build Tools not found and MODE=automated:

**macOS:**

```
✗ Xcode Command Line Tools not found

Would you like me to install Xcode Command Line Tools?
1. Yes, install automatically
2. No, show me manual instructions
3. Skip (cannot build plugins)

Choose (1-3): _
```

**User choice 1:**

```bash
xcode-select --install
```

Wait for installation to complete, then verify:

```bash
xcode-select -p
```

**Linux:**

```
✗ GCC/G++ not found

Would you like me to install build tools via package manager?
1. Yes, install automatically
2. No, show me manual instructions
3. Skip (cannot build plugins)

Choose (1-3): _
```

**User choice 1:**

Detect distro and install:

```bash
# Detect distro
if [ -f /etc/debian_version ]; then
  sudo apt update
  sudo apt install build-essential
elif [ -f /etc/redhat-release ]; then
  sudo dnf groupinstall "Development Tools"
fi
```

**Windows:**

Manual instructions only (cannot automate MSVC installation).

## CMake Validation {#cmake-automated}

### Automated Mode - Installation Menu

When CMake not found and MODE=automated:

```
✗ CMake 3.15+ not found

Would you like me to install CMake via Homebrew?
1. Yes, install automatically
2. No, show me manual instructions
3. Skip (build will fail)

Choose (1-3): _
```

**User choice 1:**

```bash
# macOS
brew install cmake

# Linux (if snap available)
sudo snap install cmake --classic

# Linux (Ubuntu/Debian)
sudo apt update
sudo apt install cmake

# Verify
cmake --version
```

## JUCE Validation {#juce-automated-install}

### Automated Mode - JUCE Installation (STAGE 4)

When JUCE not found and user chooses automated install:

```bash
# Set installation directory
JUCE_DIR="${HOME}/JUCE"

# Display progress
echo "Installing JUCE 8.0.3 to ${JUCE_DIR}..."
echo "⏳ Downloading JUCE from juce.com..."

# Download JUCE 8.0.3
JUCE_VERSION="8.0.3"
JUCE_URL="https://github.com/juce-framework/JUCE/releases/download/${JUCE_VERSION}/juce-${JUCE_VERSION}-linux.zip"

# Determine OS-specific URL
case "$(uname -s)" in
  Darwin*)
    JUCE_URL="https://github.com/juce-framework/JUCE/releases/download/${JUCE_VERSION}/juce-${JUCE_VERSION}-osx.zip"
    ;;
  Linux*)
    JUCE_URL="https://github.com/juce-framework/JUCE/releases/download/${JUCE_VERSION}/juce-${JUCE_VERSION}-linux.zip"
    ;;
  MINGW*|MSYS*|CYGWIN*)
    JUCE_URL="https://github.com/juce-framework/JUCE/releases/download/${JUCE_VERSION}/juce-${JUCE_VERSION}-windows.zip"
    ;;
esac

# Download and extract
curl -L -o /tmp/juce.zip "$JUCE_URL"
unzip -q /tmp/juce.zip -d /tmp/
mv /tmp/JUCE "${JUCE_DIR}"
rm /tmp/juce.zip

# Verify installation
if [ -f "${JUCE_DIR}/modules/juce_core/juce_core.h" ]; then
  echo "✓ JUCE 8.0.3 installed to ${JUCE_DIR}"
else
  echo "✗ Installation failed - juce_core.h not found"
  exit 1
fi
```

**If installation succeeds:**
- Save JUCE_DIR to config
- Continue to pluginval validation

**If installation fails:**
- Display error message
- Fall back to guided mode
- Show references/juce-setup-guide.md
- Wait for user to install manually
- After user confirms, return to JUCE STAGE 1 to re-validate

## pluginval Validation {#pluginval-automated}

### Automated Mode - Installation Menu

When pluginval not found and MODE=automated:

```
✗ pluginval not found (optional)

Would you like me to install pluginval?
1. Yes, install automatically
2. No, show me manual instructions
3. Skip pluginval (optional - validation tests won't run)

Choose (1-3): _
```

**User choice 1:**

```bash
# Set installation directory
PLUGINVAL_DIR="${HOME}/.local/bin"
mkdir -p "$PLUGINVAL_DIR"

# Download latest release from GitHub
PLUGINVAL_VERSION="1.0.3"

case "$(uname -s)" in
  Darwin*)
    PLUGINVAL_URL="https://github.com/Tracktion/pluginval/releases/download/v${PLUGINVAL_VERSION}/pluginval_macOS.zip"
    ;;
  Linux*)
    PLUGINVAL_URL="https://github.com/Tracktion/pluginval/releases/download/v${PLUGINVAL_VERSION}/pluginval_Linux.zip"
    ;;
  MINGW*|MSYS*|CYGWIN*)
    PLUGINVAL_URL="https://github.com/Tracktion/pluginval/releases/download/v${PLUGINVAL_VERSION}/pluginval_Windows.zip"
    ;;
esac

# Download and extract
curl -L -o /tmp/pluginval.zip "$PLUGINVAL_URL"
unzip -q /tmp/pluginval.zip -d "$PLUGINVAL_DIR"
chmod +x "${PLUGINVAL_DIR}/pluginval"
rm /tmp/pluginval.zip

# Verify installation
"${PLUGINVAL_DIR}/pluginval" --version
```

**User choice 3:**
Skip pluginval (it's optional). Continue to configuration persistence.

## Validation Command Reference

All validation commands follow this pattern:

```bash
bash .claude/skills/system-setup/assets/system-check.sh --check-<dependency> ${TEST_MODE:+--test=$TEST_MODE}
```

### Check Commands

- `--check-python` - Check for Python 3.8+
- `--check-build-tools` - Check for platform-specific build tools
- `--check-cmake` - Check for CMake 3.15+
- `--check-juce` - Check for JUCE 8.0+ in standard locations
- `--check-pluginval` - Check for pluginval
- `--validate-juce-path "$PATH"` - Validate user-provided JUCE path
- `--detect-platform` - Detect OS and architecture

### Response Format

All check commands return JSON:

```json
{
  "found": true,
  "version": "3.11.5",
  "path": "/usr/local/bin/python3",
  "meets_minimum": true
}
```

Or for not found:

```json
{
  "found": false,
  "message": "Python not found in PATH"
}
```
```

**Replace in SKILL.md (lines 154-455):**
```markdown
For detailed validation workflows, installation commands, and error handling for each dependency, see `references/dependency-validation-flows.md`.

The dependency validation follows the pattern defined in `<critical_sequence name="dependency_validation">` above, with specific installation procedures in the references file.
```

**Token savings:** ~3,000 tokens

**Verification:**
- File `references/dependency-validation-flows.md` exists
- SKILL.md lines 154-455 replaced with reference
- SKILL.md reduced by ~300 lines

### Fix 2.2: Extract error handling patterns to references/
**Location:** Lines 534-606
**Operation:** EXTRACT
**Create file:** `references/error-handling-patterns.md`
**File content:**
```markdown
# Error Handling Patterns

This document contains detailed error recovery workflows referenced by the main SKILL.md.

## Automated Installation Failure

**Trigger:** Automated installation command exits with non-zero status

**Recovery Sequence:**

1. Capture stderr output from failed command
2. Display error message:
   ```
   ✗ Failed to install {dependency} via {method}
   Error: {error_output}
   Falling back to manual instructions...
   ```
3. Display manual instructions from `references/platform-requirements.md` for the specific dependency
4. Present recovery menu:
   ```
   After installing manually:
   1. I've installed it manually, re-check now
   2. Skip this dependency (continue with others)
   3. Exit setup

   Choose (1-3): _
   ```
5. If user chooses option 1: Return to dependency validation (re-run check command)
6. If user chooses option 2: Mark as skipped, continue to next dependency
7. If user chooses option 3: Exit skill with incomplete setup

**Important:** Fallback applies ONLY to current dependency. MODE remains "automated" for subsequent dependencies.

## Version Too Old

**Trigger:** Dependency found but version < minimum required

**Recovery Sequence:**

1. Display warning:
   ```
   ✗ {dependency} {found_version} found, but {minimum_version} required
   ```
2. Branch on MODE:

   **Automated mode:**
   ```
   Would you like me to upgrade {dependency}?
   1. Yes, upgrade automatically
   2. No, show me manual upgrade instructions
   3. Skip (use existing version anyway - may cause build failures)

   Choose (1-3): _
   ```

   **Guided mode:**
   Display manual upgrade instructions from `references/platform-requirements.md` for the specific dependency.

   **Check-only mode:**
   Report issue in final report, continue to next dependency.

## Custom Path Validation Failure (JUCE-specific)

**Trigger:** User provides JUCE path that doesn't contain valid installation

**Recovery Sequence:**

1. Display error:
   ```
   ✗ Invalid JUCE installation at provided path

   The path must contain JUCE modules (modules/juce_core/juce_core.h).
   ```
2. Present retry menu:
   ```
   Would you like to:
   1. Try a different path (return to JUCE STAGE 3)
   2. Install JUCE to default location (GOTO JUCE STAGE 4)
   3. Show manual installation instructions

   Choose (1-3): _
   ```
3. Execute user's choice:
   - Option 1: Return to JUCE STAGE 3 (custom path validation)
   - Option 2: Proceed to JUCE STAGE 4 (automated install) - only if MODE=automated
   - Option 3: Display `references/juce-setup-guide.md`, wait for user

## Test Mode Errors

**Trigger:** TEST_MODE is set but system-check.sh doesn't recognize --test flag

**Recovery Sequence:**

1. Display error:
   ```
   ✗ Test mode is set but system-check.sh doesn't support --test flag

   This usually means system-check.sh is outdated or missing.
   ```
2. Present recovery menu:
   ```
   Options:
   1. Continue without test mode (use real system)
   2. Exit and update system-check.sh

   Choose (1-2): _
   ```

## Configuration Write Failure

**Trigger:** Failed to write `.claude/system-config.json`

**Recovery Sequence:**

1. Display error:
   ```
   ✗ Failed to write configuration to .claude/system-config.json

   Error: {error_message}

   Possible causes:
   - .claude/ directory doesn't exist
   - Insufficient permissions
   - Disk full
   ```
2. Attempt to create .claude/ directory:
   ```bash
   mkdir -p .claude
   ```
3. Retry write operation
4. If still fails, display:
   ```
   ✗ Cannot save configuration

   You can manually create .claude/system-config.json with these values:
   {display_config_values}

   Continue anyway?
   1. Yes (configuration won't be saved)
   2. No, exit setup

   Choose (1-2): _
   ```

## Platform Detection Failure

**Trigger:** system-check.sh --detect-platform returns error or unexpected platform

**Recovery Sequence:**

1. Display error:
   ```
   ✗ Failed to detect platform automatically

   Error: {error_message}
   ```
2. Prompt user to specify manually:
   ```
   Please select your platform:
   1. macOS (darwin)
   2. Linux
   3. Windows
   4. Exit setup

   Choose (1-4): _
   ```
3. If user selects platform, proceed with manual platform setting
4. If user exits, terminate skill

## Common Issues and Solutions

### Homebrew not installed (macOS)

When attempting automated installation and Homebrew is missing:

```bash
# Detect Homebrew absence
if ! command -v brew &> /dev/null; then
  echo "Homebrew not found. Installing Homebrew first..."
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

  # Add to PATH
  case "$(uname -m)" in
    arm64)
      eval "$(/opt/homebrew/bin/brew shellenv)"
      ;;
    x86_64)
      eval "$(/usr/local/bin/brew shellenv)"
      ;;
  esac
fi
```

### Permission denied errors

When installation fails due to permissions:

1. Display error
2. Suggest using sudo for system-wide install OR
3. Suggest using local install location (~/. local/bin)

### Network failures during download

When curl/wget fails to download dependencies:

1. Display network error
2. Check internet connectivity
3. Offer retry with alternative mirror/URL
4. Fall back to manual instructions if retry fails
```

**Replace in SKILL.md (lines 534-606):**
```markdown
For detailed error handling workflows and recovery procedures, see `references/error-handling-patterns.md`.

Error handling is enforced via `<error_handling>` tags throughout the dependency validation sequence.
```

**Token savings:** ~700 tokens

**Verification:**
- File `references/error-handling-patterns.md` exists
- SKILL.md lines 534-606 replaced with reference
- SKILL.md reduced by ~70 lines

### Fix 2.3: Extract implementation notes to references/
**Location:** Lines 653-706
**Operation:** EXTRACT
**Create file:** `references/implementation-notes.md`
**File content:**
```markdown
# Implementation Notes

## Success Criteria

This skill successfully completes when:

1. Platform detected and confirmed by user
2. All 5 dependencies validated (or user explicitly skipped non-critical ones)
3. Configuration saved to `.claude/system-config.json`
4. Configuration file added to .gitignore
5. Final system report displayed
6. User selected action from final menu

## Common Pitfalls

### 1. Installing when dependency already exists

**Problem:** Running automated install when dependency is already present can cause:
- Multiple versions installed
- PATH conflicts
- Version confusion

**Solution:** ALWAYS run check command first. Only offer installation if found=false.

### 2. Not validating version requirements

**Problem:** Finding a dependency doesn't mean it meets minimum version requirements.

**Solution:** Parse version from check command response and compare against minimum. If too old, offer upgrade.

### 3. Using relative paths in configuration

**Problem:** Relative paths break when working directory changes.

**Solution:** Always expand ~ to full home directory path. Validate all paths are absolute before saving to config.

### 4. Not testing executables actually run

**Problem:** A file may exist but not be executable or functional.

**Solution:** system-check.sh runs each executable with --version to confirm functionality.

### 5. Forgetting test mode in subcommands

**Problem:** TEST_MODE set at entry but not propagated to all system-check.sh calls.

**Solution:** Every system-check.sh invocation MUST include `${TEST_MODE:+--test=$TEST_MODE}` to conditionally append test flag.

### 6. Auto-proceeding without user confirmation

**Problem:** Installing dependencies without explicit user approval violates principle of user control.

**Solution:** Every installation presents a menu with options. WAIT for user choice. NEVER auto-install.

### 7. Not respecting MODE throughout execution

**Problem:** Starting in "automated" mode but then forcing manual steps, or vice versa.

**Solution:** Store MODE variable at skill entry. Check MODE before every installation/guidance decision.

### 8. Using AskUserQuestion instead of inline menus

**Problem:** AskUserQuestion tool has different behavior than inline numbered menus.

**Solution:** ALWAYS use inline numbered menus as shown in examples. Format: "Choose (1-X): _"

## Testing Recommendations

Test this skill against all scenarios using test-setup-skill.sh:

1. **Fresh system** - No dependencies installed
2. **Partial system** - Some dependencies present, others missing
3. **Complete system** - All dependencies already installed
4. **Outdated system** - Dependencies present but versions too old
5. **Custom JUCE path** - JUCE installed in non-standard location
6. **Network failures** - Simulate download failures
7. **Permission issues** - Test without sudo access

For each scenario, test all three modes:
- Automated
- Guided
- Check-only

Verify:
- TEST_MODE properly propagates to all system-check.sh calls
- Checkpoint menus appear at correct moments
- Config file has all required fields
- Mode-specific behavior is respected
- Error recovery works correctly

## Platform-Specific Notes

### macOS

- Build tools = Xcode Command Line Tools
- Install via: `xcode-select --install`
- Verify with: `xcode-select -p`
- Homebrew is the primary package manager
- Architecture detection important: arm64 (M1/M2) vs x86_64 (Intel)

### Linux

- Build tools = GCC/G++ toolchain
- Package managers vary by distro:
  - Debian/Ubuntu: apt
  - Fedora/RHEL: dnf
  - Arch: pacman
- Snap packages available for some tools (CMake, pluginval)

### Windows

- Build tools = MSVC Build Tools
- Cannot automate MSVC installation (requires GUI installer)
- Always fall back to guided mode for build tools on Windows
- Path handling differences (backslashes vs forward slashes)

## Maintenance Notes

### Updating minimum versions

To update minimum version requirements:

1. Update system-check.sh validation logic
2. Update menu templates in SKILL.md
3. Update documentation in references/platform-requirements.md
4. Test against systems with old and new versions

### Adding new dependencies

To add a new dependency:

1. Add check command to system-check.sh
2. Add validation step to `<critical_sequence name="dependency_validation">`
3. Add installation workflow to `references/dependency-validation-flows.md`
4. Add field to required_fields in `<state_requirement name="configuration_persistence">`
5. Update test scenarios

### Modifying validation workflow

The validation pattern is defined in `<dependency_validation_pattern>`. Changes here automatically apply to all dependencies. Only modify if fundamental validation logic changes.
```

**Replace in SKILL.md (lines 653-706):**
```markdown
For success criteria, common pitfalls, testing recommendations, and maintenance notes, see `references/implementation-notes.md`.
```

**Token savings:** ~500 tokens

**Verification:**
- File `references/implementation-notes.md` exists
- SKILL.md lines 653-706 replaced with reference
- SKILL.md reduced by ~50 lines

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Clarify skill description
**Location:** Lines 1-30
**Operation:** REPLACE
**Before:**
```markdown
<description>
Validates and configures all dependencies required for the Plugin Freedom System.
</description>
```

**After:**
```markdown
<description>
Validates and configures all dependencies required for the Plugin Freedom System. This is a STANDALONE skill that runs BEFORE plugin workflows begin. It checks for Python, build tools, CMake, JUCE, and pluginval, optionally installing missing dependencies with user approval. Configuration is saved to .claude/system-config.json for use by other skills.
</description>
```

**Verification:** Description now explicitly states standalone nature and configuration output

### Fix 3.2: Add explicit cross-references
**Location:** Line 70 (after delegation_rules)
**Operation:** INSERT
**Change:**
```markdown
<cross_references>
  <file>references/platform-requirements.md</file>
  <file>references/dependency-validation-flows.md</file>
  <file>references/error-handling-patterns.md</file>
  <file>references/implementation-notes.md</file>
  <script>assets/system-check.sh</script>
</cross_references>
```

**Verification:** Cross-references section lists all referenced files

## File Creation Manifest

Files to create:

1. `references/dependency-validation-flows.md` - Detailed validation workflows for Python, Build Tools, CMake, JUCE, pluginval (extracted from lines 154-455)
2. `references/error-handling-patterns.md` - Error recovery procedures (extracted from lines 534-606)
3. `references/implementation-notes.md` - Success criteria, pitfalls, testing, maintenance (extracted from lines 653-706)

## Execution Checklist

- [ ] Phase 1: All critical sequences wrapped in XML
  - [ ] Fix 1.1: skill_entry critical sequence (lines 76-114)
  - [ ] Fix 1.2: mode_definitions inserted (line 101)
  - [ ] Fix 1.3: platform_detection critical sequence (lines 106-136)
  - [ ] Fix 1.4: dependency_validation critical sequence (lines 139-455)
  - [ ] Fix 1.5: configuration_persistence state requirement (lines 459-487)
  - [ ] Fix 1.6: checkpoint_protocol enforcement (lines 610-627)
  - [ ] Fix 1.8: critical_requirements and anti_patterns (lines 667-706)
  - [ ] Fix 1.10: error_handling enforcement (lines 534-606)
  - [ ] Fix 1.11: menu_templates inserted (line 170)
  - [ ] Fix 1.12: final_system_report checkpoint (lines 492-528)
  - [ ] Fix 1.13: state_updates clarification (line 530)
  - [ ] Fix 1.14: delegation_rules inserted (line 60)
  - [ ] Fix 1.15: completion_criteria added (line 706)
- [ ] Phase 1: All decision gates have enforcement tags
- [ ] Phase 1: All delegation rules explicit
- [ ] Phase 1: Test mode propagation enforced in all check commands
- [ ] Phase 2: Large blocks extracted to references/
  - [ ] Fix 2.1: dependency-validation-flows.md created
  - [ ] Fix 2.2: error-handling-patterns.md created
  - [ ] Fix 2.3: implementation-notes.md created
- [ ] Phase 2: Context reduced by ~4,200 tokens (25%)
- [ ] Phase 3: All requirements use MUST/SHOULD/NEVER
  - [ ] Fix 3.1: Description clarified
  - [ ] Fix 3.2: Cross-references added
- [ ] Phase 3: Ambiguous pronouns clarified (via dependency_validation_pattern)
- [ ] Verification: All critical issues from analysis addressed
- [ ] Verification: All XML tags properly closed
- [ ] Verification: All file extractions have corresponding references in SKILL.md
- [ ] Verification: Skill health estimated to improve from Yellow to Green

## Verification Steps

After implementing all fixes:

### 1. Structure verification
```bash
# Check XML tags are balanced
grep -c "<critical_sequence" .claude/skills/system-setup/SKILL.md
grep -c "</critical_sequence>" .claude/skills/system-setup/SKILL.md

grep -c "<state_requirement" .claude/skills/system-setup/SKILL.md
grep -c "</state_requirement>" .claude/skills/system-setup/SKILL.md

grep -c "<checkpoint_protocol" .claude/skills/system-setup/SKILL.md
grep -c "</checkpoint_protocol>" .claude/skills/system-setup/SKILL.md

grep -c "<error_handling" .claude/skills/system-setup/SKILL.md
grep -c "</error_handling>" .claude/skills/system-setup/SKILL.md
```

Expected output: All counts should be equal (opening tags = closing tags)

### 2. Content extraction verification
```bash
# Check files were created
ls -lh .claude/skills/system-setup/references/dependency-validation-flows.md
ls -lh .claude/skills/system-setup/references/error-handling-patterns.md
ls -lh .claude/skills/system-setup/references/implementation-notes.md

# Check line count reduction
wc -l .claude/skills/system-setup/SKILL.md
# Expected: ~420 lines (down from ~706 lines)
```

### 3. Test mode enforcement verification
```bash
# Check all system-check.sh calls include test mode
grep "system-check.sh" .claude/skills/system-setup/SKILL.md | grep -v "TEST_MODE"
# Expected: No output (all calls should include TEST_MODE)
```

### 4. Functional testing
```bash
# Run test suite against all scenarios
bash .claude/skills/system-setup/assets/test-setup-skill.sh

# Test specific scenarios
/setup --test=fresh_system
/setup --test=partial_system
/setup --test=complete_system
/setup --test=custom_juce_path
```

Expected: All tests pass, checkpoint menus appear at correct moments, configuration saved correctly

### 5. Token count verification
```bash
# Count tokens in SKILL.md (approximate: words * 1.3)
wc -w .claude/skills/system-setup/SKILL.md
# Expected: ~9,850 words (~12,800 tokens, down from ~13,100 words / ~17,000 tokens)
```

## Estimated Impact

### Before
- Total lines: ~706
- Estimated tokens: ~17,000
- Critical sequence enforcement: 0 instances
- Ambiguous conditionals: ~15 locations
- Overall health: Yellow

### After
- Total lines: ~420 (40% reduction)
- Estimated tokens: ~12,800 (25% reduction)
- Critical sequence enforcement: 5 major sequences
- Ambiguous conditionals: 0 (all clarified with XML)
- Overall health: Green (estimated)

### Improvements
- **Comprehension:** High - XML enforcement eliminates execution order ambiguity
- **Maintainability:** High - Patterns extracted once, referenced by all dependencies
- **Correctness:** High - Enforcement prevents Claude from skipping steps or forgetting test mode
- **Debugging:** High - Clear sequence steps make it easy to identify which step failed
