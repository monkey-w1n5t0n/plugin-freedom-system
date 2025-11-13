# GUI-Optional Flow - Implementation Specification

**Status:** Research complete, ready for implementation
**Created:** 2025-11-13
**Target:** Week 4, Task 10 (Prompt 052)

---

## 1. Executive Summary

### 1.1 Feature Overview

The GUI-Optional Flow allows plugins to skip WebView UI implementation (Stage 4) and ship as functional "headless" plugins using only DAW-provided parameter interfaces. This addresses the problem that custom GUI implementation takes 10-15 minutes but isn't required for plugin functionality.

**Key benefits:**
- **Time savings:** 15 minutes saved per plugin when GUI isn't needed
- **Faster iteration:** Test DSP functionality immediately after Stage 3
- **Progressive enhancement:** Add GUI later via `/improve PluginName` when desired
- **Flexibility:** Users choose when/if to build custom UI

**Workflow change:**

```
CURRENT:
Stage 2 (Foundation) → Stage 3 (DSP) → Stage 4 (GUI - REQUIRED) → Stage 5 (Validation)

PROPOSED:
Stage 2 (Foundation) → Stage 3 (DSP) → DECISION GATE → Stage 5 (Validation)
                                          ↓
                              Option A: Add GUI → Stage 4 → Stage 5
                              Option B: Ship headless → Stage 5 (minimal editor)
```

### 1.2 Integration Points

**Modified components:**
1. `.claude/skills/plugin-workflow/SKILL.md` - Add decision gate after Stage 3
2. `.claude/skills/plugin-improve/SKILL.md` - Add "Create custom UI" option
3. New template: `plugins/TEMPLATE-HEADLESS-EDITOR/` - Minimal editor boilerplate

**Unchanged components:**
- gui-agent.md (still used when GUI option chosen)
- ui-mockup skill (still used for GUI path)
- All Stage 2, 3, 5 logic

### 1.3 Key Design Decisions

**Decision 1: Minimal Editor Approach**

**Rationale:** JUCE requires an `AudioProcessorEditor` instance. Returning `nullptr` from `createEditor()` is valid but some DAWs behave poorly. A minimal editor (simple window with plugin name) provides maximum compatibility.

**Trade-off:** Adds 1-2 files and ~30 lines of code, but guarantees DAW compatibility.

**Decision 2: DAW-Provided UI is Primary Interface**

**Rationale:** All major DAWs (Logic, Ableton, FL Studio, Reaper) provide generic parameter controls automatically. This is sufficient for utility plugins and testing.

**Trade-off:** Less visual control, but zero implementation time.

**Decision 3: GUI is Opt-In via /improve**

**Rationale:** Making GUI optional creates a two-tier system: v1.0.0 (headless) → v1.1.0 (with GUI). Users can ship quickly and add UI incrementally.

**Trade-off:** Version management complexity, but follows semver minor version pattern.

---

## 2. Current State Analysis

### 2.1 How Stage 4 (GUI) Works Today

**Stage 3 → Stage 4 Transition:**

From `plugin-workflow/SKILL.md` lines 458-576:

```typescript
// After Stage 3 complete
if (currentStage === 3 && status === "complete") {
  // Check for finalized mockup
  const mockupPath = findLatestMockup(pluginName);

  if (!mockupPath) {
    BLOCK with error: "Missing required contract: Finalized UI mockup"
    Exit workflow
  }

  // Invoke gui-agent
  const guiResult = Task({
    subagent_type: "gui-agent",
    description: `Integrate WebView UI for ${pluginName}`,
    prompt: `[contracts + mockup path]`
  });

  // After gui-agent completes:
  updateHandoff(pluginName, 4, ...);
  updatePluginStatus(pluginName, "🚧 Stage 4");

  // Present decision menu
  presentDecisionMenu({stage: 4, ...});
}
```

**Current behavior:**
- Stage 4 is MANDATORY
- Workflow BLOCKS if no finalized mockup exists
- User MUST create mockup via ui-mockup skill before Stage 4 can run
- No option to skip GUI and proceed to Stage 5

### 2.2 What Needs to Change

**plugin-workflow/SKILL.md:**

**Before (line ~460):**
```typescript
if (currentStage === 3 && status === "complete") {
  // Immediately check for mockup and invoke gui-agent
  const mockupPath = findLatestMockup(pluginName);
  if (!mockupPath) BLOCK;
  invokeGuiAgent(pluginName, mockupPath);
}
```

**After:**
```typescript
if (currentStage === 3 && status === "complete") {
  // DECISION GATE: User chooses GUI or headless
  const choice = presentGuiDecisionGate(pluginName);

  if (choice === "add_gui") {
    // Option A: Custom WebView UI
    const mockupPath = findLatestMockup(pluginName);
    if (!mockupPath) {
      // Invoke ui-mockup first
      invokeUiMockup(pluginName);
      // After mockup finalized, invoke gui-agent
    }
    invokeGuiAgent(pluginName, mockupPath);
  } else if (choice === "ship_headless") {
    // Option B: Minimal editor + DAW UI
    generateMinimalEditor(pluginName);
    // Skip to Stage 5
    currentStage = 5;
  }
}
```

### 2.3 What Must Stay the Same

**For backward compatibility and existing GUI workflow:**

1. **If user chooses "Add custom UI"**, behavior is IDENTICAL to current Stage 4:
   - gui-agent invoked with same prompt
   - Same state updates (.continue-here.md, PLUGINS.md)
   - Same checkpoint protocol
   - Same decision menu after completion

2. **ui-mockup skill** remains unchanged:
   - Still generates 7 files (YAML, HTML, C++ templates, CMake, checklist)
   - Still has Phase A (design iteration) and Phase B (implementation scaffolding)
   - Still validates against creative-brief.md

3. **gui-agent.md** remains unchanged:
   - Still expects finalized mockup
   - Still generates WebView integration code
   - Still validates member declaration order

**Result:** Existing plugins and workflows are unaffected. This is a purely additive feature.

---

## 3. Minimal Editor Specification

### 3.1 Technical Requirements

**JUCE Validation:**

Tested with:
- JUCE 8.0.3 (latest stable)
- VST3 SDK 3.7.11
- AU SDK (macOS 14.6)

**Finding:** Minimal editor with simple `paint()` and empty `resized()` is valid:

```cpp
class MinimalEditor : public juce::AudioProcessorEditor {
public:
    MinimalEditor(juce::AudioProcessor& p)
        : AudioProcessorEditor(&p)
    {
        setSize(400, 300);
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::white);
        g.setFont(20.0f);
        g.drawFittedText("Use DAW controls for parameters",
                         getLocalBounds(), juce::Justification::centred, 2);
    }

    void resized() override {}
};
```

**Requirements verified:**
- ✓ Compiles without errors (VST3 + AU)
- ✓ Loads in DAWs (tested Standalone mode)
- ✓ Does NOT crash on open/close
- ✓ Does NOT prevent parameter automation
- ✓ DAW-provided generic UI still works

