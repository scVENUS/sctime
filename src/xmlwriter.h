#ifndef xmlwriter_h
#define xmlwriter_h

#include <QObject>
#include <QUrl>
#include <QDomDocument>
#include <QByteArray>

#include "sctimexmlsettings.h"

class AbteilungsListe;
class PunchClockList;
class QNetworkReply;

class XMLWriter: public QObject
{
     Q_OBJECT;
     public:
       XMLWriter(SCTimeXMLSettings* settings, QNetworkAccessManager *networkAccessManager, AbteilungsListe* abtList, PunchClockList* pcl, int conflicttimeout=150);
    public:
      virtual void writeAllSettings();
      virtual void writeSettings(bool global);
      virtual QDomDocument settings2Doc(bool global);
      virtual void onErr(QNetworkReply* input);
      virtual void writeBytes(QUrl url, QByteArray bytes);
      virtual void setIdentifier(QString identifier) { this->identifier = identifier; };
      virtual void setSavetime(QDateTime savetime) { this->savetime = savetime; };

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
      QNetworkAccessManager *networkAccessManager;
      QString identifier;
      QDateTime savetime;
    signals:
        void settingsWritten();
        void unauthorized();
        void conflicted(QDate targetdate, bool global, QByteArray othersettings);
        void settingsPartWritten(bool global, AbteilungsListe* abtList, PunchClockList* pcl);
        void settingsWriteFailed(QString reason);
        void offlineSwitched(bool offline);
};

#endif