# Fix Plan: plugin-ideation

## Summary
- Critical fixes: **11**
- Extraction operations: **4**
- Total estimated changes: **15**
- Estimated time: **80** minutes

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Add Critical Sequence Enforcement to Mode Detection
**Location:** Lines 16-26
**Operation:** WRAP
**Before:**
```markdown
## Mode Detection

**Check if plugin exists:**

```bash
grep "^### $PLUGIN_NAME$" PLUGINS.md
```

- If found → **Improvement Mode**
- If not found → **New Plugin Mode**
```

**After:**
```markdown
## Mode Detection

<critical_sequence>
<sequence_name>mode_detection</sequence_name>
<enforcement>must_complete_before_proceeding</enforcement>

<step number="1" required="true">
**Check if plugin exists:**

```bash
grep "^### $PLUGIN_NAME$" PLUGINS.md
```

**Route based on result:**
- If found → Proceed to Improvement Mode (Line 338)
- If not found → Proceed to New Plugin Mode (Line 27)
</step>
</critical_sequence>
```

**Verification:** Confirm `<critical_sequence>` tags wrap lines 16-26 and mode detection cannot be skipped

---

### Fix 1.2: Wrap New Plugin Mode in Critical Sequence
**Location:** Lines 17-336
**Operation:** WRAP
**Before:**
```markdown
## New Plugin Mode

### Phase 1: Free-Form Collection

Start with open question:
```

**After:**
```markdown
## New Plugin Mode

<critical_sequence>
<sequence_name>new_plugin_workflow</sequence_name>
<enforcement>must_complete_in_order</enforcement>
<phases>

### Phase 1: Free-Form Collection

<step number="1" required="true">
Start with open question:
```

**Note:** Continue wrapping each phase (Phase 2, Phase 3, Phase 3.5, Phase 3.7, Phase 4, Phase 5, Phase 6) with `<step>` tags as detailed in Fix 1.3-1.8

**Verification:** Confirm opening `<critical_sequence>` at line 17, closing `</critical_sequence>` after Phase 6 (around line 336)

---

### Fix 1.3: Wrap Phase 2 in Step Tag
**Location:** Lines 48-80
**Operation:** WRAP
**Before:**
```markdown
### Phase 2: Gap Analysis and Question Prioritization

**Extract from Phase 1 response, then identify gaps:**
```

**After:**
```markdown
### Phase 2: Gap Analysis and Question Prioritization

<step number="2" required="true">
**Extract from Phase 1 response, then identify gaps:**

1. Parse user's free-form description
2. Check which tiers are covered
3. Identify missing critical/functional information
4. Never ask about already-provided information
</step>
```

**Verification:** Confirm Phase 2 content wrapped in `<step number="2" required="true">`

---

### Fix 1.4: Wrap Phase 3 in Step Tag and Add Explicit Marker
**Location:** Lines 82-138
**Operation:** WRAP + REPLACE
**Before:**
```markdown
### Phase 3: Question Batch Generation

**Generate exactly 4 questions using AskUserQuestion based on identified gaps.**
```

**After:**
```markdown
### Phase 3: Question Batch Generation

<step number="3" required="true">
**MUST generate exactly 4 questions using AskUserQuestion based on identified gaps.**
```

**Verification:** Confirm `<step number="3">` tag and "MUST generate" instead of "Generate"

---

### Fix 1.5: Wrap Phase 3.5 Decision Gate with XML
**Location:** Lines 139-156
**Operation:** WRAP + INSERT
**Before:**
```markdown
### Phase 3.5: Decision Gate

**Use AskUserQuestion with 3 options after each question batch:**

```
Question:
  question: "Ready to finalize the creative brief?"
  header: "Next step"
  multiSelect: false
  options:
    - label: "Yes - I'm ready to finalize"
      description: "Create creative-brief.md with all collected information"
    - label: "No - Ask me more questions"
      description: "I want to refine some details further"
    - label: "Let me add more thoughts"
      description: "I want to provide additional free-form input first"
```

Route based on answer:
- Option 1 → Proceed to Phase 4 (document creation)
- Option 2 → Return to Phase 2 (re-analyze gaps, generate next 4 questions)
- Option 3 → Collect free-form text, merge with context, return to Phase 2
```

