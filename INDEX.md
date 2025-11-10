# PLUGIN FREEDOM SYSTEM Documentation Index

**Last Updated:** 2025-11-10
**Purpose:** Complete documentation map and navigation guide for the PLUGIN FREEDOM SYSTEM

---

## Executive Overview

### What is the PLUGIN FREEDOM SYSTEM?

The PLUGIN FREEDOM SYSTEM is an AI-assisted plugin development system that enables conversational creation of professional VST3 and AU audio plugins for macOS. Using Claude Code, musicians and producers can design and build custom plugins through natural dialogue, without writing code.

The system removes traditional barriers to plugin development by handling all technical complexity—C++, DSP algorithms, JUCE framework integration, and build automation. You describe what you want (vintage tape delay, granular synthesizer, custom compressor), and the system implements it through an automated 7-stage workflow.

**Key Innovation:** Progressive disclosure through interactive decision menus at every checkpoint. Features are discovered through use, not documentation. Claude Code's checkpointing system (Esc+Esc) enables risk-free exploration—try any option without fear of breaking your work.

### Version 2.0 Features

🆕 **Dispatcher Pattern** - Each implementation stage runs in fresh context (prevents context accumulation)
🆕 **Contract-Driven Development** - parameter-spec.md + architecture.md enforce zero drift between design and implementation
🆕 **Hybrid Validation** - Fast deterministic hooks (2s) + deep semantic validator (60s)
🆕 **Interactive Decision System** - Numbered option menus at every checkpoint (unlimited options, progressive discovery)
🆕 **WebView UI System** - HTML/CSS interfaces with JUCE 8 integration (rapid prototyping, modern aesthetics)

---

## 🚀 Quick Start Guide

### For New Users

**Start here:** Read in this order:
1. `architecture/00-PHILOSOPHY.md` - Core principle: discovery through play
2. `architecture/01-executive-summary.md` - High-level system goals
3. `architecture/02-core-abstractions.md` - 5 fundamental concepts
4. `procedures/core/interactive-decision-system.md` - How features are discovered
5. `procedures/webview/README.md` - Modern UI design approach

**Then try:** Run `/dream [your plugin idea]` and explore the interactive options

### For Implementers

**Critical reading path:**
1. `architecture/04-component-architecture.md` - How components interact
2. `architecture/05-routing-architecture.md` - Slash commands → skills → subagents
3. `procedures/skills/plugin-workflow.md` - Complete 7-stage implementation
4. `procedures/webview/` - All 15 WebView implementation guides (read README.md first)
5. `architecture/FINAL-AUDIT-REPORT.md` - Verify system is implementation-ready

**Then reference:** `cross-reference-map.md` for architecture ↔ procedure mappings

### For Extenders

**Hook into the system:**
1. `architecture/10-extension-architecture.md` - Adding skills, hooks, MCP servers
2. `architecture/13-error-handling-recovery.md` - Hook validation patterns (lines 427-461)
3. `procedures/agents/troubleshooter.md` - Example specialized agent

---

## Documentation Structure

