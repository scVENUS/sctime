/*
    Copyright (C) 2018 science+computing ag
       Authors: Florian Schmitt et al.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "sctimexmlsettings.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QTextCodec>
#include <QRect>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QApplication>
#include <QProcessEnvironment>
#include <QDebug>
#ifndef WIN32
#include <langinfo.h>
#include <errno.h>
#else
#include <windows.h>
#endif
#include "qdom.h"
#include "abteilungsliste.h"
#include "punchclock.h"
#include "globals.h"
#include "timecounter.h"
#define WIN_CODEC "utf8"
#include "globals.h"
#include "util.h"
#include "punchclockchecker.h"
#include "xmlreader.h"
#include "xmlwriter.h"

/** Schreibt die Eintraege in ein Shellskript */
void SCTimeXMLSettings::writeShellSkript(AbteilungsListe* abtList, PunchClockList* pcl)
{
  if (abtList->checkInState()) {
      trace(QObject::tr("Shell script not written because it has already been checked in."));
      return;
  }
  QString filename=configDir.filePath("zeit-"+abtList->getDatum().toString("yyyy-MM-dd")+".sh");
  QString tmpfilename=filename+".tmp";
  QFile workfile(tmpfilename);
  QFile targetfile(filename);

  // we remove the target file - we have all the data in the xml file anyways and can regenerate it even if we crash
  // this ensures that no other tools try to read an unfinished or obsolete sh file
  targetfile.remove();

  if (!workfile.open(QIODevice::WriteOnly)) {
      QMessageBox::warning(NULL, QObject::tr("sctime: writing shell script"), workfile.fileName() + ": " + workfile.errorString());
      return;
  }
  QTextStream stream( & workfile);
  stream.setCodec(charmap());
  int sek, abzurSek;
  abtList->getGesamtZeit(sek,abzurSek);
  QRegExp apostrophExp=QRegExp("'");

  TimeCounter tc(sek), tcAbzur(abzurSek);
  stream<<
           "#!/bin/sh\n"
           "# -*- coding: "<<charmap()<<" -*-\n\n"
           "set -e\n"
           "trap '[ $? -gt 0 ] && echo \"ABBRUCH in $PWD/$0 bei Zeile $LINENO - nicht alle Buchungen sind uebernommen\" >&2 && exit 1' 0"
           "\n\n"
           "# Zeit Aufrufe von sctime "<< qApp->applicationVersion() <<" generiert \n"
           "# Gesamtzeit: "<<tc.toString()<<"/"<<tcAbzur.toString()<<"\n"
           "ZEIT_ENCODING="<<charmap()<<"\n"
           "export ZEIT_ENCODING\n";

  AbteilungsListe::iterator abtPos;

  for (abtPos=abtList->begin(); abtPos!=abtList->end(); ++abtPos) {
    QString abt=abtPos->first;
    KontoListe* kontoliste=&(abtPos->second);
    bool firstInBereich=true;
    for (KontoListe::iterator kontPos=kontoliste->begin(); kontPos!=kontoliste->end(); ++kontPos) {
      UnterKontoListe* unterkontoliste=&(kontPos->second);
      for (UnterKontoListe::iterator ukontPos=unterkontoliste->begin(); ukontPos!=unterkontoliste->end(); ++ukontPos) {
        EintragsListe* eintragsliste=&(ukontPos->second);
        QStringList bereitschaften= eintragsliste->getBereitschaft();
        if ((bereitschaften.size()&&firstInBereich)) {
          stream<<"\n# Bereich: "<<abt<<"\n";
          firstInBereich=false;
        }
        for (int i=0; i<bereitschaften.size(); i++)
        {
           stream<<"zeitbereit "<<abtList->getDatum().toString("yyyy-MM-dd")<<" "<<
                 kontPos->first<<" "<<ukontPos->first<<"\t"<<bereitschaften.at(i)<<"\n";
        }
        for (EintragsListe::iterator etPos=eintragsliste->begin(); etPos!=eintragsliste->end(); ++etPos) {
          if ((etPos->second.sekunden!=0)||(etPos->second.sekundenAbzur!=0)) {
             if (firstInBereich) {
               stream<<"\n# Bereich: "<<abt<<"\n";
               firstInBereich=false;
             }
             QString kommentar=etPos->second.kommentar.replace(apostrophExp,""); // Apostrophe nicht in Skript speichern!
             
             QString srstr="";
             QSet<QString> specialRemunSet = etPos->second.getAchievedSpecialRemunSet();
             foreach (QString specialremun, specialRemunSet) {
               if (srstr.isEmpty()) {
                  srstr=specialremun;
               } else {
                  srstr+=","+specialremun;
               }
             }
             if (!srstr.isEmpty()) {
               srstr="--sonderzeiten='"+srstr+"' ";
             }
             stream<<zeitKommando<<" "<<srstr<<
                     abtList->getDatum().toString("yyyy-MM-dd")<<" "<<
                     kontPos->first<<" "<<ukontPos->first<<"\t"<<
                     roundTo(1.0/3600*etPos->second.sekunden,0.01)<<"/"<<
                     roundTo(1.0/3600*etPos->second.sekundenAbzur,0.01)<<
                     " \'"<<kommentar.simplified()<<"\'\n";
          }
        }
      }
    }
  }

  if (pcl!=NULL) {
     PunchClockStateBase* pcc;
    // TODO: implement and use factory
    #if PUNCHCLOCKDE23
      pcc=new PunchClockStateDE23();
    #else
      pcc=new PunchClockStateNoop();
    #endif
    QString consolidatedIntervals=pcc->getConsolidatedIntervalString(pcl);
    delete pcc;
    if (!consolidatedIntervals.isEmpty()) {
      stream<<endl<<"# Source intervals for zeitarbeit (they are consolidated according to legal requirements in the actual command):";
      stream<<endl<<"#";
      
      for (auto pce: *pcl) {
        if (pce.first>pce.second) {
          pce.second=23*60*60+59*60;
        }
        stream<<" "<<QTime::fromMSecsSinceStartOfDay(pce.first*1000).toString("H:mm")<<"-"<<QTime::fromMSecsSinceStartOfDay(pce.second*1000).toString("H:mm");
      }
      stream<<endl<<"zeitarbeit "<<abtList->getDatum().toString("yyyy-MM-dd")<<" "<<consolidatedIntervals;
      stream<<endl;
    }
  }

  stream<<endl;
  workfile.close();

  if (!workfile.rename(filename)) {
    QMessageBox::critical(NULL, QObject::tr("sctime: saving sh file"),
                         QObject::tr("%1 cannot be renamed to %2: %3").arg(workfile.fileName(), filename, workfile.errorString()));
  }
}

