<objective>
Create a comprehensive INDEX.md file that serves as a complete map and reference guide for the PLUGIN FREEDOM SYSTEM plugin development documentation system. This index will be the primary entry point for understanding what documentation exists, where it's located, why it's structured this way, and how to navigate it when creating implementation plans.

The end goal is to enable future Claude Code sessions to quickly understand the entire documentation structure and purpose without having to explore randomly. This is critical for the upcoming implementation phase where we'll need to reference specific procedures and architecture decisions.
</objective>

<context>
This is the documentation foundation for PLUGIN FREEDOM SYSTEM - a JUCE-based plugin development system that uses WebView for UIs and follows a progressive disclosure philosophy. The documentation has two layers:
- architecture/ - High-level design decisions (WHAT and WHY)
- procedures/ - Detailed implementation guides (HOW and WHEN)
- prompts/ - Reusable prompts for common tasks
- Cross-references have been added linking architecture to procedures

The INDEX.md will be used by Claude Code when creating implementation plans to understand:
1. Where to find specific implementation details
2. The relationship between different components
3. The philosophy and principles guiding the system
4. The progression from ideation to installed plugin
</context>

<research>
Before creating the INDEX.md, thoroughly explore and understand the documentation structure:

1. Use the Task tool with subagent_type=Explore to deeply analyze the architecture/ directory:
   - What design decisions are documented?
   - How do the documents relate to each other?
   - What is the logical reading order?
   - What are the key concepts and principles?

2. Use the Task tool with subagent_type=Explore to thoroughly examine the procedures/ directory:
   - What implementation patterns are specified?
   - How are procedures organized (skills/, commands/, core/, agents/, webview/)?
   - Which procedures map to which architecture concepts?
   - What are the critical implementation details?

3. Use the Task tool with subagent_type=Explore to understand the prompts/ directory:
   - What reusable workflows are captured?
   - How do prompts relate to the documentation?

4. Examine key relationship files:
   @cross-reference-map.md
   @architecture-updates-summary.md
   @overview.md (if it exists)

For maximum efficiency, run the explore tasks in parallel to gather comprehensive understanding quickly.
</research>

<requirements>
The INDEX.md must include:

1. **Executive Overview** - What is PLUGIN FREEDOM SYSTEM in 2-3 paragraphs
2. **Quick Start Guide** - How to begin using the documentation
3. **Documentation Structure** - Visual hierarchy showing all directories and their purposes
4. **Architecture Guide** - Ordered reading path through architecture documents with brief descriptions
5. **Procedures Reference** - Categorized listing of all procedures with one-line descriptions
6. **Cross-Reference Matrix** - Key mappings between architecture and procedures
7. **Core Concepts Glossary** - Brief definitions of key terms (WebView, aesthetic system, progressive disclosure, etc.)
8. **Implementation Roadmap** - Suggested order for implementing the system
9. **Navigation Hints** - How to efficiently find what you need

Be explicit about:
- The reading order for someone new to the system
- Which documents are prerequisites for others
- The critical WebView documentation that must be followed exactly
- The v2.0 features (aesthetic system, interactive menus, checkpointing)
</requirements>

<implementation>
Structure the INDEX.md to be:
- Scannable with clear headings and sections
- Progressively detailed (overview → specifics)
- Cross-linked with relative paths to all documents
- Annotated with brief descriptions of each document's purpose
- Organized to support both linear reading and quick reference

Include visual markers:
- 🎯 for critical/must-read documents
- 📚 for reference material
- 🚀 for quick start items
- ⚠️ for important warnings (especially WebView patterns)
- 🆕 for v2.0 specific features
</implementation>

<output>
Create the INDEX.md file at the project root:
- `./INDEX.md` - Comprehensive documentation map and navigation guide

The file should be self-contained and not require reading other documents to understand the structure. It should serve as both an introduction for newcomers and a quick reference for implementation.
</output>

<verification>
Before declaring complete, verify:
1. Every architecture document is listed with its purpose
2. Every procedure file is categorized and described
3. Cross-references between layers are clearly mapped
4. Critical implementation warnings are highlighted (especially WebView paths)
5. Reading order is logical and clear
6. Someone unfamiliar with PLUGIN FREEDOM SYSTEM could understand the system structure from this file alone
7. All file paths are correct and use relative links
</verification>

<success_criteria>
The INDEX.md successfully serves as a documentation map when:
- A new Claude Code session can understand the entire system structure from this file
- Implementation planning can quickly locate relevant procedures for each architecture concept
- The relationship between architecture and procedures is immediately clear
- Critical implementation details (WebView, aesthetics, menus) are prominently noted
- Navigation is intuitive for both linear study and quick reference
</success_criteria>