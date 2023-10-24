#include "xmlreader.h"
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QDomElement>
#include <QApplication>
#include <QDesktopWidget>
#include "globals.h"

void XMLReader::open()
{
    QString filename;
    if (global)
        filename = "settings.xml";
    else
    {
        filename = "zeit-" + abtList->getDatum().toString("yyyy-MM-dd") + ".xml";
        QFileInfo qf(configDir.filePath(abtList->getDatum().toString("yyyy")), filename);
        if (abtList)
        {
            if (qf.exists())
            {
                filename = abtList->getDatum().toString("yyyy") + "/" + filename;
                abtList->setCheckInState(true);
            }
            else
                abtList->setCheckInState(false);
        }
    }
#ifndef RESTONLY
    QFile f(configDir.filePath(filename));
    SCTimeXMLSettings *settings = (SCTimeXMLSettings *)(parent());
    if (!f.open(QIODevice::ReadOnly))
    {
        logError(f.fileName() + ": " + f.errorString());
        if (global || f.exists())
        {
            // keine Fehlerausgabe, wenn "zeit-HEUTE.xml" fehlt
            QMessageBox::warning(NULL, QObject::tr("sctime: opening configuration file"),
                                 QObject::tr("%1 : %2").arg(f.fileName(), f.errorString()));
            if (global)
                settings->backupSettingsXml = false;
        }
        return;
    }
    parse(&f);

#else
    auto env = QProcessEnvironment::systemEnvironment();
    QString user = env.value("SCTIME_USER");
    QString baseurl = env.value("SCTIME_BASE_URL");
    auto request = QNetworkRequest(QUrl(baseurl + "/../accountingdata/" + user + "/" + filename));
    networkAccessManager.get(request);
#endif
}

