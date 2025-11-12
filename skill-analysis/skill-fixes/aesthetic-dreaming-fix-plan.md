# Fix Plan: aesthetic-dreaming

## Summary
- Critical fixes: **11**
- Extraction operations: **4**
- Total estimated changes: **27**
- Estimated time: **65 minutes**

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Wrap Workflow Overview with Critical Sequence Enforcement
**Location:** Lines 22-34
**Operation:** WRAP
**Before:**
```markdown
## Workflow Overview

1. **Free-form collection** - "What aesthetic are you envisioning?"
2. **Gap analysis** - Extract covered concepts, identify missing visual elements
3. **Adaptive questioning** - Generate 4 contextual questions via AskUserQuestion
4. **Decision gate** - Finalize / Continue / Add context
5. **Loop until finalized** - Repeat phases 2-4 as needed
6. **Name aesthetic** - Suggest or accept custom name, generate unique ID
7. **Test preview selection** - Choose test plugin types for mockups (optional)
8. **Generate files** - aesthetic.md, metadata.json, test previews, update manifest
9. **Decision menu** - Open previews / Refine / Create another / Apply to plugin
```

**After:**
```xml
<workflow_overview>
<critical_sequence enforcement="strict">
This workflow MUST execute phases in order. Do NOT skip phases or proceed to finalization before completing gap analysis and question batches.

**Phase Flow:**
1. **Phase 1: Free-form collection** - Gather initial aesthetic vision
2. **Phase 2: Gap analysis** - Identify missing visual elements
3. **Phase 3: Question batch** - Ask 4 contextual questions via AskUserQuestion
4. **Phase 3.5: Decision gate** - Finalize / Continue / Add context
5. **Phase 3.7: Name aesthetic** (if finalizing)
6. **Phase 4: Test preview selection**
7. **Phase 5: Generate files**
8. **Phase 6: Decision menu**

**Loop Structure:**
Phases 2-3-3.5 repeat until user chooses "finalize" at decision gate.

**Critical Rules:**
- Do NOT skip phases
- Do NOT execute phases out of order
- Do NOT proceed to finalization before decision gate approval
- NEVER auto-proceed past decision gates
</critical_sequence>
</workflow_overview>
```

**Verification:** Confirm XML tags `<workflow_overview>` and `<critical_sequence enforcement="strict">` are present at lines 22 and 23.

### Fix 1.2: Wrap Phase 1 with XML Structure
**Location:** Lines 38-70
**Operation:** WRAP
**Before:**
```markdown
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

**Extract all mentioned visual concepts:**

**Extraction targets:**

- **Visual Identity:** Overall vibe, mood, design philosophy
- **Color System:** Palette, warm/cool, contrast, specific colors mentioned
- **Typography:** Font choices, sizing philosophy
- **Control Styling:** Knobs, sliders, buttons, rotaries
- **Spacing:** Density, breathing room, layout approach
- **Surface Treatment:** Textures, shadows, depth, glossy vs. matte
- **Details:** Special features, animations, microinteractions
- **Best For:** Plugin types, use cases mentioned
- **Inspirations:** References to existing designs

**Capture everything mentioned verbatim in accumulated context.**
```

**After:**
```xml
<phase id="1" name="free_form_collection">
<instructions>
Present open invitation to user. Extract all mentioned visual concepts verbatim and accumulate in context.
</instructions>

<required_prompt>
What aesthetic are you envisioning?

Describe the visual design system you want to create—share as much or as little as you want. I'll ask follow-ups for anything missing.

Consider: overall vibe, color palette, control styling, spacing, textures, typography, and plugin types this would suit.

You can reference existing designs, describe feelings, or provide technical specifics.
</required_prompt>

<extraction_targets>
- Visual Identity: Overall vibe, mood, design philosophy
- Color System: Palette, warm/cool, contrast, specific colors
- Typography: Font choices, sizing philosophy
- Control Styling: Knobs, sliders, buttons, rotaries
- Spacing: Density, breathing room, layout approach
- Surface Treatment: Textures, shadows, depth, glossy vs. matte
- Details: Special features, animations, microinteractions
- Best For: Plugin types, use cases
- Inspirations: References to existing designs
</extraction_targets>

<state_requirement>
MUST accumulate all extracted concepts in persistent context. This context carries forward through ALL phases and question batches. NEVER lose information from previous phases.
</state_requirement>
</phase>
```

**Verification:** Confirm `<phase id="1">` tag at line 38 and `</phase>` closing tag at line 70.

### Fix 1.3: Wrap Phase 2 with XML Structure and Anti-Pattern Rules
**Location:** Lines 72-112
**Operation:** WRAP
**Before:**
```markdown
## Phase 2: Gap Analysis

**Parse user's accumulated context:**

1. **Read everything collected so far** (from Phase 1 + any previous question batches)
2. **Check coverage against tier priority system:**
   - **Tier 1 (Critical):** Overall vibe/mood, color philosophy, control style
   - **Tier 2 (Visual Core):** Specific colors, typography, spacing, textures, control details
   - **Tier 3 (Context):** Best suited for, special features, inspirations
3. **Identify gaps:** What's missing from each tier?
4. **Prioritize questions:** Focus on highest-tier gaps first

**Anti-pattern example (avoid asking redundant questions):**

If user stated: "I want a dark, modern aesthetic with blue accents and flat controls"

**DO NOT ASK:**

- "What's the overall vibe?" (modern already stated)
- "Should it be light or dark?" (dark already stated)
- "What accent color?" (blue already stated)
- "Flat or skeuomorphic?" (flat already stated)

**Instead, focus on uncovered gaps:**

- Typography choices?
- Spacing philosophy?
- Surface textures?
- Best plugin types?

**Questions should extract NEW information, not repeat what user already provided.**
```

**After:**
```xml
<phase id="2" name="gap_analysis">
<critical_sequence>
1. Parse user's accumulated context (from Phase 1 + any previous question batches)
2. Check coverage against tier priority system
3. Identify top 4 gaps by tier priority
4. NEVER ask about already-provided information
</critical_sequence>

<tier_system>
**Tier 1 (Critical):** Overall vibe/mood, color philosophy, control style
**Tier 2 (Visual Core):** Specific colors, typography, spacing, textures, control details
**Tier 3 (Context):** Best suited for, special features, inspirations
</tier_system>

<anti_pattern>
NEVER ask redundant questions. If user stated "dark modern blue", do NOT ask:
- "What's the overall vibe?" (modern already stated)
- "Light or dark?" (dark already stated)
- "What accent color?" (blue already stated)

Instead focus on uncovered gaps: typography, spacing, textures, plugin types.
</anti_pattern>

<delegation_rule>
Detailed question banks are in references/aesthetic-questions.md. Reference for gap analysis strategies and question options.
</delegation_rule>
</phase>
```

