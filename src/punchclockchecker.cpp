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


#include "punchclock.h"
#include "punchclockchecker.h"

#include <QObject>

const int MINUTE=60;
const int HOUR=60*MINUTE;

class WorkEvent;
class WorkEvent {
public:
    WorkEvent(): time(0),isBegin(false) {};
    WorkEvent(int t, bool b): time(t),isBegin(b) {};
    WorkEvent(const WorkEvent& token): WorkEvent(token.time, token.isBegin) {};
    int time;
    bool isBegin;
};

class WorkEventList: public std::list<WorkEvent> {
};

void AddPclToWorkEventList(WorkEventList *wel, PunchClockList * pcl) {
    for (auto pcentry: *pcl) {
      // do not wrap around midnight, but cut any entry that seems to go past.
      // please note: this also cuts entries if begin and end is ordered wrong
      if (pcentry.second<pcentry.first) {
        pcentry.second=23*HOUR+59*MINUTE;
      }
      wel->push_back(WorkEvent(pcentry.first, true));
      wel->push_back(WorkEvent(pcentry.second,false));
    }
}

bool compare_interval (const WorkEvent& first, const WorkEvent& second) {
    return ((first.time<second.time)||((first.time==second.time)&&first.isBegin&&!second.isBegin));
}

void PunchClockStateDE23::check(PunchClockList * pcl, int currentTime, const PunchClockStateBase* yesterdayStateBase) {
    auto yesterdayState=dynamic_cast<const PunchClockStateDE23*>(yesterdayStateBase);
    currentWarning="";
    warnId=PW_NONE;
    WorkEventList wel;
    AddPclToWorkEventList(&wel, pcl);
    if (wel.size()==0) {
      workEnd=0;
      breakTimeThisWorkday=0;
      lastLegalBreakEnd=0;
      workTimeThisWorkday=0;
    }
    wel.sort(compare_interval);
    int laststart=-24*HOUR;
    int lastend=-24*HOUR;
    // we might have overlapping intervals
    int workingIntervalLevel=0;
    // please note that there may be a difference between the calendaric day and the work day.
    // these variables should contain the values for the current workday.
    int worktimeworkday=0;
    int breaktimeworkday=0;
    if (yesterdayState!=NULL) {
      laststart+=yesterdayState->lastLegalBreakEnd;
      lastend+=yesterdayState->workEnd;
      worktimeworkday+=yesterdayState->workTimeThisWorkday;
      breaktimeworkday+=yesterdayState->breakTimeThisWorkday;
    }
    int laststartlegal=laststart;
    int breaktimetodayafter15h=0;
    int breaktimetodayafter18h=0;
    for (auto itoken: wel) {
        if (itoken.isBegin) {
            workingIntervalLevel++;
            if (workingIntervalLevel==1) {
                if (itoken.time-lastend>=11*HOUR) {
                  worktimeworkday=0;
                  breaktimeworkday=0;
                  laststartlegal=itoken.time;
                }
                else if (itoken.time-lastend>=15*MINUTE) {
                  breaktimeworkday+=itoken.time-lastend;
                  if (itoken.time>15*HOUR) {
                     breaktimetodayafter15h+=itoken.time-(std::max(lastend, 15*HOUR));
                  }
                  if (itoken.time>18*HOUR) {
                     breaktimetodayafter18h+=itoken.time-(std::max(lastend, 18*HOUR));
                  }
                  laststartlegal=itoken.time;
                }
                laststart=itoken.time;
            }
        } else {
            workingIntervalLevel--;
            if (workingIntervalLevel==0) {
                int lastendold=lastend;
                lastend=itoken.time;
                // we only count pause intervals in the legal sense here. if the pause does not count, use the previous end instead
                if (laststart==laststartlegal) {
                  worktimeworkday+=lastend-laststart;
                } else {
                  worktimeworkday+=lastend-lastendold;
                }
            }
        }
    }
    if ((currentTime-laststartlegal>=6*HOUR-MINUTE)&&(currentTime-lastend<15*MINUTE)) {
      currentWarning=QObject::tr("You are working for 6 hours without a longer break. You should take a break of at least 15 minutes now.");
      warnId=PW_NO_BREAK_6H;
    } else
    if (worktimeworkday>6*HOUR-MINUTE && breaktimeworkday<30*MINUTE) {
      currentWarning=QObject::tr("You are working for 6 hours without many breaks. You should take an additional break of at least 15 minutes in the next three hours.");
      warnId=PW_TOO_SHORT_BREAK_6H;
    }
    if (worktimeworkday>9*HOUR-MINUTE && breaktimeworkday<45*MINUTE) {
      currentWarning=QString(QObject::tr("You are working for 9 hours without enough breaks. You should take a break of at least %1 minutes now.")).arg(std::max(15,(45-breaktimeworkday/MINUTE)));
      warnId=PW_TOO_SHORT_BREAK_9H;
    }
    if (worktimeworkday>10*HOUR) {
      currentWarning=QObject::tr("You are working for more than 10 hours on this workday. You should take a break of at least 11 hours now.");
      warnId=PW_OVER_10H;
    }
    
    breakTimeThisWorkday=breaktimeworkday;
    workEnd=lastend;
    lastLegalBreakEnd=laststartlegal;
    workTimeThisWorkday=worktimeworkday;
}

PunchClockStateBase::PunchClockStateBase() {
    currentWarning="";
    warnId=PUNCHWARN::PW_NONE;
}

PunchClockStateDE23::PunchClockStateDE23() {
    workEnd=0;
    breakTimeThisWorkday=0;
    lastLegalBreakEnd=0;
    workTimeThisWorkday=0;
}

QString PunchClockStateDE23::serialize() {
    return QString("%1;%2;%3;%4;%5;%6").arg("DE23").arg(date.toString("yyyyMMdd")).arg(workEnd).arg(breakTimeThisWorkday).arg(lastLegalBreakEnd).arg(workTimeThisWorkday);
}

void PunchClockStateDE23::deserialize(const QString& s) {
    auto list=s.split(";");
    if (list.length()==0) {
      *this=PunchClockStateDE23();
      return;
    }
    QString type=list[0];
    if ((type!="DE23")||list.length()!=6) {
      *this=PunchClockStateDE23();
      return;
    }
    date=QDate::fromString(list[1],"yyyyMMdd");
    workEnd=list[2].toInt();
    breakTimeThisWorkday=list[3].toInt();
    lastLegalBreakEnd=list[4].toInt();
    workTimeThisWorkday=list[5].toInt();
}