/*

    Copyright (C) 2024 science+computing ag
       Authors: Florian Schmitt, Marcus Camen et al.

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

#include "conflictdialog.h"

#include "xmlreader.h"
#include "qmessagebox.h"
#include "timemainwindow.h"
#include "globals.h"
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

ConflictDialog::ConflictDialog(SCTimeXMLSettings* settings, QNetworkAccessManager* networkAccessManager, QDate targetdate, bool global, QDomDocument docRemote, TimeMainWindow* tmw): 
    QDialog(tmw), tmw(tmw), settings(settings), global(global), docRemote(docRemote), targetdate(targetdate), networkAccessManager(networkAccessManager) {
    setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    connect(buttonReplace,&QPushButton::pressed, this, &ConflictDialog::performReplace);
    connect(buttonMerge,&QPushButton::pressed, this, &ConflictDialog::performMerge);
    connect(buttonClose,&QPushButton::pressed, this, &ConflictDialog::performClose);
    connect(buttonKeep,&QPushButton::pressed, this, &ConflictDialog::performKeep);
}

ConflictDialog::ConflictDialog(SCTimeXMLSettings* settings, QNetworkAccessManager* networkAccessManager, QDate targetdate, bool global, QDomDocument docRemote, TimeMainWindow* tmw, QDomDocument docLocal): 
    QDialog(tmw), tmw(tmw), settings(settings), global(global), docRemote(docRemote), targetdate(targetdate), docLocal(docLocal), networkAccessManager(networkAccessManager) {
    setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    connect(buttonReplace,&QPushButton::pressed, this, &ConflictDialog::performReplace);
    connect(buttonMerge,&QPushButton::pressed, this, &ConflictDialog::performMerge);
    connect(buttonClose,&QPushButton::pressed, this, &ConflictDialog::performClose);
    connect(buttonKeep,&QPushButton::pressed, this, &ConflictDialog::performKeep);
}

void ConflictDialog::performMerge() {

    AbteilungsListe* conflictedAbtList=NULL;
    PunchClockList * conflictedPunchClockList=NULL;
    /*bool istoday=false;
    if (tmw->abtList->getDatum()==targetdate) {
      conflictedAbtList=tmw->abtList;
      conflictedPunchClockList=tmw->m_punchClockList;
    } else if (tmw->abtListToday->getDatum()==targetdate) {
      istoday=true;
      conflictedAbtList=tmw->abtListToday;
      conflictedPunchClockList=tmw->m_punchClockListToday;
    } else {
      // this should not happen, but handle anyways, just in case...
      errorDialog();
      return;
    }*/

    bool istoday=false;
    if (tmw->abtList->getDatum()==targetdate) {
      if (!docLocal.isNull()) {
        // load local document
        XMLReader reader(settings, networkAccessManager, global, false, true, tmw->abtList, tmw->m_punchClockList);
        reader.fillSettingsFromDocument(docLocal, settings);
      } 
      conflictedAbtList=new AbteilungsListe(targetdate, tmw->abtList);
      conflictedPunchClockList=new PunchClockList();
    } else if (tmw->abtListToday->getDatum()==targetdate) {
      istoday=true;
      if (!docLocal.isNull()) {
        // load local document
        XMLReader reader(settings, networkAccessManager, global, false, true, tmw->abtListToday, tmw->m_punchClockListToday);
        reader.fillSettingsFromDocument(docLocal, settings);
      } 
      conflictedAbtList=new AbteilungsListe(targetdate, tmw->abtListToday);
      conflictedPunchClockList=new PunchClockList();
    } else {
      // this should not happen, but handle anyways, just in case...
      errorDialog();
      return;
    }

    XMLReader reader(settings, networkAccessManager, global, false, true, conflictedAbtList, conflictedPunchClockList);
    
    reader.fillSettingsFromDocument(docRemote, settings);
    if (istoday) {
       mergeAbtList(tmw->abtListToday, conflictedAbtList);
    } else {
       mergeAbtList(tmw->abtList, conflictedAbtList);
    }
    emit finished(2);
}

