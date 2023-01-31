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
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("8:00"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("8:43","10:07"));
   state.check(&pcl, toSecs("8:50"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("10:09","11:58"));
   state.check(&pcl, toSecs("11:58"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("12:32","14:01"));
   state.check(&pcl, toSecs("14:05"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("14:20","18:15"));
   state.check(&pcl, toSecs("18:15"), &yesterday);
   QCOMPARE(state.workTimeThisWorkday, toSecs("8:39"));
   QCOMPARE(state.currentWarning, QString(""));
   state.check(&pcl, toSecs("23:59"), &yesterday);
   QCOMPARE(state.workTimeThisWorkday, toSecs("8:39"));
   QCOMPARE(state.warnId, PW_NONE);
}

void TestPunchClockChecker::testLongDayWithVeryShortBreak() {
   PunchClockList pcl;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("8:00"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("8:43","10:07"));
   state.check(&pcl, toSecs("8:50"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("10:09","14:01"));
   state.check(&pcl, toSecs("14:05"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("14:20","18:15"));
   state.check(&pcl, toSecs("18:15"), &yesterday);
   QCOMPARE(state.currentWarning, QString("You are working for 9 hours without enough breaks. You should take a break of at least 26 minutes now."));
   QCOMPARE(state.warnId, PW_TOO_SHORT_BREAK_9H);
}

void TestPunchClockChecker::testLongDayWithMediumBreak() {
   PunchClockList pcl;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("8:00"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("8:43","10:07"));
   state.check(&pcl, toSecs("8:50"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("10:09","14:01"));
   state.check(&pcl, toSecs("14:05"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("14:35","18:30"));
   state.check(&pcl, toSecs("18:30"), &yesterday);
   QCOMPARE(state.currentWarning, QString("You are working for 9 hours without enough breaks. You should take a break of at least 15 minutes now."));
   QCOMPARE(state.warnId, PW_TOO_SHORT_BREAK_9H);
}

void TestPunchClockChecker::testMissingLunchBreak() {
   PunchClockList pcl;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("8:00"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("8:43","10:07"));
   state.check(&pcl, toSecs("8:50"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("10:09","14:01"));
   state.check(&pcl, toSecs("14:05"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("14:10","16:15"));
   state.check(&pcl, toSecs("16:15"), &yesterday);
   QCOMPARE(state.currentWarning, "You are working for 6 hours without a longer break. You should take a break of at least 15 minutes now.");
   QCOMPARE(state.warnId, PW_NO_BREAK_6H);
}

void TestPunchClockChecker::testShortLunchBreak() {
   PunchClockList pcl;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("8:00"),&yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("8:00","12:00"));
   state.check(&pcl, toSecs("8:50"),&yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("12:15","14:30"));
   state.check(&pcl, toSecs("14:30"),&yesterday);
   QCOMPARE(state.currentWarning, QString("You are working for 6 hours without many breaks. You should take an additional break of at least 15 minutes in the next three hours."));
   QCOMPARE(state.warnId, PW_TOO_SHORT_BREAK_6H);
}

void TestPunchClockChecker::testLongDayWithBreaks() {
   PunchClockList pcl;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("8:00"),&yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("8:43","10:07"));
   state.check(&pcl, toSecs("8:50"),&yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("10:09","11:58"));
   state.check(&pcl, toSecs("11:58"),&yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("12:32","14:01"));
   state.check(&pcl, toSecs("14:05"),&yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("14:20","18:15"));
   state.check(&pcl, toSecs("18:15"),&yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("18:17","20:00"));
   state.check(&pcl, toSecs("23:59"),&yesterday);
   QCOMPARE(state.currentWarning, QString("You are working for more than 10 hours on this workday. You should take a break of at least 11 hours now."));
   QCOMPARE(state.workTimeThisWorkday, toSecs("10:24"));
   QCOMPARE(state.warnId, PW_OVER_10H);
}

void TestPunchClockChecker::testOverNightWorkWithoutbreak() {
   PunchClockList pcl;
   PunchClockStateDE23 emptyday;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("19:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("19:10","23:59"));
   state.check(&pcl, toSecs("23:59"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   QCOMPARE(state.workTimeThisWorkday, toSecs("4:49"));
   pcl.clear();
   yesterday=state;
   pcl.push_back(entry("0:0","0:40"));
   state.check(&pcl, toSecs("0:40"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("0:50","3:10"));
   state.check(&pcl, toSecs("3:10"), &yesterday);
   QCOMPARE(state.currentWarning, QString("You are working for 6 hours without a longer break. You should take a break of at least 15 minutes now."));
   QCOMPARE(state.warnId, PW_NO_BREAK_6H);
}

void TestPunchClockChecker::testOverNightWorkShortBreakBeforeMidnight() {
   PunchClockList pcl;
   PunchClockStateDE23 emptyday;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("19:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("19:10","23:00"));
   state.check(&pcl, toSecs("23:00"), &emptyday);
   pcl.push_back(entry("23:20","23:59"));
   state.check(&pcl, toSecs("23:59"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   QCOMPARE(state.workTimeThisWorkday, toSecs("4:29"));
   pcl.clear();
   yesterday=state;
   pcl.push_back(entry("0:0","0:40"));
   state.check(&pcl, toSecs("0:40"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("0:50","3:10"));
   state.check(&pcl, toSecs("3:10"), &yesterday);
   QCOMPARE(state.currentWarning, QString("You are working for 6 hours without many breaks. You should take an additional break of at least 15 minutes in the next three hours."));
   QCOMPARE(state.warnId, PW_TOO_SHORT_BREAK_6H);
}

void TestPunchClockChecker::testOverNightWorkMediumBreakOverMidnight() {
   PunchClockList pcl;
   PunchClockStateDE23 emptyday;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("19:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("19:10","23:40"));
   state.check(&pcl, toSecs("23:40"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   QCOMPARE(state.workTimeThisWorkday, toSecs("4:30"));
   pcl.clear();
   yesterday=state;
   pcl.push_back(entry("0:13","0:40"));
   state.check(&pcl, toSecs("0:40"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("0:50","3:10"));
   state.check(&pcl, toSecs("3:10"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   QCOMPARE(state.warnId, PW_NONE);
}

void TestPunchClockChecker::testLongOverNightWorkMediumBreakOverMidnight() {
   PunchClockList pcl;
   PunchClockStateDE23 emptyday;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("19:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("19:10","23:40"));
   state.check(&pcl, toSecs("23:40"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   QCOMPARE(state.workTimeThisWorkday, toSecs("4:30"));
   pcl.clear();
   yesterday=state;
   pcl.push_back(entry("0:13","0:40"));
   state.check(&pcl, toSecs("0:40"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("0:50","5:10"));
   state.check(&pcl, toSecs("5:10"), &yesterday);
   QCOMPARE(state.currentWarning, QString("You are working for 9 hours without enough breaks. You should take a break of at least 15 minutes now."));
   QCOMPARE(state.warnId, PW_TOO_SHORT_BREAK_9H);
}

void TestPunchClockChecker::testLongOverNightShortBreakBeforeMidnight() {
   PunchClockList pcl;
   PunchClockStateDE23 emptyday;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("19:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("19:10","23:40"));
   state.check(&pcl, toSecs("23:40"), &emptyday);
   pcl.push_back(entry("23:56","23:59"));
   state.check(&pcl, toSecs("23:59"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   QCOMPARE(state.workTimeThisWorkday, toSecs("4:33"));
   pcl.clear();
   yesterday=state;
   pcl.push_back(entry("0:13","0:40"));
   state.check(&pcl, toSecs("0:40"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("0:50","5:10"));
   state.check(&pcl, toSecs("5:10"), &yesterday);
   QCOMPARE(state.currentWarning, QString("You are working for 9 hours without enough breaks. You should take a break of at least 29 minutes now."));
   QCOMPARE(state.warnId, PW_TOO_SHORT_BREAK_9H);
}

void TestPunchClockChecker::testLongOverNightEnoughBreaks() {
   PunchClockList pcl;
   PunchClockStateDE23 emptyday;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("19:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("19:10","23:40"));
   state.check(&pcl, toSecs("23:40"), &emptyday);
   pcl.push_back(entry("23:56","23:59"));
   state.check(&pcl, toSecs("23:59"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   QCOMPARE(state.workTimeThisWorkday, toSecs("4:33"));
   pcl.clear();
   yesterday=state;
   pcl.push_back(entry("0:13","0:40"));
   state.check(&pcl, toSecs("0:40"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("1:15","5:40"));
   state.check(&pcl, toSecs("5:40"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   QCOMPARE(state.warnId, PW_NONE);
}

void TestPunchClockChecker::testComplexWorkdaysOK() {
   PunchClockList pcl;
   PunchClockStateDE23 emptyday;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("1:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("1:00","3:00"));
   state.check(&pcl, toSecs("3:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("3:00","5:00"));
   state.check(&pcl, toSecs("5:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   state.check(&pcl, toSecs("19:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("19:10","23:40"));
   state.check(&pcl, toSecs("23:40"), &emptyday);
   pcl.push_back(entry("23:56","23:59"));
   state.check(&pcl, toSecs("23:59"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   QCOMPARE(state.workTimeThisWorkday, toSecs("4:33"));
   pcl.clear();
   yesterday=state;
   pcl.push_back(entry("0:13","0:40"));
   state.check(&pcl, toSecs("0:40"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("1:15","5:40"));
   state.check(&pcl, toSecs("5:40"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   QCOMPARE(state.warnId, PW_NONE);
}

void TestPunchClockChecker::testComplexWorkdaysNotOK() {
   PunchClockList pcl;
   PunchClockStateDE23 emptyday;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("1:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("1:00","3:00"));
   state.check(&pcl, toSecs("3:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("3:00","5:00"));
   state.check(&pcl, toSecs("5:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("10:00","11:00"));
   state.check(&pcl, toSecs("11:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   state.check(&pcl, toSecs("19:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("19:10","23:40"));
   state.check(&pcl, toSecs("23:40"), &emptyday);
   pcl.push_back(entry("23:56","23:59"));
   state.check(&pcl, toSecs("23:59"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   QCOMPARE(state.workTimeThisWorkday, toSecs("9:33"));
   pcl.clear();
   yesterday=state;
   pcl.push_back(entry("0:13","0:40"));
   state.check(&pcl, toSecs("0:40"), &yesterday);
   QCOMPARE(state.workTimeThisWorkday, toSecs("10:14"));
   QCOMPARE(state.currentWarning, QString("You are working for more than 10 hours on this workday. You should take a break of at least 11 hours now."));
   pcl.push_back(entry("1:15","5:40"));
   state.check(&pcl, toSecs("5:40"), &yesterday);
   QCOMPARE(state.currentWarning, QString("You are working for more than 10 hours on this workday. You should take a break of at least 11 hours now."));
   QCOMPARE(state.warnId, PW_OVER_10H);
}

void TestPunchClockChecker::testComplexWorkdaysNotOK2() {
   PunchClockList pcl;
   PunchClockStateDE23 emptyday;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("1:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("1:00","3:00"));
   state.check(&pcl, toSecs("3:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("3:00","5:00"));
   state.check(&pcl, toSecs("5:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("13:00","14:00"));
   state.check(&pcl, toSecs("11:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   state.check(&pcl, toSecs("19:00"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("19:10","23:40"));
   state.check(&pcl, toSecs("23:40"), &emptyday);
   pcl.push_back(entry("23:56","23:59"));
   state.check(&pcl, toSecs("23:59"), &emptyday);
   QCOMPARE(state.currentWarning, QString(""));
   QCOMPARE(state.workTimeThisWorkday, toSecs("9:33"));
   pcl.clear();
   yesterday=state;
   pcl.push_back(entry("0:13","0:40"));
   state.check(&pcl, toSecs("0:40"), &yesterday);
   QCOMPARE(state.currentWarning, QString("You are working for more than 10 hours on this workday. You should take a break of at least 11 hours now."));
   QCOMPARE(state.workTimeThisWorkday, toSecs("10:14"));
   pcl.push_back(entry("1:15","5:40"));
   state.check(&pcl, toSecs("5:40"), &yesterday);
   QCOMPARE(state.currentWarning, QString("You are working for more than 10 hours on this workday. You should take a break of at least 11 hours now."));
   QCOMPARE(state.workTimeThisWorkday, toSecs("14:39"));
   QCOMPARE(state.warnId, PW_OVER_10H);
}

void TestPunchClockChecker::testEarlyMorningLongSegmentUnordered() {
   PunchClockList pcl;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   state.check(&pcl, toSecs("1:00"), &yesterday);
   QCOMPARE(state.currentWarning, QString(""));
   pcl.push_back(entry("7:00","13:59"));
   pcl.push_back(entry("2:00","4:30"));
   pcl.push_back(entry("14:05","14:14"));
   state.check(&pcl, toSecs("14:14"), &yesterday);
   QCOMPARE(state.workTimeThisWorkday, toSecs("9:44"));
   QCOMPARE(state.currentWarning, QString("You are working for 6 hours without a longer break. You should take a break of at least 15 minutes now."));
   QCOMPARE(state.warnId, PW_NO_BREAK_6H);
}

void TestPunchClockChecker::testLongSegmentOverMidnight() {
   PunchClockList pcl;
   PunchClockStateDE23 emptyday;
   PunchClockStateDE23 yesterday;
   PunchClockStateDE23 state;
   pcl.push_back(entry("21:00","23:00"));
   pcl.push_back(entry("23:40","23:59"));
   yesterday.check(&pcl, toSecs("15:00"), &emptyday);
   QCOMPARE(yesterday.workTimeThisWorkday, toSecs("2:19"));
   pcl.clear();
   pcl.push_back(entry("0:0","6:00"));
   state.check(&pcl, toSecs("6:00"), &yesterday);
   QCOMPARE(state.workTimeThisWorkday, toSecs("8:20"));
   QCOMPARE(state.currentWarning, QString("You are working for 6 hours without a longer break. You should take a break of at least 15 minutes now."));
   QCOMPARE(state.warnId, PW_NO_BREAK_6H);
}

void TestPunchClockChecker::testSerialization() {
   PunchClockStateDE23 state1;
   PunchClockStateDE23 state2;

   state1.breakTimeThisWorkday=toSecs("0:40");
   state1.lastLegalBreakEnd=toSecs("13:10");
   state1.workEnd=toSecs("17:10");
   state1.workTimeThisWorkday=toSecs("8:10");
   state1.date=QDate::fromString("20230401","yyyyMMdd");

   QString s=state1.serialize();
   QCOMPARE(s,"DE23;20230401;61800;2400;47400;29400");
   state2.deserialize(s);
   QCOMPARE(state1.breakTimeThisWorkday, state2.breakTimeThisWorkday);
   QCOMPARE(state1.lastLegalBreakEnd, state2.lastLegalBreakEnd);
   QCOMPARE(state1.workTimeThisWorkday, state2.workTimeThisWorkday);
   QCOMPARE(state1.workEnd, state2.workEnd);
   QCOMPARE(state1.date, state2.date);
}

QTEST_MAIN(TestPunchClockChecker)
#include "moc_punchclockchecker_test.cpp"