#include "xmlwriter.h"
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QApplication>
#include <QDomDocument>
#include <QMessageBox>
#include <QTextStream>
#include <QProcessEnvironment>
#include <QBuffer>
#include "sctimexmlsettings.h"
#include "globals.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

void XMLWriter::checkReply(QNetworkReply* input) {
   auto errcode=input->error();
   if (errcode!=0) {
      onErr(input);
      return;
   }
   if (input->isFinished()) {
     if (input->attribute(QNetworkRequest::HttpStatusCodeAttribute)==202) {
         auto conflictingclient=input->rawHeader("sctime-client-info");
         emit conflicted(abtList->getDatum(), global, (input->readAll()));
         emit settingsPartWritten(global, abtList, pcl);
         emit offlineSwitched(false);
         input->deleteLater();
     } else {  
       emit settingsPartWritten(global, abtList, pcl);
       emit offlineSwitched(false);
       input->deleteLater();
     }
   }
}

void XMLWriter::gotReply() {
    auto obj=sender();
    checkReply(dynamic_cast<QNetworkReply*>(obj));
}

void XMLWriter::onErr(QNetworkReply* input) {
    logError(tr("Communication error on writing to server, going offline"));
    if (!settings->restCurrentlyOffline()) {
       emit offlineSwitched(true);
    }
    if (input->attribute(QNetworkRequest::HttpStatusCodeAttribute)==401) {
      emit unauthorized();
    }
    // we have already saved them locally, so should be able to continue anyway
    emit settingsPartWritten(global, abtList, pcl);
    input->deleteLater();
}

void XMLWriter::writeBytes(QUrl url, QByteArray ba) {
  auto request = new QNetworkRequest(url);
  auto bac=new QByteArray(ba);
  request->setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
  auto buf =new QBuffer(bac);
  buf->open(QIODeviceBase::ReadOnly);
  QNetworkReply *reply = networkAccessManager->put(*request, buf);
  connect(reply, &QNetworkReply::finished, this, &XMLWriter::gotReply);
  //connect(reply, &QNetworkReply::readyRead, this, &XMLWriter::gotReply);
  //connect(reply, &QNetworkReply::errorOccurred,
  //      this, &XMLWriter::onErr);
}

void XMLWriter::writeAllSettings() {
  writeAll=true;
  writeSettings(true);
}

