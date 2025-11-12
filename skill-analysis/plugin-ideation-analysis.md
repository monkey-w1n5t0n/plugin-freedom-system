# Skill Analysis: plugin-ideation

## Executive Summary
- Overall health: **Yellow**
- Critical issues: **3**
- Optimization opportunities: **8**
- Estimated context savings: **~4,500 tokens / 20%**

## Dimension Scores (1-5)

1. Structure Compliance: **4/5** - Good YAML frontmatter, but description lacks autonomous trigger terms
2. Content Extraction: **2/5** - Large implementation sections remain inline; template already extracted but examples not
3. Instruction Clarity: **4/5** - Good numbered lists and structure, but lacks explicit requirement markers
4. XML Organization: **2/5** - Almost no XML tags for critical sequences or decision gates
5. Context Efficiency: **3/5** - Some verbosity; repeated patterns across modes
6. Claude-Optimized Language: **4/5** - Generally clear, some ambiguous pronoun usage
7. System Integration: **3/5** - Checkpoint protocol mentioned but not enforced with XML
8. Critical Sequence Enforcement: **1/5** - No XML wrappers for critical workflows

## Critical Issues (blockers for Claude comprehension)

### Issue 1: Missing Critical Sequence Enforcement (Lines 17-336)
The entire workflow (Phase 1 → Phase 2 → Phase 3 → Phase 3.5 → Phase 3.7 → Phase 4 → Phase 5 → Phase 6) is described procedurally but lacks `<critical_sequence>` tags to prevent Claude from skipping or reordering steps.

**Impact:** Claude may jump directly to document creation without completing question batches or may skip the decision gate.

### Issue 2: Decision Gates Not Wrapped (Lines 139-156, 453-471)
Phase 3.5 decision gates are described inline without `<decision_gate>` tags, making it unclear that execution must halt for user input.

**Impact:** Claude may auto-proceed past decision points, violating the checkpoint protocol requirement "WAIT for user response - NEVER auto-proceed."

### Issue 3: Delegation Rules Not Explicit (Lines 330-336, 572-579)
Lines like "Invoke `ui-mockup` skill" are instructions but not wrapped in `<delegation_rule>` tags, making it unclear when Claude MUST delegate vs when it's optional.

**Impact:** Claude may attempt to implement features within this skill instead of delegating to specialized skills.

## Optimization Opportunities

### Content to Extract to references/

**1. Lines 48-173 (Phase 2 & Phase 3): Question Generation Examples**
- Current: ~2,800 tokens inline
- Action: Extract to `references/question-generation-patterns.md`
- Keep in SKILL.md: "See references/question-generation-patterns.md for examples"
- Estimated savings: **~2,500 tokens**

**2. Lines 618-692 (Examples Section): Question Generation Based on Input Detail**
- Current: ~1,500 tokens of examples
- Action: Extract to `references/adaptive-questioning-examples.md`
- Keep in SKILL.md: Brief reference with link
- Estimated savings: **~1,300 tokens**

**3. Lines 694-734 (Adaptive Questioning Strategy + Continuous Iteration)**
- Current: ~800 tokens of strategy explanation
- Action: Extract to `references/adaptive-strategy.md`
- Keep in SKILL.md: One-paragraph summary
- Estimated savings: **~600 tokens**

**Total extraction savings: ~4,400 tokens**

### Content to Extract to assets/

**4. Lines 474-515: Improvement Proposal Template**
- Current: Inline markdown template
- Action: Extract to `assets/improvement-proposal.md` (similar to existing creative-brief.md)
- Keep in SKILL.md: "Use template at assets/improvement-proposal.md"
- Estimated savings: **~100 tokens** (template itself + reference to it)

### XML Tag Opportunities

**5. Lines 17-336: Wrap New Plugin Mode in `<critical_sequence>`**

**BEFORE:**
```markdown
## New Plugin Mode

### Phase 1: Free-Form Collection

Start with open question:
```
What would you like to build?
...
```
```

