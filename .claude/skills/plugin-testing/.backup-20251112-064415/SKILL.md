---
name: plugin-testing
description: Automated stability tests and validation suite
allowed-tools:
  - Read
  - Bash
  - Task # For deep-research on failures
preconditions:
  - Plugin must exist
  - Plugin status must NOT be 💡 (must have implementation)
---

# plugin-testing Skill

**Purpose:** Catch crashes, parameter issues, and state bugs in 2 minutes with automated tests.

## Workflow Overview

This skill provides three test modes:

1. **Automated Testing** (~2 min) - Quick C++ unit tests for stability
2. **Build + Pluginval** (~5-10 min) - Industry-standard validation (50+ tests) ⭐ **RECOMMENDED**
3. **Manual DAW Testing** (~30-60 min) - Real-world testing with guided checklist

## Phase 1: Detect Plugin and Mode Selection

**Input:** User invokes `/test [PluginName]` or asks to test a plugin

**Steps:**

1. Parse plugin name from user input
2. Verify plugin exists in `PLUGINS.md`
3. Check plugin status - must NOT be 💡 (must have implementation)
4. Check for Tests/ directory existence

**If mode not specified, present decision menu:**

```
How would you like to test [PluginName]?

1. Automated stability tests (~2 min)
   → Quick C++ unit tests (parameter response, state save/load, CPU usage)
   → Requires: Tests/ directory with Catch2 tests

2. Build + pluginval (~5-10 min) ⭐ RECOMMENDED
   → Industry-standard validation (50+ tests, strictness level 10)
   → Tests: Thread safety, automation, state recall, audio validation
   → Requires: Release build, pluginval installed

3. Manual DAW testing (~30-60 min)
   → Guided checklist for real-world testing
   → Tests: Sonic quality, workflow, edge cases in actual DAW

4. Skip testing (NOT RECOMMENDED)
   → Proceed to installation without validation

Choose (1-4): _
```

**Recommendation logic:**

- If `Tests/` directory exists: Suggest option 1 first, then 2
- If no `Tests/` directory: Recommend option 2 (pluginval)
- Before final release: Always suggest option 2 + 3

**Parse shorthand commands:**

- `/test [PluginName] build` → Jump to Mode 2
- `/test [PluginName] quick` → Jump to Mode 1
- `/test [PluginName] manual` → Jump to Mode 3

## Phase 2: Execute Test Mode

### Mode 1: Automated Testing

**See:** `references/test-specifications.md` for detailed test implementations

**Prerequisites check:**

```bash
# Check for Tests/ directory
test -d "plugins/$PLUGIN_NAME/Tests/"

# Check for StabilityTests.cpp
test -f "plugins/$PLUGIN_NAME/Tests/StabilityTests.cpp"
```

**If missing:** Inform user that automated tests require test infrastructure and suggest Mode 2 (pluginval) instead.

**Build and run tests:**

```bash
cd build
cmake .. -DBUILD_TESTS=ON
cmake --build . --target ${PLUGIN_NAME}Tests
./Tests/${PLUGIN_NAME}Tests
```

**Parse test output:**

- Extract pass/fail for each of 5 tests
- Identify failed test names and error messages
- Calculate pass rate (X/5 passed)

**Report format:**

**All tests pass:**

```
✅ All tests PASSED (5/5)

Parameter response: PASS (5/5 parameters respond correctly)
State save/load: PASS (all parameters restore correctly)
Silent input: PASS (output RMS: 0.00002 - essentially silent)
Feedback test: PASS (max output over 1000 iterations: 1.2)
CPU performance: PASS (real-time factor: 0.03 - using 3% CPU)

Build: Release mode, macOS arm64, 44.1kHz, 512 samples
```

**Some tests fail:**

```
❌ Tests FAILED (3/5 passed)

✅ Parameter response: PASS (5/5 parameters)
❌ State save/load: FAIL - Parameters not restoring: [bypass, mix]
✅ Silent input: PASS (output RMS: 0.0)
❌ Feedback test: FAIL - Output exploding (iteration 247, max: 87.3)
✅ CPU performance: PASS (real-time factor: 0.03)

What would you like to do?
1. Investigate failures (launch troubleshooting agents)
2. Show test code (see exact test implementation)
3. Show full test output (detailed logs)
4. I'll fix manually
5. Other

Choose (1-5): _
```

**After results, offer next steps (see Phase 3)**

### Mode 2: Build + Pluginval

