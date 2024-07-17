#include "checkindialog.h"
#include "xmlreader.h"
#include "sctimexmlsettings.h"
#include "util.h"
#include "punchclockchecker.h"

#define DATEFORMATSTR "dd.MM.yyyy"

// Constructor
CheckinDialog::CheckinDialog(SCTimeXMLSettings* settings, QNetworkAccessManager* networkAccessManager, QWidget *parent)
    : QDialog(parent), settings(settings), networkAccessManager(networkAccessManager)
{
    setupUi(this);
    abtList=NULL;
    connect(checkinButton, SIGNAL(clicked()), this, SLOT(checkin()));
    connect(skipButton, SIGNAL(clicked()), this, SLOT(skip()));
    connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));
    daySelector->addItems(CheckinDialog::listNotCheckedIn());
    connect(daySelector, &QListWidget::currentItemChanged, this, &CheckinDialog::openDateForItem);
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
CheckinDialog::~CheckinDialog()
{
}

void CheckinDialog::openDateForItem(QListWidgetItem *current, QListWidgetItem *previous) {
   if (current==NULL) {
    //TODO
    return;
   }
   QString datestr=current->text();
   openDate(QDate::fromString(datestr, DATEFORMATSTR));
}

void CheckinDialog::openDate(const QDate& date) {
    //todo: special cases if date is already open - use existing pointers to sync
    if (abtList!=NULL) {
      delete abtList;
    }
    if (pcl!=NULL) {
      delete pcl;
    }
    abtList=new AbteilungsListe(date, (KontoDatenInfo*)NULL);
    pcl=new PunchClockList();
        

    XMLReader* reader = new XMLReader(settings,networkAccessManager, false,false,true,abtList,pcl);
    connect(reader,&XMLReader::settingsRead,this,&CheckinDialog::loadDate);
    connect(reader,&XMLReader::settingsRead,reader,&XMLReader::deleteLater);
    reader->open();
}

void CheckinDialog::loadDate() {
  QRegularExpression apostrophExp=QRegularExpression("'");
    
  AbteilungsListe::iterator abtPos;

  onCallTable->setRowCount(0);
  hoursTable->setRowCount(0);

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
  settings->writeShellSkriptToStream(stream, abtList, pcl);

  commandEdit->setPlainText(filecontent);
}

QStringList CheckinDialog::listNotCheckedIn()
{
  // TODO: identify the right days
   QDate today=QDate::currentDate();
   QStringList result;
   for (int i=-6; i<=0; i++) {
     result.append(today.addDays(i).toString(DATEFORMATSTR));
   }
   return result;
}

void CheckinDialog::checkin()
{
  //TODO: checkin and switch to next day, instead of skip
  skip();
}

void CheckinDialog::skip()
{
  auto currentidx=daySelector->currentRow();
  if (currentidx+1<daySelector->count()) {
    daySelector->setCurrentRow(currentidx+1);
  }
}