# Fix Plan: plugin-improve

## Summary
- Critical fixes: **9**
- Extraction operations: **7**
- Total estimated changes: **23**
- Estimated time: **210 minutes (3.5 hours)**

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Wrap Precondition Checking in enforcement gate
**Location:** Lines 34-61
**Operation:** WRAP
**Before:**
```markdown
## Precondition Checking

**Before starting, verify:**

1. Read PLUGINS.md:
```
**After:**
```markdown
<gate_preconditions enforcement="strict">
## Precondition Checking

**MUST execute before any other phase. BLOCK if conditions not met.**

**Before starting, verify:**

1. Read PLUGINS.md:
```
**Note:** Add closing tag `</gate_preconditions>` after line 61
**Verification:** Confirm `<gate_preconditions>` opening at line 34 and `</gate_preconditions>` closing after line 61

### Fix 1.2: Replace AskUserQuestion with inline menu (Phase 0 Specificity)
**Location:** Lines 82-95
**Operation:** REPLACE
**Before:**
```markdown
**Assess specificity:**

- **Specific enough (1-2 clarification questions max):** Proceed to Phase 0.3 (4-question clarification batch)
- **Vague:** Present choice using AskUserQuestion:

```
Question:
  question: "Your request needs more detail. How should I proceed?"
  header: "Approach"
  options:
    - label: "Brainstorm approaches together", description: "I'll ask questions to explore options"
```
**After:**
```markdown
**Assess specificity:**

- **Specific enough (1-2 clarification questions max):** Proceed to Phase 0.3 (4-question clarification batch)
- **Vague:** Present inline decision menu:

```
Your request needs more detail. How should I proceed?

1. Brainstorm approaches together - I'll ask questions to explore options
2. Implement something reasonable - I'll investigate and propose a solution
3. Other

Choose (1-3): _
```

**Handle responses:**
- Option 1 → Invoke plugin-ideation skill in improvement mode
- Option 2 → Proceed to Phase 0.45 (Research Detection)
- Option 3 → Collect free-form text, reassess
```
**Verification:** No AskUserQuestion tool references in lines 82-95

### Fix 1.3: Replace AskUserQuestion with inline menus (Phase 0.3 Clarification)
**Location:** Lines 99-151
**Operation:** REPLACE
**Before:**
```markdown
## Phase 0.3: Clarification Questions (If Specific)

**If request is specific enough, ask targeted questions using AskUserQuestion tool:**

[4 AskUserQuestion blocks]
```
**After:**
```markdown
## Phase 0.3: Clarification Questions (If Specific)

**If request is specific enough, ask targeted questions using inline menus:**

**Question 1 - What to change:**
```
What needs to change?

1. Fix a bug - Something is broken or behaving incorrectly
2. Add a feature - New capability or enhancement
3. Improve existing behavior - Refine how something works
4. Other

Choose (1-4): _
```

**Question 2 - Scope:**
```
How extensive is this change?

1. Single function/method - Localized change in one place
2. Multiple related components - Changes across a few files
3. System-wide change - Affects architecture or many components
4. Other

Choose (1-4): _
```

**Question 3 - Priority:**
```
Version bump priority?

1. PATCH (bug fix) - Backward compatible fix, increment 0.0.X
2. MINOR (feature) - New feature, backward compatible, increment 0.X.0
3. MAJOR (breaking) - Breaking change, incompatible API, increment X.0.0
4. Other

Choose (1-4): _
```

**Question 4 - Testing:**
```
Should I run regression tests?

1. Yes, full regression suite - Compare new behavior vs baseline backup
2. Yes, but manual review if failures - Show me what changed and let me decide
3. No, skip regression tests - Just build and verify compilation
4. Other

Choose (1-4): _
```

**Collect all responses before proceeding to Phase 0.4**
```
**Verification:** No AskUserQuestion tool references in lines 99-151

### Fix 1.4: Replace AskUserQuestion with inline menu (Phase 0.4 Decision Gate)
**Location:** Lines 154-169
**Operation:** REPLACE
**Before:**
```markdown
## Phase 0.4: Decision Gate

**Show user what you understand, ask for confirmation:**

Use AskUserQuestion:
[AskUserQuestion block]
```
**After:**
```markdown
## Phase 0.4: Decision Gate

**Show user what you understand, ask for confirmation:**

```
I understand you want to:
- [Summary of change from Question 1]
- Scope: [Answer from Question 2]
- Version bump: [Answer from Question 3]
- Regression testing: [Answer from Question 4]

Is this correct?

1. Yes, proceed - Continue to Phase 0.45 (Research Detection)
2. No, refine - Ask me follow-up questions
3. No, cancel - Stop the workflow
4. Other

Choose (1-4): _
```

