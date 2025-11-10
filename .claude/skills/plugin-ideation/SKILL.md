---
name: plugin-ideation
description: Adaptive brainstorming for plugin concepts and improvements
allowed-tools:
  - Read
  - Write
  - Bash
preconditions:
  - None (entry point)
---

# plugin-ideation Skill

**Purpose:** Interactive brainstorming and creative vision capture for new plugins and improvements to existing plugins.

## Mode Detection

**Check if plugin exists:**

```bash
grep "^### $PLUGIN_NAME$" PLUGINS.md
```

- If found → **Improvement Mode**
- If not found → **New Plugin Mode**

## New Plugin Mode

### Phase 1: Free-Form Collection

Start with open question:
```
What would you like to build?

Tell me about your plugin idea. Share as much or as little as you want—I'll ask follow-ups for anything missing.
```

**Extract from response:**
- Plugin type (effect/synth/utility)
- Core concept and sonic goals
- Parameter ideas and ranges
- UI vision and layout preferences
- Use cases and target users
- Inspirations and references

### Phase 2: Targeted Refinement

Ask **only** about gaps not covered in Phase 1. One question at a time.

**If plugin type not mentioned:**
```
Is this an effect, synthesizer, or utility plugin?
```

**If core concept unclear:**
```
What's the main sonic character or processing goal?
```

**If parameters not discussed:**
```
What parameters would you like to control? (ranges, defaults)
```

**If UI not mentioned:**
```
Any preferences for the visual style or layout?
```

**If use cases unclear:**
```
What situations would you use this in? (mixing, production, creative effects, etc.)
```

**Adaptive listening:** Skip any question if information was already provided.

### Phase 3: Progress Checkpoints

After extracting ~5-7 pieces of information, show summary:

```
Here's what we have so far:

Plugin: [Name]
Type: [Effect/Synth/Utility]
Core Concept: [Description]
Parameters: [List]
UI Vision: [Description]
Use Cases: [Scenarios]

What would you like to do?
1. Continue exploring (dive deeper into specific areas)
2. Finalize creative brief (recommended if complete)
3. Ask me about [suggest area for expansion]
4. Other

Choose (1-4): _
```

If user chooses to continue, ask:
```
Which area would you like to explore more?
1. Parameters and ranges
2. UI and visual design
3. DSP approach and algorithms
4. Use cases and workflow
5. Inspirations and references
6. Other

Choose (1-6): _
```

### Phase 4: Document Creation

When user says "finalize" or chooses option 2, create:

**File:** `plugins/[PluginName]/.ideas/creative-brief.md`

**Format:**
```markdown
# [PluginName] - Creative Brief

## Overview

**Type:** [Effect/Synth/Utility]
**Core Concept:** [One-sentence description]
**Status:** 💡 Ideated
**Created:** [Date]

## Vision

[Prose description of plugin concept, sonic goals, inspiration]

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| [Name] | [Min-Max] | [Value] | [Purpose] |
| ... | ... | ... | ... |

## UI Concept

**Layout:** [Description]
**Visual Style:** [Description]
**Key Elements:** [List special UI components]

## Use Cases

- [Scenario 1]
- [Scenario 2]
- [Scenario 3]

## Inspirations

- [Plugin/hardware reference 1]
- [Plugin/hardware reference 2]
- [Sonic reference]

## Technical Notes

[Any specific DSP approaches, algorithms, or technical considerations mentioned]

## Next Steps

- [ ] Create UI mockup (`/dream [PluginName]` → option 3)
- [ ] Start implementation (`/implement [PluginName]`)
```

**Also update PLUGINS.md:**

Add entry if doesn't exist:
```markdown
### [PluginName]

**Status:** 💡 Ideated
**Type:** [Effect/Synth/Utility]
**Created:** [Date]
**Description:** [One-sentence summary]
```

### Phase 5: Session Handoff

Create `.continue-here.md` in `plugins/[PluginName]/.ideas/`:

**Format:**
```markdown
---
plugin: [PluginName]
stage: ideation
status: creative_brief_complete
last_updated: [YYYY-MM-DD HH:MM:SS]
---

# Resume Point

## Current State: Creative Brief Complete

Creative brief has been finalized for [PluginName]. Ready to proceed to UI mockup or implementation.

## Completed So Far

**Ideation:** ✓ Complete
- Core concept defined
- Parameters specified
- UI vision captured
- Use cases identified

## Next Steps

1. Create UI mockup to visualize design (recommended)
2. Start implementation directly
3. Research similar plugins for inspiration

## Context to Preserve

**Key Decisions:**
- Plugin type: [Effect/Synth/Utility]
- Core concept: [Brief summary]

**Files Created:**
- plugins/[PluginName]/.ideas/creative-brief.md
```