**See:** `references/pluginval-guide.md` for detailed implementation

**Prerequisites check:**

```bash
# Check for pluginval.app first (standard macOS install), then fall back to PATH
if [ -x "/Applications/pluginval.app/Contents/MacOS/pluginval" ]; then
    PLUGINVAL_PATH="/Applications/pluginval.app/Contents/MacOS/pluginval"
elif command -v pluginval >/dev/null 2>&1; then
    PLUGINVAL_PATH="pluginval"
else
    echo "Pluginval not found"
    PLUGINVAL_PATH=""
fi
```

**If missing:**

```
Pluginval not found.

Install options:
1. Via Homebrew: brew install --cask pluginval
   (Installs to /Applications/pluginval.app)
2. Download from: https://github.com/Tracktion/pluginval/releases
   (Place in /Applications/pluginval.app)
3. Skip pluginval, try automated tests instead

After installing, pluginval.app should be in /Applications/
No need to add to PATH or create symlinks.

Choose (1-3): _
```

**Build release binaries:**

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel

# Locate binaries
VST3_PATH="build/plugins/$PLUGIN_NAME/${PLUGIN_NAME}_artefacts/Release/VST3/${PRODUCT_NAME}.vst3"
AU_PATH="build/plugins/$PLUGIN_NAME/${PLUGIN_NAME}_artefacts/Release/AU/${PRODUCT_NAME}.component"
```

**Run pluginval:**

```bash
"${PLUGINVAL_PATH}" --validate "$VST3_PATH" \
          --strictness-level 10 \
          --timeout-ms 30000 \
          --verbose

"${PLUGINVAL_PATH}" --validate "$AU_PATH" \
          --strictness-level 10 \
          --timeout-ms 30000 \
          --verbose
```

**Parse pluginval output** (see `references/pluginval-guide.md` for failure parsing)

**Report format:**

**All tests pass:**

```
✅ Build successful (Release mode)

Binaries:
- VST3: build/plugins/[Name]/[Name]_artefacts/Release/VST3/[Product].vst3 (2.4 MB)
- AU: build/plugins/[Name]/[Name]_artefacts/Release/AU/[Product].component (2.4 MB)

✅ Pluginval PASSED (strictness level 10)

VST3: All 50 tests passed
AU: All 50 tests passed

Total validation time: 8.7s

Plugin is ready for installation and real-world testing.
```

**Some tests fail:**

```
✗ Pluginval FAILED (47/50 passed)

Failed tests:
- [8/50] State save/load: Plugin state not restoring correctly
- [23/50] Thread safety: Memory allocation in processBlock()
- [34/50] AU validation: Component not code-signed

What would you like to do?
1. Investigate failures (detailed analysis)
2. Show full pluginval output (complete logs)
3. Continue anyway (NOT RECOMMENDED - failures may cause crashes)
4. I'll fix manually
5. Other

Choose (1-5): _
```

**After results, offer next steps (see Phase 3)**

### Mode 3: Manual DAW Testing

**See:** `assets/manual-testing-checklist.md` for full template

**Load parameter spec:**

```bash
# Read parameter-spec.md to generate specific tests
cat "plugins/$PLUGIN_NAME/.ideas/parameter-spec.md"
```

**Generate customized checklist:**

1. Read `assets/manual-testing-checklist.md` template
2. Extract parameters from `parameter-spec.md`
3. Insert parameter-specific tests into checklist
4. Display complete checklist to user

**Checklist structure:**

- Setup (load plugin in DAW)
- Parameter testing (for each parameter from spec)
- Automation testing
- Preset recall
- Project save/load
- Sample rate changes
- Buffer size changes
- Channel configuration
- Edge cases
- Sonic quality
- Stress testing
- Cross-DAW testing (optional)
- Final verification
- Notes/issues section

**User confirms checklist completion, then offer next steps (see Phase 3)**

## Phase 3: Post-Test Decision Menu

**After Mode 1 (automated tests) passes:**

```
✓ All automated tests passed (5/5)

What's next?
1. Continue to next stage (if in workflow)
2. Run pluginval for industry-standard validation (recommended)
3. Install plugin (/install-plugin)
4. Review detailed test results
5. Other

Choose (1-5): _
```

**After Mode 2 (pluginval) passes:**

```
✓ Pluginval passed (50/50 tests, strictness level 10)