void ConflictDialog::performReplace() {

    AbteilungsListe* conflictedAbtList=NULL;
    PunchClockList * conflictedPunchClockList=NULL;
    bool istoday=false;
    if (tmw->abtList->getDatum()==targetdate) {
      conflictedAbtList=new AbteilungsListe(targetdate, tmw->abtList);
      conflictedPunchClockList=new PunchClockList();
    } else if (tmw->abtListToday->getDatum()==targetdate) {
      istoday=true;
      conflictedAbtList=new AbteilungsListe(targetdate, tmw->abtListToday);
      conflictedPunchClockList=new PunchClockList();
    } else {
      // this should not happen, but handle anyways, just in case...
      errorDialog();
      return;
    }

    XMLReader reader(settings, networkAccessManager, global, false, true, conflictedAbtList, conflictedPunchClockList);
    
    reader.fillSettingsFromDocument(docRemote, settings);
    if (!istoday) {
       if (tmw->abtList==tmw->abtListToday) {
         delete tmw->abtList;
         tmw->abtList=conflictedAbtList;
         tmw->abtListToday=conflictedAbtList;
       } else {
         delete tmw->abtList;
         tmw->abtList=conflictedAbtList;
       }
     } else {
       delete tmw->abtListToday;
       tmw->abtListToday=conflictedAbtList;
     }
     emit finished(1);

}

// this function terminates the application hard, without saving files, etc. This avoids additional conflicts with other sessions.
void ConflictDialog::performClose() {
    qApp->exit();
#ifdef __EMSCRIPTEN__
    emscripten_run_script(QString(" document.documentElement.innerHTML = \"<html><head><title>sctime - session closed</title></head><body>This session has been closed.</body></html>\"").toUtf8().data());
#else
    exit(0);
#endif
}

void ConflictDialog::performKeep() {
  if (!docLocal.isNull()) {
     // load local document
     XMLReader reader(settings, networkAccessManager, global, false, true, tmw->abtList, tmw->m_punchClockList);
     reader.fillSettingsFromDocument(docLocal, settings);
 } 
  emit finished(1);
}

void ConflictDialog::errorDialog() {
    QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
            tr("sctime: unresolvable conflict"),
            tr("There seems to be a conflict with another session that could not be resolved. Please check your entries."),
            QMessageBox::Ok, dynamic_cast<QWidget*>(this->parent()));
    connect(msgbox, &QMessageBox::finished,
    [=](){
      msgbox->deleteLater();
    });
    msgbox->open();
    msgbox->raise();
}


void ConflictDialog::mergeAbtList(AbteilungsListe* target, AbteilungsListe* other) {
  for (AbteilungsListe::iterator abtPos=other->begin(); abtPos!=other->end(); ++abtPos) {
    QString abteilungstr=abtPos->first;
    KontoListe* kontoliste=&(abtPos->second);
    for (KontoListe::iterator kontPos=kontoliste->begin(); kontPos!=kontoliste->end(); ++kontPos) {
      QString kontostr=kontPos->first;

      //bool kontpers=((other->getKontoFlags(abteilungstr,kontostr))&UK_PERSOENLICH);

      UnterKontoListe* unterkontoliste=&(kontPos->second);
      for (UnterKontoListe::iterator ukontPos=unterkontoliste->begin();
           ukontPos!=unterkontoliste->end(); ++ukontPos) {
        QString unterkontostr=ukontPos->first;
        
        QStringList bereitschaften= ukontPos->second.getBereitschaft();
        if (bereitschaften.size()>0) {
            UnterKontoListe::iterator itUkTarget;
            UnterKontoListe* uklTarget;
            target->findUnterKonto(itUkTarget, uklTarget, abteilungstr, kontostr, unterkontostr);
            itUkTarget->second.setBereitschaft(bereitschaften);
        }

        EintragsListe* eintragsliste=&(ukontPos->second);

        for (EintragsListe::iterator etPos=eintragsliste->begin(); etPos!=eintragsliste->end(); ++etPos) {
          if (!(etPos->second==UnterKontoEintrag())) {
              EintragsListe::iterator itEtTarget;
              EintragsListe* eintragslisteTarget;
              int idx;
              QString comment=etPos->second.kommentar;
              bool found=target->findDuplicateEntry(itEtTarget, eintragslisteTarget, idx, abteilungstr, kontostr, unterkontostr, etPos->second);
              if (!found||(itEtTarget->second.sekunden!=etPos->second.sekunden)||(itEtTarget->second.sekundenAbzur!=etPos->second.sekundenAbzur)) {
                   idx=target->insertEintrag(abteilungstr, kontostr, unterkontostr);
                   target->findEintrag(itEtTarget, eintragslisteTarget, abteilungstr, kontostr, unterkontostr, idx);
                   itEtTarget->second=etPos->second;
                 }
          }
        }
      }
    }
  }
}