#ifndef ACCOUNTLISTCOMMITER_H

#include <QObject>
#include "datasource.h"

class AbteilungsListe;
class KontoTreeView;
class SCTimeXMLSettings;
class PunchClockList;
class XMLWriter;
class XMLReader;
class QNetworkAccessManager;

// commits a new account list. create a new instance of this object everytime you need to do this - states are not cleaned up.
class AccountListCommiter: public QObject {
        Q_OBJECT;
public:
       AccountListCommiter(QObject* parent, QNetworkAccessManager* networkAccessManager, DSResult data, SCTimeXMLSettings* settings, KontoTreeView *kontotree, AbteilungsListe* abtList, AbteilungsListe* abtListToday, PunchClockList *pcl, bool withoutSave):
          QObject(parent), data(data), settings(settings), kontoTree(kontotree), abtList(abtList), abtListToday(abtListToday), pcl(pcl), withoutSave(withoutSave),diff(0), networkAccessManager(networkAccessManager) {

       };
public slots:
       void start();
private slots:
       void reloadAbtList();
       void reloadAbtListToday();
       void writeAbtListToday();
       void finish();
       void cleanupOnErr();
signals:
       void finished();
private:
       DSResult data;
       SCTimeXMLSettings* settings;
       KontoTreeView *kontoTree; 
       AbteilungsListe* abtList;
       AbteilungsListe* abtListToday;
       PunchClockList *pcl;
       XMLWriter* writer;
       XMLReader* reader;
       // normally you should save before commiting. If you have already done that you can skip this step.
       bool withoutSave;

       int diff;
       QNetworkAccessManager* networkAccessManager;
};
#endif