/*
    Copyright (C) 2025 science+computing ag
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

#include "dateoverviewwidget.h"
#include "sctimexmlsettings.h"
#include "util.h"
#include "punchclockchecker.h"

// Constructor
DateOverviewWidget::DateOverviewWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    onCallTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Account"));
    onCallTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Subaccount"));
    onCallTable->setHorizontalHeaderItem(2, new QTableWidgetItem("OnCallTimes"));

    hoursTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Account"));
    hoursTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Subaccount"));
    hoursTable->setHorizontalHeaderItem(2, new QTableWidgetItem("Hours"));
    hoursTable->setHorizontalHeaderItem(3, new QTableWidgetItem("Acc. Hours"));
    hoursTable->setHorizontalHeaderItem(4, new QTableWidgetItem("Special Remuns"));
    hoursTable->setHorizontalHeaderItem(5, new QTableWidgetItem("Comment"));

    punchClockTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Start"));
    punchClockTable->setHorizontalHeaderItem(1, new QTableWidgetItem("End"));
}

// Destructor
DateOverviewWidget::~DateOverviewWidget()
{
}

void DateOverviewWidget::loadDate(AbteilungsListe* abtList, PunchClockList* pcl, SCTimeXMLSettings* settings) {
  QRegularExpression apostrophExp=QRegularExpression("'");
    
  AbteilungsListe::iterator abtPos;

  onCallTable->setRowCount(0);
  hoursTable->setRowCount(0);

  if (abtList!=NULL) {
    for (abtPos=abtList->begin(); abtPos!=abtList->end(); ++abtPos) {
      QString abt=abtPos->first;
      KontoListe* kontoliste=&(abtPos->second);
      for (KontoListe::iterator kontPos=kontoliste->begin(); kontPos!=kontoliste->end(); ++kontPos) {
        UnterKontoListe* unterkontoliste=&(kontPos->second);
        for (UnterKontoListe::iterator ukontPos=unterkontoliste->begin(); ukontPos!=unterkontoliste->end(); ++ukontPos) {
          EintragsListe* eintragsliste=&(ukontPos->second);
          QStringList bereitschaften= eintragsliste->getBereitschaft();

          int rc=onCallTable->rowCount();
          onCallTable->setRowCount(rc+bereitschaften.size());
          for (int i=0; i<bereitschaften.size(); i++)
          {
            onCallTable->setItem(rc+i, 0, new QTableWidgetItem(kontPos->first));
            onCallTable->setItem(rc+i, 1, new QTableWidgetItem(ukontPos->first));
            onCallTable->setItem(rc+i, 2, new QTableWidgetItem(bereitschaften.at(i)));
          }
          for (EintragsListe::iterator etPos=eintragsliste->begin(); etPos!=eintragsliste->end(); ++etPos) {
            if ((etPos->second.sekunden!=0)||(etPos->second.sekundenAbzur!=0)) {
              int rc=hoursTable->rowCount();
              hoursTable->insertRow(rc);
              QString kommentar=etPos->second.kommentar.replace(apostrophExp,""); // remove apostrophes to ensure scripting does not break later on
              
              QString srstr="";
              QSet<QString> specialRemunSet = etPos->second.getAchievedSpecialRemunSet();
              foreach (QString specialremun, specialRemunSet) {
                if (srstr.isEmpty()) {
                    srstr=specialremun;
                } else {
                    srstr+="\n"+specialremun;
                }
              }
              hoursTable->setItem(rc, 0, new QTableWidgetItem(kontPos->first));
              hoursTable->setItem(rc, 1, new QTableWidgetItem(ukontPos->first));
              hoursTable->setItem(rc, 2, new QTableWidgetItem(QString("%1").arg(roundTo(1.0/3600*etPos->second.sekunden,0.01))));
              hoursTable->setItem(rc, 3, new QTableWidgetItem(QString("%1").arg(roundTo(1.0/3600*etPos->second.sekundenAbzur,0.01))));
              hoursTable->setItem(rc, 4, new QTableWidgetItem(srstr));
              hoursTable->setItem(rc, 5, new QTableWidgetItem(kommentar.simplified()));
              
            }
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
      punchClockTable->setRowCount(pcl->size());
      int i=0;
      for (auto pce: *pcl) {
        if (pce.first>pce.second) {
          pce.second=23*60*60+59*60;
        }
        punchClockTable->setItem(i, 0, new QTableWidgetItem(QTime::fromMSecsSinceStartOfDay(pce.first*1000).toString("H:mm")));
        punchClockTable->setItem(i, 1, new QTableWidgetItem(QTime::fromMSecsSinceStartOfDay(pce.second*1000).toString("H:mm")));
        i++;
      }
    } else {
      punchClockTable->setRowCount(0);
    }
  }
#endif
  onCallBox->setVisible(onCallTable->rowCount()>0);
  hoursBox->setVisible(hoursTable->rowCount()>0);
  punchClockBox->setVisible(punchClockTable->rowCount()>0);

  QByteArray filecontent;
  QTextStream stream(&filecontent);
  if (settings!=NULL) {
    settings->writeShellSkriptToStream(stream, abtList, pcl);
  }

  commandEdit->setPlainText(filecontent);
}