**Verification:** Confirm `<phase id="2">` tag at line 72, `<anti_pattern>` tag includes redundancy rules, and `</phase>` closing tag at line 112.

### Fix 1.4: Wrap Phase 3 with XML Structure (Question Batch)
**Location:** Lines 114-233
**Operation:** WRAP + REFERENCE
**Before:**
```markdown
## Phase 3: Adaptive Questioning

**Generate exactly 4 questions via AskUserQuestion tool:**

**Tool format:**

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
          label: "Skeuomorphic",
          description: "Physical hardware realism"
        },
        {
          label: "Soft/Neumorphic",
          description: "Subtle shadows and depth"
        }
      ]
    },
    // ... repeat for 3 more questions
  ]
})
```

[... extensive examples continue for ~120 lines ...]

**After receiving answers:**

1. Accumulate context with previous responses
2. Re-analyze gaps
3. Proceed to decision gate
```

**After:**
```xml
<phase id="3" name="question_batch">
<instructions>
Generate exactly 4 questions using AskUserQuestion tool based on gaps identified in Phase 2.
</instructions>

<requirements>
- MUST use AskUserQuestion tool (not inline menu)
- Provide 3-4 meaningful options per question (not just open text)
- Always include "Other" option for custom input
- Users can type "skip" in "Other" to skip questions
- Questions MUST target identified gaps, prioritized by tier
</requirements>

<tool_usage>
Tool: AskUserQuestion
Parameters:
  - questions: array[1-4] of question objects
  - Each question has: question, header, multiSelect, options[2-4]
  - Always include "Other" option automatically
</tool_usage>

<example>
See references/question-examples.md for detailed AskUserQuestion formatting examples and question banks by category.
</example>

<state_transition>
After receiving question batch answers:
1. Merge answers with accumulated context (cumulative, never overwrite)
2. Proceed DIRECTLY to decision gate (Phase 3.5)
3. Decision gate will route to Phase 2 (re-analyze) or Phase 3.7 (finalize)
</state_transition>
</phase>
```

**Verification:** Confirm `<phase id="3">` tag at line 114, reference to `question-examples.md` is present, and verbose examples are removed.

### Fix 1.5: Wrap Phase 3.5 Decision Gate with Enforcement Tags
**Location:** Lines 235-287
**Operation:** WRAP
**Before:**
```markdown
## Phase 3.5: Decision Gate

**Present decision via AskUserQuestion:**

```javascript
AskUserQuestion({
  questions: [
    {
      question: "Do you have enough information to finalize this aesthetic template?",
      header: "Ready?",
      multiSelect: false,
      options: [
        {
          label: "Yes, finalize it",
          description: "Proceed to naming and file generation"
        },
        {
          label: "Ask me 4 more questions",
          description: "Continue the discovery loop"
        },
        {
          label: "Let me add more context first",
          description: "I'll provide additional details before next question batch"
        }
      ]
    }
  ]
})
```

**Routing logic:**

- **Option 1:** "Yes, finalize it" → Proceed to Phase 3.7 (Name aesthetic)
- **Option 2:** "Ask me 4 more questions" → Return to Phase 2 (re-analyze gaps with accumulated context)
- **Option 3:** "Let me add more context first" → Collect free-form input, merge with context, return to Phase 2

**Context accumulation:**

All context is cumulative. Never lose information from previous phases or question batches.
```

**After:**
```xml
<decision_gate id="finalize_or_continue" phase="3.5" enforcement="blocking">
<critical_sequence>
MUST present this decision gate after EVERY question batch (Phase 3).
MUST use AskUserQuestion tool (not inline numbered list).
MUST wait for user response before proceeding.
</critical_sequence>

<required_options>
1. "Yes, finalize it" → Proceed to Phase 3.7 (name aesthetic)
2. "Ask me 4 more questions" → Return to Phase 2 (re-analyze gaps)
3. "Let me add more context first" → Collect free-form text, return to Phase 2
</required_options>

<tool_format>
AskUserQuestion({
  questions: [{
    question: "Do you have enough information to finalize this aesthetic template?",
    header: "Ready?",
    multiSelect: false,
    options: [
      { label: "Yes, finalize it", description: "Proceed to naming and file generation" },
      { label: "Ask me 4 more questions", description: "Continue the discovery loop" },
      { label: "Let me add more context first", description: "I'll provide additional details before next question batch" }
    ]
  }]
})
</tool_format>

<routing_logic>
IF option 1 THEN proceed to Phase 3.7
ELSE IF option 2 THEN return to Phase 2 (with accumulated context)
ELSE IF option 3 THEN collect user input, merge with context, return to Phase 2
</routing_logic>

<state_requirement>
Context accumulation is cumulative across all iterations. Never lose previously collected information.
</state_requirement>
</decision_gate>
```

**Verification:** Confirm `<decision_gate>` tag has `enforcement="blocking"` attribute at line 235.

### Fix 1.6: Wrap Phase 3.7 with Conditional Execution Tags
**Location:** Lines 289-336
**Operation:** WRAP + CONDENSE
**Before:**
```markdown
## Phase 3.7: Name the Aesthetic

**This phase only runs if user chose "finalize" at decision gate.**

**Check if name already provided:**

If the user's initial description included a name (e.g., "I want a 'vintage tape' aesthetic"), skip this phase and use that name.

Otherwise, suggest 3 names based on accumulated context:

**Name generation approach:**

1. **Extract key descriptors:** Dominant colors, vibe words, style references
2. **Generate 3 suggestions:**
   - Descriptive (e.g., "Warm Analog Brass")
   - Abstract/Evocative (e.g., "Golden Age")
   - Technical/Style (e.g., "Vintage Hardware")

[... extensive examples continue for ~30 lines ...]

**Slugify name:**

- Convert to lowercase
- Replace spaces with hyphens
- Remove special characters
- Append counter if ID collision: "modern-professional-001"
```

**After:**
```xml
<phase id="3.7" name="name_aesthetic">
<preconditions>
Only execute if user chose "finalize" at decision gate (Phase 3.5).
</preconditions>

<conditional_execution>
IF aesthetic name already provided in initial description THEN skip this phase and proceed to Phase 4.
ELSE ask user for name via AskUserQuestion with 3 suggested names + "Other" option.
</conditional_execution>

<name_generation_rules>
- Suggest 3 names based on accumulated context (vibe + colors + style)
- Examples: "Dark modern blue" → "Modern Professional", "Blue Tech", "Dark Elegance"
- Allow user to provide custom name via "Other" option
</name_generation_rules>

