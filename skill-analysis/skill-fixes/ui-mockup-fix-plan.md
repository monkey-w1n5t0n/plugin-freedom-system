# Fix Plan: ui-mockup

## Summary
- Critical fixes: 8
- Extraction operations: 6
- Total estimated changes: 18
- Estimated time: 150 minutes (2.5 hours)

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Enforce Phase A/B Split with XML Critical Sequence
**Location:** Lines 15-34
**Operation:** WRAP
**Before:**
```markdown
## Workflow Overview

**TWO-PHASE WORKFLOW:**

### Phase A: Design Iteration (Fast)
Generate 2 design files for rapid iteration:
1. v[N]-ui.yaml - Machine-readable design specification
2. v[N]-ui-test.html - Browser-testable mockup with parameter bindings

**STOP HERE** - Present decision menu for user to iterate or finalize.

### Phase B: Implementation Scaffolding (After Finalization)
Generate 5 implementation files only after user approves design:
3. v[N]-ui.html - Production HTML (optimized, no test controls)
4. v[N]-PluginEditor.h - C++ header boilerplate
5. v[N]-PluginEditor.cpp - C++ implementation with WebView integration
6. v[N]-CMakeLists.txt - Build configuration snippet
7. v[N]-integration-checklist.md - Step-by-step integration guide
```
**After:**
```markdown
## Workflow Overview

<critical_sequence phases="A,B" enforcement="gate">
<phase id="A" name="Design Iteration">
**Purpose:** Generate 2 design files for rapid iteration.

**Outputs:**
1. v[N]-ui.yaml - Machine-readable design specification
2. v[N]-ui-test.html - Browser-testable mockup with parameter bindings

<decision_gate id="design_approval" required="true">
**STOP:** Do NOT proceed to Phase B until user approves design via Phase 5.5 decision menu.

**Gate criteria:**
- User selected option 3 or 5 from Phase 5.5 menu ("Finalize...")
- Design validated against WebView constraints
</decision_gate>
</phase>

<phase id="B" name="Implementation Scaffolding" requires_gate="design_approval">
**Purpose:** Generate 5 implementation files ONLY after Phase A approval.

**Outputs:**
3. v[N]-ui.html - Production HTML (optimized, no test controls)
4. v[N]-PluginEditor.h - C++ header boilerplate
5. v[N]-PluginEditor.cpp - C++ implementation with WebView integration
6. v[N]-CMakeLists.txt - Build configuration snippet
7. v[N]-integration-checklist.md - Step-by-step integration guide
</phase>
</critical_sequence>
```
**Verification:** XML tags present at lines 15 and 34, nested structure visible

### Fix 1.2: Add Blocking Decision Gate to Phase 5.5
**Location:** Lines 457-510
**Operation:** WRAP
**Before:**
```markdown
## ⚠️ CRITICAL STOP POINT - Phase 5.5: Design Decision Menu

**DO NOT PROCEED TO PHASE 6 WITHOUT USER CONFIRMATION**

After generating YAML + test HTML, present this decision menu:

✓ Mockup v[N] design created (2 files)

What's next?

1. Check alignment with creative brief (invoke design-sync skill)
2. Refine design (iterate on mockup v[N+1])
3. Finalize this design (proceed to implementation scaffolding)
4. Open mockup in browser (test.html)
5. Finalize and commit all versions (stop after Phase B)
6. Review YAML structure
7. Save as template (invoke ui-template-library skill)
8. Other

Choose (1-8): _

**WAIT for user response before continuing.**
```
**After:**
```markdown
<decision_gate id="phase_5_5_approval" blocking="true">
## Phase 5.5: Design Decision Menu

**Gate enforcement:** Phases 6-10 are CONDITIONALLY EXECUTED based on user choice.

<menu_presentation>
Present this decision menu:

✓ Mockup v[N] design created (2 files)

What's next?

1. Check alignment with creative brief (invoke design-sync skill)
2. Refine design (iterate on mockup v[N+1])
3. Finalize this design (proceed to implementation scaffolding)
4. Open mockup in browser (test.html)
5. Finalize and commit all versions (stop after Phase B)
6. Review YAML structure
7. Save as template (invoke ui-template-library skill)
8. Other

Choose (1-8): _
</menu_presentation>

<conditional_execution requires="user_choice">
**Execution routing:**
- IF user chose option 3 or 5 (finalize) THEN proceed to Phase 6
- IF user chose option 2 (refine) THEN return to Phase 2 with v[N+1]
- IF user chose option 1 (check alignment) THEN invoke design-sync skill
- ELSE handle custom options

**DO NOT proceed to Phase 6 unless user explicitly chose finalization option.**
</conditional_execution>
</decision_gate>
```
**Verification:** Confirm XML tags at lines 457 and 510, conditional_execution block present

