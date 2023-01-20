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

#include "punchclockchecker_test.h"
#include "punchclock.h"
#include "punchclockchecker.h"
#include <QTime>

int toSecs(const QString& s) {
   return QTime::fromString(s, "H:m").msecsSinceStartOfDay()/1000;
}

PunchClockEntry entry(const QString& begin, const QString& end) {
   return PunchClockEntry(toSecs(begin), toSecs(end));
}

void TestPunchClockChecker::testNormalDay() {
   PunchClockList pcl;
   QCOMPARE(checkCurrentState(&pcl, toSecs("8:00")), QString(""));
   pcl.push_back(entry("8:43","10:07"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("8:50")), QString(""));
   pcl.push_back(entry("10:09","11:58"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("11:58")), QString(""));
   pcl.push_back(entry("12:32","14:01"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("14:05")), QString(""));
   pcl.push_back(entry("14:20","18:15"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("18:15")), QString(""));
   QCOMPARE(checkCurrentState(&pcl, toSecs("23:59")), QString(""));
}

void TestPunchClockChecker::testLongDayWithVeryShortBreak() {
   PunchClockList pcl;
   QCOMPARE(checkCurrentState(&pcl, toSecs("8:00")), QString(""));
   pcl.push_back(entry("8:43","10:07"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("8:50")), QString(""));
   pcl.push_back(entry("10:09","14:01"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("14:05")), QString(""));
   pcl.push_back(entry("14:20","18:15"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("18:15")), "You are working for 9 hours without many breaks. You should take a break of at least 26 minutes now.");
}

void TestPunchClockChecker::testLongDayWithMediumBreak() {
   PunchClockList pcl;
   QCOMPARE(checkCurrentState(&pcl, toSecs("8:00")), QString(""));
   pcl.push_back(entry("8:43","10:07"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("8:50")), QString(""));
   pcl.push_back(entry("10:09","14:01"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("14:05")), QString(""));
   pcl.push_back(entry("14:35","18:30"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("18:30")), "You are working for 9 hours without many breaks. You should take a break of at least 15 minutes now.");
}

void TestPunchClockChecker::testMissingLunchBreak() {
   PunchClockList pcl;
   QCOMPARE(checkCurrentState(&pcl, toSecs("8:00")), QString(""));
   pcl.push_back(entry("8:43","10:07"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("8:50")), QString(""));
   pcl.push_back(entry("10:09","14:01"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("14:05")), QString(""));
   pcl.push_back(entry("14:10","16:15"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("16:15")), "You are working for 6 hours without a longer break. You should take a break of at least 15 minutes now.");
}

void TestPunchClockChecker::testShortLunchBreak() {
   PunchClockList pcl;
   QCOMPARE(checkCurrentState(&pcl, toSecs("8:00")), QString(""));
   pcl.push_back(entry("8:00","12:00"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("8:50")), QString(""));
   pcl.push_back(entry("12:15","14:30"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("14:30")), "You are working for 6 hours without many breaks. You should take a break of at least 15 minutes in the next three hours.");
}

void TestPunchClockChecker::testLongDayWithBreaks() {
   PunchClockList pcl;
   QCOMPARE(checkCurrentState(&pcl, toSecs("8:00")), QString(""));
   pcl.push_back(entry("8:43","10:07"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("8:50")), QString(""));
   pcl.push_back(entry("10:09","11:58"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("11:58")), QString(""));
   pcl.push_back(entry("12:32","14:01"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("14:05")), QString(""));
   pcl.push_back(entry("14:20","18:15"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("18:15")), QString(""));
   pcl.push_back(entry("18:17","20:00"));
   QCOMPARE(checkCurrentState(&pcl, toSecs("23:59")), QString(""));
}


QTEST_MAIN(TestPunchClockChecker)
#include "moc_punchclockchecker_test.cpp"