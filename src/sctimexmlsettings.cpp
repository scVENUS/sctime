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
#include <QRect>
#include <QMessageBox>
#include <QApplication>
#include <QProcessEnvironment>
#include <QDebug>
#include <QRegularExpression>
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
#include "punchclock.h"
#ifndef DISABLE_PUNCHCLOCK
#include "punchclockchecker.h"
#endif
#include "xmlreader.h"
#include "xmlwriter.h"

/** Schreibt die Eintraege in ein Shellskript */
void SCTimeXMLSettings::writeShellSkript(AbteilungsListe* abtList, PunchClockList* pcl) {
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
  stream.setEncoding(charmap());
  writeShellSkriptToStream(stream,abtList,pcl);
  workfile.close();

  if (!workfile.rename(filename)) {
    QMessageBox::critical(NULL, QObject::tr("sctime: saving sh file"),
                         QObject::tr("%1 cannot be renamed to %2: %3").arg(workfile.fileName(), filename, workfile.errorString()));
  }
}

void SCTimeXMLSettings::writeShellSkriptToStream(QTextStream& stream, AbteilungsListe* abtList, PunchClockList* pcl)
{
  
  int sek, abzurSek;
  abtList->getGesamtZeit(sek,abzurSek);
  QRegularExpression apostrophExp=QRegularExpression("'");

  TimeCounter tc(sek), tcAbzur(abzurSek);

  const char * encodingName=QStringConverter::nameForEncoding(charmap());

  stream<<
           "#!/bin/sh\n"
           "# -*- coding: "<<encodingName<<" -*-\n\n"
           "set -e\n"
           "trap '[ $? -gt 0 ] && echo \"ABBRUCH in $PWD/$0 bei Zeile $LINENO - nicht alle Buchungen sind uebernommen\" >&2 && exit 1' 0"
           "\n\n"
           "# Zeit Aufrufe von sctime "<< qApp->applicationVersion() <<" generiert \n"
           "# Gesamtzeit: "<<tc.toString()<<"/"<<tcAbzur.toString()<<"\n"
           "ZEIT_ENCODING="<<encodingName<<"\n"
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
#ifndef DISABLE_PUNCHCLOCK
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
      stream<<Qt::endl<<"# Source intervals for zeitarbeit (they are consolidated according to legal requirements in the actual command):";
      stream<<Qt::endl<<"#";
      
      for (auto pce: *pcl) {
        if (pce.first>pce.second) {
          pce.second=23*60*60+59*60;
        }
        stream<<" "<<QTime::fromMSecsSinceStartOfDay(pce.first*1000).toString("H:mm")<<"-"<<QTime::fromMSecsSinceStartOfDay(pce.second*1000).toString("H:mm");
      }
      stream<<Qt::endl<<"zeitarbeit "<<abtList->getDatum().toString("yyyy-MM-dd")<<" "<<consolidatedIntervals;
      stream<<Qt::endl;
    }
  }
#endif
  stream<<Qt::endl;
}

// returns the encoding that the user has chosen by his locale settings
QStringConverter::Encoding SCTimeXMLSettings::charmap() {
#if defined(WIN32) || defined(Q_OS_MAC)
    return QStringConverter::Utf8;
#else
    QString codeset = QString(nl_langinfo(CODESET)).toUpper();
  // FIXME: this heuristics might go wrong in some special cases (for example euro sign and iso-8859-15 encoding), but there does 
  // not seem to be a better way using QT that works with all QT6 versions
  // the best way to fix encoding issues would probably be to have the command always output utf8, which would reduce complexity here
    if ((codeset!="UTF-8") || (codeset!="UTF8")) {
      return QStringConverter::Latin1;
    } else {
      return QStringConverter::Utf8;
    }
#endif
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