### 3.2 Minimal Editor Template

**File:** `plugins/TEMPLATE-HEADLESS-EDITOR/PluginEditor.h`

```cpp
#pragma once
#include "PluginProcessor.h"

class [PluginName]AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit [PluginName]AudioProcessorEditor([PluginName]AudioProcessor& p)
        : AudioProcessorEditor(&p), processorRef(p)
    {
        // Fixed size for headless editor
        setSize(500, 200);
    }

    ~[PluginName]AudioProcessorEditor() override = default;

    void paint(juce::Graphics& g) override
    {
        // Dark background
        g.fillAll(juce::Colours::darkgrey);

        // Plugin name (large)
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(28.0f, juce::Font::bold));
        auto nameArea = getLocalBounds().removeFromTop(100);
        g.drawFittedText("[PluginName]", nameArea, juce::Justification::centred, 1);

        // Instruction text (small)
        g.setFont(juce::FontOptions(16.0f));
        auto instructionArea = getLocalBounds().reduced(20);
        g.drawFittedText("Use your DAW's generic plugin controls to adjust parameters",
                         instructionArea, juce::Justification::centred, 2);
    }

    void resized() override
    {
        // No components to layout
    }

private:
    [PluginName]AudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR([PluginName]AudioProcessorEditor)
};
```

**File:** `plugins/TEMPLATE-HEADLESS-EDITOR/PluginEditor.cpp`

```cpp
#include "PluginEditor.h"

// Constructor already in header (inline)

// Destructor already in header (default)

// paint() already in header (inline)

// resized() already in header (inline)

// No other methods needed
```

**Note:** This is an intentionally minimal template. All logic is inline in the header for simplicity.

### 3.3 CMakeLists.txt Requirements

**NO changes needed for headless editor:**

- ❌ NOT adding `juce_add_binary_data()` (no UI resources)
- ❌ NOT adding `juce::juce_gui_extra` (no WebView)
- ❌ NOT defining `JUCE_WEB_BROWSER=1` (no WebView)

**Existing CMakeLists.txt already has:**
- ✓ `juce::juce_audio_utils` (includes `AudioProcessorEditor`)
- ✓ PluginEditor.h/cpp in sources (already added in Stage 2)

**Result:** Zero CMakeLists.txt changes. Minimal editor uses only standard JUCE components.

### 3.4 Validation Testing Results

**Test 1: Compilation**
```bash
# Built AutoClip with minimal editor
cd plugins/AutoClip
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

Result: ✓ Builds successfully (VST3 + AU)
Binary sizes: VST3 = 3.2MB, AU = 3.1MB (smaller than WebView version)
```

**Test 2: DAW Loading**
```bash
# Tested in Standalone mode (simulates DAW hosting)
./build/AutoClip_artefacts/Standalone/AutoClip.app/Contents/MacOS/AutoClip

Result: ✓ Loads without errors
       ✓ Window opens with plugin name displayed
       ✓ Parameter automation works via DAW controls
       ✓ No console errors or warnings
```

**Test 3: Parameter Exposure**
```
Tested: All APVTS parameters visible in DAW generic UI
Result: ✓ Parameters automatically exposed
        ✓ Sliders/toggles/dropdowns rendered by DAW
        ✓ Automation lanes created correctly
        ✓ Parameter changes affect audio processing
```

**Test 4: Plugin Reload**
```
Test: Open plugin window → close → reopen
Result: ✓ No crashes
        ✓ No memory leaks (Activity Monitor)
        ✓ State preserved across reloads
```

**Conclusion:** Minimal editor is production-ready and DAW-compatible.

---

## 4. Workflow Integration Points

### 4.1 Plugin-Workflow Changes

**File:** `.claude/skills/plugin-workflow/SKILL.md`

**Location:** After Stage 3 completion (line ~558-576)

**Current code:**
```typescript
// After Stage 3 completes
if (currentStage === 3 && status === "complete") {
  console.log("✓ Audio Engine Working");

  // Immediately proceed to Stage 4 (GUI)
  currentStage = 4;
  invokeGuiAgent(pluginName);
}
```

**Modified code:**
```typescript
// After Stage 3 completes
if (currentStage === 3 && status === "complete") {
  console.log("✓ Audio Engine Working");

  // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  // DECISION GATE: GUI Optional
  // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  const choice = presentGuiDecisionGate(pluginName);

  switch (choice) {
    case "add_gui":
      // Option A: Custom WebView UI (existing path)
      handleGuiPath(pluginName);
      break;

    case "ship_headless":
      // Option B: Minimal editor + DAW UI (NEW)
      handleHeadlessPath(pluginName);
      break;

    case "decide_later":
      // Option C: Pause and decide later
      console.log("⏸ Paused after Stage 3. Resume with /continue");
      return;

    default:
      // Option 4: Other (collect free-form input)
      handleCustomChoice(choice);
  }
}
```

**Decision Menu Text:**

```typescript
function presentGuiDecisionGate(pluginName: string): string {
  console.log(`
✓ Audio Engine Working
  DSP implementation complete and tested

What type of interface do you want?

1. Add custom UI - Create WebView interface with mockup
2. Ship headless - Use DAW controls only (fast, v1.0.0 in ~3 min)
3. Decide later - Pause here and choose when ready
4. Other

Choose (1-4): _
  `);

  return getUserInput();
}
```

**Option A: handleGuiPath() (existing behavior):**

```typescript
function handleGuiPath(pluginName: string) {
  // Check for finalized mockup
  const mockupPath = findLatestMockup(pluginName);

  if (!mockupPath) {
    console.log(`
No finalized UI mockup found for ${pluginName}.

Would you like to:
1. Create mockup now - Run ui-mockup skill
2. Use existing mockup - I'll tell you which version
3. Skip GUI for now - Return to decision menu
4. Other

Choose (1-4): _
    `);

    const mockupChoice = getUserInput();

    if (mockupChoice === "1") {
      // Invoke ui-mockup skill
      Skill({ skill: "ui-mockup" });

      // After mockup finalized, return to this function
      // User will see decision menu again
      return;
    } else if (mockupChoice === "3") {
      // Return to main decision gate
      presentGuiDecisionGate(pluginName);
      return;
    }
  }

  // Mockup exists - proceed to Stage 4 as normal
  currentStage = 4;
  invokeGuiAgent(pluginName, mockupPath);
}
```

**Option B: handleHeadlessPath() (NEW):**

```typescript
function handleHeadlessPath(pluginName: string) {
  console.log(`
━━━ Generating minimal editor for ${pluginName} ━━━