void XMLWriter::writeSettings(bool global) {
   this->global=global;
   if ((abtList->checkInState())&&(!global)) {
      trace(QObject::tr("zeit-DAY.sh not written because it has already been checked in"));
      // nothing to do, so just emit success signal
      emit settingsPartWritten(global, abtList, pcl);
      return;
  }
  #ifndef NO_XML
  QDomDocument doc("settings");

  QDateTime savetime=QDateTime::currentDateTimeUtc();

  QDomElement root = doc.createElement( "sctime" );
  // TODO: XML/HTML-Quoting
  root.setAttribute("version", qApp->applicationVersion());
  root.setAttribute("system", QSysInfo::productType());
  root.setAttribute("date", savetime.toString(Qt::ISODate));
  root.setAttribute("identifier", clientId);
  doc.appendChild( root );
  QDomElement generaltag = doc.createElement( "general" );

  if (global) {
    QDomElement timeinctag = doc.createElement( "timeincrement" );
    timeinctag.setAttribute("seconds",settings->timeInc);
    generaltag.appendChild(timeinctag);

    QDomElement pcctag = doc.createElement( "pccdata" );
    pcctag.setAttribute("current",settings->m_currentPCCdata);
    pcctag.setAttribute("previous",settings->m_prevPCCdata);
    generaltag.appendChild(pcctag);

    QDomElement fasttimeinctag = doc.createElement( "fasttimeincrement" );
    fasttimeinctag.setAttribute("seconds",settings->fastTimeInc);
    generaltag.appendChild(fasttimeinctag);

    QDomElement zeitkommandotag = doc.createElement( "zeitkommando" );
    zeitkommandotag.appendChild(doc.createTextNode(settings->zeitKommando));
    generaltag.appendChild(zeitkommandotag);

    if (!settings->zeitKontenKommando().isEmpty()) {
      QDomElement zeitkontenkommandotag = doc.createElement( "zeitkontenkommando" );
      zeitkontenkommandotag.appendChild(doc.createTextNode(settings->zeitKontenKommando()));
      generaltag.appendChild(zeitkontenkommandotag);
    }

    if (settings->m_workingTimeWarnings==false) {
        QDomElement maxworktag = doc.createElement( "working_time_warnings" );
        maxworktag.setAttribute("on","no");
        generaltag.appendChild(maxworktag);
    }

    QDomElement mainwindowpositiontag = doc.createElement( "windowposition" );
    mainwindowpositiontag.setAttribute("x",settings->mainwindowPosition.x());
    mainwindowpositiontag.setAttribute("y",settings->mainwindowPosition.y());
    generaltag.appendChild(mainwindowpositiontag);

    QDomElement mainwindowsizetag = doc.createElement("windowsize");
    mainwindowsizetag.setAttribute("width",settings->mainwindowSize.width());
    mainwindowsizetag.setAttribute("height",settings->mainwindowSize.height());
    generaltag.appendChild(mainwindowsizetag);

    for (unsigned int i=0; i<settings->columnwidth.size() - 1; i++) { // do not save the width of the comment column
        QDomElement columnwidthtag = doc.createElement("column");
        columnwidthtag.setAttribute("width",settings->columnwidth[i]);
        generaltag.appendChild(columnwidthtag);
    }

    QDomElement defcommentdm = doc.createElement("defcommentdisplay");
    QString dm;
    switch(settings->defCommentDisplayMode()) {
      case SCTimeXMLSettings::DM_NOTUSEDBOLD: dm="DefaultCommentsNotUsedBold"; break;
      case SCTimeXMLSettings::DM_NOTBOLD: dm="NotBold"; break;
      default: dm ="DefaultCommentsBold"; break;
    }
    defcommentdm.setAttribute("mode",dm);
    generaltag.appendChild(defcommentdm);

    QDomElement saveeintragtag = doc.createElement("saveeintrag");
    QString always="no";
    if (settings->alwaysSaveEintrag) always="yes";
    saveeintragtag.setAttribute("always",always);
    generaltag.appendChild(saveeintragtag);

    QDomElement powerusertag = doc.createElement("poweruserview");
    QString on="no";
    if (settings->powerUserView()) on="yes";
    powerusertag.setAttribute("on",on);
    generaltag.appendChild(powerusertag);

    QDomElement persoenlichekontensummentag = doc.createElement("persoenliche_kontensumme");
    on="no";
    if (settings->persoenlicheKontensumme()) on="yes";
    persoenlichekontensummentag.setAttribute("on",on);
    generaltag.appendChild(persoenlichekontensummentag);

    QDomElement typecolumntag = doc.createElement("typecolumn");
    on="no";
    if (settings->showTypeColumn()) on="yes";
    typecolumntag.setAttribute("show",on);
    generaltag.appendChild(typecolumntag);
    
    QDomElement pspcolumntag = doc.createElement("pspcolumn");
    on="no";
    if (settings->showPSPColumn()) on="yes";
    pspcolumntag.setAttribute("show",on);
    generaltag.appendChild(pspcolumntag);
    
    QDomElement srseltag = doc.createElement("specialremunselector");
    on="no";
    if (settings->showSpecialRemunSelector()) on="yes";
    srseltag.setAttribute("show",on);
    generaltag.appendChild(srseltag);
    
    QDomElement usedefaultcommentifuniquetag = doc.createElement("usedefaultcommentifunique");
    on="no";
    if (settings->useDefaultCommentIfUnique()) on="yes";
    usedefaultcommentifuniquetag.setAttribute("on",on);
    generaltag.appendChild(usedefaultcommentifuniquetag);

    if (settings->useCustomFont()) {
        QDomElement customfonttag = doc.createElement("customfont");
        QString size="";
        size=size.setNum(settings->customFontSize());
        customfonttag.setAttribute("family",settings->customFont());
        customfonttag.setAttribute("size",size);
        generaltag.appendChild(customfonttag);
    }

    QDomElement singleclicktag = doc.createElement("singleclickactivation");
    on="no";
    if (settings->singleClickActivation()) on="yes";
    singleclicktag.setAttribute("on",on);
    generaltag.appendChild(singleclicktag);

    QDomElement warnISO8859tag = doc.createElement("warnISO8859");
    on="no";
    if (settings->warnISO8859()) on="yes";
    warnISO8859tag.setAttribute("on",on);
    generaltag.appendChild(warnISO8859tag);

    QDomElement sortByCommentTextTag = doc.createElement("sortByCommentText");
    on="no";
    if (settings->sortByCommentText()) on="yes";
    sortByCommentTextTag.setAttribute("on",on);
    generaltag.appendChild(sortByCommentTextTag);

    QDomElement dragndroptag = doc.createElement("dragndrop");
    on="no";
    if (settings->dragNDrop()) on="yes";
    dragndroptag.setAttribute("on",on);
    generaltag.appendChild(dragndroptag);

    QDomElement overtimeregulatedmodetag = doc.createElement("overtimeregulatedmode");
    on =settings->overtimeRegulatedModeActive() ? "yes" : "no";
    overtimeregulatedmodetag.setAttribute("on",on);
    generaltag.appendChild(overtimeregulatedmodetag);

    QDomElement overtimeothermodetag = doc.createElement("overtimeothermode");
    on =settings->overtimeOtherModeActive() ? "yes" : "no";
    overtimeothermodetag.setAttribute("on",on);
    generaltag.appendChild(overtimeothermodetag);

    QDomElement nightmodetag = doc.createElement("nightmode");
    on =settings->nightModeActive() ? "yes" : "no";
    nightmodetag.setAttribute("on",on);
    generaltag.appendChild(nightmodetag);

    QDomElement lastrecordedtimestamptag = doc.createElement("lastrecordedtimestamp");
    QString ts = settings->lastRecordedTimestamp().toString(settings->timestampFormat());
    lastrecordedtimestamptag.setAttribute("value",ts);
    generaltag.appendChild(lastrecordedtimestamptag);

    QDomElement publicholidaymodetag = doc.createElement("publicholidaymode");
    on =settings->publicHolidayModeActive() ? "yes" : "no";
    publicholidaymodetag.setAttribute("on",on);
    generaltag.appendChild(publicholidaymodetag);

    QDomElement stayOfflineTag = doc.createElement("stayoffline");
    on =settings->restSaveOffline() ? "yes" : "no";
    stayOfflineTag.setAttribute("on",on);
    generaltag.appendChild(stayOfflineTag);

    QDomElement kontodlgwindowpositiontag = doc.createElement( "kontodlgwindowposition" );
    kontodlgwindowpositiontag.setAttribute("x",settings->unterKontoWindowPosition.x());
    kontodlgwindowpositiontag.setAttribute("y",settings->unterKontoWindowPosition.y());
    generaltag.appendChild(kontodlgwindowpositiontag);

    QDomElement kontodlgwindowsizetag = doc.createElement("kontodlgwindowsize");
    kontodlgwindowsizetag.setAttribute("width",settings->unterKontoWindowSize.width());
    kontodlgwindowsizetag.setAttribute("height",settings->unterKontoWindowSize.height());
    generaltag.appendChild(kontodlgwindowsizetag);

    for (unsigned int i=0; i<settings->defaultcommentfiles.size(); i++) {
        QDomElement defaultcommentfiletag = doc.createElement("defaultcommentsfile");
        defaultcommentfiletag.setAttribute("name",settings->defaultcommentfiles[i]);
        generaltag.appendChild(defaultcommentfiletag);
    }

    QDomElement qdeBackends = doc.createElement("backends");
    qdeBackends.setAttribute("names", settings->backends);
    generaltag.appendChild(qdeBackends);

    QDomElement qdeDatabase = doc.createElement("database");
    // do not save database server and database name for now until we have some
    // concept how to update them in case they ever change
    //qdeDatabase.setAttribute("server", databaseserver);
    //qdeDatabase.setAttribute("name", database);
    if (!settings->databaseuser.isEmpty())
      qdeDatabase.setAttribute("user", settings->databaseuser);
    if (!settings->databasepassword.isEmpty())
      qdeDatabase.setAttribute("password", settings->databasepassword);
    if (qdeDatabase.hasAttributes())
      qdeBackends.appendChild(qdeDatabase);
  }

  QDomElement aktivtag = doc.createElement("aktives_konto");
  QString abt,ko,uko;
  int idx;
  abtList->getAktiv(abt,ko,uko,idx);
  aktivtag.setAttribute("abteilung",abt);
  aktivtag.setAttribute("konto",ko);
  aktivtag.setAttribute("unterkonto",uko);
  aktivtag.setAttribute("index",idx);
  generaltag.appendChild(aktivtag);

  root.appendChild(generaltag);

  for (AbteilungsListe::iterator abtPos=abtList->begin(); abtPos!=abtList->end(); ++abtPos) {
    QString abteilungstr=abtPos->first;
    QDomElement abttag = doc.createElement( "abteilung" );
    abttag.setAttribute("name",abteilungstr);
    bool abtIsEmpty=true;

    if (global) {
        if (abtList->hasBgColor(abteilungstr)) {
          abtIsEmpty=false;
          abttag.setAttribute("color",settings->color2str(abtList->getBgColor(abteilungstr)));
        }
    }

    KontoListe* kontoliste=&(abtPos->second);
    for (KontoListe::iterator kontPos=kontoliste->begin(); kontPos!=kontoliste->end(); ++kontPos) {
      QString kontostr=kontPos->first;
      QDomElement konttag = doc.createElement( "konto" );
      konttag.setAttribute("name",kontostr);

      bool kontIsEmpty=true;
      bool kontpers=((abtList->getKontoFlags(abteilungstr,kontostr))&UK_PERSOENLICH);
      if (kontpers) {
        kontIsEmpty=false;
        konttag.setAttribute("persoenlich","yes");
      }

      if (global) {
        if (abtList->hasBgColor(abteilungstr,kontostr)) {
          kontIsEmpty=false;
          konttag.setAttribute("color",settings->color2str(abtList->getBgColor(abteilungstr,kontostr)));
        }
      }

      UnterKontoListe* unterkontoliste=&(kontPos->second);
      for (UnterKontoListe::iterator ukontPos=unterkontoliste->begin();
           ukontPos!=unterkontoliste->end(); ++ukontPos) {
        QString unterkontostr=ukontPos->first;
        QDomElement ukonttag = doc.createElement( "unterkonto" );
        ukonttag.setAttribute("name",unterkontostr);

        bool ukontIsEmpty=true;
        bool ukontpers=(abtList->getUnterKontoFlags(abteilungstr,kontostr,unterkontostr)&UK_PERSOENLICH);
        if (ukontpers) {
          ukontIsEmpty=false;
          ukonttag.setAttribute("persoenlich","yes");
        }
        else if (kontpers) {
          ukontIsEmpty=false;
          ukonttag.setAttribute("persoenlich","no"); // Falls Einstellung vom zugeh. Konto abweicht.
        }

        if (global) {
          if (abtList->hasBgColor(abteilungstr,kontostr,unterkontostr)) {
            ukontIsEmpty=false;
            ukonttag.setAttribute("color",settings->color2str(abtList->getBgColor(abteilungstr,kontostr,unterkontostr)));
          }
        }

        if (!global) {
          QStringList bereitschaften= ukontPos->second.getBereitschaft();
          for (int i=0; i<bereitschaften.size(); i++)
          {
            QDomElement bertag = doc.createElement( "bereitschaft" );
            bertag.setAttribute("type",bereitschaften.at(i));
            ukonttag.appendChild (bertag);
          }
        }

        if ((!global)||settings->alwaysSaveEintrag) {
          EintragsListe* eintragsliste=&(ukontPos->second);

          for (EintragsListe::iterator etPos=eintragsliste->begin();
               etPos!=eintragsliste->end(); ++etPos) {
            if (!(etPos->second==UnterKontoEintrag())) {
              ukontIsEmpty=false;

              QString etStr=QString().setNum(etPos->first);
              QDomElement ettag = doc.createElement( "eintrag" );
              ettag.setAttribute("nummer",etStr);


              if (abtList->isPersoenlich(abteilungstr,kontostr,unterkontostr,etPos->first)) {
                ettag.setAttribute("persoenlich","yes");
              } else if (ukontpers) {
                ettag.setAttribute("persoenlich","no");
              }
              if (etPos->second.kommentar!="")
                ettag.setAttribute("kommentar",etPos->second.kommentar);
              if (!global) {
                if (etPos->second.sekunden!=0)
                  ettag.setAttribute("sekunden",etPos->second.sekunden);
                if (etPos->second.sekundenAbzur!=0)
                  ettag.setAttribute("abzurechnende_sekunden",etPos->second.sekundenAbzur);
                QSet<QString> specialRemunSet = etPos->second.getAchievedSpecialRemunSet();
                foreach (QString specialremun, specialRemunSet) {
                  QDomElement srtag = doc.createElement( "specialremun" );
                  srtag.setAttribute("name",specialremun);
                  ettag.appendChild(srtag);
                }
              }
              ukonttag.appendChild (ettag);
            }
          }
        }

        if (!ukontIsEmpty) {
          kontIsEmpty=false;
          if (ukontPos->second.getFlags()&IS_CLOSED)
            ukonttag.setAttribute("open","no");
          konttag.appendChild(ukonttag);
        }
      }
      if (!kontIsEmpty) {
        abtIsEmpty=false;
        if (kontPos->second.getFlags()&IS_CLOSED)
          konttag.setAttribute("open","no");
        abttag.appendChild(konttag);
      }
    }
    if (!abtIsEmpty) {
      if (abtPos->second.getFlags()&IS_CLOSED)
        abttag.setAttribute("open","no");
      root.appendChild( abttag );
      }
  }

  if (!global && pcl!=NULL) {
    QDomElement punchclocktag = doc.createElement( "punchclock" );
    for (auto pce: *pcl) {
      QDomElement pcentrytag= doc.createElement( "pcentry" );
      if (pce.first>pce.second) {
        pce.second=23*60*60+59*60;
      }
      pcentrytag.setAttribute("begin", QTime::fromMSecsSinceStartOfDay(pce.first*1000).toString("H:m"));
      pcentrytag.setAttribute("end", QTime::fromMSecsSinceStartOfDay(pce.second*1000).toString("H:m"));
      punchclocktag.appendChild(pcentrytag);
    }
    root.appendChild(punchclocktag);
  }

  QString filename(global ? "settings.xml" : "zeit-"+abtList->getDatum().toString("yyyy-MM-dd")+".xml");
  filename=configDir.filePath(filename);
  QFile fnew(filename + ".tmp");
  QDateTime filemod = QFileInfo(filename).lastModified();
  if (!global && settings->m_lastSave.isValid() && filemod.isValid() && filemod>settings->m_lastSave.addSecs(30)) {
      QMessageBox::StandardButton answer = QMessageBox::question(NULL, QObject::tr("sctime: saving settings"), QObject::tr("%1 has been modified since the last changes done by this sctime instance. Do you wanto to overwrite theses changes?").arg(fnew.fileName()));
      if (answer!=QMessageBox::Yes) {
         emit settingsWriteFailed("aborted");
         return;
      }
  }
  if (!fnew.open(QIODevice::WriteOnly)) {
      QMessageBox::critical(NULL, QObject::tr("sctime: saving settings"), QObject::tr("opening file %1 for writing failed. Please make sure the sctime settings directory is available. Details: %2").arg(fnew.fileName(), fnew.errorString()));
      emit settingsWriteFailed("write failed");
      return;
  }
  // may contain passwords and private data in general
  fnew.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
  const char xmlcharmap[] = "UTF-8";
  QTextStream stream(&fnew);
  stream.setEncoding(QStringConverter::Utf8);
  stream<<"<?xml version=\"1.0\" encoding=\""<< xmlcharmap <<"\"?>"<<Qt::endl;
  stream<<doc.toString()<<Qt::endl;
  fnew.close();
  QFile fcurrent(filename);
  if (global && settings->backupSettingsXml && fcurrent.exists()) {
    // Backup erstellen für settings.xml
    QFile fbackup(filename + ".bak");
    if (fbackup.exists()) fbackup.remove();
    if (!fcurrent.copy(fbackup.fileName()))
      QMessageBox::warning(NULL, QObject::tr("sctime: saving settings"),
                           QObject::tr("%1 cannot be copied to %2: %3").arg(filename, fbackup.fileName(), fcurrent.errorString()));
    else
      settings->backupSettingsXml = false;
  }
#ifndef WIN32
  // unter Windows funktioniert kein "rename", wenn es den Zielnamen schon gibt.
  // Doch unter UNIX kann ich damit Dateien atomar ersetzen.
  if (rename(fnew.fileName().toLocal8Bit(), filename.toLocal8Bit())!=0) {
      QMessageBox::information(NULL, QObject::tr("sctime: saving settings"),
                        QObject::tr("%1 cannot be renamed to %2: %3").arg(fnew.fileName(), filename, strerror(errno)));
      emit settingsWriteFailed("rename error");
      return;
  }
#else
  fcurrent.remove();
  if (!fnew.rename(filename)) {
    QMessageBox::critical(NULL, QObject::tr("sctime: saving settings"),
                         QObject::tr("%1 cannot be renamed to %2: %3").arg(fnew.fileName(), filename, fnew.errorString()));
    emit settingsWriteFailed("rename error");
    return;
  }
  #endif
#ifndef RESTCONFIG
  emit settingsPartWritten(global, abtList, pcl);
#else // RESTCONFIG
  // sync previously written files in browser
  EM_ASM(
      FS.syncfs(function (err) {});
  );
  // write files by rest api
  if (settings->restSaveOffline())
  {
    logError("skipping online writing");
    emit settingsPartWritten(global, abtList, pcl);
  } else {
    QByteArray ba;
    QTextStream bastream(&ba);
    bastream.setEncoding(QStringConverter::Utf8);
    bastream<<"<?xml version=\"1.0\" encoding=\""<< xmlcharmap <<"\"?>"<<Qt::endl;
    bastream<<doc.toString()<<Qt::endl;
    QString baseurl=getRestBaseUrl();
    QString postfix = "";
    if (!global) {
      postfix =  "&date=" + abtList->getDatum().toString("yyyy-MM-dd");
    }
    writeBytes(QUrl(baseurl + "/" + REST_SETTINGS_ENDPOINT + "?clientid=" + clientId + "&conflicttimeout=" + QString::number(conflicttimeout) + postfix), qCompress(ba));
  }

#endif // RESTCONFIG
#endif //NO_XML
  if (!global) {
     settings->m_lastSave = savetime;
  }
}

void XMLWriter::continueAfterWriting(bool global, AbteilungsListe* abtList, PunchClockList* pcl)
{
  /* we come here twice in writeAllCase. first after writing the global settings, and then after writing the settings of the day. after that we send the finished signal*/
  if (global&&writeAll) {
    writeAll=false;
    writeSettings(false);
  } else {
    emit settingsWritten();
  }
}