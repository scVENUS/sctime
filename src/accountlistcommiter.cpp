#include "accountlistcommiter.h"
#include "abteilungsliste.h"
#include "kontotreeview.h"
#include "sctimexmlsettings.h"
#include "punchclock.h"
#include "xmlwriter.h"

void AccountListCommiter::start() {
  abtList->kontoDatenInfoSuccess = true;
  kontoTree->flagClosedPersoenlicheItems();
  std::vector<int> columnwidthlist;
  kontoTree->getColumnWidthList(columnwidthlist);
  settings->setColumnWidthList(columnwidthlist);
  writer=new XMLWriter(settings,abtList,pcl);
  connect(writer,&XMLWriter::settingsWritten, this, &AccountListCommiter::reloadAbtList);
  connect(writer, &XMLWriter::settingsWriteFailed, this, &AccountListCommiter::cleanupOnErr);
  writer->writeAllSettings(); // settings wont survive the reload
}

void AccountListCommiter::reloadAbtList() {
  disconnect(writer,&XMLWriter::settingsWritten, this, &AccountListCommiter::reloadAbtList);
  diff = abtList->getZeitDifferenz();
  abtList->reload(data);
  if (abtList!=abtListToday) {
     connect(settings,&SCTimeXMLSettings::settingsRead, this, &AccountListCommiter::writeAbtListToday);
  } else {
     connect(settings,&SCTimeXMLSettings::settingsRead, this, &AccountListCommiter::finish);
  }
  settings->readSettings(abtList, NULL);
}

void AccountListCommiter::reloadAbtListToday() {
  connect(settings,&SCTimeXMLSettings::settingsRead, this, &AccountListCommiter::finish);
  writer->deleteLater();
  abtListToday->reload(data);
  settings->readSettings(abtListToday, NULL);
}

void AccountListCommiter::writeAbtListToday() {
   disconnect(settings,&SCTimeXMLSettings::settingsRead, this, &AccountListCommiter::writeAbtListToday);
   writer->deleteLater();
   writer=new XMLWriter(settings,abtListToday,pcl);
   connect(writer,&XMLWriter::settingsWritten, this, &AccountListCommiter::reloadAbtListToday);
   writer->writeAllSettings(); // settings wont survive the reload
}

void AccountListCommiter::finish() {
  kontoTree->load(abtList);
  abtList->setZeitDifferenz(diff);
  kontoTree->closeFlaggedPersoenlicheItems();
  emit finished();
}

void AccountListCommiter::cleanupOnErr() {
  writer->deleteLater();
}