```
plugin-freedom-system/
├── INDEX.md                          # ← You are here
├── overview.md                       # User-facing system description
├── cross-reference-map.md            # Architecture ↔ procedure mappings (87 mappings)
├── architecture-updates-summary.md   # Cross-reference implementation log
│
├── architecture/                     # WHAT and WHY (18 docs)
│   ├── README.md                     # Architecture navigation index
│   ├── 00-PHILOSOPHY.md              # Progressive disclosure philosophy
│   ├── 01-executive-summary.md       # High-level goals and decisions
│   ├── 02-core-abstractions.md       # 5 fundamental concepts
│   ├── 04-component-architecture.md  # Component relationships
│   ├── 05-routing-architecture.md    # Command routing
│   ├── 06-state-architecture.md      # 4 state types
│   ├── 07-communication-architecture.md # Subagent reports
│   ├── 08-data-flow-diagrams.md      # Visual flows
│   ├── 09-file-system-design.md      # Directory structure
│   ├── 10-extension-architecture.md  # Hooks, skills, MCP
│   ├── 11-build-automation-architecture.md # 7-phase pipeline
│   ├── 12-webview-integration-design.md # WebView UI system
│   ├── 13-error-handling-recovery.md # Error taxonomy & recovery
│   ├── 14-design-decisions.md        # Rationale for choices
│   ├── 15-migration-notes.md         # v1 → v2 changes
│   ├── 16-implementation-roadmap.md  # 6-phase plan (7-9 days)
│   ├── 17-testing-strategy.md        # Testing approach
│   ├── 03-model-selection-extended-thinking-strategy.md # AI model selection
│   └── FINAL-AUDIT-REPORT.md         # ✅ 0 critical issues
│
├── procedures/                       # HOW and WHEN (42 docs)
│   ├── agents/                       # Specialized subagents
│   │   └── troubleshooter.md         # Diagnostic and resolution system
│   ├── commands/                     # Slash command implementations (9)
│   │   ├── continue.md               # Resume from checkpoint
│   │   ├── doc-fix.md                # Update documentation
│   │   ├── dream.md                  # Plugin ideation
│   │   ├── implement.md              # Full 0-6 stage workflow
│   │   ├── improve.md                # Iterative enhancement
│   │   ├── install-plugin.md         # Deploy to system folders
│   │   ├── show-standalone.md        # HTML mockup preview
│   │   ├── test.md                   # Run validation suite
│   │   └── troubleshoot-juce.md      # Fix JUCE errors
│   ├── core/                         # Foundational patterns (2)
│   │   ├── checkpoint-system.md      # Hard/soft/decision checkpoints
│   │   └── interactive-decision-system.md # Numbered option menus
│   ├── examples/                     # Reference implementations (2)
│   │   ├── plugin-ideation.md        # Creative brief example
│   │   └── plugin-improve.md         # Version improvement example
│   ├── scripts/                      # Low-level automation (1)
│   │   └── build-and-install.md      # CMake + system installation
│   ├── skills/                       # Automation capabilities (12)
│   │   ├── build-automation.md       # CMake builds, error detection
│   │   ├── context-resume.md         # Parse handoff files
│   │   ├── deep-research.md          # Multi-agent investigation
│   │   ├── design-sync.md            # Brief ↔ mockup validation
│   │   ├── juce-foundation.md        # JUCE 8 patterns
│   │   ├── plugin-ideation.md        # Creative brief generation
│   │   ├── plugin-improve.md         # Version management
│   │   ├── plugin-lifecycle.md       # 7-stage orchestration
│   │   ├── plugin-testing.md         # Build + pluginval validation
│   │   ├── plugin-workflow.md        # Stage 1-6 implementation
│   │   ├── troubleshooting-docs.md   # Capture resolutions
│   │   └── ui-mockup.md              # HTML/CSS mockup generation
│   └── webview/                      # 🎯 Complete JUCE 8 WebView docs (15)
│       ├── README.md                 # ⚠️ START HERE - Master index
│       ├── 01-overview.md            # Architecture, when to use
│       ├── 02-project-setup.md       # CMake, WebView2, build
│       ├── 03-communication-patterns.md # C++ ↔ JavaScript
│       ├── 04-parameter-binding.md   # ⚠️ Relay pattern
│       ├── 05-audio-visualization.md # Real-time meters, FFT
│       ├── 06-development-workflow.md # Hot reload, debugging
│       ├── 07-distribution.md        # Embedding, installers
│       ├── QUICK-REFERENCE.md        # Cheat sheet
│       ├── best-practices.md         # ⚠️ CRITICAL: Member order
│       ├── common-problems.md        # Troubleshooting
│       └── EXAMPLES-GUIDE.md         # Working code samples
│
└── prompts/                          # Reusable automation workflows (5)
    ├── 001-analyze-architecture-inconsistencies.md
    ├── 002-architecture-final-audit.md
    ├── 003-analyze-architecture-procedure-mapping.md
    ├── 004-implement-architecture-cross-references.md
    └── 005-create-documentation-index.md # ← Created this INDEX.md
```

---

## Architecture Guide (Ordered Reading Path)

### Phase 1: Understanding (Why and What)

**00-PHILOSOPHY.md** 🎯
*Core principle: discovery through play, not documentation*
- Progressive disclosure philosophy
- Risk-free exploration via Claude Code checkpointing
- Interactive decision menus at every workflow checkpoint
- **Related:** `procedures/core/interactive-decision-system.md`, `procedures/core/checkpoint-system.md`

**01-executive-summary.md** 🎯
*High-level architecture goals and key decisions*
- Subagent architecture for fresh context per stage
- Contract-driven development (parameter-spec + architecture)
- Hybrid validation (hooks + validator subagent)
- **Related:** `procedures/commands/implement.md`, `procedures/skills/plugin-workflow.md`

**02-core-abstractions.md** 🎯
*5 fundamental concepts that define the system*
1. Navigation Index (CLAUDE.md) - tells Claude WHERE to find components
2. Skills (Workflows) - high-level coordination, dispatcher pattern
3. Subagents - autonomous implementation in fresh contexts
4. Contracts (Specifications) - single source of truth (parameter-spec.md, architecture.md)
5. Hybrid Validation - fast hooks (2s) + deep validator (60s)
- **Related:** `procedures/skills/plugin-workflow.md`, `procedures/agents/troubleshooter.md`

### Phase 2: System Architecture (How Components Work)

**04-component-architecture.md** 📚
*Component relationships, interfaces, and data flow*
- Skill → Subagent → Validator flow
- CLAUDE.md as navigation index (not orchestrator)
- Hook-based quality gates (6 hooks used)
- **Related:** `procedures/commands/`, `procedures/skills/`, `procedures/agents/`

**05-routing-architecture.md** 📚
*Slash command entry points and decision routing*
- `/dream`, `/implement`, `/improve`, `/continue` commands
- Interactive decision system at checkpoints (inline numbered menus)
- Vagueness detection and option generation
- **Related:** `procedures/core/interactive-decision-system.md`, all `procedures/commands/`

