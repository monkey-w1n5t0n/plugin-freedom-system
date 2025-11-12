# Adaptive Questioning Examples

## Example 1: Detailed Input (New Plugin)

**User input:**
"I want a tape delay with wow and flutter modulation. Should have three knobs and a vintage aesthetic."

**Extraction:**
- Type: Effect ✓
- Core concept: Tape delay with modulation ✓
- Unique value: Wow/flutter (analog character) ✓
- Parameters: Three knobs (not specific) ⚠
- Visual style: Vintage ✓
- Target user: Unknown ✗
- Presets: Unknown ✗
- DSP approach: Unknown ✗

**Identified gaps (Tier 1-2):**
1. Target user (Tier 1)
2. Parameter specifics (Tier 2)
3. Presets (Tier 2)
4. DSP approach (Tier 2)

**Generated questions:**
```
questions:
  - question: "Who is this tape delay primarily for?"
    header: "Audience"
    multiSelect: false
    options:
      - label: "Producers & beatmakers"
        description: "Quick creative effects, preset-focused workflow"
      - label: "Mix engineers"
        description: "Precision controls, surgical delay applications"
      - label: "Sound designers"
        description: "Experimental textures, modulation depth"
      - label: "Live performers"
        description: "Reliable, easy-to-tweak on stage"

  - question: "What should the three knobs control?"
    header: "Parameters"
    multiSelect: false
    options:
      - label: "Time, Feedback, Mix"
        description: "Classic delay controls"
      - label: "Time, Wow/Flutter, Mix"
        description: "Emphasize tape character"
      - label: "Time, Feedback, Wow/Flutter"
        description: "Always 100% wet (for send/return use)"

  - question: "Should this include factory presets?"
    header: "Presets"
    multiSelect: false
    options:
      - label: "Yes - starting points for common sounds"
        description: "Slap, long decay, vintage tape, etc."
      - label: "No - users create their own"
        description: "Minimal, creative tool"

  - question: "How should the tape modulation work?"
    header: "DSP"
    multiSelect: false
    options:
      - label: "Random LFO (wow + flutter)"
        description: "Unpredictable vintage tape character"
      - label: "Tempo-synced modulation"
        description: "Musical, rhythmic variation"
      - label: "Manual control (knob sets depth)"
        description: "User dials in desired amount"
```

---

## Example 2: Vague Input (New Plugin)

**User input:**
"I want to make a distortion plugin."

**Extraction:**
- Type: Effect ✓
- Core concept: Distortion (extremely vague) ⚠
- Unique value: Unknown ✗
- Parameters: Unknown ✗
- Target user: Unknown ✗
- Everything else: Unknown ✗

**Identified gaps (Start with Tier 1 foundational questions):**
1. Unique value (Tier 1) - What makes this different?
2. Target user (Tier 1)
3. Core concept refinement (Tier 1) - What kind of distortion?
4. Parameters (Tier 2)

**Generated questions:**
```
questions:
  - question: "What makes this distortion unique?"
    header: "Unique value"
    multiSelect: false
    options:
      - label: "Specific character"
        description: "Tube warmth, tape saturation, bit crushing, etc."
      - label: "Flexible/multi-mode"
        description: "Multiple distortion types in one plugin"
      - label: "Creative/experimental"
        description: "Unusual processing, glitch effects"

  - question: "Who is this for?"
    header: "Audience"
    multiSelect: false
    options:
      - label: "Mix engineers"
        description: "Subtle saturation, harmonic enhancement"
      - label: "Producers"
        description: "Creative sound design, track character"
      - label: "Guitarists/bassists"
        description: "Amp-like distortion, pedalboard replacement"

  - question: "What distortion character are you going for?"
    header: "Sound"
    multiSelect: false
    options:
      - label: "Warm analog saturation"
        description: "Tube or tape-style harmonics"
      - label: "Aggressive digital clip"
        description: "Hard clipping, fuzz, bit reduction"
      - label: "Waveshaping/folding"
        description: "Complex harmonic generation"

  - question: "What controls should users have?"
    header: "Parameters"
    multiSelect: false
    options:
      - label: "Drive, Tone, Mix"
        description: "Classic three-knob layout"
      - label: "Drive, Pre/Post EQ, Mix"
        description: "Frequency shaping flexibility"
      - label: "Drive, Character/Mode, Mix"
        description: "Switchable distortion types"
```

**Note:** With vague input, start broad (Tier 1) and narrow down through multiple question batches.

---

## Pattern Recognition

### Detailed Input Strategy:
1. Extract all provided information
2. Identify specific gaps in Tier 2-3
3. Ask targeted questions to fill gaps
4. Fewer question batches needed (1-2 usually sufficient)

### Vague Input Strategy:
1. Start with foundational Tier 1 questions
2. Use answers to refine direction
3. Follow-up batches dig into Tier 2-3 details
4. More question batches needed (2-4 typical)
5. Each batch narrows focus based on previous answers

### Common Trap to Avoid:
**Don't ask about what's already known.**
If user says "tape delay with wow and flutter," do NOT ask "What type of effect?" (already know: delay with modulation character)
