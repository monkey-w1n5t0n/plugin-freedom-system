---
name: doc-fix
description: Document a recently solved problem for the knowledge base
---

# /doc-fix

When user runs `/doc-fix [optional context]`, invoke the troubleshooting-docs skill (Phase 7).

## Preconditions

- A problem must have been recently solved in the conversation
- The solution must be non-trivial (not a simple typo)
- Enough context exists to document problem and solution

If no recent problem is found in conversation, ask user to describe what they want to document.

## Behavior

1. Extract from recent conversation:
   - **Problem:** Error messages, symptoms, unexpected behavior
   - **Context:** Plugin name, JUCE version, component, stage
   - **Failed Attempts:** What didn't work and why
   - **Solution:** What actually fixed it (code/config changes)
   - **Root Cause:** Technical explanation
   - **Prevention:** How to avoid in future

2. Create structured documentation:
   - Location: `docs/troubleshooting/[descriptive-name].md`
   - YAML frontmatter: problem_type, component, symptoms, root_cause, severity, tags
   - Sections: Problem, Environment, Failed Attempts, Solution, Explanation, Prevention

3. Validate YAML syntax
4. Cross-reference with existing troubleshooting docs
5. Stage documentation with git
6. Offer to commit with the fix

## Success Output

```
✓ Documentation created

File: docs/troubleshooting/[descriptive-name].md
Location: Staged with fix commit

This solution will now be found by deep-research in future sessions.
```

## When To Use

**Use when:**
- You just confirmed a fix works
- The problem was non-trivial
- Future sessions would benefit from knowing the solution
- You want to preserve institutional knowledge

**Auto-triggers after:**
- "that worked", "it's fixed", "working now" (for non-trivial bugs)
- Multiple attempts needed to solve
- Complex debugging completed

## Why This Matters

**Builds knowledge base:**
- Future sessions find solutions faster
- deep-research searches local docs first (Level 1: Fast Path)
- No solving same problem repeatedly
- Institutional knowledge compounds over time

**The feedback loop:**
1. Hit problem → deep-research searches local docs
2. Fix problem → troubleshooting-docs creates doc
3. Hit similar problem → Found instantly in local docs
4. Knowledge grows with each problem solved

## Routes To

`troubleshooting-docs` skill (Phase 7)

## Note

This skill will be implemented in Phase 7. For now, this command exists as a placeholder to enable discovery via autocomplete.
