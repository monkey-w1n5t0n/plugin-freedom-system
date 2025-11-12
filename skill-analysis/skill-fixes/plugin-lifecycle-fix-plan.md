# Fix Plan: plugin-lifecycle

## Summary
- Critical fixes: **5**
- Extraction operations: **1** (optional, low priority)
- Total estimated changes: **6 operations**
- Estimated time: **37 minutes** (critical path) | **60 minutes** (full implementation)

## Phase 1: Critical Fixes (Enforcement & Blocking Issues)

### Fix 1.1: Add Critical Sequence Enforcement to Installation Workflow
**Location:** Lines 56-70
**Operation:** WRAP
**Before:**
```markdown
## Installation Workflow

The complete installation process is documented in detail in the reference files:

1. **Build Verification** - Check Release binaries exist, offer to build if missing
2. **Product Name Extraction** - Extract PRODUCT_NAME from CMakeLists.txt
3. **Old Version Removal** - Remove existing installations to prevent conflicts
4. **Copy to System Folders** - Install VST3 and AU to macOS plugin directories
5. **Permissions Verification** - Set 755 permissions for DAW access
6. **Cache Clearing** - Clear Ableton Live and Logic Pro caches
7. **Verification** - Confirm installation with file checks and size validation
8. **PLUGINS.md Update** - Record installation status and locations
```
**After:**
```markdown
## Installation Workflow

<critical_sequence enforcement="strict" blocking="true">
The following steps MUST execute in order without skipping:

1. **Build Verification** - Check Release binaries exist, offer to build if missing
   - Delegate to: `references/installation-process.md` Step 1
   - Blocking: YES - cannot proceed without binaries

2. **Product Name Extraction** - Extract PRODUCT_NAME from CMakeLists.txt
   - Delegate to: `references/installation-process.md` Step 2
   - Required for: All subsequent steps that reference ${PRODUCT_NAME}

3. **Old Version Removal** - Remove existing installations to prevent conflicts
   - Delegate to: `references/installation-process.md` Step 3
   - Blocking: NO - can proceed if no old version exists

4. **Copy to System Folders** - Install VST3 and AU to macOS plugin directories
   - Delegate to: `references/installation-process.md` Step 4
   - Blocking: YES - core installation step

5. **Permissions Verification** - Set 755 permissions for DAW access
   - Delegate to: `references/installation-process.md` Step 5
   - Blocking: YES - DAWs cannot load plugins without correct permissions

6. **Cache Clearing** - Clear Ableton Live and Logic Pro caches
   - Delegate to: `references/cache-management.md`
   - Blocking: NO - warn if fails but continue

7. **Verification** - Confirm installation with file checks and size validation
   - Delegate to: `references/installation-process.md` Step 7
   - Blocking: YES - must confirm success before declaring completion

8. **PLUGINS.md Update** - Record installation status and locations
   - Delegate to: `references/installation-process.md` Step 8
   - Blocking: YES - state tracking is part of success criteria

</critical_sequence>
```
**Verification:**
- Check that `<critical_sequence>` tags exist at lines 58 and 89
- Verify each step has "Blocking: YES/NO" marker
- Confirm "Delegate to:" reference for each step

---

### Fix 1.2: Add Decision Gate to Destroy Safety Protocol
**Location:** Lines 129-135
**Operation:** WRAP
**Before:**
```markdown
**Safety features:**
- Timestamped backup created before deletion
- Requires typing exact plugin name to confirm
- Blocks if status is 🚧 (protects in-progress work)
```
**After:**
```markdown
<decision_gate type="destructive_confirmation" bypass="never">
**Safety Protocol - Requires Explicit User Confirmation:**

<gate_conditions>
1. Check status ≠ 🚧 (block if in development)
2. Create timestamped backup to `backups/destroyed/`
3. Show deletion preview (files, sizes, locations)
4. Require exact plugin name match (case-sensitive)
</gate_conditions>

<confirmation_format>
User MUST type exact plugin name to proceed.
- Correct match → Proceed to deletion
- Incorrect match → Abort with error message
- Empty response → Abort
</confirmation_format>

You MUST NOT proceed with deletion until user provides correct confirmation.
This gate cannot be bypassed - it protects against accidental data loss.
</decision_gate>
```
**Verification:**
- Check that `<decision_gate>` tags wrap safety protocol at lines ~129-150
- Verify `bypass="never"` attribute exists
- Confirm `<gate_conditions>` and `<confirmation_format>` sections present

---

### Fix 1.3: Add Checkpoint Protocol Decision Gate
**Location:** Lines 177-200
**Operation:** WRAP
**Before:**
```markdown
## Decision Menu After Installation

After successful installation:

```
✓ [PluginName] installed successfully

