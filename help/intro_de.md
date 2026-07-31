# sctime


1. [Einführung](#intro)
1. [Einstellungen](#usage)
1. [Kontenbaum](#tree)
1. [Aktives Konto](#active)
1. [Zeiten in der GUI bearbeiten](#edit)
1. [Zeiten speichern](#save)
1. [Standard-Kommentare, Mikrokonten, vordefinierte Kommentare](#comments)
1. [Anwesenheitszeiterfassung](#fromto)
1. [Bereitschaftszeiten](#bereit)
1. [Sonderzeiten](#sonder)
1. [Typ und PSP](psp#)
1. [Tipps & Tricks](#tipps)


<span id="intro"></span>

## Einführung

sctime erfaßt Arbeitszeiten in Echtzeit, indem es die verstreichende Zeit
fortlaufend auf einem vom Nutzer aktiv ausgewähltem Zeit(unter)konto
aufsummiert. 
- Mögliche Zeitkonten werden zentral vorgegeben, ändern sich je nach Bedarf und werden daher beim Programmaufruf neu eingelesen.
- sctime schreibt die Zeiten laufend in eine Zeitdatei `zeit-YYYY-MM-DD.sh`. 

Die Zeiten können während der Erfassung in der GUI mit Kommentaren erweitert werden,
um zu beschreiben, was in dieser Zeit getan wurde.
- Für jedes Unterkonto können mehrere Zeiten (in der GUI: Zeilen) mit jeweils unterschiedlichen Kommentaren erfaßt werden, in der Zeitdatei stehen dann entsprechend viele unterschiedliche Einträge.
- Jede dieser Unterkonto-Kommentarzeilen hat ein eigenes Unterkonto-Bearbeitungsfenster.
- Wie unter [Standard-Kommentare](#comments) erläutert, können für alle Unterkonten Standard-Kommentare zentral vorgegeben werden, die der Anwender dann nur noch aus der Drop-Down-Liste des Kommentars (Doppelklick) aktivieren muss.

Der Anwender behält die Kontrolle über die erfassten Zeiten und kann seine Zeitdateien `zeit-YYYY-MM-DD.sh` im Nachgang bearbeiten.

<span id="usage"></span>

## Einstellungen

| Option | Default | Beschreibung | 
| --- | --- | --- |
|Kommentare für den nächsten Tag übernehmen|-|speichert alle Kommentare für den nächsten Tag.|
|Poweruser Ansicht|-| bietet in der GUI zusätzliche Komfortoptionen für abrechenbare Zeiten an. |
|Konto durch Singleclick aktivieren|-|Default: Doppelklick notwendig|
|Kontotyp anzeigen|-|Zeigt Kontotyp an|
|PSP Element anzeigen|-|Zeigt PSP Element an|
|Default Kommentar automatisch verwenden, wenn eindeutig|-| wenn die sctime GUI nur einen Kommentar findet, verwendet sie diesen automatisch|
|Drag 'n' drop aktivieren|X|aktiviert Drag 'n' drop für Zeiten|
|Summe in persönliche Konten|-|zeigt Summen für Konten und für Unterkonten mit mehreren Kommentaren |
|Sonderzeitauswahl anzeigen|-|zeigt die Auswahl der Sonderzeiten im Unterkonto-Dialog mit an|
|Warnen wenn Kommentar nicht in ISO8859 darstellbar|X|bei Eingabe von ungewöhnlichen Zeichen, um spätere Probleme bei der Auswertung zu verhindern|
|Nach Kommentartext sortieren (statt numerisch)|-|sortiert bei mehreren Kommentarzeilen von Unterkonten nach Anfrangbuchstaben|
|Offline bleiben nach Initialisierung|-| **nur Web GUI:** speichert Zeitdateien immer im Browser |
|Schreibe keine konsolidierten Arbeitszeiten in die SH-Datei |-| schreibt keine Anfangs- und Endzeiten in die Zeitdateien |

<span id="tree"></span>
## Kontenbaum

Die Kontenhierarchie besteth aus einer Baumstruktur: Bereich -> Konto -> Unterkonto

Unter `Konten` gibt es zwei Einstiegspunkte:
- `Alle Konten`
- `Persönliche Konten`

`Alle Konten` zeigt alle zentral bereitgestellten Konten an. 
- Zeiten können nur auf unterster Ebene (Unterkonto) erfasst werden.
- Bei Änderungen an der Struktur  `Konto - Kontoliste neu laden (Ctrl+R)` ausführen.
- Ein Doppelklick auf ein Unterkonto wählt es als das aktive aus (s. [Einstellungen](#usage)). sctime beginnt sofort mit der Erfassung von Zeiten für dieses Unterkonto.


Unterkonten in den Bereich `Persönliche Konten` übernehmen und Unterkonto-Bearbeitungsfenster:
- Ein Rechtsklick (oder ein Doppel-Klick bei Single Klick-Aktivierung) auf eine Unterkonto-Zeile öffnet das Bearbeitungs-Fenster des Unterkontos. 
- `In die persönlichen Konten übernehmen` anhaken.
- Kommentar eintragen oder auswählen. Wenn in `Einstellungen - Einstellungen - Allgemein` die Option `Kommentare für nächsten Tag übernehmen` aktiviert ist, speichert die sctime GUI alle Kommentare, so daß man diese nicht neu eingeben muss.
- (Abzurechnende) Zeiten bearbeiten.
- Wenn eine Beschreibung und ein Verantwortlicher für das Unterkonto zentral definiert sind, wird diese Information hier angezeigt. 

Zusätzliche Zeiterfassung auf dasselbe Unterkonto mit anderem Kommentar:
- `Konto - Eintrag hinzufügen` legt für das aktive Unterkonto eine zusätzliche Zeile an, in der ein anderer Kommentar eingetragen oder ausgewählt werden kann.
- Ab zwei Zeilen läßt sich das Unterkonto in der GUI zu- und aufklappen.

<span id="active"></span>

## Aktives Konto

Sobald man das Programm startet:
- läuft die Zeiterfassung
- auf dem mit einem Haken als aktiv markierten Unterkonto.

Je nach Setting aktiviert ein einzelner oder ein Doppel-Klick (Default) das gewünschte Unterkonto.

<span id="edit"></span>

## Zeiten in der GUI bearbeiten
Der aktuelle Tag oder ein vergangener Tag (`Zeit - Datum wählen...`) können geändert werden:
- in der GUI direkt (Icons für Zeit erhöhen/verringern) 
- im Bearbeitungs-Fenster des jew. Unterkontos

Die `Gesamtzeit` ganz rechts unten zeigt, wieviele Minuten + oder - die Zeiten in Summe geändert wurden.
- Dies erleichtert die Umbuchung von Aufwänden zwischen verschiedenen Unterkonten. 
- Wenn hier keine Differenz angezeigt wird, war die Umbuchung erfolgreich.
- Wenn man zuviel/zuwenig Zeiten erfaßt hat (Pausen vergessen), kann man je nach Bearf mit `Zeit - Differenz auf Null Ctrl+N` die aktuelle Erhöhung/Reduzierung der Gesamtstunden wieder auf 0 setzen.

<span id="save"></span>

## Zeiten speichern

Im Betrieb speichert die sctime GUI automatisch alle fünf Minuten. 

Die aktuellen Zeiten und die
Einstellungen werden sofort gespeichert:
- bei Programmende
- wenn der Anwender aktiv speichert (via Button, Ctrl+S, `Konto - Speichern`)



<span id="comments"></span>

## Standard-Kommentare/ Mikrokonten/ vordefinierte Kommentare

Es gibt gute Gründe dafür, Standard-Kommentare vorzugeben: 
- Komfort: der Anwender muss nur noch aus einer Liste auswählen. 
- Einheitlichkeit: erleichtert die Auswertung

Die drei oben genannten Begriffe bedeuten das Gleiche: 
- Für alle Unterkonten können Kommentare zur Auswahl im Unterkonto-Bearbeitungsfenster der GUI zentral vordefiniert werden. 
- Standardkommentare kann der Administrator vorgeben, oder alle Anwender dürfen dies mittels eines entsprechenden Kommandozeilentools.  
- Die Anwender müssen die Kontenliste neu laden, um neue Standardkommentare zu erhalten.

Mikrokonten geben der Kontenbaumstruktur noch eine weitere Ebene:
- z.B. kann ein Standardkommentar `Bugfix:` vorgegeben werden. 
- Der Anwender kann nach dem Doppelpunkt weitere Details eintragen.
- Mittels `Bugfix:` können alle Zeiten für das Thema zusammen ausgewertet werden.




<span id="fromto"></span>

## Anwesenheitszeiterfassung

Die sctime GUI erfasst Anwesenheitszeiten, um die Einhaltung von
Arbeitsschutzvorschriften nachzuweisen, und ggf. Warndialoge zur
Erinnerung an fehlende Pausen zu generieren.

Zur Dokumentation wird eine bereinigte Zusammenfassung der
Anwesenheitszeiten in `zeit-YYYY-MM-DD.sh`-Dateien abgelegt oder im Browser gespeichert. 
- Als
Anwesenheitszeit zählt die sctime GUI automatisch alle Zeiträume, in denen sie
läuft und nicht pausiert ist. 
- Die erfassten Zeiten können über `Zeit - Anwesenheitszeiten` 
eingesehen und bei Bedarf korrigiert werden.

Wenn die auf Konten gebuchte Gesamtzeit geändert wurde (z.B. einige
Minuten einem Konto hinzugefügt wurden, da man vergessen hat die Pause
rechtzeitig heraus zu nehmen), bekommt man als Unterstützung beim Aufruf
von `Zeit - Differenz auf Null` angeboten, den Beginn des aktuellen
Arbeitsintervalls in den Anwesenheitszeiten ebenfalls entsprechend zu
korrigieren.


<span id="bereit"></span>

## Bereitschaftszeiten (Stempel-Icon)

Bereitschaften gibt der Administrator zentral vor.

Um Bereitschaftszeiten zu erfassen
- Zeiteintrag wählen,  zu dem die Bereitschaft gehört. 
- `Vergütung - Bereitschaftszeiten setzen... Ctrl+B` auswählen

Stempel in der Menüleiste klicken. 
- in der Auswahl möglicher
Bereitschaftskategorien eine oder mehrere
Kategorien auswählen. 
- Anschließend  `Ok` klicken.

Zum Löschen klickt man wieder auf den Stempel, wählt angewählte
Kategorien ab, und bestätigt mit `Ok`.

<span id="sonder"></span>

## Sonderzeiten (Mond-Icon)

Bei Sonderzeiten handelt es sich um geleistete Arbeitszeit, die zu
"ungewöhnlichen" Zeiten wie z.B. Nachts oder an Feiertagen geleistet
wird. Gegebenfalls werden Zuschläge für geleistete Sonderzeiten
abgerechnet. Sie sollten daher nur nach den aktuell geltenden Regelungen
und Vereinbarungen eingetragen werden. Sonderzeitkategorien gibt der Administrator zentral vor.

Um Sonderzeiten zu erfassen:

- Zeiteintrag wählen, zu dem die Sonderzeit gehört. 
- `Vergütung - Setze Sonderzeit Kategorien... Ctrl+T` auswählen.
- in der Auswahl möglicher Sonderzeitkategorien 
eine oder mehrere Kategorien auswählen. 
- Anschließend `Ok` klicken.

Zum Löschen klickt man wieder auf den Mond, wählt angewählte Kategorien
ab, und bestätigt mit `Ok`.

Wenn zentral Konten mit dem Typ 'x' und 'o' markiert sind, können keine Sonderzeiten gesetzt werden.

### Sonderzeit-Modus
Ein Sonderzeit-Modus verknüpft eine Sonderzeit-Kategorie mit dem automatischen Setzen dieser Kategorie für alle danach erfassten Zeiten bis der Sonderzeit-Modus deaktiviert wird, z.B. Nachtarbeit:
- Ein Sonderzeit-Modus kann nur zentral vorgegeben werden.
- Wenn Sonderzeit-Modi gesetzt sind, sieht der Anwender sie in `Vergütung` unterhalb von `Setze Sonderzeiten Kategorien...`. 
- Wenn man einen schon vorhandenen
Zeiteintrag aktivieren möchte, bei dem die Kategorie nicht gesetzt ist,
wird stattdessen ein neuer Eintrag erzeugt mit entsprechend gesetzter
Kategorie, damit die Sonderzeit getrennt vorn der normalen Zeiterfassung bleibt.

**Wichtig:** Sonderzeit-Modi sind nur eine Komfortfunktion.
Sie prüfen nicht, ob der Anwender in der jeweiligen
Situation anspruchsberichtigt ist. Dafür muss der Mitarbeiter 
selbst die im Unternehmen aktuell geltenden Regelungen kennen und
beachten.

<span id="psp"></span>

## Typ und PSP
Beides kann zur differenzierten Auswertung/Gruppierung von Konten für jedes Unterkonto zentral vorgegeben werden. 
- Der Anwender kann die Werte nicht ändern. 
- Per Default sind sie in der GUI ausgeblendet, s. [Einstellungen](#usage)

<span id="tipps"></span>

## Tipps & Tricks
### Drag'n'Drop

Mittels Drag 'n' Drop lassen sich die Zeiten zwischen Unterkonten und
Einträgen verschieben. 

Wird dabei die „Shift“-Taste gedrückt, werden
auch die gesetzten Kommentare verschoben.

### Logging

Unter `Hilfe - Meldungen` kann man Log-Meldungen zu
Datenquellen und ähnlichem einsehen. 


### (Unter-)konten farbig markieren

`Konto - Hintergrundfarbe wählen/entfernen` erlaubt es (Unter-)konten farbig zu hinterlegen.