### Phase 6: Decision Menu

Present next steps:

```
✓ Creative brief complete: [PluginName]

What's next?
1. Create UI mockup (recommended) ← Visualize before building
2. Start implementation
3. Research similar plugins ← Find inspiration and examples
4. Save for later
5. Other

Choose (1-5): _
```

**Handle responses:**
- Option 1 → Invoke `ui-mockup` skill
- Option 2 → Invoke `plugin-workflow` skill (after warning about contracts)
- Option 3 → Invoke `deep-research` skill (Phase 7 - stub for now, just acknowledge)
- Option 4 → Confirm handoff file created, exit
- Option 5 → Ask what they'd like to do

## Improvement Mode

### Phase 0: Vagueness Detection

Check if request is specific:

**Request IS specific if it has:**
- Feature name (e.g., "resonance parameter")
- Action (e.g., "add", "fix", "change")
- Acceptance criteria (e.g., "range 0-1", "increase to 500px")

**Request IS vague if lacking above:**
- "improve the filters"
- "better presets"
- "UI feels cramped"

**If vague, present choice:**
```
Your request is somewhat vague. How should I proceed?

1. Brainstorm approaches first (recommended) ← Explore options together
2. Implement something reasonable ← I'll investigate and propose a solution

Choose (1-2): _
```

If option 1 chosen, continue with improvement brainstorming.
If option 2 chosen, exit to plugin-improve skill.

### Phase 1: Free-Form Collection

Ask:
```
What would you like to improve in [PluginName]?

Describe what you want to change, add, or fix. I'll ask follow-ups for anything missing.
```

**Extract from response:**
- What aspect (DSP/Parameters/UI/Features)
- Current pain point or limitation
- Proposed change
- Why this improvement matters
- Backward compatibility concerns
- How to test success

### Phase 2: Targeted Refinement

Ask **only** about gaps:

**If aspect unclear:**
```
Which aspect would you like to improve?
1. Audio processing (DSP)
2. Parameters (add/modify/remove)
3. User interface (layout/controls)
4. Features (presets, MIDI, etc.)
5. Performance or stability
6. Other
```

**If current state not described:**
```
What's the current behavior you want to change?
```

**If proposed change vague:**
```
Specifically, what should change? (be as detailed as possible)
```

**If testing criteria not mentioned:**
```
How will we know the improvement is successful?
```

**If backward compatibility not discussed:**
```
Should this maintain compatibility with existing presets/sessions?
```

### Phase 3: Progress Checkpoints

After extracting key information:

```
Here's what we have:

Plugin: [PluginName]
Improvement: [Name]
Aspect: [DSP/Parameters/UI/Features]
Current State: [Description]
Proposed Change: [Description]
Success Criteria: [How to verify]
Compatibility: [Breaking/Non-breaking]

What would you like to do?
1. Finalize improvement brief (recommended)
2. Explore more details
3. Other

Choose (1-3): _
```

### Phase 4: Document Creation

Create: `plugins/[PluginName]/.ideas/improvements/[feature-name].md`

**Format:**
```markdown
# [PluginName] - [Improvement Name]

**Created:** [Date]
**Type:** [Feature/Enhancement/Fix]
**Aspect:** [DSP/Parameters/UI/Features]
**Version Impact:** [PATCH/MINOR/MAJOR]

## Current State

[Description of current behavior or limitation]

## Proposed Change

[Detailed description of what should change]

## Rationale

[Why this improvement matters]

## Implementation Notes

[Technical considerations, files to modify, algorithms to use]

## Backward Compatibility

[Breaking/Non-breaking, migration strategy if needed]

## Testing Criteria

- [ ] [Test 1]
- [ ] [Test 2]
- [ ] [Test 3]

## Success Metrics

[How to know the improvement is complete and working]
```

### Phase 5: Session Handoff

Create `.continue-here.md` in `plugins/[PluginName]/.ideas/`:

```markdown
---
plugin: [PluginName]
stage: improvement_planning
status: improvement_brief_complete
improvement: [feature-name]
last_updated: [YYYY-MM-DD HH:MM:SS]
---

# Resume Point

## Current State: Improvement Brief Complete

Improvement proposal finalized for [PluginName]: [ImprovementName]

## Completed So Far

**Planning:** ✓ Complete
- Current state analyzed
- Proposed change defined
- Testing criteria established

## Next Steps

1. Start implementation (/improve [PluginName])
2. Research implementation approaches
3. Review existing code

## Context to Preserve

**Improvement:** [feature-name]
**Type:** [Feature/Enhancement/Fix]
**Version Impact:** [PATCH/MINOR/MAJOR]

**Files Created:**
- plugins/[PluginName]/.ideas/improvements/[feature-name].md
```

