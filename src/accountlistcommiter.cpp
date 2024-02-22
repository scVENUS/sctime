#include "accountlistcommiter.h"
#include "abteilungsliste.h"
#include "kontotreeview.h"
#include "sctimexmlsettings.h"
#include "punchclock.h"
#include "xmlwriter.h"
#include "xmlreader.h"

void AccountListCommiter::start() {
  abtList->kontoDatenInfoSuccess = true;
  kontoTree->flagClosedPersoenlicheItems();
  std::vector<int> columnwidthlist;
  kontoTree->getColumnWidthList(columnwidthlist);
  settings->setColumnWidthList(columnwidthlist);
  reader=NULL;
  if (!withoutSave) {
    writer=new XMLWriter(settings,abtList,pcl);
    connect(writer,&XMLWriter::settingsWritten, this, &AccountListCommiter::reloadAbtList);
    connect(writer, &XMLWriter::settingsWriteFailed, this, &AccountListCommiter::cleanupOnErr);
    writer->writeAllSettings(); // settings wont survive the reload
  } else {
    writer=NULL;
    reloadAbtList();
  }
}

void AccountListCommiter::reloadAbtList() {
  if (writer!=NULL) {
    disconnect(writer,&XMLWriter::settingsWritten, this, &AccountListCommiter::reloadAbtList);
    writer->deleteLater();
    writer=NULL;
  }
  diff = abtList->getZeitDifferenz();
  abtList->reload(data);
  reader = new XMLReader(settings, true, false, true, abtList, NULL);
  if (abtList!=abtListToday) {
     connect(reader,&XMLReader::settingsRead, this, &AccountListCommiter::writeAbtListToday);
  } else {
     connect(reader,&XMLReader::settingsRead, this, &AccountListCommiter::finish);
  }
  reader->open();
}

void AccountListCommiter::reloadAbtListToday() {
  reader = new XMLReader(settings, true, false, true, abtListToday, NULL);
  connect(reader,&XMLReader::settingsRead, this, &AccountListCommiter::finish);
  writer->deleteLater();
  writer=NULL;
  abtListToday->reload(data);
  reader->open();
}

void AccountListCommiter::writeAbtListToday() {
   disconnect(reader,&XMLReader::settingsRead, this, &AccountListCommiter::writeAbtListToday);
   reader->deleteLater();
   reader=NULL;
   //writer->deleteLater();
   writer=new XMLWriter(settings,abtListToday,pcl);
   connect(writer,&XMLWriter::settingsWritten, this, &AccountListCommiter::reloadAbtListToday);
   writer->writeAllSettings(); // settings wont survive the reload
}

void AccountListCommiter::finish() {
  reader->deleteLater();
  reader=NULL;
  kontoTree->load(abtList);
  abtList->setZeitDifferenz(diff);
  kontoTree->closeFlaggedPersoenlicheItems();
  emit finished();
}

void AccountListCommiter::cleanupOnErr() {
  if (writer!=NULL) {
     writer->deleteLater();
  }
  if (reader!=NULL) {
     reader->deleteLater();
  }
}