**Handle responses:**
- Option 1 → Proceed to Phase 0.45
- Option 2 → Return to Phase 0.3, ask follow-up questions
- Option 3 → Stop workflow, wait for new instruction
- Option 4 → Collect free-form text, reassess
```
**Verification:** No AskUserQuestion tool references in lines 154-169

### Fix 1.5: Wrap Research Detection in handoff protocol XML
**Location:** Lines 171-211
**Operation:** WRAP
**Before:**
```markdown
## Phase 0.45: Research Detection

**BEFORE starting investigation, check conversation history for deep-research findings:**
```
**After:**
```markdown
<handoff_protocol source="deep-research" target="plugin-improve">
## Phase 0.45: Research Detection

**PROTOCOL:** Check for pre-computed research before starting investigation

<detection_signals>
**BEFORE starting investigation, check conversation history for deep-research findings:**

**Scan recent messages for:**
```
**Note:** After the section describing "If NO research findings found:", add closing tags:
```markdown
</detection_signals>

</handoff_protocol>
```
**Verification:** Confirm `<handoff_protocol>` wraps lines 171-211 with proper closing tags

### Fix 1.6: Wrap Backup Verification in critical sequence
**Location:** Lines 298-336
**Operation:** WRAP
**Before:**
```markdown
## Phase 0.9: Backup Verification

**Goal:** Ensure rollback is possible if improvement fails
```
**After:**
```markdown
<critical_sequence phase="backup-verification" enforcement="strict">
## Phase 0.9: Backup Verification

**CRITICAL INVARIANT:** Phase 1 MUST NOT execute until backup verified.
**ENFORCEMENT:** Block execution, halt workflow if backup fails.
**VIOLATION CONSEQUENCE:** Data loss, no rollback path.

**Goal:** Ensure rollback is possible if improvement fails
```
**Note:** Add closing tag `</critical_sequence>` after line 336 (end of Phase 0.9)
**Verification:** Confirm `<critical_sequence>` tags wrap Phase 0.9 completely

### Fix 1.7: Wrap Backup Creation in critical sequence
**Location:** Lines 389-404
**Operation:** WRAP
**Before:**
```markdown
## Phase 2: Create Backup

**CRITICAL: Must complete before any code changes**
```
**After:**
```markdown
<critical_sequence phase="backup-creation" enforcement="strict">
## Phase 2: Create Backup

**CRITICAL INVARIANT:** Code changes (Phase 3) MUST NOT execute until backup created.
**ENFORCEMENT:** Block execution, halt workflow if backup creation fails.
**VIOLATION CONSEQUENCE:** No recovery if Phase 3 fails.

**CRITICAL: Must complete before any code changes**
```
**Note:** Add closing tag `</critical_sequence>` after line 404 (end of Phase 2)
**Verification:** Confirm `<critical_sequence>` tags wrap Phase 2 completely

### Fix 1.8: Wrap Regression Testing in validation gate
**Location:** Lines 598-699
**Operation:** WRAP
**Before:**
```markdown
## Phase 5.5: Regression Testing

**Check:** Does `.claude/skills/plugin-testing/SKILL.md` exist?
```
**After:**
```markdown
<validation_gate gate="regression-tests" required="conditional">
## Phase 5.5: Regression Testing

**GATE CONDITION:** If plugin-testing skill exists AND baseline backup available
**GATE FAILURE:** Present rollback options, require user decision

**Check:** Does `.claude/skills/plugin-testing/SKILL.md` exist?
```
**Note:** Add closing tag `</validation_gate>` after line 699 (end of Phase 5.5)
**Verification:** Confirm `<validation_gate>` tags wrap Phase 5.5 completely

### Fix 1.9: Wrap Phase 8 Completion in checkpoint protocol
**Location:** Lines 773-794
**Operation:** WRAP
**Before:**
```markdown
## Phase 8: Completion

**Present numbered decision menu (inline format):**
```
**After:**
```markdown
<checkpoint_protocol>
## Phase 8: Completion

**MUST present numbered decision menu using inline format (NOT AskUserQuestion tool)**

**Present numbered decision menu (inline format):**
```
**Note:** Add closing tag `</checkpoint_protocol>` after line 794 (end of Phase 8)
**Verification:** Confirm `<checkpoint_protocol>` tags wrap Phase 8 completely

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Extract Enhanced CHANGELOG Template
**Location:** Lines 430-540
**Operation:** EXTRACT
**Create file:** `references/changelog-format.md`
**File content:**
```markdown
# Enhanced CHANGELOG Format

## Template Structure

Add new version entry at top with technical details:

```markdown
# Changelog

All notable changes to this plugin will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this plugin adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [VERSION] - YYYY-MM-DD

### Added
- **Feature name:** Brief description
  - Technical detail 1
  - Technical detail 2
  - Impact: What this enables or improves

### Changed
- **Component name:** What changed
  - Before: Previous behavior
  - After: New behavior
  - Reason: Why this change was made

### Fixed
- **Issue:** What was broken
  - Root cause: Technical explanation
  - Solution: How it was fixed
  - Affects: Which parameters/features

### Breaking Changes (MAJOR versions only)
- **API change:** What's incompatible
  - Migration: How to update existing uses
  - Deadline: When old behavior will be removed

### Migration Notes (MAJOR versions only)
**From vX.Y.Z to vA.B.C:**
1. Update parameter mappings
2. Rebuild host applications
3. Test automation compatibility

### Technical Details
- Files modified: [list]
- DSP changes: [algorithm updates]
- Parameter changes: [APVTS modifications]
- Testing: [regression test results]

### Dependencies
- JUCE version: X.Y.Z
- Minimum macOS: X.Y
- Minimum Windows: X.Y
```