void XMLReader::parse(QIODevice *input)
{
    SCTimeXMLSettings *settings = (SCTimeXMLSettings *)(parent());
    // TODO
    /*if (input->error()!=QNetworkReply::NoError) {
      emit settings->settingsPartRead(global, abtList, pcl); //TODO: we probably want a settingsReadFailedSignal instead
      return;
    }*/
    QDomDocument doc("settings");
    QString errMsg;
    int errLine, errCol;
    // QByteArray bytes=input->readAll();
    if (!doc.setContent(input, &errMsg, &errLine, &errCol))
    {
        QMessageBox::critical(NULL, QObject::tr("sctime: reading configuration file"),
                              QObject::tr("error in %1, line %2, column %3: %4.").arg(errMsg).arg(errLine).arg(errCol).arg(errMsg));
        if (global)
            settings->backupSettingsXml = false;
        return;
    }
#ifndef RESTONLY
    // when closing a networkrequest, we get another finished signal. Not sure if this bug or feature - for now just dont close it
    input->close();
#endif
    QDomElement aktiveskontotag;
    QDomElement docElem = doc.documentElement();
    QString lastVersion = docElem.attribute("version");
    if (global)
    {
        settings->defaultcommentfiles.clear();
        settings->columnwidth.clear();
        settings->setUseCustomFont(false);
    }

    for (QDomNode node1 = docElem.firstChild(); !node1.isNull(); node1 = node1.nextSibling())
    {
        QDomElement elem1 = node1.toElement();
        if (!elem1.isNull())
        {
            if ((elem1.tagName() == "abteilung") && (abtList))
            {
                QString abteilungstr = elem1.attribute("name");
                if (abteilungstr.isNull())
                    continue;
                abtList->insertAbteilung(abteilungstr);
                if (elem1.attribute("open") == "yes")
                    abtList->setAbteilungFlags(abteilungstr, IS_CLOSED, FLAG_MODE_NAND);
                if (elem1.attribute("open") == "no")
                    abtList->setAbteilungFlags(abteilungstr, IS_CLOSED, FLAG_MODE_OR);

                if (!elem1.attribute("color").isEmpty())
                {
                    abtList->setBgColor(SCTimeXMLSettings::str2color(elem1.attribute("color")), abteilungstr);
                }
                for (QDomNode node2 = elem1.firstChild(); !node2.isNull(); node2 = node2.nextSibling())
                {
                    QDomElement elem2 = node2.toElement();
                    if (!elem2.isNull())
                    {
                        if (elem2.tagName() == "konto")
                        {
                            QString kontostr = elem2.attribute("name");
                            if (kontostr.isNull())
                                continue;
                            abtList->insertKonto(abteilungstr, kontostr);
                            if (elem2.attribute("open") == "yes")
                                abtList->setKontoFlags(abteilungstr, kontostr, IS_CLOSED, FLAG_MODE_NAND);
                            if (elem2.attribute("open") == "no")
                                abtList->setKontoFlags(abteilungstr, kontostr, IS_CLOSED, FLAG_MODE_OR);
                            abtList->moveKontoPersoenlich(abteilungstr, kontostr, (elem2.attribute("persoenlich") == "yes"));
                            if (!elem2.attribute("color").isEmpty())
                            {
                                abtList->setBgColor(SCTimeXMLSettings::str2color(elem2.attribute("color")), abteilungstr, kontostr);
                            }

                            for (QDomNode node3 = elem2.firstChild(); !node3.isNull(); node3 = node3.nextSibling())
                            {
                                QDomElement elem3 = node3.toElement();
                                if (!elem3.isNull())
                                {
                                    if (elem3.tagName() == "unterkonto")
                                    {
                                        QString unterkontostr = elem3.attribute("name");
                                        if (unterkontostr.isNull())
                                            continue;
                                        abtList->insertUnterKonto(abteilungstr, kontostr, unterkontostr);
                                        if (elem3.attribute("open") == "yes")
                                            abtList->setUnterKontoFlags(abteilungstr, kontostr, unterkontostr,
                                                                        IS_CLOSED, FLAG_MODE_NAND);
                                        if (elem3.attribute("open") == "no")
                                            abtList->setUnterKontoFlags(abteilungstr, kontostr, unterkontostr,
                                                                        IS_CLOSED, FLAG_MODE_OR);
                                        if ((elem3.attribute("persoenlich") == "yes") || (elem3.attribute("persoenlich") == "no"))
                                            abtList->moveUnterKontoPersoenlich(abteilungstr, kontostr, unterkontostr, (elem3.attribute("persoenlich") == "yes"));

                                        if (!elem3.attribute("color").isEmpty())
                                        {
                                            abtList->setBgColor(SCTimeXMLSettings::str2color(elem3.attribute("color")), abteilungstr, kontostr, unterkontostr);
                                        }

                                        bool ukontPers = ((abtList->getUnterKontoFlags(abteilungstr, kontostr, unterkontostr)) & UK_PERSOENLICH);
                                        bool dummydeleted = false;

                                        for (QDomNode node4 = elem3.firstChild(); !node4.isNull(); node4 = node4.nextSibling())
                                        {
                                            QDomElement elem4 = node4.toElement();
                                            if (!elem4.isNull())
                                            {
                                                if (elem4.tagName() == "bereitschaft")
                                                {
                                                    QString bereitschaft = elem4.attribute("type");
                                                    if (bereitschaft.isNull())
                                                        continue;
                                                    UnterKontoListe::iterator itUk;
                                                    UnterKontoListe *ukl;
                                                    if (abtList->findUnterKonto(itUk, ukl, abteilungstr, kontostr, unterkontostr))
                                                    {
                                                        QStringList bereitschaften;
                                                        bereitschaften = itUk->second.getBereitschaft();
                                                        bereitschaften.append(bereitschaft);
                                                        itUk->second.setBereitschaft(bereitschaften);
                                                    }
                                                }
                                                if (elem4.tagName() == "eintrag")
                                                {
                                                    QString eintragstr = elem4.attribute("nummer");
                                                    if (eintragstr.isNull())
                                                        continue;
                                                    int idx = eintragstr.toInt();

                                                    EintragsListe::iterator eti;
                                                    EintragsListe *etl;

                                                    if (!abtList->findEintrag(eti, etl, abteilungstr, kontostr, unterkontostr, idx))
                                                    {
                                                        if ((idx != 0) && (abtList->findEintrag(eti, etl, abteilungstr, kontostr, unterkontostr, 0)) && ((eti->second).isEmpty()))
                                                        {
                                                            abtList->deleteEintrag(abteilungstr, kontostr, unterkontostr, 0); // Leere Dummy Eintraege mit Index 0 loswerden.
                                                            dummydeleted = true;
                                                        }
                                                        abtList->insertEintrag(abteilungstr, kontostr, unterkontostr, idx);
                                                    }
                                                    QSet<QString> specialRemunSet;
                                                    for (QDomNode node5 = elem4.firstChild(); !node5.isNull(); node5 = node5.nextSibling())
                                                    {
                                                        QDomElement elem5 = node5.toElement();
                                                        if (elem5.tagName() == "specialremun")
                                                        {
                                                            specialRemunSet.insert(elem5.attribute("name"));
                                                        }
                                                    }
                                                    if (!specialRemunSet.isEmpty())
                                                    {
                                                        abtList->findEintrag(eti, etl, abteilungstr, kontostr, unterkontostr, idx);
                                                        eti->second.setAchievedSpecialRemunSet(specialRemunSet);
                                                    }
                                                    if (elem4.attribute("persoenlich") == "yes")
                                                        abtList->setEintragFlags(abteilungstr, kontostr, unterkontostr, idx,
                                                                                 UK_PERSOENLICH, FLAG_MODE_OR);
                                                    else if (elem4.attribute("persoenlich") == "no")
                                                        abtList->setEintragFlags(abteilungstr, kontostr, unterkontostr, idx,
                                                                                 UK_PERSOENLICH, FLAG_MODE_NAND);
                                                    else if (ukontPers)
                                                        abtList->setEintragFlags(abteilungstr, kontostr, unterkontostr,
                                                                                 idx, UK_PERSOENLICH, FLAG_MODE_OR);
                                                    else
                                                        abtList->setEintragFlags(abteilungstr, kontostr, unterkontostr,
                                                                                 idx, UK_PERSOENLICH, FLAG_MODE_NAND);
                                                    if (!elem4.attribute("sekunden").isNull())
                                                    {
                                                        abtList->setSekunden(abteilungstr, kontostr, unterkontostr, idx,
                                                                             elem4.attribute("sekunden").toInt(), true);
                                                    }
                                                    if (!elem4.attribute("abzurechnende_sekunden").isNull())
                                                    {
                                                        abtList->setSekundenAbzur(abteilungstr, kontostr, unterkontostr, idx,
                                                                                  elem4.attribute("abzurechnende_sekunden").toInt());
                                                    }
                                                    if (!elem4.attribute("kommentar").isNull())
                                                    {
                                                        abtList->setKommentar(abteilungstr, kontostr, unterkontostr, idx,
                                                                              elem4.attribute("kommentar"));
                                                    }
                                                }
                                            }
                                        }
                                        if ((!dummydeleted) && (ukontPers))
                                        {
                                            EintragsListe::iterator eti;
                                            EintragsListe *etl;
                                            if (!abtList->findEintrag(eti, etl, abteilungstr, kontostr, unterkontostr, 0))
                                            {
                                                if (etl->size() == 0)
                                                { // bei Bedarf Dummy Erzeugen
                                                    abtList->insertEintrag(abteilungstr, kontostr, unterkontostr, 0);
                                                }
                                            }
                                            abtList->setEintragFlags(abteilungstr, kontostr, unterkontostr,
                                                                     etl->begin()->first, UK_PERSOENLICH, FLAG_MODE_OR);
                                        }
                                        // Eintraege deaktivieren, falls eine Verbindung zur Datenbank aufgebaut werden konnte,
                                        // sie dort aber nicht erscheinen.
                                        if (((abtList->getUnterKontoFlags(abteilungstr, kontostr, unterkontostr) & IS_IN_DATABASE) == 0) && (abtList->kontoDatenInfoConnected()))
                                        {
                                            int sek, sekabzur;
                                            abtList->getUnterKontoZeiten(abteilungstr, kontostr, unterkontostr, sek, sekabzur);
                                            // Nur deaktivieren, falls nicht bereits auf dieses Konto gebucht wurde
                                            if ((sek == 0) && (sekabzur == 0))
                                            {
                                                abtList->setUnterKontoFlags(abteilungstr, kontostr, unterkontostr, IS_DISABLED, FLAG_MODE_OR);
                                            }
                                            else
                                                abtList->setUnterKontoFlags(abteilungstr, kontostr, unterkontostr, IS_DISABLED, FLAG_MODE_NAND);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if ((elem1.tagName() == "punchclock") && (pcl))
            {
                pcl->clear();
                for (QDomNode node2 = elem1.firstChild(); !node2.isNull(); node2 = node2.nextSibling())
                {
                    QDomElement elem2 = node2.toElement();
                    if (!elem2.isNull())
                    {
                        if (elem2.tagName() == "pcentry")
                        {
                            QString beginstr = elem2.attribute("begin");
                            QString endstr = elem2.attribute("end");
                            if (beginstr.isNull() || endstr.isNull())
                                continue;
                            QTime begintime = QTime::fromString(beginstr, "H:m");
                            QTime endtime = QTime::fromString(endstr, "H:m");
                            if (!begintime.isValid() || !endtime.isValid())
                                continue;
                            if (begintime > endtime)
                                continue;
                            pcl->push_back(PunchClockEntry((int)(begintime.msecsSinceStartOfDay() / 1000), (int)(endtime.msecsSinceStartOfDay() / 1000)));
                        }
                    }
                }
            }
            if (elem1.tagName() == "general")
            {
                for (QDomNode node2 = elem1.firstChild(); !node2.isNull(); node2 = node2.nextSibling())
                {
                    QDomElement elem2 = node2.toElement();
                    if (!elem2.isNull())
                    {
                        if (elem2.tagName() == "timeincrement")
                        {
                            QString secondsstr = elem2.attribute("seconds");
                            if (secondsstr.isNull())
                                continue;
                            settings->timeInc = secondsstr.toInt();
                        }
                        if (elem2.tagName() == "fasttimeincrement")
                        {
                            QString secondsstr = elem2.attribute("seconds");
                            if (secondsstr.isNull())
                                continue;
                            settings->fastTimeInc = secondsstr.toInt();
                        }
                        if (elem2.tagName() == "zeitkommando")
                        {
                            QString kommandostr = elem2.firstChild().toCharacterData().data();
                            if (kommandostr.isNull())
                                continue;
                            settings->zeitKommando = kommandostr;
                        }
                        if (elem2.tagName() == "zeitkontenkommando")
                        {
                            QString zeitkontenkommandostr = elem2.firstChild().toCharacterData().data();
                            if (zeitkontenkommandostr.isNull())
                                continue;
                            settings->setZeitKontenKommando(zeitkontenkommandostr);
                        }
                        if (elem2.tagName() == "defcommentdisplay")
                        {
                            QString dmstring = elem2.attribute("mode");
                            if (dmstring.isNull())
                                continue;
                            SCTimeXMLSettings::DefCommentDisplayModeEnum dm = SCTimeXMLSettings::DM_BOLD;
                            if (dmstring == "DefaultCommentsNotUsedBold")
                            {
                                dm = SCTimeXMLSettings::DM_NOTUSEDBOLD;
                            }
                            else if (dmstring == "NotBold")
                            {
                                dm = SCTimeXMLSettings::DM_NOTBOLD;
                            }
                            settings->setDefCommentDisplayMode(dm);
                        }
                        if (elem2.tagName() == "dragndrop")
                        {
                            settings->setDragNDrop(elem2.attribute("on") == "yes");
                        }
                        if (elem2.tagName() == "persoenliche_kontensumme")
                        {
                            settings->setPersoenlicheKontensumme(elem2.attribute("on") == "yes");
                        }
                        if (elem2.tagName() == "working_time_warnings")
                        {
                            settings->m_workingTimeWarnings = (elem2.attribute("on") != "no");
                        }
                        if (elem2.tagName() == "aktives_konto")
                        {
                            aktiveskontotag = elem2; // Aktives Konto merken und zum Schluss setzen, damit es vorher erzeugt wurde
                        }
                        if (elem2.tagName() == "windowposition")
                        {
                            bool ok;
                            int x = QString(elem2.attribute("x")).toInt(&ok);
                            int y = QString(elem2.attribute("y")).toInt(&ok);
                            if (ok)
                            {
                                QPoint pos(x, y);
                                QRect rootwinsize = QApplication::desktop()->screenGeometry();
                                if (rootwinsize.contains(pos)) // Position nicht setzen, wenn Fenster sonst ausserhalb
                                    settings->mainwindowPosition = pos;
                            }
                        }
                        if (elem2.tagName() == "windowsize")
                        {
                            QString xstr = elem2.attribute("width");
                            if (xstr.isNull())
                                continue;
                            QString ystr = elem2.attribute("height");
                            if (ystr.isNull())
                                continue;
                            settings->mainwindowSize = QSize(xstr.toInt(), ystr.toInt());
                        }
                        if (elem2.tagName() == "saveeintrag")
                        {
                            settings->alwaysSaveEintrag = (elem2.attribute("always") == "yes");
                        }
                        if (elem2.tagName() == "typecolumn")
                        {
                            settings->m_showTypeColumn = (elem2.attribute("show") == "yes");
                        }
                        if (elem2.tagName() == "pccdata")
                        {
                            settings->m_currentPCCdata = elem2.attribute("current");
                            settings->m_prevPCCdata = elem2.attribute("previous");
                        }
                        if (elem2.tagName() == "pspcolumn")
                        {
                            settings->m_showPSPColumn = (elem2.attribute("show") == "yes");
                        }
                        if (elem2.tagName() == "specialremunselector")
                        {
                            settings->setShowSpecialRemunSelector(elem2.attribute("show") == "yes");
                        }
                        if (elem2.tagName() == "usedefaultcommentifunique")
                        {
                            settings->m_useDefaultCommentIfUnique = (elem2.attribute("on") == "yes");
                        }
                        if (elem2.tagName() == "poweruserview")
                        {
                            settings->setPowerUserView((elem2.attribute("on") == "yes"));
                        }
                        if (elem2.tagName() == "overtimeregulatedmode")
                        {
                            settings->setOvertimeRegulatedModeActive((elem2.attribute("on") == "yes"));
                        }
                        if (elem2.tagName() == "overtimeothermode")
                        {
                            settings->setOvertimeOtherModeActive((elem2.attribute("on") == "yes"));
                        }
                        if (elem2.tagName() == "nightmode")
                        {
                            settings->setNightModeActive((elem2.attribute("on") == "yes"));
                        }
                        if (elem2.tagName() == "publicholidaymode")
                        {
                            settings->setPublicHolidayModeActive((elem2.attribute("on") == "yes"));
                        }
                        if (elem2.tagName() == "lastrecordedtimestamp")
                        {
                            QDateTime ts = QDateTime::fromString(elem2.attribute("value"), SCTimeXMLSettings::timestampFormat());
                            settings->setLastRecordedTimestamp(ts);
                        }
                        if (elem2.tagName() == "customfont")
                        {
                            settings->setUseCustomFont(true);
                            settings->setCustomFont(elem2.attribute("family"));
                            settings->setCustomFontSize(elem2.attribute("size").toInt());
                        }
                        if (elem2.tagName() == "singleclickactivation")
                        {
                            settings->setSingleClickActivation((elem2.attribute("on") == "yes"));
                        }
                        if (elem2.tagName() == "warnISO8859")
                        {
                            settings->setWarnISO8859((elem2.attribute("on") == "yes"));
                        }
                        if (elem2.tagName() == "sortByCommentText")
                        {
                            settings->setSortByCommentText((elem2.attribute("on") == "yes"));
                        }
                        if (elem2.tagName() == "kontodlgwindowposition")
                        {
                            bool ok;
                            int x = QString(elem2.attribute("x")).toInt(&ok);
                            int y = QString(elem2.attribute("y")).toInt(&ok);
                            if (ok)
                            {
                                QPoint pos(x, y);
                                QRect rootwinsize = QApplication::desktop()->screenGeometry();
                                if (rootwinsize.contains(pos)) // Position nicht setzen, wenn Fenster sonst ausserhalb
                                    settings->unterKontoWindowPosition = pos;
                            }
                        }
                        if (elem2.tagName() == "kontodlgwindowsize")
                        {
                            QString xstr = elem2.attribute("width");
                            if (xstr.isNull())
                                continue;
                            QString ystr = elem2.attribute("height");
                            if (ystr.isNull())
                                continue;
                            settings->unterKontoWindowSize = QSize(xstr.toInt(), ystr.toInt());
                        }
                        if ((global) && (elem2.tagName() == "defaultcommentsfile"))
                        {
                            settings->defaultcommentfiles.push_back(elem2.attribute("name", "defaultcomments.xml"));
                        }
                        if ((global) && (elem2.tagName() == "column"))
                        {
                            settings->columnwidth.push_back(elem2.attribute("width", "50").toInt());
                        }
                        if (global && elem2.tagName() == "backends")
                        {
                            settings->backends = elem2.attribute("names", settings->defaultbackends);
                            if (SCTimeXMLSettings::compVersion("0.80.1", lastVersion) != -1) // in case of upgrade add new backend
                            {
                                settings->backends.replace("file", "json file");
                            }

                            for (QDomNode node3 = elem2.firstChild(); !node3.isNull(); node3 = node3.nextSibling())
                            {
                                QDomElement elem3 = node3.toElement();
                                if (!elem3.isNull())
                                {
                                    if (elem3.tagName() == "database")
                                    {
                                        settings->databaseserver = elem3.attribute("server", settings->defaultdatabaseserver);
                                        settings->database = elem3.attribute("name", settings->defaultdatabase);
                                        settings->databaseuser = elem3.attribute("user");
                                        settings->databasepassword = elem3.attribute("password");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if ((!aktiveskontotag.isNull()) && abtList)
    {
        QString abtstr = aktiveskontotag.attribute("abteilung");
        QString kostr = aktiveskontotag.attribute("konto");
        QString ukostr = aktiveskontotag.attribute("unterkonto");
        int idx = aktiveskontotag.attribute("index").toInt();
        abtList->setAsAktiv(abtstr, kostr, ukostr, idx);
    }
    emit settings->settingsPartRead(global, abtList, pcl);
}