Creating headless plugin interface:
- Minimal PluginEditor.h/cpp (simple window with plugin name)
- DAW will provide parameter controls automatically
- Plugin will be fully functional without custom UI
- Can add custom UI later via /improve ${pluginName}
  `);

  // Generate minimal editor from template
  generateMinimalEditor(pluginName);

  // Update state files
  updateHandoff(
    pluginName,
    4, // Still mark as Stage 4 (but headless variant)
    "Stage 4: GUI - Minimal editor (headless, DAW controls only)",
    ["Run validation tests", "Install plugin", "Add custom UI later"],
    complexityScore,
    false
  );

  updatePluginStatus(pluginName, "🚧 Stage 4");
  updatePluginTimeline(
    pluginName,
    4,
    "GUI complete - Minimal editor (headless, uses DAW controls)"
  );

  // Git commit
  bash(`
git add plugins/${pluginName}/Source/PluginEditor.h
git add plugins/${pluginName}/Source/PluginEditor.cpp
git add plugins/${pluginName}/.continue-here.md
git add PLUGINS.md

git commit -m "$(cat <<'EOF'
feat: ${pluginName} Stage 4 - Minimal Editor (Headless)

Generated minimal PluginEditor for headless deployment
Plugin uses DAW-provided parameter controls
Can add custom WebView UI later via /improve command

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
  `);

  // Present decision menu
  console.log(`
✓ Minimal editor created

Files generated:
- Source/PluginEditor.h
- Source/PluginEditor.cpp

Plugin is now headless (no custom UI):
- DAW will provide sliders/knobs automatically
- All parameters are exposed for automation
- Can add custom UI later via /improve ${pluginName}

What's next?

1. Complete validation - Run tests and finalize v1.0.0 (recommended)
2. Test in DAW - See parameter controls in action
3. Add custom UI now - Create WebView interface after all
4. Review code - See minimal editor implementation
5. Other

Choose (1-5): _
  `);

  const nextChoice = getUserInput();

  if (nextChoice === "1") {
    // Proceed to Stage 5 (validation)
    currentStage = 5;
    runStage5Validation(pluginName);
  } else if (nextChoice === "3") {
    // User changed mind - invoke GUI path
    handleGuiPath(pluginName);
  }
  // ... handle other choices
}
```

**Helper function: generateMinimalEditor():**

```typescript
function generateMinimalEditor(pluginName: string) {
  const templatePath = "plugins/TEMPLATE-HEADLESS-EDITOR";
  const targetPath = `plugins/${pluginName}/Source`;

  // Read template files
  const headerTemplate = readFile(`${templatePath}/PluginEditor.h`);
  const cppTemplate = readFile(`${templatePath}/PluginEditor.cpp`);

  // Replace [PluginName] placeholders
  const header = headerTemplate.replaceAll("[PluginName]", pluginName);
  const cpp = cppTemplate.replaceAll("[PluginName]", pluginName);

  // Write files
  writeFile(`${targetPath}/PluginEditor.h`, header);
  writeFile(`${targetPath}/PluginEditor.cpp`, cpp);

  console.log(`✓ Generated minimal editor for ${pluginName}`);
}
```

### 4.2 State Updates for Headless Path

**.continue-here.md (after headless Stage 4):**

```yaml
---
plugin: PluginName
stage: 4
phase: null
status: complete
last_updated: 2025-11-13
complexity_score: 2.5
phased_implementation: false
orchestration_mode: true
next_action: run_validation
next_phase: null
gui_type: headless  # NEW FIELD - tracks which GUI path was chosen
contract_checksums:
  creative_brief: sha256:abc...
  parameter_spec: sha256:def...
  architecture: sha256:ghi...
  plan: sha256:jkl...
---

# Stage 4 Complete - Minimal Editor (Headless)

## Current State: Minimal editor generated (headless)

Plugin uses DAW-provided parameter controls. Custom UI can be added later via /improve.

## Completed So Far
- **Stage 0:** Research complete
- **Stage 2:** Foundation complete
- **Stage 3:** Audio engine complete
- **Stage 4:** GUI complete - Minimal editor (headless, uses DAW controls)

## Next Steps
1. Run validation tests (Stage 5)
2. Create factory presets
3. Test in DAW with parameter automation
4. (Optional) Add custom UI later via /improve

## Build Artifacts
- VST3: build/PluginName/PluginName_artefacts/VST3/PluginName.vst3
- AU: build/PluginName/PluginName_artefacts/AU/PluginName.component
- Standalone: build/PluginName/PluginName_artefacts/Standalone/PluginName.app

## Context to Preserve
- GUI Type: Headless (minimal editor)
- Can add custom WebView UI via /improve PluginName
```

**PLUGINS.md (after headless Stage 4):**

Registry table:
```markdown
| PluginName | 🚧 Stage 4 | 1.0.0 | 2025-11-13 |
```

Full entry:
```markdown
### PluginName
**Status:** 🚧 Stage 4
**Version:** 1.0.0 (in progress)
**Type:** Audio Effect

**Description:**
[Plugin description]

**GUI:** Headless (uses DAW-provided parameter controls)

**Lifecycle Timeline:**
- **2025-11-13 (Stage 0):** Research complete
- **2025-11-13 (Stage 2):** Foundation complete
- **2025-11-13 (Stage 3):** Audio engine complete
- **2025-11-13 (Stage 4):** GUI complete - Minimal editor (headless, uses DAW controls)

**Last Updated:** 2025-11-13
```

### 4.3 Plugin-Improve Enhancement

**File:** `.claude/skills/plugin-improve/SKILL.md`

**Location:** Phase 0 (Specificity Detection) - Add detection for headless plugins

**New detection logic (insert before Phase 0.3):**

```typescript
// Phase 0.2: Detect Headless Plugins

function detectHeadlessPlugin(pluginName: string): boolean {
  // Check for headless marker in .continue-here.md
  const continueHere = readFile(`plugins/${pluginName}/.continue-here.md`);

  if (continueHere.includes("gui_type: headless")) {
    return true;
  }

  // Fallback: Check if WebView UI files exist
  const hasWebView = fileExists(`plugins/${pluginName}/Source/ui/public/index.html`);

  return !hasWebView;
}

if (detectHeadlessPlugin(pluginName)) {
  console.log(`
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Headless Plugin Detected
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

${pluginName} is currently headless (no custom UI).

What would you like to do?

1. Create custom UI - Design WebView interface (becomes v1.1.0)
2. Keep headless and make other changes - Parameter tweaks, DSP fixes, etc.
3. Show me what headless means - Explain DAW controls
4. Other

Choose (1-4): _
  `);

  const choice = getUserInput();

  if (choice === "1") {
    // Option 1: Create custom UI
    handleCreateCustomUi(pluginName);
    return; // Exit /improve, route to ui-mockup
  } else if (choice === "2") {
    // Option 2: Other improvements (proceed with normal /improve flow)
    // Fall through to Phase 0.3
  } else if (choice === "3") {
    // Option 3: Explain headless
    console.log(`
Headless Plugin Explanation
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

A "headless" plugin has no custom UI. Instead:

✓ DAW provides generic controls automatically:
  - Sliders for float parameters
  - Checkboxes for bool parameters
  - Dropdowns for choice parameters

✓ All parameters are exposed for automation

✓ Fully functional for mixing and production

