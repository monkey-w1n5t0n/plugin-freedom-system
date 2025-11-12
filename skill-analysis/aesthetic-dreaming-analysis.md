# Skill Analysis: aesthetic-dreaming

## Executive Summary
- Overall health: **Yellow**
- Critical issues: **3**
- Optimization opportunities: **8**
- Estimated context savings: **~6,500 tokens / 24%**

## Dimension Scores (1-5)

1. Structure Compliance: **4/5**
2. Content Extraction: **2/5**
3. Instruction Clarity: **3/5**
4. XML Organization: **1/5**
5. Context Efficiency: **2/5**
6. Claude-Optimized Language: **3/5**
7. System Integration: **2/5**
8. Critical Sequence Enforcement: **1/5**

## Critical Issues (blockers for Claude comprehension)

1. **Missing critical sequence enforcement (Lines 38-287)** - The entire adaptive questioning workflow (Phases 1-4) lacks XML tags to enforce sequential execution. Claude could skip phases, mix them, or deviate from the loop structure.

2. **Decision gate protocol violation (Line 617-630)** - Phase 6 decision menu uses inline numbered list format but contradicts CLAUDE.md which states "Do NOT use AskUserQuestion tool for decision menus - use inline numbered lists". However, Phases 3.5 and 3.7 DO use AskUserQuestion for decision gates, creating confusion.

3. **No handoff protocol definition (Lines 777-787)** - Integration points mention invoking ui-template-library and ui-mockup but lack structured handoff protocol with clear preconditions and postconditions.

## Optimization Opportunities

### Content to Extract to references/

**Lines 163-287 (Question Batch Examples): ~3,200 tokens → references/question-examples.md**
- Contains extensive AskUserQuestion code examples
- These are implementation details, not core instructions
- Claude needs the pattern, not every verbose example
- Estimated savings: **3,000 tokens**

**Lines 486-566 (Test Preview Generation Details): ~2,100 tokens → references/test-preview-protocol.md**
- Very detailed test plugin spec reading and ui-template-library invocation
- This is reference material for "how to generate previews"
- Main SKILL.md just needs "Generate test previews using ui-template-library"
- Estimated savings: **1,800 tokens**

**Lines 689-753 (Question Generation Examples 1-2): ~1,800 tokens → references/extraction-examples.md**
- Already has aesthetic-questions.md in references/ with similar content
- These examples demonstrate gap analysis but are redundant with existing reference
- Estimated savings: **1,200 tokens**

**Lines 793-828 (Error Handling): ~800 tokens → references/error-handling.md**
- Edge cases and error recovery flows
- Not critical to main workflow
- Estimated savings: **500 tokens**

### Content to Extract to assets/

**Lines 391-447 (Aesthetic Template Location and Structure): No extraction needed**
- Already references external template: assets/aesthetic-template.md
- Good pattern, already optimized

**Create new asset: assets/phase-flow-diagram.md**
- Visual representation of phase flow: 1 → 2 → 3 → 3.5 → (loop or 3.7) → 4 → 5 → 6
- Would clarify the loop structure and decision gates
- Estimated benefit: Improved comprehension (not token savings)

### XML Tag Opportunities

**Lines 22-34 (Workflow Overview): Wrap in `<workflow_overview>`**
```xml
BEFORE:
## Workflow Overview

1. **Free-form collection** - "What aesthetic are you envisioning?"
2. **Gap analysis** - Extract covered concepts, identify missing visual elements
...

AFTER:
<workflow_overview>
<critical_sequence>
This workflow MUST execute phases in order. Do NOT skip phases or proceed to finalization before completing gap analysis and question batches.

1. **Phase 1: Free-form collection** - Gather initial aesthetic vision
2. **Phase 2: Gap analysis** - Identify missing visual elements
3. **Phase 3: Question batch** - Ask 4 contextual questions via AskUserQuestion
4. **Phase 3.5: Decision gate** - Finalize / Continue / Add context
5. **Phase 3.7: Name aesthetic** (if finalizing)
6. **Phase 4: Test preview selection**
7. **Phase 5: Generate files**
8. **Phase 6: Decision menu**

Loop: Phases 2-3-3.5 repeat until user chooses "finalize" at decision gate.
</critical_sequence>
</workflow_overview>
```