// returns the encoding that the user has chosen by his locale settings
const char* SCTimeXMLSettings::charmap() {
#if defined(WIN32) || defined(Q_OS_MAC)
    return "UTF-8";
#else
    return nl_langinfo(CODESET); // has same result as the command "locale charmap"
#endif
}

void SCTimeXMLSettings::readSettings(AbteilungsListe* abtList, PunchClockList* pcl)
{
  abtList->clearKonten();
  readSettings(true, abtList, pcl);
}

void SCTimeXMLSettings::continueAfterReading(bool global, AbteilungsListe* abtList, PunchClockList* pcl)
{
  /* we come here twice. first after reading the gobal settings, and then after reading the settings of the day. after that we send the finished signal*/
  if (global) {
    readSettings(false, abtList, pcl);
  } else {
    emit settingsRead();
  }
}

void SCTimeXMLSettings::continueAfterWriting(bool global, AbteilungsListe* abtList, PunchClockList* pcl)
{
  /* we come here twice. first after writing the gobal settings, and then after writing the settings of the day. after that we send the finished signal*/
  if (global) {
    writeSettings(false, abtList, pcl);
  } else {
    emit settingsWritten();
  }
}

void SCTimeXMLSettings::readSettings()
{
  // Nur globale Einstellungen lesen
  readSettings(true, NULL, NULL);
}

