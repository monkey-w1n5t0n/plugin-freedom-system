# TapeAge WebView UI Integration Checklist

**Version:** v2 (Premium Skeuomorphic Design)
**Generated:** 2025-11-10
**Purpose:** Step-by-step guide to integrate WebView UI into TapeAge plugin during Stage 5 (GUI)

---

## Prerequisites

- [ ] Stage 4 (DSP) completed - plugin processes audio correctly
- [ ] Plugin builds and loads in DAW
- [ ] Parameters defined in PluginProcessor (drive, age, mix)
- [ ] JUCE project configured with CMake

---

## Step 1: Organize Files

### 1.1 Create UI Directory

```bash
cd plugins/TapeAge
mkdir -p Source/ui
```

### 1.2 Copy HTML File

```bash
cp .ideas/mockups/v2-ui.html Source/ui/v2-ui.html
```

**Verify:** `Source/ui/v2-ui.html` exists and contains production HTML

---

## Step 2: Update CMakeLists.txt

### 2.1 Add Binary Data Section

Open `plugins/TapeAge/CMakeLists.txt` and add:

```cmake
juce_add_binary_data(TapeAgeAssets
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/ui/v2-ui.html
)
```

**Location:** Before `juce_add_plugin()` call

### 2.2 Enable WebView Support

In `juce_add_plugin()`, add:

```cmake
NEEDS_WEB_BROWSER TRUE
```

### 2.3 Link Binary Data

In `target_link_libraries()`, add:

```cmake
target_link_libraries(TapeAge
    PRIVATE
        TapeAgeAssets  # Add this line
        juce::juce_audio_utils
        juce::juce_dsp
    # ... rest of links
)
```

### 2.4 Platform-Specific Links (macOS)

```cmake
if(APPLE)
    target_link_libraries(TapeAge PRIVATE "-framework WebKit")
endif()
```

**Verify:** CMake configuration runs without errors

---

## Step 3: Update PluginEditor.h

### 3.1 Copy Boilerplate

```bash
cp .ideas/mockups/v2-PluginEditor.h Source/PluginEditor.h
```

**Or manually add:**

```cpp
#include <JuceHeader.h>
#include "PluginProcessor.h"

class TapeAgeAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     private juce::Timer
{
public:
    TapeAgeAudioProcessorEditor(TapeAgeAudioProcessor&);
    ~TapeAgeAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void handleWebViewMessage(const juce::var& message);
    void updateWebViewParameter(const juce::String& paramID, float value);
    void updateVUMeter(float dbLevel);

    TapeAgeAudioProcessor& audioProcessor;
    std::unique_ptr<juce::WebBrowserComponent> webView;

    float currentVULevel = -60.0f;
    float peakVULevel = -60.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TapeAgeAudioProcessorEditor)
};
```

**Verify:** File compiles without syntax errors

---

## Step 4: Update PluginEditor.cpp

### 4.1 Copy Implementation

```bash
cp .ideas/mockups/v2-PluginEditor.cpp Source/PluginEditor.cpp
```

**Or manually implement:**

Key sections to add:
- Constructor: Load HTML from BinaryData, create WebView
- `timerCallback()`: Update VU meter at 60 Hz
- `handleWebViewMessage()`: Handle parameter changes from JavaScript
- `updateWebViewParameter()`: Send parameter updates to JavaScript
- `updateVUMeter()`: Send VU meter levels to JavaScript

**Verify:** File compiles without errors

---

## Step 5: Update PluginProcessor

### 5.1 Add Output Level Tracking

In `PluginProcessor.h`, add:

```cpp
private:
    std::atomic<float> outputLevel { -60.0f };

public:
    float getOutputLevel() const { return outputLevel.load(); }
```

### 5.2 Calculate Output Level in processBlock()

In `PluginProcessor.cpp`, add after DSP processing:

```cpp
void TapeAgeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    // ... existing DSP code ...

    // Calculate output level for VU meter
    float maxLevel = 0.0f;
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float channelMax = buffer.getMagnitude(channel, 0, buffer.getNumSamples());
        maxLevel = std::max(maxLevel, channelMax);
    }

    float dbLevel = juce::Decibels::gainToDecibels(maxLevel, -60.0f);
    outputLevel.store(dbLevel);
}
```

**Verify:** Plugin builds successfully

---

## Step 6: Build and Test

### 6.1 Clean Build

```bash
cd build
cmake --build . --config Release --clean-first
```

### 6.2 Test in Standalone

```bash
open TapeAge.app  # macOS
# or
./TapeAge  # Linux
```