**After:**
```markdown
### Phase 3.5: Decision Gate

<decision_gate>
<gate_name>finalize_or_continue</gate_name>
<blocking>true</blocking>
<checkpoint_protocol>true</checkpoint_protocol>

<step number="4" required="true">
**MUST use AskUserQuestion with 3 options after each question batch:**

```
Question:
  question: "Ready to finalize the creative brief?"
  header: "Next step"
  multiSelect: false
  options:
    - label: "Yes - I'm ready to finalize"
      description: "Create creative-brief.md with all collected information"
    - label: "No - Ask me more questions"
      description: "I want to refine some details further"
    - label: "Let me add more thoughts"
      description: "I want to provide additional free-form input first"
```

**MUST wait for user response. NEVER auto-proceed.**

Route based on answer:
- Option 1 → Proceed to Phase 4 (document creation)
- Option 2 → Return to Phase 2 (re-analyze gaps, generate next 4 questions)
- Option 3 → Collect free-form text, merge with context, return to Phase 2
</step>
</decision_gate>
```

**Verification:** Confirm `<decision_gate>` wraps Phase 3.5, includes `blocking="true"` and "MUST wait" instruction

---

### Fix 1.6: Wrap Phase 3.7 in Step Tag and Add Explicit Marker
**Location:** Lines 158-202
**Operation:** WRAP + REPLACE
**Before:**
```markdown
### Phase 3.7: Plugin Name

**Before creating documents, check if plugin name was provided at any point during conversation.**
```

**After:**
```markdown
### Phase 3.7: Plugin Name

<step number="5" required="true">
**MUST check if plugin name was provided before creating documents.**

If name NOT yet provided, MUST ask via AskUserQuestion:
```

**Verification:** Confirm `<step number="5">` tag and "MUST check" / "MUST ask" markers

---

### Fix 1.7: Wrap Phase 4 in Step Tag and Add State Requirement
**Location:** Lines 204-268
**Operation:** WRAP + REPLACE
**Before:**
```markdown
### Phase 4: Document Creation

When user chooses "finalize" and name is confirmed, create:
```

**After:**
```markdown
### Phase 4: Document Creation

<step number="6" required="true">
**MUST wait until user chooses "finalize" and name is confirmed, then create:**

**File:** `plugins/[PluginName]/.ideas/creative-brief.md`
</step>
```

**Verification:** Confirm `<step number="6">` tag and "MUST wait until" marker

---

### Fix 1.8: Wrap Phase 5 with State Requirement Tag
**Location:** Lines 270-298
**Operation:** WRAP
**Before:**
```markdown
### Phase 5: Session Handoff

**Create handoff file for resuming later:**
```

**After:**
```markdown
### Phase 5: Session Handoff

<state_requirement>
<requirement>must_create_continue_file</requirement>
<step number="7" required="true">
**MUST create handoff file for resuming later:**

**File:** `plugins/[PluginName]/.ideas/.continue-here.md`
</step>
</state_requirement>
```

**Verification:** Confirm `<state_requirement>` wraps Phase 5, includes `must_create_continue_file` requirement

---

### Fix 1.9: Wrap Phase 6 Decision Gate and Add Delegation Rules
**Location:** Lines 300-336
**Operation:** WRAP + REPLACE
**Before:**
```markdown
### Phase 6: Decision Menu

**Present next steps:**

```
✓ Creative brief complete: [PluginName]

What's next?
1. Create UI mockup (recommended)
2. Start implementation
3. Research similar plugins
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
```

**After:**
```markdown
### Phase 6: Decision Menu

<decision_gate>
<gate_name>next_action</gate_name>
<blocking>true</blocking>
<checkpoint_protocol>true</checkpoint_protocol>

<step number="8" required="true">
**MUST present next steps:**

```
✓ Creative brief complete: [PluginName]

What's next?
1. Create UI mockup (recommended)
2. Start implementation
3. Research similar plugins
4. Save for later
5. Other

Choose (1-5): _
```

<delegation_rules>
**Handle responses:**

<delegation_rule skill="ui-mockup" trigger="option_1">
- Condition: User chooses option 1
- Action: MUST invoke ui-mockup skill via Skill tool
- Do NOT attempt to create mockup within this skill
</delegation_rule>

<delegation_rule skill="plugin-workflow" trigger="option_2">
- Condition: User chooses option 2
- Action: MUST invoke plugin-workflow skill via Skill tool
- Warning: MUST warn user about contract requirements before delegating
</delegation_rule>