### Fix 1.3: Add Executable Validation Gate to Phase 5.3
**Location:** Lines 376-400
**Operation:** WRAP
**Before:**
```markdown
## Phase 5.3: Validate WebView Constraints (Before Decision Menu)

**CRITICAL:** Validate generated HTML against WebView constraints before presenting to user.

**Validation checklist:**

```bash
# Check for forbidden CSS viewport units
! grep -q "100vh\|100vw\|100dvh\|100svh" v[N]-ui-test.html

# Check for required html/body height
grep -q "html, body.*height: 100%" v[N]-ui-test.html
```

**If validation fails:**
- ❌ REJECT: Regenerate mockup with corrections
- Do NOT present to user until constraints are satisfied
```
**After:**
```markdown
## Phase 5.3: Validate WebView Constraints (Before Decision Menu)

<requirement type="validation" blocking="true">
<validation_checklist>
**Execute validation before Phase 5.5:**

```bash
# Store validation results
VALIDATION_PASSED=true

# Check 1: No viewport units
if grep -q "100vh\|100vw\|100dvh\|100svh" v[N]-ui-test.html; then
    echo "❌ FAIL: Forbidden viewport units found"
    VALIDATION_PASSED=false
fi

# Check 2: Required html/body height
if ! grep -q "html, body.*height: 100%" v[N]-ui-test.html; then
    echo "❌ FAIL: Missing required html/body height: 100%"
    VALIDATION_PASSED=false
fi

# Check 3: Native feel CSS
if ! grep -q "user-select: none" v[N]-ui-test.html; then
    echo "❌ FAIL: Missing user-select: none"
    VALIDATION_PASSED=false
fi

# Check 4: Context menu disabled
if ! grep -q 'contextmenu.*preventDefault' v[N]-ui-test.html; then
    echo "❌ FAIL: Context menu not disabled"
    VALIDATION_PASSED=false
fi

# Gate decision
if [ "$VALIDATION_PASSED" = false ]; then
    echo "Regenerating mockup with corrections..."
    # Return to Phase 4 with fixes
else
    echo "✅ All WebView constraints validated"
    # Proceed to Phase 5.5
fi
```

**Failure handling:**
IF any check fails THEN:
  1. Log specific violation
  2. Regenerate mockup with corrections
  3. Re-run validation
  4. Do NOT proceed to Phase 5.5 until ALL checks pass
</validation_checklist>
</requirement>
```
**Verification:** Executable validation script present, blocking attribute set

### Fix 1.4: Add State Requirement Enforcement to Phase 10.5
**Location:** Lines 832-875
**Operation:** WRAP
**Before:**
```markdown
## Phase 10.5: Finalization Commit

[commit instructions]
```
**After:**
```markdown
<state_requirement>
<commit_protocol phase="finalization">
## Phase 10.5: Finalization Commit

**MUST commit all implementation files and update workflow state.**

**Files to commit:**
- All 7 mockup files (v[N]-*.{html,yaml,h,cpp,txt,md})
- parameter-spec.md (if v1 only)
- .continue-here.md (workflow state)

**State updates required:**
```bash
# Update .continue-here.md
sed -i '' "s/mockup_finalized: .*/mockup_finalized: true/" .continue-here.md
sed -i '' "s/finalized_version: .*/finalized_version: [N]/" .continue-here.md
sed -i '' "s/stage_0_status: .*/stage_0_status: ui_design_complete/" .continue-here.md
```

**Verification:**
- [ ] Git commit succeeded
- [ ] .continue-here.md updated
- [ ] mockup_finalized: true
- [ ] stage_0_status: ui_design_complete

ONLY proceed to "After Completing All Phases" menu if verification passes.
</commit_protocol>
</state_requirement>
```
**Verification:** State requirement wrapper present, verification checklist included