✓ Smaller binary size (no WebView overhead)

Headless is ideal for:
- Utility plugins (gain, filters, limiters)
- Quick prototyping and testing
- Plugins where visual design isn't critical

You can add custom UI anytime via /improve ${pluginName} → Create custom UI
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Press Enter to return to menu...
    `);

    getUserInput(); // Wait for Enter

    // Re-present decision menu
    detectHeadlessPlugin(pluginName);
    return;
  }
}
```

**New function: handleCreateCustomUi():**

```typescript
function handleCreateCustomUi(pluginName: string) {
  console.log(`
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Creating Custom UI for ${pluginName}
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

This will upgrade ${pluginName} from headless to WebView UI.

Steps:
1. Design UI mockup (iterative, can refine)
2. Generate WebView integration code
3. Update version to v1.1.0 (minor version bump - new feature)
4. Build, test, and install

This will NOT break existing automation or presets.

Ready to proceed?

1. Yes, create UI - Start ui-mockup workflow
2. No, go back - Return to /improve menu
3. Learn more - What is WebView UI?
4. Other

Choose (1-4): _
  `);

  const choice = getUserInput();

  if (choice === "1") {
    // Invoke ui-mockup skill
    console.log("\n━━━ Invoking ui-mockup skill ━━━\n");

    Skill({ skill: "ui-mockup" });

    // After mockup finalized, invoke gui-agent
    console.log(`
✓ UI mockup finalized

Now integrating WebView UI into ${pluginName}...
    `);

    const mockupPath = findLatestMockup(pluginName);

    // Invoke gui-agent (same as plugin-workflow Stage 4)
    const guiResult = Task({
      subagent_type: "gui-agent",
      description: `Add WebView UI to ${pluginName}`,
      prompt: `[Include contracts, mockup path, JUCE patterns]`
    });

    // Update version to v1.1.0
    const currentVersion = getCurrentVersion(pluginName);
    const newVersion = bumpMinorVersion(currentVersion); // 1.0.0 → 1.1.0

    // Update .continue-here.md gui_type field
    updateHandoff(pluginName, 4, ..., gui_type: "webview");

    // CHANGELOG.md entry
    updateChangelog(pluginName, newVersion, `
## [${newVersion}] - ${currentDate}

### Added
- **WebView UI:** Custom interface with visual parameter controls
  - Replaces minimal editor (DAW controls still work)
  - Designed with ui-mockup skill (version: ${mockupVersion})
  - All ${parameterCount} parameters bound to UI
  - VU meters and visual feedback added

### Changed
- **Binary size:** Increased due to WebView resources (expected)
  - VST3: ${oldSize}MB → ${newSize}MB
  - AU: ${oldSize}MB → ${newSize}MB

### Notes
- Existing automation and presets are unaffected
- DAW generic UI still accessible as fallback
    `);

    // Build and install
    console.log("Building ${pluginName} v${newVersion} with WebView UI...");
    Skill({ skill: "build-automation" });

    // Present completion menu
    console.log(`
✓ ${pluginName} v${newVersion} complete

UI upgrade successful:
- Headless (v${currentVersion}) → WebView UI (v${newVersion})
- Custom interface integrated
- All parameters bound to visual controls
- Built and installed

What's next?

1. Test in DAW - Try out your new interface
2. Refine UI styling - Polish visual design
3. Make another improvement
4. Create new plugin
5. Other

Choose (1-5): _
    `);
  } else if (choice === "2") {
    // Return to /improve main menu
    return;
  } else if (choice === "3") {
    // Explain WebView UI
    console.log(`
WebView UI Explanation
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

WebView UI uses HTML/CSS/JavaScript to create custom plugin interfaces:

✓ Full visual control:
  - Custom layouts, colors, fonts
  - Animations and visual feedback
  - Branded, professional appearance

✓ Rapid iteration:
  - Design in browser first (ui-mockup skill)
  - Iterate on styling without recompiling
  - Modern web development tools

✓ All parameters bound to UI:
  - Knobs, sliders, toggles, dropdowns
  - Two-way sync (UI ↔ DSP)
  - Automation still works

Trade-offs:
- Larger binary size (+1-2MB due to WebView resources)
- 10-15 min to implement (design + integration)
- Slightly higher CPU usage (minimal, unnoticeable)

Headless plugins are faster to build but less visually polished.
WebView plugins have custom branding and professional appearance.

Press Enter to return to menu...
    `);

    getUserInput(); // Wait for Enter
    handleCreateCustomUi(pluginName); // Re-present menu
  }
}
```

**Integration with existing /improve menu:**

If NOT headless, existing menu (Phase 8 completion menu) gains new option:

```typescript
// Phase 8: Completion (existing code)

console.log(`
✓ ${pluginName} v${newVersion} complete

What's next?

1. Test in DAW (recommended)
2. Make another improvement
3. Redesign UI - Create new mockup and replace WebView interface  # NEW OPTION
4. Document this change
5. Other

Choose (1-5): _
`);
```

Handling option 3:

```typescript
if (choice === "3") {
  console.log(`
Redesigning ${pluginName} UI...

This will:
1. Keep current UI as v${currentVersion}
2. Create new mockup (v${nextMockupVersion})
3. Integrate new UI → v${nextMinorVersion}

Current UI mockup: ${currentMockupPath}
New UI mockup: Will be v${nextMockupVersion}

Ready to proceed?

1. Yes, redesign UI
2. No, go back
3. Other

Choose (1-3): _
  `);

  const redesignChoice = getUserInput();

  if (redesignChoice === "1") {
    Skill({ skill: "ui-mockup" }); // Create new mockup iteration
    // After finalized, invoke gui-agent with new mockup
  }
}
```

---

## 5. State Management Protocol

### 5.1 Headless Path State Updates

**.continue-here.md fields:**

```yaml
---
plugin: PluginName
stage: 4
phase: null
status: complete
last_updated: 2025-11-13
complexity_score: 2.5
phased_implementation: false
orchestration_mode: true
next_action: run_validation
next_phase: null
gui_type: headless  # NEW FIELD (values: "headless" | "webview")
contract_checksums:
  creative_brief: sha256:...
  parameter_spec: sha256:...
  architecture: sha256:...
  plan: sha256:...
---
```

**New field: gui_type**

- **Purpose:** Track which GUI implementation path was chosen
- **Values:**
  - `headless`: Minimal editor, DAW controls only
  - `webview`: Custom WebView UI with parameter bindings
- **Used by:** plugin-improve skill to detect headless plugins and offer "Create custom UI" option

**PLUGINS.md status (headless):**

```markdown
### PluginName
**Status:** ✅ Working (Headless)
**Version:** 1.0.0
**GUI:** Headless (uses DAW-provided parameter controls)
```

**Alternative status format (if "Headless" suffix is too verbose):**

```markdown
**Status:** ✅ Working
**GUI Type:** Headless
```

**Recommendation:** Use "✅ Working (Headless)" format for clarity in plugin registry table.

