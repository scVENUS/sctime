#include "xmlreader.h"
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QDomElement>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include "globals.h"
#include "resthelper.h"

void XMLReader::open()
{
  connect(this, &XMLReader::settingsPartRead, this, &XMLReader::continueAfterReading, Qt::UniqueConnection);
  bool usefilestorageonly=true;
#ifdef RESTCONFIG
  SCTimeXMLSettings *settings = (SCTimeXMLSettings *)(parent());
  usefilestorageonly=settings->restSaveOffline();
#endif
  if (usefilestorageonly||forceLocalRead) {
    auto f=openFile(usefilestorageonly);
    if (f!=NULL) {
      parse(f);
      delete f;
    } 
  } else {
    openREST();
  }
}

QFile* XMLReader::openFile(bool handleerr) {
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

    QFile* f=new QFile(configDir.filePath(filename));
    SCTimeXMLSettings *settings = (SCTimeXMLSettings *)(parent());
    if (!f->open(QIODevice::ReadOnly)&&handleerr)
    {
        logError(f->fileName() + ": " + f->errorString());
        if (global || f->exists())
        {
            if (global)
                settings->backupSettingsXml = false;
            // keine Fehlerausgabe, wenn "zeit-HEUTE.xml" fehlt
            auto msgbox=new QMessageBox(QMessageBox::Warning, tr("sctime: opening configuration file"),
                           tr("%1 : %2").arg(f->fileName(), f->errorString()),QMessageBox::NoButton, dynamic_cast<QWidget*>(this->parent()));
            connect(msgbox, &QMessageBox::finished,[=](){
               msgbox->deleteLater();
            });
            msgbox->open();
            msgbox->raise();
        }
        emit settingsPartRead(global, abtList, pcl, false, "");
        delete f;
        return NULL;
    }
    return f;
}

void XMLReader::openREST() {
    QString baseurl = getRestBaseUrl();
    QString postfix = "";
    if (!global) {
      postfix =  "?date=" + abtList->getDatum().toString("yyyy-MM-dd");
    }
    auto request = QNetworkRequest(QUrl(baseurl + "/" + REST_SETTINGS_ENDPOINT + postfix));
    QNetworkReply *reply = networkAccessManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &XMLReader::gotReply);
}

void XMLReader::gotReply() {
    auto obj=sender();
    parse(dynamic_cast<QNetworkReply*>(obj));
}