int SCTimeXMLSettings::compVersion(const QString& v1, const QString& v2)
{
  QString version1(v1);
  QString version2(v2);
  if (version1.startsWith("v")) {
    version1=version1.remove(0,1);
  }
  if (version2.startsWith("v")) {
    version2=version2.remove(0,1);
  }
  QStringList v1list=version1.split(".");
  QStringList v2list=version2.split(".");
  for (int i=0; i<v1list.size() ; i++) {
     if (i>=v2list.size())
	return 1;
     int v1=v1list[i].toInt();
     int v2=v2list[i].toInt();
     if (v1>v2) {
	return 1;
     } else
     if (v1<v2) {
	return -1;
     } 
  }
  return 0; 
}

/**
 * Liest alle Einstellungen.
 */

void SCTimeXMLSettings::readSettings(bool global, AbteilungsListe* abtList, PunchClockList* pcl)
{
  XMLReader *reader = new XMLReader(this, global, abtList, pcl);
  reader->open();
}

/** Schreibt saemtliche Einstellungen und Eintraege auf Platte */
bool SCTimeXMLSettings::writeSettings(AbteilungsListe* abtList, PunchClockList* pcl)
{
  bool success;
  // Globale Einstellungen
  success = writeSettings(true, abtList, pcl);
  // Einstellungen fuer den aktuellen Tag
  success = writeSettings(false, abtList, pcl) && success;
  return success;
}

/**
 * Schreibt die Einstellungen und Eintraege auf Platte. Wenn global==true,
 * werden globale Einstellungen fuer alle Tage gespeichert, sonst nur
 * fuer das aktuelle Datum.
 */
