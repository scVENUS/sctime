# Testing

## 1. Current coverage

The only automated test suite today lives in [test/](../test/), built as a
standalone qmake app (`test/test.pro`, `QT += testlib`, `TEMPLATE = app`,
`TARGET = test`) using the [Qt Test](https://doc.qt.io/qt-6/qtest-overview.html)
framework (`QtTest/QtTest`, `QCOMPARE`, `private slots:` as test cases).

It covers exactly one unit: `PunchClockStateDE23::check()` — the break-time
legality checker described in
[06-punchclock-oncall.md](06-punchclock-oncall.md) —
via `TestPunchClockChecker` in
[test/punchclockchecker_test.cpp](../test/punchclockchecker_test.cpp) /
[.h](../test/punchclockchecker_test.h). Test names double as a readable spec
of the legal break-time rules being enforced:

- `testNormalDay`, `testNormalDayOverlappingIntervals`
- `testMissingLunchBreak`, `testShortLunchBreak`, `testVeryShortLunchBreak`
- `testLongDayWithVeryShortBreak`, `testLongDayWithMediumBreak`, `testLongDayWithBreaks`
- `testOverNightWorkWithoutbreak`, `testOverNightWorkShortBreakBeforeMidnight`,
  `testOverNightWorkMediumBreakOverMidnight`
- `testLongOverNightWorkMediumBreakOverMidnight`, `testLongOverNightShortBreakBeforeMidnight`,
  `testLongOverNightEnoughBreaks`
- `testComplexWorkdaysOK`, `testComplexWorkdaysNotOK`, `testComplexWorkdaysNotOK2`
- `testEarlyMorningLongSegmentUnordered`, `testLongSegmentOverMidnight`
- `testSerialization` — round-trips `serialize()`/`deserialize()`

Helper functions `toSecs("H:m")` and `entry(begin, end)` convert human-readable
clock times into the second-of-day integers `PunchClockEntry` stores,
keeping test cases readable (e.g. `entry("8:43","10:07")`).

`TestPunchClockChecker` is declared `friend class` inside `PunchClockStateDE23`
([punchclockchecker.h](../src/punchclockchecker.h)) specifically so tests can
assert on private counters like `workTimeThisWorkday` — a deliberate,
narrowly-scoped whitebox-testing seam.

## 2. Building and running the tests

```bash
cd test
qmake -r test.pro
make
./test              # runs all QtTest cases, prints pass/fail per slot
```

Notes:

- `test.pro` sets `INCLUDEPATH += . ../src` and compiles
  `../src/punchclockchecker.cpp` directly (no linking against the main
  `sctime` binary/library) — it is a from-scratch minimal build of just the
  unit under test plus its direct dependency (`punchclock.h`).
- `DEFINES += PUNCHCLOCKDE23` selects the DE23 ruleset, matching what's
  usually configured via `tenant.pri` in the main app build (see
  [08-build-targets.md](08-build-targets.md)).
- `CONFIG += debug` — the test binary is always built in debug mode.

## 3. Extending test coverage

There is currently no test coverage for the domain model
(`AbteilungsListe` and friends), the data sources, XML
(de)serialization, or the offline sync/conflict-resolution logic — these are
the areas most likely to regress silently and would benefit most from new
`QtTest`-based unit tests, following the same pattern as
`punchclockchecker_test`:

1. Add a new `.pro` (or extend `test.pro`) that includes only the header(s)
   and source(s) under test plus their direct dependencies (avoid pulling in
   all of `TimeMainWindow`, which needs the full application context).
2. Prefer classes with few Qt-widget dependencies as the first candidates —
   e.g. `AbteilungsListe`'s pure map operations, or `XMLReader`/`XMLWriter`
   document (de)serialization given an in-memory `QDomDocument`, are more
   testable in isolation than anything touching `QNetworkAccessManager` or
   the live UI.

Continue with the [Glossary](10-glossary.md).
