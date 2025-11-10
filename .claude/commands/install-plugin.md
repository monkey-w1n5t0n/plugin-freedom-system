---
name: install-plugin
description: Install completed plugin to system folders for DAW use
---

# /install-plugin

When user runs `/install-plugin [PluginName]`, invoke the plugin-lifecycle skill.

## Preconditions

Before running this command:
- Plugin status must be ✅ Working (Stage 6 complete)
- Plugin must have successful Release build
- pluginval validation must have passed
- Plugin must have been tested in DAW from build folder

If any precondition fails, block execution and guide user to complete Stage 6 first.

## Behavior

1. Verify plugin exists and status is ✅ Working
2. Build plugin in Release mode (optimized, production-ready)
3. Extract PRODUCT_NAME from CMakeLists.txt
4. Remove old versions from system folders
5. Install to:
   - VST3: `~/Library/Audio/Plug-Ins/VST3/`
   - AU: `~/Library/Audio/Plug-Ins/Components/`
6. Set proper permissions (755)
7. Clear DAW caches:
   - Ableton Live: `~/Library/Preferences/Ableton/Live */PluginCache.db`
   - Logic Pro: `~/Library/Caches/AudioUnitCache/*`
   - Kill AudioComponentRegistrar process
8. Verify installation (check timestamps, file sizes)
9. Update PLUGINS.md: ✅ Working → 📦 Installed
10. Save logs to `logs/[PluginName]/build_TIMESTAMP.log`

## Success Output

```
✓ [PluginName] installed successfully

Installed formats:
- VST3: ~/Library/Audio/Plug-Ins/VST3/[ProductName].vst3 (X.X MB)
- AU: ~/Library/Audio/Plug-Ins/Components/[ProductName].component (X.X MB)

Cache status: Cleared (all DAWs)

Next steps:
1. Open your DAW (Logic Pro, Ableton Live, etc.)
2. Rescan plugins (or restart DAW)
3. Load plugin in a project
4. Test audio processing and presets
```

## Routes To

`plugin-lifecycle` skill