What's next?
1. Test in DAW (recommended) → Load in Logic/Ableton/Reaper
2. Create another plugin → /dream
3. Document this plugin → Create user guide/manual
4. Share plugin (export build) → Future: Package for distribution
5. Other

Choose (1-5): _
```

**Handle responses:**
- **Option 1:** Provide DAW testing guidance
- **Option 2:** Invoke `plugin-ideation` skill
- **Option 3:** Suggest creating user manual in `.ideas/`
- **Option 4:** Provide build export instructions (future feature)
- **Option 5:** Ask what they'd like to do
```
**After:**
```markdown
## Decision Menu After Installation

<decision_gate type="checkpoint" enforcement="strict">
After successful installation, you MUST present this decision menu and WAIT for user response.

<checkpoint_protocol>
1. Display completion statement with checkmark
2. Present numbered menu (5 options)
3. STOP execution and wait for user choice
4. Do NOT auto-proceed or make assumptions
5. Only continue after receiving user selection (1-5)
</checkpoint_protocol>

<menu_format>
```
✓ [PluginName] installed successfully

What's next?
1. Test in DAW (recommended) → Load in Logic/Ableton/Reaper
2. Create another plugin → /dream
3. Document this plugin → Create user guide/manual
4. Share plugin (export build) → Future: Package for distribution
5. Other

Choose (1-5): _
```
</menu_format>

<response_handlers>
- **Option 1:** Provide DAW testing guidance (terminal - ends workflow)
- **Option 2:** Invoke `plugin-ideation` skill via natural language (not direct Tool call)
- **Option 3:** Suggest creating user manual in `.ideas/` directory
- **Option 4:** Provide build export instructions (future feature note)
- **Option 5:** Ask open-ended "What would you like to do?"
</response_handlers>

This is a system-wide checkpoint protocol enforcement. Never skip this menu.
</decision_gate>
```
**Verification:**
- Check that `<decision_gate type="checkpoint">` wraps entire section at lines ~177-220
- Verify `<checkpoint_protocol>`, `<menu_format>`, and `<response_handlers>` sections exist
- Confirm "STOP execution and wait" appears in checkpoint_protocol

---

### Fix 1.4: Add Handoff Protocol Section
**Location:** After Line 218 (end of Integration Points section)
**Operation:** INSERT
**Change:**
```markdown

<handoff_protocol>
## Invocation Protocol

When this skill is invoked, check the triggering context:

<invocation_modes>
1. **Direct command** (e.g., `/install-plugin GainKnob`)
   - Extract plugin name from command argument
   - Set mode = 1 (Installation)
   - Execute installation workflow

2. **From plugin-workflow** (after Stage 6 completion)
   - Plugin name provided in handoff state
   - Offer installation via decision menu
   - IF user chooses install → Set mode = 1 and execute

3. **From plugin-improve** (after successful changes)
   - Plugin name in context
   - Offer reinstallation (only if previously installed)
   - IF user accepts → Set mode = 1 and execute

4. **Natural language** ("Install TapeAge")
   - Parse plugin name from utterance
   - Set mode = 1 (Installation)
   - Execute installation workflow

5. **Interactive menu** (from `/clean` command)
   - Plugin name provided
   - Set mode = Menu
   - Present options, wait for selection, route to chosen mode
</invocation_modes>

<return_protocol>
After completion, this skill MUST:
1. Present decision menu (checkpoint protocol)
2. Wait for user response
3. Return control to user (terminal skill - does not invoke other skills)

Do NOT automatically proceed to next action.
</return_protocol>
</handoff_protocol>
```
**Verification:**
- Check that new section exists after line 218 (Integration Points)
- Verify `<handoff_protocol>` opening and closing tags
- Confirm `<invocation_modes>` documents 5 entry points
- Confirm `<return_protocol>` documents terminal skill behavior

---

### Fix 1.5: Clarify Invocation Pattern in Mode Dispatcher
**Location:** Lines 42-43
**Operation:** REPLACE
**Before:**
```markdown
**Pattern:** Commands are thin routers that invoke this skill with a specific mode. The skill dispatches to the appropriate reference file for detailed implementation.
```
**After:**
```markdown
**Invocation Pattern:**
When user runs `/install-plugin [Name]`, the command expands to a prompt that invokes THIS skill.
You then read the mode from context and delegate to the appropriate reference file:
- Mode 1 → Read and execute `references/installation-process.md`
- Mode 2 → Read and execute `references/uninstallation-process.md`
- Mode 3 → Read and execute `references/mode-3-reset.md`
- Mode 4 → Read and execute `references/mode-4-destroy.md`
- Menu → Present menu, wait for selection, then route to chosen mode
```
**Verification:**
- Check that text at lines 42-48 now explicitly describes command expansion
- Verify bullet list shows mode → reference file mapping
- Confirm "Read and execute" verbiage (not just "dispatch to")