What's next?
1. Install plugin to system folders (recommended)
2. Run manual DAW testing checklist
3. Review full validation report
4. Build for distribution (future feature)
5. Other

Choose (1-5): _
```

**After Mode 3 (manual testing) complete:**

```
✓ Manual DAW testing complete

What's next?
1. Install plugin (/install-plugin) → Ready for production use
2. Mark plugin as release-ready (update PLUGINS.md status)
3. Report issues found (if any)
4. Run additional automated tests
5. Other

Choose (1-5): _
```

## Phase 4: Failure Investigation (Option 1)

**When user chooses "Investigate failures":**

**For each failed test:**

1. Read detailed explanation from `references/troubleshooting.md`
2. Provide specific fix recommendations
3. Offer to show relevant code sections
4. If available (Phase 7), launch `Task` agent with `deep-research` to find root cause

**Example investigation output:**

```
Investigating "State save/load" failure...

Root cause: Parameters not saved in getStateInformation()

The plugin is using raw AudioProcessor parameters instead of APVTS.

Fix:
1. Open PluginProcessor.h
2. Verify you have: juce::AudioProcessorValueTreeState apvts;
3. Open PluginProcessor.cpp
4. In getStateInformation():
   auto state = apvts.copyState();
   std::unique_ptr<juce::XmlElement> xml(state.createXml());
   copyXmlToBinary(*xml, destData);

5. In setStateInformation():
   std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
   if (xml.get() != nullptr)
       apvts.replaceState(juce::ValueTree::fromXml(*xml));

This ensures APVTS handles all state management automatically.

Would you like me to:
1. Show the current getStateInformation() implementation
2. Launch deep-research agent to scan codebase
3. I'll fix it myself
4. Other

Choose (1-4): _
```

## Phase 5: Log Test Results

**Save detailed logs for every test run:**

**Location:** `logs/[PluginName]/test_[timestamp].log`

**Log format:**

```
================================================================================
Test Report: [PluginName]
================================================================================
Date: 2025-01-10 14:32:17
Mode: [Mode 1 / Mode 2 / Mode 3]
Plugin: [PluginName] v1.0.0
Formats: VST3, AU

[Mode-specific details]

Test Results:
[Pass/fail summary]

Total time: [duration]

Conclusion: [Plugin ready / Failures detected]
================================================================================

[Full output below]
...
```

**Update state files:**

- `.continue-here.md` → Mark testing complete, update next step
- `PLUGINS.md` → Update test status (last tested date, results)

## Success Criteria

Testing is successful when:

- ✅ Tests run without crashes (even if some fail, process completes)
- ✅ All tests pass OR failures are documented with clear explanations
- ✅ User understands what failed and why (no mystery errors)
- ✅ Logs saved for future reference (`logs/[PluginName]/`)
- ✅ User knows next step (install, fix issues, continue workflow)
- ✅ Test results stored in PLUGINS.md (test date, pass/fail, mode used)

**NOT required for success:**

- 100% pass rate (failures are learning opportunities)
- Fixing all issues immediately (user can defer fixes)
- Running all 3 test modes (one mode is sufficient for validation)

## Integration Points

**Invoked by:**

- `/test [PluginName]` command
- `/test [PluginName] build` → Direct to mode 2
- `/test [PluginName] quick` → Direct to mode 1
- `/test [PluginName] manual` → Direct to mode 3
- `plugin-workflow` skill → After Stages 4, 5, 6
- `plugin-improve` skill → After implementing changes
- Natural language: "Test [PluginName]", "Run validation on [PluginName]"

**Invokes (future):**

- `deep-research` skill (Phase 7) → When user chooses "Investigate failures"

**Creates:**

- Test logs in `logs/[PluginName]/test_[timestamp].log`
- Build artifacts in `build/plugins/[PluginName]/`

**Updates:**

- `.continue-here.md` → Testing checkpoint
- `PLUGINS.md` → Test status

**Blocks:**

- Installation (`/install-plugin`) → Recommends testing first if not done recently

## Reference Documentation

- **Test specifications:** `references/test-specifications.md` - Detailed implementation of 5 automated tests
- **Pluginval guide:** `references/pluginval-guide.md` - Industry-standard validation details
- **Manual testing guide:** `references/manual-testing-guide.md` - DAW testing methodology
- **Troubleshooting:** `references/troubleshooting.md` - Common issues and fixes

## Template Assets

- **Manual testing checklist:** `assets/manual-testing-checklist.md` - DAW testing template