<delegation_rule skill="deep-research" trigger="option_3">
- Condition: User chooses option 3
- Action: MUST invoke deep-research skill via Skill tool
- Note: deep-research skill is fully implemented (Phase 7 complete)
</delegation_rule>

- Option 4 → Confirm handoff file created, exit
- Option 5 → Ask what they'd like to do
</delegation_rules>
</step>
</decision_gate>

</phases>
</critical_sequence>
```

**Verification:** Confirm `<decision_gate>` wraps Phase 6, `<delegation_rules>` with three `<delegation_rule>` tags, closing `</critical_sequence>` after Phase 6

---

### Fix 1.10: Wrap Improvement Mode in Critical Sequence
**Location:** Lines 338-579
**Operation:** WRAP (same pattern as New Plugin Mode)
**Before:**
```markdown
## Improvement Mode

### Phase 0: Vagueness Detection
```

**After:**
```markdown
## Improvement Mode

<critical_sequence>
<sequence_name>improvement_workflow</sequence_name>
<enforcement>must_complete_in_order</enforcement>
<phases>

### Phase 0: Vagueness Detection

<decision_gate>
<gate_name>vagueness_check</gate_name>
<blocking>true</blocking>
```

**Note:** Apply same step-wrapping pattern as New Plugin Mode to all phases in Improvement Mode (Phase 0, Phase 1, Phase 2, Phase 3, Phase 3.5, Phase 4, Phase 5, Phase 6)

**Verification:** Confirm opening `<critical_sequence>` at line 338, `<step>` tags for each phase, `<decision_gate>` tags for Phase 0 and Phase 3.5 and Phase 6, closing `</critical_sequence>` after Phase 6 (around line 579)

---

### Fix 1.11: Add Trigger Terms to Description
**Location:** Line 3
**Operation:** REPLACE
**Before:**
```yaml
description: Adaptive brainstorming for plugin concepts and improvements
```

**After:**
```yaml
description: Adaptive brainstorming for plugin concepts and improvements. Autonomous triggers: "I want to make...", "Explore improvements to...", "brainstorm", "ideate", "new plugin", "improve plugin"
```

**Verification:** Confirm description includes "Autonomous triggers:" clause

---

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Extract Question Generation Patterns to References
**Location:** Lines 48-173
**Operation:** EXTRACT
**Create file:** `references/question-generation-patterns.md`
**File content:**
```markdown
# Question Generation Patterns

## Question Tiers

### Tier 1: Critical (always ask if missing)
**Plugin type:**
- "What type of plugin? (Effect / Instrument / MIDI / Utility / Analyzer)"
- "Who is this for? (Beginners / Producers / Mix Engineers / Sound Designers / Live Performers)"

**Core concept:**
- "What's the main sonic goal?"
- "What problem does this solve?"

**Unique value:**
- "What makes this different from existing plugins?"
- "What's the 'signature sound' or feature?"

### Tier 2: Functional (ask early to shape design)
**Parameters:**
- "What controls should the user have?"
- "Any specific ranges or behaviors?"

**Audio routing:**
- "Mono, stereo, or sidechain input?"
- "Wet/dry mix control?"

**Presets:**
- "Should this come with factory presets?"
- "Any specific starting points users should have?"

**DSP approach:**
- "Any specific algorithms or processing techniques?"
- "Reference plugins for similar sound?"

### Tier 3: Optional (nice-to-have context)
**Workflow:**
- "Typical use case in a project?"
- "Serial or parallel processing?"

**Visual style:**
- "Any aesthetic inspiration?"
- "Minimal vs detailed UI?"

**Advanced features:**
- "Modulation sources?"
- "Automation features?"

### Tier 4: Never Ask (UI implementation details)
**Do NOT ask about:**
- Exact pixel dimensions ("How wide should the knob be?")
- Font choices ("What font for labels?")
- Color codes ("What hex color for the background?")
- Layout specifics ("Should buttons be left or right?")

**Why:** These are determined during UI mockup phase, not ideation.

## Question Batch Generation Rules

### Generate exactly 4 questions per batch:

1. **Prioritize gaps in Tier 1-2**
   - If any Tier 1 missing → Include it
   - Fill remaining slots with Tier 2 gaps
   - Only ask Tier 3 if Tiers 1-2 complete

