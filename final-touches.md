# Final Touches: 85 → 90/100 Health Score

**Current State:** 85/100 (14/15 skills production ready)
**Target State:** 90/100 (15/15 skills production ready + tested)
**Estimated Effort:** 6 hours
**Priority:** High (production readiness)

---

## Overview

The skill refactoring achieved 85/100 health through systematic analysis, fixes, and QA audits. Two remaining gaps prevent production deployment:

1. **system-setup incomplete** - Critical dependency validation lacks XML enforcement (−5 points)
2. **Untested in production** - Structural verification only, no functional testing (−5 points)

This document provides step-by-step instructions to close both gaps.

---

## Part 1: Complete system-setup Skill (4 hours)

### The Problem

system-setup has 11 unfixed issues from the original fix plan. The most critical is **Fix 1.4** - wrapping the dependency validation workflow in XML enforcement tags.

**Current state:** 5 dependencies (Python, build-tools, CMake, JUCE, pluginval) validated through ~300 lines of nested conditional logic without XML structure.

**Risk:** Claude can:
- Skip validation steps
- Execute checks out of order
- Ignore version requirements
- Proceed without user confirmation in automated mode
- Miss error recovery paths

### Fix 1.4: Dependency Validation Sequence

**Location:** `.claude/skills/system-setup/SKILL.md` lines 139-455

**Goal:** Wrap the dependency validation workflow in `<critical_sequence>` with nested `<dependency>`, `<validation>`, and `<decision_gate>` tags.

#### Step 1: Backup Current Version (5 min)

```bash
cp .claude/skills/system-setup/SKILL.md .claude/skills/system-setup/SKILL.md.pre-xml-wrap
```

#### Step 2: Create XML Structure Template (30 min)

**Template for each dependency:**

```xml
<dependency name="[name]" order="[1-5]" depends_on="[prior deps]">
  <detection>
    <command>[bash command to check existence]</command>
    <version_check minimum="[version]">
      <command>[version check command]</command>
    </version_check>
  </detection>

  <validation>
    <if condition="found_and_valid">
      <action>Save to config, continue to next dependency</action>
    </if>

    <if condition="found_but_too_old">
      <decision_gate type="user_choice" wait_required="true">
        <option id="1">Upgrade via [method]</option>
        <option id="2">Manual instructions</option>
        <option id="3">Skip with warning</option>
      </decision_gate>
    </if>

    <if condition="not_found">
      <decision_gate type="mode_based">
        <automated_mode>
          <critical_sequence>
            <step id="check_installer">[Check if installer exists]</step>
            <step id="install_if_missing">[Install if needed]</step>
            <step id="run_install">[Run installation]</step>
            <step id="verify">[Verify success]</step>
            <decision_gate type="fallback">
              <if condition="success">Continue</if>
              <else>Fall back to guided mode</else>
            </decision_gate>
          </critical_sequence>
        </automated_mode>

        <guided_mode>
          <action>Display manual installation instructions from references/[file].md</action>
          <action>Wait for user confirmation</action>
        </guided_mode>

        <check_only_mode>
          <action>Report missing, continue</action>
        </check_only_mode>
      </decision_gate>
    </if>
  </validation>
</dependency>
```

#### Step 3: Apply to Python Dependency (45 min)

**Current (lines ~155-200):** Prose instructions for checking Python

**After:**

```xml
<dependency name="python" order="1" required="true">
  <detection>
    <command>python3 --version</command>
    <version_check minimum="3.8">
      <parse_pattern>Python (\d+\.\d+\.\d+)</parse_pattern>
    </version_check>
  </detection>

  <validation>
    <if condition="found AND version >= 3.8">
      <action>
        Display: "✓ Python [version] found at [path]"
        Save to system-config.json
        Continue to dependency 2 (build-tools)
      </action>
    </if>

    <if condition="found AND version < 3.8">
      <decision_gate type="upgrade" wait_required="true">
        Display: "✗ Python [version] found, but version 3.8+ is required"

        Options:
        1. Update via Homebrew (automated)
        2. Manual update instructions
        3. Continue anyway (build may fail)

        Choose (1-3): _
      </decision_gate>
    </if>

    <if condition="not_found">
      <decision_gate type="mode">
        <automated_mode>
          <critical_sequence enforcement="strict">
            <step id="check_homebrew">Check if Homebrew installed</step>
            <step id="install_homebrew" depends_on="check_homebrew">
              IF NOT installed: Run Homebrew installer
              ELSE: Continue
            </step>
            <step id="install_python" depends_on="install_homebrew">
              Run: brew install python3
            </step>
            <step id="verify" depends_on="install_python">
              Run: python3 --version
              IF success: Save to config
              ELSE: Fall back to guided mode
            </step>
          </critical_sequence>
        </automated_mode>

        <guided_mode>
          <action>Display content from references/platform-requirements.md § Python</action>
          <action>WAIT for user to complete installation</action>
          <action>Prompt: "Press Enter when Python is installed..."</action>
          <action>Re-run detection and validation</action>
        </guided_mode>

        <check_only_mode>
          <action>Report: "✗ Python 3.8+ not found"</action>
          <action>Continue to next dependency</action>
        </check_only_mode>
      </decision_gate>
    </if>
  </validation>
</dependency>
```

