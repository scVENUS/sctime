<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="de_DE">
<context>
    <name>ConflictDialog</name>
    <message>
        <location filename="conflictdialog.cpp" line="169"/>
        <source>sctime: unresolvable conflict</source>
        <translation>sctime: unlösbarer Konflikt</translation>
    </message>
    <message>
        <location filename="conflictdialog.cpp" line="170"/>
        <source>There seems to be a conflict with another session that could not be resolved. Please check your entries.</source>
        <translation>Es scheint ein Konflikt mit einer anderen Session zu bestehen, der nicht aufgelöst werden konnte, Bitte prüfe Deine Zeiteinträge.</translation>
    </message>
</context>
<context>
    <name>ConflictDialogBase</name>
    <message>
        <location filename="conflictdialogbase.ui" line="23"/>
        <source>sctime: conflict on saving</source>
        <translation>sctime: Konflikt beim Speichern</translation>
    </message>
    <message>
        <location filename="conflictdialogbase.ui" line="35"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;There seems to be another session existing, that has also produced data.&lt;/p&gt;&lt;p&gt;You have several options now:&lt;/p&gt;&lt;p&gt;Close this session: in this case you&apos;ll loose data from this session, but you can continue in the existing session.&lt;/p&gt;&lt;p&gt;Replace local data with data from existing session: in this case further writing from the other, existing session will be disabled, and the data, that you recorded in this window will be replaced by the data from the existing session. &lt;/p&gt;&lt;p&gt;Merge local data with data from existing session: in this case further writing from the other, existing session will be disabled, and the data, that you recorded in this window will be merged with the data from the existing session. Merging might lead to duplicated entries, please check your data if you do this...&lt;/p&gt;&lt;p&gt;Keep data disables the existing session and keeps your local data unchanged. The data on the server from the other session will be ignored and overwritten.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <oldsource>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;There seems to be another session existing, that has also produced data.&lt;/p&gt;&lt;p&gt;You have several options now:&lt;/p&gt;&lt;p&gt;Close this session: in this case you&apos;ll loose data from this session, but you can continue in the existing session.&lt;/p&gt;&lt;p&gt;Replace local data with data from existing session: in this case further writing from the other, existing session will be disabled, and the data, that you recorded in this window will be replaced by the data from the existing session. &lt;/p&gt;&lt;p&gt;Merge local data with data from existing session: in this case further writing from the other, existing session will be disabled, and the data, that you recorded in this window will be merged with the data from the existing session. Merging might lead to duplicated entries, please check your data if you do this...&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</oldsource>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Es scheint eine andere Session zu geben, die ebenfalls Daten erzeugt hat.&lt;/p&gt;&lt;p&gt;Du hast nun folgende Optionen:&lt;/p&gt;&lt;p&gt;Diese Session schliessen: In diesem Fall verlierst Du die Daten dieser Session, aber Du kannst in der anderen, existierenden Session weiterarbeiten.&lt;/p&gt;&lt;p&gt;Lokale Daten mit Daten aus existierender Session ersetzen: in diesem Fall wird der Schreibzugriff der anderen existierenden Session unterbunden, und alle Daten die in diesem Fenster erfasst wurden, werden durch Daten der existierenden Session ersetzt. &lt;/p&gt;&lt;p&gt;Lokale Daten mit Daten von existierender Session mergen: in diesem Fall wird der Schreibzugriff der anderen existierenden Session unterbunden, und die Daten die in diesem Fenster erfasst wurden, werden mit denen der existierenden Session gemergt, also zusammengeführt. Mergen kann zu doppelten Einträgen führen, bitte prüfe Deine Daten, wenn Du diese Option wählst...&lt;/p&gt;&lt;p&gt;Daten behalten: deaktiviert die existierende Session und behält Deine lokalen Daten. Von der anderen session stammende Daten auf dem Server werden ignoriert und überschrieben.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="conflictdialogbase.ui" line="60"/>
        <source>Close this session</source>
        <translation>Diese Session schliessen</translation>
    </message>
    <message>
        <location filename="conflictdialogbase.ui" line="74"/>
        <source>Keep data</source>
        <translation>Daten behalten</translation>
    </message>
    <message>
        <location filename="conflictdialogbase.ui" line="50"/>
        <source>Replace data</source>
        <translation>Daten ersetzen</translation>
    </message>
    <message>
        <location filename="conflictdialogbase.ui" line="67"/>
        <source>Merge data</source>
        <translation>Daten mergen</translation>
    </message>
</context>
<context>
    <name>DateDialog</name>
    <message>
        <location filename="datedialog.cpp" line="80"/>
        <source>Week %1</source>
        <translation>Woche %1</translation>
    </message>
</context>
<context>
    <name>DateDialogBase</name>
    <message>
        <location filename="datedialogbase.ui" line="20"/>
        <source>Select date</source>
        <translation>Datum wählen</translation>
    </message>
    <message>
        <location filename="datedialogbase.ui" line="38"/>
        <source>Date to edit:</source>
        <translation>Zu editierendes Datum:</translation>
    </message>
    <message>
        <location filename="datedialogbase.ui" line="47"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="datedialogbase.ui" line="57"/>
        <source>&amp;Apply</source>
        <oldsource>Apply</oldsource>
        <translation>A&amp;nwenden</translation>
    </message>
    <message>
        <location filename="datedialogbase.ui" line="64"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Abbrechen</translation>
    </message>
    <message>
        <location filename="datedialogbase.ui" line="81"/>
        <source>today</source>
        <translation>heute</translation>
    </message>
</context>
<context>
    <name>DeleteSettingsDialog</name>
    <message>
        <location filename="deletesettingsdialog.cpp" line="49"/>
        <source>This operation deletes all your working times that you have not checked in.
Proceed?</source>
        <translation>Diese Funktion löscht alle Zeiteinträge, die noch nicht eingecheckt sind.\nMöchtest Du fortfahren?</translation>
    </message>
    <message>
        <location filename="deletesettingsdialog.cpp" line="101"/>
        <source>The file could not be deleted on the server. Error code is %1</source>
        <translation>Die Datei konnte nicht auf dem Server gelöscht werden. Der Fehlerode lautet %1</translation>
    </message>
</context>
<context>
    <name>DeleteSettingsDialogBase</name>
    <message>
        <location filename="deletesettingsdialogbase.ui" line="20"/>
        <source>sctime: delete settings</source>
        <translation type="unfinished">Einstellungen und Daten löschen</translation>
    </message>
    <message>
        <location filename="deletesettingsdialogbase.ui" line="32"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Please select the settings you wish to delete.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;WARNING:&lt;/span&gt; deleting daily settings also deletes all data that you recorded for this day (including time entries).&lt;/p&gt;&lt;p&gt;It is recommended to choose to stop the app - otherwise current settings remain in memory and will be written regulary, possibly recreating some of the deleted files.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Bitte wähle die Einstellungen und Daten, die Du löschen möchtest.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;WARNUNG:&lt;/span&gt; Löschen der Tageseinstellungen löscht auch alle aufgezeichneteten Zeiten für diesen Tag (inklusive Zeiteinträge).&lt;/p&gt;&lt;p&gt;Es ist empfehlenswert die Anwendung zu beenden - andernfalls verbleiben die Einstellungen im Speicher der Applikation und werden weiterhin regelmässig abgespeichert, was möglicherweise gelöschte Dateien neu erzeugt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="deletesettingsdialogbase.ui" line="55"/>
        <source>Data to delete</source>
        <translation type="unfinished">Zu löschende Daten</translation>
    </message>
    <message>
        <location filename="deletesettingsdialogbase.ui" line="61"/>
        <source>Global setting (local copy)</source>
        <translation type="unfinished">Globale Einstellungen (lokale Kopie)</translation>
    </message>
    <message>
        <location filename="deletesettingsdialogbase.ui" line="68"/>
        <source>Global setting (copy on server)</source>
        <translation type="unfinished">Globale Einstellungen (Kopie auf dem Server)</translation>
    </message>
    <message>
        <location filename="deletesettingsdialogbase.ui" line="75"/>
        <source>All daily settings and time entries (local copy)</source>
        <translation type="unfinished">Alle Tageseinstellungen und Zeiteinträge (lokale Kopie)</translation>
    </message>
    <message>
        <location filename="deletesettingsdialogbase.ui" line="82"/>
        <source>Stop App after deleting (recommended)</source>
        <translation type="unfinished">Anwendung beenden nach dem löschen (empfohlen)</translation>
    </message>
</context>
<context>
    <name>DownloadSHDialogBase</name>
    <message>
        <location filename="downloadshdialogbase.ui" line="14"/>
        <source>Download SH Files</source>
        <translation>SH-Dateien herunterladen</translation>
    </message>
    <message>
        <location filename="downloadshdialogbase.ui" line="20"/>
        <source>Please choose the range of dates to download:</source>
        <translation>Bitte wähle den Zeitraum den Du herunterladen möchtest:</translation>
    </message>
    <message>
        <location filename="downloadshdialogbase.ui" line="27"/>
        <source>Startdate</source>
        <translation>Anfangsdatum</translation>
    </message>
    <message>
        <location filename="downloadshdialogbase.ui" line="44"/>
        <source>Enddate</source>
        <translation>Enddatum</translation>
    </message>
</context>
<context>
    <name>FindKontoDialog</name>
    <message>
        <location filename="findkontodialog.cpp" line="72"/>
        <source>sctime - Search</source>
        <translation>sctime - Suche</translation>
    </message>
    <message>
        <location filename="findkontodialog.cpp" line="84"/>
        <source>Please select type of item to search for:</source>
        <translation>Bitte wählen Sie den gesuchten Typ:</translation>
    </message>
    <message>
        <location filename="findkontodialog.cpp" line="86"/>
        <source>Please select name of item to search for:</source>
        <translation>Bitte wählen Sie den gesuchten Namen:</translation>
    </message>
    <message>
        <location filename="findkontodialog.cpp" line="102"/>
        <source>Search result</source>
        <translation>Suchergebnis</translation>
    </message>
    <message>
        <location filename="findkontodialog.cpp" line="89"/>
        <source>&amp;OK</source>
        <oldsource>OK</oldsource>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="findkontodialog.cpp" line="91"/>
        <source>&amp;Cancel</source>
        <oldsource>Chancel</oldsource>
        <translation>&amp;Abbrechen</translation>
    </message>
    <message>
        <location filename="findkontodialog.cpp" line="92"/>
        <source>&amp;Search</source>
        <oldsource>Search</oldsource>
        <translation>&amp;Suchen</translation>
    </message>
