#ifndef xmlreader_h
#define xmlreader_h

#include <QObject>
#include <QString>
#include "sctimexmlsettings.h"

class AbteilungsListe;
class PunchClockList;
class QIODevice;

class XMLReader: public QObject
{
     Q_OBJECT;
     public:
       XMLReader(SCTimeXMLSettings* parent, bool global, AbteilungsListe* abtList, PunchClockList* pcl): QObject(parent), global(global), abtList(abtList), pcl(pcl) {
               
       };
    public: 
      virtual void open();      

    public slots:
      virtual void parse(QIODevice* input);
      virtual void gotReply();
    /*private signals:
      void deviceOpenedForReading(QIODevice*);*/
    private:
      bool global;
      AbteilungsListe* abtList;
      PunchClockList* pcl;
      QNetworkAccessManager networkAccessManager;
    signals:
      void settingsPartRead(bool global, AbteilungsListe* abtList, PunchClockList* pcl, bool success, QString message);
};

#endif