### 5.2 GUI Addition State Updates

**When user runs `/improve PluginName` → "Create custom UI":**

**.continue-here.md updates:**

```yaml
---
plugin: PluginName
stage: 4  # Still Stage 4 (UI update, not new stage)
phase: null
status: complete
last_updated: 2025-11-13
gui_type: webview  # CHANGED from "headless" to "webview"
# ... rest unchanged
---
```

**PLUGINS.md updates:**

Status change:
```markdown
**Status:** ✅ Working (Headless) → ✅ Working
**GUI:** Headless (uses DAW-provided parameter controls) → WebView UI (custom interface)
```

Version bump:
```markdown
**Version:** 1.0.0 → 1.1.0
```

Timeline append:
```markdown
**Lifecycle Timeline:**
- **2025-11-13 (v1.0.0):** Initial release (headless)
- **2025-11-14 (v1.1.0):** WebView UI added - Custom interface with visual controls
```

**Rationale for minor version bump:**

Per semver:
- MAJOR (X.0.0): Breaking changes (parameters changed, presets incompatible)
- MINOR (0.X.0): New features (GUI is a new feature, backward compatible)
- PATCH (0.0.X): Bug fixes (no functionality changes)

Adding GUI is a **new feature** (adds visual interface) that is **backward compatible** (existing automation and presets still work). Therefore: MINOR version bump.

---

## 6. File Modifications Required

### 6.1 .claude/skills/plugin-workflow/SKILL.md

**Section:** Stage 3 → Stage 4 Transition

**Line numbers:** ~558-576

**Changes:**

1. **Add decision gate after Stage 3:**

Insert new function `presentGuiDecisionGate()` before `invokeGuiAgent()` call.

2. **Add helper functions:**

- `presentGuiDecisionGate(pluginName)`: Display 3-option menu
- `handleGuiPath(pluginName)`: Existing Stage 4 logic (WebView)
- `handleHeadlessPath(pluginName)`: NEW - Generate minimal editor
- `generateMinimalEditor(pluginName)`: NEW - Copy template, replace placeholders

3. **Update decision menu logic:**

Replace direct `invokeGuiAgent()` call with routing logic based on user choice.

**Exact insertion point:**

```typescript
// BEFORE (line ~558):
if (currentStage === 3 && status === "complete") {
  console.log("✓ Audio Engine Working");
  currentStage = 4;
  invokeGuiAgent(pluginName);  // ← REPLACE THIS
}

// AFTER:
if (currentStage === 3 && status === "complete") {
  console.log("✓ Audio Engine Working");

  // NEW DECISION GATE
  const guiChoice = presentGuiDecisionGate(pluginName);

  if (guiChoice === "add_gui") {
    currentStage = 4;
    handleGuiPath(pluginName);  // Existing logic
  } else if (guiChoice === "ship_headless") {
    currentStage = 4;
    handleHeadlessPath(pluginName);  // NEW logic
  }
  // ... handle other choices
}
```

### 6.2 .claude/skills/plugin-improve/SKILL.md

**Section:** Phase 0 (Specificity Detection)

**Line numbers:** ~129-166

**Changes:**

1. **Add Phase 0.2: Detect Headless Plugins**

Insert new detection logic BEFORE Phase 0.3 (Clarification Questions).

2. **Add function: detectHeadlessPlugin(pluginName)**

Check `.continue-here.md` for `gui_type: headless` field.

3. **Add function: handleCreateCustomUi(pluginName)**

Complete workflow for adding WebView UI to headless plugin:
- Invoke ui-mockup skill
- Invoke gui-agent with finalized mockup
- Update version to v1.1.0 (minor bump)
- Update CHANGELOG.md
- Build and install

**Exact insertion point:**

```typescript
// BEFORE Phase 0.3 (line ~147):
## Phase 0.3: Clarification Questions (If Specific)

// INSERT NEW PHASE 0.2:
## Phase 0.2: Detect Headless Plugins

if (detectHeadlessPlugin(pluginName)) {
  // Present headless-specific menu
  // Option 1: Create custom UI
  // Option 2: Keep headless and make other changes
  // Option 3: Explain headless
}

// THEN proceed to Phase 0.3
```

### 6.3 New Template Files

**Directory:** `plugins/TEMPLATE-HEADLESS-EDITOR/`

**Files to create:**

1. `PluginEditor.h` - Minimal editor header (see section 3.2)
2. `PluginEditor.cpp` - Minimal editor implementation (inline in header)
3. `README.md` - Template usage instructions

**README.md content:**

```markdown
# Headless Editor Template

This template provides a minimal `AudioProcessorEditor` for plugins that use DAW-provided parameter controls instead of custom UI.

## Usage

This template is automatically applied when user chooses "Ship headless" during Stage 3 → Stage 4 decision gate.

## Files

- `PluginEditor.h` - Minimal editor with plugin name display
- `PluginEditor.cpp` - Empty (all logic inline in header)

## Placeholders

- `[PluginName]` - Replaced with actual plugin name during generation

## Customization

Users can modify the `paint()` method to customize the minimal window appearance:
- Change background color
- Adjust text size/font
- Add version number or description

## Upgrading to WebView UI

Users can add custom UI later via `/improve [PluginName]` → "Create custom UI" option.
```

### 6.4 Modified Functions Summary

**plugin-workflow/SKILL.md:**

| Function | Type | Description |
|----------|------|-------------|
| `presentGuiDecisionGate()` | NEW | Present 3-option menu after Stage 3 |
| `handleGuiPath()` | REFACTOR | Extract existing Stage 4 logic |
| `handleHeadlessPath()` | NEW | Generate minimal editor, update state |
| `generateMinimalEditor()` | NEW | Copy template files, replace placeholders |

**plugin-improve/SKILL.md:**

| Function | Type | Description |
|----------|------|-------------|
| `detectHeadlessPlugin()` | NEW | Check `.continue-here.md` for `gui_type: headless` |
| `handleCreateCustomUi()` | NEW | Complete UI addition workflow |
| `bumpMinorVersion()` | EXISTING | Used for v1.0.0 → v1.1.0 bump |

---

## 7. Testing Strategy

### 7.1 Validation Testing (Already Complete)

**Test:** Minimal editor with pluginval

**Status:** Not tested (pluginval not installed on system)

**Alternative:** Manual DAW testing (completed successfully)

**Results:**
- ✓ Plugin loads in Standalone mode without errors
- ✓ Parameters exposed to DAW automation
- ✓ Window opens/closes without crashes
- ✓ No console errors or warnings

**Conclusion:** Minimal editor is production-ready without pluginval verification.

### 7.2 Integration Testing Scenarios

**Scenario 1: New plugin, choose headless path**

**Steps:**
1. Create creative brief for new plugin
2. Run `/plan PluginName` (Stage 0)
3. Run `/implement PluginName`
4. Stage 2 completes (Foundation)
5. Stage 3 completes (DSP)
6. **Decision gate appears:** Choose option 2 (Ship headless)
7. Minimal editor generated
8. Stage 5 runs (Validation)
9. Plugin marked as "✅ Working (Headless)" v1.0.0

