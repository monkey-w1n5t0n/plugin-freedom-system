# Stage 4: DSP

**Context:** This file is part of the plugin-workflow skill.
**Invoked by:** Main workflow dispatcher after Stage 3 completion
**Purpose:** Implement audio processing where parameters control DSP

---

**Goal:** Implement audio processing, parameters control DSP

**Duration:** 15-45 minutes (depending on complexity)

**Preconditions:**
- Stage 3 complete (parameters implemented)
- architecture.md exists (DSP component specifications)
- plan.md exists (complexity score, phase breakdown)

## Complexity-Based Routing

### 1. Read Complexity Score from plan.md

Determine if phased implementation is required:

```typescript
const planContent = readFile(`plugins/${pluginName}/.ideas/plan.md`)
const complexityScore = extractComplexityScore(planContent)
const hasPhases = planContent.includes("### Phase 4.1") || planContent.includes("## Stage 4: DSP Phases")

console.log(`Complexity: ${complexityScore} (${hasPhases ? 'phased' : 'single-pass'})`)
```

### 2a. Single-Pass Implementation (Complexity ≤2)

**If complexity ≤2 OR no phases defined:**

Invoke dsp-agent once for complete DSP implementation using model: sonnet.

### 2b. Phased Implementation (Complexity ≥3)

**If complexity ≥3 AND plan.md defines phases:**

Parse phase breakdown and execute each phase sequentially (Phase 4.1, 4.2, 4.3):
- Use opus model for complexity ≥4
- Git commit after each phase
- Decision menu between phases
- Stop on failure

## Post-DSP Actions

### Auto-Invoke plugin-testing Skill

After Stage 4 succeeds (all phases if phased):

1. Run automated tests (build, load, process, parameter, state)
2. If tests fail: STOP, present failure menu
3. If tests pass: Continue

**CRITICAL: Do NOT proceed to Stage 5 if tests fail.**

### Invoke validator for Complexity ≥4 Plugins

For complexity ≥4 plugins, run semantic validation:
- Check all DSP components from architecture.md implemented
- Verify processBlock() real-time safety (no allocations, ScopedNoDenormals)
- Check parameter connections
- Advisory layer (user makes final call)

## State Updates and Commit

```bash
git add plugins/[PluginName]/Source/
git add plugins/[PluginName]/.continue-here.md
git add PLUGINS.md

git commit -m "feat: [PluginName] Stage 4 - DSP complete"
```

## Decision Menu

```
✓ Stage 4 complete: Audio processing operational

Plugin: [PluginName]
DSP components: [N]
Parameters connected: [N]
Tests: All passed

What's next?
1. Continue to Stage 5 (implement UI) (recommended)
2. Test audio manually in DAW
3. Review DSP code
4. Adjust DSP before UI
5. Pause here
6. Other

Choose (1-6): _
```

---

**Return to:** Main plugin-workflow orchestration in `SKILL.md`