bool SCTimeXMLSettings::writeSettings(bool global, AbteilungsListe* abtList, PunchClockList* pcl)
{
  if ((abtList->checkInState())&&(!global)) {
      trace(QObject::tr("zeit-DAY.sh not written because it has already been checked in"));
      // nothing to do, so just return true
      return true;
  }
  #ifndef NO_XML
  QDomDocument doc("settings");

  QDomElement root = doc.createElement( "sctime" );
  // TODO: XML/HTML-Quoting
  root.setAttribute("version", qApp->applicationVersion());
  doc.appendChild( root );
  QDomElement generaltag = doc.createElement( "general" );

  if (global) {
    QDomElement timeinctag = doc.createElement( "timeincrement" );
    timeinctag.setAttribute("seconds",timeInc);
    generaltag.appendChild(timeinctag);

    QDomElement pcctag = doc.createElement( "pccdata" );
    pcctag.setAttribute("current",m_currentPCCdata);
    pcctag.setAttribute("previous",m_prevPCCdata);
    generaltag.appendChild(pcctag);

    QDomElement fasttimeinctag = doc.createElement( "fasttimeincrement" );
    fasttimeinctag.setAttribute("seconds",fastTimeInc);
    generaltag.appendChild(fasttimeinctag);

    QDomElement zeitkommandotag = doc.createElement( "zeitkommando" );
    zeitkommandotag.appendChild(doc.createTextNode(zeitKommando));
    generaltag.appendChild(zeitkommandotag);

    if (!zeitKontenKommando().isEmpty()) {
      QDomElement zeitkontenkommandotag = doc.createElement( "zeitkontenkommando" );
      zeitkontenkommandotag.appendChild(doc.createTextNode(zeitKontenKommando()));
      generaltag.appendChild(zeitkontenkommandotag);
    }

    if (m_workingTimeWarnings==false) {
        QDomElement maxworktag = doc.createElement( "working_time_warnings" );
        maxworktag.setAttribute("on","no");
        generaltag.appendChild(maxworktag);
    }

    QDomElement mainwindowpositiontag = doc.createElement( "windowposition" );
    mainwindowpositiontag.setAttribute("x",mainwindowPosition.x());
    mainwindowpositiontag.setAttribute("y",mainwindowPosition.y());
    generaltag.appendChild(mainwindowpositiontag);

    QDomElement mainwindowsizetag = doc.createElement("windowsize");
    mainwindowsizetag.setAttribute("width",mainwindowSize.width());
    mainwindowsizetag.setAttribute("height",mainwindowSize.height());
    generaltag.appendChild(mainwindowsizetag);

    for (unsigned int i=0; i<columnwidth.size() - 1; i++) { // do not save the width of the comment column
        QDomElement columnwidthtag = doc.createElement("column");
        columnwidthtag.setAttribute("width",columnwidth[i]);
        generaltag.appendChild(columnwidthtag);
    }

    QDomElement defcommentdm = doc.createElement("defcommentdisplay");
    QString dm;
    switch(defCommentDisplayMode()) {
      case DM_NOTUSEDBOLD: dm="DefaultCommentsNotUsedBold"; break;
      case DM_NOTBOLD: dm="NotBold"; break;
      default: dm ="DefaultCommentsBold"; break;
    }
    defcommentdm.setAttribute("mode",dm);
    generaltag.appendChild(defcommentdm);

    QDomElement saveeintragtag = doc.createElement("saveeintrag");
    QString always="no";
    if (alwaysSaveEintrag) always="yes";
    saveeintragtag.setAttribute("always",always);
    generaltag.appendChild(saveeintragtag);

    QDomElement powerusertag = doc.createElement("poweruserview");
    QString on="no";
    if (powerUserView()) on="yes";
    powerusertag.setAttribute("on",on);
    generaltag.appendChild(powerusertag);

    QDomElement persoenlichekontensummentag = doc.createElement("persoenliche_kontensumme");
    on="no";
    if (persoenlicheKontensumme()) on="yes";
    persoenlichekontensummentag.setAttribute("on",on);
    generaltag.appendChild(persoenlichekontensummentag);

    QDomElement typecolumntag = doc.createElement("typecolumn");
    on="no";
    if (showTypeColumn()) on="yes";
    typecolumntag.setAttribute("show",on);
    generaltag.appendChild(typecolumntag);
    
    QDomElement pspcolumntag = doc.createElement("pspcolumn");
    on="no";
    if (showPSPColumn()) on="yes";
    pspcolumntag.setAttribute("show",on);
    generaltag.appendChild(pspcolumntag);
    
    QDomElement srseltag = doc.createElement("specialremunselector");
    on="no";
    if (showSpecialRemunSelector()) on="yes";
    srseltag.setAttribute("show",on);
    generaltag.appendChild(srseltag);
    
    QDomElement usedefaultcommentifuniquetag = doc.createElement("usedefaultcommentifunique");
    on="no";
    if (useDefaultCommentIfUnique()) on="yes";
    usedefaultcommentifuniquetag.setAttribute("on",on);
    generaltag.appendChild(usedefaultcommentifuniquetag);

    if (useCustomFont()) {
        QDomElement customfonttag = doc.createElement("customfont");
        QString size="";
        size=size.setNum(customFontSize());
        customfonttag.setAttribute("family",customFont());
        customfonttag.setAttribute("size",size);
        generaltag.appendChild(customfonttag);
    }

    QDomElement singleclicktag = doc.createElement("singleclickactivation");
    on="no";
    if (singleClickActivation()) on="yes";
    singleclicktag.setAttribute("on",on);
    generaltag.appendChild(singleclicktag);

    QDomElement warnISO8859tag = doc.createElement("warnISO8859");
    on="no";
    if (warnISO8859()) on="yes";
    warnISO8859tag.setAttribute("on",on);
    generaltag.appendChild(warnISO8859tag);

    QDomElement sortByCommentTextTag = doc.createElement("sortByCommentText");
    on="no";
    if (sortByCommentText()) on="yes";
    sortByCommentTextTag.setAttribute("on",on);
    generaltag.appendChild(sortByCommentTextTag);

    QDomElement dragndroptag = doc.createElement("dragndrop");
    on="no";
    if (dragNDrop()) on="yes";
    dragndroptag.setAttribute("on",on);
    generaltag.appendChild(dragndroptag);

    QDomElement overtimeregulatedmodetag = doc.createElement("overtimeregulatedmode");
    on =overtimeRegulatedModeActive() ? "yes" : "no";
    overtimeregulatedmodetag.setAttribute("on",on);
    generaltag.appendChild(overtimeregulatedmodetag);

    QDomElement overtimeothermodetag = doc.createElement("overtimeothermode");
    on =overtimeOtherModeActive() ? "yes" : "no";
    overtimeothermodetag.setAttribute("on",on);
    generaltag.appendChild(overtimeothermodetag);

    QDomElement nightmodetag = doc.createElement("nightmode");
    on =nightModeActive() ? "yes" : "no";
    nightmodetag.setAttribute("on",on);
    generaltag.appendChild(nightmodetag);

    QDomElement lastrecordedtimestamptag = doc.createElement("lastrecordedtimestamp");
    QString ts = lastRecordedTimestamp().toString(timestampFormat());
    lastrecordedtimestamptag.setAttribute("value",ts);
    generaltag.appendChild(lastrecordedtimestamptag);

    QDomElement publicholidaymodetag = doc.createElement("publicholidaymode");
    on =publicHolidayModeActive() ? "yes" : "no";
    publicholidaymodetag.setAttribute("on",on);
    generaltag.appendChild(publicholidaymodetag);

    QDomElement kontodlgwindowpositiontag = doc.createElement( "kontodlgwindowposition" );
    kontodlgwindowpositiontag.setAttribute("x",unterKontoWindowPosition.x());
    kontodlgwindowpositiontag.setAttribute("y",unterKontoWindowPosition.y());
    generaltag.appendChild(kontodlgwindowpositiontag);

    QDomElement kontodlgwindowsizetag = doc.createElement("kontodlgwindowsize");
    kontodlgwindowsizetag.setAttribute("width",unterKontoWindowSize.width());
    kontodlgwindowsizetag.setAttribute("height",unterKontoWindowSize.height());
    generaltag.appendChild(kontodlgwindowsizetag);

    for (unsigned int i=0; i<defaultcommentfiles.size(); i++) {
        QDomElement defaultcommentfiletag = doc.createElement("defaultcommentsfile");
        defaultcommentfiletag.setAttribute("name",defaultcommentfiles[i]);
        generaltag.appendChild(defaultcommentfiletag);
    }

    QDomElement qdeBackends = doc.createElement("backends");
    qdeBackends.setAttribute("names", backends);
    generaltag.appendChild(qdeBackends);

    QDomElement qdeDatabase = doc.createElement("database");
    // do not save database server and database name for now until we have some
    // concept how to update them in case they ever change
    //qdeDatabase.setAttribute("server", databaseserver);
    //qdeDatabase.setAttribute("name", database);
    if (!databaseuser.isEmpty())
      qdeDatabase.setAttribute("user", databaseuser);
    if (!databasepassword.isEmpty())
      qdeDatabase.setAttribute("password", databasepassword);
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
          abttag.setAttribute("color",color2str(abtList->getBgColor(abteilungstr)));
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
          konttag.setAttribute("color",color2str(abtList->getBgColor(abteilungstr,kontostr)));
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
            ukonttag.setAttribute("color",color2str(abtList->getBgColor(abteilungstr,kontostr,unterkontostr)));
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

        if ((!global)||alwaysSaveEintrag) {
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

  QString filename(global ? "settings.xml" : "zeit-"+abtList->getDatum().toString("yyyy-MM-dd")+".xml");
#ifndef RESTCONFIG
  filename=configDir.filePath(filename);
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

  QFile fnew(filename + ".tmp");
  QDateTime filemod = QFileInfo(filename).lastModified();
  if (!global && m_lastSave.isValid() && filemod.isValid() && filemod>m_lastSave.addSecs(30)) {
      QMessageBox::StandardButton answer = QMessageBox::question(NULL, QObject::tr("sctime: saving settings"), QObject::tr("%1 has been modified since the last changes done by this sctime instance. Do you wanto to overwrite theses changes?").arg(fnew.fileName()));
      if (answer!=QMessageBox::Yes) {
         return false;
      }
  }
  if (!fnew.open(QIODevice::WriteOnly)) {
      QMessageBox::critical(NULL, QObject::tr("sctime: saving settings"), QObject::tr("opening file %1 for writing failed. Please make sure the sctime settings directory is available. Details: %2").arg(fnew.fileName(), fnew.errorString()));
      return false;
  }
  // may contain passwords and private data in general
  fnew.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
  const char xmlcharmap[] = "UTF-8";
  QTextStream stream(&fnew);
  stream.setCodec(xmlcharmap);
  stream<<"<?xml version=\"1.0\" encoding=\""<< xmlcharmap <<"\"?>"<<endl;
  stream<<doc.toString()<<endl;
  fnew.close();
  QFile fcurrent(filename);
  if (global && backupSettingsXml && fcurrent.exists()) {
    // Backup erstellen für settings.xml
    QFile fbackup(filename + ".bak");
    if (fbackup.exists()) fbackup.remove();
    if (!fcurrent.copy(fbackup.fileName()))
      QMessageBox::warning(NULL, QObject::tr("sctime: saving settings"),
                           QObject::tr("%1 cannot be copied to %2: %3").arg(filename, fbackup.fileName(), fcurrent.errorString()));
    else
      backupSettingsXml = false;
  }
  emit settingsPartWritten(global, abtList, pcl);
#ifndef WIN32
  // unter Windows funktioniert kein "rename", wenn es den Zielnamen schon gibt.
  // Doch unter UNIX kann ich damit Dateien atomar ersetzen.
  if (rename(fnew.fileName().toLocal8Bit(), filename.toLocal8Bit())!=0) {
      QMessageBox::information(NULL, QObject::tr("sctime: saving settings"),
                        QObject::tr("%1 cannot be renamed to %2: %3").arg(fnew.fileName(), filename, strerror(errno)));
      return false;
  }
#else
  fcurrent.remove();
  if (!fnew.rename(filename)) {
    QMessageBox::critical(NULL, QObject::tr("sctime: saving settings"),
                         QObject::tr("%1 cannot be renamed to %2: %3").arg(fnew.fileName(), filename, fnew.errorString()));
    return false;
  }
  #endif

#else // RESTCONFIG
  QByteArray ba;
  const char xmlcharmap[] = "UTF-8";
  QTextStream stream(&ba);
  stream.setCodec(xmlcharmap);
  stream<<"<?xml version=\"1.0\" encoding=\""<< xmlcharmap <<"\"?>"<<endl;
  stream<<doc.toString()<<endl;
  XMLWriter* writer=new XMLWriter(this, global, abtList);
  auto env=QProcessEnvironment::systemEnvironment();
  QString user=env.value("SCTIME_USER");
  QString baseurl=env.value("SCTIME_BASE_URL");
  writer->writeBytes(QUrl(baseurl+"/../accountingdata/"+user+"/"+filename), ba);
#endif // RESTCONFIG
#endif //NO_XML
  if (!global) {
     m_lastSave = QDateTime::currentDateTime();
  }
  return true;
}

QString SCTimeXMLSettings::color2str(const QColor& color)
{
  return QString("#%1%2%3").arg(color.red(),2,16,QChar('0')).arg(color.green(),2,16,QChar('0')).arg(color.blue(),2,16,QChar('0'));
}

QColor SCTimeXMLSettings::str2color(const QString& str)
{
  if (str.length()!=7)
    return Qt::white;
  QColor color;
  color.setRed(str.mid(1,2).toInt(NULL,16));
  color.setGreen(str.mid(3,2).toInt(NULL,16));
  color.setBlue(str.mid(5,2).toInt(NULL,16));
  return color;
}

