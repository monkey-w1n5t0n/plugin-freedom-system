---
name: aesthetic-dreaming
description: Create aesthetic templates without first creating a plugin - capture visual design concepts through adaptive questioning
allowed-tools:
  - Read
  - Write
  - Bash
  - AskUserQuestion
  - Skill
preconditions:
  - None (entry point)
---

# aesthetic-dreaming Skill

**Purpose:** Create aesthetic templates before implementing any plugins. Capture visual design systems through adaptive questioning, generate structured aesthetic.md, and optionally create test previews demonstrating the aesthetic across different plugin types.

**Solves:** The friction of needing to create a full plugin just to design and save an aesthetic template. Build a library of visual design systems first, apply them later.

---

## Workflow Overview

1. **Free-form collection** - "What aesthetic are you envisioning?"
2. **Gap analysis** - Extract covered concepts, identify missing visual elements
3. **Adaptive questioning** - Generate 4 contextual questions via AskUserQuestion
4. **Decision gate** - Finalize / 4 more questions / add context
5. **Loop until finalized** - Repeat phases 2-4 as needed
6. **Name the aesthetic** - Capture memorable name and slug
7. **Test preview selection** - Choose 0-4 plugin types for preview generation (optional)
8. **Generate aesthetic.md** - Structured prose description following template
9. **Generate test previews** - Apply aesthetic to selected test plugins
10. **Decision menu** - Open previews / refine / save to library

---

## Phase 1: Free-Form Collection

**Start with open invitation:**

```
What aesthetic are you envisioning?

Describe the visual design system you want to create. Share as much or as little as you want—I'll ask follow-ups for anything missing.

Think about:
- Overall vibe and mood
- Color palette and philosophy
- Control styling (knobs, sliders, buttons)
- Spacing and density
- Textures and surface treatments
- Typography approach
- Best suited for what types of plugins

You can reference existing designs, describe feelings, or get technical with specifics.
```

**Extract from response:**

- **Visual Identity:** Overall vibe, mood, design philosophy
- **Color System:** Palette, warm/cool, contrast approach, specific colors
- **Typography:** Font choices, sizing philosophy, technical vs decorative
- **Control Styling:** Knob appearance, slider style, button design
- **Spacing:** Density (tight/comfortable/generous), breathing room
- **Surface Treatment:** Textures, shadows, depth, borders
- **Details:** Special features, animations, embellishments
- **Best For:** Plugin types, use cases, not recommended for
- **Inspirations:** References to existing designs or hardware

**Capture everything mentioned verbatim in accumulated context.**

---

## Phase 2: Gap Analysis and Question Prioritization

**Question Priority Tiers for Visual Design:**

- **Tier 1 (Critical):** Overall vibe/mood, color philosophy (warm/cool), control style (flat/3D/skeuomorphic)
- **Tier 2 (Visual Core):** Specific colors, typography approach, spacing density, surface textures
- **Tier 3 (Context):** Best suited for, special features, technical patterns, inspirations

**Extract from Phase 1 response, then identify gaps:**

1. Parse user's free-form description
2. Check which tiers are covered
3. Identify missing critical visual information
4. **Never ask about already-provided information**

**Example of smart extraction:**

```
User: "I want a dark modern aesthetic with blue accents and clean typography. Everything should feel professional and technical."

Extracted:
- Vibe: Modern, professional, technical ✓
- Color philosophy: Dark with blue accents ✓
- Typography: Clean ✓
- Control style: Not mentioned ✗
- Specific colors: Blue mentioned, need specifics ✗
- Spacing: Not mentioned ✗
- Textures: Not mentioned ✗

Gaps identified:
- Control styling approach? (Tier 1)
- Specific shade of blue? (Tier 2)
- Spacing density? (Tier 2)
- Surface treatment? (Tier 2)
```

**Prioritize gaps by tier, select top 4 for question batch.**

---

## Phase 3: Question Batch Generation

**Generate exactly 4 questions using AskUserQuestion based on identified gaps.**

**Rules:**
- If 4+ gaps exist: ask top 4 by tier priority
- If fewer gaps exist: pad with "nice to have" tier 3 questions
- Provide meaningful options (not just open text prompts)
- Always include "Other" option for custom input
- Users can skip questions via "Other" option and typing "skip"
- **Never ask about information already provided**

**Example question batch (via AskUserQuestion):**

For the dark modern aesthetic example above:

```javascript
AskUserQuestion({
  questions: [
    {
      question: "What control styling approach?",
      header: "Controls",
      multiSelect: false,
      options: [
        {
          label: "Flat 2D",
          description: "Minimal depth, clean lines, modern"
        },
        {
          label: "Subtle 3D",
          description: "Slight shadows and highlights"
        },
        {
          label: "Skeuomorphic",
          description: "Realistic hardware imitation"
        },
        {
          label: "Other",
          description: "Different approach or custom input"
        }
      ]
    },
    {
      question: "What shade of blue for accents?",
      header: "Blue accent",
      multiSelect: false,
      options: [
        {
          label: "Electric bright blue",
          description: "#00a8ff - high energy"
        },
        {
          label: "Professional medium blue",
          description: "#3b82f6 - balanced"
        },
        {
          label: "Muted slate blue",
          description: "#64748b - subtle"
        },
        {
          label: "Other",
          description: "Different shade or multiple blues"
        }
      ]
    },
    {
      question: "Spacing and density preference?",
      header: "Spacing",
      multiSelect: false,
      options: [
        {
          label: "Tight (8-16px gaps)",
          description: "Compact, information-dense"
        },
        {
          label: "Comfortable (20-32px gaps)",
          description: "Balanced breathing room"
        },
        {
          label: "Generous (40+px gaps)",
          description: "Spacious, minimal aesthetic"
        },
        {
          label: "Other",
          description: "Custom spacing approach"
        }
      ]
    },
    {
      question: "Surface treatment approach?",
      header: "Textures",
      multiSelect: false,
      options: [
        {
          label: "Completely flat",
          description: "Zero texture, pure colors"
        },
        {
          label: "Subtle shadows only",
          description: "Depth through elevation"
        },
        {
          label: "Noise/grain texture",
          description: "Slight texture overlay"
        },
        {
          label: "Other",
          description: "Different surface approach"
        }
      ]
    }
  ]
})
```

**After receiving answers:**
1. Accumulate context with previous responses
2. Re-analyze gaps
3. Proceed to decision gate

---

## Phase 3.5: Decision Gate

**Use AskUserQuestion with 3 options after each question batch:**

```javascript
AskUserQuestion({
  questions: [
    {
      question: "Ready to finalize the aesthetic concept?",
      header: "Next step",
      multiSelect: false,
      options: [
        {
          label: "Yes, finalize it",
          description: "Create aesthetic.md and optional previews"
        },
        {
          label: "Ask me 4 more questions",
          description: "Continue refining the design"
        },
        {
          label: "Let me add more context first",
          description: "Provide additional details before questions"
        }
      ]
    }
  ]
})
```

**Route based on answer:**
- Option 1 → Proceed to Phase 3.7 (name aesthetic)
- Option 2 → Return to Phase 2 (re-analyze gaps, generate next 4 questions)
- Option 3 → Collect free-form text, merge with context, return to Phase 2

**Context accumulation example:**

After Batch 1 answers: "Flat 2D", "Professional medium blue (#3b82f6)", "Comfortable (20-32px)", "Subtle shadows only"

Updated context:
- Control style: Flat 2D ✓
- Blue accent: #3b82f6 ✓
- Spacing: Comfortable (20-32px gaps) ✓
- Surface: Subtle shadows for depth ✓

New gaps for Batch 2:
- Typography specifics? (Tier 2)
- Control size preferences? (Tier 2)
- Best suited for what plugins? (Tier 3)
- Any special features? (Tier 3)

---

## Phase 3.7: Name the Aesthetic

**Before generating files, capture aesthetic name if not yet provided.**

If name NOT yet provided, ask via AskUserQuestion:

```javascript
AskUserQuestion({
  questions: [
    {
      question: "What should this aesthetic be called?",
      header: "Name",
      multiSelect: false,
      options: [
        {
          label: "[SuggestedName1]",
          description: "Based on vibe and colors"
        },
        {
          label: "[SuggestedName2]",
          description: "Alternative naming"
        },
        {
          label: "[SuggestedName3]",
          description: "Different approach"
        },
        {
          label: "Other",
          description: "I'll provide my own name"
        }
      ]
    }
  ]
})
```

**Name suggestion examples:**
- Dark modern blue → "Modern Professional", "Blue Tech", "Dark Elegance"
- Vintage warm analog → "Vintage Hardware", "Analog Warmth", "Retro Studio"
- Minimal flat white → "Pure Minimal", "Clean Canvas", "Studio White"