**Lines 38-70 (Phase 1): Wrap in `<phase id="1">`**
```xml
<phase id="1" name="free_form_collection">
<instructions>
Present open invitation to user. Extract all mentioned visual concepts verbatim and accumulate in context.
</instructions>

<required_prompt>
What aesthetic are you envisioning?

Describe the visual design system you want to create...
</required_prompt>

<extraction_targets>
- Visual Identity: Overall vibe, mood, design philosophy
- Color System: Palette, warm/cool, contrast, specific colors
- Typography: Font choices, sizing philosophy
- Control Styling: Knob, slider, button design
- Spacing: Density, breathing room
- Surface Treatment: Textures, shadows, depth
- Details: Special features, animations
- Best For: Plugin types, use cases
- Inspirations: References to existing designs
</extraction_targets>

<state_requirement>
MUST accumulate all extracted concepts before proceeding to Phase 2.
NEVER ask about information provided in this phase.
</state_requirement>
</phase>
```

**Lines 72-112 (Phase 2): Wrap in `<phase id="2">`**
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
</anti_pattern>

<delegation_rule>
Detailed question banks are in references/aesthetic-questions.md. Reference for gap analysis strategies and question options.
</delegation_rule>
</phase>
```

**Lines 114-233 (Phase 3): Wrap in `<phase id="3">`**
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

<example>
See references/question-examples.md for detailed AskUserQuestion formatting examples.
</example>

<state_requirement>
After receiving answers, accumulate context with previous responses before proceeding to Phase 3.5.
</state_requirement>
</phase>
```

**Lines 235-287 (Phase 3.5): Wrap in `<decision_gate>`**
```xml
<decision_gate id="finalize_or_continue" phase="3.5">
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

**Lines 289-336 (Phase 3.7): Wrap in `<phase id="3.7">`**
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
- Check for existing IDs and append counter: "modern-professional-001"
- Increment until unique ID found
</slug_generation>
</phase>
```

**Lines 338-384 (Phase 4): Wrap in `<phase id="4">`**
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

<routing_logic>
IF user selects 0 options THEN skip test preview generation, proceed directly to Phase 5 (aesthetic.md only)
ELSE IF user selects 1-4 options THEN proceed to Phase 5 with selected preview types
</routing_logic>

<state_requirement>
Store selected preview types in context for Phase 5 generation.
</state_requirement>
</phase>
```

**Lines 386-613 (Phase 5): Wrap in `<phase id="5">`**
```xml
<phase id="5" name="generate_files">
<critical_sequence>
MUST execute steps in this exact order:
1. Read aesthetic template
2. Generate structured prose for all 12 sections
3. Write aesthetic.md
4. Generate metadata.json
5. Generate test previews (if any selected in Phase 4)
6. Update manifest.json
7. Present confirmation message
</critical_sequence>

<step id="1">
Read template: /Users/lexchristopherson/Developer/plugin-freedom-system/.claude/skills/ui-template-library/assets/aesthetic-template.md
</step>

<step id="2">
Transform accumulated context into structured prose for 12 sections. See references/test-preview-protocol.md for detailed prose generation principles.
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

**Lines 617-655 (Phase 6): Wrap in `<phase id="6">`**
```xml
<phase id="6" name="decision_menu">
<checkpoint_protocol>
This is a system checkpoint. MUST follow Plugin Freedom System checkpoint protocol.
</checkpoint_protocol>

<decision_menu format="inline_numbered_list">
MUST use inline numbered list format (NOT AskUserQuestion tool) per CLAUDE.md checkpoint protocol.

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

<state_requirement>
MUST wait for user response. NEVER auto-proceed to any option.
</state_requirement>
</phase>
```