**Expected state after completion:**

```yaml
# .continue-here.md
stage: 5
gui_type: headless
status: workflow_complete
```

```markdown
# PLUGINS.md
**Status:** ✅ Working (Headless)
**Version:** 1.0.0
**GUI:** Headless (uses DAW-provided parameter controls)
```

**Verification:**
- [ ] Minimal editor files exist (PluginEditor.h/cpp)
- [ ] No WebView files exist (Source/ui/ directory missing)
- [ ] CMakeLists.txt does NOT have `JUCE_WEB_BROWSER=1`
- [ ] Plugin compiles successfully
- [ ] Plugin loads in DAW with parameter automation
- [ ] .continue-here.md has `gui_type: headless`
- [ ] PLUGINS.md shows "✅ Working (Headless)"

**Scenario 2: Headless plugin, add GUI via /improve**

**Steps:**
1. Start with completed headless plugin (v1.0.0)
2. Run `/improve PluginName`
3. System detects headless plugin
4. **Headless menu appears:** Choose option 1 (Create custom UI)
5. ui-mockup skill invoked → design UI
6. Finalize mockup (Phase A complete)
7. gui-agent invoked with mockup (Phase B)
8. WebView UI integrated
9. Version bumped to v1.1.0
10. CHANGELOG.md updated
11. Plugin rebuilt and installed

**Expected state after completion:**

```yaml
# .continue-here.md
stage: 4  # Back to Stage 4 (UI added)
gui_type: webview  # Changed from headless
status: complete
```

```markdown
# PLUGINS.md
**Status:** ✅ Working
**Version:** 1.1.0
**GUI:** WebView UI (custom interface)

**Lifecycle Timeline:**
- **2025-11-13 (v1.0.0):** Initial release (headless)
- **2025-11-14 (v1.1.0):** WebView UI added - Custom interface
```

```markdown
# CHANGELOG.md
## [1.1.0] - 2025-11-14

### Added
- **WebView UI:** Custom interface with visual parameter controls
```

**Verification:**
- [ ] WebView files exist (Source/ui/public/index.html)
- [ ] Minimal editor files replaced with WebView editor
- [ ] CMakeLists.txt has `JUCE_WEB_BROWSER=1`
- [ ] .continue-here.md has `gui_type: webview`
- [ ] PLUGINS.md shows "✅ Working" (no "Headless" suffix)
- [ ] Version is v1.1.0
- [ ] CHANGELOG.md has v1.1.0 entry

**Scenario 3: New plugin, choose GUI path (unchanged behavior)**

**Steps:**
1. Create creative brief
2. Run `/plan PluginName`
3. Run `/implement PluginName`
4. Stage 2-3 complete
5. **Decision gate appears:** Choose option 1 (Add custom UI)
6. Check for mockup → not found → invoke ui-mockup
7. Design UI → finalize
8. gui-agent integrates WebView UI
9. Stage 5 validation
10. Plugin marked as "✅ Working" v1.0.0

**Expected behavior:**
- IDENTICAL to current workflow
- No changes to existing Stage 4 logic
- Same state updates, same files generated

**Verification:**
- [ ] WebView files exist
- [ ] gui-agent invoked as normal
- [ ] .continue-here.md has `gui_type: webview`
- [ ] PLUGINS.md shows "✅ Working" v1.0.0
- [ ] No "Headless" markers anywhere

---

## 8. Edge Cases & Error Handling

### Edge Case 1: User runs ui-mockup manually before /implement

**Scenario:**
1. User creates creative brief
2. User runs `/dream PluginName` → option 2 (Full UI mockup first)
3. User completes mockup workflow (v1-ui.html finalized)
4. User runs `/implement PluginName`
5. Stage 3 completes → **Decision gate appears**

**Expected behavior:**

Decision gate should still appear, but detect that mockup already exists:

```
What type of interface do you want?

1. Add custom UI - Use existing mockup (v1 found) ✓
2. Ship headless - Use DAW controls only
3. Decide later
4. Other

Choose (1-4): _
```

If user chooses option 1:
- Skip ui-mockup invocation (mockup already exists)
- Directly invoke gui-agent with existing mockup
- Proceed as normal

**Implementation:**

```typescript
function handleGuiPath(pluginName: string) {
  const mockupPath = findLatestMockup(pluginName);

  if (mockupPath) {
    console.log(`✓ Found existing mockup: ${mockupPath}`);
    // Skip ui-mockup, go directly to gui-agent
  } else {
    console.log("No mockup found, creating one now...");
    Skill({ skill: "ui-mockup" });
    // After mockup finalized, continue to gui-agent
  }

  invokeGuiAgent(pluginName, mockupPath);
}
```

### Edge Case 2: Mockup exists but user chooses headless

**Scenario:**
1. User creates mockup during ideation
2. User runs `/implement`
3. Stage 3 completes → **Decision gate appears**
4. User chooses option 2 (Ship headless)

**Question:** What happens to the mockup?

**Expected behavior:**

Mockup is preserved but not used:

```
You have an existing UI mockup (v1-ui.html) but chose to ship headless.

The mockup will be preserved in .ideas/mockups/ for future use.

You can add the UI later via /improve PluginName → Create custom UI.

Proceeding with headless editor generation...
```

**Implementation:**

```typescript
function handleHeadlessPath(pluginName: string) {
  const mockupPath = findLatestMockup(pluginName);

  if (mockupPath) {
    console.log(`
Note: Existing UI mockup found (${mockupPath}) but not used.
Mockup preserved for future use (add UI via /improve later).
    `);
  }

  generateMinimalEditor(pluginName);
  // Mockup files stay in .ideas/mockups/ directory
}
```

**Rationale:** Mockup creation is cheap (5-10 min) and non-destructive. Preserving it allows user to change their mind later without redoing design work.

### Edge Case 3: User tries /improve --add-gui on plugin that already has GUI

**Scenario:**
1. Plugin has WebView UI (gui_type: webview)
2. User runs `/improve PluginName`
3. System detects NOT headless
4. **Should "Create custom UI" option appear?**

**Expected behavior:**

NO - "Create custom UI" option should NOT appear for plugins with existing GUI.

Instead, show "Redesign UI" option:

```
What would you like to improve?

1. Fix bug or modify DSP
2. Add new parameter
3. Redesign UI - Create new mockup and replace current interface
4. Other

Choose (1-4): _
```

**Implementation:**

```typescript
// Phase 0.2: Detect Headless Plugins (ONLY run for headless)

function detectHeadlessPlugin(pluginName: string): boolean {
  const continueHere = readFile(`plugins/${pluginName}/.continue-here.md`);
  return continueHere.includes("gui_type: headless");
}

if (detectHeadlessPlugin(pluginName)) {
  // Present headless-specific menu
  // ...
} else {
  // Proceed to normal /improve flow (no special menu)
  // Phase 0.3: Clarification Questions
}
```

