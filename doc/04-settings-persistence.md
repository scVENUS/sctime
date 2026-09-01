# Settings, Persistence & XML

## 1. On-disk layout

All local state lives under the config directory (`configDir`, default
`~/.sctime`, overridable with `--configdir=DIR`):

| File | Written by | Contents |
|---|---|---|
| `settings.xml` (+ `.bak` backup) | `XMLWriter::writeSettings(global=true)` | Global settings: window geometry, backends, comment defaults, punch-clock/on-call config, night-mode, columns, feature toggles |
| `zeit-YYYY-MM-DD.xml` | `XMLWriter::writeSettings(global=false)` | One file per day: that day's `AbteilungsListe` (bookings) + punch clock state |
| `zeit-YYYY-MM-DD.sh` | `SCTimeXMLSettings` (via `configDir.filePath(...)`) | Legacy shell-script-format export of a day's bookings, also used by *Download SH files* |
| `lastsync.txt` | `SyncOfflineHelper::setLastSyncTime()` | Timestamp of last successful REST sync |
| `machineid.txt` | `sctime.cpp` | Cached machine identifier used as REST client ID |
| `defaultcomments.xml` | (read-only asset) | Default per-account comment texts, read via `DefaultCommentReader` |

`settings.xml` and `zeit-*.xml` are the two "documents" the rest of the system
calls **global** vs. **non-global** (per-day) settings — the same `XMLReader`
and `XMLWriter` handle both, parameterized by a `global` bool.

## 2. `SCTimeXMLSettings` — the settings god object

`SCTimeXMLSettings` ([src/sctimexmlsettings.h](../src/sctimexmlsettings.h)) is a
single `QObject` holding **every** user-configurable value as a plain data
member with matching getters/setters — window geometry, data-source backend
list, time increments, comment display mode, punch-clock warnings on/off,
custom fonts, REST offline state, and more. It declares `XMLReader`/`XMLWriter`
as `friend class` so they can read/write its private fields directly during
(de)serialization, avoiding a large public setter surface just for I/O.

Defaults are set directly in the constructor (see the snippet in
[01-architecture-overview.md](01-architecture-overview.md) style — e.g.
`defaultbackends = "QPSQL QODBC command json file"`, `timeInc = 5*60` seconds).

## 3. Read/write flow

```mermaid
sequenceDiagram
    participant TMW as TimeMainWindow
    participant XR as XMLReader
    participant Local as Local file (settings.xml / zeit-DATE.xml)
    participant Net as REST (sctimegui/v1/settingsdata)

    TMW->>XR: open() [global? forceLocalRead? autoContinueOnConflict?]
    alt forceLocalRead or REST offline
        XR->>Local: openFile() + parse()
    else online
        XR->>Net: openREST() GET settingsdata
        Net-->>XR: response body
        XR->>XR: parse(QIODevice*)
    end
    XR->>XR: fillSettingsFromDocument(doc, settings)
    alt remote copy newer / conflicting local edits
        XR-->>TMW: conflictedWithLocal(date, global, localDoc, remoteDoc)
        TMW->>TMW: show ConflictDialog (see 05-offline-sync-conflicts.md)
    else another client is editing the same document
        XR-->>TMW: conflictingClientRunning(date, global, remoteDoc)
    else
        XR-->>TMW: settingsPartRead(...) / settingsRead()
    end
```

```mermaid
sequenceDiagram
    participant TMW as TimeMainWindow
    participant XW as XMLWriter
    participant Local as Local file
    participant Net as REST (sctimegui/v1/settingsdata)

    TMW->>XW: writeSettings(global) / writeAllSettings()
    XW->>XW: settings2Doc(global) -> QDomDocument
    alt offline mode
        XW->>Local: write XML to settings.xml or zeit-DATE.xml
    else online
        XW->>Net: PUT/POST serialized document
        Net-->>XW: checkReply()
        alt server reports newer remote version
            XW-->>TMW: conflicted(date, global, othersettings)
        else
            XW-->>TMW: settingsWritten()
        end
    end
```

## 4. Backups & safety

- `backupSettingsXml` (default `true`) makes `XMLWriter` keep `settings.xml.bak`
  before overwriting `settings.xml`, so a corrupt write can be recovered from
  manually.
- `Delete configuration data` ([deletesettingsdialog.cpp](../src/deletesettingsdialog.cpp))
  is the user-facing "nuke everything" escape hatch — it globs `zeit-*.xml` and
  `zeit-*.sh` alongside `settings.xml`/`settings.xml.bak` for deletion.

## 5. Default comments & tags

Two small reader classes complement `XMLReader`:

- `DefaultCommentReader` ([src/defaultcommentreader.h](../src/defaultcommentreader.h)) —
  loads `defaultcommentfiles` (default: `defaultcomments.xml`) to pre-fill likely
  comment text per account/sub-account.
- `DefaultTagReader` ([src/defaulttagreader.h](../src/defaulttagreader.h)) —
  similar mechanism for tag-like metadata.

Continue with [Offline Sync & Conflict Resolution](05-offline-sync-conflicts.md).