**06-state-architecture.md** 📚
*4 state types: lifecycle, session, version, logs*
- PLUGINS.md registry (💡→🚧→✅→📦 state machine)
- .continue-here.md handoff files for resumption
- CHANGELOG.md version management (Keep a Changelog format)
- **Related:** `procedures/skills/context-resume.md`, `procedures/skills/plugin-improve.md`

**07-communication-architecture.md** 📚
*Inter-component messaging and subagent reports*
- Dispatcher → Subagent: Task tool invocation with complete specs
- Subagent → Dispatcher: JSON report parsing
- Error propagation: always surface with options
- **Related:** All subagent procedures (expected JSON schemas)

**08-data-flow-diagrams.md** 📚
*Visual representation of request → result flows*
- Stage execution sequences
- State transitions
- Error path flows
- **Related:** `procedures/skills/plugin-workflow.md`, `procedures/skills/build-automation.md`

### Phase 3: System Design Details

**09-file-system-design.md** 📚
*Directory structure and file naming conventions*
- .claude/ (skills, commands, agents, MCP)
- plugins/[Name]/.ideas/ (contracts, mockups, specs)
- logs/, backups/ (versioned artifacts)
- **Related:** `procedures/skills/context-resume.md`

**10-extension-architecture.md** 📚
*Hooks, skills, MCP servers, extensibility*
- 6 hooks used: PostToolUse, UserPromptSubmit, Stop, SubagentStop, PreCompact, SessionStart
- Skill YAML frontmatter structure
- MCP server integration patterns
- **Related:** `architecture/13-error-handling-recovery.md:427-461` (hook patterns)

**11-build-automation-architecture.md** 🎯
*7-phase build pipeline and failure protocol*
- build-and-install.sh (validation → build → install → verify)
- 4-option failure protocol (Investigate/Show/Wait/Other)
- Parallel VST3 + AU builds
- **Related:** `procedures/scripts/build-and-install.md`, `procedures/skills/build-automation.md`

**12-webview-integration-design.md** 🎯 ⚠️
*WebView UI system - CRITICAL IMPLEMENTATION REFERENCE*
- HTML → UI architecture (no translation layer)
- Two-phase design (yaml mockup → finalization)
- Parameter binding via relay pattern
- ⚠️ **MUST READ:** `procedures/webview/README.md` - all WebView integration must follow proven patterns
- **Critical files:**
  - `procedures/webview/04-parameter-binding.md` - relay pattern
  - `procedures/webview/best-practices.md` - member declaration order (prevents crashes)
  - `procedures/webview/03-communication-patterns.md` - C++ ↔ JavaScript
  - `procedures/webview/common-problems.md` - troubleshooting

**13-error-handling-recovery.md** 📚
*Error taxonomy and recovery workflows*
- 7 error types (contract, build, runtime, validation, communication, session, data)
- Hook validation patterns (lines 427-461)
- Conditional hook execution (graceful skip pattern)
- **Related:** `procedures/skills/build-automation.md`, `procedures/agents/troubleshooter.md`

### Phase 4: Decision Context (Why Things Are This Way)

**14-design-decisions.md** 📚
*Rationale for 7 major architectural choices*
- Dispatcher pattern vs monolithic skills (fresh context per stage)
- Slash commands vs conversational routing (deterministic, discoverable)
- Interactive menus vs yes/no prompts (unlimited options, progressive disclosure)
- **Related:** `procedures/core/interactive-decision-system.md`

**15-migration-notes.md** 📚
*v1 → v2 changes and backward compatibility*
- What changed: dispatcher pattern, contracts, hybrid validation
- What stayed: JUCE foundation, WebView approach, file structure
- Migration workflow considerations

**03-model-selection-extended-thinking-strategy.md** 📚
*When to use Opus/Sonnet/Haiku and extended thinking*
- dsp-agent uses Opus (complex algorithms)
- validator uses Sonnet (semantic reasoning)
- troubleshooter Level 3-4 uses extended thinking

### Phase 5: Implementation Planning

**16-implementation-roadmap.md** 🚀
*6-phase plan (7-9 days total)*
- Phase 0: Foundation (0.5 days)
- Phase 1: Core sub-agent architecture (1-2 days)
- Phase 2: Workflow orchestration (2 days)
- Phase 3: Build + validation (1.5 days)
- Phase 4: Enhanced features (1-2 days)
- Phase 5: Interactive decision system (1 day)
- **Related:** `procedures/skills/`, `procedures/commands/`, `procedures/core/`

**17-testing-strategy.md** 📚
*3-tier testing pyramid and acceptance criteria*
- System tests for E2E workflows (primary validation)
- Integration tests for component interactions
- Unit tests for critical logic
- **Related:** `procedures/skills/plugin-testing.md`, `procedures/commands/test.md`

### Phase 6: Quality Verification

**FINAL-AUDIT-REPORT.md** ✅
*Complete architecture audit - 0 critical issues*
- All blocking items resolved
- 18/18 components fully specified
- Terminology consistent (all "subagent", no hyphens)
- System is implementation-ready

---

## Procedures Reference (Categorized)

### Agents - Specialized Subagents

**`agents/troubleshooter.md`** 🎯
Diagnostic system that analyzes problems, generates structured reports (symptoms, root causes, solutions), and documents resolutions in `/troubleshooting/` for knowledge accumulation.