**Lines 777-787 (Integration Points): Wrap in `<handoff_protocol>`**
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
  <context>Phase 5: Test preview generation</context>
  <preconditions>
    - aesthetic.md must be generated
    - Test plugin spec must exist in assets/test-plugin-specs.json
    - Aesthetic ID must be valid
  </preconditions>
  <postconditions>
    - HTML file created in .claude/aesthetics/[id]/test-previews/
    - Returns success/failure status
  </postconditions>
</skill>

<skill name="ui-mockup">
  <operation>create_mockup_with_aesthetic</operation>
  <context>Phase 6 Option 4: Apply to existing plugin</context>
  <preconditions>
    - Plugin must exist in PLUGINS.md
    - Aesthetic must exist in manifest.json
    - Plugin must have parameter-spec.md
  </preconditions>
  <postconditions>
    - ui-mockup Phase 0 pre-selects aesthetic
    - Mockup generation proceeds with aesthetic applied
  </postconditions>
</skill>
</invokes>

<integration_notes>
When aesthetic-dreaming creates an aesthetic, it becomes immediately available in ui-mockup Phase 0 aesthetic selection menu.
</integration_notes>
</handoff_protocol>
```

### Instruction Clarity Improvements

**Line 28 - Ambiguous loop description:**
```
BEFORE:
5. **Loop until finalized** - Repeat phases 2-4 as needed

AFTER:
5. **Loop until finalized** - Repeat phases 2 (gap analysis) → 3 (questions) → 3.5 (decision gate) until user chooses "finalize" at gate
```

**Line 70 - Vague extraction instruction:**
```
BEFORE:
**Capture everything mentioned verbatim in accumulated context.**

AFTER:
<state_requirement>
Accumulate all extracted concepts in persistent context. This context carries forward through ALL phases and question batches. NEVER lose information from previous phases.
</state_requirement>
```

**Line 233 - Missing state transition:**
```
BEFORE:
**After receiving answers:**
1. Accumulate context with previous responses
2. Re-analyze gaps
3. Proceed to decision gate

AFTER:
<state_transition>
After receiving question batch answers:
1. Merge answers with accumulated context (cumulative, never overwrite)
2. Proceed DIRECTLY to decision gate (Phase 3.5)
3. Decision gate will route to Phase 2 (re-analyze) or Phase 3.7 (finalize)
</state_transition>
```

**Lines 398-418 - Prose generation principles buried:**
```
BEFORE:
**Prose generation principles:**

- **Be interpretable, not rigid:** "Generous spacing with 32px gaps" not "spacing: 32px"
- **Capture intent:** "Warm brass controls evoking vintage hardware" not "color: #d4a574"
...

AFTER:
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
```

**Lines 658-683 - Adaptive questioning strategy lacks structure:**
```
BEFORE:
## Adaptive Questioning Strategy

**Extract first, then fill gaps:**

1. User provides initial description
2. Parse response for covered visual concepts
...

AFTER:
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

See references/aesthetic-questions.md for detailed gap analysis strategies.
</adaptive_questioning_strategy>
```

### Verbosity Reductions

**Lines 38-56 (Phase 1 prompt): 430 words → 180 words**
```
BEFORE:
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

AFTER:
<required_prompt>
What aesthetic are you envisioning?

Describe the visual design system you want to create—share as much or as little as you want. I'll ask follow-ups for anything missing.

Consider: overall vibe, color palette, control styling, spacing, textures, typography, and plugin types this would suit.

You can reference existing designs, describe feelings, or provide technical specifics.
</required_prompt>
```
**Savings: 250 words → ~150 tokens**

**Lines 289-336 (Phase 3.7): 520 words → 280 words**
- The name suggestion examples and validation rules can be condensed
- Move slug generation rules to references/
**Savings: 240 words → ~140 tokens**

**Lines 759-772 (Continuous iteration support): Can be removed entirely**
- This is redundant with the decision gate loop structure
- Already covered by Phase 3.5 routing logic
**Savings: 150 words → ~90 tokens**

### Critical Sequence Enforcement

**Lines 22-34: Main workflow lacks enforcement wrapper**
```
CURRENT: Plain numbered list (no enforcement)