2. **Never ask what's already known**
   - Parse previous responses
   - Skip questions where answer already provided
   - Refine questions based on context

3. **Use AskUserQuestion tool format:**
   ```
   questions:
     - question: "What type of plugin?"
       header: "Type"
       multiSelect: false
       options:
         - label: "Effect"
           description: "Process audio signals"
         - label: "Instrument"
           description: "Generate sounds"
   ```

4. **Balance question types:**
   - Mix closed (options) and open (text input via "Other")
   - Avoid all yes/no questions
   - Prefer specific over general

## Extraction Example: From User Input to Questions

**User says:**
"I want a reverb plugin."

**Extract what's known:**
- Type: Effect ✓
- Core concept: Reverb (vague - need refinement) ⚠
- Unique value: Unknown ✗
- Parameters: Unknown ✗
- Target user: Unknown ✗

**Generate 4 questions:**
1. **Tier 1 gap:** "What makes this reverb unique?" (unique value)
2. **Tier 1 gap:** "Who is this for?" (target user)
3. **Tier 2 gap:** "What reverb character?" (refine core concept)
4. **Tier 2 gap:** "What controls should users have?" (parameters)

**Avoid asking:**
- "What type of plugin?" (already know: Effect)
- "What color should the UI be?" (Tier 4 - UI details)
```

**Replace with (in SKILL.md lines 48-173):**
```markdown
### Phase 2: Gap Analysis and Question Prioritization

<step number="2" required="true">
**Extract from Phase 1 response, then identify gaps.**

For detailed question tier system and batch generation rules, see:
- `references/question-generation-patterns.md`

**Quick reference:**
- Tier 1: Critical (type, core concept, unique value)
- Tier 2: Functional (parameters, routing, presets, DSP)
- Tier 3: Optional (workflow, visual style, advanced features)
- Tier 4: NEVER ask UI implementation details

**Generate exactly 4 questions per batch prioritizing Tier 1-2 gaps.**
</step>
```

**Verification:** File exists at `.claude/skills/plugin-ideation/references/question-generation-patterns.md` and SKILL.md lines 48-173 reduced to ~20 lines

---

### Fix 2.2: Extract Adaptive Questioning Examples to References
**Location:** Lines 618-692
**Operation:** EXTRACT
**Create file:** `references/adaptive-questioning-examples.md`
**File content:**
```markdown
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
```

**Replace with (in SKILL.md lines 618-692):**
```markdown
## Examples: Question Generation Based on Input Detail

For detailed examples of adaptive question generation, see:
- `references/adaptive-questioning-examples.md`

**Quick summary:**
- **Detailed input:** Focus questions on gaps in Tier 2-3 (parameters, DSP, workflow)
- **Vague input:** Start with Tier 1 foundational questions, then narrow focus through multiple batches
- **Key principle:** Never ask about information already provided
```

**Verification:** File exists at `.claude/skills/plugin-ideation/references/adaptive-questioning-examples.md` and SKILL.md lines 618-692 reduced to ~10 lines

---

### Fix 2.3: Extract Adaptive Strategy to References
**Location:** Lines 694-734
**Operation:** EXTRACT
**Create file:** `references/adaptive-strategy.md`
**File content:**
```markdown
# Adaptive Questioning Strategy

## Core Principle: Extract First, Then Fill Gaps

The adaptive questioning system is designed to minimize user effort while maximizing useful context. This is achieved through a two-phase approach:

### Phase 1: Free-Form Extraction
Allow users to provide information in whatever format is natural to them:
- Detailed written descriptions
- Short one-liners
- Bulleted lists
- Stream-of-consciousness thoughts
- Reference examples ("like Plugin X but with Y")

**Key insight:** Users often know what they want but may not know which details matter for implementation. Let them dump their thoughts, then parse systematically.

### Phase 2: Gap-Driven Questioning
After extracting all provided information:
1. Map user input to the tier system (Tier 1: Critical → Tier 4: Never Ask)
2. Identify which tiers have gaps
3. Prioritize Tier 1-2 gaps for questions
4. Generate exactly 4 questions per batch targeting highest-priority gaps
5. Present decision gate: "Ready to finalize?" vs "Ask more questions"
6. Repeat until user chooses to finalize

## Why This Works

