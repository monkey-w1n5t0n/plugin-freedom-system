---
name: show-standalone
description: Open plugin UI in Standalone mode for visual inspection
---

# /show-standalone

When user runs `/show-standalone [PluginName]`, execute directly (no skill routing).

## Preconditions

- Plugin must exist
- Plugin must have source code (any stage)

This command works at any stage of development, even before completion.

## Behavior

1. Identify plugin name from argument or context
2. Check for existing Standalone build:
   ```
   build/plugins/[PluginName]/[PluginName]_artefacts/Debug/Standalone/[PluginName].app
   ```
3. If build doesn't exist, build it:
   ```bash
   cmake --build build --target [PluginName]_Standalone
   ```
4. Launch the application:
   ```bash
   open "build/plugins/[PluginName]/[PluginName]_artefacts/Debug/Standalone/[PluginName].app"
   ```
5. Report success

## Success Output

```
✓ [PluginName] UI opened successfully

The plugin window should now be visible on your screen.
- Test all controls for proper layout and behavior
- Check parameter ranges and default values
- Verify graphics and custom components render correctly
```

## Troubleshooting

**If build fails:**
- Verify CMakeLists.txt includes Standalone in FORMATS
- Check for compilation errors
- Try clean rebuild: `cmake --build build --target [Plugin]_Standalone --clean-first`

**If app won't launch:**
- Check Console.app for crash logs
- Verify .app bundle exists at expected path
- Try running from terminal to see error output

**UI appears blank:**
- Check paint() method in PluginEditor.cpp
- Verify component initialization in constructor
- Look for assertion failures

## Use Cases

- Rapid UI iteration during Stage 5 (GUI)
- Visual design verification
- Quick parameter testing without DAW
- Layout debugging
- Component rendering checks

## Notes

- Uses Debug build (faster compilation during development)
- Provides built-in audio I/O for basic testing
- Supports MIDI input for testing MIDI-controlled parameters
- Not a replacement for full DAW testing
- Can keep open and rebuild to see changes