**Verification:**
```bash
# Check XML balance
grep -o '<dependency' .claude/skills/system-setup/SKILL.md | wc -l
grep -o '</dependency>' .claude/skills/system-setup/SKILL.md | wc -l
# Should match

# Verify structure
grep -A 50 '<dependency name="python"' .claude/skills/system-setup/SKILL.md
```

#### Step 4: Apply to Remaining 4 Dependencies (2 hours)

Repeat Step 3 pattern for:

1. **build-tools** (order="2", depends_on="1")
   - macOS: Xcode CLI Tools
   - Special case: GUI installation dialog
   - Manual intervention required

2. **cmake** (order="3", depends_on="1")
   - Similar to Python
   - Version check: 3.15+

3. **juce** (order="4", depends_on="2,3")
   - **Most complex**: Multiple search locations
   - Custom path support
   - Retry logic for custom paths
   - Needs extra nesting:
     ```xml
     <if condition="not_found_standard">
       <decision_gate type="custom_path">
         Ask: "Do you have JUCE in a custom location?"
         <if yes>
           <validation_loop max_attempts="3">
             Prompt for path
             Validate path (check for modules/juce_core/juce_core.h)
             IF valid: Save, continue
             IF invalid: Retry or abort
           </validation_loop>
         </if>
         <if no>
           [Standard install workflow]
         </if>
       </decision_gate>
     </if>
     ```

4. **pluginval** (order="5", depends_on="4")
   - Optional dependency
   - Similar to Python pattern
   - Simpler (no version check needed)

#### Step 5: Wrap Entire Sequence (30 min)

**Before the first `<dependency>` tag, add:**

```xml
<critical_sequence name="dependency-validation" enforcement="strict">
  **These dependencies MUST be validated in order. Each dependency may block on user input.**

  <execution_rules>
    - Execute dependencies sequentially (1 → 2 → 3 → 4 → 5)
    - Each dependency must complete or be explicitly skipped before proceeding
    - Mode choice (automated/guided/check-only) persists across all dependencies unless changed
    - Failed automated installs fall back to guided mode
    - User can abort at any decision gate
  </execution_rules>
```

**After the last `</dependency>` tag, add:**

```xml
</critical_sequence>
```

#### Step 6: Test XML Validity (15 min)

```bash
# Count all opening/closing tags
cd .claude/skills/system-setup
grep -o '<[^/>]*>' SKILL.md | sort | uniq -c > tags_opening.txt
grep -o '</[^>]*>' SKILL.md | sort | uniq -c > tags_closing.txt

# Compare - should match for each tag type
diff tags_opening.txt tags_closing.txt

# Check nesting
python3 << 'EOF'
import re
with open('SKILL.md') as f:
    content = f.read()
stack = []
for match in re.finditer(r'<(/?)(\w+)', content):
    is_closing = match.group(1) == '/'
    tag = match.group(2)
    if is_closing:
        if not stack or stack[-1] != tag:
            print(f"ERROR: Closing </{tag}> but stack is {stack}")
        else:
            stack.pop()
    else:
        stack.append(tag)
if stack:
    print(f"ERROR: Unclosed tags: {stack}")
else:
    print("SUCCESS: All tags balanced")
EOF
```

#### Step 7: Verify Content Preservation (15 min)

```bash
# Compare pre and post XML wrap
diff -u \
  .claude/skills/system-setup/SKILL.md.pre-xml-wrap \
  .claude/skills/system-setup/SKILL.md \
  | grep -E '^\+[^+]|^\-[^-]' \
  | less

# Should see only XML tags added, no content removed
```

### Fix 1.1, 1.3, 1.5, 1.6: Quick Wins (30 min total)

These are simple fixes from the original fix plan:

**Fix 1.1** - Test mode enforcement (10 min)
```xml
<test_mode_protocol>
IF --test=SCENARIO argument provided:
  MUST pass --test=$SCENARIO to ALL system-check.sh invocations
  MUST display test mode banner at start
  MUST NOT make actual system changes
</test_mode_protocol>
```

**Fix 1.3** - Critical requirements (already done in execution, verify present)