**AFTER:**
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
What would you like to build?
...
```
</step>

### Phase 2: Gap Analysis and Question Prioritization

<step number="2" required="true">
**Extract from Phase 1 response, then identify gaps:**
1. Parse user's free-form description
2. Check which tiers are covered
3. Identify missing critical/functional information
4. Never ask about already-provided information
</step>

### Phase 3: Question Batch Generation

<step number="3" required="true">
**Generate exactly 4 questions using AskUserQuestion based on identified gaps.**
</step>

### Phase 3.5: Decision Gate

<decision_gate>
<gate_name>finalize_or_continue</gate_name>
<blocking>true</blocking>
<step number="4" required="true">
**Use AskUserQuestion with 3 options after each question batch:**

Route based on answer:
- Option 1 → Proceed to Phase 4 (document creation)
- Option 2 → Return to Phase 2 (re-analyze gaps, generate next 4 questions)
- Option 3 → Collect free-form text, merge with context, return to Phase 2
</step>
</decision_gate>

### Phase 3.7: Plugin Name

<step number="5" required="true">
**Before creating documents, check if plugin name was provided at any point during conversation.**

If name NOT yet provided, ask via AskUserQuestion.
</step>

### Phase 4: Document Creation

<step number="6" required="true">
When user chooses "finalize" and name is confirmed, create:

**File:** `plugins/[PluginName]/.ideas/creative-brief.md`
</step>

### Phase 5: Session Handoff

<state_requirement>
<requirement>must_create_continue_file</requirement>
<step number="7" required="true">
Create `.continue-here.md` in `plugins/[PluginName]/.ideas/`
</step>
</state_requirement>

### Phase 6: Decision Menu

<decision_gate>
<gate_name>next_action</gate_name>
<blocking>true</blocking>
<checkpoint_protocol>true</checkpoint_protocol>
<step number="8" required="true">
Present next steps:

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
- Option 2 → Invoke `plugin-workflow` skill
- Option 3 → Invoke `deep-research` skill
- Option 4 → Confirm handoff file created, exit
- Option 5 → Ask what they'd like to do
</step>
</decision_gate>

</phases>
</critical_sequence>
```

**Estimated improvement:** High - prevents sequence violations

**6. Lines 330-336: Wrap Delegation in `<delegation_rule>`**

**BEFORE:**
```markdown
**Handle responses:**
- Option 1 → Invoke `ui-mockup` skill
- Option 2 → Invoke `plugin-workflow` skill (after warning about contracts)
- Option 3 → Invoke `deep-research` skill (Phase 7 - stub for now, just acknowledge)
- Option 4 → Confirm handoff file created, exit
- Option 5 → Ask what they'd like to do
```

**AFTER:**
```markdown
<delegation_rules>
**Handle responses:**

<delegation_rule skill="ui-mockup" trigger="option_1">
- Condition: User chooses option 1
- Action: MUST invoke ui-mockup skill
- Do NOT implement mockup within this skill
</delegation_rule>

<delegation_rule skill="plugin-workflow" trigger="option_2">
- Condition: User chooses option 2
- Action: MUST invoke plugin-workflow skill
- Warning: Warn about contracts before delegating
</delegation_rule>

<delegation_rule skill="deep-research" trigger="option_3">
- Condition: User chooses option 3
- Action: MUST invoke deep-research skill
- Note: Phase 7 implementation
</delegation_rule>

- Option 4 → Confirm handoff file created, exit
- Option 5 → Ask what they'd like to do
</delegation_rules>
```

**7. Lines 16-26: Add Mode Detection as `<critical_sequence>`**

**BEFORE:**
```markdown
## Mode Detection

**Check if plugin exists:**

```bash
grep "^### $PLUGIN_NAME$" PLUGINS.md
```

- If found → **Improvement Mode**
- If not found → **New Plugin Mode**
```

**AFTER:**
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

### Instruction Clarity Improvements

**8. Line 3: Add Trigger Terms to Description**

**BEFORE:**
```yaml
description: Adaptive brainstorming for plugin concepts and improvements
```

**AFTER:**
```yaml
description: Adaptive brainstorming for plugin concepts and improvements. Autonomous triggers: "I want to make...", "Explore improvements to...", "brainstorm", "ideate", "new plugin", "improve plugin"
```

**Impact:** Helps Claude autonomously activate this skill when appropriate.

**9. Lines Throughout: Add Explicit Requirement Markers**

Add MUST/NEVER/ALWAYS/SHOULD markers to critical instructions:

- Line 37: "Start with open question:" → "MUST start with open question:"
- Line 83: "Generate exactly 4 questions" → "MUST generate exactly 4 questions"
- Line 204: "When user chooses 'finalize'" → "MUST wait until user chooses 'finalize'"
- Line 740: "Do NOT commit" → "NEVER commit - user handles commits"

### Verbosity Reductions

**10. Lines 581-598: Condense Vagueness Detection Rules**

**BEFORE (380 words):**
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

**AFTER (180 words - 53% reduction):**
```markdown
## Vagueness Detection Rules

<vagueness_check>
**Specific request has:**
- Named feature + Concrete action + Measurable criteria

**Vague request lacks:**
- Named feature OR concrete action OR measurable criteria

**Decision rule:**
If 2+ vague indicators AND 0 specific indicators:
→ Present choice: "Brainstorm approaches first" vs "Implement something reasonable"
</vagueness_check>
```

**Estimated savings: ~200 tokens**

### Critical Sequence Enforcement

**11. Lines 338-579: Improvement Mode Needs Same XML Structure**