### Fix 1.5: Clarify Conditional Creative Brief Reading
**Location:** Line 108
**Operation:** REPLACE
**Before:**
```markdown
**CRITICAL: Always read creative-brief.md before starting.**
```
**After:**
```markdown
**CRITICAL: Read creative-brief.md if it exists:**

```bash
if [ -f "plugins/$PLUGIN_NAME/.ideas/creative-brief.md" ]; then
    # Extract context (see references/context-extraction.md)
    # Continue to Phase 1.5 with design-informed prompt
else
    # Skip to Phase 1.5 with generic prompt (standalone mode)
fi
```

**Note:** preconditions="None" means skill can work standalone without creative-brief.md, but MUST read it when present.
```
**Verification:** Conditional logic explicit, preconditions clarified

### Fix 1.6: Clarify Decision Menu Presentation Format
**Location:** Line 299
**Operation:** INSERT
**Before:**
```markdown
**Example question batch (via AskUserQuestion):**
```
**After:**
```markdown
**Note:** Internal question routing uses AskUserQuestion tool, but final decision menus (Phase 5.5, Phase 10.7) MUST use inline numbered format per checkpoint protocol.

**Example question batch (via AskUserQuestion):**
```
**Verification:** Note present before example, explains internal vs external format

### Fix 1.7: Add Phase 6-10 Conditional Wrapper
**Location:** Lines 511-831
**Operation:** WRAP
**Before:**
```markdown
## Phase 6: Generate Production HTML (v[N]-ui.html)
[content]
```
**After:**
```markdown
<conditional_execution requires_gate="phase_5_5_approval">
## Phase 6: Generate Production HTML (v[N]-ui.html)
[all Phase 6-10 content unchanged]
</conditional_execution>
```
**Verification:** Opening tag at line 511, closing tag after Phase 10 ends

### Fix 1.8: Add Critical Sequence Wrapper for Phase 6-10
**Location:** Lines 511-831
**Operation:** WRAP (nested inside conditional_execution)
**Before:**
```markdown
<conditional_execution requires_gate="phase_5_5_approval">
## Phase 6: Generate Production HTML (v[N]-ui.html)
## Phase 7: Generate C++ Header (v[N]-PluginEditor.h)
## Phase 8: Generate C++ Implementation (v[N]-PluginEditor.cpp)
## Phase 9: Generate CMake Configuration (v[N]-CMakeLists.txt)
## Phase 10: Generate Integration Checklist (v[N]-integration-checklist.md)
</conditional_execution>
```
**After:**
```markdown
<conditional_execution requires_gate="phase_5_5_approval">
<critical_sequence phases="6,7,8,9,10" enforcement="sequential">
## Phase 6: Generate Production HTML (v[N]-ui.html)
## Phase 7: Generate C++ Header (v[N]-PluginEditor.h)
## Phase 8: Generate C++ Implementation (v[N]-PluginEditor.cpp)
## Phase 9: Generate CMake Configuration (v[N]-CMakeLists.txt)
## Phase 10: Generate Integration Checklist (v[N]-integration-checklist.md)
</critical_sequence>
</conditional_execution>
```
**Verification:** Nested structure visible, phases execute sequentially

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Extract C++ Boilerplate Generation Logic
**Location:** Lines 581-664
**Operation:** EXTRACT
**Create file:** `references/cpp-boilerplate-generation.md`
**File content:**
```markdown
# C++ Boilerplate Generation for WebView UI

## Header Generation (v[N]-PluginEditor.h)

**Template replacement strategy:**

1. **Include guards:** `#ifndef [PLUGIN_NAME_UPPER]_PLUGINEDITOR_H`
2. **WebView integration:** Include `<juce_gui_extra/juce_gui_extra.h>`
3. **Member variables:**
   - `std::unique_ptr<juce::WebBrowserComponent> webView;`
   - `juce::AudioProcessorValueTreeState& apvts;`
4. **Constructor:** `PluginEditor(AudioProcessor& p, juce::AudioProcessorValueTreeState& vts);`
5. **Destructor:** `~PluginEditor() override;`
6. **JUCE overrides:**
   - `void paint(juce::Graphics&) override;`
   - `void resized() override;`

**Complete template:**