### Phase 6: Decision Menu

```
✓ Improvement brief complete: [ImprovementName]

What's next?
1. Start implementation (recommended)
2. Research implementation approaches ← Find examples and best practices
3. Review existing code first
4. Save for later
5. Other

Choose (1-5): _
```

**Handle responses:**
- Option 1 → Invoke `plugin-improve` skill
- Option 2 → Invoke `deep-research` skill (Phase 7 - stub)
- Option 3 → Read relevant source files, then re-present menu
- Option 4 → Confirm handoff file created, exit
- Option 5 → Ask what they'd like to do

## Vagueness Detection Rules

**Check for specificity:**

```
Specific indicators:
- Named feature ("resonance parameter", "bypass switch")
- Concrete action ("add", "remove", "change from X to Y")
- Measurable criteria ("range 0-1", "increase to 500px", "reduce by 3dB")

Vague indicators:
- Generic improvements ("better", "improve", "enhance")
- Unspecified targets ("the UI", "presets", "sound")
- No success criteria mentioned
```

If 2+ vague indicators and 0 specific indicators → Present brainstorm vs implement choice.

## Grounded Feasibility

**When user proposes ambitious ideas:**

Don't shut down creativity, but flag for research:

```
That's an interesting direction! [Specific technical consideration] might be complex—we can research approaches in Stage 0 (Research phase).

Continue exploring, or finalize brief with a research note?
```

Examples:
- Physical modeling → "Physical modeling can be CPU-intensive"
- Machine learning → "ML in real-time audio requires careful optimization"
- Complex visualizations → "3D graphics in plugins have rendering considerations"

**Gently note challenges without saying "no."**

## Adaptive Questioning Strategy

**Extract first, then fill gaps:**

1. User provides initial description
2. Parse response for covered topics
3. Generate questions only for missing topics
4. Present questions one at a time
5. After each answer, re-evaluate what's still missing
6. Stop when sufficient information gathered

**Don't ask redundant questions.**

If user says "I want a tape delay with wow and flutter, three knobs, vintage look," don't ask:
- ❌ "What type of plugin?" (it's a delay = effect)
- ❌ "What parameters?" (wow, flutter mentioned)
- ❌ "Visual style?" (vintage mentioned)

DO ask:
- ✓ "What should the third knob control?" (only two mentioned)
- ✓ "What range for wow/flutter parameters?"
- ✓ "Any specific tape references for the sound?"

## Continuous Iteration Support

User can request deep dives:

```
User: "Ask me more about the UI"
→ System focuses on UI-specific questions

User: "Let's explore presets"
→ System asks about preset strategy

User: "Tell me what you think about the DSP"
→ System provides feasibility analysis
```

**Support free-form exploration until user says "finalize."**

## Git Integration

After creating documents:

```bash
git add plugins/[PluginName]/.ideas/
git add PLUGINS.md
# Do NOT commit - user handles commits
```

**Stage files but don't commit.**

## Error Handling

**If plugin name contains invalid characters:**
```
Plugin names should be UpperCamelCase with no spaces or special characters.

Suggested: [CleanName]
Use this name? (y/n): _
```

**If improvement file already exists:**
```
Improvement proposal "[feature-name].md" already exists.

Options:
1. Create new version (/improve is better for implementing existing proposals)
2. Choose different name
3. Overwrite existing (not recommended)

Choose (1-3): _
```

**If creative brief already exists:**
```
Creative brief already exists for [PluginName].

Options:
1. View existing brief
2. Create improvement proposal instead (/improve)
3. Overwrite (will lose existing brief)

Choose (1-3): _
```

## Integration Points

**Invoked by:**
- `/dream` command (new plugin or improvement)
- `/dream [PluginName]` command
- Natural language: "I want to make...", "Explore improvements to..."

**Invokes:**
- `ui-mockup` skill (option after creative brief)
- `plugin-workflow` skill (option after creative brief)
- `plugin-improve` skill (option after improvement brief)
- `deep-research` skill (option for research) - Phase 7

## Success Criteria

Skill is successful when:
- Creative brief captures complete vision
- No redundant questions asked
- User feels heard and understood
- Document is actionable for implementation
- Handoff file enables resume
- Next steps are clear and discoverable
