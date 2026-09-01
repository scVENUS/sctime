# Glossary

sctime's domain vocabulary is German (the codebase originates from a German
organization: "s+c" / science+computing ag). This glossary maps the terms
you'll encounter in class/variable names, XML attributes, and the UI to their
English meaning, to make it easier to navigate the source.

| Term | Meaning | Where you'll see it |
|---|---|---|
| Abteilung | Department | `AbteilungsListe`, `abteilungsliste.h/.cpp` |
| Konto | Account | `KontoListe`, `KontoTreeView`, `KontoTreeItem`, `findkontodialog.h` |
| Unterkonto | Sub-account | `UnterKontoListe`, `UnterKontoEintrag`, `UnterKontoDialog` |
| Eintrag | Entry (a single time booking) | `EintragsListe`, `eintragAktivieren`, `eintragHinzufuegen`, `eintragEntfernen` |
| Zeit | Time | `sctime` itself, `changeZeit`, `zeitChanged`, `zeitKommando` setting |
| Sekunden | Seconds | `UnterKontoEintrag::sekunden` |
| Abzur / Abzurechnend | "To be billed/invoiced" — the portion of worked time that is billable to the customer | `sekundenAbzur`, `gesamtZeitAbzurChanged`, `pauseAbzur` |
| Bereitschaft | On-call / standby duty | `BereitschaftsListe`, `BereitschaftsModel`, `BereitschaftsView`, `OnCallDialog` |
| Sonderzeitkategorie / Sonderzeiten | Special remuneration category (e.g. night/holiday surcharge) | `SpecialRemunTypeList`, `specialremunerationsdialog.*` |
| Zeitkonto | Time account (general term for a bookable account) | comments throughout `src/` |
| Zeitkommando | Time command (`zeitKommando` setting, e.g. `"zeit"`) | `SCTimeXMLSettings` |
| Zeitkonten | "Time accounts" — the legacy Unix command providing the account list | `CommandReader`, `--zeitkontenfile=` CLI option |
| Zeitbereitls | The legacy Unix command listing on-call ("Bereitschaft") categories | `setupdsm.cpp`, `--bereitschaftsfile=` CLI option |
| Kommentar | Comment (free text on an entry) | `UnterKontoEintrag::kommentar`, `DefaultCommentReader` |
| Kostenstelle | Cost center | `CostCenter` in the [offline data JSON schema](03-data-sources-backends.md#5-offline-data-json-schema) |
| Verantwortlicher / Stellvertreter | Person responsible / deputy for an account | `ResponsiblePersons` in the offline data JSON schema |
| Datum | Date | `AbteilungsListe(const QDate& _datum, ...)` |
| Aktiv / Aktives Konto | Active account (the one currently accumulating time) | `aktivesKontoPruefen`, `setAktivesProjekt` |
| Persönlich / Persönliche Konten | Personal accounts (vs. shared/team accounts) | `moveEintragPersoenlich`, `PERSOENLICHE_KONTEN_STRING` |
| Gesamtzeit | Total time | `gesamtZeitChanged` signal |
| Abgerechnet bis / InvoicedUntil | Date up to which an account has been invoiced | `InvoicedUntil` in the offline data JSON schema |

For the offline-data JSON schema itself (account tree, on-call times, special
remunerations), see [Offline data JSON schema](03-data-sources-backends.md#5-offline-data-json-schema).

Back to [the index](README.md).