```cpp
#ifndef [PLUGIN_NAME_UPPER]_PLUGINEDITOR_H
#define [PLUGIN_NAME_UPPER]_PLUGINEDITOR_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

class [PluginName]Editor : public juce::AudioProcessorEditor
{
public:
    [PluginName]Editor([PluginName]Processor& p, juce::AudioProcessorValueTreeState& vts);
    ~[PluginName]Editor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    [PluginName]Processor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;
    std::unique_ptr<juce::WebBrowserComponent> webView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR([PluginName]Editor)
};

#endif // [PLUGIN_NAME_UPPER]_PLUGINEDITOR_H
```

## Implementation Generation (v[N]-PluginEditor.cpp)

**Template replacement strategy:**

1. **Constructor implementation:**
   - Call `setSize(width, height)` from YAML
   - Initialize APVTS reference
   - Create WebBrowserComponent
   - Load HTML file: `webView->goToURL(juce::File::getCurrentWorkingDirectory().getChildFile("v[N]-ui.html").getFullPathName())`
   - Add parameter listeners for each YAML control

2. **Destructor:** Empty or cleanup listeners

3. **paint():** Fallback background (webView handles UI)

4. **resized():** `webView->setBounds(getLocalBounds())`

**Complete template:**

```cpp
#include "PluginEditor.h"

[PluginName]Editor::[PluginName]Editor([PluginName]Processor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), audioProcessor(p), apvts(vts)
{
    setSize([WIDTH_FROM_YAML], [HEIGHT_FROM_YAML]);

    webView = std::make_unique<juce::WebBrowserComponent>();
    addAndMakeVisible(webView.get());

    // Load HTML
    auto htmlFile = juce::File::getCurrentWorkingDirectory().getChildFile("v[N]-ui.html");
    webView->goToURL(htmlFile.getFullPathName());

    // Parameter listeners (generate for each control in YAML)
    // [FOR_EACH_CONTROL]
}

[PluginName]Editor::~[PluginName]Editor()
{
}

void [PluginName]Editor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black); // Fallback
}

void [PluginName]Editor::resized()
{
    webView->setBounds(getLocalBounds());
}
```

## Replacement Variable Mapping

**Extract from context:**
- `[PLUGIN_NAME_UPPER]` - Plugin name in uppercase (e.g., LUSHVERB)
- `[PluginName]` - Plugin name in PascalCase (e.g., LushVerb)
- `[WIDTH_FROM_YAML]` - window.width from YAML
- `[HEIGHT_FROM_YAML]` - window.height from YAML
- `[N]` - Current version number (e.g., 1, 2, 3)
- `[FOR_EACH_CONTROL]` - Loop through YAML controls array

**Parameter listener generation:**
For each control in YAML `controls` array:
```cpp
apvts.addParameterListener("[control.id]", this);
```
```
**Replace with:**
```markdown
## Phase 7: Generate C++ Header (v[N]-PluginEditor.h)

Generate using template replacement strategy from `references/cpp-boilerplate-generation.md#header-generation`.

**Required inputs from context:**
- Plugin name (PascalCase and UPPERCASE)
- Current version number [N]

**Output:** Single .h file with WebView integration boilerplate.
```
**Verification:** File exists at references/cpp-boilerplate-generation.md, SKILL.md reduced by ~84 lines

### Fix 2.2: Extract CMake Configuration Details
**Location:** Lines 699-738
**Operation:** EXTRACT
**Create file:** `references/cmake-generation.md`
**File content:**
```markdown
# CMake Configuration Snippet Generation

## Purpose
Generate v[N]-CMakeLists.txt snippet for adding WebView UI files to plugin build.

## Template Structure

```cmake
# WebView UI Files (v[N])
target_sources([PluginName]
    PRIVATE
        v[N]-ui.html
)

# Copy HTML to build directory at build time
add_custom_command(
    TARGET [PluginName] POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/v[N]-ui.html
        $<TARGET_FILE_DIR:[PluginName]>/v[N]-ui.html
    COMMENT "Copying v[N] WebView UI files to build directory"
)

# Optional: Copy YAML for reference
add_custom_command(
    TARGET [PluginName] POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/v[N]-ui.yaml
        $<TARGET_FILE_DIR:[PluginName]>/v[N]-ui.yaml
    COMMENT "Copying v[N] YAML spec to build directory"
)
```

## Replacement Variables

- `[PluginName]` - Plugin name in PascalCase (e.g., LushVerb)
- `[N]` - Current version number (e.g., 1, 2, 3)

