#ifndef ACCOUNTLISTCOMMITER_H

#include <QObject>
#include "datasource.h"

class AbteilungsListe;
class KontoTreeView;
class SCTimeXMLSettings;
class PunchClockList;
class XMLWriter;

// commits a new account list. create a new instance of this object everytime you need to do this - states are not cleaned up.
class AccountListCommiter: public QObject {
        Q_OBJECT;
public:
       AccountListCommiter(QObject* parent, DSResult data, SCTimeXMLSettings* settings, KontoTreeView *kontotree, AbteilungsListe* abtList, AbteilungsListe* abtListToday, PunchClockList *pcl):
          QObject(parent), data(data), settings(settings), kontoTree(kontotree), abtList(abtList), abtListToday(abtListToday), pcl(pcl), diff(0) {

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

       int diff;        
};
#endif