## Section Usage Guide

### For PATCH Versions (0.0.X)
**Use:** "Fixed" section primarily
**Include:**
- Root cause analysis
- Technical solution
- Affected components

**Example:**
```markdown
## [1.2.3] - 2025-11-12

### Fixed
- **Reverb tail cutoff:** Decay was stopping abruptly at 80% threshold
  - Root cause: Incorrect envelope calculation in DampingProcessor
  - Solution: Fixed exponential decay curve math
  - Affects: Decay Time and Damping parameters
```

### For MINOR Versions (0.X.0)
**Use:** "Added" and "Changed" sections
**Include:**
- Feature description
- Technical implementation notes
- Parameter additions/modifications

**Example:**
```markdown
## [1.3.0] - 2025-11-12

### Added
- **Pre-delay control:** Adjustable delay before reverb onset (0-200ms)
  - New parameter: Pre-Delay (predelay)
  - Implementation: Ring buffer with linear interpolation
  - Impact: Enables early reflection simulation

### Changed
- **Room size range:** Extended from 0-100% to 0-200%
  - Before: Small to medium rooms only
  - After: Small rooms to cathedral spaces
  - Reason: User request for larger spaces
```

### For MAJOR Versions (X.0.0)
**Use:** All sections including "Breaking Changes" and "Migration Notes"
**Include:**
- Complete changelog
- Incompatible changes documented
- Step-by-step migration guide

**Example:**
```markdown
## [2.0.0] - 2025-11-12

### Breaking Changes
- **Parameter IDs renamed:** Standardized to camelCase format
  - Migration: Update automation mappings in DAW
  - Old → New: room_size → roomSize, decay_time → decayTime
  - Deadline: v1.x support ends 2026-01-01

### Migration Notes
**From v1.x to v2.0:**
1. Export automation data from DAW
2. Replace old parameter IDs with new (see mapping above)
3. Re-import automation
4. Test all presets

### Added
- **Freeze function:** Capture current reverb tail and hold indefinitely
  - New parameter: Freeze (freeze)
  - Implementation: Buffer capture with crossfade
  - Impact: Creative sound design capability
```

## Best Practices

1. **Always include dates:** Use ISO format (YYYY-MM-DD)
2. **Link issues:** Reference GitHub issues if applicable
3. **Technical depth:** Include enough detail for developers to understand changes
4. **User impact:** Explain what users will notice
5. **Testing notes:** Document regression test results
6. **Version links:** Add comparison links at bottom of file

## Version Comparison Links

At the bottom of CHANGELOG.md, add:

```markdown
[Unreleased]: https://github.com/user/repo/compare/v1.2.3...HEAD
[1.2.3]: https://github.com/user/repo/compare/v1.2.2...v1.2.3
[1.2.2]: https://github.com/user/repo/compare/v1.2.1...v1.2.2
```
```
**Replace in SKILL.md (lines 430-540):**
```markdown
**Update CHANGELOG.md with enhanced format:**

See `references/changelog-format.md` for complete template structure and section usage guide.

**Quick reference:**
- **PATCH (0.0.X):** Use "Fixed" section primarily
- **MINOR (0.X.0):** Use "Added"/"Changed" sections
- **MAJOR (X.0.0):** Include "Breaking Changes" and "Migration Notes" sections

**Always include:**
- Date in ISO format (YYYY-MM-DD)
- Technical details (root cause, solution, affected components)
- User impact (what changes in behavior)
- Testing notes (regression test results if available)
```
**Verification:** File exists at `references/changelog-format.md` and SKILL.md reduced by ~110 lines

### Fix 2.2: Extract Versioning Logic
**Location:** Lines 846-871
**Operation:** EXTRACT
**Create file:** `references/versioning.md`
**File content:**
```markdown
# Semantic Versioning Decision Tree

## Version Bump Logic

```
Request analysis:
├─ Breaking change detected? → MAJOR (X.0.0)
│  ├─ Parameter ID changes
│  ├─ API signature changes
│  ├─ Incompatible state format
│  └─ Removed features
│
├─ New feature added? → MINOR (0.X.0)
│  ├─ New parameters
│  ├─ New capabilities
│  ├─ Enhanced features
│  └─ Backward compatible changes
│
└─ Bug fix only? → PATCH (0.0.X)
   ├─ Fixed crashes
   ├─ Fixed incorrect behavior
   ├─ Performance fixes
   └─ No new features
```