<slug_generation>
- Slugify name: "Modern Professional" → "modern-professional"
- Check for existing IDs in manifest.json and append counter: "modern-professional-001"
- Increment until unique ID found
</slug_generation>
</phase>
```

**Verification:** Confirm `<phase id="3.7">` tag has `<conditional_execution>` block that checks for existing name.

### Fix 1.7: Wrap Phase 4 with XML Structure
**Location:** Lines 338-384
**Operation:** WRAP
**Before:**
```markdown
## Phase 4: Test Preview Selection

**Ask user which test plugin types to generate:**

```javascript
AskUserQuestion({
  questions: [
    {
      question: "Which test plugin mockups would you like to generate?",
      header: "Test Previews",
      multiSelect: true, // User can select 0-4
      options: [
        {
          label: "Simple Compressor",
          description: "4 params: Threshold, Ratio, Attack, Release"
        },
        {
          label: "Complex Reverb",
          description: "12 params: Size, Damping, Mix, Pre-delay, etc."
        },
        {
          label: "Drum Machine",
          description: "16 pads + velocity + sequencer controls"
        },
        {
          label: "Simple Synth",
          description: "Oscillators, filters, envelopes"
        }
      ]
    }
  ]
})
```

**If user selects 0 options:** Skip test preview generation entirely (proceed to Phase 5 with aesthetic.md only)

**If user selects 1-4 options:** Generate HTML mockups for selected types (Phase 5)
```

**After:**
```xml
<phase id="4" name="test_preview_selection">
<instructions>
Present test preview selection menu using AskUserQuestion with multiSelect: true.
</instructions>

<requirements>
- MUST use AskUserQuestion tool with multiSelect: true
- Offer 4 test plugin types: simple-compressor, complex-reverb, drum-machine, simple-synth
- User can select 0-4 options (0 = skip previews entirely)
</requirements>

<tool_format>
AskUserQuestion({
  questions: [{
    question: "Which test plugin mockups would you like to generate?",
    header: "Test Previews",
    multiSelect: true,
    options: [
      { label: "Simple Compressor", description: "4 params: Threshold, Ratio, Attack, Release" },
      { label: "Complex Reverb", description: "12 params: Size, Damping, Mix, Pre-delay, etc." },
      { label: "Drum Machine", description: "16 pads + velocity + sequencer controls" },
      { label: "Simple Synth", description: "Oscillators, filters, envelopes" }
    ]
  }]
})
</tool_format>

<routing_logic>
IF user selects 0 options THEN skip test preview generation, proceed directly to Phase 5 (aesthetic.md only)
ELSE IF user selects 1-4 options THEN proceed to Phase 5 with selected preview types
</routing_logic>

<state_requirement>
Store selected preview types in context for Phase 5 generation.
</state_requirement>
</phase>
```

**Verification:** Confirm `<phase id="4">` tag and `multiSelect: true` is documented in requirements.

### Fix 1.8: Wrap Phase 5 with Critical Sequence Tags
**Location:** Lines 386-613
**Operation:** WRAP + REFERENCE
**Before:**
```markdown
## Phase 5: Generate Files

**Execute steps in order:**

### Step 1: Read Aesthetic Template

**Template path:**

```bash
TEMPLATE_PATH="/Users/lexchristopherson/Developer/plugin-freedom-system/.claude/skills/ui-template-library/assets/aesthetic-template.md"
```

Read template to understand 12-section structure.

### Step 2: Generate Structured Prose

**Prose generation principles:**

- **Be interpretable, not rigid:** "Generous spacing with 32px gaps" not "spacing: 32px"
- **Capture intent:** "Warm brass controls evoking vintage hardware" not "color: #d4a574"
- **Explain WHY:** Describe reasoning behind design choices
- **Include examples:** Balance prose with concrete values (colors, sizes)
- **Follow template structure:** All 12 sections, same order, every time

[... extensive prose examples and test preview protocol continue for ~220 lines ...]
```

**After:**
```xml
<phase id="5" name="generate_files">
<critical_sequence enforcement="strict" allow_reordering="false">
MUST execute steps in this exact order (no parallelization):
1. Read template
2. Generate prose (requires template)
3. Write aesthetic.md (requires prose)
4. Generate metadata.json (requires aesthetic ID)
5. Generate test previews (requires aesthetic.md completion)
6. Update manifest (requires metadata.json)
7. Present confirmation (requires all files created)
</critical_sequence>

<step id="1">
Read template: /Users/lexchristopherson/Developer/plugin-freedom-system/.claude/skills/ui-template-library/assets/aesthetic-template.md
</step>

<step id="2">
<prose_generation_requirements>
Transform accumulated context into interpretable prose, NOT rigid specifications.

MUST follow these principles:
- Be descriptive: "Generous spacing with 32px gaps" NOT "spacing: 32px"
- Capture intent: "Warm brass controls evoking vintage hardware" NOT "color: #d4a574"
- Explain WHY: Describe reasoning behind design choices
- Include examples: Balance prose with concrete values (colors, sizes)
- Follow template structure: All 12 sections, same order, every time

See ui-template-library/assets/aesthetic-template.md for exact section structure.
</prose_generation_requirements>
</step>

<step id="3">
Write aesthetic.md to: .claude/aesthetics/[aesthetic-id]/aesthetic.md
</step>

<step id="4">
Generate metadata.json with: id, name, description, created timestamp, tags (inferred from context), isTemplateOnly: true, testPreviews array
</step>

<step id="5">
<conditional_execution>
IF test previews selected in Phase 4 THEN:
  FOR EACH selected plugin type:
    - Read spec from assets/test-plugin-specs.json
    - Invoke ui-template-library skill "apply" operation
    - Save HTML to .claude/aesthetics/[id]/test-previews/[plugin-type].html
  END FOR
END IF
</conditional_execution>
See references/test-preview-protocol.md for detailed invocation protocol.
</step>

<step id="6">
Read .claude/aesthetics/manifest.json, append new aesthetic entry, write updated manifest.
</step>

<step id="7">
Present confirmation with file paths and preview open commands.
</step>
</phase>
```

**Verification:** Confirm `<critical_sequence enforcement="strict" allow_reordering="false">` attribute at line 386. Verify reference to `test-preview-protocol.md`.

### Fix 1.9: Wrap Phase 6 with Checkpoint Protocol Tags
**Location:** Lines 617-655
**Operation:** WRAP
**Before:**
```markdown
## Phase 6: Decision Menu

**Present final options:**

```
✅ Aesthetic "Modern Professional" ready!

What's next?
1. Open test previews - View mockups in browser
2. Refine aesthetic - Adjust and regenerate aesthetic.md
3. Create another aesthetic - Start new aesthetic
4. Apply to existing plugin - Choose plugin from PLUGINS.md
5. Done

Choose (1-5): _
```