Apply the same `<critical_sequence>` wrapping to Improvement Mode as recommended for New Plugin Mode above.

**Structure:**
```markdown
## Improvement Mode

<critical_sequence>
<sequence_name>improvement_workflow</sequence_name>
<enforcement>must_complete_in_order</enforcement>
<phases>

### Phase 0: Vagueness Detection
<decision_gate>
...
</decision_gate>

### Phase 1: Free-Form Collection
<step number="1" required="true">
...
</step>

... [remaining phases with step wrappers]

</phases>
</critical_sequence>
```

## Examples of Issues Found

### Example 1: Missing Critical Sequence Enforcement

**Location:** Lines 17-336 (New Plugin Mode)

**Problem:** The entire 6-phase workflow is described linearly but lacks XML enforcement tags. Claude could skip Phase 2 (Gap Analysis) and jump to Phase 4 (Document Creation) if the user says "just make a creative brief."

**Before:**
```markdown
## New Plugin Mode

### Phase 1: Free-Form Collection

Start with open question:
...

### Phase 2: Gap Analysis and Question Prioritization
...
```

**After:**
```markdown
## New Plugin Mode

<critical_sequence>
<sequence_name>new_plugin_workflow</sequence_name>
<enforcement>must_complete_in_order</enforcement>

### Phase 1: Free-Form Collection
<step number="1" required="true">
Start with open question:
...
</step>

### Phase 2: Gap Analysis and Question Prioritization
<step number="2" required="true">
...
</step>
</critical_sequence>
```

**Impact:** Prevents Claude from violating the adaptive questioning flow.

---

### Example 2: Delegation Not Enforced

**Location:** Lines 330-336 (Phase 6 Decision Menu)

**Problem:** Instructions say "Invoke `ui-mockup` skill" but it's just a bullet point. Claude might try to generate a mockup inline instead of delegating.

**Before:**
```markdown
**Handle responses:**
- Option 1 → Invoke `ui-mockup` skill
- Option 2 → Invoke `plugin-workflow` skill (after warning about contracts)
```

**After:**
```markdown
<delegation_rules>
<delegation_rule skill="ui-mockup" trigger="option_1">
- Condition: User chooses option 1
- Action: MUST invoke ui-mockup skill via Skill tool
- Do NOT attempt to create mockup within this skill
</delegation_rule>

<delegation_rule skill="plugin-workflow" trigger="option_2">
- Condition: User chooses option 2
- Action: MUST invoke plugin-workflow skill via Skill tool
- Warning: Warn user about contract requirements before delegating
</delegation_rule>
</delegation_rules>
```

**Impact:** Enforces the dispatcher pattern and prevents skill scope creep.

---

### Example 3: Decision Gate Not Blocking

**Location:** Lines 139-156 (Phase 3.5 Decision Gate)

**Problem:** Decision gate is described but not wrapped in XML. Claude might auto-proceed if it thinks it has enough context.

**Before:**
```markdown
### Phase 3.5: Decision Gate

**Use AskUserQuestion with 3 options after each question batch:**

```
Question:
  question: "Ready to finalize the creative brief?"
  ...
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

**MUST use AskUserQuestion with 3 options after each question batch:**

<step number="4" required="true">
```
Question:
  question: "Ready to finalize the creative brief?"
  ...
```

**MUST wait for user response. NEVER auto-proceed.**

Route based on answer:
- Option 1 → Proceed to Phase 4 (document creation)
- Option 2 → Return to Phase 2 (re-analyze gaps, generate next 4 questions)
- Option 3 → Collect free-form text, merge with context, return to Phase 2
</step>
</decision_gate>
```

**Impact:** Aligns with checkpoint protocol requirement: "WAIT for user response - NEVER auto-proceed."

---

### Example 4: Verbose Repeated Content

**Location:** Lines 618-692 (Examples Section)

**Problem:** Two full examples with detailed question batches (~1,500 tokens) that could be extracted to references/ since they're supportive rather than procedural.

**Before:**
```markdown
## Examples: Question Generation Based on Input Detail

### Example 1: Detailed Input (New Plugin)

```
User: "I want a tape delay with wow and flutter modulation. Should have three knobs and a vintage aesthetic."

Extracted:
- Type: Effect ✓
- Core concept: Tape delay with modulation ✓
... [75 lines of examples]
```

**After:**
```markdown
## Examples: Question Generation Based on Input Detail

For detailed examples of adaptive question generation patterns, see:
- `references/adaptive-questioning-examples.md` - Detailed Input Example (tape delay)
- `references/adaptive-questioning-examples.md` - Vague Input Example (distortion plugin)

**Quick reference:**
- Detailed input → Focus questions on gaps (Tier 2-3)
- Vague input → Start with foundational questions (Tier 1), then refine
```

**Impact:** Reduces SKILL.md from 803 lines to ~650 lines while preserving all guidance.