**Name validation:**
- Can contain spaces and normal characters
- Will be slugified for aesthetic ID: "Modern Professional" → "modern-professional-001"
- Increment counter if slug already exists

**If name already provided** (in initial description or context), skip this phase.

---

## Phase 4: Test Preview Selection

**After aesthetic concept is finalized and named, offer test preview generation.**

Present selection menu via AskUserQuestion with **multiSelect: true**:

```javascript
AskUserQuestion({
  questions: [
    {
      question: "Generate test previews to see this aesthetic applied? (select 0-4 plugin types)",
      header: "Test previews",
      multiSelect: true,
      options: [
        {
          label: "Simple compressor",
          description: "4 parameters: threshold, ratio, attack, release"
        },
        {
          label: "Complex reverb",
          description: "8 parameters: size, damping, predelay, mix, tone, width, decay, diffusion"
        },
        {
          label: "Drum machine",
          description: "16-pad grid + 4 controls: tempo, swing, volume, pattern"
        },
        {
          label: "Simple synth",
          description: "6 oscillator controls: waveform, frequency, attack, decay, sustain, release"
        }
      ]
    }
  ]
})
```

**User can select:**
- **0 options** (skip previews entirely) → Jump to Phase 6 (save aesthetic)
- **1-4 options** → Generate selected test previews in Phase 5
- **"Other" custom input** → User can specify different test plugin types

**Why multiSelect?**
- User might want to see aesthetic across multiple layouts
- User might want to skip previews entirely (fast path to save)
- User controls exactly which previews to generate

---

## Phase 5: Generate Aesthetic.md and Test Previews

### Step 1: Read Aesthetic Template

```bash
TEMPLATE="/Users/lexchristopherson/Developer/plugin-freedom-system/.claude/skills/ui-template-library/assets/aesthetic-template.md"
```

**Use Read tool to load template structure.**

### Step 2: Generate Structured Prose

**Transform accumulated context into structured prose descriptions for all 12 sections:**

1. **Visual Identity** - 2-3 sentence overall vibe summary
2. **Color System** - Primary palette, control colors, philosophy
3. **Typography** - Font families, sizing, styling, philosophy
4. **Controls** - Knob style, slider style, button style (detailed descriptions)
5. **Spacing & Layout Philosophy** - Density, gaps, adaptation strategy
6. **Surface Treatment** - Textures, depth, shadows, borders
7. **Details & Embellishments** - Special features, interactions
8. **Technical Patterns** - Border radius, transitions, layout techniques
9. **Interaction Feel** - Responsiveness, feedback, tactility
10. **Best Suited For** - Plugin types, design contexts
11. **Application Guidelines** - Parameter count adaptation, customization points
12. **Example Color Codes** - Concrete CSS values as reference

**Prose generation principles:**

- **Be interpretable, not rigid:** "Generous spacing with 32px gaps" not "spacing: 32px"
- **Capture intent:** "Warm brass controls evoking vintage hardware" not "color: #d4a574"
- **Provide context:** Explain WHY design choices were made
- **Include concrete examples:** Balance prose with specific values for reference
- **Follow template structure exactly:** Same sections, same order, every time

**Example prose transformation:**

```
Context: "Flat 2D controls, medium blue accent (#3b82f6), comfortable spacing"
↓
Section prose:
"Controls use flat 2D styling with subtle shadows for depth without heavy
skeuomorphism. Medium professional blue (#3b82f6) serves as primary accent,
providing energy without overwhelming. Comfortable spacing (24-32px gaps)
creates breathing room while maintaining information density for complex plugins."
```

### Step 3: Write aesthetic.md

```bash
AESTHETIC_ID=$(generate_aesthetic_id "$AESTHETIC_NAME")
AESTHETIC_DIR="/Users/lexchristopherson/Developer/plugin-freedom-system/.claude/aesthetics/$AESTHETIC_ID"
mkdir -p "$AESTHETIC_DIR"
```

**Generate aesthetic.md using Write tool:**

- Fill template with all generated prose sections
- Include concrete color codes in "Example Color Codes" section
- Add application guidelines for different parameter counts
- Preserve exact template structure

**Aesthetic ID generation:**
- Slugify name: "Modern Professional" → "modern-professional"
- Check for existing IDs and append counter: "modern-professional-001"
- Increment until unique ID found

### Step 4: Generate metadata.json