### Avoids Interrogation Fatigue
Instead of asking 20 questions upfront, adaptive questioning:
- Lets users provide as much or as little detail as they want initially
- Only asks about what's actually missing
- Allows users to bail out early ("Yes - I'm ready to finalize") if they're satisfied

### Respects User Knowledge Levels
Different users have different levels of clarity:
- **Beginners:** Vague ideas → Need more Tier 1 foundational questions
- **Experts:** Detailed specs → Jump straight to Tier 2-3 refinement questions
- **Everyone:** Decision gate after each batch lets them control depth

### Prevents Scope Creep
By explicitly categorizing Tier 4 (Never Ask UI Details), the system:
- Keeps ideation focused on concept and functionality
- Defers visual design to the mockup phase
- Avoids bikeshedding over pixel dimensions during brainstorming

## Continuous Iteration Until Satisfaction

The decision gate after each question batch creates a natural checkpoint:
```
Ready to finalize the creative brief?
1. Yes - I'm ready to finalize
2. No - Ask me more questions
3. Let me add more thoughts
```

This allows:
- **Option 1:** Early exit if user feels they've provided enough context
- **Option 2:** System-driven refinement (Claude asks next 4 targeted questions)
- **Option 3:** User-driven refinement (user adds free-form thoughts, then system re-analyzes gaps)

**Result:** Creative brief contains exactly as much detail as the user wants to provide, no more, no less.

## Implementation Notes

### Gap Analysis Algorithm
1. Parse all previous user responses (Phase 1 free-form + all question batch answers)
2. Check Tier 1 coverage:
   - Type, Core concept, Unique value, Target user
   - If any missing → Prioritize for next batch
3. Check Tier 2 coverage:
   - Parameters, Audio routing, Presets, DSP approach
   - Fill remaining question slots with these gaps
4. Check Tier 3 coverage:
   - Workflow, Visual style, Advanced features
   - Only ask if Tiers 1-2 complete
5. Never ask Tier 4 (UI implementation details)

### Question Batch Composition
- Always generate exactly 4 questions (not 3, not 5)
- Use AskUserQuestion tool with options
- Balance question types (closed options vs open "Other" text input)
- Make each question independent (can be answered in any order)

### State Management
- Track all extracted information across conversation
- Never re-ask questions about known information
- Merge free-form additions (Option 3) with existing context before re-analyzing gaps
```

**Replace with (in SKILL.md lines 694-734):**
```markdown
## Adaptive Questioning Strategy

For detailed explanation of the extract-first-then-fill-gaps approach, see:
- `references/adaptive-strategy.md`

**Core principle:**
1. Let user provide free-form input (no structure required)
2. Extract known information, identify gaps using tier system
3. Generate 4 targeted questions per batch
4. Present decision gate: finalize vs continue questioning
5. Repeat until user chooses to finalize

This avoids interrogation fatigue while ensuring critical details are captured.
```

**Verification:** File exists at `.claude/skills/plugin-ideation/references/adaptive-strategy.md` and SKILL.md lines 694-734 reduced to ~15 lines

---

### Fix 2.4: Extract Improvement Proposal Template to Assets
**Location:** Lines 474-515
**Operation:** EXTRACT
**Create file:** `assets/improvement-proposal.md`
**File content:**
```markdown
# Improvement Proposal: [PluginName]

## Current State
[Brief description of what exists today]

## Proposed Changes

### What to Add/Change
[Detailed description of the improvement]

### Why This Matters
[User value, problem solved, or creative opportunity]

### Implementation Notes
[Any technical considerations, DSP approaches, or integration points]

## Parameters Affected
[List any new parameters or changes to existing ones]
- Parameter name: Range, behavior, default value

## UI Changes
[If applicable, describe visual or layout changes needed]

## Presets
[Should existing presets be updated? New presets added?]

## Success Criteria
[How will you know this improvement is complete and working?]

---

**Metadata**
- Proposed by: [User]
- Date: [YYYY-MM-DD]
- Status: [Draft / In Progress / Implemented]
```

**Replace with (in SKILL.md lines 474-515):**
```markdown
### Phase 4: Document Creation

<step number="6" required="true">
When user chooses "finalize," create:

**File:** `plugins/[PluginName]/.ideas/improvement-proposal-[YYYYMMDD].md`