</context>
<context>
    <name>JSONReaderUrl</name>
    <message>
        <location filename="JSONReader.cpp" line="275"/>
        <source>Couldn&apos;t open json from uri %1 with error code %2</source>
        <translation>Konnte Json von Uri %1 nicht laden mit Fehlercode %2</translation>
    </message>
</context>
<context>
    <name>KontoTreeView</name>
    <message>
        <location filename="kontotreeview.cpp" line="51"/>
        <source>Accounts</source>
        <translation>Konten</translation>
    </message>
    <message>
        <location filename="kontotreeview.cpp" line="52"/>
        <source>Type</source>
        <translation>Typ</translation>
    </message>
    <message>
        <location filename="kontotreeview.cpp" line="53"/>
        <source>PSP</source>
        <translation>PSP</translation>
    </message>
    <message>
        <location filename="kontotreeview.cpp" line="54"/>
        <source>Active</source>
        <translation>Aktiv</translation>
    </message>
    <message>
        <location filename="kontotreeview.cpp" line="55"/>
        <source>Time</source>
        <translation>Zeit</translation>
    </message>
    <message>
        <location filename="kontotreeview.cpp" line="56"/>
        <source>Accountable</source>
        <translation>Abzur.</translation>
    </message>
    <message>
        <location filename="kontotreeview.cpp" line="57"/>
        <source>Comment</source>
        <translation>Kommentar</translation>
    </message>
</context>
<context>
    <name>OnCallDialog</name>
    <message>
        <location filename="oncalldialog.cpp" line="8"/>
        <source>sctime: On-call times</source>
        <translation>sctime: Bereitschaftszeiten</translation>
    </message>
    <message>
        <location filename="oncalldialog.cpp" line="9"/>
        <source>On-call times</source>
        <translation>Bereitschaftszeiten</translation>
    </message>
    <message>
        <location filename="oncalldialog.cpp" line="14"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="oncalldialog.cpp" line="16"/>
        <source>Cancel</source>
        <translation>Abbruch</translation>
    </message>
    <message>
        <location filename="oncalldialog.cpp" line="20"/>
        <source>Please choose the rendered on-call times for this subaccount.</source>
        <translation>Bitte wählen Sie die geleisteten Bereitschaften für dieses Unterkonto aus.</translation>
    </message>
</context>
<context>
    <name>PauseDialog</name>
    <message>
        <location filename="pausedialog.cpp" line="10"/>
        <source>sctime: Pause</source>
        <translation>sctime: Pause</translation>
    </message>
    <message>
        <location filename="pausedialog.cpp" line="11"/>
        <source>Accounting has been stopped at %1.
The duration of the current break is %2.
Resume work with OK.</source>
        <oldsource>Accounting has been stopped at %1.
The duration of the current break is %2 minutes.
Resume work with OK.</oldsource>
        <translation>Die Zeiterfassung wurde um %1 Uhr angehalten.
Die Dauer der aktuellen Pause beträgt %2.
Ende der Pause mit OK.</translation>
    </message>
</context>
<context>
    <name>PauseDialogBase</name>
    <message>
        <location filename="pausedialogbase.ui" line="20"/>
        <source>Pause</source>
        <translation>Pause</translation>
    </message>
    <message>
        <location filename="pausedialogbase.ui" line="32"/>
        <source>Accounting has been stopped at %1.
The duration of the current break is %2 minutes.
Resume work with OK.</source>
        <translation>Die Zeiterfassung wurde um %1 Uhr angehalten.\n Die Dauer der aktuellen Pause beträgt %2.\n Ende der Pause mit OK.</translation>
    </message>
</context>
<context>
    <name>PreferenceDialogBase</name>
    <message>
        <location filename="preferencedialogbase.ui" line="14"/>
        <source>Settings</source>
        <translation>Einstellungen</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="27"/>
        <source>General</source>
        <translation>Allgemein</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="33"/>
        <source>General Settings</source>
        <translation>Allgemeine Einstellungen</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="39"/>
        <source>Re-use previous day&apos;s comments</source>
        <translation>Kommentare für nächsten Tag übernehmen</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="46"/>
        <source>Poweruser view</source>
        <translation>Poweruser Ansicht</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="53"/>
        <source>Activate account by single click</source>
        <translation>Konto durch Singleclick aktivieren</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="60"/>
        <source>Show account type</source>
        <translation>Kontotyp anzeigen</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="67"/>
        <source>Show PSP element number</source>
        <translation>PSP Element anzeigen</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="74"/>
        <source>Automatically use default comment if it is unique</source>
        <translation>Default Kommentar automatisch verwenden, wenn eindeutig</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="81"/>
        <source>Activate drag &apos;n&apos; drop</source>
        <translation>Drag &apos;n&apos; drop aktivieren</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="88"/>
        <source>Show sum in personal accounts</source>
        <translation>Summe in persönlichen Konten</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="95"/>
        <source>Show special remuneration selector</source>
        <translation>Sonderzeitauswahl anzeigen</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="102"/>
        <source>Warn if comment doesn&apos;t conform to ISO8859-1</source>
        <translation>Warnen wenn Kommentar nicht in ISO8859 darstellbar</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="109"/>
        <source>Sort by comment text (instead by number)</source>
        <translation>Nach Kommentartext sortieren (statt numerisch)</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="152"/>
        <source>Default Comments Display Mode</source>
        <translation>Anzeigemodus für Defaultkommentare</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="158"/>
        <source>Used default comments in bold</source>
        <translation>Verwendete Defaultkommentare in fett</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="165"/>
        <source>Not used default comments in bold</source>
        <translation>Nicht verwendete Defaultkommentare in fett</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="172"/>
        <source>Don&apos;t mark default comments</source>
        <translation>Keine Markierung</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="279"/>
        <location filename="preferencedialogbase.ui" line="285"/>
        <source>Fonts</source>
        <translation>Fonts</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="293"/>
        <source>Use custom font</source>
        <translation>Eigene Schrift verwenden</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="303"/>
        <source>Select Font</source>
        <translation>Schrift wählen</translation>
    </message>
    <message>
        <source>AaBbCc</source>
        <translation type="obsolete">AaBbCc</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="214"/>
        <source>Increments</source>
        <translation>Zeitinkrement</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="222"/>
        <source>Basic time increment</source>
        <translation>Einfache Zeitinkrement Buttons</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="243"/>
        <source>Fast time increment</source>
        <translation>Schnelle Zeitinkrement Buttons</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="322"/>
        <source>Accounts</source>
        <translation>Konten</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="327"/>
        <source>Active</source>
        <translation>Aktiv</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="196"/>
        <location filename="preferencedialogbase.ui" line="332"/>
        <source>Time</source>
        <translation>Zeit</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="119"/>
        <source>Stay offline after initialization</source>
        <translation>Offline bleiben nach Initialisierung</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="146"/>
        <source>Comments</source>
        <translation>Kommentare</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="340"/>
        <source>Accountable</source>
        <translation>Abzur.</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="348"/>
        <source>Comment</source>
        <translation>Kommentar</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="353"/>
        <source>All Accounts</source>
        <translation>Alle Konten</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="358"/>
        <source>Personal Accounts</source>
        <translation>Persönliche Konten</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="362"/>
        <source>Department</source>
        <translation>Abteilung</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="369"/>
        <source>management</source>
        <translation>Verwaltung</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="384"/>
        <location filename="preferencedialogbase.ui" line="387"/>
        <source>0:30</source>
        <translation>0:30</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="390"/>
        <source>talked to boss about workload</source>
        <translation>Mit Chef über Auslastung gesprochen</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="395"/>
        <source>training</source>
        <translation>Ausbildung</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="401"/>
        <location filename="preferencedialogbase.ui" line="404"/>
        <location filename="preferencedialogbase.ui" line="623"/>
        <location filename="preferencedialogbase.ui" line="635"/>
        <location filename="preferencedialogbase.ui" line="686"/>
        <location filename="preferencedialogbase.ui" line="698"/>
        <source>0:00</source>
        <translation>0:00</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="410"/>
        <source>Customer 1</source>
        <translation>Kunde 1</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="414"/>
        <source>project1</source>
        <translation>projekt1</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="475"/>
        <source>0</source>
        <translation>0</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="499"/>
        <source>2:00</source>
        <translation>2:00</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="511"/>
        <source>1:45</source>
        <translation>1:45</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="523"/>
        <source>web server setup: some work done, lots to do</source>
        <translation>Webserver: einiges fertig aber noch viel zu tun</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="537"/>
        <source>1</source>
        <translation>1</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="561"/>
        <location filename="preferencedialogbase.ui" line="573"/>
        <source>0:15</source>
        <translation>0:15</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="585"/>
        <source>backup: tapes changed</source>
        <translation>Backup: Bänder gewechselt</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="599"/>
        <source>2</source>
        <translation>2</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="662"/>
        <source>project2</source>
        <translation>projekt2</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="767"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="preferencedialogbase.ui" line="783"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Abbrechen</translation>
    </message>