```json
{
  "id": "modern-professional-001",
  "name": "Modern Professional",
  "description": "Dark modern aesthetic with blue accents and clean typography",
  "sourcePlugin": null,
  "sourceVersion": null,
  "created": "2025-11-11T15:30:00Z",
  "usedInPlugins": [],
  "tags": ["modern", "professional", "dark", "blue", "clean", "technical"],
  "isTemplateOnly": true,
  "testPreviews": ["simple-compressor", "complex-reverb"]
}
```

**Tag inference from accumulated context:**
- Extract style descriptors: modern, vintage, minimal, etc.
- Extract color descriptors: dark, warm, cool, bright, etc.
- Extract feel descriptors: professional, playful, technical, organic, etc.

**isTemplateOnly: true** - Marks this aesthetic as created without source plugin

**testPreviews array** - Tracks which test previews were generated

### Step 5: Generate Test Previews (if selected)

**For each selected test plugin type:**

1. **Load test plugin spec:**

```bash
TEST_SPECS="/Users/lexchristopherson/Developer/plugin-freedom-system/.claude/skills/aesthetic-dreaming/assets/test-plugin-specs.json"
```

**Read spec for selected plugin type (e.g., "simple-compressor"):**

```json
{
  "id": "simple-compressor",
  "name": "Test Compressor",
  "parameters": [
    {
      "id": "THRESHOLD",
      "name": "Threshold",
      "type": "FLOAT",
      "range": [-60.0, 0.0],
      "default": -20.0,
      "prominent": false
    },
    {
      "id": "RATIO",
      "name": "Ratio",
      "type": "FLOAT",
      "range": [1.0, 20.0],
      "default": 4.0,
      "prominent": false
    },
    {
      "id": "ATTACK",
      "name": "Attack",
      "type": "FLOAT",
      "range": [0.1, 100.0],
      "default": 10.0,
      "prominent": false
    },
    {
      "id": "RELEASE",
      "name": "Release",
      "type": "FLOAT",
      "range": [10.0, 1000.0],
      "default": 100.0,
      "prominent": false
    }
  ]
}
```

2. **Invoke ui-template-library "apply" operation:**

```javascript
// Pseudo-code for invocation
ui_template_library.apply({
  aesthetic_id: "modern-professional-001",
  target_plugin_name: "TestCompressor",
  parameter_spec: test_plugin_spec,
  output_dir: ".claude/aesthetics/modern-professional-001/test-previews/",
  filename: "simple-compressor.html"
})
```

**Integration with ui-template-library:**
- Pass aesthetic ID and test plugin spec
- ui-template-library interprets aesthetic.md
- Chooses appropriate layout for parameter count
- Generates HTML with aesthetic applied
- Saves to test-previews/ subdirectory

3. **Repeat for each selected test plugin**

**Test preview directory structure:**

```
.claude/aesthetics/modern-professional-001/
├── aesthetic.md
├── metadata.json
└── test-previews/
    ├── simple-compressor.html
    ├── complex-reverb.html
    └── drum-machine.html
```

**Note:** No preview.html at root level (since no source plugin mockup)

### Step 6: Update Manifest

```bash
MANIFEST="/Users/lexchristopherson/Developer/plugin-freedom-system/.claude/aesthetics/manifest.json"
```

**Read existing manifest, append new aesthetic entry, write updated manifest:**

```json
{
  "version": "1.0",
  "aesthetics": [
    {
      "id": "modern-professional-001",
      "name": "Modern Professional",
      "description": "Dark modern aesthetic with blue accents",
      "sourcePlugin": null,
      "created": "2025-11-11T15:30:00Z",
      "tags": ["modern", "professional", "dark", "blue"]
    }
  ]
}
```

### Step 7: Confirmation

```
✅ Aesthetic created: "Modern Professional" (modern-professional-001)

Files created:
- .claude/aesthetics/modern-professional-001/aesthetic.md (prose description)
- .claude/aesthetics/modern-professional-001/metadata.json (metadata)

Test previews generated:
- .claude/aesthetics/modern-professional-001/test-previews/simple-compressor.html
- .claude/aesthetics/modern-professional-001/test-previews/complex-reverb.html

Preview test mockups:
  open .claude/aesthetics/modern-professional-001/test-previews/simple-compressor.html
  open .claude/aesthetics/modern-professional-001/test-previews/complex-reverb.html

You can now apply this aesthetic to any plugin:
- From ui-mockup skill: Choose "Start from aesthetic template"
- Or manually invoke: ui-template-library with "apply" operation
```