## Integration Instructions

**Add to existing CMakeLists.txt:**
1. Locate the `target_sources` section for the plugin target
2. Add the generated snippet after existing source files
3. Ensure HTML files are copied to the correct build directory

**Multiple versions:**
If multiple UI versions exist (v1, v2, v3), user must choose which version's HTML to copy. Only finalized version should be included in production builds.

## Notes

- YAML file copy is optional (for debugging/reference)
- HTML file MUST be in same directory as plugin binary at runtime
- CMAKE_COMMAND ensures cross-platform compatibility
- POST_BUILD ensures HTML is always fresh after compilation
```
**Replace with:**
```markdown
## Phase 9: Generate CMake Configuration (v[N]-CMakeLists.txt)

Generate CMake snippet using template from `references/cmake-generation.md`.

**Required inputs:**
- Plugin name (PascalCase)
- Current version number [N]

**Output:** Snippet file ready to paste into main CMakeLists.txt.
```
**Verification:** File exists at references/cmake-generation.md, SKILL.md reduced by ~40 lines

### Fix 2.3: Extract Context Extraction Examples
**Location:** Lines 115-150
**Operation:** EXTRACT
**Create file:** Append to `references/context-extraction.md`
**File content (append to existing):**
```markdown
## Example: Extracting from creative-brief.md

**Creative brief excerpt:**
```
# LushVerb - Creative Brief

## Core Concept
A lush, musical reverb with three intuitive controls: Size, Damping, Mix.

## Visual Aesthetic
- Dark, moody background (deep blue-grey)
- Three large rotary knobs in horizontal row
- Vintage hardware-inspired styling
```

**Extracted context for Phase 1.5:**
```
Plugin: LushVerb
Concept: Lush, musical reverb
Controls: Size, Damping, Mix (three rotary knobs)
Layout: Horizontal row
Style: Vintage hardware, dark moody (deep blue-grey background)
```

**What to extract:**
- Plugin name
- Core concept (one sentence)
- Control list (names and types)
- Layout preference (if specified)
- Visual aesthetic keywords (colors, style, mood)

**What to skip:**
- Detailed DSP descriptions
- Technical implementation notes
- Unrelated sections (if creative brief has multiple plugins)
```
**Replace with:**
```markdown
**Context extraction:** See `references/context-extraction.md#example-extracting-from-creative-brief` for detailed examples.
```
**Verification:** Content appended to existing references/context-extraction.md, SKILL.md reduced by ~36 lines

### Fix 2.4: Extract Question Batch Examples
**Location:** Lines 246-296
**Operation:** EXTRACT
**Create file:** Append to `references/design-questions.md`
**File content (append to existing):**
```markdown
## Example Question Batches

### Example 1: Three-knob reverb with minimal brief

**Context gaps identified:**
- Layout structure (not specified)
- Knob visual style (not specified)
- Background treatment (color mentioned but not layout)

**Generated question batch:**

```yaml
Question 1:
  question: "Layout structure for the three knobs?"
  header: "Layout"
  options:
    - label: "Horizontal row of knobs"
      description: "Classic single-row layout"
    - label: "Vertical stack"
      description: "Narrow, tall layout"
    - label: "Triangle arrangement"
      description: "Two bottom, one top"
    - label: "Other"
      description: "Custom arrangement"

Question 2:
  question: "Knob visual style preference?"
  header: "Knob Style"
  options:
    - label: "Realistic 3D knobs"
      description: "Skeuomorphic hardware look"
    - label: "Flat minimal knobs"
      description: "Modern, clean circles"
    - label: "Vintage radio knobs"
      description: "Textured, aged appearance"
    - label: "Other"
      description: "Custom design"

Question 3:
  question: "Background visual treatment?"
  header: "Background"
  options:
    - label: "Solid color with subtle gradient"
      description: "Clean, professional"
    - label: "Textured surface (metal, wood)"
      description: "Hardware-inspired"
    - label: "Geometric patterns"
      description: "Modern, abstract"
    - label: "Other"
      description: "Custom design"

Question 4:
  question: "Additional UI elements needed?"
  header: "Extras"
  options:
    - label: "Plugin name/logo only"
      description: "Minimal chrome"
    - label: "Preset browser"
      description: "Dropdown or menu"
    - label: "Visual feedback (meters, waveforms)"
      description: "Dynamic display"
    - label: "Other"
      description: "Custom elements"
```

