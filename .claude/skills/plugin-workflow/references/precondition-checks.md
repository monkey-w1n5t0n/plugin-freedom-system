# Precondition Checks

## Contract Verification

Before starting Stage 2, verify all required planning documents exist:

```bash
# Check for architecture document (Stage 0 output)
test -f "plugins/$PLUGIN_NAME/.ideas/architecture.md"

# Check for implementation plan (Stage 1 output)
test -f "plugins/$PLUGIN_NAME/.ideas/plan.md"

# Check for creative brief (ideation output)
test -f "plugins/$PLUGIN_NAME/.ideas/creative-brief.md"
```

## Status Verification

Read PLUGINS.md to verify current plugin status:

```bash
# Extract plugin section
grep "^### $PLUGIN_NAME$" PLUGINS.md
```

Parse the Status line to determine current state:
- 🚧 Stage 1 → Planning just completed, OK to proceed
- 🚧 Stage N (N ≥ 2) → Implementation in progress, OK to resume
- 💡 Ideated → Planning not started, BLOCK
- ✅ Working → Plugin complete, BLOCK (suggest /improve)
- 📦 Installed → Plugin complete and installed, BLOCK (suggest /improve)

## Block Messages

### Missing Contracts
```
[PluginName] is missing required planning documents.

Missing files will be listed here:
- architecture.md (from Stage 0)
- plan.md (from Stage 1)
- creative-brief.md (from ideation)

Run /plan [PluginName] to complete planning stages 0-1.
```

### Wrong Status
```
[PluginName] needs planning before implementation.
Run /plan [PluginName] to complete stages 0-1.
```

OR

```
[PluginName] is already complete.
Use /improve [PluginName] to make changes.
```