**Use template:** `assets/improvement-proposal.md`

**Fill in:**
- Current state (what exists)
- Proposed changes (what to add/modify)
- Why this matters (user value)
- Parameters affected (if any)
- UI changes (if any)
- Success criteria (how to verify)
</step>
```

**Verification:** File exists at `.claude/skills/plugin-ideation/assets/improvement-proposal.md` and SKILL.md references it instead of inline template

---

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Add Explicit Requirement Marker to Phase 1
**Location:** Line 37
**Operation:** REPLACE
**Before:**
```markdown
Start with open question:
```

**After:**
```markdown
MUST start with open question:
```

**Verification:** Line 37 now reads "MUST start with open question:"

---

### Fix 3.2: Add Explicit Marker to Document Creation Gate
**Location:** Line 204
**Operation:** REPLACE
**Before:**
```markdown
When user chooses "finalize" and name is confirmed, create:
```

**After:**
```markdown
MUST wait until user chooses "finalize" and name is confirmed, then create:
```

**Verification:** Line 204 includes "MUST wait until" instead of "When"

---

### Fix 3.3: Add Explicit Marker to Git Integration Warning
**Location:** Line 740
**Operation:** REPLACE
**Before:**
```markdown
## Git Integration

**Do NOT commit** - The user handles commits according to the checkpoint protocol.
```

**After:**
```markdown
## Git Integration

**NEVER commit** - The user handles commits according to the checkpoint protocol.

This skill only stages files. Commits are managed by the user or workflow orchestrator.
```

**Verification:** Line 740 uses "NEVER commit" instead of "Do NOT commit" and includes clarification

---

### Fix 3.4: Condense Vagueness Detection Rules
**Location:** Lines 581-598
**Operation:** REPLACE
**Before:**
```markdown
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
```

**After:**
```markdown
## Vagueness Detection Rules

<vagueness_check>
**Specific request has:**
- Named feature + Concrete action + Measurable criteria

**Vague request lacks:**
- Named feature OR concrete action OR measurable criteria

