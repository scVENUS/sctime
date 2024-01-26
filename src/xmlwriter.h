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
       XMLWriter(SCTimeXMLSettings* settings, AbteilungsListe* abtList, PunchClockList* pcl, int conflicttimeout=150): settings(settings), abtList(abtList), pcl(pcl), conflicttimeout(conflicttimeout) {
               connect(this, &XMLWriter::settingsPartWritten, this, &XMLWriter::continueAfterWriting);
               writeAll=false;
       };
    public:
      virtual void writeAllSettings();
      virtual void writeSettings(bool global);
      virtual void onErrCompat(QNetworkReply::NetworkError code);
      virtual void writeBytes(QUrl url, QByteArray bytes);

  public slots:
      virtual void checkReply(QNetworkReply* input);
      virtual void gotReply();
    /*private signals:
      void deviceOpenedForReading(QIODevice*);*/
    private slots:
      void continueAfterWriting(bool global, AbteilungsListe* abtList, PunchClockList* pcl);
    private:
      SCTimeXMLSettings* settings;
      bool writeAll;
      bool global;
      int conflicttimeout;
      AbteilungsListe* abtList;
      PunchClockList* pcl;
      QNetworkAccessManager networkAccessManager;
    signals:
        void settingsWritten();
        void unauthorized();
        void conflicted(QDate targetdate, bool global, QByteArray othersettings);
        void settingsPartWritten(bool global, AbteilungsListe* abtList, PunchClockList* pcl);
        void settingsWriteFailed(QString reason);
        void offlineSwitched(bool offline);
};

#endif