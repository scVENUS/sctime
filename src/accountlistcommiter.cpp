#include "accountlistcommiter.h"
#include "abteilungsliste.h"
#include "kontotreeview.h"
#include "sctimexmlsettings.h"
#include "punchclock.h"

void AccountListCommiter::start() {
  abtList->kontoDatenInfoSuccess = true;
  kontoTree->flagClosedPersoenlicheItems();
  std::vector<int> columnwidthlist;
  kontoTree->getColumnWidthList(columnwidthlist);
  settings->setColumnWidthList(columnwidthlist);
  connect(settings,&SCTimeXMLSettings::settingsWritten, this, &AccountListCommiter::reloadAbtList);
  settings->writeSettings(abtList, pcl); // settings wont survive the reload
}

void AccountListCommiter::reloadAbtList() {
  disconnect(settings,&SCTimeXMLSettings::settingsWritten, this, &AccountListCommiter::reloadAbtList);
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
  abtListToday->reload(data);
  settings->readSettings(abtListToday, NULL);
}

void AccountListCommiter::writeAbtListToday() {
   disconnect(settings,&SCTimeXMLSettings::settingsRead, this, &AccountListCommiter::writeAbtListToday);
   connect(settings,&SCTimeXMLSettings::settingsWritten, this, &AccountListCommiter::reloadAbtListToday);
   settings->writeSettings(abtListToday, pcl); // settings wont survive the reload
}

void AccountListCommiter::finish() {
  kontoTree->load(abtList);
  abtList->setZeitDifferenz(diff);
  kontoTree->closeFlaggedPersoenlicheItems();
  emit finished();
}