**Decision rule:**
If 2+ vague indicators AND 0 specific indicators:
→ MUST present choice: "Brainstorm approaches first" vs "Implement something reasonable"
</vagueness_check>
```

**Verification:** Lines 581-598 condensed from ~380 words to ~180 words, wrapped in `<vagueness_check>` tag

---

## File Creation Manifest

Files to create:
1. `.claude/skills/plugin-ideation/references/question-generation-patterns.md` - Question tier system, batch generation rules, extraction examples (~2,500 tokens)
2. `.claude/skills/plugin-ideation/references/adaptive-questioning-examples.md` - Full worked examples (detailed input: tape delay, vague input: distortion) (~1,300 tokens)
3. `.claude/skills/plugin-ideation/references/adaptive-strategy.md` - Extract-first-then-fill-gaps strategy explanation, continuous iteration rationale (~600 tokens)
4. `.claude/skills/plugin-ideation/assets/improvement-proposal.md` - Template for improvement proposals (~100 tokens)

## Execution Checklist

### Phase 1: Critical Fixes (45 minutes)
- [ ] Fix 1.1: Mode detection wrapped in `<critical_sequence>`
- [ ] Fix 1.2: New Plugin Mode opening `<critical_sequence>` tag added
- [ ] Fix 1.3: Phase 2 wrapped in `<step number="2">`
- [ ] Fix 1.4: Phase 3 wrapped in `<step number="3">` with MUST marker
- [ ] Fix 1.5: Phase 3.5 wrapped in `<decision_gate>` with blocking enforcement
- [ ] Fix 1.6: Phase 3.7 wrapped in `<step number="5">` with MUST markers
- [ ] Fix 1.7: Phase 4 wrapped in `<step number="6">` with MUST marker
- [ ] Fix 1.8: Phase 5 wrapped in `<state_requirement>` with must_create_continue_file
- [ ] Fix 1.9: Phase 6 wrapped in `<decision_gate>` with `<delegation_rules>` for ui-mockup, plugin-workflow, deep-research
- [ ] Fix 1.10: Improvement Mode wrapped in `<critical_sequence>` with same step/gate pattern
- [ ] Fix 1.11: YAML description includes autonomous trigger terms

### Phase 2: Content Extraction (20 minutes)
- [ ] Fix 2.1: Lines 48-173 extracted to `references/question-generation-patterns.md`
- [ ] Fix 2.1: SKILL.md lines 48-173 replaced with brief reference (~20 lines)
- [ ] Fix 2.2: Lines 618-692 extracted to `references/adaptive-questioning-examples.md`
- [ ] Fix 2.2: SKILL.md lines 618-692 replaced with brief reference (~10 lines)
- [ ] Fix 2.3: Lines 694-734 extracted to `references/adaptive-strategy.md`
- [ ] Fix 2.3: SKILL.md lines 694-734 replaced with brief reference (~15 lines)
- [ ] Fix 2.4: Lines 474-515 extracted to `assets/improvement-proposal.md`
- [ ] Fix 2.4: SKILL.md lines 474-515 replaced with template reference

### Phase 3: Polish (15 minutes)
- [ ] Fix 3.1: Line 37 uses "MUST start" instead of "Start"
- [ ] Fix 3.2: Line 204 uses "MUST wait until" instead of "When"
- [ ] Fix 3.3: Line 740 uses "NEVER commit" with clarification
- [ ] Fix 3.4: Lines 581-598 condensed and wrapped in `<vagueness_check>`

### Verification Tests
- [ ] SKILL.md opens with valid YAML frontmatter including trigger terms
- [ ] Mode Detection (lines 16-26) wrapped in `<critical_sequence>` with step tag
- [ ] New Plugin Mode (lines 17-336) has opening `<critical_sequence>` and closing `</critical_sequence>`
- [ ] All 8 phases in New Plugin Mode have `<step>` tags (numbered 1-8)
- [ ] Phase 3.5 and Phase 6 decision gates have `<decision_gate>` with blocking="true"
- [ ] Phase 6 delegation rules wrapped in `<delegation_rules>` with three `<delegation_rule>` tags
- [ ] Phase 5 wrapped in `<state_requirement>` tag
- [ ] Improvement Mode (lines 338-579) has same XML structure as New Plugin Mode
- [ ] Four new files created in references/ and assets/ directories
- [ ] SKILL.md reduced from ~803 lines to ~650 lines (~19% reduction)
- [ ] Context reduced from ~22,298 tokens to ~17,600 tokens (~21% reduction)
- [ ] All MUST/NEVER/ALWAYS markers added to critical instructions
- [ ] No "just do it" or ambiguous pronouns remain
- [ ] Git integration section explicitly states "NEVER commit"

## Post-Implementation Verification

### Expected Outcomes:
1. **Enforcement improvement:** Claude cannot skip phases or auto-proceed past decision gates
2. **Delegation clarity:** Claude must use Skill tool to invoke ui-mockup, plugin-workflow, deep-research (cannot implement inline)
3. **Context reduction:** Faster skill loading, reduced chance of missing critical instructions
4. **Maintenance improvement:** Examples and templates can be updated without touching SKILL.md core logic

### Regression Prevention:
- Mode detection must complete before workflow starts
- All phases must execute in order (1 → 2 → 3 → 3.5 → 3.7 → 4 → 5 → 6)
- Decision gates must block until user responds
- .continue-here.md file must be created (state requirement enforced)
- Delegation to other skills must use Skill tool (not inline implementation)

### Health Score Impact:
- **Before:** Overall health Yellow (3 critical issues, Structure 4/5, XML 2/5, Critical Sequence 1/5)
- **After (estimated):** Overall health Green (0 critical issues, Structure 5/5, XML 5/5, Critical Sequence 5/5)

---

## Summary

**Total fixes: 15**
- **Phase 1 (Critical):** 11 fixes addressing enforcement, blocking, and delegation
- **Phase 2 (Extraction):** 4 extractions reducing context by ~4,700 tokens
- **Phase 3 (Polish):** 4 clarity improvements with explicit markers

**Estimated time: 80 minutes**
- Critical sequence enforcement: 45 min
- Content extraction: 20 min
- Explicit markers and polish: 15 min

**Phase 1 critical fix count: 11**

**Expected improvement:**
- Context reduction: 22,298 → 17,600 tokens (~21% reduction)
- Line reduction: 803 → 650 lines (~19% reduction)
- Health score: Yellow → Green
- Critical issues: 3 → 0
- XML Organization: 2/5 → 5/5
- Critical Sequence Enforcement: 1/5 → 5/5