---

## Phase 6: Decision Menu

Present final options:

```
✅ Aesthetic "Modern Professional" ready!

What's next?
1. Open test previews - View mockups in browser
2. Refine aesthetic - Adjust and regenerate aesthetic.md
3. Create another aesthetic - Start new aesthetic
4. Apply to existing plugin - Choose plugin from PLUGINS.md
5. Done
```

**Option handling:**

- **Option 1: Open test previews**
  - Run `open` command for each test preview HTML
  - Return to decision menu

- **Option 2: Refine aesthetic**
  - Ask what to change (free-form)
  - Re-run question batches focusing on changed areas
  - Regenerate aesthetic.md (preserve aesthetic ID)
  - Optionally regenerate test previews

- **Option 3: Create another aesthetic**
  - Return to Phase 1 with fresh context

- **Option 4: Apply to existing plugin**
  - Read PLUGINS.md to list available plugins
  - User selects plugin
  - Invoke ui-mockup skill with aesthetic pre-selected
  - (ui-mockup will call ui-template-library "apply")

- **Option 5: Done**
  - Exit skill

---

## Adaptive Questioning Strategy

**Extract first, then fill gaps:**

1. User provides initial description
2. Parse response for covered visual concepts
3. Generate questions only for missing concepts
4. Present 4 questions via AskUserQuestion
5. After each batch, re-evaluate what's still missing
6. Present decision gate
7. Repeat until user finalizes

**Don't ask redundant questions.**

If user says "dark modern blue aesthetic with flat controls," don't ask:
- ❌ "What's the overall vibe?" (modern already stated)
- ❌ "Flat or 3D controls?" (flat already stated)
- ❌ "Color philosophy?" (blue already mentioned)

DO ask:
- ✓ "Specific shade of blue?"
- ✓ "Spacing density preference?"
- ✓ "Typography approach?"
- ✓ "Best suited for what plugin types?"

---

## Question Generation Examples

### Example 1: Detailed Input

```
User: "I want a vintage analog aesthetic with brass knobs, warm earth tones, and a subtle paper texture. Should feel like 1960s hardware."

Extracted:
- Vibe: Vintage analog, 1960s hardware ✓
- Color philosophy: Warm earth tones ✓
- Control style: Brass knobs ✓
- Surface: Paper texture ✓
- Typography: Not mentioned ✗
- Specific colors: Earth tones (generic) ✗
- Spacing: Not mentioned ✗
- Best for: Not mentioned ✗

Gaps identified (4 needed):
- Specific earth tone colors? (Tier 2)
- Typography approach? (Tier 2)
- Spacing density? (Tier 2)
- Best suited for what plugins? (Tier 3)

Question Batch 1:
1. "What specific earth tone palette?" → [Orange/brown/cream, Brown/tan/beige, Green/olive/khaki, Other]
2. "Typography approach?" → [Technical sans-serif, Classic serif, Vintage typewriter, Other]
3. "Spacing density?" → [Tight vintage, Comfortable balanced, Generous spacious, Other]
4. "Best suited for what plugins?" → [Tape/analog effects, Vintage compressors, All warm effects, Other]
```

### Example 2: Vague Input

```
User: "A clean modern aesthetic"

Extracted:
- Vibe: Clean, modern ✓
- Everything else: Not mentioned ✗

Gaps identified (4 needed):
- Color philosophy? (Tier 1)
- Control style? (Tier 1)
- Specific colors? (Tier 2)
- Typography? (Tier 2)

Question Batch 1:
1. "Color philosophy?" → [Dark with bright accents, Light with dark accents, Monochromatic, Other]
2. "Control styling?" → [Flat 2D, Subtle 3D, Skeuomorphic, Other]
3. "Primary color?" → [Blue professional, Gray neutral, Green organic, Other]
4. "Typography approach?" → [Technical/readable, Decorative/bold, Minimal/light, Other]

[Then decision gate]

If user chooses "Ask me 4 more questions":
- User answered: "Dark with bright accents", "Flat 2D", "Blue professional", "Technical/readable"

Updated context:
- Vibe: Clean, modern ✓
- Color: Dark with blue accents ✓
- Controls: Flat 2D ✓
- Typography: Technical/readable ✓

New gaps for Batch 2:
- Specific blue shade? (Tier 2)
- Spacing preference? (Tier 2)
- Surface treatment? (Tier 2)
- Best suited for? (Tier 3)
```