**Option handling:**

1. **Open test previews:** Run `open` commands for test preview HTML files, return to menu
2. **Refine aesthetic:** Ask what to change, re-run question batches (Phases 2-3-3.5), regenerate aesthetic.md (preserve ID)
3. **Create another aesthetic:** Return to Phase 1 with fresh context
4. **Apply to existing plugin:** Read PLUGINS.md, user selects plugin, invoke ui-mockup skill with aesthetic pre-selected
5. **Done:** Exit skill

**Wait for user response. Never auto-proceed.**
```

**After:**
```xml
<phase id="6" name="decision_menu">
<checkpoint_protocol>
This is a system checkpoint. MUST follow Plugin Freedom System checkpoint protocol.
MUST use inline numbered list format (NOT AskUserQuestion tool) per CLAUDE.md checkpoint protocol.
</checkpoint_protocol>

<decision_menu format="inline_numbered_list">
✅ Aesthetic "[Name]" ready!

What's next?
1. Open test previews - View mockups in browser
2. Refine aesthetic - Adjust and regenerate aesthetic.md
3. Create another aesthetic - Start new aesthetic
4. Apply to existing plugin - Choose plugin from PLUGINS.md
5. Done

Choose (1-5): _
</decision_menu>

<routing_logic>
Option 1: Run `open` commands for test preview HTML files, return to menu
Option 2: Ask what to change, re-run question batches (Phases 2-3-3.5), regenerate aesthetic.md (preserve ID)
Option 3: Return to Phase 1 with fresh context
Option 4: Read PLUGINS.md, user selects plugin, invoke ui-mockup skill with aesthetic pre-selected
Option 5: Exit skill
</routing_logic>

<note>
Phase 3.5 and 3.7 use AskUserQuestion because they are INTERNAL decision gates (workflow branching).
Phase 6 uses inline numbered list because it is a SYSTEM CHECKPOINT (user discovers next actions).
</note>

<state_requirement>
MUST wait for user response. NEVER auto-proceed to any option.
</state_requirement>
</phase>
```

**Verification:** Confirm `<checkpoint_protocol>` tag documents distinction between internal gates (AskUserQuestion) and system checkpoints (inline numbered lists).

### Fix 1.10: Add Handoff Protocol with Preconditions/Postconditions
**Location:** Lines 777-787
**Operation:** WRAP + EXPAND
**Before:**
```markdown
**Invoked by:**
- `/dream` command (option: "Create aesthetic template")
- Natural language: "Create aesthetic template", "Design visual system"
- Direct skill invocation: `Skill("aesthetic-dreaming")`

**Invokes:**
- `ui-template-library` skill (apply operation for test previews)
- `ui-mockup` skill (option 4 in decision menu: apply to existing plugin)

**Integration notes:**
When aesthetic-dreaming creates an aesthetic, it becomes immediately available in ui-mockup Phase 0 aesthetic selection menu.
```

**After:**
```xml
<handoff_protocol>
<invoked_by>
- /dream command (option: "Create aesthetic template")
- Natural language: "Create aesthetic template", "Design visual system"
- Direct skill invocation: Skill("aesthetic-dreaming")
</invoked_by>