---

## Phase 2: Content Extraction (Context Reduction)

### Fix 2.1: Extract Menu Template to Assets (Optional, Low Priority)
**Location:** Lines 144-154
**Operation:** EXTRACT
**Create file:** `assets/menu-template.txt`
**File content:**
```
Plugin Lifecycle Management

Choose an operation:
1. Install/Reinstall → Copy binaries to system folders, set permissions, clear caches
2. Uninstall → Remove from system folders (keep source code)
3. Reset to Ideation → Remove all implementation (keep .ideas/)
4. Destroy → Complete removal including source code (with backup)
5. Cancel → Exit without changes

Choose (1-5): _
```
**Replace with:**
```markdown
See `assets/menu-template.txt` for the interactive menu format.
```
**Verification:**
- File exists at `.claude/skills/plugin-lifecycle/assets/menu-template.txt`
- SKILL.md reference updated at line ~144
- SKILL.md reduced by ~10 lines

**NOTE:** This extraction is optional and low-priority. Skip if time-constrained.

---

## Phase 3: Polish (Clarity & Verbosity)

### Fix 3.1: Reduce System Folders Explanation
**Location:** Lines 44-53
**Operation:** REPLACE
**Before:**
```markdown
**Why this matters:**

DAWs scan system folders for plugins. Installing to the correct locations with proper permissions ensures your plugin appears in the DAW's plugin list.

**System folders (macOS):**

- **VST3**: `~/Library/Audio/Plug-Ins/VST3/`
- **AU (Audio Unit)**: `~/Library/Audio/Plug-Ins/Components/`
- **AAX** (Pro Tools): `~/Library/Application Support/Avid/Audio/Plug-Ins/` (future support)
```
**After:**
```markdown
**Installation targets (macOS):**
- VST3: `~/Library/Audio/Plug-Ins/VST3/`
- AU: `~/Library/Audio/Plug-Ins/Components/`
- AAX: `~/Library/Application Support/Avid/Audio/Plug-Ins/` (future)

DAWs scan these locations. Correct installation = plugin appears in DAW.
```
**Verification:**
- Text at lines 44-48 now condensed to 5 lines (down from 10)
- All three plugin formats still listed
- Core message preserved ("DAWs scan these locations")

---

### Fix 3.2: Specify Mode in Notes for Claude Header
**Location:** Line 256
**Operation:** REPLACE
**Before:**
```markdown
## Notes for Claude

**When executing this skill:**
```
**After:**
```markdown
## Notes for Claude - Mode 1 (Installation)

**When executing the installation workflow (Mode 1):**
```
**Verification:**
- Header at line ~256 now specifies "Mode 1 (Installation)"
- Clarifies that notes apply specifically to installation workflow

---

### Fix 3.3: Add Explicit Conditionals to Menu Logic
**Location:** Lines 155-160
**Operation:** REPLACE
**Before:**
```markdown
**Menu logic:**
- Read current plugin status from PLUGINS.md
- Show appropriate options based on status
- Route to selected mode
- Handle cancellation gracefully
```
**After:**
```markdown
**Menu logic:**
1. Read current status from PLUGINS.md
2. Filter options based on status:
   - IF status = "💡 Ideated" → Show options 2, 3, 4 (cannot uninstall what isn't built)
   - IF status = "✅ Working" → Show options 1, 2, 3, 4 (not installed yet)
   - IF status = "📦 Installed" → Show ALL options (full lifecycle available)
   - IF status = "🚧 In Progress" → Show only option 4 (Cancel)
3. Present filtered menu, wait for user choice
4. Route to selected mode (1-4) or exit on Cancel
```
**Verification:**
- Text at lines 155-162 now has explicit IF/THEN conditionals
- Four status values with corresponding option filters documented
- Numbered steps (1-4) provide clear execution order

---

### Fix 3.4: Remove Redundant Common Pitfalls Section
**Location:** Lines 269-275
**Operation:** DELETE
**Remove:**
```markdown
**Common pitfalls:**

- Forgetting to quote paths with spaces (breaks with product names like "Tape Age")
- Not removing old versions (causes DAW confusion)
- Not clearing caches (DAW shows stale plugin metadata)
- Missing PLUGINS.md update (state tracking incomplete)
```
**Verification:**
- Lines 269-275 removed entirely
- Content already covered in "Notes for Claude" section (lines 258-268)
- File reduced by ~7 lines

---

## File Creation Manifest

Files to create:
1. `assets/menu-template.txt` - Interactive cleanup menu template (OPTIONAL, low priority)

