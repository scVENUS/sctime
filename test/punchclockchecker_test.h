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

#ifndef PUNCHCLOCKCHECKER_TEST_H
#define PUNCHCLOCKCHECKER_TEST_H

#include <QtTest/QtTest>

class TestPunchClockChecker: public QObject
{
    Q_OBJECT
private slots:
    void testNormalDay();
    void testMissingLunchBreak();
    void testLongDayWithVeryShortBreak();
    void testLongDayWithMediumBreak();
    void testLongDayWithBreaks();
    void testShortLunchBreak();
    void testOverNightWorkWithoutbreak();
    void testOverNightWorkShortBreakBeforeMidnight();
    void testOverNightWorkMediumBreakOverMidnight();
    void testLongOverNightWorkMediumBreakOverMidnight();
    void testLongOverNightShortBreakBeforeMidnight();
    void testLongOverNightEnoughBreaks();
    void testComplexWorkdaysOK();
    void testComplexWorkdaysNotOK();
    void testComplexWorkdaysNotOK2();
    void testEarlyMorningLongSegmentUnordered();
    void testLongSegmentOverMidnight();
};

#endif