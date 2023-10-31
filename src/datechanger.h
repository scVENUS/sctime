#ifndef DATECHANGER_H
#define DATECHANGER_H

#include <QObject>
#include "timemainwindow.h"

class DateChanger: public QObject {
        Q_OBJECT;
public:
       DateChanger(TimeMainWindow* parent, const QDate &datum, bool changeVisible, bool changeToday):
          QObject(parent), m_timeMainWindow(parent), targetDate(datum), changeVisible(changeVisible), changeToday(changeToday)  {

       };
public slots:
       void start();
private slots:
       void resetLists();
       void updatePunchClock();
signals:
       void finished(const QDate &date, bool changeVisible, bool changeToday, bool currentDateSel);
private:
       TimeMainWindow *m_timeMainWindow;
       QMetaObject::Connection lastconn;
       int expectedActions;
       QString abt, ko, uko;
       int idx;
       const QDate &targetDate;
       QDate currentDate;
       bool changeVisible;
       bool changeToday;
       bool currentDateSel;
};

#endif