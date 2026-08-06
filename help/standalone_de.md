# sctime Standalone GUI

1. [Logfile](#log)
1. [Datenhaltung](#data) 
1. [Lokale Sperre](#sperre)
1. [Schriften](#font)
1. [Hinweise für Windows](#win)
1. [Hinweise für Mac](#ios)
1. [Datenquellen (Poweruser)](#datasources)
1. [Teilweiser Offline-Betrieb](#offline)
1. [Offline-Betrieb portabel](#portabel)


<span id="log"></span>

## Logfile
Wenn sctime mit der Option
`--logfile=FILENAME` gestartet wird, werden Meldungen des Systems in
eine Datei geschrieben, und können so z.B. auch nach Abstürzen
ausgewertet werden. 


<span id="data"></span>

## Datenhaltung: Speicherort und -format

Das Programm speichert die von ihm erzeugten Daten (Zeiten und
Einstellungen) im Unterverzeichnis `.sctime` des persönlichen
Verzeichnisses des Benutzers. Die Zeiten werden sowohl als Shellskripte
als auch als XML-Dateien abgelegt. 

Alle Einstellungen werden in `settings.xml` gespeichert. Das Programm
legt eine Sicherungskopie des Standes vor dem letzten Start an:
`settings.xml.bak`. Die Liste der persönlichen Konten wird sowohl in
`settings.xml` als auch in `zeit-YYYY-MM-DD.xml` gespeichert.

Die Kodierung (Encoding) der `zeit-YYYY-MM-DD.sh`-Dateien ist immer UTF-8.

<span id="sperre"></span>

## Lokale Sperre

Sobald das Programm startet, versucht es exklusiv eine lokale Sperre
anzulegen. Das Betriebssystem entfernt diese lokale Sperre zuverlässig
nach einem eventuellen Absturz des Programms (unter UNIX: `lockf()`). Wenn
die lokale Sperre angelegt werden kann, prüft das Programm die Existenz
der Datei `.sctime/LOCK`. Wenn es sie gibt und einen fremden
Rechnernamen enthält, dann geht das Programm davon aus, dass eine
weitere Instanz auf einem anderen Rechner läuft und beendet sich.

<span id="font"></span>

## Schriften

Standardmäßig werden die Einstellungen des Desktops übernommen. Im 
Einstellungsdialog kann man jedoch auch eine andere im System verfügbare 
Schriftart und -größe auswählen.

<span id="win"></span>

## Hinweise für Windows User

Unter Windows werden alle Konfigurationsdateien unter `H:\.sctime`
gesucht. Auch die erzeugten Zeit-Shell-Scripts und -XML-Dateien werden
dort abgelegt.

Das Passwort zur Zeitdatenbank wird in der Datei `H:\.Zeit` erwartet
(wenn es nicht in `settings.xml` konfiguriert ist).

Allgemein kann jede Nennung von `~` (Tilde) als Platzhalter für das
persönliche Verzeichnis unter UNIX im Kontext von Windows gedanklich
durch `H:` ersetzt werden.

<span id="ios"></span>

<!--
## Hinweise für Mac User

- `scTime.app` für den Mac bringt Qt und die PostgreSQL-Client-Bibliothek
als sogenannte private Frameworks selbst mit und ist so ohne externe
Komponenten lauffähig.
- `QODBC` steht nicht (oder zumindest nicht ohne externe
Softwarekomponenten) als Datenquelle zur Verfügung.

- Die Datenquelle `QPSQL` steht zur Verfügung. Falls der Nutzername am Mac
von dem in der Zeitdatenbank abweicht, muß das Modul via `settings.xml`
mit dem richtigen Nutzernamen konfiguriert werden. Weitere Hinweise
hierzu unter [Datenquellen Datenbanken](#datasource_database).

- Werden die Konten- und Bereitschaftsliste als Einzel-Dateien
bereitgestellt, müssen diese im Format UTF-8 sein. (Um genau zu sein:
Das Programm erwartet, dass diese Dateien die Kodierung aufweisen, die
von `locale charmap` angegeben wird.) Am besten erzeugt man sie mit
einem Aufruf der folgenden Form:
```
ssh user@linuxserver "LC_CTYPE=de_DE.UTF-8 zeitkonten --separator=\"|\" --sonderzeiten --psp --mikrokonten" >$HOME/.sctime/zeitkonten.txt
```

Dabei wird von passwortfreiem login ausgegangen.

Die JSON-Datei ist automatisch immer UTF-8 kodiert, so dass keine
weiteren Optionen zum Aufruf von `zeit-sctime-offline` benötigt werden.
-->




<span id="datasources"></span>

## Datenquellen (Poweruser)

Das Programm lädt die Kontenliste und die Liste der Bereitschaftsarten
mit einer der folgenden Methoden:

- `QPSQL`: Datenbank-Treiber von Qt für Postgres
- `QODBC`: Datenbank-Treiber von Qt; verwendet die ODBC-Datenquelle
  `postgres_zeit`
- `command`: verwendet die Kommandos `zeitkonten` und `zeitbereitls.`
  Ist unter Windows wirkungslos, da hier die in notwendigen Befehle
  nicht zur Verfügung stehen.
- `json`: liest die Datei `sctime-offline.json` aus dem
  Konfigurationsverzeichnis (üblicherweise `~/.sctime`)
- `file`: Veraltet. Liest die Dateien `zeitkonten.txt`,
  `sonderzeitls.txt` und `zeitbereitls.txt` aus dem
  Konfigurationsverzeichnis (üblicherweise `~/.sctime`)

Die Methoden werden in dieser Reihenfolge angewendet, bis die erste
erfolgreich ist. Typische Ursachen für das Fehlschlagen einer Methode
sind fehlende Dateien oder Einstellungen. Die auftretenden Fehler sind
unter dem Menüpunkt `Meldungen` im Menü `Hilfe` einsehbar und geben
Hinweise auf die Ursachen.

Im Attribut `names` des XML-Elements `sctime/general/backends` in
`settings.xml` kann die Standardreihenfolge der Module umkonfiguriert
werden. Hierzu werden die obigen Namen durch Leerzeichen getrennt in der
gewünschten Reihenfolge aufgezählt. sctime arbeitet sie dann in der
Reihenfolge ihrer Nennung ab.

Alternativ kann die Reihenfolge auch mit der Option `--datasource=NAME`
angegeben werden. Bei mehrmaligem Übergeben der Option werden die Module
in der Reihenfolge ihrer Nennung abgearbeitet.

<span id="datasource_command"></span>

### Datenquelle `command`

Die aufgerufenen Befehle lauten:

    zeitkonten --mikrokonten --psp --sonderzeiten --separator='|'
    zeitbereitls --separator='|'
    sonderzeitls --separator='|'

Momentan können sie nicht konfiguriert werden.

### Datenquelle `json`

Alle notwendigen Daten sind in einer einzelnen JSON-Datei enthalten. Der
Standardpfad zur Datei ist `~/.sctime/sctime-offline.json`. Dieser kann
mittels der Option `--offlinefile` geaendert werden. 

TODO Flo: syntax json 

### Datenquelle `file`

Die Dateien werden als `~/.sctime/zeitkonten.txt`,
`~/.sctime/sonderzeiten.txt` und `~/.sctime/zeitbereitls.txt` erwartet.

Alternative Pfade können mittels der Parameter `--zeitkontenfile`,
`--sonderzeitenfile` und `--bereitschaftsfile` beim Aufruf angegeben
werden.

Um die Dateien mit dem erforderlichen Inhalt bereitzustellen, bietet
sich der manuelle Aufruf der unter [Datenquelle
command](#datasource_command) angegebenen Befehle an. 

<span id="datasource_database"></span>

### Datenquellen `QPSQL` und `QODBC` (Datenbanken)

Mit den folgenden Attributen des Elements
`sctime/general/backends/database` können die Parameter der
Datenbank-Datenquellen konfiguriert werden. Die möglichen Attribute
sind:

- `server`: der Name des zu kontaktierenden Servers
- `name`: der Name der Datenbank auf dem Server
- `user`: der zu verwendende Nutzername. Ist das Attribut nicht
  angegeben oder der Wert leer, wird der Anmeldename des Betriebssystems
  als Anmeldename verwendet.
- `password`: Passwort des Datenbanknutzers. Ist das Attribut nicht
  vorhanden oder leer, wird nach einer Datei `~/.Zeit` (in dieser
  Gross-/Kleinschreibung, unter Windows `H:\.Zeit`) gesucht und dessen
  erste Zeile (ohne den Zeilenumbruch) als Passwort verwendet. Existiert
  auch die Passwortdatei nicht, wird der Nutzername als Kennwort
  versucht.

<span id="offline"></span>

## Teil-Offline-Betrieb (im Zusammenhang mit Datenbank)

Die bereits erläuterte Suchreihenfolge für Kontendaten erlaubt auch
einen teilweisen Offline-Betrieb: Solange die Datenbank erreicht werden
kann, werden die aktuellen Kontendaten bei jedem Start von dort bezogen.
Nur wenn die Datenbank nicht erreichbar ist, wird auf die Dateien in
`~/.sctime` zurückgegriffen. Man kann diese Dateien also für
offline-Betrieb bereitlegen und bei Verfügbarkeit trotzdem automatisch
die Datenbank nutzen.

Dabei gibt es jedoch zwei Dinge zu beachten

1.  Man sollte nicht vergessen, die Dateien regelmäßig oder zumindest
    vor jedem offline-Einsatz zu aktualisieren.
2.  Durch den Verbindungsversuch zur Datenbank kann zu langen
    Wartezeiten beim Programmstart und Neulesen der Kontenliste kommen.
    Das ist vor allem dann der Fall, wenn man sich in einem Netzwerk
    befindet, in dem der konfigurierte Datenbankservername auflösbar,
    der zugehörige Server aber nicht erreichbar ist, weil eine Firewall
    dorthin gerichteten Traffic stillschweigend verwirft. In einem
    solchen Fall kann die Suchreihenfolge wie unter
    [Datenquellen](#datasources) beschrieben, vorübergehend abgeändert
    oder permanent umkonfiguriert werden.


<span id="portabel"></span>

## Portable Edition (USB-Stick)

sctime kann ohne Probleme von einem USB-Stick gestartet werden. Jedoch
ändert sich unter Windows beim Arbeiten an verschiedenen Rechnern immer
wieder der Laufwerksbuchstabe des USB-Sticks. Das hat zur Folge, daß der
default `H:\.sctime` für die Suche nach Einstellungsdateien nur in den
seltensten Fällen zutreffend ist.

Als Hilfsmittel kann man sich jedoch den Fakt zunutze machen, daß unter
Windows das aktuelle Verzeichniss eines Programmes, das über eine
Verknüpfung gestartet wurde, dessen Arbeitsverzeichnisangabe leer ist,
dem Verzeichnis der Verknüpfung entspricht. Man erstellt also an der
Wurzel des USB-Sticks eine Verknüpfung. Darin trät man den folgenden
Aufruf von sctime mit ausschließlich relativen Pfaden ein:

```
\sctime\sctime.exe --configdir=\.sctime
```

Das Feld zur Eingabe des Arbeitsverzeichnisses leert man vollständig.

Ein so gestartetes sctime sucht nun alle Dateien im Unterverzeichnis
`.sctime` des USB-Sticks, unabhängig davon, welchen Laufwerksbuchstaben
er gerade hat.

UNIX/Linux-Nutzer sorgen einfach auf geeignetem Wege dafür daß der
USB-Stick immer unter demselben Pfad gemountet wird.