<invokes>
<skill name="ui-template-library">
  <operation>apply</operation>
  <context>Phase 5, Step 5: Test preview generation</context>

  <preconditions>
    MUST verify before invocation:
    - aesthetic.md exists at .claude/aesthetics/[id]/aesthetic.md
    - Test plugin spec exists in assets/test-plugin-specs.json
    - Aesthetic ID is valid and unique
    - Output directory .claude/aesthetics/[id]/test-previews/ exists
  </preconditions>

  <invocation_parameters>
    - aesthetic_id: string (e.g., "modern-professional-001")
    - target_plugin_name: string (e.g., "TestCompressor")
    - parameter_spec: object (from test-plugin-specs.json)
    - output_dir: string (.claude/aesthetics/[id]/test-previews/)
    - filename: string (e.g., "simple-compressor.html")
  </invocation_parameters>

  <postconditions>
    Verify after invocation:
    - HTML file created at specified path
    - File is valid standalone HTML (can be opened in browser)
    - Success status returned from skill
  </postconditions>

  <error_handling>
    IF skill invocation fails:
    - Log error message
    - Continue with remaining previews (don't abort entire Phase 5)
    - Report failure in Phase 5 confirmation message
  </error_handling>
</skill>

<skill name="ui-mockup">
  <operation>create_mockup_with_aesthetic</operation>
  <context>Phase 6, Option 4: Apply to existing plugin</context>

  <preconditions>
    MUST verify before invocation:
    - Plugin exists in PLUGINS.md
    - Plugin has parameter-spec.md
    - Aesthetic exists in manifest.json
  </preconditions>

  <handoff_data>
    Pass to ui-mockup:
    - aesthetic_id: string (pre-select in Phase 0)
    - plugin_name: string (user selected from PLUGINS.md)
  </handoff_data>

  <postconditions>
    ui-mockup skill takes over:
    - Loads aesthetic.md for interpretation
    - Generates mockup with aesthetic applied
    - aesthetic-dreaming skill exits after handoff
  </postconditions>
</skill>
</invokes>

<integration_notes>
When aesthetic-dreaming creates an aesthetic, it becomes immediately available in ui-mockup Phase 0 aesthetic selection menu.
</integration_notes>
</handoff_protocol>
```

**Verification:** Confirm `<handoff_protocol>` includes `<preconditions>`, `<postconditions>`, and `<error_handling>` blocks for both skills.

### Fix 1.11: Add Adaptive Questioning Strategy Structure
**Location:** Lines 658-683
**Operation:** WRAP
**Before:**
```markdown
## Adaptive Questioning Strategy

**Extract first, then fill gaps:**

1. User provides initial description
2. Parse response for covered visual concepts
3. Identify what's missing (using tier priority)
4. Generate questions targeting gaps
5. Repeat until user finalizes

**Key principle:** Never ask about information already provided.

**Example:**

User says: "I want a sleek, dark interface with neon blue accents and flat modern controls."

**Extraction:**
- Vibe: sleek, dark, modern
- Color: neon blue accents
- Controls: flat

**Gaps identified (by tier):**
- Typography choices? (Tier 2)
- Spacing philosophy? (Tier 2)
- Surface textures? (Tier 2)
- Best plugin types? (Tier 3)

**Generate 4 questions targeting these gaps.**
```

**After:**
```xml
<adaptive_questioning_strategy>
<critical_sequence>
This is the core algorithm for gap-driven questioning:

1. Extract concepts from user input (Phase 1 or additional context)
2. Check coverage against tier system (Tier 1 → Tier 2 → Tier 3)
3. Identify gaps (missing concepts by tier priority)
4. Generate 4 questions targeting highest-priority gaps (Phase 3)
5. Present decision gate (Phase 3.5)
6. IF continue THEN repeat steps 2-5 with accumulated context
7. IF finalize THEN proceed to Phase 3.7

NEVER ask about concepts already provided.
NEVER repeat questions in different words.
ALWAYS extract maximum information from each user response.
</critical_sequence>

<example>
User says: "I want a sleek, dark interface with neon blue accents and flat modern controls."

**Extraction:**
- Vibe: sleek, dark, modern
- Color: neon blue accents
- Controls: flat

**Gaps identified (by tier):**
- Typography choices? (Tier 2)
- Spacing philosophy? (Tier 2)
- Surface textures? (Tier 2)
- Best plugin types? (Tier 3)

**Generate 4 questions targeting these gaps.**
</example>

See references/aesthetic-questions.md for detailed gap analysis strategies.
</adaptive_questioning_strategy>
```

**Verification:** Confirm `<critical_sequence>` tag at line 658 documents the core algorithm with 7-step process and NEVER/ALWAYS rules.

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Extract Question Batch Examples to References
**Location:** Lines 163-227
**Operation:** EXTRACT
**Create file:** `references/question-examples.md`

**File content:**
```markdown
# Question Batch Examples

## Full AskUserQuestion Example

### Example 1: Control Styling Question

```javascript
AskUserQuestion({
  questions: [
    {
      question: "What control styling approach fits your vision?",
      header: "Controls",
      multiSelect: false,
      options: [
        {
          label: "Flat 2D",
          description: "Minimal depth, clean lines, modern aesthetic"
        },
        {
          label: "Skeuomorphic",
          description: "Physical hardware realism with shadows and depth"
        },
        {
          label: "Soft/Neumorphic",
          description: "Subtle shadows and depth, soft appearance"
        },
        {
          label: "Glassy/Translucent",
          description: "Transparency effects, light reflections"
        }
      ]
    }
  ]
})
```

### Example 2: Color Palette Question

```javascript
AskUserQuestion({
  questions: [
    {
      question: "What color palette approach?",
      header: "Colors",
      multiSelect: false,
      options: [
        {
          label: "Monochromatic",
          description: "Single hue with varying brightness"
        },
        {
          label: "Complementary",
          description: "Opposing colors for high contrast"
        },
        {
          label: "Analogous",
          description: "Adjacent colors for harmony"
        },
        {
          label: "Triadic",
          description: "Three evenly spaced colors"
        }
      ]
    }
  ]
})
```

### Example 3: Typography Question

```javascript
AskUserQuestion({
  questions: [
    {
      question: "What typography style fits your aesthetic?",
      header: "Typography",
      multiSelect: false,
      options: [
        {
          label: "Sans-serif modern",
          description: "Clean, contemporary, highly readable"
        },
        {
          label: "Monospace technical",
          description: "Fixed-width, developer-style"
        },
        {
          label: "Serif classic",
          description: "Traditional, sophisticated"
        },
        {
          label: "Display/Custom",
          description: "Unique, stylized, brand-specific"
        }
      ]
    }
  ]
})
```

### Example 4: Spacing Philosophy Question

```javascript
AskUserQuestion({
  questions: [
    {
      question: "What's your spacing philosophy?",
      header: "Spacing",
      multiSelect: false,
      options: [
        {
          label: "Compact/Dense",
          description: "More controls visible, efficient use of space"
        },
        {
          label: "Balanced",
          description: "Moderate spacing, neither cramped nor sparse"
        },
        {
          label: "Generous/Spacious",
          description: "Breathing room, minimalist feel"
        },
        {
          label: "Adaptive",
          description: "Varies by plugin complexity"
        }
      ]
    }
  ]
})
```

## Question Generation Strategies

### Strategy 1: Tier-Based Prioritization

Always prioritize gaps by tier:
1. **Tier 1 (Critical)**: Overall vibe/mood, color philosophy, control style
2. **Tier 2 (Visual Core)**: Specific colors, typography, spacing, textures, control details
3. **Tier 3 (Context)**: Best suited for, special features, inspirations

### Strategy 2: Context-Aware Phrasing

Tailor questions to user's accumulated context:
- If user mentioned "vintage", ask about specific era references
- If user mentioned "technical", ask about precision vs. approachability
- If user mentioned specific plugin type, ask about typical use cases

### Strategy 3: Option Design

For each question:
- Provide 3-4 distinct, non-overlapping options
- Include descriptions that explain the choice
- Avoid jargon unless user demonstrated technical fluency
- "Other" option is automatically included by tool

### Strategy 4: Redundancy Prevention

Before generating questions:
1. List all concepts already mentioned
2. Cross-reference against tier checklist
3. Only ask about uncovered gaps
4. Never rephrase a previous question

## Full Question Banks

See references/aesthetic-questions.md for comprehensive question banks organized by category (controls, colors, typography, spacing, textures, special features).
```

**Replace lines 163-227 in SKILL.md with:**
```xml
<example>
See references/question-examples.md for detailed AskUserQuestion formatting examples and question banks by category.
</example>
```

**Verification:** File `references/question-examples.md` exists and contains ~3,000 tokens of examples. SKILL.md reduced by ~65 lines.

### Fix 2.2: Extract Test Preview Protocol to References
**Location:** Lines 486-566
**Operation:** EXTRACT
**Create file:** `references/test-preview-protocol.md`

**File content:**
```markdown
# Test Preview Generation Protocol

## Overview

Test previews are standalone HTML mockups that demonstrate how the aesthetic would look applied to different plugin types. They use the ui-template-library skill to generate visual representations.

## Step-by-Step Process

### 1. Load Test Plugin Spec

**Spec location:**
```bash
/Users/lexchristopherson/Developer/plugin-freedom-system/.claude/skills/aesthetic-dreaming/assets/test-plugin-specs.json
```

**Read spec for selected plugin type:**

```javascript
const specs = JSON.parse(fs.readFileSync(TEST_SPECS_PATH))
const spec = specs[pluginType] // e.g., "simple-compressor"
```

**Spec structure:**
```json
{
  "id": "simple-compressor",
  "name": "Test Compressor",
  "description": "Classic dynamic range compressor",
  "parameters": [
    {
      "id": "threshold",
      "name": "Threshold",
      "type": "slider",
      "range": { "min": -60, "max": 0, "default": -20 },
      "unit": "dB"
    },
    {
      "id": "ratio",
      "name": "Ratio",
      "type": "knob",
      "range": { "min": 1, "max": 20, "default": 4 },
      "unit": ":1"
    },
    // ... more parameters
  ]
}
```

### 2. Invoke ui-template-library Skill

**Invocation pattern:**

```javascript
Skill("ui-template-library", {
  operation: "apply",
  aesthetic_id: "modern-professional-001",
  target_plugin_name: "TestCompressor",
  parameter_spec: spec,
  output_dir: ".claude/aesthetics/modern-professional-001/test-previews/",
  filename: "simple-compressor.html"
})
```

**Parameters explained:**
- `aesthetic_id`: Unique ID from Phase 3.7 slug generation
- `target_plugin_name`: From spec.name
- `parameter_spec`: Full spec object from test-plugin-specs.json
- `output_dir`: Directory for HTML file (must exist before invocation)
- `filename`: Output filename (plugin-type.html)

### 3. Skill Processing

The ui-template-library skill will:
1. Read aesthetic.md from `.claude/aesthetics/[id]/aesthetic.md`
2. Parse 12 design system sections
3. Apply prose guidance to parameter spec
4. Generate HTML with inline CSS
5. Write standalone HTML file to output_dir

### 4. Verify Output

**Check for:**
- File exists at `.claude/aesthetics/[id]/test-previews/[plugin-type].html`
- File is valid HTML (not empty or error message)
- File can be opened in browser with `open` command

**Example verification:**
```bash
ls -lh .claude/aesthetics/modern-professional-001/test-previews/simple-compressor.html
# Should show file size > 10KB

open .claude/aesthetics/modern-professional-001/test-previews/simple-compressor.html
# Should open in default browser
```

### 5. Error Handling

**If invocation fails:**
1. Log error message with plugin type and aesthetic ID
2. Continue with remaining preview types (don't abort entire Phase 5)
3. Include failure note in Phase 5 confirmation message

**Common errors:**
- Aesthetic.md not found → Check ID and file path
- Invalid parameter spec → Verify test-plugin-specs.json structure
- ui-template-library skill not available → Check skill installation
- Output directory missing → Create directory before invocation

## Test Plugin Types

### simple-compressor
- 4 parameters: Threshold, Ratio, Attack, Release
- Classic dynamics processor layout
- Good for testing: knobs, sliders, meters

### complex-reverb
- 12 parameters: Size, Damping, Mix, Pre-delay, Early reflections, etc.
- Multi-section layout with tabs/groups
- Good for testing: grouping, spacing, visual hierarchy

### drum-machine
- 16 pads + velocity + sequencer controls
- Grid-based layout with interactive elements
- Good for testing: color system, button states, density

### simple-synth
- Oscillators, filters, envelopes, LFOs
- Multi-module layout with routing
- Good for testing: module organization, signal flow visualization

## Integration Notes

- Test previews are optional (user can skip in Phase 4)
- Previews are purely visual (no actual audio processing)
- Each preview is a standalone HTML file (no dependencies)
- Previews can be regenerated by re-running Phase 5

## Prose Generation Principles for Previews

When ui-template-library interprets aesthetic.md for test previews:

1. **Color System** → CSS variables and color palette
2. **Typography** → Font families, sizes, weights
3. **Control Styling** → Knob/slider visual treatment
4. **Spacing** → Padding, margins, grid gaps
5. **Surface Treatment** → Backgrounds, borders, shadows
6. **Details** → Animations, transitions, hover states

The prose should be interpretable, not prescriptive. ui-template-library makes visual choices guided by the prose.
```

**Replace lines 486-566 in SKILL.md with:**
```xml
<step id="5">
<conditional_execution>
IF test previews selected in Phase 4 THEN:
  FOR EACH selected plugin type:
    - Read spec from assets/test-plugin-specs.json
    - Invoke ui-template-library skill "apply" operation
    - Save HTML to .claude/aesthetics/[id]/test-previews/[plugin-type].html
  END FOR
END IF
</conditional_execution>
See references/test-preview-protocol.md for detailed invocation protocol.
</step>
```

**Verification:** File `references/test-preview-protocol.md` exists and contains ~1,800 tokens. SKILL.md reduced by ~80 lines.

### Fix 2.3: Extract Error Handling to References
**Location:** Lines 793-828
**Operation:** EXTRACT
**Create file:** `references/error-handling.md`

**File content:**
```markdown
# Error Handling and Edge Cases

## Phase 1: Free-Form Collection

### Edge Case: User provides no information
**Scenario:** User responds with "I don't know" or very minimal description

**Handling:**
- Acknowledge the starting point
- Proceed directly to Phase 2 gap analysis
- All tiers will show as gaps, generate questions covering Tier 1 critical elements

### Edge Case: User provides complete specification
**Scenario:** User's initial description covers all 3 tiers comprehensively

**Handling:**
- Proceed through Phase 2 gap analysis (will find minimal gaps)
- In Phase 3, generate 4 clarification questions (fine details, confirmations)
- Present decision gate as usual
- User will likely choose "finalize" immediately

## Phase 2: Gap Analysis

### Error: Failed to parse user context
**Scenario:** Accumulated context is malformed or missing

**Handling:**
- Log parsing error
- Default to treating all tiers as gaps
- Proceed with comprehensive question generation

### Error: No gaps identified (all tiers covered)
**Scenario:** Context already covers all visual elements

**Handling:**
- Generate 4 confirmatory questions ("Did I understand correctly that...")
- Allow user to refine or finalize at decision gate

## Phase 3: Question Batch

### Error: AskUserQuestion tool fails
**Scenario:** Tool invocation returns error or timeout

**Handling:**
- Retry once with same questions
- If retry fails, fall back to inline text questions with markdown formatting
- Collect free-form responses and proceed to Phase 3.5

### Edge Case: User skips all questions
**Scenario:** User types "skip" in "Other" for all 4 questions

**Handling:**
- Acknowledge skipped questions
- Proceed to decision gate
- User can choose "Add more context" to provide information differently

## Phase 3.5: Decision Gate

### Edge Case: User requests option not in menu
**Scenario:** User types custom response not matching 3 options

**Handling:**
- Clarify the 3 available options
- Re-present decision gate with same format
- If user persists, interpret intent and route to closest option

### Error: Decision gate tool fails
**Scenario:** AskUserQuestion fails at critical decision point

**Handling:**
- Fall back to inline numbered list
- Wait for user to type 1, 2, or 3
- Parse response and route accordingly

## Phase 3.7: Name Aesthetic

### Edge Case: User rejects all 3 suggested names
**Scenario:** User chooses "Other" but provides no custom name

**Handling:**
- Prompt again: "What would you like to call this aesthetic?"
- If still no response, generate default name: "[primary-color]-[vibe]-aesthetic"
- Example: "blue-modern-aesthetic"

### Error: Name collision (ID already exists)
**Scenario:** Generated slug matches existing aesthetic ID in manifest.json

**Handling:**
- Append numeric suffix: "-001", "-002", etc.
- Check manifest again until unique ID found
- Maximum 999 suffixes, then error and ask user for different name

## Phase 4: Test Preview Selection

### Edge Case: User selects 0 previews
**Scenario:** User doesn't select any test plugin types

**Handling:**
- Skip Step 5 in Phase 5 (test preview generation)
- Only generate aesthetic.md and metadata.json
- Update manifest with empty testPreviews array

### Edge Case: User requests preview type not in list
**Scenario:** User types custom plugin type in "Other"

**Handling:**
- Acknowledge request but explain only 4 pre-defined types available
- Suggest closest match from available types
- User can choose from 4 types or select none

## Phase 5: Generate Files

### Error: Template file not found
**Scenario:** aesthetic-template.md missing from ui-template-library/assets/

**Handling:**
- Log error: "Template file not found at [path]"
- Abort Phase 5 and report to user
- Suggest running system verification or reinstalling ui-template-library skill

### Error: Cannot write aesthetic.md
**Scenario:** Directory doesn't exist or permissions issue

**Handling:**
- Attempt to create directory: `.claude/aesthetics/[id]/`
- If creation fails, log error and report to user
- Abort Phase 5 (cannot proceed without writing aesthetic.md)

### Error: ui-template-library skill invocation fails
**Scenario:** Skill not available or returns error for test preview

**Handling:**
- Log warning for specific plugin type
- Continue with remaining preview types (don't abort entire phase)
- Report partial success in confirmation message: "Generated 3 of 4 test previews (complex-reverb failed)"

### Error: Manifest.json not found or malformed
**Scenario:** Cannot read or parse .claude/aesthetics/manifest.json

**Handling:**
- Create new manifest.json with single entry (current aesthetic)
- Log warning that previous aesthetics may not be indexed
- Proceed with Phase 5 completion

### Error: Manifest update fails (write error)
**Scenario:** Cannot write updated manifest.json

**Handling:**
- Log error but continue (aesthetic files still created)
- Report in confirmation: "Aesthetic created but manifest not updated"
- Aesthetic will still be functional, just not in centralized index

## Phase 6: Decision Menu

### Error: Test preview files missing for Option 1
**Scenario:** User chooses "Open test previews" but files don't exist

**Handling:**
- Check for file existence before running `open` commands
- Report which previews are missing
- Open only the available previews
- Return to decision menu

### Error: PLUGINS.md not found for Option 4
**Scenario:** User chooses "Apply to existing plugin" but PLUGINS.md missing

**Handling:**
- Log error: "PLUGINS.md not found at [path]"
- Report to user: "No plugins found. Create a plugin first with /dream or /plan"
- Return to decision menu with Option 4 disabled

### Error: ui-mockup skill not available for Option 4
**Scenario:** User chooses "Apply to existing plugin" but skill can't be invoked

**Handling:**
- Log error: "ui-mockup skill not found"
- Report to user: "Cannot invoke ui-mockup. Verify skill installation."
- Return to decision menu with Option 4 disabled

## General Error Handling

### Skill-level errors

**Principle:** Fail gracefully, never abort entire workflow unless critical files can't be written.

**Logging:**
- All errors logged with timestamp, phase ID, and error message
- Errors include suggested remediation steps

**User communication:**
- Clear, non-technical explanation of what went wrong
- Specific next steps to resolve
- Always offer path forward (retry, skip, or alternative route)

**Recovery strategy:**
- Phases 1-4: Non-critical, can continue with partial information
- Phase 5: Critical for file generation, must abort if template or write fails
- Phase 6: Non-critical, can repeat or exit gracefully
```

**Replace lines 793-828 in SKILL.md with:**
```xml
<error_handling>
See references/error-handling.md for complete edge case handling and recovery strategies.
</error_handling>
```

**Verification:** File `references/error-handling.md` exists and contains ~800 tokens. SKILL.md reduced by ~35 lines.

### Fix 2.4: Remove Redundant Continuous Iteration Section
**Location:** Lines 759-772
**Operation:** DELETE
**Content to remove:**
```markdown
**Continuous iteration support:**

The workflow supports multiple refinement cycles:

1. User can ask for more questions at any time (decision gate)
2. User can add context between question batches
3. All context is cumulative (never lost)
4. User can refine after file generation (Phase 6, Option 2)

This ensures the aesthetic is precisely what the user envisions, not rushed to completion.
```

**Justification:** This content is redundant with:
- Phase 3.5 decision gate routing logic (already documents continuous iteration)
- Phase 6 Option 2 (already documents post-generation refinement)
- State requirements in multiple phases (already document cumulative context)

**Verification:** Lines 759-772 removed. Verify no broken references to this section elsewhere in SKILL.md.

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Clarify Loop Description in Workflow Overview
**Location:** Line 28
**Operation:** REPLACE
**Before:**
```markdown
5. **Loop until finalized** - Repeat phases 2-4 as needed
```

**After:**
```markdown
5. **Loop until finalized** - Repeat phases 2 (gap analysis) → 3 (questions) → 3.5 (decision gate) until user chooses "finalize" at gate
```

**Verification:** Line 28 now explicitly lists phases in loop sequence.

### Fix 3.2: Add State Requirement for Context Persistence
**Location:** Line 70 (end of Phase 1)
**Operation:** Already covered in Fix 1.2
**Status:** No additional change needed (merged into Phase 1 XML structure)

### Fix 3.3: Add State Transition Clarity to Phase 3
**Location:** Line 233
**Operation:** Already covered in Fix 1.4
**Status:** No additional change needed (merged into Phase 3 XML structure with `<state_transition>` tag)

### Fix 3.4: Elevate Prose Generation Requirements in Phase 5
**Location:** Lines 398-418
**Operation:** Already covered in Fix 1.8
**Status:** No additional change needed (merged into Phase 5 Step 2 with `<prose_generation_requirements>` tag)

## File Creation Manifest

### Files to create in references/:

1. **references/question-examples.md** (Fix 2.1)
   - Full AskUserQuestion code examples with annotations
   - Question banks by category (controls, colors, typography, spacing)
   - Question generation strategies
   - ~3,000 tokens

2. **references/test-preview-protocol.md** (Fix 2.2)
   - Step-by-step test preview generation process
   - Spec structure documentation
   - ui-template-library skill invocation protocol
   - Verification and error handling
   - ~1,800 tokens

3. **references/error-handling.md** (Fix 2.3)
   - Edge cases for all 6 phases
   - Error recovery strategies
   - Graceful degradation patterns
   - User communication templates
   - ~800 tokens

### Files to modify:

1. **SKILL.md** (All fixes)
   - Add XML structure tags for phases, decision gates, handoff protocol
   - Replace verbose examples with references
   - Add critical sequence enforcement attributes
   - Remove redundant sections
   - Estimated changes: ~27 operations across ~700 lines

### Files already existing (no changes):

1. **references/aesthetic-questions.md** (already exists)
   - Comprehensive question banks by category
   - No changes needed (will be cross-referenced by new question-examples.md)

2. **assets/aesthetic-template.md** (already exists)
   - 12-section structure template
   - Already referenced correctly in Phase 5

3. **assets/test-plugin-specs.json** (already exists)
   - Test plugin parameter specifications
   - Already referenced correctly in Phase 5 and new test-preview-protocol.md

## Execution Checklist

### Phase 1 Checklist: Critical Fixes
- [ ] Fix 1.1: Workflow overview wrapped in `<workflow_overview><critical_sequence enforcement="strict">`
- [ ] Fix 1.2: Phase 1 wrapped in `<phase id="1">` with `<state_requirement>`
- [ ] Fix 1.3: Phase 2 wrapped in `<phase id="2">` with `<anti_pattern>` and `<delegation_rule>`
- [ ] Fix 1.4: Phase 3 wrapped in `<phase id="3">` with reference to question-examples.md
- [ ] Fix 1.5: Phase 3.5 wrapped in `<decision_gate enforcement="blocking">`
- [ ] Fix 1.6: Phase 3.7 wrapped in `<phase id="3.7">` with `<conditional_execution>`
- [ ] Fix 1.7: Phase 4 wrapped in `<phase id="4">` with `<routing_logic>`
- [ ] Fix 1.8: Phase 5 wrapped in `<phase id="5">` with `<critical_sequence enforcement="strict" allow_reordering="false">`
- [ ] Fix 1.9: Phase 6 wrapped in `<phase id="6">` with `<checkpoint_protocol>` and note about inline vs. AskUserQuestion
- [ ] Fix 1.10: Handoff protocol expanded with `<preconditions>`, `<postconditions>`, `<error_handling>` for both skills
- [ ] Fix 1.11: Adaptive questioning strategy wrapped in `<adaptive_questioning_strategy><critical_sequence>`

### Phase 2 Checklist: Content Extraction
- [ ] Fix 2.1: references/question-examples.md created with ~3,000 tokens of examples
- [ ] Fix 2.1: Lines 163-227 replaced with reference tag in SKILL.md
- [ ] Fix 2.2: references/test-preview-protocol.md created with ~1,800 tokens
- [ ] Fix 2.2: Lines 486-566 replaced with reference tag in SKILL.md
- [ ] Fix 2.3: references/error-handling.md created with ~800 tokens
- [ ] Fix 2.3: Lines 793-828 replaced with reference tag in SKILL.md
- [ ] Fix 2.4: Lines 759-772 (continuous iteration section) deleted

### Phase 3 Checklist: Polish
- [ ] Fix 3.1: Line 28 loop description clarified with explicit phase sequence
- [ ] Fix 3.2: State requirement clarity (merged into Fix 1.2)
- [ ] Fix 3.3: State transition clarity (merged into Fix 1.4)
- [ ] Fix 3.4: Prose generation requirements elevated (merged into Fix 1.8)

### Verification Checklist
- [ ] All critical sequences wrapped in `<critical_sequence>` tags
- [ ] All decision gates have `enforcement="blocking"` attribute
- [ ] All delegation rules explicit with `<delegation_rule>` tags
- [ ] Large example blocks extracted to references/
- [ ] Test preview protocol extracted to references/
- [ ] Error handling extracted to references/
- [ ] Context reduced by ~6,500 tokens (24%)
- [ ] All requirements use MUST/SHOULD/NEVER capitalized
- [ ] Ambiguous pronouns clarified with explicit subject references
- [ ] Skill health improved from Yellow to estimated Green

## Estimated Impact

### Before:
- Overall health: **Yellow**
- Critical issues: **3**
- Context size: **~27,000 tokens**
- XML Organization: **1/5**
- Critical Sequence Enforcement: **1/5**

### After:
- Overall health: **Green** (estimated)
- Critical issues: **0**
- Context size: **~20,500 tokens** (-24%)
- XML Organization: **5/5**
- Critical Sequence Enforcement: **5/5**

### Improvement Areas:
- **Enforcement:** Multi-phase workflow now has explicit guards against skipping phases or deviating from sequence
- **Clarity:** Handoff protocols specify preconditions/postconditions, preventing integration failures
- **Context:** 6,500 tokens moved to references, making main workflow easier to comprehend
- **Maintainability:** Examples and protocols separated from core logic, can be updated independently

## Execution Notes

### Order of Operations:
1. **Create reference files first** (Fixes 2.1, 2.2, 2.3) - This ensures references exist before SKILL.md references them
2. **Add XML structure** (Fixes 1.1-1.11) - Wrap existing content in semantic tags
3. **Replace verbose sections** (already included in Fixes 1.4, 1.8) - Swap examples with reference tags
4. **Delete redundant sections** (Fix 2.4) - Remove continuous iteration section
5. **Polish inline text** (Fix 3.1) - Clarify loop description

### Testing Strategy:
1. Invoke skill with `/dream` command or `Skill("aesthetic-dreaming")`
2. Provide minimal initial description (test gap analysis)
3. Verify Phase 2 identifies appropriate gaps by tier
4. Verify Phase 3 generates 4 non-redundant questions via AskUserQuestion
5. Verify Phase 3.5 decision gate uses AskUserQuestion (not inline menu)
6. Verify Phase 3.7 naming executes only after "finalize" choice
7. Verify Phase 6 checkpoint uses inline numbered list (not AskUserQuestion)
8. Verify ui-template-library invocation includes precondition checks
9. Verify aesthetic.md generated with all 12 sections
10. Verify manifest.json updated correctly

### Rollback Strategy:
- Keep backup of original SKILL.md before modifications
- If verification fails, compare specific phase behavior before/after
- Isolated fixes (each phase is self-contained), can revert individual phases if needed

## Success Criteria

The fix plan succeeds if:
- ✅ All 11 critical fixes applied without breaking existing functionality
- ✅ All 4 extraction operations completed, reference files created
- ✅ Context reduced by ~24% (6,500 tokens) while maintaining clarity
- ✅ Skill health improves from Yellow to Green
- ✅ XML Organization score improves from 1/5 to 5/5
- ✅ Critical Sequence Enforcement score improves from 1/5 to 5/5
- ✅ Test invocation completes full workflow (Phases 1-6) without errors
- ✅ Decision gates correctly route based on user choices
- ✅ Handoff protocols prevent integration failures
- ✅ No loss of existing functionality or user experience degradation