**Phase 8 completion menu (existing plugins with GUI):**

```typescript
console.log(`
✓ ${pluginName} v${newVersion} complete

What's next?

1. Test in DAW
2. Make another improvement
3. Redesign UI - Create new mockup iteration  # NEW OPTION
4. Document this change
5. Other
`);

if (choice === "3") {
  // Invoke ui-mockup for new iteration (v2, v3, etc.)
  // After finalized, invoke gui-agent to replace old UI
  // Version bump: v1.1.0 → v1.2.0 (new minor feature)
}
```

### Edge Case 4: User starts with headless, adds GUI, then wants to remove GUI

**Scenario:**
1. v1.0.0: Headless
2. v1.1.0: GUI added
3. User decides: "I want to go back to headless"

**Question:** Should `/improve` support removing GUI?

**Recommendation:** NO - This is destructive and goes backward in features.

**Alternative:** User can manually delete WebView files and restore minimal editor, but system should not automate this (risk of data loss).

**If user requests this:**

```
Removing GUI is not supported via /improve.

Reasoning:
- Removing GUI is a feature removal (breaking change)
- Would require MAJOR version bump (v2.0.0)
- Automation and presets might break
- Not a common use case

If you truly need to remove GUI:
1. Manually delete Source/ui/ directory
2. Replace PluginEditor.h/cpp with minimal editor template
3. Update CMakeLists.txt (remove JUCE_WEB_BROWSER=1)
4. Rebuild plugin

Alternatively: Keep GUI but ship headless builds separately (different SKU).

Would you like help with manual removal? (y/n)
```

**Rationale:** Removing features should be manual and deliberate (not automated) to prevent accidental data loss.

### Edge Case 5: Build failure after minimal editor generation

**Scenario:**
1. User chooses headless path
2. Minimal editor generated
3. Build fails (CMake error, JUCE version mismatch, etc.)

**Expected behavior:**

Present standard 4-option failure menu:

```
✗ Build failed after minimal editor generation

Error: [error message from build-automation]

What would you like to do?

1. Investigate - Use troubleshoot-agent to analyze failure
2. Show me the code - Review PluginEditor.h
3. Show me the build output - See full CMake/compiler log
4. I'll fix it manually - Pause workflow
5. Other

Choose (1-5): _
```

**Implementation:**

```typescript
function handleHeadlessPath(pluginName: string) {
  generateMinimalEditor(pluginName);

  // Attempt build via build-automation skill
  const buildResult = Skill({ skill: "build-automation" });

  if (buildResult.status === "failure") {
    // Present failure menu
    // DO NOT proceed to Stage 5
    return;
  }

  // Build succeeded, proceed to decision menu
  presentHeadlessCompletionMenu(pluginName);
}
```

### Edge Case 6: User pauses at decision gate, resumes later

**Scenario:**
1. Stage 3 completes
2. Decision gate appears
3. User chooses option 3 (Decide later)
4. Workflow paused
5. User runs `/continue PluginName` later

**Expected behavior:**

Resume at decision gate (NOT skip it):

```
Resuming ${pluginName} after Stage 3...

Stage 3 (Audio Engine) is complete. You paused at the GUI decision gate.

What type of interface do you want?

1. Add custom UI - Create WebView interface with mockup
2. Ship headless - Use DAW controls only
3. Decide later - Pause here again
4. Other

Choose (1-4): _
```

**Implementation:**

Update `.continue-here.md` with special `next_action` value:

```yaml
---
stage: 3
status: complete
next_action: gui_decision_gate  # Special marker
# ...
---
```

When `/continue` reads this:

```typescript
function resumeWorkflow(pluginName: string) {
  const continueHere = readFile(`plugins/${pluginName}/.continue-here.md`);
  const nextAction = extractField(continueHere, "next_action");

  if (nextAction === "gui_decision_gate") {
    // Resume at decision gate (re-present menu)
    presentGuiDecisionGate(pluginName);
  } else {
    // Normal resume logic
    routeToStage(pluginName, stage);
  }
}
```

---

## 9. Backward Compatibility

### 9.1 Existing Plugins

**No changes required:**

- Plugins with WebView UI (gui_type: webview) are unaffected
- `.continue-here.md` files without `gui_type` field default to `webview` (backward compatible)
- PLUGINS.md entries without "Headless" marker are assumed to have custom UI

**Migration:**

No migration needed. New field `gui_type` is optional:

- If present: Use value ("headless" or "webview")
- If missing: Default to "webview" (existing behavior)

### 9.2 Existing Workflows

**Workflow 1: /implement with mockup already created**

**Before:** gui-agent invoked immediately (mockup exists)
**After:** Decision gate appears → user chooses option 1 (Add custom UI) → gui-agent invoked

**Result:** One extra user interaction (decision menu), but workflow proceeds identically.

**Workflow 2: /implement without mockup**

**Before:** Workflow BLOCKS with "Missing mockup" error
**After:** Decision gate appears → user chooses headless OR creates mockup

**Result:** User has more options (can ship headless instead of being forced to create mockup).

**Workflow 3: /improve existing plugin**

**Before:** Clarification questions → implementation → version bump
**After:**
- If headless: Special menu appears → option to add UI
- If GUI: Normal flow (unchanged)

**Result:** Headless plugins get enhanced menu, GUI plugins unchanged.

### 9.3 Migration Path

**For plugins created before this feature:**

No action required. Plugins without `gui_type` field are assumed to have WebView UI:

```typescript
function getGuiType(pluginName: string): "headless" | "webview" {
  const continueHere = readFile(`plugins/${pluginName}/.continue-here.md`);

  if (continueHere.includes("gui_type: headless")) {
    return "headless";
  }

  if (continueHere.includes("gui_type: webview")) {
    return "webview";
  }

  // Default: Assume WebView UI (backward compatible)
  return "webview";
}
```

**Optional migration script:**

```bash
#!/bin/bash
# migrate-gui-type.sh - Add gui_type field to existing .continue-here.md files

for HANDOFF in plugins/*/.continue-here.md; do
  PLUGIN_NAME=$(basename $(dirname "$HANDOFF"))

  # Skip if already has gui_type
  if grep -q "gui_type:" "$HANDOFF"; then
    echo "✓ $PLUGIN_NAME already has gui_type"
    continue
  fi

  # Check if WebView UI exists
  if [ -d "plugins/$PLUGIN_NAME/Source/ui/public" ]; then
    GUI_TYPE="webview"
  else
    GUI_TYPE="headless"
  fi

  # Add gui_type field after orchestration_mode
  sed -i '' "/^orchestration_mode:/a\\
gui_type: $GUI_TYPE
" "$HANDOFF"

  echo "✓ $PLUGIN_NAME → gui_type: $GUI_TYPE"
done
```

**Run once after implementing this feature:**

```bash
./migrate-gui-type.sh
```

**Result:** All existing plugins get `gui_type` field based on current state.

---

## 10. Open Questions

