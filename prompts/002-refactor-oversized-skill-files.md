<objective>
Refactor oversized Claude Code skill files into modular, maintainable structures following Anthropic's skill pattern best practices. The goal is to improve readability, reduce file sizes to manageable levels (<700 lines per file), and maintain system functionality without any data loss or behavioral changes.

Three skills exceed the 700-line threshold and require refactoring:
- plugin-workflow/SKILL.md: 3332 lines (CRITICAL - truncated by Read tool at 2000 lines)
- plugin-lifecycle/SKILL.md: 990 lines
- context-resume/SKILL.md: 866 lines

This refactoring is essential because Claude Code's Read tool cannot fully load files over 2000 lines, making plugin-workflow/SKILL.md literally unreadable by the system.
</objective>

<context>
You are working in the Plugin Freedom System, a JUCE plugin development workflow system built on Claude Code. Skills are the primary organizational unit, following Anthropic's pattern:

```
.claude/skills/[skill-name]/
├── SKILL.md (entry point, metadata, orchestration logic)
├── references/ (detailed implementations, substeps)
└── assets/ (templates, examples, contracts)
```

Current structure already exists but SKILL.md files have grown monolithic. The refactoring must preserve all content while splitting into logical subfiles in the references/ directory.

@.claude/skills/plugin-workflow/SKILL.md
@.claude/skills/plugin-lifecycle/SKILL.md
@.claude/skills/context-resume/SKILL.md
</context>

<safety_requirements>
CRITICAL - This is a zero-tolerance refactoring:

1. **Backup first**: Create timestamped backups before ANY modifications
   - `cp SKILL.md SKILL.md.backup-$(date +%Y%m%d-%H%M%S)`

2. **No content loss**: Every single line must be preserved somewhere in the new structure

3. **Preserve functionality**: All skill invocations, subagent calls, and decision logic must work identically

4. **Atomic commits**: Commit each skill refactoring separately with clear messages

5. **Verification gates**: After each skill refactoring, verify the new structure is complete before moving to the next skill
</safety_requirements>

<analysis_phase>
Before making ANY changes, thoroughly analyze each skill:

## For plugin-workflow/SKILL.md (3332 lines):

1. Read the FULL file in sections if needed (use offset/limit parameters)
2. Identify major sections:
   - Precondition Checking
   - Stage Dispatcher (routing logic)
   - Stage 0: Research
   - Stage 1: Planning
   - Stage 2: Foundation
   - Stage 3: Shell
   - Stage 4: DSP
   - Stage 5: GUI
   - Stage 6: Validation
   - Any other top-level sections

3. Map cross-references between sections
4. Identify what MUST stay in main SKILL.md vs what can move to references/

## For plugin-lifecycle/SKILL.md (990 lines):

1. Identify major operational phases or substeps
2. Determine logical split points (likely: install, uninstall, validate, backup/restore)
3. Map dependencies between sections

## For context-resume/SKILL.md (866 lines):

1. Identify resume strategies or workflow types
2. Determine if splitting by stage or by resume pattern
3. Map state management logic
</analysis_phase>

<refactoring_strategy>
## Target Structure for Each Skill

### SKILL.md (Main Entry Point) - Target: 200-400 lines
Should contain:
- YAML frontmatter (name, description, tools, preconditions)
- Purpose and overview
- High-level workflow/orchestration logic
- Decision routing (which reference file to invoke when)
- Links to reference files with clear context
- Common error handling patterns
- Version changelog (if present)

Should NOT contain:
- Detailed step-by-step implementations
- Large code examples (move to references/)
- Repetitive patterns (extract to references/)
- Stage-specific logic (move to references/)

### references/ Subdirectory
Each reference file should:
- Be self-contained for its specific substep/phase
- Be 300-600 lines maximum
- Include all context needed for that substep
- Have a clear, descriptive filename
- Start with a header explaining its purpose and when it's invoked

## Proposed Split for plugin-workflow:

```
SKILL.md (300-400 lines)
  ├── Frontmatter, purpose, overview
  ├── Precondition checking logic
  ├── Stage dispatcher (routing only)
  ├── State management patterns
  └── Links to stage implementations

references/
  ├── stage-0-research.md (~400 lines)
  ├── stage-1-planning.md (~350 lines)
  ├── stage-2-foundation.md (~450 lines)
  ├── stage-3-shell.md (~450 lines)
  ├── stage-4-dsp.md (~600 lines - complex, may need further split)
  ├── stage-5-gui.md (~500 lines)
  ├── stage-6-validation.md (~400 lines)
  └── decision-menus.md (~150 lines - shared menu patterns)
```

## Proposed Split for plugin-lifecycle:

Analyze first, then determine structure. Likely:
```
SKILL.md (200-300 lines)
references/
  ├── install-process.md
  ├── uninstall-process.md
  ├── validation-checks.md
  └── backup-restore.md
```

## Proposed Split for context-resume:

Analyze first, then determine structure. Likely:
```
SKILL.md (200-300 lines)
references/
  ├── stage-resume-patterns.md
  ├── handoff-parsing.md
  ├── state-reconstruction.md
  └── error-recovery.md
```
</refactoring_strategy>