---

## Continuous Iteration Support

User can request deep dives:

```
User: "Ask me more about the color palette"
→ System focuses on color-specific questions

User: "Let's explore control styling in detail"
→ System asks about knob/slider/button specifics

User: "Tell me what you think about this aesthetic so far"
→ System provides summary and feasibility notes
```

**Support free-form exploration until user says "finalize."**

---

## Integration Points

**Invoked by:**
- `/dream` command (option 4: "Create aesthetic template")
- Natural language: "Create aesthetic template", "Design visual system"

**Invokes:**
- `ui-template-library` skill (apply operation for test previews)
- `ui-mockup` skill (option 4 in decision menu: apply to existing plugin)

**Integration with ui-mockup:**
When aesthetic-dreaming creates an aesthetic, it becomes immediately available in ui-mockup Phase 0 menu for applying to any plugin.

---

## Error Handling

**If aesthetic name contains invalid characters:**
```
Aesthetic names can contain spaces and normal characters.
They'll be slugified for the ID: "Modern Professional" → "modern-professional-001"

Suggested clean name: [CleanName]
Use this name? (y/n): _
```

**If aesthetic ID already exists:**
```
Aesthetic "modern-professional-001" already exists.

Options:
1. Create new version (modern-professional-002)
2. Choose different name
3. Overwrite existing (will lose existing aesthetic)

Choose (1-3): _
```

**If test plugin spec not found:**
```
Test plugin spec "[plugin-type]" not found in assets/test-plugin-specs.json.

Skipping this preview. Continue with other selected previews? (y/n): _
```

**If ui-template-library skill fails:**
```
Failed to generate test preview for "[plugin-type]".

Error: [error message]

Continue with remaining previews? (y/n): _
```

---

## Success Criteria

Skill is successful when:
- Aesthetic concept captured through adaptive questions
- No redundant questions asked (respects already-provided info)
- aesthetic.md generated with all 12 sections filled
- Test previews generated for selected plugin types (if any)
- Files stored in correct locations (.claude/aesthetics/[id]/)
- Aesthetic immediately available in ui-mockup skill
- User feels creative flow, not interrogated
- Clear next steps presented

---

## Implementation Notes

### Why This Approach Works

**1. Adaptive questioning matches plugin-ideation pattern:**
- Free-form collection first (capture everything)
- Gap analysis identifies missing pieces
- Questions generated contextually, not from rigid script
- Decision gate allows iteration or finalization

**2. Visual design tiers make sense:**
- Tier 1 (Critical): Vibe, color philosophy, control style
- Tier 2 (Visual Core): Specific colors, typography, spacing, textures
- Tier 3 (Context): Best suited for, inspirations, special features

**3. Test previews solve the visualization problem:**
- User can see aesthetic applied to real layouts
- Multiple parameter counts demonstrate flexibility
- Optional (user can skip if they just want prose description)

**4. Integration with ui-template-library:**
- aesthetic-dreaming creates the aesthetic.md
- ui-template-library interprets and applies it
- Separation of concerns: creation vs application

### Format Consistency

**aesthetic.md structure:**
- Exactly 12 sections in fixed order
- Prose descriptions with concrete examples
- Interpretable by both humans and Claude
- Human-editable, machine-parseable

**Test previews:**
- Stored in test-previews/ subdirectory
- Named by plugin type: simple-compressor.html
- Standalone HTML files (open in browser)
- Demonstrate aesthetic across different layouts

### Quality Control

**Before saving aesthetic:**
- ✅ Verify all template sections are present
- ✅ Check for placeholder text not replaced
- ✅ Validate color codes are valid hex/rgb
- ✅ Ensure prose is descriptive (not just values)
- ✅ Confirm aesthetic ID is unique

**Before generating test previews:**
- ✅ Verify test plugin specs exist
- ✅ Check ui-template-library skill is available
- ✅ Ensure aesthetic.md is complete
- ✅ Validate output directory exists

---

## Reference Documentation

**See also:**
- `ui-template-library` skill - Apply and save operations
- `ui-mockup` skill - Integration with Phase 0 aesthetic selection
- `plugin-ideation` skill - Adaptive questioning pattern reference
- `assets/test-plugin-specs.json` - Test plugin parameter definitions
- `references/aesthetic-questions.md` - Question bank and strategies