**Tier classification:**
- Question 1 (Layout): Tier 1 - Critical gap
- Question 2 (Knob style): Tier 2 - Visual gap
- Question 3 (Background): Tier 2 - Visual gap
- Question 4 (Extras): Tier 3 - Polish gap

### Example 2: Complex multi-effect with detailed brief

**Context gaps identified:**
- Tab navigation style (brief mentions "tabbed interface" but no visual style)
- Module layout within tabs (not specified)

**Generated question batch:**

```yaml
Question 1:
  question: "Tab navigation visual style?"
  header: "Tab Style"
  options:
    - label: "Top horizontal tabs"
      description: "Classic web-style tabs"
    - label: "Side vertical tabs"
      description: "Narrow vertical strip"
    - label: "Icon-based tab bar"
      description: "Minimal, icon-only"
    - label: "Other"
      description: "Custom navigation"

Question 2:
  question: "Module layout within each tab?"
  header: "Module Layout"
  options:
    - label: "Grid arrangement"
      description: "Organized rows/columns"
    - label: "Free-form positioning"
      description: "Custom placement"
    - label: "Single-column flow"
      description: "Vertical stacking"
    - label: "Other"
      description: "Custom layout"
```

**Note:** Fewer questions needed when brief is detailed. Focus on genuinely ambiguous aspects.
```
**Replace with:**
```markdown
**Question batch generation:**

Generate 4 questions using AskUserQuestion based on identified gaps.

**Question structure pattern:**
- question: Clear, specific question about the gap
- header: Short category label (max 12 chars)
- options: 2-4 distinct choices + "Other" (automatically added)

**See:** `references/design-questions.md#example-question-batches` for question templates and tiering examples.

**Tier priority:**
1. Critical gaps (layout, control types) - ask first
2. Visual gaps (style, key elements) - ask second
3. Polish gaps (colors, animations) - ask if needed
```
**Verification:** Content appended to existing references/design-questions.md, SKILL.md reduced by ~51 lines

### Fix 2.5: Replace Inline YAML Template with Reference
**Location:** Lines 325-355
**Operation:** REPLACE
**Before:**
```yaml
window:
  width: 600
  height: 400
  resizable: false
  title: "PluginName"

controls:
  - id: "size"
    type: "knob"
    label: "Size"
    x: 100
    y: 150
    width: 80
    height: 80
    min: 0.0
    max: 1.0
    default: 0.5
[...28 more lines...]
```
**After:**
```markdown
**YAML structure:** See `assets/ui-yaml-template.yaml` for complete template with all control types.

**Required sections:**
- window: Dimensions, resizability
- controls: Array of control definitions (id, type, position, range)
- styling: Colors, fonts, theme tokens
```
**Verification:** Inline YAML removed, reference link present, ~31 lines reduced

### Fix 2.6: Extract JavaScript Binding Example to Assets
**Location:** Lines 379-402
**Operation:** EXTRACT
**Create file:** `assets/parameter-binding-example.js`
**File content:**
```javascript
// Parameter Binding Pattern for JUCE WebView Communication

// Example: Knob control bound to JUCE parameter

// 1. HTML knob element
<input type="range"
       id="size-knob"
       class="knob-control"
       min="0"
       max="100"
       value="50">

// 2. JavaScript binding (in v[N]-ui-test.html)
const sizeKnob = document.getElementById('size-knob');

// Listen for user interaction
sizeKnob.addEventListener('input', (e) => {
    const normalizedValue = e.target.value / 100; // Normalize to 0.0-1.0

    // Send to JUCE (mock for test.html, real for production)
    if (window.webkit && window.webkit.messageHandlers.juce) {
        // Production: Send to JUCE
        window.webkit.messageHandlers.juce.postMessage({
            action: 'parameterChange',
            parameterId: 'size',
            value: normalizedValue
        });
    } else {
        // Test mode: Console log
        console.log(`Parameter 'size' changed to ${normalizedValue}`);
    }
});

// Listen for parameter updates from JUCE (automation, preset changes)
window.updateParameter = function(parameterId, value) {
    if (parameterId === 'size') {
        sizeKnob.value = value * 100; // Denormalize from 0.0-1.0
    }
};