void XMLReader::parse(QIODevice *input)
{
    SCTimeXMLSettings *settings = dynamic_cast<SCTimeXMLSettings *>(parent());
    QNetworkReply* netinput = dynamic_cast<QNetworkReply*>(input);
    QFile* fileinput = dynamic_cast<QFile*>(input);
    QDomDocument doclocal("settings");
    QDomDocument docremote("settings");
    QString errMsg;
    QString resname;
    QString remoteclientid;
    int errLine, errCol;
    bool readSuccessLocal=false;
    bool readSuccessRemote=false;
    bool unmergedExists=false;

    if (netinput!=NULL) {
        auto clientinfo=getRestHeader(netinput,"sctime-client-info");
        remoteclientid=getRestHeader(netinput,"sctime-clientid");
        auto modified=getRestHeader(netinput,"sctime-modified");
        bool isrestresponse=true;
        #ifdef ENSURE_REST_HEADER
        auto sctimerestresponse=getRestHeader(netinput,"sctime-rest-response");
        isrestresponse=(sctimerestresponse=="true");
        #endif
        if ((netinput->error()!=QNetworkReply::NoError)||(!isrestresponse)) {
            logError("trying to open local file");
            if (netinput->attribute(QNetworkRequest::HttpStatusCodeAttribute)!=404 && !settings->restCurrentlyOffline()) {
               emit offlineSwitched(true);
            }
            if ((netinput->attribute(QNetworkRequest::HttpStatusCodeAttribute)==401)||(!isrestresponse)) {
               emit unauthorized();
            }
            auto f=openFile(true);
            if (f!=NULL) {
              parse(f);
              delete f;
            }
            input->deleteLater();
            return;
        } else {
            emit offlineSwitched(false);
        }
        if (!netinput->isFinished()) {
            return;
        }
        QByteArray ba=netinput->readAll();
        readSuccessRemote = docremote.setContent(qUncompress(ba), &errMsg, &errLine, &errCol);
        
        input->deleteLater();
        resname=netinput->url().toString();
        if (readSuccessRemote) {
          fileinput=openFile(false);
          fileinput->deleteLater();
        }
    }
    if (fileinput!=NULL) {
      if (!readSuccessRemote&&!fileinput->exists()) {
        emit settingsPartRead(global, abtList, pcl, false, "");
        return;
      }
      resname=fileinput->fileName();
      QFile unmerged(resname+ ".unmerged");
      unmergedExists = unmerged.exists();
      readSuccessLocal = doclocal.setContent(fileinput, &errMsg, &errLine, &errCol);
      fileinput->close();

      if (!readSuccessRemote) {
        if (unmergedExists) {
            if (unmerged.open(QIODevice::ReadOnly)) {
                readSuccessRemote = docremote.setContent(&unmerged, &errMsg, &errLine, &errCol);
                unmerged.close();
            }

        }
      }
    }

    auto rootElemRemote=docremote.documentElement();
    QString machineID=getMachineIdentifier();
    QString remoteDateStr=rootElemRemote.attribute("date");
    remoteID=rootElemRemote.attribute("identifier");
    remoteSaveTime=QDateTime::fromString(remoteDateStr, Qt::ISODate);

    bool readsuccess=false;
    QDomDocument doc;
    if (readSuccessLocal&&readSuccessRemote) {
        auto rootElemLocal=doclocal.documentElement();
        
        QString localID=rootElemLocal.attribute("identifier");
        QString localDateStr=rootElemLocal.attribute("date");
        QDateTime localDate=QDateTime::fromString(localDateStr, Qt::ISODate);
        if (remoteSaveTime==localDate) {
           if (remoteID==localID) {
             // standard case, just proceed
             readsuccess=true;
             doc=docremote;
           } else {
             // very improbable case, log it and use local version
             logError(tr("two different clients have written a settings file with the same date."));
             readsuccess=true;
             doc=doclocal;
           }
        } else if (remoteSaveTime>localDate) {
           if (remoteID==localID) {
             readsuccess=true;
             doc=docremote;
             logError(tr("using newer remote version."));
           } else {
             logError(tr("two different clients have written a settings file with the same date."));
             emit conflictedWithLocal(abtList->getDatum(), global, doclocal, docremote);
             if (!autoContinueOnConflict&&!continueThisConflict) {
               return;
             } else {
               logError(tr("Autocontinue: using newer remote version."));
               continueThisConflict =false;
               readsuccess=true;
               doc=docremote;
             }
           } 
        } else if (remoteSaveTime<localDate) {
           if (remoteID==localID) {
             readsuccess=true;
             doc=doclocal;
             logError(tr("using newer local version."));
           } else {
             logError(tr("two different clients have written a settings file with the same date."));
             emit conflictedWithLocal(abtList->getDatum(), global, doclocal, docremote);
             if (!autoContinueOnConflict&&!continueThisConflict) {
               return;
             } else {
               logError(tr("Autocontinue: using newer local version."));
               continueThisConflict =false;
               readsuccess=true;
               doc=doclocal;
             }
           } 
        }
    } else if (readSuccessLocal) {
        readsuccess=true;
        doc=doclocal;
    } else if (readSuccessRemote) {
        readsuccess=true;
        doc=docremote;
    }

    // another client has written a file recently
    if (readSuccessRemote && (clientId!=remoteclientid) && (remoteSaveTime.secsTo(QDateTime::currentDateTime())<150)) {
        logError(tr("conflict detected. localid is %1 remoteid is %2").arg(clientId).arg(remoteclientid));
        emit conflictingClientRunning(abtList->getDatum(), global, docremote);
        if (!autoContinueOnConflict&&!continueThisConflict) {
           return;
        } else {
            continueThisConflict =false;
        }
    }
    
    // QByteArray bytes=input->readAll();
    if (!readsuccess)
    {
        if (global)
            settings->backupSettingsXml = false;
        emit settingsPartRead(global, abtList, pcl, false, "error reading configuration file");
        QMessageBox *msgbox=new QMessageBox(QMessageBox::Critical,
            tr("sctime: reading configuration file"),
            QObject::tr("error in %1, line %2, column %3: %4.").arg(resname).arg(errLine).arg(errCol).arg(errMsg),
            QMessageBox::Ok, dynamic_cast<QWidget*>(this->parent()));
        connect(msgbox, &QMessageBox::finished,msgbox, &QMessageBox::deleteLater);
        msgbox->open();
        msgbox->raise();
        return;
    }
    if (unmergedExists) {
        QFile::remove(resname+ ".unmerged");
    }

    fillSettingsFromDocument(doc, settings);
}