---

### Example 5: Ambiguous Pronoun Usage

**Location:** Line 334

**Problem:** "Phase 7 - stub for now, just acknowledge" - unclear what "just acknowledge" means.

**Before:**
```markdown
- Option 3 → Invoke `deep-research` skill (Phase 7 - stub for now, just acknowledge)
```

**After:**
```markdown
<delegation_rule skill="deep-research" trigger="option_3">
- Condition: User chooses option 3
- Action: MUST invoke deep-research skill
- Note: deep-research skill is fully implemented (Phase 7 complete)
</delegation_rule>
```

**Impact:** Removes ambiguity and corrects outdated comment (Phase 7 is complete per system docs).

## Estimated Impact

### Context Reduction
- **Extraction to references/**: ~4,400 tokens saved
- **Verbosity reduction**: ~200 tokens saved
- **Template extraction**: ~100 tokens saved
- **Total**: ~4,700 tokens (~20% reduction from 22,298 to ~17,600 tokens)

### Comprehension Improvement: **High**
- XML tags make critical sequences unambiguous
- Decision gates explicitly block execution
- Delegation rules prevent scope creep
- Reduced context means faster loading and less chance of Claude missing critical instructions

### Maintenance Improvement: **High**
- Examples in references/ can be updated without touching SKILL.md
- XML structure makes it easy to identify enforcement points
- Clear separation between procedure (SKILL.md) and examples (references/)
- Template changes don't require SKILL.md updates

## Priority Recommendations

### 1. Add Critical Sequence Enforcement (HIGHEST PRIORITY)
**Impact:** Prevents workflow violations
**Effort:** 45 minutes
**Files affected:** SKILL.md lines 17-336 and 338-579

Wrap both New Plugin Mode and Improvement Mode in `<critical_sequence>` tags with:
- `<step>` tags for each phase
- `<decision_gate>` tags for Phase 3.5 and Phase 6
- `<delegation_rule>` tags for skill invocations
- `<state_requirement>` tags for .continue-here.md creation

**Why critical:** Without this, Claude can skip phases or auto-proceed past decision gates, violating the checkpoint protocol that's central to the Plugin Freedom System design.

### 2. Extract Examples to references/ (HIGH PRIORITY)
**Impact:** 4,400 token reduction (~20%)
**Effort:** 20 minutes
**Files created:**
- `references/question-generation-patterns.md` (Lines 48-173)
- `references/adaptive-questioning-examples.md` (Lines 618-692)
- `references/adaptive-strategy.md` (Lines 694-734)

**Why important:** Reduces context load while preserving all guidance. Examples are useful for understanding but not needed during execution once the pattern is learned.

### 3. Add Explicit Requirement Markers (MEDIUM PRIORITY)
**Impact:** Clarity improvement for critical instructions
**Effort:** 15 minutes
**Changes:** Add MUST/NEVER/ALWAYS/SHOULD to ~12 key instructions

**Why useful:** Makes it crystal clear which instructions are mandatory vs recommended. Aligns with Anthropic's guidance on explicit requirements.

## Implementation Notes

### Files that would be created in references/:
1. `references/question-generation-patterns.md` - Question tier system, batch generation rules, extraction examples
2. `references/adaptive-questioning-examples.md` - Full worked examples (tape delay detailed input, distortion vague input)
3. `references/adaptive-strategy.md` - Extract-first-then-fill-gaps strategy explanation

### Files that would be created in assets/:
1. `assets/improvement-proposal.md` - Template for improvement proposals (similar to existing creative-brief.md)

### Estimated time to implement all fixes:
**Total: ~80 minutes**
- Critical sequence enforcement: 45 min
- Content extraction: 20 min
- Explicit markers: 15 min

### Implementation order:
1. First: Add XML tags (highest impact on correctness)
2. Second: Extract examples (reduces context)
3. Third: Add explicit markers (polish)

---

## Additional Notes

### Strengths of Current Implementation:
- Well-organized phase structure
- Good use of AskUserQuestion tool
- Clear mode detection (New Plugin vs Improvement)
- Comprehensive examples
- Good integration with checkpoint protocol (described, if not enforced)

### What Works Well:
- The adaptive questioning strategy is sound
- The tier system (Tier 1: Critical → Tier 4: Never Ask UI) is clear
- The decision gate concept is correct (just needs XML enforcement)
- Git integration section is appropriately minimal (stage but don't commit)

### System Integration Notes:
- Correctly uses inline numbered decision menus (not AskUserQuestion for menus)
- Correctly delegates to other skills
- Creates proper handoff files (.continue-here.md)
- Updates state files (PLUGINS.md)

The skill is structurally sound and functionally complete. The main improvements are about **making implicit invariants explicit** through XML tags and **reducing context load** through extraction.
