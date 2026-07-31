## sctime Web GUI

### Settings importieren und löschen

- `Einstellungen - Importieren` erlaubt den Import einer bereits vorhandenen `settings.xml`-Datei aus der sctime Standalone GUI.

- `Einstellungen - Konfigurationsdaten löschen` erlaubt dem Anwender seine gespeicherten Daten zu löschen.
  - Dringend beachten, daß man hier alle Einstellungen und alle erfassten Zeiten löschen kann, s. Auswahl im Menü. 
  - Die Daten sind nicht wiederherstellbar.


### Offline Zeiten erfassen

`Einstellungen - Einstellungen - Allgemein - Offline bleiben nach Initialisierung` setzt dauerhaft den Offline Modus. 
  - Alle Daten (Einstellungen und Zeiten) werden im Offline Modus nur im Browser gespeichert. 
  - Wenn der Anwender seine Browserdaten löscht, sind auch die sctime Daten weg!
  - Sobald man den Modus deaktivert, synchronisiert die sctime Web GUI die Daten und meldet dies auch in der Statuszeile.

<span id="dataexp"></span>

### Offline Zeiten exportieren
Wer seine Zeiten ausschliesslich offline erfasst, muss sie zur Weiterverarbeitung exportieren:

- `Konto - SH Dateien herunterladen` 
- Zeitraum auswählen
- Speicherort auswählen und bestätigen

Die sctime Web GUI exportiert eine `.zip`-Datei:
- `.zip`-Datei auspacken
- enthaltene `zeit-YYYY-MM-DD.sh`-Dateien weiterverarbeiten

