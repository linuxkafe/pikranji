# Pikranji — Roadmap

## Sprint 1: Documentation & NDS Modularization (Current)
| ID | Title | Impact | Effort | Priority | Status |
|----|-------|--------|--------|----------|--------|
| T001 | Create AES project structure & docs | High | Medium | High | In Progress |
| T002 | Modularize nds/source/main.c into separate modules | High | High | High | Pending |
| T003 | Add C docstrings to all NDS functions | Medium | Medium | High | Pending |
| T004 | Fix missing English meanings in puzzles.h | Medium | Low | High | Pending |
| T005 | Add validation for puzzle data integrity | Medium | Low | Medium | Pending |

## Sprint 2: Quality Gates & Testing
| ID | Title | Impact | Effort | Priority | Status |
|----|-------|--------|--------|----------|--------|
| T006 | Add Makefile targets: check, test, lint | High | Medium | High | Backlog |
| T007 | Create unit tests for puzzle logic (C) | High | High | High | Backlog |
| T008 | Add hostile analysis lint hook | Medium | Low | Medium | Backlog |

## Sprint 3: Feature Enhancements
| ID | Title | Impact | Effort | Priority | Status |
|----|-------|--------|--------|----------|--------|
| T009 | Add puzzle validation on load (check solvability) | Medium | Medium | Medium | Backlog |
| T010 | Improve NDS VRAM usage (separate banks properly) | Medium | Medium | Medium | Backlog |
| T011 | Add difficulty indicator UI | Low | Low | Low | Backlog |

## Backlog (Discovered Mid-Task)
- [DISCOVERED mid-task] Duplicate formatters in serialize/ — separate refactor ticket needed