#ifndef DATECHANGER_H
#define DATECHANGER_H

#include <QObject>
#include "timemainwindow.h"

class XMLWriter;

class DateChanger: public QObject {
        Q_OBJECT;
public:
       DateChanger(TimeMainWindow* parent, QNetworkAccessManager *networkAccessManager, const QDate &datum, bool changeVisible, bool changeToday):
          QObject(parent), m_timeMainWindow(parent), targetDate(datum), changeVisible(changeVisible), changeToday(changeToday), networkAccessManager(networkAccessManager)  {

       };
public slots:
       void start();
private slots:
       void resetLists();
       void updatePunchClock();
signals:
       void finished(const QDate &date, bool changeVisible, bool changeToday, bool currentDateSel);
       void offlineSwitched(bool offline);
private:
       void write(AbteilungsListe* abtlist, PunchClockList* pcl);
       TimeMainWindow *m_timeMainWindow;
       QMetaObject::Connection lastconn;
       int expectedActions;
       QString abt, ko, uko;
       int idx;
       QDate targetDate;
       QDate currentDate;
       bool changeVisible;
       bool changeToday;
       bool currentDateSel;
       QNetworkAccessManager *networkAccessManager;
};

#endif