<implementation_process>
Execute refactoring in this exact order:

## Phase 1: Analysis & Planning

1. Read each oversized SKILL.md completely (use Read with offset/limit if needed)
2. Document the actual section structure discovered
3. Confirm the split strategy for each skill
4. Create detailed split plan with line number ranges

## Phase 2: Backup

```bash
cd .claude/skills
for skill in plugin-workflow plugin-lifecycle context-resume; do
  cp "$skill/SKILL.md" "$skill/SKILL.md.backup-$(date +%Y%m%d-%H%M%S)"
done
```

Confirm backups exist before proceeding.

## Phase 3: Refactor plugin-workflow (Highest Priority)

1. Create new reference files in `references/` subdirectory
2. Extract each stage implementation to its reference file
3. Update main SKILL.md to:
   - Keep dispatcher logic
   - Add clear links to reference files with invocation context
   - Preserve all metadata and frontmatter
4. Verify: Check that sum of all new files' content equals original SKILL.md
5. Test: Verify skill can still be invoked (read through main logic)
6. Commit: `git add . && git commit -m "refactor(plugin-workflow): split 3332-line SKILL.md into modular references"`

## Phase 4: Refactor plugin-lifecycle

(Same process as Phase 3)

## Phase 5: Refactor context-resume

(Same process as Phase 3)

## Phase 6: Verification

For each refactored skill:
1. Read new SKILL.md - verify it's coherent and complete as entry point
2. Read each new reference file - verify complete and self-contained
3. Check for broken cross-references
4. Verify total line count (original lines = sum of new files)
5. Check git diff to confirm no content lost

## Phase 7: Cleanup

Only after ALL verifications pass:
```bash
# Remove backups (keep for now, document location)
echo "Backups preserved at:"
find .claude/skills -name "SKILL.md.backup-*" -ls
```
</implementation_process>

<reference_file_format>
Each reference file should follow this structure:

```markdown
# [Stage/Phase Name]: [Description]

**Context:** This file is part of the [skill-name] skill.
**Invoked by:** [Describe when/how this is called from main SKILL.md]
**Purpose:** [1-2 sentence overview]

---

## [Section 1]

[Content...]

## [Section 2]

[Content...]

---

**Return to:** Main [skill-name] orchestration in `SKILL.md`
```
</reference_file_format>

<cross_reference_updates>
After splitting, update any internal links:

1. Search for references to section headers that moved to references/
2. Update markdown links: `[Stage 4: DSP](#stage-4-dsp)` → `[Stage 4: DSP](references/stage-4-dsp.md)`
3. Update any "See Stage X" mentions with proper links
4. Ensure dispatcher logic points to correct reference files
</cross_reference_updates>

<success_criteria>
Before declaring complete, verify ALL of these:

✓ All three skills refactored
✓ No SKILL.md file exceeds 700 lines
✓ Backups exist for all original files
✓ Git commits created for each skill refactoring
✓ All content preserved (no lines lost)
✓ Cross-references updated correctly
✓ Each reference file has clear header explaining context
✓ Main SKILL.md files are coherent entry points with clear orchestration logic
✓ references/ subdirectories organized logically
✓ Total line count verification: sum(new files) = original file for each skill
✓ Documentation of what was split and why
</success_criteria>

<verification_script>
Create a verification script to confirm integrity:

```bash
#!/bin/bash
# verify-skill-refactoring.sh

echo "Verifying skill refactoring integrity..."

for skill in plugin-workflow plugin-lifecycle context-resume; do
  echo "\n=== Checking $skill ==="

  # Check backup exists
  backup=$(ls .claude/skills/$skill/SKILL.md.backup-* 2>/dev/null | head -1)
  if [ -n "$backup" ]; then
    echo "✓ Backup found: $backup"
    original_lines=$(wc -l < "$backup")
    echo "  Original: $original_lines lines"
  else
    echo "✗ No backup found!"
    continue
  fi

  # Count new SKILL.md
  new_main=$(wc -l < ".claude/skills/$skill/SKILL.md")
  echo "  New SKILL.md: $new_main lines"

  # Count all reference files
  if [ -d ".claude/skills/$skill/references" ]; then
    ref_total=$(find ".claude/skills/$skill/references" -name "*.md" -exec wc -l {} + | tail -1 | awk '{print $1}')
    echo "  References total: $ref_total lines"
    echo "  Combined: $((new_main + ref_total)) lines"

    diff=$((original_lines - (new_main + ref_total)))
    if [ $diff -eq 0 ]; then
      echo "✓ Line count matches perfectly"
    else
      echo "⚠ Line count difference: $diff lines"
    fi
  fi
done
```
</verification_script>

<output>
Execute all phases sequentially. After completion:

1. Create `./docs/skill-refactoring-report.md` documenting:
   - What was split and why
   - Before/after file sizes
   - New structure for each skill
   - Verification results

2. All refactored skills committed to git separately

3. Final summary showing:
   - Original line counts
   - New file structure with line counts
   - Verification checklist status
</output>