void XMLReader::fillSettingsFromDocument(const QDomDocument& doc, SCTimeXMLSettings* settings) {
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
                                                        if (!bereitschaften.contains(bereitschaft)) {
                                                            bereitschaften.append(bereitschaft);
                                                            itUk->second.setBereitschaft(bereitschaften);
                                                        }
                                                    }
                                                }
                                                if (elem4.tagName() == "eintrag")
                                                {
                                                    QString eintragstr = elem4.attribute("nummer");
                                                    if (eintragstr.isNull()||eintragstr.isEmpty())
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
#ifndef DISABLE_PUNCHCLOCK
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
#endif
            if (elem1.tagName() == "general")
            {
                for (QDomNode node2 = elem1.firstChild(); !node2.isNull(); node2 = node2.nextSibling())
                {
                    QDomElement elem2 = node2.toElement();
                    if (!elem2.isNull())
                    {
                        if (elem2.tagName() == "aktives_konto")
                        {
                            aktiveskontotag = elem2; // Aktives Konto merken und zum Schluss setzen, damit es vorher erzeugt wurde
                        }
                        if (global)
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
                            if (elem2.tagName() == "write_consolidated_intervals")
                            {
                                settings->m_writeConsolidatedIntervals = (elem2.attribute("on") != "no");
                            }

                            if (elem2.tagName() == "windowposition")
                            {
                                bool ok;
                                int x = QString(elem2.attribute("x")).toInt(&ok);
                                int y = QString(elem2.attribute("y")).toInt(&ok);
                                if (ok)
                                {
                                    QPoint pos(x, y);
                                    QRect rootwinsize(QGuiApplication::primaryScreen()->availableGeometry());
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
                            if (elem2.tagName() == "stayoffline")
                            {
                                settings->setRestSaveOffline((elem2.attribute("on") == "yes"));
                            }
                            if (elem2.tagName() == "kontodlgwindowposition")
                            {
                                bool ok;
                                int x = QString(elem2.attribute("x")).toInt(&ok);
                                int y = QString(elem2.attribute("y")).toInt(&ok);
                                if (ok)
                                {
                                    QPoint pos(x, y);
                                    QRect rootwinsize(QGuiApplication::primaryScreen()->availableGeometry());
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
    }

    if ((!aktiveskontotag.isNull()) && abtList)
    {
        QString abtstr = aktiveskontotag.attribute("abteilung");
        QString kostr = aktiveskontotag.attribute("konto");
        QString ukostr = aktiveskontotag.attribute("unterkonto");
        int idx = aktiveskontotag.attribute("index").toInt();
        abtList->setAsAktiv(abtstr, kostr, ukostr, idx);
    }
    emit settingsPartRead(global, abtList, pcl, true, "");
}

void XMLReader::continueAfterReading(bool global, AbteilungsListe* abtList, PunchClockList* pcl)
{
  /* we come here twice. first after reading the global settings, and then after reading the settings of the day. after that we send the settingsRead signal*/
  if (global) {
    this->global = false;
    open();
  } else {
    emit settingsRead();
  }
}