RECOMMENDED:
<workflow_overview>
<critical_sequence enforcement="strict">
Phases MUST execute in this order:
1 → 2 → 3 → 3.5 → {loop back to 2 OR proceed to 3.7} → 4 → 5 → 6

Do NOT:
- Skip phases
- Execute phases out of order
- Proceed to finalization before decision gate approval
- Auto-proceed past decision gates

Loop structure:
- Phases 2-3-3.5 form the adaptive questioning loop
- Loop continues until user chooses "finalize" at Phase 3.5
- Only then proceed to Phase 3.7 (naming)
</critical_sequence>
</workflow_overview>
```

**Lines 235-287: Decision gate needs enforcement attributes**
```
<decision_gate id="finalize_or_continue" phase="3.5" enforcement="blocking">
<wait_for_user_response>true</wait_for_user_response>
<auto_proceed>false</auto_proceed>
<required_options count="3">
  Option 1: Finalize → Phase 3.7
  Option 2: Continue → Phase 2 (with accumulated context)
  Option 3: Add context → Collect input, then Phase 2
</required_options>
</decision_gate>
```

**Lines 386-613: File generation sequence needs strict ordering**
```
<phase id="5" name="generate_files">
<critical_sequence enforcement="strict" allow_reordering="false">
Steps MUST execute in this exact order (no parallelization):
1. Read template
2. Generate prose (requires template)
3. Write aesthetic.md (requires prose)
4. Generate metadata.json (requires aesthetic ID)
5. Generate test previews (requires aesthetic.md completion)
6. Update manifest (requires metadata.json)
7. Present confirmation (requires all files created)
</critical_sequence>
</phase>
```

## Examples of Issues Found

### Example 1: Missing Phase Sequence Enforcement

**Issue:** Phases 1-6 are described in prose but lack XML structure to prevent deviation.

**Impact:** Claude could skip gap analysis (Phase 2) and go straight to question generation (Phase 3), resulting in redundant questions.

**BEFORE:**
```markdown
## Workflow Overview

1. **Free-form collection** - "What aesthetic are you envisioning?"
2. **Gap analysis** - Extract covered concepts, identify missing visual elements
3. **Adaptive questioning** - Generate 4 contextual questions via AskUserQuestion
```

**AFTER:**
```xml
<workflow_overview>
<critical_sequence>
Phases MUST execute in order. Do NOT skip phases.

<phase_flow>
1 (Free-form) → 2 (Gap analysis) → 3 (Questions) → 3.5 (Decision gate)
  ↑                                                              ↓
  └─────────────── Loop if "continue" ────────────────────────┘
                              ↓ if "finalize"
                         3.7 (Naming) → 4 (Preview selection) → 5 (Generate files) → 6 (Decision menu)
</phase_flow>
</critical_sequence>
</workflow_overview>
```

### Example 2: Decision Menu Protocol Inconsistency

**Issue:** Phase 6 uses inline numbered list while Phases 3.5 and 3.7 use AskUserQuestion, but CLAUDE.md says "Do NOT use AskUserQuestion tool for decision menus."

**Impact:** Confusion about which approach to use for decision gates vs. checkpoint decision menus.

**BEFORE:**
```markdown
## Phase 6: Decision Menu

Present final options:

```
✅ Aesthetic "Modern Professional" ready!

What's next?
1. Open test previews - View mockups in browser
2. Refine aesthetic - Adjust and regenerate aesthetic.md
...
```
```

**AFTER:**
```xml
<phase id="6" name="decision_menu">
<checkpoint_protocol>
This is a SYSTEM CHECKPOINT per Plugin Freedom System protocol.
MUST use inline numbered list format (NOT AskUserQuestion).
</checkpoint_protocol>