</context>
<context>
    <name>PunchClockDialogBase</name>
    <message>
        <location filename="punchclockdialogbase.ui" line="14"/>
        <source>Punch Clock Dialog</source>
        <translation>Anwesenheitszeiten</translation>
    </message>
    <message>
        <location filename="punchclockdialogbase.ui" line="20"/>
        <source>Here you can edit your attendance time records (begin and end time of each interval you were working). sctime tries to capture them automatically like the accounting records, but both are largely independent and may deviate. This dialog allows you to edit the raw data, you do not need to worry about consolidating overlapping intervals and similar issues. You can see a preview of the consolidated data that would be recorded in the central system at the bottom.</source>
        <translation>Hier kannst Du Deine Anwesenheitszeiten editieren (Uhrzeit von Anfang und Ende jedes Intervals in dem Du gearbeitet hast). Sctime versucht diese automatisch aufzuzeichen wie die geleisteten Stunden, aber beides ist weitgehend unabhängig und kann voneinander abweichen. Mit diesem Dialog kannst Du diese Rohdaten editieren. Du brauchst Dir dabei über Überlappungen und ähnliches keine Gedanken zu machen. Die konsolidierten Daten, die basierend auf den Rohdaten im Zentralsystem abgelegt werden würden, können in der Vorschau weiter unten im Dialog geprüft werden.</translation>
    </message>
    <message>
        <location filename="punchclockdialogbase.ui" line="42"/>
        <source>Editor</source>
        <translation>Editor</translation>
    </message>
    <message>
        <location filename="punchclockdialogbase.ui" line="112"/>
        <source>Preview</source>
        <translation>Vorschau</translation>
    </message>
    <message>
        <source>Here you can edit your attendance time records (begin and end time of each interval you were working). sctime tries to capture them automatically like the accounting records, but both are largely independent and may deviate. </source>
        <translation type="vanished">Hier kannst Du Deine Anwesenheitszeiten editieren (Start und Ende jedes Zeitraums in dem Du gearbeitet hast). sctime versucht diese Zeiten automatisch aufzuzeichnen, analog zu Kontobuchungen. Beide Systeme sind unabhängig und können voneinander abweichen.</translation>
    </message>
    <message>
        <location filename="punchclockdialogbase.ui" line="75"/>
        <source>Insert</source>
        <translation>Einfügen</translation>
    </message>
    <message>
        <location filename="punchclockdialogbase.ui" line="68"/>
        <source>Delete</source>
        <translation>Löschen</translation>
    </message>
</context>
<context>
    <name>QDatePicker</name>
    <message>
        <source>Week %1</source>
        <translation type="vanished">Woche %1</translation>
    </message>
    <message>
        <source>Select week</source>
        <translation type="vanished">Woche wählen</translation>
    </message>
    <message>
        <source>Select day</source>
        <translation type="vanished">Heutigen Tag wählen</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="abteilungsliste.cpp" line="501"/>
        <source>Unknown Mode in applyFlagMode</source>
        <translation>Unbekannter Modus in applyFlagMode</translation>
    </message>
    <message>
        <location filename="bereitschaftsmodel.cpp" line="94"/>
        <source>Designation</source>
        <translation>Bezeichnung</translation>
    </message>
    <message>
        <location filename="datasource.cpp" line="61"/>
        <location filename="datasource.cpp" line="74"/>
        <source>%1: no data source available</source>
        <translation>%1: keine Datenquelle verfügbar</translation>
    </message>
    <message>
        <location filename="datasource.cpp" line="87"/>
        <source>file &apos;%1&apos; cannot be opened: %2</source>
        <translation>kann  Datei &apos;%1&apos; nicht öffnen: %2</translation>
    </message>
    <message>
        <location filename="datasource.cpp" line="91"/>
        <source>Reading </source>
        <translation>Lese </translation>
    </message>
    <message>
        <location filename="datasource.cpp" line="118"/>
        <source>Line %1 of &apos;%2&apos; has only %3 columns instead of %4</source>
        <translation>Zeile %1 von &apos;%2&apos; hat nur %3 Spalten statt %4</translation>
    </message>
    <message>
        <location filename="datasource.cpp" line="134"/>
        <source>Connecting to database %1 on %2 with driver %3 as user %4</source>
        <translation>Verbindungsaufbau zu Datenbank %1 auf %2 mit Treiber %3 als Benutzer %4</translation>
    </message>
    <message>
        <location filename="datasource.cpp" line="137"/>
        <source>connection failed: </source>
        <translation>Verbindungsaufbau fehlgeschlagen: </translation>
    </message>
    <message>
        <location filename="datasource.cpp" line="142"/>
        <source>Error (&apos;%1&apos;) when executing query: %2</source>
        <translation>Fehler (&apos;%1&apos;) in Abfrage: %2</translation>
    </message>
    <message>
        <location filename="datasource.cpp" line="178"/>
        <source>Cannot run command &apos;%1&apos;: %s2</source>
        <translation>Kann Kommando &apos;%1&apos; nicht ausführen: %s2</translation>
    </message>
    <message>
        <location filename="JSONReader.cpp" line="298"/>
        <location filename="datasource.cpp" line="182"/>
        <source>Running command: </source>
        <translation>Führe aus: </translation>
    </message>
    <message>
        <location filename="JSONReader.cpp" line="300"/>
        <source>Cannot run command &apos;%1&apos;: %2</source>
        <translation>Kann Kommando &apos;%1&apos; nicht ausführen: %2</translation>
    </message>
    <message>
        <location filename="JSONReader.cpp" line="306"/>
        <source>Command &apos;%1&apos; has non-zero exitcode: %s2</source>
        <translation>Kommando &apos;%1&apos; hat einen Exitcode ungleich Null: %s2</translation>
    </message>
    <message>
        <location filename="datasource.cpp" line="194"/>
        <source>Error when executing command &apos;%1&apos;: %2</source>
        <translation>Fehler bei &apos;%1&apos;: %2</translation>
    </message>
    <message>
        <location filename="datasource.cpp" line="194"/>
        <source>abnormal termination</source>
        <translation>nicht normal beendet</translation>
    </message>
    <message>
        <location filename="defaultcommentreader.cpp" line="39"/>
        <source>default comments: cannot open file &apos;%1&apos;: %2</source>
        <translation>Standardkommentare: kann Datei &apos;%1&apos; nicht öffnen: %2</translation>
    </message>
    <message>
        <location filename="defaultcommentreader.cpp" line="46"/>
        <source>default comments: </source>
        <translation>Standardkommentare: </translation>
    </message>
    <message>
        <location filename="defaultcommentreader.cpp" line="86"/>
        <source>default comments: cannot find subaccount %1</source>
        <translation>Standardkommentar: kann Unterkonto %1 nicht finden</translation>
    </message>
    <message>
        <location filename="findkontodialog.cpp" line="32"/>
        <source>All</source>
        <translation>Alle</translation>
    </message>
    <message>
        <location filename="findkontodialog.cpp" line="33"/>
        <source>Account</source>
        <translation>Konto</translation>
    </message>
    <message>
        <location filename="findkontodialog.cpp" line="34"/>
        <source>Subaccount</source>
        <translation>Unterkonto</translation>
    </message>
    <message>
        <location filename="findkontodialog.cpp" line="35"/>
        <source>Comment</source>
        <translation>Kommentar</translation>
    </message>
    <message>
        <location filename="findkontodialog.cpp" line="36"/>
        <source>Microaccount</source>
        <translation>Mikrokonto</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="99"/>
        <source>Cannot create lock %1: %2</source>
        <translation>Kann die lokale Sperre %1 nicht anlegen: %2</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="103"/>
        <source>This Program (%1) is already running (on this machine)</source>
        <translation>Das Programm „%1“ läuft bereits (auf diesem Rechner)</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="111"/>
        <source>Error when removing local lock for this program (%1)</source>
        <translation>Fehler aufgetreten beim Löschen der lokalen Sperre „%1“</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="129"/>
        <source>This Program (%1) is already running (on this machine as this user) (%2)</source>
        <translation>Das Programm „%1“ läuft bereits (auf diesem Rechner und für diesen Benutzer) (%2)</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="130"/>
        <source>This Program (%1) is already running (on this machine) (%2)</source>
        <translation>Das Programm „%1“ läuft bereits (auf diesem Rechner ) (%2)</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="131"/>
        <source>Error when locking file %1: %2</source>
        <translation>Fehler beim Sperren der Datei „%1“: %2</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="173"/>
        <source>This Program is already running on another machine (%1: %2).
</source>
        <translation>Das Programm läuft bereits auf einem anderen Rechner (%1: „%2“).
</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="175"/>
        <source>
If this is not the case and the lock file is left over from a crash on that other machine, then just once run the program again on that machine to clean up the lock file.

A potentially dangerous alternative is to remove the lock file %1 manually.</source>
        <oldsource>
If this is not the case and the lock file is left over from a crash onthat other machine, then just once run the program again on that machineto clean up the lock file.

A potentially dangerous alternative is to remove the lock file %1 manually.</oldsource>
        <translation>
Wenn das nicht so ist, sondern das Programm sich beim letzten Mal auf einem anderen Rechner nicht regulär beenden konnte, dann starten Sie bitte das Programm nochmal auf diesem anderen Rechner!

Eine gefährliche Alternative ist, die Datei „%1“  zu löschen.</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="183"/>
        <source>%1 already exists.</source>
        <translation>%1 existiert bereits.</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="187"/>
        <source>%1 could not be created: %2</source>
        <translation>%1 konnte nicht angelegt werden (%2)</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="209"/>
        <source>lock file %1 could not be opened for update: %1</source>
        <translation>Lock-Datei %1 konnte nicht zum schreiben geöffnet werden: %1</translation>
    </message>
    <message>
        <source>lock file %1 could not be opened: %1</source>
        <translation type="obsolete">Konnte das Lockfile %1 nicht öffnen (%1)</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="223"/>
        <source>lock file %1 could not be removed: %2</source>
        <translation>Konnte %1 nicht entfernen: %2</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="247"/>
        <source>lock file %1 has been changed by someone else: %2</source>
        <translation>Die Datei %1 wurde von außen verändert: %2</translation>
    </message>
    <message>
        <location filename="lock.cpp" line="251"/>
        <source>lock file %1 has been deleted by someone else.</source>
        <translation>Die Datei %1 wurde von außen gelöscht.</translation>
    </message>
    <message>
        <source>Available Options:
--configdir=DIR	location of the directory where your files will be placed
			(default: ~/.sctime)
--datasource=		(repeatable) use these data sources
			(default: &apos;QPSQL&apos;&apos;,&apos; &apos;QODBC&apos;&apos;, &apos;command&apos; and &apos;file&apos;&apos;);
			overrides &lt;backends/&gt; in settings.xml
