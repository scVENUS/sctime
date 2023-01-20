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

QString checkCurrentState(PunchClockList * pcl, int currentTime) {
    QString warning;
    WorkEventList wel;
    AddPclToWorkEventList(&wel, pcl);
    if (wel.size()==0) {
      return "";
    }
    wel.sort(compare_interval);
    int workdaystart=-12*60*60;
    int laststart=workdaystart;
    int laststartlegal=workdaystart;
    int lastend=workdaystart;
    // we might have overlapping intervals
    int workingIntervalLevel=0;
    // TODO: use offset of last day, if applicable
    // please note that there may be a difference between the calendaric day and the work day.
    // these variables should contain the values for the current workday.
    int worktimeworkday=0;
    int breaktimeworkday=0;
    int breaktimetodayafter15h=0;
    int breaktimetodayafter18h=0;
    for (auto itoken: wel) {
        if (itoken.isBegin) {
            workingIntervalLevel++;
            if (workingIntervalLevel==1) {
                if (itoken.time-laststart>=11*60*60) {
                  workdaystart=itoken.time;
                  worktimeworkday=0;
                  breaktimeworkday=0;
                  laststartlegal=itoken.time;
                }
                else if (itoken.time-lastend>=15*60) {
                  breaktimeworkday+=itoken.time-laststart;
                  if (itoken.time>15*60*60) {
                     breaktimetodayafter15h+=itoken.time-(std::max(laststart, 15*60*60));
                  }
                  if (itoken.time>18*60*60) {
                     breaktimetodayafter18h+=itoken.time-(std::max(laststart, 18*60*60));
                  }
                  laststartlegal=itoken.time;
                }
                laststart=itoken.time;
            }
        } else {
            workingIntervalLevel--;
            if (workingIntervalLevel==0) {
                lastend=itoken.time;
                worktimeworkday+=lastend-laststart;
            }
        }
    }
    if ((currentTime-laststartlegal>=6*60*60-60)&&(currentTime-lastend<15*60)) {
      warning="You are working for 6 hours without a longer break. You should take a break of at least 15 minutes now.";
    }
    if (worktimeworkday>6*60*60-60 && breaktimeworkday<30*60) {
      warning=QString().sprintf("You are working for 6 hours without many breaks. You should take a break of at least 15 minutes in the next three hours.");
    }
    if (worktimeworkday>9*60*60-60 && breaktimeworkday<30*60) {
      warning=QString().sprintf("You are working for 9 hours and did not take enough breaks. You should take a break of at least %d minutes now.", std::max(30-breaktimeworkday/60, 15));
    }
    if (worktimeworkday>9*60*60-60 && breaktimeworkday<45*60) {
      warning=QString().sprintf("You are working for 9 hours without many breaks. You should take a break of at least %d minutes now.", std::max(15,(45-breaktimeworkday/60)));
    }
    
    return warning;
}