**Expected:**
- Window opens at 500x450px
- VU meter visible with vintage design
- "TAPE AGE" title centered
- 3 brass knobs (DRIVE, AGE, MIX) visible
- Knobs rotate when dragged vertically
- VU meter needle animates with audio input

### 6.3 Test in DAW

Load plugin in your DAW (Logic, Ableton, etc.)

**Test:**
- [ ] UI loads correctly
- [ ] Knobs respond to mouse drag
- [ ] Parameter automation works
- [ ] VU meter updates in real-time
- [ ] UI survives plugin state save/restore
- [ ] No crashes when opening/closing editor

---

## Step 7: Troubleshooting

### Issue: WebView shows blank white screen

**Cause:** HTML not loaded or JavaScript error

**Fix:**
1. Check BinaryData includes v2-ui.html:
   ```bash
   grep "v2ui_html" build/JuceLibraryCode/BinaryData.h
   ```
2. Check browser console (if available)
3. Verify HTML syntax in v2-ui.html

### Issue: Knobs don't respond to drag

**Cause:** JavaScript event handlers not attached

**Fix:**
1. Check JavaScript console for errors
2. Verify `window.addEventListener('DOMContentLoaded', ...)` runs
3. Ensure `data-param` attributes match parameter IDs

### Issue: VU meter doesn't animate

**Cause:** Timer not running or `getOutputLevel()` not implemented

**Fix:**
1. Verify `startTimerHz(60)` called in constructor
2. Check `getOutputLevel()` returns valid dB values (-60 to +3)
3. Ensure `updateVUMeter()` calls JavaScript correctly

### Issue: Parameters don't sync between UI and DAW automation

**Cause:** Message handlers not connected

**Fix:**
1. Implement `handleWebViewMessage()` to receive UI changes
2. Implement `updateWebViewParameter()` to send host changes
3. Verify parameter IDs match between C++ and JavaScript

### Issue: Plugin crashes on close

**Cause:** WebView not properly destroyed

**Fix:**
1. Ensure `webView.reset()` or destructor runs
2. Call `stopTimer()` in destructor
3. Check for dangling pointers

---

## Step 8: Final Validation

### 8.1 Visual Check

- [ ] Window size exactly 500x450px
- [ ] VU meter with bronze frame and glass effect
- [ ] "TAPE AGE" title with proper letter spacing
- [ ] 3 brass knobs with knurled edges and center shine
- [ ] Labels (DRIVE, AGE, MIX) below knobs
- [ ] Vertical centering of all content
- [ ] Creamy beige background with subtle texture
- [ ] No layout glitches or clipping

### 8.2 Functional Check

- [ ] All knobs rotate smoothly (-135° to +135°)
- [ ] Indicator lines align with knob rotation
- [ ] VU meter needle moves with audio
- [ ] Red zone indicators visible above 0 dB
- [ ] Parameters persist across plugin instances
- [ ] No memory leaks (test with Activity Monitor)

### 8.3 Cross-Platform Check (if applicable)

- [ ] macOS: Tested in Logic Pro / Ableton
- [ ] Windows: Tested in Reaper / FL Studio
- [ ] Linux: Tested in Ardour / Bitwig

---

## Step 9: Mark Stage Complete

Once all checks pass:

```bash
# Update PLUGINS.md
# Update .continue-here.md
# Commit changes
git add plugins/TapeAge/Source/
git commit -m "feat(TapeAge): Stage 5 - WebView GUI complete"
```

---

## Reference Files

**Mockup files:**
- `.ideas/mockups/v2-ui.yaml` - Design specification
- `.ideas/mockups/v2-ui-test.html` - Browser test version
- `.ideas/mockups/v2-ui.html` - Production HTML (copy to Source/ui/)
- `.ideas/mockups/v2-PluginEditor.h` - C++ header boilerplate
- `.ideas/mockups/v2-PluginEditor.cpp` - C++ implementation boilerplate
- `.ideas/mockups/v2-CMakeLists.txt` - Build configuration snippets
- `.ideas/mockups/v2-integration-checklist.md` - This file

**Contract files:**
- `.ideas/creative-brief.md` - Original plugin vision
- `.ideas/parameter-spec.md` - Parameter definitions (immutable)
- `.ideas/architecture.md` - DSP architecture
- `.ideas/plan.md` - Implementation plan

---

## Success Criteria

Stage 5 (GUI) is complete when:

✅ Plugin loads with WebView UI in standalone and DAW
✅ All 3 parameters controllable via UI
✅ VU meter animates in real-time
✅ UI matches v2 mockup design exactly
✅ No crashes, memory leaks, or UI glitches
✅ Parameters sync with DAW automation
✅ Ready for Stage 6 (Validation)

---

**Next Stage:** Stage 6 - Validation (Pluginval, presets, final testing)