<decision_gate type="checkpoint" format="inline_numbered_list">
✅ Aesthetic "[Name]" ready!

What's next?
1. Open test previews
2. Refine aesthetic
3. Create another aesthetic
4. Apply to existing plugin
5. Done

Choose (1-5): _
</decision_gate>

<note>
Phase 3.5 and 3.7 use AskUserQuestion because they are INTERNAL decision gates (workflow branching).
Phase 6 uses inline numbered list because it is a SYSTEM CHECKPOINT (user discovers next actions).
</note>
</phase>
```

### Example 3: Verbose Question Examples Taking Context

**Issue:** Lines 163-227 contain full AskUserQuestion code examples that could be referenced instead of inlined.

**Impact:** ~3,000 tokens of context used for examples when pattern + reference would suffice.

**BEFORE (in SKILL.md, lines 163-227):**
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
        // ... 60 more lines of example code
```

**AFTER (in SKILL.md):**
```xml
<phase id="3" name="question_batch">
<instructions>
Generate exactly 4 questions using AskUserQuestion tool based on gaps from Phase 2.
</instructions>

<tool_usage>
Tool: AskUserQuestion
Parameters:
  - questions: array[1-4] of question objects
  - Each question has: question, header, multiSelect, options[2-4]
  - Always include "Other" option for custom input
</tool_usage>

<example>
See references/question-examples.md for detailed formatting examples and question banks by category.
</example>
</phase>
```

**AFTER (in references/question-examples.md):**
```markdown
# Question Batch Examples

## Full AskUserQuestion Example

[Move all example code here with annotations]
```

**Savings: ~3,000 tokens**

### Example 4: Test Preview Protocol Buried in Phase 5

**Issue:** Lines 486-566 contain detailed protocol for test preview generation that interrupts the main flow.

**Impact:** Main workflow buried under implementation details. Hard to see the forest for the trees.

**BEFORE:**
```markdown
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
  // ... 80 more lines
```

**AFTER (in SKILL.md):**
```xml
<step id="5">
<conditional_execution>
IF test previews selected in Phase 4:
  FOR EACH selected plugin type:
    - Load spec from assets/test-plugin-specs.json
    - Invoke ui-template-library skill "apply" operation
    - Save HTML to .claude/aesthetics/[id]/test-previews/[type].html
END IF
</conditional_execution>

<protocol_reference>
See references/test-preview-protocol.md for detailed invocation protocol and spec format.
</protocol_reference>
</step>
```

**AFTER (in references/test-preview-protocol.md):**
```markdown
# Test Preview Generation Protocol

## Step-by-Step Process
1. Load test plugin spec from assets/test-plugin-specs.json
2. Extract spec for selected plugin type
3. Invoke ui-template-library skill with parameters...

[Move all detailed examples and code here]
```

**Savings: ~1,800 tokens**

### Example 5: No Handoff Preconditions/Postconditions

**Issue:** Lines 777-787 mention integration with ui-template-library and ui-mockup but don't specify preconditions or postconditions.

**Impact:** Claude might invoke skills at wrong time or with incomplete context.

**BEFORE:**
```markdown
**Invokes:**
- `ui-template-library` skill (apply operation for test previews)
- `ui-mockup` skill (option 4 in decision menu: apply to existing plugin)
```

**AFTER:**
```xml
<handoff_protocol>
<skill name="ui-template-library" operation="apply">
<when>Phase 5, Step 5 (test preview generation)</when>

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

<skill name="ui-mockup" operation="create_with_aesthetic">
<when>Phase 6, Option 4 (apply to existing plugin)</when>

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
</handoff_protocol>
```

## Estimated Impact

### Context Reduction: 6,500 tokens (24% reduction)

**Breakdown:**
- Extract question examples to references/: **-3,000 tokens**
- Extract test preview protocol to references/: **-1,800 tokens**
- Extract error handling to references/: **-500 tokens**
- Remove redundant iteration section: **-90 tokens**
- Condense verbose sections: **-400 tokens**
- Add XML structure: **+290 tokens** (net cost but worth it for clarity)

