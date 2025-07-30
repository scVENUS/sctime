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

#ifndef PUNCHCLOCK_H
#define PUNCHCLOCK_H

#include <utility>
#include <list>


class PunchClockEntry: public std::pair<int, int>{
  public:
     PunchClockEntry(int begin, int end): std::pair<int, int>(begin, end) {
     }
};

class PunchClockList: public std::list<PunchClockEntry> {
   public:
     PunchClockList(): std::list<PunchClockEntry>() {
      m_currentEntry=this->end();
     }

     PunchClockList(const PunchClockList& src): std::list<PunchClockEntry>(src) {
      m_currentEntry=src.currentEntry();
     }

     void setCurrentEntry(PunchClockList::iterator entry) {
      m_currentEntry=entry;
     }

     PunchClockList::iterator currentEntry() const {
      return m_currentEntry;
     }

     void clear() {
       std::list<PunchClockEntry>::clear();
       m_currentEntry=this->end();
     }
   private:
     PunchClockList::iterator m_currentEntry;

};

#endif
