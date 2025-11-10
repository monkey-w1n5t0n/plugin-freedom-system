---
name: troubleshoot-juce
description: DEPRECATED - Use deep-research skill instead
---

# /troubleshoot-juce

**STATUS:** DEPRECATED

This command has been replaced by the `deep-research` skill (Phase 7).

## Migration

**Old way:**
```bash
/troubleshoot-juce [error description]
```

**New way:**
- Say "investigate this error" (natural language detection)
- Choose "Option 1: Investigate" in build failure protocols
- Claude automatically invokes deep-research skill when appropriate

## Why This Changed

Skills provide better integration:
- Automatic activation when problems detected
- No need to remember slash command
- Contextual awareness of current work
- Integrated into build-automation failure protocol
- Seamless handoff to troubleshooting-docs skill

## What It Used To Do

Performed graduated depth research:
1. Search local troubleshooting docs (Level 1: Fast Path)
2. Search Context7 for official JUCE documentation (Level 2: Authoritative)
3. Web research with source evaluation (Level 3: Comprehensive)
4. Cross-reference multiple sources
5. Report findings with confidence level

**This protocol still exists - now automated in deep-research skill.**

## Related

- `deep-research` skill - Replacement (automatic invocation)
- `troubleshooting-docs` skill - Creates docs that deep-research searches
- `/doc-fix` command - Document solutions after fixing

## Note

This file is kept for backwards compatibility but will be removed in future versions. Use natural language ("investigate this") or build failure protocol instead.
