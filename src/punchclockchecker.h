/*

    Copyright (C) 2023 science+computing ag
       Authors: Florian Schmitt

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef PUNCHCLOCKCHECKER_H
#define PUNCHCLOCKCHECKER_H

#include "punchclock.h"
#include <QString>
#include <QDate>

enum PUNCHWARN {PW_NONE, PW_NO_BREAK_6H, PW_TOO_SHORT_BREAK_6H, PW_TOO_SHORT_BREAK_9H, PW_OVER_10H};

class PunchClockStateBase {
public:
    PunchClockStateBase();
    virtual ~PunchClockStateBase() {};
    virtual QString serialize()=0;
    virtual void deserialize(const QString& s)=0;
    virtual void check(PunchClockList * pcl, int currentTime, const PunchClockStateBase* yesterdayState)=0;
    virtual QString getConsolidatedIntervalString(PunchClockList * pcl)=0;
    virtual void copyFrom(const PunchClockStateBase* source) { warnId=source->warnId; currentWarning=source->currentWarning; date=source->date;};
    virtual int getBreaktimeThisWorkday(){return -1;};
    virtual const QDate& getDate() const { return date; };
    virtual void setDate(const QDate& d) { date = d; };
    PUNCHWARN warnId;
    QString currentWarning;
protected:
    QDate date;
};

class PunchClockStateNoop: public PunchClockStateBase {
public:
    PunchClockStateNoop() {};
    virtual ~PunchClockStateNoop() {};
    virtual QString serialize() { return "NOOP"; };
    virtual void deserialize(const QString& s) {};
    virtual QString getConsolidatedIntervalString(PunchClockList * pcl) { return "";};
    virtual void check(PunchClockList * pcl, int currentTime, const PunchClockStateBase* yesterdayState) {};
};

#ifndef DISABLE_PUNCHCLOCK
class PunchClockStateDE23: public PunchClockStateBase {
public:
    PunchClockStateDE23();
    virtual ~PunchClockStateDE23() {};
    virtual QString serialize();
    virtual void deserialize(const QString& s);
    virtual QString getConsolidatedIntervalString(PunchClockList * pcl);
    virtual void check(PunchClockList * pcl, int currentTime, const PunchClockStateBase* yesterdayState);
    virtual void copyFrom(const PunchClockStateBase* source);
    virtual int getBreaktimeThisWorkday(){return breakTimeThisWorkday;};
private:
    int workEnd;
    int breakTimeThisWorkday;
    int lastLegalBreakEnd;
    int workTimeThisWorkday;
    friend class TestPunchClockChecker;
};
#endif

PunchClockStateBase* newPunchClockState();

#endif