// 3. JUCE C++ side (PluginEditor.cpp)
// Parameter listener callback
void parameterChanged(const juce::String& parameterID, float newValue) override
{
    if (parameterID == "size") {
        // Execute JavaScript to update knob
        webView->evaluateJavascript(
            "window.updateParameter('size', " + juce::String(newValue) + ");"
        );
    }
}
```
**Replace with:**
```markdown
**Parameter binding:** Two-way communication between HTML controls and JUCE parameters.

See `assets/parameter-binding-example.js` for complete bidirectional binding pattern (HTML → JUCE and JUCE → HTML).

**Key concepts:**
- HTML controls send normalized values (0.0-1.0) to JUCE via `window.webkit.messageHandlers.juce`
- JUCE sends updates back via `evaluateJavascript("window.updateParameter(...)")`
- Test HTML mocks JUCE communication with console.log
```
**Verification:** File created at assets/parameter-binding-example.js, SKILL.md reduced by ~24 lines

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Condense Versioning Strategy Section
**Location:** Lines 899-926
**Operation:** REPLACE
**Before:**
```markdown
## Versioning Strategy

[28 lines explaining version numbering system]
```
**After:**
```markdown
## Versioning Strategy

**Pattern:** v1, v2, v3... Each UI version is saved separately.

**Purpose:**
- Explore different layouts without losing previous work
- A/B test designs before committing
- Keep design history for rollback

**File naming:** All 7 files prefixed with version (e.g., `v2-ui.html`, `v2-PluginEditor.h`)

**Implementation:** Latest version used for Stage 5 unless user specifies different version.

**See:** `references/versioning.md` for file management details.
```
**Create file:** `references/versioning.md`
**File content:**
```markdown
# UI Mockup Versioning System

## Version Numbering

**Pattern:** v1, v2, v3, v4...

**Increments:**
- Start at v1 for first mockup
- Increment by 1 for each new design iteration
- No decimals (v1.1 is not allowed)

## File Naming Convention

**All 7 files share version prefix:**
- v[N]-ui.yaml
- v[N]-ui-test.html
- v[N]-ui.html (after finalization)
- v[N]-PluginEditor.h (after finalization)
- v[N]-PluginEditor.cpp (after finalization)
- v[N]-CMakeLists.txt (after finalization)
- v[N]-integration-checklist.md (after finalization)

**Example for v2:**
```
plugins/LushVerb/.ui/
├── v1-ui.yaml
├── v1-ui-test.html
├── v1-ui.html
├── v1-PluginEditor.h
├── v1-PluginEditor.cpp
├── v1-CMakeLists.txt
├── v1-integration-checklist.md
├── v2-ui.yaml         ← Current iteration
├── v2-ui-test.html    ← Current iteration
└── [v2 implementation files after finalization]
```

## Version Selection for Implementation

**Default behavior:** Use latest version number for Stage 5 integration.

**Explicit selection:** User can specify different version:
- "Use v2 design for implementation" → Integrate v2-ui.html
- "Finalize v1 instead" → Mark v1 as production, archive v2+

## Finalized vs Unfinalized Versions

**Finalized version:**
- Has all 7 files generated
- `mockup_finalized: true` in .continue-here.md
- `finalized_version: [N]` recorded
- Ready for Stage 5 integration

**Unfinalized version:**
- Has only 2 files (YAML + test HTML)
- User iteration in progress
- Not ready for Stage 5

## Version Cleanup

**When to clean up:**
- User finalizes v3, wants to remove abandoned v1 and v2
- User wants to start fresh from v1

