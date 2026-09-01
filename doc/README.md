# sctime Developer Documentation

This is the developer overview of **sctime**, a Qt/C++ desktop and WebAssembly
application for recording working time ("Zeiterfassung") and on-call/standby time
("Bereitschaft") against a hierarchy of departments, accounts, and sub-accounts.

## Table of Contents

1. [Architecture Overview](01-architecture-overview.md) — big picture, module map, process view
2. [Domain & Data Model](02-domain-data-model.md) — Abteilung/Konto/Unterkonto/Eintrag hierarchy
3. [Data Sources & Backends](03-data-sources-backends.md) — pluggable `Datasource` abstraction (file/SQL/command/REST-JSON), offline data JSON schema
4. [Settings, Persistence & XML](04-settings-persistence.md) — `SCTimeXMLSettings`, `XMLReader`/`XMLWriter`, on-disk layout
5. [Offline Sync & Conflict Resolution](05-offline-sync-conflicts.md) — how local and server state are merged
6. [Punch Clock & On-Call Tracking](06-punchclock-oncall.md) — legal break-time state machine, Bereitschaft model
7. [UI Layer](07-ui-layer.md) — main window, tree view, dialogs, `sctime://` deep links
8. [Build Targets & Packaging](08-build-targets.md) — desktop (Linux/Windows/macOS) and WebAssembly builds, tenants
9. [Testing](09-testing.md) — existing unit tests and how to extend them
10. [Glossary](10-glossary.md) — German domain terms used throughout the code

## Other files in this directory

| File | Purpose |
|---|---|
| [registerlink.reg.example](registerlink.reg.example) | Windows registry example that registers the `sctime:` URL protocol so account deep links open in sctime — see [07-ui-layer.md §5](07-ui-layer.md#5-sctime-deep-links--windows-url-protocol-registration) |
| [sctime.doxygen](sctime.doxygen) | Doxygen config to generate a low-level, comment-based API reference from the source tree (complements this hand-written overview); run `doxygen doc/sctime.doxygen` from the repo root |

## Quick Facts

| | |
|---|---|
| Language | C++ (C++11), Qt 6 (Qt 4/5 compatible in older history) |
| Build system | qmake (`sctime.pro` → `src/src.pro`) |
| UI | Qt Widgets, `.ui` forms compiled via `uic` |
| Persistence | Local XML files per day/account tree + optional SQL/REST backends |
| Networking | `QNetworkAccessManager` + REST/JSON endpoints (`sctimegui/v1/...`) |
| Alternate target | WebAssembly via Emscripten (`CONFIG += wasm`), REST-only, browser-persisted (IndexedDB via `idbfs.js`) |
| Tests | `test/` — Qt Test based, currently covering `PunchClockChecker` |
| License | GPLv3 (see [COPYING](../COPYING)) |

## How to Explore Further

- Read [08-build-targets.md](08-build-targets.md) before attempting a first build.
- Start reading code at `src/sctime.cpp` (entry point) → `src/timemainwindow.h/.cpp`
  (application shell) → `src/abteilungsliste.h` (core domain model).
