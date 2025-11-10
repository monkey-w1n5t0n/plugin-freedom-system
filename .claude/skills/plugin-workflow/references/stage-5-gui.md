# Stage 5: GUI

**Context:** This file is part of the plugin-workflow skill.
**Invoked by:** Main workflow dispatcher after Stage 4 completion
**Purpose:** Integrate WebView UI with parameter bindings

---

**Goal:** Integrate WebView UI with parameter bindings

**Duration:** 20-60 minutes (depending on complexity)

**Preconditions:**
- Stage 4 complete and tests passed (DSP operational)
- Finalized UI mockup exists (v[N]-ui.html)
- parameter-spec.md exists

## Actions

### 1. Locate Finalized UI Mockup

**Scan for finalized mockup version:**

```bash
LATEST_MOCKUP=$(ls -1 plugins/${PLUGIN_NAME}/.ideas/mockups/v*-ui.html | sort -V | tail -1)
```

**If no mockup found:** STOP IMMEDIATELY, exit skill, guide user to create mockup via /mockup command.

### 2. Read Complexity and Check for Phases

Same as Stage 4 - check if phased implementation needed (complexity ≥3):

```typescript
const planContent = readFile(`plugins/${pluginName}/.ideas/plan.md`)
const complexityScore = extractComplexityScore(planContent)
const hasPhases = planContent.includes("### Phase 5.1") || planContent.includes("## Stage 5: GUI Phases")
```

### 3. Single-Pass vs Phased Implementation

**Single-pass (complexity ≤2):**
Invoke gui-agent once for complete UI integration using model: sonnet

**Phased (complexity ≥3):**
Execute Phase 5.1, 5.2 sequentially with git commits between phases

**CRITICAL:** Member declaration order in PluginEditor.h prevents 90% of release build crashes:
```cpp
// Correct order:
Relays → WebView → Attachments
```

### 4. Handle Success/Failure

**If success:** Display mockup version, binding count, member order status
**If failure:** 4-option menu (investigate, show code, show build output, manual fix)

Special handling for binding_mismatch error (zero-drift violation):
- Expected parameters from parameter-spec.md must match implemented relay/attachment pairs
- Missing bindings indicate incomplete implementation

## Post-GUI Actions

### Auto-Invoke plugin-testing Skill

After Stage 5 succeeds:
1. Run all 5 tests + UI-specific checks (6 total)
2. Verify WebView renders and parameters sync
3. If tests fail: STOP, do not proceed to Stage 6

### Invoke validator for Complexity ≥4 Plugins

Validate:
- Member declaration order correct (Relays → WebView → Attachments)
- All parameters from parameter-spec.md have UI bindings
- HTML element IDs match relay names
- UI aesthetic matches mockup
- Visual feedback (knobs respond to parameter changes)
- WebView initialization includes error handling

## State Updates and Commit

```bash
git add plugins/[PluginName]/Source/
git add plugins/[PluginName]/ui/
git add plugins/[PluginName]/CMakeLists.txt
git add plugins/[PluginName]/.continue-here.md
git add PLUGINS.md

git commit -m "feat: [PluginName] Stage 5 - GUI complete"
```

## Decision Menu

```
✓ Stage 5 complete: UI operational

Plugin: [PluginName]
UI: WebView integrated from [mockup version]
Bindings: [N] parameters bound to UI
Tests: All passed (including UI sync)

What's next?
1. Continue to Stage 6 (final validation) (recommended)
2. Test UI manually in DAW
3. Review UI code
4. Adjust UI styling
5. Pause here
6. Other

Choose (1-6): _
```

**CRITICAL: Do NOT proceed to Stage 6 if tests fail.**

---

**Return to:** Main plugin-workflow orchestration in `SKILL.md`