Files to modify:
1. `.claude/skills/plugin-lifecycle/SKILL.md` - All critical fixes and polish operations

---

## Execution Checklist

### Phase 1: Critical Fixes (25 minutes)
- [ ] Fix 1.1: Installation workflow wrapped in `<critical_sequence>` with blocking markers
- [ ] Fix 1.2: Destroy safety wrapped in `<decision_gate type="destructive_confirmation">`
- [ ] Fix 1.3: Post-installation menu wrapped in `<decision_gate type="checkpoint">`
- [ ] Fix 1.4: Handoff protocol section added after line 218
- [ ] Fix 1.5: Invocation pattern clarified with explicit mode → file mapping

### Phase 2: Content Extraction (5 minutes, OPTIONAL)
- [ ] Fix 2.1: Menu template extracted to `assets/menu-template.txt`

### Phase 3: Polish (12 minutes)
- [ ] Fix 3.1: System folders explanation reduced from 10 to 5 lines
- [ ] Fix 3.2: "Notes for Claude" header specifies Mode 1
- [ ] Fix 3.3: Menu logic has explicit IF/THEN conditionals
- [ ] Fix 3.4: Redundant "Common pitfalls" section removed

### Verification (5 minutes)
- [ ] All `<critical_sequence>` tags have `enforcement="strict"` and `blocking="true"`
- [ ] All decision gates have `type` and `enforcement` attributes
- [ ] Handoff protocol documents all 5 invocation modes
- [ ] No broken references (all delegations point to existing files)
- [ ] Context reduced by ~200 tokens (system folders + common pitfalls)
- [ ] Skill health improved from "Green" to "Green+" (enforcement coverage now 5/5)

---

## Impact Assessment

### Before Fixes
- **XML Organization:** 2/5 (missing critical sequences)
- **Critical Sequence Enforcement:** 2/5 (described but not enforced)
- **Instruction Clarity:** 4/5 (some vague conditionals)
- **Context Efficiency:** 4/5 (minor verbosity)

### After Fixes
- **XML Organization:** 5/5 (all workflows wrapped in enforcement tags)
- **Critical Sequence Enforcement:** 5/5 (explicit blocking markers, decision gates)
- **Instruction Clarity:** 5/5 (explicit conditionals, mode-specific notes)
- **Context Efficiency:** 5/5 (reduced by ~200 tokens, 3%)

### Overall Health
- **Before:** Green (2 critical issues)
- **After:** Green+ (0 critical issues, best-in-class enforcement)

---

## Estimated Time Breakdown

| Phase | Task | Time |
|-------|------|------|
| **Phase 1** | Fix 1.1: Installation critical sequence | 8 min |
| | Fix 1.2: Destroy decision gate | 5 min |
| | Fix 1.3: Checkpoint protocol gate | 7 min |
| | Fix 1.4: Handoff protocol | 12 min |
| | Fix 1.5: Invocation pattern clarification | 3 min |
| **Phase 1 Subtotal** | | **35 min** |
| **Phase 2** | Fix 2.1: Menu template extraction (optional) | 5 min |
| **Phase 2 Subtotal** | | **5 min** |
| **Phase 3** | Fix 3.1: System folders reduction | 3 min |
| | Fix 3.2: Notes header specification | 1 min |
| | Fix 3.3: Menu logic conditionals | 5 min |
| | Fix 3.4: Remove common pitfalls | 1 min |
| **Phase 3 Subtotal** | | **10 min** |
| **Verification** | | **5 min** |
| **Total (Critical Path)** | Phase 1 only | **35 min** |
| **Total (Full Implementation)** | All phases | **55 min** |

---

## Priority Justification

### Why Phase 1 is Critical
1. **Fix 1.1 (Installation Sequence):** Without enforcement, Claude may skip build verification or permissions checks, leading to broken installations that fail silently in DAWs.
2. **Fix 1.2 (Destroy Gate):** Destructive operations without explicit confirmation violate safety protocols and risk data loss.
3. **Fix 1.3 (Checkpoint Protocol):** System-wide invariant violation. Breaking checkpoint protocol undermines the entire discovery mechanism.
4. **Fix 1.4 (Handoff Protocol):** Invoked from 5+ entry points. Without formal protocol, context confusion leads to wrong-mode execution.
5. **Fix 1.5 (Invocation Clarity):** Prevents "which file do I read?" confusion at dispatcher layer.

### Why Phase 2 is Optional
- Menu template extraction saves only ~100 tokens
- Template is already clear in current location
- Not worth the time investment for marginal gain

### Why Phase 3 is Polish
- Improvements are clarity-focused, not correctness-focused
- Claude will still execute correctly without these fixes
- Time better spent on Phase 1 enforcement if constrained
