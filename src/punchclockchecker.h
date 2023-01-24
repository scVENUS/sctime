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

enum PUNCHWARN {PW_NONE, PW_NO_BREAK_6H, PW_TOO_SHORT_BREAK_6H, PW_TOO_SHORT_BREAK_9H, PW_OVER_10H};

class PunchClockState {
public:
    PunchClockState() {
        workEnd=0;
        breakTimeThisWorkday=0;
        lastLegalBreakEnd=0;
        workTimeThisWorkday=0;
        currentWarning="";
        warnId=PUNCHWARN::PW_NONE;
    }
    int workEnd;
    int breakTimeThisWorkday;
    int lastLegalBreakEnd;
    int workTimeThisWorkday;
    PUNCHWARN warnId;
    QString currentWarning;
};

PunchClockState checkCurrentState(PunchClockList * pcl, int currentTime, const PunchClockState& yesterdayState);


#endif