### Commands - Slash Command Entry Points

| Command | Purpose | Routes To |
|---------|---------|-----------|
| **`commands/continue.md`** | Resume paused workflows from `.continue-here.md` | context-resume skill |
| **`commands/doc-fix.md`** | Update documentation based on learnings | documentation maintenance |
| **`commands/dream.md`** 🚀 | Creative plugin ideation with AI brainstorming | plugin-ideation skill |
| **`commands/implement.md`** 🎯 | Full 0-6 stage plugin implementation | plugin-workflow skill |
| **`commands/improve.md`** | Iterative enhancement (v1.0 → v1.1) | plugin-improve skill |
| **`commands/install-plugin.md`** | Copy built plugins to system folders | build-automation |
| **`commands/show-standalone.md`** | Generate standalone HTML mockup preview | ui-mockup skill |
| **`commands/test.md`** | Run validation suite (build + pluginval) | plugin-testing skill |
| **`commands/troubleshoot-juce.md`** | Fix JUCE compilation/runtime errors | troubleshooter agent |

### Core - Foundational System Patterns

**`core/interactive-decision-system.md`** 🎯
*Numbered option menus at every checkpoint*
- Progressive feature discovery (users learn by exploring)
- No artificial option limit (dynamic lists: 11 plugins, 8 aesthetics)
- Risk-free exploration via Claude Code checkpointing (Esc+Esc)
- Examples: "What's next?" after stages, "How to start?" for mockups

**`core/checkpoint-system.md`** 🎯
*Three checkpoint types: hard, soft, decision*
- `.continue-here.md` handoff files preserve context
- Enables pause/resume workflows
- Git commits align with checkpoints for rollback safety
- Integrates with interactive decisions for safe experimentation

### Skills - Automation Capabilities

**`skills/build-automation.md`** 🎯
*CMake builds, error detection, retry logic*
- Wraps build-and-install.sh script
- 4-option failure protocol (Investigate/Show/Wait/Other)
- Parallel VST3 + AU builds

**`skills/context-resume.md`**
*Parse `.continue-here.md`, reconstruct state, continue workflows*
- Resumes from any checkpoint (hard, soft, decision)
- Preserves conversation context and next actions

**`skills/deep-research.md`**
*Multi-agent parallel investigation*
- JUCE docs, forums, GitHub issue search
- Graduated research protocol (simple → complex)

**`skills/design-sync.md`**
*Validate mockup ↔ creative brief consistency*
- Ensures UI design matches original vision
- Catches design drift before implementation

**`skills/juce-foundation.md`** 🎯
*JUCE 8 patterns, WebView setup, CMakeLists.txt structure*
- Real-time safety rules
- Modern JUCE API usage
- Stage 2 foundation patterns

