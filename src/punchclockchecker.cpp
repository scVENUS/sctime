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
      wel->push_back(WorkEvent(pcentry.first, true));
      wel->push_back(WorkEvent(pcentry.second,false));
    }
}

bool compare_interval (const WorkEvent& first, const WorkEvent& second) {
    return ((first.time<second.time)||((first.time==second.time)&&first.isBegin&&!second.isBegin));
}

PunchClockState checkCurrentState(PunchClockList * pcl, int currentTime, const PunchClockState& yesterdayState) {
    PunchClockState currentState;
    currentState.currentWarning="";
    currentState.warnId=PW_NONE;
    WorkEventList wel;
    AddPclToWorkEventList(&wel, pcl);
    if (wel.size()==0) {
      return currentState;
    }
    wel.sort(compare_interval);
    int laststart=yesterdayState.lastLegalBreakEnd-24*HOUR;
    int laststartlegal=laststart;
    int lastend=yesterdayState.workEnd-24*HOUR;
    // we might have overlapping intervals
    int workingIntervalLevel=0;
    // TODO: use offset of last day, if applicable
    // please note that there may be a difference between the calendaric day and the work day.
    // these variables should contain the values for the current workday.
    int worktimeworkday=yesterdayState.workTimeThisWorkday;
    int breaktimeworkday=yesterdayState.breakTimeThisWorkday;
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
      currentState.currentWarning=QObject::tr("You are working for 6 hours without a longer break. You should take a break of at least 15 minutes now.");
      currentState.warnId=PW_NO_BREAK_6H;
    } else
    if (worktimeworkday>6*HOUR-MINUTE && breaktimeworkday<30*MINUTE) {
      currentState.currentWarning=QObject::tr("You are working for 6 hours without many breaks. You should take an additional break of at least 15 minutes in the next three hours.");
      currentState.warnId=PW_TOO_SHORT_BREAK_6H;
    }
    if (worktimeworkday>9*HOUR-MINUTE && breaktimeworkday<45*MINUTE) {
      currentState.currentWarning=QString(QObject::tr("You are working for 9 hours without enough breaks. You should take a break of at least %1 minutes now.")).arg(std::max(15,(45-breaktimeworkday/MINUTE)));
      currentState.warnId=PW_TOO_SHORT_BREAK_9H;
    }
    if (worktimeworkday>10*HOUR) {
      currentState.currentWarning=QObject::tr("You are working for more than 10 hours on this workday. You should take a break of at least 11 hours now.");
      currentState.warnId=PW_OVER_10H;
    }
    
    currentState.breakTimeThisWorkday=breaktimeworkday;
    currentState.workEnd=lastend;
    currentState.lastLegalBreakEnd=laststartlegal;
    currentState.workTimeThisWorkday=worktimeworkday;
    return currentState;
}