**Total: ~27,000 tokens → ~20,500 tokens**

### Comprehension Improvement: **High**

**Reasoning:**
- XML tags enforce critical sequences (Phases 1-6, decision gates, file generation)
- Clear phase boundaries prevent deviation
- Handoff protocols specify preconditions/postconditions
- Anti-patterns and requirements explicitly marked
- Routing logic structured with IF/THEN/ELSE
- State requirements prevent context loss

**Without XML structure:** Claude could skip phases, ask redundant questions, mix up decision gate formats, invoke skills incorrectly.

**With XML structure:** Claude has explicit guardrails for every phase transition and decision point.

### Maintenance Improvement: **High**

**Reasoning:**
- References/ contain implementation details (can update without changing main flow)
- Examples separated from instructions (easier to add new question types)
- Handoff protocols documented (clear integration contracts)
- Phase structure makes it easy to add new phases or modify existing
- XML tags provide semantic structure (can search for `<phase id="3">` directly)

## Priority Recommendations

### 1. Add XML Critical Sequence Enforcement (HIGHEST PRIORITY)

**What:** Wrap Phases 1-6 in `<phase>` tags with `<critical_sequence>` enforcement.

**Why:** This is a multi-phase workflow with loops and decision gates. Without XML structure, Claude can deviate from the sequence, skip gap analysis, or ask redundant questions.

**Impact:** Prevents workflow violations, ensures adaptive questioning works correctly.

**Effort:** 30 minutes

**Files to modify:**
- SKILL.md (add XML tags around lines 22-655)

### 2. Extract Verbose Examples to References (HIGH PRIORITY)

**What:** Move lines 163-227 (question examples), 486-566 (test preview protocol), and 793-828 (error handling) to references/ directory.

**Why:** These examples consume ~5,300 tokens but are reference material, not core instructions. Main workflow gets buried under implementation details.

**Impact:** 19% context reduction while maintaining clarity through references.

**Effort:** 20 minutes

**Files to create:**
- references/question-examples.md
- references/test-preview-protocol.md
- references/error-handling.md

**Files to modify:**
- SKILL.md (replace verbose sections with `<protocol_reference>` tags)

### 3. Add Handoff Protocol with Preconditions/Postconditions (MEDIUM PRIORITY)

**What:** Wrap lines 777-787 in `<handoff_protocol>` with structured preconditions, postconditions, and error handling for ui-template-library and ui-mockup skill invocations.

**Why:** Integration points lack clear contracts. Claude might invoke skills with missing context or at wrong time.

**Impact:** Prevents integration failures, makes skill invocations reliable.

**Effort:** 15 minutes

**Files to modify:**
- SKILL.md (expand integration section with XML handoff protocol)

## Implementation Notes

### Files that would be created in references/:
1. **question-examples.md** - Full AskUserQuestion code examples with annotations
2. **test-preview-protocol.md** - Test plugin spec loading and ui-template-library invocation protocol
3. **error-handling.md** - Edge cases and error recovery flows
4. **extraction-examples.md** (optional) - Consolidate with existing aesthetic-questions.md

### Files that would be created in assets/:
1. **phase-flow-diagram.md** (optional) - Visual ASCII diagram of phase flow and decision gates

### Estimated time to implement all fixes: **65 minutes**

**Breakdown:**
- Priority 1 (XML tags): 30 minutes
- Priority 2 (Extract to references): 20 minutes
- Priority 3 (Handoff protocol): 15 minutes

### Implementation Order:
1. Create references/ files first (question-examples.md, test-preview-protocol.md, error-handling.md)
2. Add XML structure to SKILL.md (phases, decision gates, handoff protocol)
3. Replace verbose sections with `<protocol_reference>` tags
4. Test by invoking skill and verifying phase sequence enforcement