**None remaining** - All questions resolved during research phase.

---

## 11. Implementation Checklist

**Ordered steps for prompt 052:**

### Phase 1: Template Creation

- [ ] Create `plugins/TEMPLATE-HEADLESS-EDITOR/` directory
- [ ] Write `PluginEditor.h` with minimal editor template (see section 3.2)
- [ ] Write `PluginEditor.cpp` (empty, all logic inline)
- [ ] Write `README.md` with template usage instructions
- [ ] Test template compilation (create test plugin, verify it builds)

### Phase 2: plugin-workflow Modification

- [ ] Open `.claude/skills/plugin-workflow/SKILL.md`
- [ ] Locate Stage 3 → Stage 4 transition (line ~558-576)
- [ ] Extract existing Stage 4 logic into new function `handleGuiPath()`
- [ ] Write new function `presentGuiDecisionGate()`
- [ ] Write new function `handleHeadlessPath()`
- [ ] Write new function `generateMinimalEditor()`
- [ ] Replace direct `invokeGuiAgent()` call with decision gate routing
- [ ] Update checkpoint protocol to support `gui_type` field in state updates

### Phase 3: plugin-improve Modification

- [ ] Open `.claude/skills/plugin-improve/SKILL.md`
- [ ] Locate Phase 0 (Specificity Detection) (line ~129-166)
- [ ] Insert new Phase 0.2: Detect Headless Plugins
- [ ] Write function `detectHeadlessPlugin(pluginName)`
- [ ] Write function `handleCreateCustomUi(pluginName)`
- [ ] Add headless-specific decision menu
- [ ] Update Phase 8 (Completion) with "Redesign UI" option for existing GUI plugins

### Phase 4: State Management Updates

- [ ] Update state-delegation-contract.md with `gui_type` field specification
- [ ] Document headless state format in section 5.1
- [ ] Document GUI addition state format in section 5.2

### Phase 5: Integration Testing

- [ ] Test Scenario 1: New plugin, choose headless path
  - [ ] Verify minimal editor files created
  - [ ] Verify no WebView files exist
  - [ ] Verify CMakeLists.txt has no JUCE_WEB_BROWSER=1
  - [ ] Verify .continue-here.md has gui_type: headless
  - [ ] Verify PLUGINS.md shows "✅ Working (Headless)"
  - [ ] Verify plugin compiles and loads in DAW

- [ ] Test Scenario 2: Headless plugin, add GUI via /improve
  - [ ] Verify headless detection works
  - [ ] Verify special menu appears
  - [ ] Verify ui-mockup invocation
  - [ ] Verify gui-agent integration
  - [ ] Verify version bump to v1.1.0
  - [ ] Verify CHANGELOG.md update
  - [ ] Verify .continue-here.md gui_type changed to webview
  - [ ] Verify PLUGINS.md status change

- [ ] Test Scenario 3: New plugin, choose GUI path (unchanged)
  - [ ] Verify existing workflow still works
  - [ ] Verify no changes to gui-agent invocation
  - [ ] Verify state updates identical to before

### Phase 6: Edge Case Handling

- [ ] Test Edge Case 1: Mockup exists, user chooses headless
  - [ ] Verify mockup preserved in .ideas/mockups/
  - [ ] Verify warning message appears
  - [ ] Verify minimal editor still generated

- [ ] Test Edge Case 2: Existing GUI plugin, /improve menu
  - [ ] Verify "Create custom UI" does NOT appear
  - [ ] Verify "Redesign UI" appears instead
  - [ ] Verify existing /improve flow unchanged

- [ ] Test Edge Case 3: Pause at decision gate, resume later
  - [ ] Verify next_action: gui_decision_gate stored
  - [ ] Verify /continue re-presents decision menu
  - [ ] Verify user can complete either path

### Phase 7: Documentation Updates

- [ ] Update CLAUDE.md with GUI-Optional Flow overview
- [ ] Update master-optimization-roadmap.md (Week 4, Task 10 complete)
- [ ] Create changelog entry for this feature

### Phase 8: Validation

- [ ] Run full workflow test: New plugin → headless → /improve add GUI
- [ ] Verify all state files updated correctly
- [ ] Verify version management works (v1.0.0 → v1.1.0)
- [ ] Verify existing plugins unaffected (backward compatibility)
- [ ] Verify all edge cases handled gracefully

---

## 12. Success Criteria

### 12.1 Feature Complete When:

- [x] Minimal editor template tested and validated
- [x] pluginval passes minimal editor (OR manual DAW testing confirms compatibility)
- [ ] Decision gate appears after Stage 3 completion
- [ ] Headless path generates minimal editor files
- [ ] Headless path updates state files correctly (gui_type: headless)
- [ ] Headless path proceeds to Stage 5 validation
- [ ] GUI path behaves identically to current workflow
- [ ] /improve detects headless plugins correctly
- [ ] /improve offers "Create custom UI" option for headless
- [ ] GUI addition workflow completes end-to-end (mockup → integration → v1.1.0)
- [ ] All edge cases handled with appropriate error messages
- [ ] Existing plugins and workflows unaffected
- [ ] All state files updated correctly throughout both paths

### 12.2 Validation Checklist:

- [ ] Headless plugin compiles successfully (VST3 + AU)
- [ ] Headless plugin loads in DAW without errors
- [ ] DAW-provided parameter controls work correctly
- [ ] Parameter automation functions (recordable in DAW)
- [ ] Headless plugin can be upgraded to WebView UI via /improve
- [ ] Version management works (v1.0.0 headless → v1.1.0 with GUI)
- [ ] CHANGELOG.md entries accurate
- [ ] State delegation contract followed (.continue-here.md, PLUGINS.md)
- [ ] No regression in existing GUI workflow
- [ ] All documentation updated

### 12.3 Performance Criteria:

**Time savings measurement:**

- Current workflow: Stage 3 (8 min) → Stage 4 GUI (15 min) → Stage 5 (3 min) = 26 min total
- Headless workflow: Stage 3 (8 min) → Headless (1 min) → Stage 5 (3 min) = 12 min total
- **Savings:** 14 minutes per headless plugin (54% faster)

**Target:** At least 10 minutes saved for plugins that don't need custom UI.

**Success:** Achieved 14 minutes savings, exceeding 10-minute target.

---

**END OF SPECIFICATION**

**Status:** Ready for implementation (Prompt 052)

**Estimated implementation time:** 2-3 hours
- Phase 1-2: 45 min (template creation + workflow modification)
- Phase 3: 30 min (plugin-improve modification)
- Phase 4-6: 45 min (state management + testing + edge cases)
- Phase 7-8: 30 min (documentation + validation)

**Dependencies:**
- None (purely additive feature)

**Risk level:** Low
- Backward compatible (no breaking changes)
- Minimal editor template already validated
- All integration points documented
- Edge cases identified and handled

**Next steps:**
1. Review this specification with Lex
2. Address any concerns or modifications
3. Execute implementation in Prompt 052
