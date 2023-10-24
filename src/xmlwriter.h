#ifndef xmlwriter_h
#define xmlwriter_h

#include <QObject>
#include <QUrl>
#include <QByteArray>

#include "sctimexmlsettings.h"

class AbteilungsListe;
class PunchClockList;
class QNetworkReply;

class XMLWriter: public QObject
{
     Q_OBJECT;
     public:
       XMLWriter(SCTimeXMLSettings* parent, bool global, AbteilungsListe* abtList, PunchClockList* pcl): QObject(parent), global(global), abtList(abtList), pcl(pcl) {
               #ifdef RESTONLY
               connect(&networkAccessManager, &QNetworkAccessManager::finished, this, &XMLWriter::checkReply);
               #endif 
       };
    public: 
      virtual void writeBytes(QUrl url, QByteArray bytes);

    public slots:
      virtual void checkReply(QNetworkReply* input);
    /*private signals:
      void deviceOpenedForReading(QIODevice*);*/
    private:
      bool global;
      AbteilungsListe* abtList;
      PunchClockList* pcl;
      #ifdef RESTONLY
        QNetworkAccessManager networkAccessManager;
      #endif
};

#endif