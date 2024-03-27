#ifndef xmlreader_h
#define xmlreader_h

#include <QObject>
#include <QString>
#include <QDomDocument>
#include "sctimexmlsettings.h"

class AbteilungsListe;
class PunchClockList;
class QIODevice;
class QFile;

class XMLReader: public QObject
{
     Q_OBJECT;
     public:
       XMLReader(SCTimeXMLSettings* parent, QNetworkAccessManager *networkAccessManager, bool global, bool forceLocalRead, bool autoContinueOnConflict, AbteilungsListe* abtList, PunchClockList* pcl): QObject(parent), global(global), forceLocalRead(forceLocalRead), autoContinueOnConflict(autoContinueOnConflict), abtList(abtList), pcl(pcl), networkAccessManager(networkAccessManager) {
          connect(this, &XMLReader::settingsPartRead, this, &XMLReader::continueAfterReading);
          continueThisConflict=false;
       };
    public: 
      virtual void open();      
      virtual QFile* openFile(bool handleerr);
      virtual void openREST();
      virtual void ignoreConflict() { continueThisConflict=true; };
    
    signals:
      void conflicted(QDate targetdate, bool global, QDomDocument remotesettings);
      void conflictedWithLocal(QDate targetdate, bool global, QDomDocument localsettings, QDomDocument remotesettings);

    public slots:
      virtual void parse(QIODevice* input);
      virtual void fillSettingsFromDocument(QDomDocument& doc, SCTimeXMLSettings* settings);
      virtual void gotReply();
      virtual void continueAfterReading(bool global, AbteilungsListe *abtList, PunchClockList *pcl);
      /*private signals:
        void deviceOpenedForReading(QIODevice*);*/
  private:
      bool global;
      bool forceLocalRead;
      bool autoContinueOnConflict;
      bool continueThisConflict;
      AbteilungsListe* abtList;
      PunchClockList* pcl;
      QNetworkAccessManager *networkAccessManager;
    signals:
      void settingsPartRead(bool global, AbteilungsListe* abtList, PunchClockList* pcl, bool success, QString message);
      void settingsRead();
      void unauthorized();
      void offlineSwitched(bool offline);
};

#endif