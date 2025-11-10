# State Management and Workflow Functions

**Context:** This file is part of the plugin-workflow skill.
**Invoked by:** All stages for state updates, git commits, and handoff management
**Purpose:** Centralized state machine functions, git operations, decision menus, and handoff file management

---

## State Machine Functions

### updatePluginStatus(pluginName, newStatus)

**Purpose:** Update plugin status emoji in PLUGINS.md.

**Valid statuses:**
- `💡 Ideated` - Creative brief exists, no Source/
- `🚧 Stage N` - In development (with stage number)
- `🚧 Stage N.M` - In development (with stage and phase)
- `✅ Working` - Stage 6 complete, not installed
- `📦 Installed` - Deployed to system folders

**Implementation:**
1. Read PLUGINS.md
2. Find section: `### [pluginName]`
3. Update `**Status:**` line with new emoji and text
4. Validate transition is legal
5. Write back to PLUGINS.md

### createPluginEntry(pluginName, type, brief)

Create initial PLUGINS.md entry when starting new plugin.

### updatePluginTimeline(pluginName, stage, description)

Add timeline entry to PLUGINS.md when stage completes:

```markdown
- **[YYYY-MM-DD] (Stage N):** [description]
```

### getPluginStatus(pluginName)

Return current status emoji for routing logic.

Returns: `{ emoji: "🚧", stage: 4, text: "Stage 4" }`

### validateTransition(currentStatus, newStatus)

**Purpose:** Enforce legal state machine transitions.

**Legal transitions:**
```
💡 Ideated → 🚧 Stage 0 (start workflow)
🚧 Stage N → 🚧 Stage N+1 (sequential stages)
🚧 Stage 6 → ✅ Working (validation complete)
✅ Working → 📦 Installed (install plugin)
```

**Illegal transitions:**
```
💡 → ✅ (can't skip implementation)
🚧 Stage 2 → 🚧 Stage 5 (can't skip stages)
✅ Working → 💡 (can't go backward)
```

## Decision Menu System

### presentDecisionMenu(context)

**Purpose:** Present context-aware decision menu at every checkpoint.

**Format - Inline Numbered List (NOT AskUserQuestion):**

```
✓ [Completion statement]

What's next?
1. [Primary action] (recommended)
2. [Secondary action]
3. [Discovery feature] ← User discovers [capability]
4. [Alternative path]
5. [Escape hatch]
6. Other

Choose (1-6): _
```

**Keyword Shortcuts:**
- "continue" → Option 1
- "pause" → Pause option
- "review" → Review option

### generateContextualOptions(context)

Generate situation-specific menu options based on:
- Current stage (0-6)
- Success/failure state
- Complexity score
- Available actions

### formatDecisionMenu(completionStatement, options)

Format options as inline numbered list with:
- Recommended markers
- Discovery hints (← progressive disclosure)
- Clear numbering

### handleMenuChoice(choice, options, context)

Parse user response and execute chosen action:
- Numbers: Execute corresponding option
- Keywords: "continue", "pause", "review"
- "Other": Ask freeform, then re-present menu

## Git Commit Functions

### commitStage(pluginName, stage, description)

**Purpose:** Create standardized git commit after stage completion.

**Commit message format:**
```
feat: [PluginName] Stage [N] - [description]

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

**Implementation:**
1. Stage changes atomically in single commit
2. Commit with standardized message using heredoc
3. Verify commit succeeded
4. Display commit hash to user
5. If commit fails: warn user, continue workflow (don't block)

**Atomic state transitions:**
PLUGINS.md update + handoff update + code changes = Single commit

### verifyGitAvailable()

Check git is available before workflow starts. Call at beginning of Stage 0.

## Handoff Management Functions

### createHandoff(pluginName, stage, context)

**Purpose:** Create initial handoff file after Stage 0 completion.

**Implementation:**
1. Read handoff template from `.claude/skills/plugin-workflow/assets/continue-here-template.md`
2. Fill in YAML frontmatter (plugin, stage, status, last_updated, etc.)
3. Fill in markdown sections with context
4. Write to `plugins/[pluginName]/.continue-here.md`

### updateHandoff(pluginName, stage, completed, nextSteps, complexityScore, phased)

**Purpose:** Update handoff file after each stage/phase completion.

**Implementation:**
1. Read existing `.continue-here.md`
2. Update YAML frontmatter (stage, phase, status, last_updated, complexity_score, phased_implementation)
3. Append to "Completed So Far" section
4. Update "Next Steps" with new actions
5. Update "Context to Preserve" with latest context
6. Write back to file

### deleteHandoff(pluginName)

**Purpose:** Remove handoff file when plugin reaches ✅ Working or 📦 Installed.

**When to call:**
- After Stage 6 complete (status → ✅ Working)
- After plugin installation (status → 📦 Installed)

## Checkpoint Types

### Hard Checkpoints (MUST pause for user decision)

**Stages:**
- Stage 0: Research complete
- Stage 1: Planning complete
- Stage 6: Validation complete

**Behavior:**
1. Complete stage work
2. Auto-commit changes
3. Update handoff file
4. Update PLUGINS.md
5. Present decision menu
6. **WAIT for user response** - do NOT auto-continue
7. Execute user choice

### Soft Checkpoints (can auto-continue)

**Phases within complex stages (complexity ≥3):**
- Stage 4.1, 4.2, 4.3: DSP phases
- Stage 5.1, 5.2: GUI phases

**Behavior:**
Present decision menu with "Continue to next phase" as recommended option. If user chooses continue, proceed. If pause, stop and preserve state.

### Decision Checkpoints

**Occur before significant choices:**
- Build failures (4-option menu)
- Validation failures (3-option menu)
- Manual pause requests

## Stage Boundary Protocol

**At every stage completion:**

1. Show completion statement
2. Run automated tests (Stages 4, 5 only)
3. Auto-commit
4. Update `.continue-here.md`
5. Update PLUGINS.md
6. Present decision menu
7. Wait for user response

**Do NOT auto-proceed without user confirmation.**

## Resume Handling

Support "resume automation" command:

1. Read `.continue-here.md` to determine current stage/phase
2. Parse YAML frontmatter
3. Continue from documented "Next Steps"
4. Load relevant context (contracts, research, plan)

---

**Return to:** Main plugin-workflow orchestration in `SKILL.md`
