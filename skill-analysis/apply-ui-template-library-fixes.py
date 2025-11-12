#!/usr/bin/env python3
"""Apply ui-template-library fixes systematically"""

import re
from pathlib import Path

SKILL_PATH = Path("/Users/lexchristopherson/Developer/plugin-freedom-system/.claude/skills/ui-template-library/SKILL.md")

def read_file(path):
    return path.read_text()

def write_file(path, content):
    path.write_text(content)

def apply_fixes():
    content = read_file(SKILL_PATH)
    lines = content.split('\n')

    # Fix 1.5: Add explicit operation selection instruction (before line 38)
    for i, line in enumerate(lines):
        if line == "## Operations":
            lines.insert(i+1, "")
            lines.insert(i+2, "**You MUST determine which operation the user is requesting, then execute ONLY that operation.**")
            break

    # Fix 1.6: Strengthen sequencing language in Save operation (after line ~68)
    for i, line in enumerate(lines):
        if line == "**High-level process:**":
            lines[i] = "**CRITICAL: Execute these steps in exact order. Do NOT skip steps.**"
            break

    # Fix 1.1: Add critical sequence to Save operation (before "## Operation 1")
    xml_save = """<operation name="save_aesthetic">
  <purpose>Capture visual design from mockup as structured prose aesthetic.md</purpose>

  <critical_sequence>
    <step id="1" required="true">
      Read mockup HTML file into memory (Read tool)
    </step>
    <step id="2" required="true" depends_on="1">
      Extract visual patterns using pattern-extraction.md strategies
    </step>
    <step id="3" required="true" depends_on="2">
      Generate prose descriptions using prose-generation.md guidelines
    </step>
    <step id="4" required="true" depends_on="3">
      Write aesthetic.md following aesthetic-template.md structure exactly
    </step>
    <step id="5" required="true" depends_on="4">
      Copy preview.html to aesthetic directory
    </step>
    <step id="6" required="true" depends_on="5">
      Generate metadata.json with inferred tags
    </step>
    <step id="7" required="true" depends_on="6">
      Update manifest.json using Read → modify → Write pattern
    </step>
    <step id="8" required="true" depends_on="7">
      Present confirmation + decision menu
    </step>
  </critical_sequence>

  <state_requirement>
    <before_completion>
      ALL steps 1-7 must complete successfully
      IF any step fails: halt, report error, DO NOT proceed to step 8
    </before_completion>

    <verification>
      Before step 8:
      - VERIFY aesthetic.md exists and is not empty
      - VERIFY preview.html exists
      - VERIFY metadata.json is valid JSON
      - VERIFY manifest.json updated successfully
    </verification>
  </state_requirement>

"""

    for i, line in enumerate(lines):
        if line == "## Operation 1: Save Aesthetic":
            lines.insert(i, xml_save)
            break

    # Fix 1.2: Replace Step 8 Confirmation with Decision Gate
    step8_save_replacement = """### Step 8: Present Decision Menu

<decision_gate wait_required="true">
  Present numbered decision menu (inline format, NOT AskUserQuestion tool).
  MUST WAIT for user response. DO NOT auto-proceed.
</decision_gate>

**Format:**
```
✅ Aesthetic saved: "[NAME]" ([ID])

Files created:
- .claude/aesthetics/[ID]/aesthetic.md
- .claude/aesthetics/[ID]/preview.html
- .claude/aesthetics/[ID]/metadata.json

What's next?
1. Apply to existing plugin (recommended)
2. Preview aesthetic in browser
3. List all aesthetics
4. Return to ui-mockup workflow
5. Other

Choose (1-5): _
```

**WAIT for user input. DO NOT execute any option automatically.**"""

    # Find and replace Step 8 Confirmation for Save operation
    in_step8 = False
    step8_start = -1
    step8_end = -1

    for i, line in enumerate(lines):
        if line == "### Step 8: Confirmation" and step8_start == -1:
            step8_start = i
            in_step8 = True
        elif in_step8 and line == "---":
            step8_end = i
            break

    if step8_start != -1 and step8_end != -1:
        # Remove old content
        del lines[step8_start:step8_end]
        # Insert new content
        lines.insert(step8_start, step8_save_replacement)

    # Fix 1.3: Add critical sequence to Apply operation
    xml_apply = """<operation name="apply_aesthetic">
  <purpose>Generate new mockup by interpreting aesthetic prose and adapting to target plugin</purpose>

  <critical_sequence>
    <step id="1" required="true">
      Load aesthetic.md and metadata.json (Read tool)
    </step>
    <step id="2" required="true">
      Read target plugin parameter-spec.md or creative-brief.md
    </step>
    <step id="3" required="true" depends_on="1,2">
      Interpret aesthetic prose using aesthetic-interpretation.md strategies
    </step>
    <step id="4" required="true" depends_on="3">
      Choose layout based on parameter count (layout-generation.md)
    </step>
    <step id="5" required="true" depends_on="3,4">
      Generate HTML/CSS applying aesthetic to chosen layout
    </step>
    <step id="6" required="true" depends_on="5">
      Save generated mockup to target plugin mockups directory
    </step>
    <step id="7" required="true" depends_on="6">
      Update aesthetic metadata.json usedInPlugins array
    </step>
    <step id="8" required="true" depends_on="7">
      Present decision menu (inline numbered, NOT AskUserQuestion)
    </step>
  </critical_sequence>

  <decision_gate wait_required="true">
    Present decision menu with preview/continue options
    MUST wait for user response
  </decision_gate>

"""

    for i, line in enumerate(lines):
        if line == "## Operation 2: Apply Aesthetic":
            lines.insert(i, xml_apply)
            break

    # Fix 1.4: Replace Step 8 for Apply operation
    step8_apply_replacement = """### Step 8: Present Decision Menu

<decision_gate wait_required="true">
  Present numbered decision menu (inline format, NOT AskUserQuestion tool).
  MUST WAIT for user response. DO NOT auto-proceed.
</decision_gate>

**Format:**
```
✅ Mockup generated: plugins/[Plugin]/mockups/mockup-[plugin].html

Applied aesthetic: "[NAME]" ([ID])

What's next?
1. Preview mockup in browser (recommended)
2. Continue to ui-mockup Phase 2 (detailed design iteration)
3. Apply different aesthetic to same plugin
4. Adjust current aesthetic and re-apply
5. Other

Choose (1-5): _
```

**WAIT for user input. DO NOT execute any option automatically.**"""

    # Find second Step 8
    step8_count = 0
    for i, line in enumerate(lines):
        if line == "### Step 8: Present Decision Menu":
            step8_count += 1
            if step8_count == 2:
                # Find end
                for j in range(i+1, len(lines)):
                    if lines[j] == "---":
                        del lines[i:j]
                        lines.insert(i, step8_apply_replacement)
                        break
                break

    # Fix 3.5: Add critical sequence to List operation
    xml_list = """<operation name="list_aesthetics">
  <critical_sequence>
    <step id="1" required="true">Read manifest.json</step>
    <step id="2" required="true">Format as markdown table</step>
    <step id="3" required="true">Show preview paths</step>
    <step id="4" required="true">Present decision menu</step>
  </critical_sequence>

  <decision_gate wait_required="true">
    Wait for user to select action from menu
  </decision_gate>

"""

    for i, line in enumerate(lines):
        if line == "## Operation 3: List Aesthetics":
            lines.insert(i, xml_list)
            break

    # Fix 1.7: Replace ui-mockup integration with handoff protocol
    handoff_protocol = """## Integration with ui-mockup Skill

<handoff_protocol>
  <from_skill>ui-mockup</from_skill>
  <to_skill>ui-template-library</to_skill>

  <trigger_conditions>
    <condition when="Phase 0" if="aesthetics exist in library">
      ui-mockup presents option "Start from aesthetic template"
      User selects option → invoke ui-template-library "apply" operation
    </condition>
    <condition when="Phase 5.5 decision menu">
      ui-mockup presents option "Save as aesthetic template"
      User selects option → invoke ui-template-library "save" operation
    </condition>
  </trigger_conditions>

  <data_contract>
    <input name="mockup_path" type="absolute_path" required="true">
      Path to finalized mockup HTML file
    </input>
    <input name="plugin_name" type="string" required="true">
      Name of plugin for aesthetic source tracking
    </input>
    <input name="aesthetic_name" type="string" required="false">
      Optional custom name for aesthetic (defaults to plugin name)
    </input>
  </data_contract>

  <return_values>
    <output name="aesthetic_id" type="string">
      Generated aesthetic ID (e.g., "vintage-hardware-001")
    </output>
    <output name="aesthetic_path" type="absolute_path">
      Path to aesthetic directory
    </output>
  </return_values>
</handoff_protocol>

<delegation_rule>
  <from>ui-mockup skill</from>
  <to>ui-template-library skill</to>
  <method>inline_invocation</method>
  <rationale>
    Both skills are lightweight and stateless.
    No need for separate agent context.
    ui-mockup simply expands to include ui-template-library operations.
  </rationale>
</delegation_rule>"""

    # Find and replace Integration section
    for i, line in enumerate(lines):
        if line == "## Integration with ui-mockup Skill":
            # Find end (next ## or ---)
            for j in range(i+1, len(lines)):
                if lines[j].startswith("##") and lines[j] != "## Integration with ui-mockup Skill":
                    del lines[i:j]
                    lines.insert(i, handoff_protocol)
                    break
            break

    # Write back
    content = '\n'.join(lines)
    write_file(SKILL_PATH, content)
    print(f"Applied Phase 1 fixes to {SKILL_PATH}")

if __name__ == "__main__":
    apply_fixes()