## Decision Table

| Change Type | Examples | Version Bump |
|------------|----------|--------------|
| Bug fix | Crash fix, calculation error, UI glitch | PATCH (0.0.X) |
| New feature | New parameter, new algorithm option | MINOR (0.X.0) |
| Enhancement | Improved UI, better performance | MINOR (0.X.0) |
| Breaking change | Parameter rename, API change | MAJOR (X.0.0) |
| Refactoring | Code cleanup, no behavior change | PATCH (0.0.X) |

## Breaking Change Detection

A change is **breaking** if:

1. **Parameter IDs change** (automation will break)
2. **Parameter ranges change** (saved values may be invalid)
3. **State format changes** (old presets won't load)
4. **Features removed** (users lose functionality)
5. **API signatures change** (if used as library)

If unsure, prefer **MINOR** over **MAJOR** (conservative approach).

## Version String Format

`MAJOR.MINOR.PATCH` where:
- **MAJOR:** Incompatible changes
- **MINOR:** Backward compatible features
- **PATCH:** Backward compatible fixes

Examples:
- `1.0.0` → `1.0.1`: Bug fix (PATCH)
- `1.0.1` → `1.1.0`: New feature (MINOR)
- `1.1.0` → `2.0.0`: Breaking change (MAJOR)

## Special Cases

### Pre-1.0.0 Versions
- Major version 0 (0.X.Y) signals initial development
- Breaking changes can use MINOR bumps (0.X.0)
- Stability not guaranteed

### Multiple Change Types
- If both feature and bug fix → MINOR
- If both feature and breaking change → MAJOR
- Always use highest severity bump
```
**Replace in SKILL.md (lines 846-871):**
```markdown
**Determine version bump using decision tree:**

See `references/versioning.md` for complete versioning logic.

**Quick reference:**
- Breaking change detected? → MAJOR (X.0.0)
- New feature added? → MINOR (0.X.0)
- Bug fix only? → PATCH (0.0.X)

If unsure, prefer MINOR over MAJOR (conservative approach).
```
**Verification:** File exists at `references/versioning.md` and SKILL.md reduced by ~30 lines

### Fix 2.3: Extract Breaking Change Detection
**Location:** Lines 799-823
**Operation:** EXTRACT
**Create file:** `references/breaking-changes.md`
**File content:**
```markdown
# Breaking Change Detection Guide

## Detection Criteria

A change is **breaking** if it makes the plugin incompatible with:
- Existing automation data
- Saved presets/state
- Host integration
- User workflows

## Specific Indicators

### 1. Parameter Changes
**Breaking:**
- Parameter ID renamed (automation breaks)
- Parameter range changed (saved values invalid)
- Parameter removed (automation breaks)
- Parameter type changed (e.g., float → choice)

**Non-breaking:**
- Parameter display name changed
- Parameter default value changed
- Parameter added (new, doesn't affect existing)

### 2. State Format Changes
**Breaking:**
- Preset file format changed (old presets won't load)
- State version incompatible (getStateInformation/setStateInformation)
- XML structure changed (if using XmlElement)

**Non-breaking:**
- Added new state fields (backward compatible)
- Improved state validation

### 3. API Changes
**Breaking:**
- Public method signature changed
- Public method removed
- Class renamed
- Namespace changed

**Non-breaking:**
- Private method changes
- Implementation details
- Internal refactoring

### 4. Behavior Changes
**Breaking:**
- DSP algorithm fundamentally different (presets sound different)
- Feature removed
- Default behavior changed (affects existing projects)

**Non-breaking:**
- Bug fix (makes behavior correct)
- Performance improvement (same output)
- UI improvement (same functionality)

## Detection Process

When analyzing changes, check:

1. **Read diff:** Look for parameter ID changes in source
2. **Check APVTS:** Any createParameterLayout() modifications?
3. **Check state:** Any getStateInformation() changes?
4. **Check API:** Any public method changes in PluginProcessor.h?
5. **Check DSP:** Any algorithm changes affecting output?

## Warning Template

If breaking changes detected, warn user:

```
⚠️  BREAKING CHANGES DETECTED

The following changes are incompatible with existing projects:

- [Specific change 1]: [Impact on users]
- [Specific change 2]: [Impact on users]

This will require a MAJOR version bump (X.0.0).

Users will need to:
1. [Migration step 1]
2. [Migration step 2]

Proceed with MAJOR version bump? (y/n): _
```

## Common False Positives

**Not breaking:**
- Adding new parameters (backward compatible)
- Changing private implementation
- Refactoring without API changes
- Fixing bugs (restoring correct behavior)
- Improving performance (same output)

## Edge Cases

**Debatable:**
- Bug fix that changes output (is current behavior "correct"?)
- Adding validation (may reject previously accepted input)
- Tightening parameter ranges (may clip existing values)

**Resolution:** Treat as breaking if users rely on current behavior, even if technically a bug.
```
**Replace in SKILL.md (lines 799-823):**
```markdown
**Check for breaking changes:**

See `references/breaking-changes.md` for complete detection criteria.

**Quick check:**
- Parameter IDs changed? → Breaking
- Parameter ranges changed? → Breaking
- State format changed? → Breaking
- Features removed? → Breaking
- API signatures changed? → Breaking

If breaking changes detected, warn user and require MAJOR version bump.
```
**Verification:** File exists at `references/breaking-changes.md` and SKILL.md reduced by ~25 lines

### Fix 2.4: Extract Regression Testing Interface
**Location:** Lines 633-644
**Operation:** EXTRACT
**Create file:** `references/regression-testing.md`
**File content:**
```markdown
# Regression Testing Protocol

## RegressionReport Interface

```typescript
interface RegressionReport {
  plugin: string;
  baselineVersion: string;
  newVersion: string;
  timestamp: string;

  summary: {
    totalTests: number;
    passed: number;
    failed: number;
    warnings: number;
  };

  results: {
    buildTest: {
      passed: boolean;
      errors?: string[];
    };

    loadTest: {
      passed: boolean;
      loadTime: number;
      errors?: string[];
    };

    parameterTest: {
      passed: boolean;
      changedParameters?: string[];
      missingParameters?: string[];
      newParameters?: string[];
    };

    audioTest: {
      passed: boolean;
      methodology: "pluginval" | "manual" | "automated";
      results?: {
        testName: string;
        passed: boolean;
        details?: string;
      }[];
    };
  };

  regressions?: {
    parameter: string;
    issue: string;
    severity: "critical" | "warning";
    baselineBehavior: string;
    newBehavior: string;
  }[];

  recommendation: "deploy" | "review" | "rollback";
  notes?: string;
}
```

## Test Execution Flow

1. **Baseline backup must exist** at `backups/${PLUGIN_NAME}/v${BASELINE_VERSION}/`
2. **Invoke plugin-testing skill** with baseline and new versions
3. **Collect test results** in RegressionReport format
4. **Analyze failures:**
   - Critical regressions → Recommend rollback
   - Warnings → Recommend review
   - All pass → Recommend deploy

## Comparison Methodology

### Build Test
- Compile both versions
- Verify no new warnings/errors

### Load Test
- Load plugin in host
- Measure load time (performance regression check)

### Parameter Test
- Compare APVTS parameter lists
- Detect: added, removed, changed parameters

### Audio Test
- Use pluginval if available
- Compare DSP output for same input
- Flag if output differs (potential regression)

## Failure Handling

### Critical Regressions
```
❌ CRITICAL REGRESSIONS DETECTED

[List of regressions with severity: critical]

Recommendation: ROLLBACK

Options:
1. Rollback to v${BASELINE_VERSION} - Undo all changes
2. Investigate failures - Debug before deploying
3. Deploy anyway - Override safety check (not recommended)

Choose (1-3): _
```

### Warnings Only
```
⚠️  WARNINGS DETECTED

[List of regressions with severity: warning]

These may be expected (e.g., intentional behavior changes).

Recommendation: REVIEW

Options:
1. Review detailed report - See full test results
2. Deploy anyway - Warnings acceptable
3. Rollback - Too risky

Choose (1-3): _
```

### All Tests Pass
```
✓ All regression tests passed

Baseline: v${BASELINE_VERSION} → New: v${NEW_VERSION}
- Build: ✓ Pass
- Load: ✓ Pass
- Parameters: ✓ No unexpected changes
- Audio: ✓ Pass

Recommendation: DEPLOY

Proceed to Phase 6 (Git Workflow)?
```

## Integration with plugin-testing

If `.claude/skills/plugin-testing/SKILL.md` exists:

1. **Invoke skill** with baseline and comparison versions
2. **Skill returns** RegressionReport JSON
3. **Parse report** and check recommendation field
4. **Present results** to user with decision menu

If plugin-testing not available:
- Skip regression testing
- Warn user: "Regression testing skipped (plugin-testing skill not installed)"
```
**Replace in SKILL.md (lines 633-644):**
```markdown
**If plugin-testing skill exists and baseline backup available:**

See `references/regression-testing.md` for complete RegressionReport interface and protocol.

**Quick summary:**
1. Invoke plugin-testing skill with baseline and new versions
2. Collect RegressionReport (build, load, parameter, audio tests)
3. Analyze failures: critical → rollback, warnings → review, pass → deploy
4. Present results with decision menu

**If regression tests fail, present rollback options before proceeding.**
```
**Verification:** File exists at `references/regression-testing.md` and SKILL.md reduced by ~40 lines

### Fix 2.5: Extract Backup Script Template
**Location:** Lines 313-317
**Operation:** EXTRACT
**Create file:** `assets/backup-template.sh`
**File content:**
```bash
#!/bin/bash
# Backup plugin source to timestamped directory

PLUGIN_NAME="$1"
CURRENT_VERSION="$2"

mkdir -p "backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/"
cp -r "plugins/${PLUGIN_NAME}/" "backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/"

echo "✓ Backup created: backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/"
```
**Replace in SKILL.md (lines 313-317):**
```markdown
**Create backup using template:**

```bash
# See assets/backup-template.sh for complete script
mkdir -p "backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/"
cp -r "plugins/${PLUGIN_NAME}/" "backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/"
```
```
**Verification:** File exists at `assets/backup-template.sh`

### Fix 2.6: Extract Rollback Script Template
**Location:** Lines 693-698
**Operation:** EXTRACT
**Create file:** `assets/rollback-template.sh`
**File content:**
```bash
#!/bin/bash
# Rollback plugin to previous version from backup

PLUGIN_NAME="$1"
ROLLBACK_VERSION="$2"

BACKUP_PATH="backups/${PLUGIN_NAME}/v${ROLLBACK_VERSION}/"

if [ ! -d "$BACKUP_PATH" ]; then
  echo "❌ Backup not found: $BACKUP_PATH"
  exit 1
fi

echo "Rolling back ${PLUGIN_NAME} to v${ROLLBACK_VERSION}..."

# Remove current source
rm -rf "plugins/${PLUGIN_NAME}/"

# Restore from backup
cp -r "$BACKUP_PATH" "plugins/${PLUGIN_NAME}/"

echo "✓ Rollback complete: ${PLUGIN_NAME} restored to v${ROLLBACK_VERSION}"
```
**Replace in SKILL.md (lines 693-698):**
```markdown
**Rollback using template:**

```bash
# See assets/rollback-template.sh for complete script
rm -rf "plugins/${PLUGIN_NAME}/"
cp -r "backups/${PLUGIN_NAME}/v${ROLLBACK_VERSION}/" "plugins/${PLUGIN_NAME}/"
```
```
**Verification:** File exists at `assets/rollback-template.sh`

### Fix 2.7: Extract Handoff Protocol Workflow
**Location:** Lines 19-32
**Operation:** EXTRACT
**Create file:** `references/handoff-protocols.md`
**File content:**
```markdown
# Handoff Protocol: deep-research → plugin-improve

## Overview

The plugin-improve skill can receive pre-computed research findings from the deep-research skill, preserving expensive research context (Opus + extended thinking) across skill boundaries.

## Handoff Trigger

**Routing Directive:** `Invoke plugin-improve skill`

When deep-research completes investigation and user chooses "Apply solution", deep-research outputs this directive for the main conversation orchestrator to act on.

## Detection Mechanism

**Phase 0.45: Research Detection** scans conversation history for:

1. **Routing directive:** "Invoke plugin-improve skill"
2. **Skill attribution:** Messages from deep-research skill
3. **Research artifacts:** Level 1/2/3 reports
4. **Problem analysis:** Root cause + recommended solution

## Workflow

```
User: /research [problem]
  ↓
deep-research skill invoked
  ↓
Investigates (Level 1/2/3)
  ↓
Presents findings with decision menu
  ↓
User: "Apply solution" (Option 1)
  ↓
deep-research outputs: "Invoke plugin-improve skill"
  ↓
Main conversation recognizes directive
  ↓
Invokes plugin-improve skill
  ↓
plugin-improve Phase 0.45 detects research
  ↓
Extracts findings, skips investigation
  ↓
Proceeds to implementation approval
```

## Information Extracted

When handoff detected, extract:

- **Problem:** [from research report]
- **Root Cause:** [from research analysis]
- **Solution:** [from research recommendations]
- **Steps:** [from research implementation plan]

## Benefits

1. **Context preservation:** Research findings carry forward
2. **Efficiency:** Skip redundant investigation (Phase 0.5)
3. **Quality:** Deep-research uses Opus + extended thinking
4. **Separation of concerns:** Investigation vs. implementation

## Handoff Detection Code

```markdown
<handoff_protocol source="deep-research" target="plugin-improve">
## Phase 0.45: Research Detection

**Scan recent messages for:**

- Routing directive: "Invoke plugin-improve skill"
- Messages from deep-research skill
- Research reports (Level 1/2/3)
- Problem analysis (root cause + solution)

**If deep-research findings found:**

Extract and present:
```
✓ Research already completed (deep-research Level N)

Problem: [extracted]
Root Cause: [extracted]
Solution: [extracted]
Steps: [extracted]

Ready to implement? (y/n): _
```

Wait for user approval → Proceed to Phase 0.9

**If NO research findings found:**

Proceed to Phase 0.5 (Investigation) - perform fresh root cause analysis

</handoff_protocol>
```

## Alternative: No Handoff

If Phase 0.45 finds no research findings:
- Proceed to Phase 0.5 (Investigation)
- Perform fresh root cause analysis
- Use current skill context (Sonnet, no extended thinking)
```
**Replace in SKILL.md (lines 19-32):**
```markdown
**Integration with deep-research:**

<handoff_protocol>
**Trigger:** deep-research outputs "Invoke plugin-improve skill"
**Detection:** Phase 0.45 scans conversation history
**Action:** Extract research findings, skip investigation (Phase 0.5)
**Benefits:** Preserve expensive research context (Opus + extended thinking)
</handoff_protocol>

See `references/handoff-protocols.md` for complete workflow and detection mechanism.
```
**Verification:** File exists at `references/handoff-protocols.md` and SKILL.md reduced by ~15 lines

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Remove Phase 7 Commentary Duplication
**Location:** Lines 872-920
**Operation:** REPLACE
**Before:**
```markdown
## Version History

**Phase 7 enhancements (2025-11):**
[~450 words of commentary on skill enhancements]
```
**After:**
```markdown
## Version History

**Phase 7 enhancements (2025-11):**
- Regression testing integration (Phase 5.5)
- Enhanced changelog format (Phase 4)
- Backup verification protocol (Phase 0.9)
- One-command rollback mechanism
- Breaking change detection

**See:** `architecture/17-testing-strategy.md`, `scripts/verify-backup.sh`, `references/regression-testing.md`
```
**Verification:** Lines 872-920 reduced from ~450 words to ~50 words

### Fix 3.2: Enhance description with trigger terms
**Location:** Lines 1-13 (YAML frontmatter)
**Operation:** REPLACE
**Before:**
```yaml
description: Fix bugs and add features with versioning, regression testing, and backups
```
**After:**
```yaml
description: Fix bugs, add features to completed plugins. Includes versioning, backups, regression testing, changelog automation. Trigger terms - improve, fix, add feature, modify plugin, version bump, rollback
```
**Verification:** Description includes explicit trigger terms for autonomous activation

### Fix 3.3: Make Phase 0.5 Tier Selection a decision tree
**Location:** Lines 213-259
**Operation:** REPLACE
**Before:**
```markdown
**Tier Selection:**

- **Tier 1 (5-10 min):** Cosmetic changes, simple fixes, obvious issues
- **Tier 2 (15-30 min):** Logic errors, parameter issues, integration bugs
- **Tier 3 (30-60 min):** Complex bugs, performance issues, architectural problems
```
**After:**
```markdown
**Tier Selection Decision Tree:**

```
Request analysis:
├─ Known pattern match? → Tier 1
├─ Single component issue? → Tier 2
├─ Multi-component / Performance / Architecture? → Tier 3
└─ Unclear? → Start Tier 1, escalate if needed
```

**Tier 1 (5-10 min):** File read + pattern match
- Cosmetic changes, simple fixes, obvious issues
- Known patterns from troubleshooting/

**Tier 2 (15-30 min):** Logic trace + integration check
- Logic errors, parameter issues, integration bugs
- Single component, requires code analysis

**Tier 3 (30-60 min):** Invoke deep-research skill (uses Opus)
- Complex bugs, performance issues, architectural problems
- Multi-component, requires deep investigation
```
**Verification:** Tier selection now uses visual decision tree format

### Fix 3.4: Add dependency markers to Phase 1
**Location:** Lines 338-387
**Operation:** INSERT
**Before:**
```markdown
## Phase 1: Pre-Implementation Checks

**Load current state:**
```
**After:**
```markdown
<phase id="1" name="pre-implementation" dependencies="backup-verification">
## Phase 1: Pre-Implementation Checks

**DEPENDENCY:** MUST NOT execute until Phase 0.9 (Backup Verification) completes.

**Load current state:**
```
**Note:** Add closing tag `</phase>` after line 387
**Verification:** Phase 1 has explicit dependency marker

### Fix 3.5: Add dependency markers to Phase 3
**Location:** Lines 407-422
**Operation:** INSERT
**Before:**
```markdown
## Phase 3: Implementation

**Execute the change:**
```
**After:**
```markdown
<phase id="3" name="implementation" dependencies="backup-creation">
## Phase 3: Implementation

**DEPENDENCY:** MUST NOT execute until Phase 2 (Backup Creation) completes.
**SAFETY:** If implementation fails, rollback path guaranteed by Phase 2 backup.

**Execute the change:**
```
**Note:** Add closing tag `</phase>` after line 422
**Verification:** Phase 3 has explicit dependency marker

### Fix 3.6: Add delegation marker to Phase 5 Build
**Location:** Lines 543-577
**Operation:** WRAP
**Before:**
```markdown
## Phase 5: Build and Test

**Delegate to build-automation skill:**
```
**After:**
```markdown
<delegation_rule target="build-automation" required="true">
## Phase 5: Build and Test

**DELEGATION:** MUST invoke build-automation skill for all build operations.
**REASON:** Centralized build logic, 7-phase pipeline with verification.

**Delegate to build-automation skill:**
```
**Note:** Add closing tag `</delegation_rule>` after line 577
**Verification:** Phase 5 build delegation explicitly marked

### Fix 3.7: Add delegation marker to Phase 7 Installation
**Location:** Lines 739-769
**Operation:** WRAP
**Before:**
```markdown
## Phase 7: Installation (Optional)

**If user requested installation:**
```
**After:**
```markdown
<delegation_rule target="plugin-lifecycle" required="false">
## Phase 7: Installation (Optional)

**DELEGATION:** If user requested installation, invoke plugin-lifecycle skill.
**REASON:** Centralized installation logic with cache clearing and verification.

**If user requested installation:**
```
**Note:** Add closing tag `</delegation_rule>` after line 769
**Verification:** Phase 7 installation delegation explicitly marked

## File Creation Manifest

Files to create:
1. `references/changelog-format.md` - Enhanced changelog template structure (170 lines)
2. `references/versioning.md` - Semantic versioning decision tree (60 lines)
3. `references/breaking-changes.md` - Breaking change detection criteria (120 lines)
4. `references/regression-testing.md` - RegressionReport interface and protocol (150 lines)
5. `references/handoff-protocols.md` - deep-research handoff workflow (90 lines)
6. `assets/backup-template.sh` - Backup creation script (10 lines)
7. `assets/rollback-template.sh` - Rollback procedure script (18 lines)

## Execution Checklist

### Phase 1: Critical Fixes (120 minutes)
- [ ] Fix 1.1: Wrap Precondition Checking in `<gate_preconditions>`
- [ ] Fix 1.2: Replace AskUserQuestion (Phase 0 Specificity)
- [ ] Fix 1.3: Replace AskUserQuestion (Phase 0.3 Clarification)
- [ ] Fix 1.4: Replace AskUserQuestion (Phase 0.4 Decision Gate)
- [ ] Fix 1.5: Wrap Research Detection in `<handoff_protocol>`
- [ ] Fix 1.6: Wrap Backup Verification in `<critical_sequence>`
- [ ] Fix 1.7: Wrap Backup Creation in `<critical_sequence>`
- [ ] Fix 1.8: Wrap Regression Testing in `<validation_gate>`
- [ ] Fix 1.9: Wrap Phase 8 Completion in `<checkpoint_protocol>`
- [ ] Verification: No AskUserQuestion tool references remain
- [ ] Verification: All critical sequences have enforcement tags
- [ ] Verification: All phases have proper XML wrapping

### Phase 2: Content Extraction (60 minutes)
- [ ] Fix 2.1: Extract changelog template → `references/changelog-format.md`
- [ ] Fix 2.2: Extract versioning logic → `references/versioning.md`
- [ ] Fix 2.3: Extract breaking changes → `references/breaking-changes.md`
- [ ] Fix 2.4: Extract regression testing → `references/regression-testing.md`
- [ ] Fix 2.5: Extract backup script → `assets/backup-template.sh`
- [ ] Fix 2.6: Extract rollback script → `assets/rollback-template.sh`
- [ ] Fix 2.7: Extract handoff protocol → `references/handoff-protocols.md`
- [ ] Verification: All 7 files created with correct content
- [ ] Verification: SKILL.md references point to correct files
- [ ] Verification: Context reduced by ~8,000 tokens

### Phase 3: Polish (30 minutes)
- [ ] Fix 3.1: Remove Phase 7 commentary duplication
- [ ] Fix 3.2: Enhance description with trigger terms
- [ ] Fix 3.3: Make Tier Selection a visual decision tree
- [ ] Fix 3.4: Add dependency markers to Phase 1
- [ ] Fix 3.5: Add dependency markers to Phase 3
- [ ] Fix 3.6: Add delegation marker to Phase 5 Build
- [ ] Fix 3.7: Add delegation marker to Phase 7 Installation
- [ ] Verification: All phases have dependency/delegation markers
- [ ] Verification: Description includes trigger terms
- [ ] Verification: Tier selection uses decision tree format

### Final Verification
- [ ] SKILL.md reduced from ~24,500 tokens to ~16,500 tokens
- [ ] All XML tags properly nested and closed
- [ ] No AskUserQuestion tool references in SKILL.md
- [ ] All decision menus use inline numbered format
- [ ] All critical sequences have enforcement="strict"
- [ ] All validation gates have required="conditional"
- [ ] All delegation rules have required=true/false
- [ ] All extracted files exist and are complete
- [ ] All SKILL.md references point to correct files
- [ ] Skill health improved from Yellow to Green (estimated)

## Estimated Impact Summary

**Context Reduction:** ~8,000 tokens (33% reduction)
- From: ~24,500 tokens
- To: ~16,500 tokens

**Comprehension Improvement:** HIGH
- Critical sequences explicitly enforced
- Handoff protocols structured
- Decision gates explicit
- Templates extracted (pure orchestration logic)

**Safety Improvement:** HIGH
- Backup verification blocking
- Backup creation blocking
- Regression testing gated
- Consistent checkpoint format

**Maintenance Improvement:** HIGH
- Templates independently updatable
- Versioning logic externalized
- Changelog format versioned
- Clear separation: orchestration vs templates vs docs
