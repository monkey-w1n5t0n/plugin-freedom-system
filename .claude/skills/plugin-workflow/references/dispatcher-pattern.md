# Dispatcher Pattern

## Overview
The orchestrator uses the dispatcher pattern to route implementation stages to specialized subagents. This ensures separation of concerns - the orchestrator handles workflow state and checkpoints, while subagents handle implementation.

## Dispatcher Logic (Pseudocode)

```javascript
async function dispatchStage(pluginName, stageNumber) {
  // Check preconditions
  const preconditionCheck = checkStagePreconditions(pluginName, stageNumber)

  if (!preconditionCheck.allowed) {
    console.log(`✗ BLOCKED: ${preconditionCheck.reason}`)
    console.log(`Action: ${preconditionCheck.action}`)
    return { status: 'blocked', reason: preconditionCheck.reason }
  }

  // ALWAYS invoke subagents via Task tool for stages 2-5
  switch(stageNumber) {
    case 2:
      // Invoke foundation-agent subagent
      return await invokeSubagent('foundation-agent', {
        pluginName,
        contracts: loadContracts(pluginName),
        requiredReading: 'juce8-critical-patterns.md'
      })
    case 3:
      // Invoke shell-agent subagent
      return await invokeSubagent('shell-agent', {
        pluginName,
        contracts: loadContracts(pluginName),
        requiredReading: 'juce8-critical-patterns.md'
      })
    case 4:
      // Invoke dsp-agent subagent
      return await invokeSubagent('dsp-agent', {
        pluginName,
        contracts: loadContracts(pluginName),
        requiredReading: 'juce8-critical-patterns.md'
      })
    case 5:
      // Invoke gui-agent subagent
      return await invokeSubagent('gui-agent', {
        pluginName,
        contracts: loadContracts(pluginName),
        requiredReading: 'juce8-critical-patterns.md'
      })
    case 6:
      // Can run directly or invoke validator subagent
      return executeStage6Validation(pluginName)  // See references/stage-6-validation.md
    default:
      return { status: 'error', reason: `Invalid stage: ${stageNumber}` }
  }
}
```

## Key Principles

1. **Always delegate stages 2-5** - Use Task tool to invoke subagents
2. **Pass contracts and Required Reading** - Every subagent receives architecture.md, plan.md, and juce8-critical-patterns.md
3. **Check preconditions first** - Validate contracts exist before dispatching
4. **Stage 6 flexibility** - Can run directly in orchestrator or via validator subagent

## Integration

The dispatcher is called from the main workflow loop:
```javascript
const result = await dispatchStage(pluginName, currentStage)
```

After dispatch completes, the orchestrator runs the checkpoint protocol (see state-management.md).