--zeitkontenfile=FILE	read the accounts list from file FILE
			(default: output of &apos;zeitkonten&apos;). Obsolete.

--bereitschaftsfile=FILE	read the &apos;Bereitschaftsarten&apos;&apos; from file FILE
				(default: output of &apos;zeitbereitls&apos;). Obsolete.

--specialremunfile=FILE       read the types of special remunerations from file FILE
                               (default: output of &apos;sonderzeitls&apos;. Obsolete.

--offlinefile=FILE       read all needed data from FILE which must be of json format
                      overides --zeitkontenfile --bereitschaftsfile and --specialremunfile

Please see the Help menu for further information (F1)!</source>
        <oldsource>Available Options:
--configdir=DIR	location of the directory where your files will be placed
			(default: ~/.sctime)
--datasource=		(repeatable) use these data sources
			(default: &apos;QPSQL&apos;&apos;,&apos; &apos;QODBC&apos;&apos;, &apos;command&apos; and &apos;file&apos;&apos;);
			overrides &lt;backends/&gt; in settings.xml
--zeitkontenfile=FILE	read the accounts list from file FILE
			(default: output of &apos;zeitkonten&apos;.

--bereitschaftsfile=FILE	read the &apos;Bereitschaftsarten&apos;&apos; from file FILE
				(default: output of &apos;zeitbereitls&apos;.

--specialremunfile=FILE       read the types of special remunerations from file FILE
                               (default: output of &apos;sonderzeitls&apos;.

Please see the Help menu for further information (F1)!</oldsource>
        <translation type="vanished">Verfügbare Optionen:
--configdir=DIR		Verzeichnis, in dem alle Dateien abgelegt werden
			(Standard: ~/.sctime)
--datasource=		(wiederholbar) diese Datenquellen benutzen
			(Standard: &apos;QPSQL&apos;&apos;,&apos; &apos;QODBC&apos;&apos;, &apos;command&apos; und &apos;file&apos;&apos;);
			überschreibt den Wert von &lt;backends/&gt; in settings.xml
--zeitkontenfile=FILE		lese Kontenliste aus Datei FILE
			(Standard: Ausgabe des Befehls &apos;zeitkonten&apos;). Veraltet.
--bereitschaftsfile=FILE		lese Bereitschaftsarten aus Datei FILE
				(Standard: Ausgabe des Befehls &apos;zeitbereitls&apos;). Veraltet.
--specialremunfile=FILE		Liest die Kategorien von globalen Sonderzeiten aus der Datei FILE
			(Standard: Ausgabe von &apos;sonderzeitls&apos;). Veraltet.
--offlinefile=FILE		Liest alle benötigten Daten aus FILE, welches im JSON Format vorliegen muss.
			überschreibt --zeitkontenfile --bereitschaftsfile and --specialremunfile

Beachten Sie bittte die Hilfefunktion für weitere Informationen (F1)!</translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="81"/>
        <source>Available Options:
--configdir=DIR		location of the directory where your files will be placed
			(default: ~/.sctime)

--datasource=		(repeatable) use these data sources
			(default: &apos;QPSQL&apos;&apos;,&apos; &apos;QODBC&apos;&apos;, &apos;command&apos; and &apos;file&apos;&apos;);
			overrides &lt;backends/&gt; in settings.xml

--zeitkontenfile=FILE	read the accounts list from file FILE
			(default: output of &apos;zeitkonten&apos;). Obsolete.

--bereitschaftsfile=FILE	read the &apos;Bereitschaftsarten&apos;&apos; from file FILE
			(default: output of &apos;zeitbereitls&apos;). Obsolete.

--specialremunfile=FILE       read the types of special remunerations from file FILE
			(default: output of &apos;sonderzeitls&apos;. Obsolete.

--offlinefile=FILE		read all needed data from FILE which must be of json format
			overides --zeitkontenfile --bereitschaftsfile and --specialremunfile

--accountlink=URL		opens sctime with the given account selected. If an instance of sctime
			is already running, the existing instance will be used for that.

Please see the Help menu for further information (F1)!</source>
        <oldsource>Available Options:
--configdir=DIR		location of the directory where your files will be placed
			(default: ~/.sctime)

--datasource=		(repeatable) use these data sources
			(default: &apos;QPSQL&apos;&apos;,&apos; &apos;QODBC&apos;&apos;, &apos;command&apos; and &apos;file&apos;&apos;);
			overrides &lt;backends/&gt; in settings.xml

--zeitkontenfile=FILE	read the accounts list from file FILE
			(default: output of &apos;zeitkonten&apos;). Obsolete.

--bereitschaftsfile=FILE	read the &apos;Bereitschaftsarten&apos;&apos; from file FILE
			(default: output of &apos;zeitbereitls&apos;). Obsolete.

--specialremunfile=FILE       read the types of special remunerations from file FILE
			(default: output of &apos;sonderzeitls&apos;. Obsolete.

--offlinefile=FILE		read all needed data from FILE which must be of json format
			overides --zeitkontenfile --bereitschaftsfile and --specialremunfile

Please see the Help menu for further information (F1)!</oldsource>
        <translation>Verfügbare Optionen:
--configdir=DIR		Verzeichnis, in dem alle Dateien abgelegt werden
			(Standard: ~/.sctime)
--datasource=		(wiederholbar) diese Datenquellen benutzen
			(Standard: &apos;QPSQL&apos;&apos;,&apos; &apos;QODBC&apos;&apos;, &apos;command&apos; und &apos;file&apos;&apos;);
			überschreibt den Wert von &lt;backends/&gt; in settings.xml
--zeitkontenfile=FILE		lese Kontenliste aus Datei FILE
			(Standard: Ausgabe des Befehls &apos;zeitkonten&apos;). Veraltet.
--bereitschaftsfile=FILE		lese Bereitschaftsarten aus Datei FILE
				(Standard: Ausgabe des Befehls &apos;zeitbereitls&apos;). Veraltet.
--specialremunfile=FILE		Liest die Kategorien von globalen Sonderzeiten aus der Datei FILE
			(Standard: Ausgabe von &apos;sonderzeitls&apos;). Veraltet.
--offlinefile=FILE		Liest alle benötigten Daten aus FILE, welches im JSON Format vorliegen muss.
			überschreibt --zeitkontenfile --bereitschaftsfile and --specialremunfile
--accountlink=URL		Öffnet sctime mit dem übergeben Konto als Auswahl. Wenn eine sctime-Instanz
			bereits läuft, wird diese hierfür verwendet.

Beachten Sie bittte die Hilfefunktion für weitere Informationen (F1)!</translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="161"/>
        <source>Error on connecting to sctime</source>
        <translation>Fehler beim Verbinden zu sctime</translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="194"/>
        <source>Personal accounts</source>
        <translation>Persönliche Konten</translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="195"/>
        <source>All accounts</source>
        <translation>Alle Konten</translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="201"/>
        <source>sctime </source>
        <translation>sctime </translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="211"/>
        <source>directory</source>
        <translation>Verzeichnis</translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="213"/>
        <location filename="sctime.cpp" line="215"/>
        <location filename="sctime.cpp" line="217"/>
        <location filename="sctime.cpp" line="219"/>
        <location filename="sctime.cpp" line="223"/>
        <source>file</source>
        <translation>Datei</translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="221"/>
        <source>source</source>
        <translation>Quelle</translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="225"/>
        <source>link</source>
        <translation>Link</translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="250"/>
        <source>sctime: Configuration problem</source>
        <oldsource>sctime: configuration problem</oldsource>
        <translation>sctime: Konfigurationsproblem</translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="251"/>
        <source>Cannot access configration directory %1.</source>
        <oldsource>Cannot access conigration directory %1.</oldsource>
        <translation>Kann nicht auf %1 zugreifen.</translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="288"/>
        <source>sctime: Cannot start</source>
        <oldsource>sctime: cannot start</oldsource>
        <translation>sctime: kann nicht starten</translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="293"/>
        <source>Unclean state</source>
        <translation>Unklarer Zustand</translation>
    </message>
    <message>
        <location filename="sctime.cpp" line="293"/>
        <source>It looks like the last instance of sctime might have crashed, probably at %1. Please check if the recorded times of that date are correct.</source>
        <translation>Die letzte Instanz von sctime scheint abgestürzt zu sein (wahrscheinlich um %1). Bitte überprüfen Sie die aufgezeichneten Zeiten dieses Datums.</translation>
    </message>
    <message>
        <location filename="sctimexmlsettings.cpp" line="53"/>
        <source>Shell script not written because it has already been checked in.</source>
        <translation>Shell-Skript nicht geschrieben, da bereits eingecheckt.</translation>
    </message>
    <message>
        <location filename="sctimexmlsettings.cpp" line="66"/>
        <source>sctime: writing shell script</source>
        <translation>sctime: Shell-Skript schreiben</translation>
    </message>
    <message>
        <location filename="sctimexmlsettings.cpp" line="78"/>
        <source>sctime: saving sh file</source>
        <translation>SH Datei speichern</translation>
    </message>
    <message>
        <source>sctime: opening configuration file</source>
        <translation type="vanished">sctime: Konfigurationsdatei öffnen</translation>
    </message>
    <message>
        <source>%1 : %2</source>
        <translation type="vanished">%1 : %2</translation>
    </message>
    <message>
        <location filename="conflictdialog.cpp" line="91"/>
        <location filename="conflictdialog.cpp" line="145"/>
        <source>sctime: reading configuration file</source>
        <translation>sctime: Konfigurationsdatei lesen</translation>
    </message>
    <message>
        <location filename="conflictdialog.cpp" line="92"/>
        <location filename="conflictdialog.cpp" line="146"/>
        <location filename="xmlreader.cpp" line="230"/>
        <source>error in %1, line %2, column %3: %4.</source>
        <translation>Fehler in %1, Zeile %2, Spalte %3: %4.</translation>
    </message>
    <message>
        <location filename="xmlwriter.cpp" line="76"/>
        <source>zeit-DAY.sh not written because it has already been checked in</source>
        <translation>zeit-TAG.sh nicht geschrieben, da bereits eingecheckt</translation>
    </message>
    <message>
        <location filename="xmlwriter.cpp" line="448"/>
        <location filename="xmlwriter.cpp" line="457"/>
        <location filename="xmlwriter.cpp" line="479"/>
        <location filename="xmlwriter.cpp" line="492"/>
        <location filename="xmlwriter.cpp" line="503"/>
        <source>sctime: saving settings</source>
        <translation>sctime: Einstellungen speichern</translation>
    </message>
    <message>
        <location filename="xmlwriter.cpp" line="448"/>
        <source>%1 has been modified since the last changes done by this sctime instance. Do you wanto to overwrite theses changes?</source>
        <translation>Datei %1 wurde verändert seit der letzten Änderung durch sctime. Sollen diese Änderungen überschrieben werden?</translation>
    </message>
    <message>
        <location filename="xmlwriter.cpp" line="457"/>
        <source>opening file %1 for writing failed. Please make sure the sctime settings directory is available. Details: %2</source>
        <translation>Datei %1 konnte nicht zum Schreiben geöffnet werden. Bitte stellen sie sicher dass das sctime Einstellungsverzeichnis verfügbar ist. Details: %2</translation>
    </message>
    <message>
        <location filename="xmlwriter.cpp" line="480"/>
        <source>%1 cannot be copied to %2: %3</source>
        <translation>Kann nicht %1 kopieren nach %2: %3</translation>
    </message>
    <message>
        <location filename="sctimexmlsettings.cpp" line="79"/>
        <location filename="xmlwriter.cpp" line="493"/>
        <location filename="xmlwriter.cpp" line="504"/>
        <source>%1 cannot be renamed to %2: %3</source>
        <translation>Kann nicht %1 umbenennen zu %2: %3</translation>
    </message>
    <message>
        <location filename="setupdsm.cpp" line="99"/>
        <source>user name cannot be determined.</source>
        <translation>Der Benutzername kann nicht festgestellt werden.</translation>
    </message>
    <message>
        <location filename="setupdsm.cpp" line="120"/>
        <source>Error when reading from file %1: %2</source>
        <translation>Beim Lesen aus Datei %1: %2</translation>
    </message>
    <message>
        <location filename="setupdsm.cpp" line="240"/>
        <source>Accounts</source>
        <translation>Konten</translation>
    </message>
    <message>
        <location filename="setupdsm.cpp" line="241"/>
        <source>On-call categories</source>
        <translation>Bereitschaftsarten</translation>
    </message>
    <message>
        <location filename="setupdsm.cpp" line="242"/>
        <source>Special Remunerations</source>
        <translation>Sonderzeiten</translation>
    </message>
    <message>
        <location filename="setupdsm.cpp" line="143"/>
        <source>available database drivers: %1.</source>
        <translation>verfügbare Datenbanktreiber: %1.</translation>
    </message>
    <message>
        <location filename="setupdsm.cpp" line="153"/>
        <source>adding jsonreader: %1.</source>
        <translation>Füge JsonReader hinzu: %1.</translation>
    </message>
    <message>
        <location filename="setupdsm.cpp" line="179"/>
        <source>data source &apos;command&apos; is not available on Windows</source>
        <translation>Datenquelle &apos;command&apos; ist unter Windows nicht verfügbar</translation>
    </message>
    <message>
        <location filename="setupdsm.cpp" line="198"/>
        <source>database driver or data source not available: </source>
        <translation>Datenbanktreiber oder Datenquelle nicht verfügbar: </translation>
    </message>
    <message>
        <location filename="setupdsm.cpp" line="203"/>
        <source>data source &apos;%1&apos; not working: %2</source>
        <translation>data translation &apos;%1&apos;not working: %2</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="106"/>
        <source>-- Start --</source>
        <translation>-- Start --</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="647"/>
        <source>sctime: accounting stopped</source>
        <translation>sctime: Zeiterfassung gestoppt</translation>
    </message>
    <message>
        <source>Couldn&apos;t open json file %1.</source>
        <oldsource>Couldn&apos;t open json file %1</oldsource>
        <translation type="vanished">Konnte JSON-Datei nicht öffnen.</translation>
    </message>
    <message>
        <source>Week %1</source>
        <translation type="obsolete">Woche %1</translation>
    </message>
    <message>
        <location filename="punchclockchecker.cpp" line="122"/>
        <source>You are working for 6 hours without a longer break. You should take a break of at least %1 minutes now.</source>
        <oldsource>You are working for 6 hours without a longer break. You should take a break of at least 15 minutes now.</oldsource>
        <translation>Du hast 6 Stunden am Stück ohne längere Pause gearbeitet. Bitte mache jetzt eine Pause von mindestens %1 Minuten.</translation>
    </message>
    <message>
        <location filename="punchclockchecker.cpp" line="126"/>
        <source>You are working for 6 hours without many breaks. You should take an additional break of at least %1 minutes now.</source>
        <oldsource>You are working for 6 hours without many breaks. You should take an additional break of at least 15 minutes in the next three hours.</oldsource>
        <translation>Du hast 6 Stunden ohne ausreichend Pausen gearbeitet. Bitte mache jetzt eine Pause von mindestens %1 Minuten.</translation>
    </message>
    <message>
        <location filename="punchclockchecker.cpp" line="130"/>
        <source>You are working for 9 hours without enough breaks. You should take a break of at least %1 minutes now.</source>
        <translation>Du hast 9 Stunden ohne ausreichend Pausen gearbeitet. Bitte mache jetzt eine Pause von mindestens %1 Minuten.</translation>
    </message>
    <message>
        <location filename="punchclockchecker.cpp" line="134"/>
        <source>You are working for more than 10 hours on this workday. You should take a break of at least 11 hours now.</source>
        <translation>Du arbeitest mehr als 10 Stunden an diesem Arbeitstag. Du solltest die Arbeit jetzt für mindestens 11 Stunden unterbrechen.</translation>
    </message>
    <message>
        <location filename="punchclockdialog.cpp" line="41"/>
        <source>Begin time</source>
        <translation>Startzeit</translation>
    </message>
    <message>
        <location filename="punchclockdialog.cpp" line="41"/>
        <source>End time</source>
        <translation>Endzeit</translation>
    </message>
    <message>
        <location filename="deletesettingsdialog.cpp" line="48"/>
        <source>sctime: deleting data</source>
        <translation>sctime: lösche Daten</translation>
    </message>
    <message>
        <location filename="deletesettingsdialog.cpp" line="100"/>
        <source>sctime: error on deleting file on server</source>
        <translation>sctime: Fehler beim löschen von Server-Datei</translation>
    </message>
</context>
<context>
    <name>SpecialRemunEntryHelper</name>
    <message>
        <location filename="specialremunentryhelper.cpp" line="36"/>
        <source>sctime: wrong special remunerations</source>
        <translation>sctime: abweichende Sonderzeitmarkierungen</translation>
    </message>
    <message>
        <location filename="specialremunentryhelper.cpp" line="37"/>
        <source>There is another entry with the same comment and the correct special remunerations. Do you want to switch to this entry? Otherwise a new entry will be created.</source>
        <translation>Es gibt einen anderen Zeiteintrag mit dem gleichen Kommentar und den korrekten Sonderzeitmarkierungen. Soll zu diesem Zeiteintrag gewechselt werden? Andernfalls wird ein neuer Zeiteintrag erzeugt.</translation>
    </message>
</context>
<context>
    <name>SpecialRemunerationDialogBase</name>
    <message>
        <location filename="specialremunerationdialogbase.ui" line="14"/>
        <source>Please choose applicable type of special remuneration</source>
        <translation>Bitte wählen Sie die gewünschten Sonderzeitkategorien</translation>
    </message>
</context>
<context>
    <name>SpecialRemunerationsDialog</name>
    <message>
        <location filename="specialremunerationsdialog.cpp" line="35"/>
        <source>sctime: Special Remuneration times</source>
        <translation>sctime: Sonderzeiten</translation>
    </message>
    <message>
        <location filename="specialremunerationsdialog.cpp" line="35"/>
        <source>No subaccount selected!</source>
        <translation>Kein Unterkonto ausgewählt!</translation>
    </message>
</context>
<context>
    <name>StatusBar</name>
    <message>
        <location filename="statusbar.cpp" line="31"/>
        <location filename="statusbar.cpp" line="58"/>
        <source>Overall time: </source>
        <translation>Gesamtzeit: </translation>
    </message>
    <message>
        <location filename="statusbar.cpp" line="71"/>
        <source>Warning: Non-current date %1 is being edited.</source>
        <oldsource>Warning: Non-current date </oldsource>
        <translation>Warnung: Es wird der %1 editiert, nicht das aktuelle Datum.</translation>
    </message>
    <message>
        <source> is being edited.</source>
        <translation type="obsolete"> editiert.</translation>
    </message>
</context>
<context>
    <name>TextViewerDialog</name>
    <message>
        <location filename="textviewerdialog.cpp" line="35"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
</context>
<context>
    <name>TimeMainWindow</name>
    <message>
        <location filename="timemainwindow.cpp" line="141"/>
        <source>sctime</source>
        <translation>sctime</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="168"/>
        <source>Main toolbar</source>
        <translation>Main ToolBar</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="170"/>
        <source>&amp;Account</source>
        <translation>&amp;Konto</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="171"/>
        <source>&amp;Time</source>
        <translation>&amp;Zeit</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="172"/>
        <source>&amp;Remuneration</source>
        <translation>Ve&amp;rgütung</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="173"/>
        <source>&amp;Settings</source>
        <translation>&amp;Einstellungen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="174"/>
        <source>&amp;Help</source>
        <translation>&amp;Hilfe</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="180"/>
        <source>&amp;Pause</source>
        <translation>&amp;Pause</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="185"/>
        <source>Pause &amp;accountable time</source>
        <oldsource>Pause counting &amp;accountable time</oldsource>
        <translation>Pause der &amp;abzur. Zeit</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="216"/>
        <source>Re&amp;set difference</source>
        <translation type="unfinished">Differenz auf &amp;Null</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="301"/>
        <source>Import</source>
        <translation type="unfinished">Importieren</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="304"/>
        <source>Delete settings files</source>
        <translation>Konfigurationsdaten löschen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="310"/>
        <source>Pause only tracking of accountable time</source>
        <oldsource>Pause only counting of accountable time</oldsource>
        <translation>Hält nur die Uhr für die abzurechnende Zeit an</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="190"/>
        <source>&amp;Save</source>
        <translation>&amp;Speichern</translation>
    </message>
    <message>
        <source>&amp;Copy</source>
        <translation type="obsolete">&amp;Copy</translation>
    </message>
    <message>
        <source>Copy name to clipboard</source>
        <translation type="obsolete">Name ins Clipboard kopieren</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="194"/>
        <source>&amp;Copy as text</source>
        <translation>Als &amp;Text kopieren</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="311"/>
        <source>Copy infos about account and entry as text to clipboard</source>
        <translation>Informationen zum Konto und Eintrag als Text in die Zwischenablage kopieren</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="198"/>
        <source>Copy as &amp;link</source>
        <translation>Als &amp;Link kopieren</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="312"/>
        <source>Copy infos about account and entry as a link to clipboard</source>
        <translation>Informationen zum Konto als Link in die Zwischenablage kopieren</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="202"/>
        <source>Paste link</source>
        <translation>Link öffnen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="313"/>
        <source>Open account from link from clipboard</source>
        <oldsource>Copy infos about account and entry from clipboard</oldsource>
        <translation>Öffne Konto über Zwischenablage-Link</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="206"/>
        <source>C&amp;hoose Date...</source>
        <translation>&amp;Datum wählen...</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="211"/>
        <source>Punch Clock</source>
        <translation>Anwesenheitszeiten</translation>
    </message>
    <message>
        <source>&amp;Set accountable equal worked</source>
        <translation type="vanished">Differenz auf &amp;Null</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="314"/>
        <source>Set active account&apos;s accountable time equal worked time</source>
        <translation>Beim gewählten Unterkonto die abzurechnenden auf die geleisteten Stunden setzen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="220"/>
        <source>Select as personal &amp;account</source>
        <translation>In persönliche &amp;Konten</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="225"/>
        <source>&amp;Quit</source>
        <translation>&amp;Beenden</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="315"/>
        <source>Quit program</source>
        <translation>Programm beenden</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="236"/>
        <source>&amp;Search account...</source>
        <translation>Konto s&amp;uchen...</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="241"/>
        <source>&amp;Reread account list</source>
        <translation>&amp;Kontoliste neu laden</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="245"/>
        <source>&amp;Settings...</source>
        <translation>&amp;Einstellungen...</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="249"/>
        <source>&amp;Manual...</source>
        <translation>&amp;Anleitung...</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="253"/>
        <source>&amp;About sctime...</source>
        <translation>&amp;Über sctime...</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="316"/>
        <source>About sctime...</source>
        <translation>Über sctime...</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="257"/>
        <source>About &amp;Qt...</source>
        <translation>Über &amp;Qt...</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="261"/>
        <source>Additional &amp;License Information...</source>
        <translation>Weitere &amp;Lizenz Information</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="264"/>
        <source>&amp;Messages...</source>
        <translation>&amp;Meldungen...</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="267"/>
        <source>&amp;Edit...</source>
        <translation>&amp;Editieren...</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="317"/>
        <source>Edit subaccount</source>
        <translation>Unterkonto editieren</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="271"/>
        <source>&amp;Activate entry</source>
        <translation>Eintrag a&amp;ktivieren</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="276"/>
        <source>Add &amp;entry</source>
        <translation>Eintrag &amp;hinzufügen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="280"/>
        <source>&amp;Delete entry</source>
        <translation>Eintrag &amp;löschen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="285"/>
        <source>Set &amp;on-call times...</source>
        <translation>Be&amp;reitschaftszeiten setzen...</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="290"/>
        <source>Set special remuneration &amp;times...</source>
        <translation>Setze Sonderzei&amp;t Kategorien...</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="294"/>
        <source>Choose &amp;background colour...</source>
        <translation>&amp;Hintergrundfarbe wählen...</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="296"/>
        <source>&amp;Remove background colour</source>
        <translation>Hintergrun&amp;dfarbe entfernen</translation>
    </message>
    <message>
        <source>&amp;Show selected account in &apos;all accounts&apos;</source>
        <translation type="vanished">&amp;Zu selektiertem Konto in &apos;Alle Konten&apos; springen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="321"/>
        <source>Increase time</source>
        <translation>Zeit erhöhen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="323"/>
        <source>Decrease time</source>
        <translation>Zeit verringern</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="509"/>
        <source>Minimal decrease time</source>
        <translation>Zeit minimal veringern</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="326"/>
        <source>Increase time fast</source>
        <translation>Zeit schnell erhöhen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="298"/>
        <source>Download sh files</source>
        <translation>SH Dateien herunterladen</translation>
    </message>
    <message>
        <source>Import settings</source>
        <translation type="vanished">Einstellungen importieren</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="307"/>
        <source>S&amp;how selected account in &apos;all accounts&apos;</source>
        <translation>Ausgewä&amp;hltes Konto in &apos;Alle Konten&apos; zeigen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="328"/>
        <source>Decrease time fast</source>
        <translation>Zeit schnell verringern</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="331"/>
        <source>Increase accountable time</source>
        <translation>Abrechenbare Zeit erhöhen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="333"/>
        <source>Decrease accountable time</source>
        <translation>Abrechenbare Zeit verringern</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="336"/>
        <source>Increase accountable time fast</source>
        <translation>Abrechenbare Zeit schnell erhöhen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="338"/>
        <source>Decrease accountable time fast</source>
        <translation>Abrechenbare Zeit schnell verringern</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="340"/>
        <source>Toggle regulated overtime mode</source>
        <translation>Modus für regulierte Mehrarbeit aktivieren</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="343"/>
        <source>Toggle other overtime mode</source>
        <translation>Modus für sonstige Mehrarbeit aktivieren</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="347"/>
        <source>Toggle night mode</source>
        <translation>Nachtmodus aktivieren</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="351"/>
        <source>Toggle public holiday mode</source>
        <translation>Feiertagsmodus aktivieren</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="574"/>
        <source>cannot start ipc server</source>
        <translation>Kann IPC-Server nicht starten</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="648"/>
        <source>The last active account was %1/%2. It seems to have been closed or renamed. Please activate a new account to start time accounting!</source>
        <translation>Ihr zuletzt aktives Konto war %1/%2. Wahrscheinlich wurde es geschlossen oder umbenannt. Bitte wählen Sie nun ein neues Konto aus, damit die Zeiterfassung beginnt!</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="677"/>
        <source>Power Buttons</source>
        <translation>Power Buttons</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="724"/>
        <source>suspend</source>
        <translation>Anhalten</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="735"/>
        <source>resume</source>
        <translation>Fortsetzen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="737"/>
        <source>resume %2; suspend was %1</source>
        <translation>Fortsetzen %2; Angehalten bei %1</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="743"/>
        <location filename="timemainwindow.cpp" line="751"/>
        <source>sctime: resume</source>
        <translation>sctime: fortsetzen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="744"/>
        <source>The machine was suspended from %1 until %2. Please check and adjust accounted time if necessary!</source>
        <translation>Der Rechner war von %1 bis %2 angehalten. Bitte die Arbeitszeiten gegebenenfalls überarbeiten!</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="752"/>
        <source>The machine was suspended from %1 until %2. Should this time be added to the active account?</source>
        <translation>Der Rechner war von %1 bis %2 angehalten. Soll diese Zeit auf dem aktiven Konto gutgeschrieben werden?</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="785"/>
        <source>Drift is %2s (%1)</source>
        <translation>Drift ist %2s (%1)</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="797"/>
        <source>sctime: Programm was frozen</source>
        <translation>sctime: Programm war stehen geblieben</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="798"/>
        <source>The program (or whole system) seems to have hung for %1min or system time was changed.
Should the time difference be added to the active account?
(current system time: %2)</source>
        <translation>Das Programm oder das System scheint %1min stehen geblieben zu sein. Möglicherweise wurde auch die Systemzeit vorgestellt.
Soll die entstandene Differenz auf das aktive Unterkonto gutschrieben werden?
(Aktuelle Systemzeit: %2)</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="803"/>
        <source>sctime: system time set back</source>
        <translation>sctime: Systemzeit zurückgestellt</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="804"/>
        <source>The system&apos;s time has been set back by %1min to %2.Should this time be subtracted from the active account?
</source>
        <translation>Die Systemzeit wurde um %1min auf %2 zurückgestellt. Soll die Arbeitszeit auf dem aktiven Unterkonto um diesen Betrag verringert werden?</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="851"/>
        <source>Minute-signal %1s arrived late (%2)</source>
        <translation>Minuten-Signal %1s verspätet (%2)</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1033"/>
        <location filename="timemainwindow.cpp" line="1356"/>
        <location filename="timemainwindow.cpp" line="2240"/>
        <source>Warning</source>
        <translation>Warnung</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2075"/>
        <source>&lt;h1&gt;&lt;img src=&apos;:/scLogo_15Farben&apos; /&gt;sctime&lt;/h1&gt;&lt;table&gt;&lt;tr&gt;&lt;td&gt;Version:&lt;/td&gt;&lt;td&gt;%1&lt;/td&gt;&lt;/tr&gt;%4&lt;tr&gt;&lt;td&gt;Qt-Version:&lt;/td&gt;&lt;td&gt;%2 (development)&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;/td&gt;&lt;td&gt;%3 (runtime)&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Developers:&lt;/td&gt;&lt;td&gt;Johannes Abt, Alexander Wuetz, Florian Schmitt&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Patches:&lt;/td&gt;&lt;td&gt;Marcus Camen&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Mac:&lt;/td&gt;&lt;td&gt;Michael Weiser&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;New icons:&lt;/td&gt;&lt;td&gt;Mayra Delgado&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;RT:&lt;/td&gt;&lt;td&gt;&lt;a href=&apos;mailto:zeittools-rt@science-computing.de&apos;&gt;zeittools-rt@science-computing.de&lt;/a&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Project page:&lt;/td&gt;&lt;td&gt;&lt;a href=&apos;http://github.com/scVENUS/sctime/&apos;&gt;http://github.com/scVENUS/sctime/&lt;/a&gt;&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;p&gt;This program is licensed under the GNU Public License v3.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2580"/>
        <source>permanently offline</source>
        <translation>dauerhaft offline</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2583"/>
        <source>offline</source>
        <translation>offline</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2585"/>
        <source>online</source>
        <translation>online</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2608"/>
        <source>sctime: invalid session</source>
        <translation>sctime: ungültige Session</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2609"/>
        <source>Your session seems to be invalid. Please confirm to open a new window to refresh it. Please provide your credentials there if your browser asks for them.</source>
        <translation>Deine Session scheint ungültig zu sein. Bitte bestätige, um ein neues Fenster zu öffnen in dem sie erneuert werden kann. Bitte gib dort Deine Login-Daten an, falls der Browser sie benötigt.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2614"/>
        <source>Refresh Session</source>
        <translation>Session erneuern</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2642"/>
        <location filename="timemainwindow.cpp" line="2682"/>
        <location filename="timemainwindow.cpp" line="2726"/>
        <source>sctime: unresolvable conflict</source>
        <translation>sctime: unlösbarer Konflikt</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2643"/>
        <location filename="timemainwindow.cpp" line="2683"/>
        <location filename="timemainwindow.cpp" line="2727"/>
        <source>There seems to be a conflict with another session that could not be resolved. Please check your entries.</source>
        <translation>Es scheint ein Konflikt mit einer anderen Session zu bestehen, der nicht aufgelöst werden konnte, Bitte prüfe Deine Zeiteinträge.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2738"/>
        <source>sctime: automatically resolved conflict</source>
        <translation>sctime: automatisch aufgelöster Konflikt</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2739"/>
        <source>There was a conflict between local and remote data when loading. sctime has automatically loaded the newer data. Please check your entries.</source>
        <translation>Es bestand ein Konflikt zwischen lokalen und remote Daten beim laden. sctime hat automatisch die neueren Daten geladen. Bitte prüfe Deine Einträge.</translation>
    </message>
    <message>
        <source>Warning: Legally allowed working time has been exceeded.</source>
        <translation type="vanished">Warnung: die gesetzlich zulässige Arbeitszeit wurde überschritten.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1171"/>
        <source>An error occured when saving data. Please check permissions and connectivity of your target directory. If this error persists and you close sctime, you will loose all changes since the last successful save (an automatic save should occur every 5 minutes).</source>
        <translation>Ein Fehler trat beim Speichern auf. Bitte überprüfen sie Berechtigungen und Verbindung zum Zielverzeichnis. Wenn dieser Fehler weiter besteht und Sie sctime beenden, gehen alle Änderungen seit der letzten erfolgreichen Speicherung verloren (ein automatisches Speichern findet alle 5 Minuten statt).</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1175"/>
        <source>An error occured when saving data.</source>
        <translation>Ein Fehler trat beim speichern von Daten auf.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1202"/>
        <source>Unclear state of Lockfile. Please check that there is no other instance of sctime running and that you have access to the sctime config directory. Otherwise loss of data may occur.</source>
        <oldsource>Unclear state of Logfile. Please check that there is no other instance of sctime running and that you have access to the sctime config directory. Otherwise loss of data may occur.</oldsource>
        <translation>Uneindeutiger Zustand des Lockfiles. Bitte prüfen Sie dass keine anderen Instanzen von sctime gestartet sind, und dass das sctime-Konfigurationsverzeichnis erreichbar ist. Andernfalls kann Datenverlust auftreten.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1206"/>
        <source>Unkown state of lockfile.</source>
        <oldsource>Unkown state of logfile.</oldsource>
        <translation>Unklarer Zustand des Lockfiles.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1391"/>
        <source>Could not switch day due to problems with saving. ATTENTION: that also means that the clock might be running on the wrong day. Please fix the problem with saving and switch manually to the current date afterwards.</source>
        <translation>Konnte den Tag nicht ändern, da ein Fehler beim Speichern auftrat. ACHTUNG: das kann bedeuten, dass die Uhr für den falschen Tag läuft. Bitte beheben Sie das Speicherproblem und wechseln Sie danach manuell auf das heutige Datum.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1457"/>
        <source>Visible day set to: </source>
        <translation>Der sichtbare tag wurde gesetzt auf:</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1461"/>
        <source>Today is now: </source>
        <translation>Heute ist jetzt:</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1496"/>
        <source>Commiting account list...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1568"/>
        <source>Remove from personal accounts</source>
        <translation>Aus persönlichen Konten entfernen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1569"/>
        <source>Do you really want to remove this item from your personal accounts?</source>
        <translation>Möchten Sie das gewählte Element wirklich aus Ihren persönlichen Konten entfernen?</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1667"/>
        <source>Do you also want to move the begin of the current working intervall by %1 minutes?</source>
        <translation>Möchtest Du auch den Start des aktuellen Arbeitsintervalls um %1 Minuten vorverschieben?</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1671"/>
        <source>Do you also want to add a pause of %1 minutes at the end of the current working interval?</source>
        <translation>Möchtest Du auch eine Pause von %1 Minuten zum Ende des aktuellen Arbeitsintervalls einfügen?</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1675"/>
        <source>Also adapt punch clock?</source>
        <translation>Anwesenheitszeiten ebenfalls anpassen?</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1767"/>
        <source>Account menu</source>
        <translation>Kontomenü</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1989"/>
        <source>Please specify only one entry for accounts of type &quot;%1&quot;!</source>
        <translation>Bitte nur einen Eintrag für Konten des Typs „%1”!</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2296"/>
        <source>sctime: Additional Information about Licensing</source>
        <translation>sctime: Weitere Lizenz Information</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2296"/>
        <source>sctime licensing</source>
        <translation>sctime Lizenz</translation>
    </message>
    <message>
        <source>sctime: wrong special remunerations</source>
        <translation type="vanished">sctime: abweichende Sonderzeitmarkierungen</translation>
    </message>
    <message>
        <source>There is another entry with the same comment and the correct special remunerations. Do you want to switch to this entry? Otherwise a new entry will be created.</source>
        <translation type="vanished">Es gibt einen anderen Zeiteintrag mit dem gleichen Kommentar und den korrekten Sonderzeitmarkierungen. Soll zu diesem Zeiteintrag gewechselt werden? Andernfalls wird ein neuer Zeiteintrag erzeugt.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2348"/>
        <location filename="timemainwindow.cpp" line="2363"/>
        <source>Unregulated OT</source>
        <translation>Sonstige MA</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2350"/>
        <location filename="timemainwindow.cpp" line="2361"/>
        <source>Regulated OT</source>
        <translation>Regulierte MA</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2371"/>
        <source>Holiday</source>
        <translation>Feiertag</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2379"/>
        <source>Night</source>
        <translation>Nacht</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2402"/>
        <source>sctime: switch nightmode on?</source>
        <translation>sctime: Nachtmodus einschalten?</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2403"/>
        <source>It is %1. Should I switch to night mode, so you get special remuneration for working late? Please also check your companies regulations before enabling nightmode.</source>
        <oldsource>It is late. Should I switch to night mode, so you get special remuneration for working late? Please also check your companies regulations before enabling nightmode</oldsource>
        <translation>Es ist %1. Soll ich den Nachtmodus einschalten, und damit Vergütung für Sonderzeit beantragen? Bitte prüfen Sie auch die gültigen Regelungen des Unternehmens, bevor Sie den Nachtmodus aktiveren.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2409"/>
        <source>sctime: switch nightmode off?</source>
        <translation>sctime: Nachtmodus ausschalten?</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2410"/>
        <source>It is %1. Should I switch night mode off? Otherwise you apply for further special remuneration. Please also check your companies regulations when keeping nightmode enabled.</source>
        <oldsource>Night has passed. Should I switch night mode off? Otherwise you apply for further special remuneration. Please also check your companies regulations when keeping nightmode enabled.</oldsource>
        <translation>Es ist %1. Soll der Nachtmodus abgeschaltet werden? Andernfalls wird weiterhin Vergütung für Sonderzeit beantragt. Bitte prüfen Sie auch die gültigen Regelungen des Unternehmens, wenn Sie den Nachtmodus aktiv lassen.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2436"/>
        <source>sctime: move worked time to new entry</source>
        <translation>sctime: geleistete Zeit zu neuem Eintrag verschieben</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2437"/>
        <source>Should %1 minutes be moved to the new selected entry?</source>
        <translation>Sollen %1 Minuten zum neu ausgewählten Zeitrag verschoben werden?</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2470"/>
        <source>sctime: could not move worked time to new entry</source>
        <translation>sctime: kann geleistete Zeit nicht zu neuem Eintrag verschieben</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2471"/>
        <source>A date change has occurrred - therefore %1 minutes of work time won&apos;t be moved automatically to the new entry. Please check your entries manually.</source>
        <translation>Ein Datumswechsel ist aufgetreten - deshalb können die %1 Minuten an geleisteter Zeit nicht automatisch auf den neuen Zeiteintrag verschoben werden. Bitte korrigieren Sie Ihre Zeiteinträge manuell.</translation>
    </message>
    <message>
        <source>: will be ignored (%1)</source>
        <translation type="obsolete">: wird ignoriert (%1)</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1049"/>
        <source>%1: Accounting stopped (%2, +%3s)</source>
        <translation>%1: Erfassung angehalten (%2, +%3s)</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1060"/>
        <source>Pause</source>
        <translation>Pause</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1062"/>
        <source>ERROR: seconds since tick: %1</source>
        <translation>Fehler: Sekunden seit Tick: %1</translation>
    </message>
    <message>
        <source>sctime: Pause</source>
        <translation type="vanished">sctime: Pause</translation>
    </message>
    <message>
        <source>Accounting has been stopped at %1. Resume work with OK.</source>
        <translation type="vanished">Die Zeiterfassung wurde um %1 Uhr angehalten. Ende der Pause mit OK.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1086"/>
        <source>End of break: </source>
        <translation>Ende der Pause: </translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1190"/>
        <source>The program will quit in a few seconds without saving.</source>
        <translation>Das Programm beendet sich in wenigen Sekunden ohne zu speichern.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1193"/>
        <source>The program will now quit without saving.</source>
        <translation>Das Programm beendet sich ohne zu speichern.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1356"/>
        <source>Cannot delete active entry</source>
        <translation>Kann aktiven Eintrag nicht löschen</translation>
    </message>
    <message>
        <source>Day set to: </source>
        <translation type="vanished">Tag gesetzt auf: </translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1468"/>
        <source> -- This day has already been checked in!</source>
        <translation> -- Dieser Tag ist bereits eingecheckt!</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1488"/>
        <source>Reading account list...</source>
        <translation>Kontenliste laden...</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1513"/>
        <source>Account list successfully read.</source>
        <translation>Kontenliste geladen.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="1650"/>
        <source>sctime - </source>
        <translation>sctime - </translation>
    </message>
    <message>
        <source>Bitte nur einen Eintrag für Konten des Typs „%1”!</source>
        <translation type="obsolete">Bitte nur einen Eintrag für Konten des Typs „%1”!</translation>
    </message>
    <message>
        <source>OK</source>
        <translation type="vanished">OK</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2061"/>
        <source>sctime: Help</source>
        <translation>sctime: Hilfe</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2061"/>
        <source>sctime help</source>
        <translation>sctime help</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2069"/>
        <source>About sctime</source>
        <translation>Über sctime</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2069"/>
        <source>sctime about</source>
        <translation>sctime about</translation>
    </message>
    <message>
        <source>&lt;h1&gt;&lt;img src=&apos;:/scLogo_15Farben&apos; /&gt;sctime&lt;/h1&gt;&lt;table&gt;&lt;tr&gt;&lt;td&gt;Version:&lt;/td&gt;&lt;td&gt;%1&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Qt-Version:&lt;/td&gt;&lt;td&gt;%2 (development)&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;/td&gt;&lt;td&gt;%3 (runtime)&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Developers:&lt;/td&gt;&lt;td&gt;Johannes Abt, Alexander Wütz, Florian Schmitt&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Patches:&lt;/td&gt;&lt;td&gt;Marcus Camen&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Mac:&lt;/td&gt;&lt;td&gt;Michael Weiser&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;New icons:&lt;/td&gt;&lt;td&gt;Mayra Delgado&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;RT:&lt;/td&gt;&lt;td&gt;&lt;a href=&apos;mailto:zeittools-rt@science-computing.de&apos;&gt;zeittools-rt@science-computing.de&lt;/a&gt;&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Project page:&lt;/td&gt;&lt;td&gt;&lt;a href=&apos;http://sourceforge.net/projects/sctime/&apos;&gt;http://sourceforge.net/projects/sctime/&lt;/a&gt;&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;p&gt;This program is licensed under the GNU Public License v2.&lt;/p&gt;</source>
        <oldsource>&lt;h1&gt;&lt;img src=&apos;:/scLogo_15Farben&apos; /&gt;sctime&lt;/h1&gt;&lt;table&gt;&lt;tr&gt;&lt;td&gt;Version:&lt;/td&gt;&lt;td&gt;%1&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Qt-Version:&lt;/td&gt;&lt;td&gt;%2 (development)&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;/td&gt;&lt;td&gt;%3 (runtime)&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Developers:&lt;/td&gt;&lt;td&gt;Johannes Abt, Alexander Wütz, Florian Schmitt&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Patches:&lt;/td&gt;&lt;td&gt;Marcus Camen&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Mac:&lt;/td&gt;&lt;td&gt;Michael Weiser&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;New icons:&lt;/td&gt;&lt;td&gt;Mayra Delgado&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Project page:&lt;/td&gt;&lt;td&gt;&lt;a href=&apos;http://sourceforge.net/projects/sctime/&apos;&gt;http://sourceforge.net/projects/sctime/&lt;/a&gt;&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;p&gt;This program is licensed under the GNU Public License v2.&lt;/p&gt;</oldsource>
        <translation type="obsolete">&lt;h1&gt;&lt;img src=&apos;:/scLogo_15Farben&apos; /&gt;sctime&lt;/h1&gt;&lt;table&gt;&lt;tr&gt;&lt;td&gt;Version:&lt;/td&gt;&lt;td&gt;%1&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Qt-Version:&lt;/td&gt;&lt;td&gt;%2 (Entwicklung)&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;&lt;/td&gt;&lt;td&gt;%3 (Laufzeit)&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Entwickler:&lt;/td&gt;&lt;td&gt;Johannes Abt, Alexander Wütz, Florian Schmitt&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Patches:&lt;/td&gt;&lt;td&gt;Marcus Camen&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Mac:&lt;/td&gt;&lt;td&gt;Michael Weiser&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Neue Icons:&lt;/td&gt;&lt;td&gt;Mayra Delgado&lt;/td&gt;&lt;/tr&gt;&lt;tr&gt;&lt;td&gt;Projektseite:&lt;/td&gt;&lt;td&gt;&lt;a href=&apos;http://sourceforge.net/projects/sctime/&apos;&gt;http://sourceforge.net/projects/sctime/&lt;/a&gt;&lt;/td&gt;&lt;/tr&gt;&lt;/table&gt;&lt;p&gt;Dieses Programm ist unter der GNU Public License v2 lizenziert.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2092"/>
        <source>sctime: Messages</source>
        <translation>sctime: Meldungen</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2092"/>
        <source>sctime message log</source>
        <translation>sctime message log</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2108"/>
        <location filename="timemainwindow.cpp" line="2134"/>
        <source>sctime: On-call times</source>
        <translation>sctime: Bereitschaftszeiten</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2108"/>
        <location filename="timemainwindow.cpp" line="2134"/>
        <source>subaccount not found!</source>
        <translation>Unterkonto nicht gefunden!</translation>
    </message>
    <message>
        <source>On-call times</source>
        <translation type="vanished">Bereitschaftszeiten</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation type="vanished">Abbruch</translation>
    </message>
    <message>
        <source>Please choose the rendered on-call times for this subaccount.</source>
        <translation type="vanished">Bitte wählen Sie die geleisteten Bereitschaften für dieses Unterkonto aus.</translation>
    </message>
    <message>
        <location filename="timemainwindow.cpp" line="2241"/>
        <source>Warning: The entered comment contains a character that is not part of ISO-8859-1 and might not render correctly on some platforms. This may cause problems with custom reporting scripts.</source>
        <translation>Warnung: In dem von Ihnen eingegebenen Kommentar kommt ein Zeichen vor, das mit ISO-8859-1 und somit auf manchen Plattformen nicht darstellbar ist. Dies führt eventuell zu Problemen mit Auswerteskripten.</translation>
    </message>
</context>
<context>
    <name>UnterKontoDialog</name>
    <message>
        <location filename="unterkontodialog.cpp" line="53"/>
        <source>Settings for subaccount</source>
        <translation>Einstellungen für Unterkonto</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="65"/>
        <source>sctime: Settings of subaccount</source>
        <translation>sctime: Einstellungen des Unterkontos</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="65"/>
        <source>Subaccount not found!</source>
        <translation>Unterkonto nicht gefunden!</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="79"/>
        <source>Cancel</source>
        <translation>Abbruch</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="82"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="113"/>
        <source>Comment</source>
        <translation>Kommentar</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="124"/>
        <source>Tags</source>
        <translation>Tags</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="130"/>
        <source>Add</source>
        <translation>Hinzufügen</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="138"/>
        <source>Time</source>
        <oldsource>Time:</oldsource>
        <translation>Zeit</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="144"/>
        <source>Accountable time</source>
        <oldsource>Accountable time:</oldsource>
        <translation>Abzurechnende Zeit</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="151"/>
        <source>Special Remuneration Categories</source>
        <translation>Sonderzeitkategorien</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="166"/>
        <source>Description: </source>
        <translation>Beschreibung: </translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="172"/>
        <source>Responsible: </source>
        <translation>Verantwortlich: </translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="186"/>
        <source>Activate entry</source>
        <translation>Eintrag aktivieren</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="191"/>
        <source>Select as personal account</source>
        <translation>In die persönlichen Konten übernehmen</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="320"/>
        <source>Error</source>
        <translation>Fehler</translation>
    </message>
    <message>
        <location filename="unterkontodialog.cpp" line="320"/>
        <source>Error: The entered description contains a character that cannot be displayed in your locale.</source>
        <translation>Fehler: In dem von Ihnen eingegebenen Kommentar kommt ein Zeichen vor, das in Ihrem Locale nicht darstellbar ist. </translation>
    </message>
</context>
<context>
    <name>XMLReader</name>
    <message>
        <location filename="xmlreader.cpp" line="61"/>
        <source>sctime: opening configuration file</source>
        <translation>sctime: Konfigurationsdatei öffnen</translation>
    </message>
    <message>
        <location filename="xmlreader.cpp" line="62"/>
        <source>%1 : %2</source>
        <translation>%1 : %2</translation>
    </message>
    <message>
        <location filename="xmlreader.cpp" line="153"/>
        <source>conflict detected. clientid is %1 remoteid is %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="xmlreader.cpp" line="177"/>
        <source>two different clients have written a settings file with the same date.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="xmlreader.cpp" line="185"/>
        <source>using newer remote version.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="xmlreader.cpp" line="187"/>
        <source>two different clients have written a settings file with the same date. using newer remote version.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="xmlreader.cpp" line="201"/>
        <source>using newer local version.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="xmlreader.cpp" line="203"/>
        <source>two different clients have written a settings file with the same date. using newer local version.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="xmlreader.cpp" line="229"/>
        <source>sctime: reading configuration file</source>
        <translation>sctime: Konfigurationsdatei lesen</translation>
    </message>
</context>
<context>
    <name>XMLWriter</name>
    <message>
        <location filename="xmlwriter.cpp" line="42"/>
        <source>gotReply called with wrong sender type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="xmlwriter.cpp" line="49"/>
        <source>Communication error on writing to server, going offline</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