**`skills/plugin-ideation.md`** 🚀
*Creative brief generation*
- Adaptive brainstorming (follows user's creative process)
- Generates creative-brief.md (concept, target user, core features, DSP, UI vision)

**`skills/plugin-improve.md`**
*Versioning (v1.0 → v1.1), changelog management, regression testing*
- Creates backups before changes
- Updates CHANGELOG.md (Keep a Changelog format)
- Prevents touching plugins in 🚧 state

**`skills/plugin-lifecycle.md`**
*7-stage workflow orchestration (Stages 0-6)*
- Complexity detection and phasing
- State machine management (💡→🚧→✅→📦)

**`skills/plugin-testing.md`**
*Build validation, pluginval integration, DAW loading tests*
- Stage 6 validation
- Automated stability checks

**`skills/plugin-workflow.md`** 🎯
*Stage 1-6 implementation details*
- Complete workflow specification
- File structure, JUCE conventions
- Subagent dispatch patterns

**`skills/troubleshooting-docs.md`**
*Capture resolutions, organize by plugin/symptom*
- Builds searchable knowledge base
- Documents proven solutions

**`skills/ui-mockup.md`** 🎯
*HTML/CSS mockup generation*
- yaml design definition → test.html
- Browser testing, design iteration
- Two-phase finalization

**`skills/ui-template-library.md`**
*Save/load design templates, reuse across plugins*
- Aesthetic library system
- Design consistency across projects

### Scripts - Low-Level Automation

**`scripts/build-and-install.md`** 🎯
*Complete CMake build + system installation*
- Handles VST3/AU formats
- Build directory setup
- Copying to ~/Library/Audio/Plug-Ins/

### WebView - JUCE 8 WebView Plugin Documentation (15 files)

⚠️ **CRITICAL:** All WebView integration MUST follow these proven patterns. Do not use generic JUCE examples—they may use outdated APIs or incomplete patterns.

**Core Documentation Series:**

**`webview/README.md`** 🎯 ⚠️
*START HERE - Master index, learning paths, problem lookup*
- Complete documentation series overview
- Quick start guide
- Code examples directory

**`webview/01-overview.md`**
*Architecture, concepts, platform backends, when to use WebView*

**`webview/02-project-setup.md`**
*CMake config, Windows WebView2, platform setup, build process*

**`webview/03-communication-patterns.md`** 🎯
*C++ ↔ JavaScript communication*
- Resource provider pattern (serves files from C++ memory)
- Native function registration
- Event-based updates

**`webview/04-parameter-binding.md`** 🎯 ⚠️
*Relay pattern for parameters*
- WebSliderRelay, WebToggleButtonRelay, WebComboBoxRelay
- Initialization order: `AudioParameter ↔ ParameterAttachment ↔ Relay ↔ JavaScript ↔ HTML`

**`webview/05-audio-visualization.md`**
*Real-time meters, FFT, Canvas rendering, performance optimization*

**`webview/06-development-workflow.md`**
*Hot reloading, dev server, debugging, DAW testing*

**`webview/07-distribution.md`**
*Embedding files, zip+binary data, installers, runtime requirements*

**Support Documentation:**

**`webview/QUICK-REFERENCE.md`** 📚
*Cheat sheet: CMake snippets, communication patterns, parameter binding templates*

**`webview/best-practices.md`** 🎯 ⚠️
*CRITICAL PATTERNS - Read before implementing*
- **Member declaration order** (prevents release build crashes):
  ```cpp
  // CORRECT ORDER (destroyed in reverse):
  juce::WebSliderRelay webGainRelay;           // 1. Relays first
  juce::WebBrowserComponent webView;           // 2. WebView second
  juce::WebSliderParameterAttachment attachment; // 3. Attachments last
  ```
- Constructor patterns
- WebView options (`.withKeepPageLoadedWhenBrowserIsHidden()` for FL Studio)

**`webview/common-problems.md`** 🎯
*Troubleshooting guide*
- Blank screens (resource provider, native integration)
- Parameter sync issues
- FL Studio Tab key problem
- Logic Pro right-click crash
- Ableton 12 Escape key crash (unresolved as of Jan 2025)

**`webview/EXAMPLES-GUIDE.md`**
*Working code samples with explanations*

**API Migration Notes:**
- ❌ JUCE 7: Data URLs (`data:text/html,...`)
- ✅ JUCE 8: Resource provider pattern (mandatory)
- ❌ JUCE 7: File URLs (`file:///path`)
- ✅ JUCE 8: `getResourceProviderRoot()`

### Examples - Reference Implementations

**`examples/plugin-ideation.md`**
*Example creative brief: "Vintage Tape Saturation"*
- Shows expected format and level of detail

**`examples/plugin-improve.md`**
*Example improvement workflow: "TapeDelay v1.0 → v1.1"*
- Version management patterns

---

## Cross-Reference Matrix

*For complete architecture ↔ procedure mappings, see `cross-reference-map.md` (87 total mappings)*

### Key Mappings (Most Critical)

| Architecture Concept | Primary Procedure | Why Critical |
|---------------------|------------------|--------------|
| **Progressive disclosure philosophy** | `procedures/core/interactive-decision-system.md` | Defines how users discover features |
| **7-stage workflow** | `procedures/skills/plugin-workflow.md` | Complete implementation specification |
| **WebView integration** | `procedures/webview/README.md` + 14 files | Prevents using outdated patterns |
| **Parameter relay pattern** | `procedures/webview/04-parameter-binding.md` | Core UI → DSP connection |
| **Build pipeline** | `procedures/scripts/build-and-install.md` | Automation script specification |
| **4-option failure protocol** | `procedures/skills/build-automation.md` | Error recovery pattern |
| **Checkpoint system** | `procedures/core/checkpoint-system.md` | Enables pause/resume workflows |
| **Contract prerequisites** | Enforced in `procedures/skills/plugin-workflow.md` Stage 1 | Prevents implementation drift |
| **Troubleshooting protocol** | `procedures/agents/troubleshooter.md` | Multi-level investigation |

### Architecture → Procedures Quick Reference

- **00-PHILOSOPHY.md** → `core/interactive-decision-system.md`, `core/checkpoint-system.md`
- **01-executive-summary.md** → `skills/plugin-workflow.md`, `commands/implement.md`
- **02-core-abstractions.md** → `skills/plugin-workflow.md`, `agents/troubleshooter.md`
- **05-routing-architecture.md** → All `commands/*.md`, `core/interactive-decision-system.md`
- **11-build-automation-architecture.md** → `scripts/build-and-install.md`, `skills/build-automation.md`
- **12-webview-integration-design.md** → All `webview/*.md` (15 files)
- **16-implementation-roadmap.md** → `skills/*.md`, `commands/*.md`, `core/*.md`

---

## Core Concepts Glossary

**Checkpoint**
*Hard, soft, or decision point in workflow*
- **Hard checkpoint:** Stage boundaries (1→2, 2→3, etc.)
- **Soft checkpoint:** Phase boundaries within stages
- **Decision checkpoint:** User chooses next action (interactive menu)
- Enabled by Claude Code's Esc+Esc feature (rollback to any checkpoint)

**Contract**
*Single source of truth specification*
- parameter-spec.md (audio parameters), architecture.md (plugin design), creative-brief.md (vision)
- **Enforced:** Stage 1 Planning blocks if parameter-spec.md or architecture.md missing
- **Terminology:** "specification" = file, "contract" = binding agreement

**Dispatcher Pattern**
*Main skill coordinates, subagents implement*
- Each subagent runs in fresh context (prevents context accumulation)
- plugin-workflow skill dispatches foundation-agent, shell-agent, dsp-agent, gui-agent
- Eliminates v1's "Frankenstein feeling" from multi-session accumulation

**Hybrid Validation**
*Two-layer error detection*
- **Layer 1 (Hooks):** Fast (2s), deterministic pattern matching, blocks on failure
- **Layer 2 (Validator Subagent):** Slow (60s), semantic reasoning, advisory
- Hooks catch simple errors fast, validator catches semantic issues

**Interactive Decision System**
*Numbered option menus at every checkpoint*
- Progressive disclosure: features discovered through exploration, not documentation
- No artificial 4-option limit (supports dynamic lists: 11 plugins, 8 aesthetics)
- Risk-free exploration via checkpointing (Esc+Esc)

**Navigation Index (CLAUDE.md)**
*Tells Claude WHERE to find components (not an orchestrator)*
- Contains: component locations, system topology, key principles (~200 words)
- Does NOT contain: detailed workflows, validation logic, command behavior

**Progressive Disclosure**
*Core philosophy: learn by doing, not reading*
- Discovery mechanisms: slash command autocomplete, contextual option menus, checkpointing
- Result: Users learn features organically without documentation

**Relay Pattern (WebView)**
*Parameter binding for web UIs*
- AudioParameter ↔ ParameterAttachment ↔ Relay ↔ JavaScript State ↔ HTML Element
- Three types: WebSliderRelay, WebToggleButtonRelay, WebComboBoxRelay
- ⚠️ **Critical:** Member declaration order prevents release build crashes

**Resource Provider (WebView)**
*Serves files from C++ memory (like local web server)*
- JUCE 8 requirement (replaces JUCE 7's data URLs)
- `.withResourceProvider([this](const auto& url) { return getResource(url); })`

**Slash Command**
*Deterministic, discoverable entry point*
- `/dream` (ideation), `/implement` (build), `/improve` (enhance), `/continue` (resume)
- Autocomplete discovery (type "/" to see all workflows)
- Commands contain instructions → route to skills

**State Machine (Plugin Lifecycle)**
*PLUGINS.md registry tracks status*
- 💡 Ideated → 🚧 Stage N → ✅ Working → 📦 Installed
- **Critical rule:** If status is 🚧, ONLY plugin-workflow can modify (not plugin-improve)

**Subagent**
*Autonomous implementation in fresh context*
- Spawned via Task tool with complete specifications
- Returns structured JSON reports
- Types: foundation-agent, shell-agent, dsp-agent, gui-agent, validator, troubleshooter

**Two-Phase UI Design**
*Mockup → finalization workflow*
- Phase 1: yaml design definition → test.html mockup (iterate in browser)
- Phase 2: test.html → 7 finalized files (index.html, ui.css, ui.js, relay setup, attachments)

**WebView Integration**
*HTML/CSS interfaces instead of JUCE GUI*
- Modern aesthetics, rapid prototyping, hot reload during development
- JUCE 8 patterns (resource provider, relay pattern, native integration)
- ⚠️ **Must follow:** `procedures/webview/` proven patterns (outdated examples exist online)

---

## Implementation Roadmap

*For detailed plan, see `architecture/16-implementation-roadmap.md`*

### 6-Phase Plan (7-9 days total)

**Phase 0: Foundation (0.5 days)**
- Setup: JUCE 8 library, Claude Code environment, directory structure
- Verify: CMake builds, pluginval runs, basic JUCE project compiles

**Phase 1: Core Sub-Agent Architecture (1-2 days)** 🎯
- Create: foundation-agent, shell-agent, dsp-agent, gui-agent specifications
- Implement: JSON report schemas, Task tool invocation patterns
- Test: Each agent completes its scope independently
- **Critical:** Fresh context per agent (dispatcher pattern core)

**Phase 2: Workflow Orchestration (2 days)** 🎯
- Implement: plugin-workflow skill (Stages 1-6)
- Create: PLUGINS.md state machine, .continue-here.md handoff files
- Test: Complete flow from ideation → working plugin
- **Critical:** Contract enforcement (parameter-spec.md + architecture.md)

**Phase 3: Build + Validation (1.5 days)** 🎯
- Implement: build-and-install.sh script, build-automation skill
- Create: 6 hooks (PostToolUse, UserPromptSubmit, Stop, SubagentStop, PreCompact, SessionStart)
- Test: Build failures trigger 4-option protocol, hooks catch quality issues
- **Critical:** Hybrid validation (fast hooks + deep validator)

**Phase 4: Enhanced Features (1-2 days)**
- Implement: plugin-improve skill, context-resume skill, troubleshooter agent
- Create: CHANGELOG.md management, backup system, troubleshooting knowledge base
- Test: Version improvements (v1.0 → v1.1), pause/resume workflows

**Phase 5: Interactive Decision System (1 day)** 🎯
- Implement: Numbered option menus at all checkpoints
- Create: Option generation logic, vagueness detection
- Test: Progressive feature discovery, risk-free exploration
- **Critical:** Foundation for "no documentation needed" goal

**Phase 6: Polish + Documentation (during all phases)**
- Document: Troubleshooting resolutions, hook patterns, WebView gotchas
- Refine: Error messages, option descriptions, checkpoint handoff format

### Success Criteria

- ✅ User can `/implement [plugin name]` → installed plugin in 40-80 minutes (human time: 5-15 min)
- ✅ All stages run in fresh contexts (no accumulation)
- ✅ Contracts enforce zero drift (parameter-spec.md → implementation)
- ✅ Build failures present 4 clear options (no dead ends)
- ✅ Features discoverable through interactive menus (no manual needed)
- ✅ WebView plugins follow proven patterns (member order, resource provider, relay)

---

## Navigation Hints

### Finding What You Need

**"I want to understand the philosophy"**
→ Start: `architecture/00-PHILOSOPHY.md`
→ Then: `architecture/14-design-decisions.md` (rationale)

**"I need to implement Stage 3 (DSP)"**
→ Architecture: `architecture/04-component-architecture.md` (dsp-agent interface)
→ Procedure: `procedures/skills/plugin-workflow.md` (Stage 3 specification)
→ Reference: `procedures/skills/juce-foundation.md` (JUCE patterns)

**"WebView integration isn't working"**
→ Start: `procedures/webview/README.md` (problem lookup table)
→ Troubleshoot: `procedures/webview/common-problems.md` (blank screens, parameter sync)
→ Verify: `procedures/webview/best-practices.md` (member declaration order)

**"How do interactive menus work?"**
→ Philosophy: `architecture/00-PHILOSOPHY.md` (progressive disclosure)
→ Architecture: `architecture/05-routing-architecture.md` (inline numbered lists)
→ Procedure: `procedures/core/interactive-decision-system.md` (implementation patterns)

**"Build is failing, what do I do?"**
→ Architecture: `architecture/11-build-automation-architecture.md` (4-option protocol)
→ Procedure: `procedures/skills/build-automation.md` (failure handling)
→ Agent: `procedures/agents/troubleshooter.md` (diagnostic system)

**"I need to add a custom skill"**
→ Architecture: `architecture/10-extension-architecture.md` (skill YAML frontmatter)
→ Examples: All `procedures/skills/*.md` (demonstrate frontmatter structure)

**"What's the difference between a skill and a subagent?"**
→ Concepts: `architecture/02-core-abstractions.md` (Skills = coordination, Subagents = implementation)
→ Details: `architecture/04-component-architecture.md` (component relationships)

**"Where are the hook implementation examples?"**
→ Architecture: `architecture/13-error-handling-recovery.md` lines 427-461 (patterns)
→ Example: UserPromptSubmit, SubagentStop hooks with complete code

### Reading Order Recommendations

**First-time learner** (understanding the system):
1. `architecture/00-PHILOSOPHY.md`
2. `architecture/01-executive-summary.md`
3. `architecture/02-core-abstractions.md`
4. `procedures/core/interactive-decision-system.md`
5. Try: `/dream [plugin idea]` and explore interactively

**Implementer** (building the system):
1. `architecture/FINAL-AUDIT-REPORT.md` (verify readiness)
2. `architecture/16-implementation-roadmap.md` (6-phase plan)
3. `architecture/04-component-architecture.md` (component interfaces)
4. `procedures/skills/plugin-workflow.md` (core workflow)
5. `procedures/webview/README.md` (UI system)
6. `cross-reference-map.md` (find specific mappings)

**Extender** (adding features):
1. `architecture/10-extension-architecture.md` (extensibility patterns)
2. `architecture/13-error-handling-recovery.md` (hook patterns)
3. Example: `procedures/agents/troubleshooter.md` (specialized agent)
4. Example: All `procedures/skills/*.md` (skill frontmatter)

**Troubleshooter** (fixing issues):
1. `procedures/webview/common-problems.md` (WebView issues)
2. `procedures/agents/troubleshooter.md` (diagnostic protocol)
3. `architecture/13-error-handling-recovery.md` (error taxonomy)
4. `/troubleshooting/` directory (historical resolutions)

---

## Critical Warnings

### WebView Implementation ⚠️

**MUST READ before implementing WebView UI:**
- `procedures/webview/README.md` - Master index
- `procedures/webview/best-practices.md` - Member declaration order (prevents 90% of crashes)
- `procedures/webview/04-parameter-binding.md` - Relay pattern (only correct way)
- `procedures/webview/common-problems.md` - Troubleshooting guide

**Why critical:** Generic JUCE WebView examples online often use:
- ❌ Outdated JUCE 7 data URL patterns (won't work in JUCE 8)
- ❌ Wrong member declaration order (causes release build crashes)
- ❌ Incomplete parameter binding (causes sync issues)

**Only use patterns from `procedures/webview/`** - they're proven and up-to-date.

### Contract Enforcement ⚠️

**Stage 1 Planning BLOCKS if missing:**
- parameter-spec.md (audio parameters definition)
- architecture.md (plugin design specification)

**Why critical:** Prevents "code first, design later" anti-pattern that causes drift.

See: `architecture/02-core-abstractions.md` (Contract abstraction)

### State Machine Rules ⚠️

**If PLUGINS.md shows status 🚧 (in development):**
- ONLY plugin-workflow can modify the plugin
- plugin-improve MUST NOT touch it (prevents concurrent modification)

**Why critical:** Prevents corruption from multiple workflows modifying same plugin.

See: `architecture/06-state-architecture.md` (Lifecycle state machine)

### Hook Validation Pattern ⚠️

**Every hook MUST check if relevant before executing:**

```bash
# Example pattern:
if [[ ! relevant_condition ]]; then
  echo "Hook not relevant, skipping gracefully"
  exit 0  # Success, not failure
fi
```

**Why critical:** Prevents hooks from blocking normal operations when not applicable.

See: `architecture/13-error-handling-recovery.md` lines 427-461

### Reading Order Dependencies ⚠️

**Prerequisites for specific documents:**

Before reading `architecture/04-component-architecture.md`:
→ Must understand: `architecture/02-core-abstractions.md` (5 concepts)

Before implementing WebView:
→ Must read: `procedures/webview/README.md` + `best-practices.md`

Before adding hooks:
→ Must understand: `architecture/13-error-handling-recovery.md` (graceful skip pattern)

Before creating subagents:
→ Must understand: `architecture/02-core-abstractions.md` (Subagent abstraction)
→ Must read: `architecture/07-communication-architecture.md` (JSON report format)

---

## Prompts Directory

*Reusable automation workflows for documentation quality and integration*

### Documentation Quality Pipeline

**Sequential workflow** (run in order):

**001-analyze-architecture-inconsistencies.md**
*Initial quality check*
- Finds: contradictions, terminology issues, interface mismatches
- Output: `./analyses/architecture-inconsistencies.md`
- **When:** After major architecture changes

**002-architecture-final-audit.md**
*Comprehensive verification*
- Verifies: all previous issues resolved, Claude Code alignment
- Output: `./architecture/FINAL-AUDIT-REPORT.md`
- **When:** After implementing fixes from 001

**003-analyze-architecture-procedure-mapping.md**
*Bridge architecture and procedures*
- Maps: architecture concepts → procedure files
- Identifies: gaps (architecture without procedures, procedures without architecture)
- Output: `./cross-reference-map.md` (87 mappings)
- **When:** After architecture and procedures exist but are disconnected

**004-implement-architecture-cross-references.md**
*Update architecture with procedure links*
- Uses: mapping from 003 to add inline cross-references
- Adds: "Related Procedures" sections to architecture docs
- Output: Modified architecture files + `./architecture-updates-summary.md`
- **When:** After completing mapping analysis (003)

**005-create-documentation-index.md**
*Create master navigation guide*
- Builds: INDEX.md at project root (this file)
- Uses: Task tool with Explore subagent to analyze structure
- Maps: all architecture, procedures, prompts
- Output: `./INDEX.md`
- **When:** After cross-references implemented (creates final documentation map)

### When to Run the Full Pipeline

Run 001 → 002 → 003 → 004 → 005 when:
- Initial architecture documentation is complete
- Procedures created but not linked to architecture
- Before starting implementation (ensures alignment)
- After major architectural changes (revalidate)

---

## Verification Checklist

Use this to verify INDEX.md completeness:

- ✅ Every architecture document listed with purpose
- ✅ Every procedure file categorized and described
- ✅ Cross-references between layers clearly mapped
- ✅ Critical implementation warnings highlighted (WebView, contracts, state machine, hooks)
- ✅ Reading order logical and clear (first-time, implementer, extender, troubleshooter)
- ✅ Someone unfamiliar with the PLUGIN FREEDOM SYSTEM can understand system structure from this file
- ✅ All file paths correct and use relative links
- ✅ 87 architecture ↔ procedure mappings referenced (in cross-reference-map.md)
- ✅ 18 architecture documents covered
- ✅ 42 procedure files covered
- ✅ 5 prompt workflows covered
- ✅ Core concepts glossary includes: checkpoints, contracts, dispatcher, validation, menus, relays, WebView, subagents
- ✅ Critical WebView documentation prominently noted (15 files, MUST READ warnings)
- ✅ Implementation roadmap summarized (6 phases, 7-9 days)
- ✅ Navigation hints cover common questions
- ✅ Visual markers used (🎯 critical, 📚 reference, 🚀 quick start, ⚠️ warnings, 🆕 v2.0 features, ✅ verification)

---

## Meta

**Document Status:** ✅ Complete
**Coverage:** 18 architecture docs, 42 procedure files, 5 prompt workflows
**Total Mappings:** 87 architecture ↔ procedure relationships (see cross-reference-map.md)
**Critical Issues:** 0 (verified by FINAL-AUDIT-REPORT.md)
**Implementation Status:** Ready for Phase 0-6 (see architecture/16-implementation-roadmap.md)

**Last Updated:** 2025-11-10
**Generated By:** prompts/005-create-documentation-index.md
**Maintained By:** Update after major architecture or procedure changes

---

**Need help?** Start with the Quick Start Guide above, or jump to the specific section matching your role (new user, implementer, extender, troubleshooter).