**How to clean up:**
- Delete all v[N]-* files for unwanted versions
- Keep finalized version only
- Update .continue-here.md with new version tracking
```
**Verification:** Versioning section condensed from 28 to ~12 lines, reference file created

### Fix 3.2: Add MUST/SHOULD Language Consistency
**Location:** Multiple (Lines 108, 376, 457, 832)
**Operation:** VERIFY
**Changes:** Already present in Fixes 1.3, 1.4, 1.5
**Verification:** Scan SKILL.md for weak language ("should consider", "it's important"), replace with MUST/SHOULD/NEVER where appropriate

### Fix 3.3: Clarify Pronoun Antecedents
**Location:** Scan entire SKILL.md for ambiguous "it", "this", "they"
**Operation:** SCAN + REPLACE
**Example transformations:**
- "It should be wrapped" → "The control definition should be wrapped"
- "This generates..." → "Phase 4 generates..."
- "They are optional" → "CSS animations are optional"
**Verification:** No ambiguous pronouns in critical sequences (Phases 1-10)

## File Creation Manifest

Files to create:
1. `references/cpp-boilerplate-generation.md` - C++ PluginEditor template generation rules
2. `references/cmake-generation.md` - CMake snippet generation template
3. `references/versioning.md` - Version management detailed guide
4. `assets/parameter-binding-example.js` - JavaScript ↔ JUCE bidirectional binding example

Files to append to:
5. `references/context-extraction.md` - Add creative-brief extraction examples
6. `references/design-questions.md` - Add question batch examples with tiering

## Execution Checklist

**Phase 1: Critical Fixes**
- [ ] Fix 1.1: Phase A/B split wrapped in `<critical_sequence>` (lines 15-34)
- [ ] Fix 1.2: Phase 5.5 decision gate wrapped with `blocking="true"` (lines 457-510)
- [ ] Fix 1.3: Phase 5.3 validation gate with executable script (lines 376-400)
- [ ] Fix 1.4: Phase 10.5 state requirement enforcement (lines 832-875)
- [ ] Fix 1.5: Creative brief conditional reading clarified (line 108)
- [ ] Fix 1.6: Decision menu format note added (line 299)
- [ ] Fix 1.7: Phase 6-10 wrapped in `<conditional_execution>` (lines 511-831)
- [ ] Fix 1.8: Phase 6-10 wrapped in nested `<critical_sequence>` (lines 511-831)

**Phase 2: Content Extraction**
- [ ] Fix 2.1: C++ boilerplate extracted to references/cpp-boilerplate-generation.md (lines 581-664)
- [ ] Fix 2.2: CMake config extracted to references/cmake-generation.md (lines 699-738)
- [ ] Fix 2.3: Context examples appended to references/context-extraction.md (lines 115-150)
- [ ] Fix 2.4: Question examples appended to references/design-questions.md (lines 246-296)
- [ ] Fix 2.5: Inline YAML replaced with reference link (lines 325-355)
- [ ] Fix 2.6: JavaScript binding extracted to assets/parameter-binding-example.js (lines 379-402)

**Phase 3: Polish**
- [ ] Fix 3.1: Versioning section condensed + reference created (lines 899-926)
- [ ] Fix 3.2: MUST/SHOULD language verified (scan all)
- [ ] Fix 3.3: Ambiguous pronouns clarified (scan all)

**Verification**
- [ ] Total line reduction: ~300 lines (987 → ~687 estimated)
- [ ] Token reduction: ~4,200 tokens (13%)
- [ ] All XML tags properly nested and closed
- [ ] All reference files created with complete content
- [ ] All asset files created with executable examples
- [ ] SKILL.md still contains complete workflow logic (no missing steps)

**Testing**
- [ ] Test Phase A/B split: Confirm Claude stops at Phase 5.5
- [ ] Test validation gate: Generate mockup with intentional `100vh`, verify rejection
- [ ] Test conditional execution: Verify Phase 6-10 only execute after Phase 5.5 approval
- [ ] Test reference links: All extracted content accessible and complete

## Estimated Time Breakdown

**Phase 1: Critical Fixes (60 minutes)**
- Fix 1.1: Phase A/B XML wrapper (10 min)
- Fix 1.2: Phase 5.5 decision gate (10 min)
- Fix 1.3: Phase 5.3 validation gate (15 min)
- Fix 1.4: Phase 10.5 state requirement (10 min)
- Fix 1.5-1.8: Clarifications and conditional wrappers (15 min)

**Phase 2: Content Extraction (60 minutes)**
- Fix 2.1: C++ boilerplate extraction (20 min)
- Fix 2.2: CMake extraction (10 min)
- Fix 2.3: Context examples extraction (10 min)
- Fix 2.4: Question examples extraction (10 min)
- Fix 2.5: YAML reference replacement (5 min)
- Fix 2.6: JavaScript binding extraction (5 min)

**Phase 3: Polish (30 minutes)**
- Fix 3.1: Versioning condensation + reference (15 min)
- Fix 3.2: MUST/SHOULD scan (10 min)
- Fix 3.3: Pronoun clarity scan (5 min)

**Total: 150 minutes (2.5 hours)**