**Fix 1.5** - Precondition gate (5 min)
```xml
<precondition_gate>
None required - this is the entry point skill
Can be invoked anytime, creates system-config.json
</precondition_gate>
```

**Fix 1.6** - State updates clarification (10 min)
```xml
<state_management>
This skill is STANDALONE and does NOT:
- Update PLUGINS.md
- Create .continue-here.md
- Invoke other workflow skills

This skill ONLY:
- Creates/updates .claude/system-config.json
- Validates system dependencies
- Exits when validation complete
</state_management>
```

### Success Criteria

- [ ] All 5 dependencies wrapped in `<dependency>` tags
- [ ] XML tags balanced (opening = closing)
- [ ] Python validation script confirms nesting is valid
- [ ] Content diff shows only XML additions, no deletions
- [ ] system-setup QA report changes from Yellow to Green
- [ ] All decision gates have `wait_required="true"` or equivalent
- [ ] Mode persistence logic is explicit
- [ ] Error recovery paths defined

---

## Part 2: Production Testing (2 hours)

### Why This Matters

Structural QA verified XML balance and file paths. Functional testing verifies the skills actually work when invoked in real scenarios.

### Test Suite

#### Test 1: New Plugin Full Cycle (45 min)

**Objective:** Verify complete workflow from ideation → implementation → installation

**Steps:**

1. Start fresh conversation (clear context)

2. Test ideation:
   ```
   /dream
   ```
   - Verify skill activates
   - Answer questions (create simple plugin: "gain control")
   - Verify creative-brief.md created
   - Verify decision menu appears
   - Choose option to proceed to planning

3. Test planning:
   ```
   /plan GainControl
   ```
   - Verify contracts created (architecture.md, plan.md)
   - Verify PLUGINS.md updated
   - Verify decision menu appears

