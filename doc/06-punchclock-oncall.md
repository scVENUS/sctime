# Punch Clock & On-Call Tracking

sctime tracks two things beyond plain project-time booking:

1. **Punch clock ("Stempeluhr") compliance** — checking recorded work intervals
   against legally required break times (Germany: `PunchClockStateDE23`,
   named after the applicable regulation "§ 23").
2. **On-call / standby duty ("Bereitschaft")** — a separate category of time
   with its own list/model, orthogonal to the department/account tree.

## 1. Punch clock data model

```mermaid
classDiagram
    class PunchClockEntry {
        <<pair of int>>
        +int begin
        +int end
    }
    class PunchClockList {
        <<list of PunchClockEntry>>
        +setCurrentEntry(iterator)
        +currentEntry() iterator
        +findEntryWithEnding(rangeStart, rangeEnd) iterator
    }
    class WorkEvent {
        +int time
        +bool isBegin
    }
    class PunchClockStateBase {
        <<abstract>>
        +PUNCHWARN warnId
        +QString currentWarning
        +QDate date
        +serialize() QString
        +deserialize(QString)
        +check(pcl, currentTime, yesterdayState)
        +getConsolidatedIntervalString(pcl) QString
        +reset()
    }
    class PunchClockStateNoop {
        +always no-op, no rule enforcement
    }
    class PunchClockStateDE23 {
        -int workEnd
        -int breakTimeThisWorkday
        -int lastLegalBreakEnd
        -int workTimeThisWorkday
        +check(pcl, currentTime, yesterdayState)
        +getBreaktimeThisWorkday() int
    }

    PunchClockList "1" o-- "many" PunchClockEntry
    PunchClockStateBase <|-- PunchClockStateNoop
    PunchClockStateBase <|-- PunchClockStateDE23
    PunchClockStateDE23 ..> WorkEvent : builds WorkEventList from PunchClockList
```

`newPunchClockState()` (a factory function in
[punchclockchecker.h](../src/punchclockchecker.h)) returns either the no-op or
the DE23 implementation depending on the `DISABLE_PUNCHCLOCK` compile define —
sites/tenants that don't need break-time enforcement can compile it out
entirely while keeping the same interface.

## 2. How `check()` works (state machine, simplified)

```mermaid
flowchart TD
    A["check(pcl, currentTime, yesterdayState)"] --> B["Build WorkEventList:\none begin+end WorkEvent per PunchClockEntry\n(clip entries that would wrap past midnight)"]
    B --> C["Sort events by time\n(ends before begins at same timestamp)"]
    C --> D{"Gap since last work interval\n>= 11 hours?"}
    D -->|yes| E["Start a fresh workday:\nreset worktime/breaktime counters"]
    D -->|no| F["Continue accumulating\nfrom yesterday's carried-over state"]
    E --> G["Walk events, tracking overlap level,\naccumulate worked time and legal break time"]
    F --> G
    G --> H{"Breaks/limits violated?\n(no break after 6h, break too short\nafter 6h/9h, shift over 10h)"}
    H -->|yes| I["Set warnId (PW_NO_BREAK_6H, PW_TOO_SHORT_BREAK_6H,\nPW_TOO_SHORT_BREAK_9H, PW_OVER_10H)\n+ human-readable currentWarning"]
    H -->|no| J["warnId = PW_NONE"]
```

The `PUNCHWARN` enum values map directly to German labor-law break rules:
after 6 hours worked, a break is required; that break must reach a minimum
length; after 9 hours, a longer break is required; and total work must not
exceed roughly 10 hours in a shift. `PunchClockStateBase::copyFrom()` /
`getDate()`/`setDate()` let the daily state be threaded across day boundaries
(`yesterdayState` parameter) so the 11-hour-rest-before-new-workday rule can
span midnight correctly.

State round-trips to disk via `serialize()`/`deserialize()` as part of the
per-day XML document written by `XMLWriter` (see
[04-settings-persistence.md](04-settings-persistence.md)) — this is why
`PunchClockStateBase` exposes plain string (de)serialization rather than
relying on Qt's XML DOM directly.

`TestPunchClockChecker` (see [09-testing.md](09-testing.md)) is declared a
`friend class` of `PunchClockStateDE23` specifically to unit test its private
counters.

## 3. On-call / "Bereitschaft" tracking

On-call time is a parallel, simpler system:

- `BereitschaftsListe` / `BereitschaftsDatenInfo` — analogous to
  `AbteilungsListe`/`KontoDatenInfo`, but for on-call category entries instead
  of the department/account tree.
- `BereitschaftsModel` ([src/bereitschaftsmodel.h](../src/bereitschaftsmodel.h)) —
  a `QAbstractTableModel` singleton (`getInstance()`) exposing the current
  on-call categories and which are selected, backing the on-call UI.
- `BereitschaftsView` / `OnCallDialog` — the UI surface for selecting which
  on-call categories apply on a given day.
- Data source: like accounts, on-call categories can come from SQL, the
  legacy `zeitbereitls` command, or the JSON/REST source
  (`bereitDSM` in [setupdsm.h](../src/setupdsm.h) — see
  [03-data-sources-backends.md](03-data-sources-backends.md)).

Continue with [UI Layer](07-ui-layer.md).