4. Test implementation:
   ```
   /implement GainControl
   ```
   - Verify plugin-workflow activates
   - Verify foundation-agent invoked (Stage 2)
   - Verify decision menu after Stage 2
   - Continue through Stage 3 (shell-agent)
   - Verify decision menu works
   - **Stop here** (don't complete full build, just verify orchestration)

5. Test resume:
   ```
   /continue GainControl
   ```
   - Verify .continue-here.md loaded
   - Verify resumes at correct stage
   - Verify state preserved

**Success Criteria:**
- [ ] Skills activate autonomously at correct trigger points
- [ ] All decision menus appear and accept input
- [ ] File references load (creative-brief, architecture, plan)
- [ ] Subagents invoke successfully
- [ ] State files created and readable
- [ ] No XML parsing errors in logs

#### Test 2: Plugin Improvement (30 min)

**Objective:** Verify modification workflow with backup/versioning

**Setup:**
```bash
# Create dummy plugin for testing
mkdir -p plugins/TestPlugin/Source
echo "// Test plugin" > plugins/TestPlugin/Source/PluginProcessor.cpp
echo "status: ✅ Working" >> PLUGINS.md
```

**Steps:**

1. Invoke improve:
   ```
   /improve TestPlugin
   ```

2. Verify backup creation
3. Verify tier selection menu
4. Request simple change ("add version comment")
5. Verify regression testing offered
6. Verify CHANGELOG.md updated

**Success Criteria:**
- [ ] Backup created before changes
- [ ] Tier selection gates work
- [ ] File modifications applied
- [ ] Version incremented correctly
- [ ] No data loss

#### Test 3: Research & Documentation (20 min)

**Objective:** Verify investigation and knowledge capture workflows

**Steps:**

1. Simulate problem scenario:
   ```
   I'm getting a CMake error about JUCE_VERSION not defined
   ```

2. If deep-research doesn't auto-activate:
   ```
   /research "CMake JUCE_VERSION undefined"
   ```

3. Verify Level 1 → 2 → 3 escalation logic
4. Once "solution" found, say:
   ```
   that worked, the fix was adding find_package(JUCE)
   ```

5. Verify troubleshooting-docs auto-activates
6. Verify category selection
7. Verify file created in correct directory

**Success Criteria:**
- [ ] Research levels escalate correctly
- [ ] Documentation captures after confirmation
- [ ] YAML frontmatter valid
- [ ] File created in category directory (not dual-indexed)

#### Test 4: Decision Menu Formats (15 min)

**Objective:** Verify all skills use inline numbered menus (not AskUserQuestion)

**Steps:**

Check decision menus in:
- plugin-ideation Phase 3.5
- plugin-workflow after each stage
- plugin-improve tier selection
- plugin-lifecycle mode selection
- system-setup mode selection

**Success Criteria:**
- [ ] All menus use format: "Choose (1-N): _"
- [ ] All menus wait for input before proceeding
- [ ] No AskUserQuestion tool invocations

#### Test 5: File Reference Loading (10 min)

**Objective:** Verify all reference files load correctly

**Steps:**

1. Invoke skill that uses references:
   ```
   /test SomePlugin
   ```

2. Verify references load:
   ```bash
   # Check logs for file read operations
   # Should see: Read tool invoked with references/manual-testing-guide.md
   ```

3. Test with aesthetic-dreaming (uses 4 references)
4. Test with plugin-workflow (uses 8 references)

**Success Criteria:**
- [ ] All reference files found (no "file not found" errors)
- [ ] Content loads completely
- [ ] No broken markdown links

### Test Execution Log

Create `testing-log.md` during testing:

```markdown
# Production Testing Log

**Date:** [timestamp]
**Tester:** [your name]

## Test 1: New Plugin Full Cycle
- [ ] /dream activated: [PASS/FAIL - notes]
- [ ] creative-brief.md created: [PASS/FAIL]
- [ ] /plan activated: [PASS/FAIL]
- [ ] contracts created: [PASS/FAIL]
- [ ] /implement activated: [PASS/FAIL]
- [ ] subagents invoked: [PASS/FAIL]
- [ ] /continue resumed: [PASS/FAIL]

**Issues found:**
1. [Issue description - line number - severity]

## Test 2: Plugin Improvement
[Same format]

## Test 3: Research & Documentation
[Same format]

## Test 4: Decision Menu Formats
[Same format]

## Test 5: File Reference Loading
[Same format]

---

## Summary
- Tests passed: [X/5]
- Critical issues: [count]
- Production ready: [YES/NO]
```

---

## Completion Checklist

### system-setup Completion
- [ ] Fix 1.4 applied (dependency validation XML)
- [ ] Fix 1.1 applied (test mode enforcement)
- [ ] Fix 1.5 applied (precondition gate)
- [ ] Fix 1.6 applied (state management)
- [ ] XML validation script passes
- [ ] Content preservation verified
- [ ] QA report re-run shows Green

### Production Testing
- [ ] Test 1 completed (full cycle)
- [ ] Test 2 completed (improvement)
- [ ] Test 3 completed (research/docs)
- [ ] Test 4 completed (decision menus)
- [ ] Test 5 completed (file references)
- [ ] testing-log.md created with results
- [ ] All critical issues resolved

### Documentation
- [ ] system-setup backup created
- [ ] XML validation results saved
- [ ] Testing log committed
- [ ] Any discovered issues documented

---

## Expected Outcome

**Before:** 85/100 (14/15 skills ready, untested)
**After:** 90/100 (15/15 skills ready, functionally verified)

**Time Investment:** 6 hours
**Risk Reduction:** High (catches runtime issues before production)
**Confidence Level:** Production-ready deployment

---

## If Issues Are Found

### Minor Issues (typos, broken links)
- Fix immediately
- Update QA report
- Continue testing

### Major Issues (workflow broken, data loss)
1. Stop testing
2. Document issue in testing-log.md
3. Create fix plan:
   - Analysis prompt (understand root cause)
   - Fix prompt (correct the issue)
   - Re-test verification
4. Resume testing after fix

### Critical Issues (system-wide pattern problem)
1. Stop all testing
2. Analyze if issue affects multiple skills
3. Create system-wide fix strategy
4. May require re-running analysis → fix → QA cycle
5. Adjust timeline estimate

---

## Post-Completion

Once 90/100 is achieved:

1. **Commit all changes:**
   ```bash
   git add .claude/skills/
   git commit -m "feat: complete skill refactoring to 90/100 health

   - Complete system-setup XML enforcement
   - Functional testing verified
   - All 15 skills production ready"
   ```

2. **Update system health tracking:**
   - Update skill-analysis/CHECKLIST.md
   - Mark all skills as ✅ Complete
   - Add testing-log.md reference

3. **Document for future:**
   - Note which patterns worked well
   - Note any discovered anti-patterns
   - Update troubleshooting docs if new issues found

4. **Deploy with confidence:**
   - System is production-ready
   - Known issues documented
   - Rollback possible via git
   - Health score verified through 4-stage pipeline (analysis → fix → QA → testing)

---

## Notes

**Why stop at 90 instead of 100?**

The 85 → 90 jump addresses production blockers. The 90 → 100 grind is diminishing returns:
- 90 → 95 requires optimization that provides minimal functional benefit
- 95 → 100 is perfection hunting with no practical gain

At 90/100, the system is:
- Functionally complete
- Structurally sound
- Tested in production scenarios
- Ready for real work

The remaining 10 points would require:
- Cross-skill standardization (polish)
- Aggressive context optimization (marginal gains)
- Edge case hunting (unlikely scenarios)
- Performance profiling (premature optimization)

**Better use of time:** Build plugins with the system and iterate based on real usage patterns.