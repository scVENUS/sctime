#include "datechanger.h"

#include <QDate>
#include "abteilungsliste.h"
#include "sctimexmlsettings.h"
#include "globals.h"

void DateChanger::start()
{
    currentDate = QDate::currentDate();
    currentDateSel = (targetDate == currentDate);

    m_timeMainWindow->abtListToday->getAktiv(abt, ko, uko, idx);

    if (m_timeMainWindow->abtListToday->getDatum().addDays(1) == currentDate)
    {
        m_timeMainWindow->m_PCSToday->date = m_timeMainWindow->abtListToday->getDatum();
        m_timeMainWindow->settings->setPreviousPCCData(m_timeMainWindow->m_PCSToday->serialize());
    }

    if (m_timeMainWindow->abtListToday != m_timeMainWindow->abtList)
    {
        lastconn = connect(m_timeMainWindow->settings, &SCTimeXMLSettings::settingsPartWritten, this, &DateChanger::resetLists);
        expectedActions = 4;
        m_timeMainWindow->settings->writeSettings(m_timeMainWindow->abtListToday, m_timeMainWindow->m_punchClockListToday);
        m_timeMainWindow->settings->writeSettings(m_timeMainWindow->abtList, m_timeMainWindow->m_punchClockList);
        // TODO: shell scripts are not written asynchronously
        m_timeMainWindow->settings->writeShellSkript(m_timeMainWindow->abtListToday, m_timeMainWindow->m_punchClockListToday);
        m_timeMainWindow->settings->writeShellSkript(m_timeMainWindow->abtList, m_timeMainWindow->m_punchClockList);
    }
    else
    {
        lastconn = connect(m_timeMainWindow->settings, &SCTimeXMLSettings::settingsPartWritten, this, &DateChanger::resetLists);
        expectedActions = 2;
        m_timeMainWindow->settings->writeSettings(m_timeMainWindow->abtList, m_timeMainWindow->m_punchClockList);
        m_timeMainWindow->settings->writeShellSkript(m_timeMainWindow->abtList, m_timeMainWindow->m_punchClockList);
    }
}

void DateChanger::resetLists()
{
    expectedActions--;
    if (expectedActions > 0)
        return;
    disconnect(lastconn);
    if (changeVisible && (m_timeMainWindow->abtListToday != m_timeMainWindow->abtList))
    {
        delete m_timeMainWindow->abtList;
        m_timeMainWindow->abtList = NULL;
        delete m_timeMainWindow->m_punchClockList;
        m_timeMainWindow->m_punchClockList = NULL;
    }
    if ((targetDate == m_timeMainWindow->abtListToday->getDatum()) && changeVisible)
    {
        m_timeMainWindow->abtList = m_timeMainWindow->abtListToday;
        m_timeMainWindow->m_punchClockList = m_timeMainWindow->m_punchClockListToday;
    }
    if (!(targetDate == m_timeMainWindow->abtListToday->getDatum()) && changeVisible)
    {
        m_timeMainWindow->abtList = new AbteilungsListe(targetDate, m_timeMainWindow->abtListToday);
        m_timeMainWindow->m_punchClockList = new PunchClockList();
    }
    if (changeToday)
    {
        if (m_timeMainWindow->abtListToday->getDatum() != currentDate)
        {
            if (m_timeMainWindow->abtList->getDatum() == currentDate)
            {
                m_timeMainWindow->abtListToday = m_timeMainWindow->abtList;
                m_timeMainWindow->m_punchClockListToday = m_timeMainWindow->m_punchClockList;
            }
            else
            {
                // if an entry of today is being edited, we don't switch the view to the current date
                if (m_timeMainWindow->entryBeingEdited && (m_timeMainWindow->abtListToday == m_timeMainWindow->abtList))
                {
                    m_timeMainWindow->abtListToday = new AbteilungsListe(targetDate, m_timeMainWindow->abtListToday);
                }
                else
                {
                    m_timeMainWindow->abtListToday->setDatum(targetDate);
                }
            }
        }
    }
    expectedActions = 2;
    lastconn = connect(m_timeMainWindow->settings, &SCTimeXMLSettings::settingsPartRead, this, &DateChanger::updatePunchClock);
    if (changeToday && (m_timeMainWindow->abtListToday != m_timeMainWindow->abtList))
    {
        m_timeMainWindow->abtListToday->clearKonten();
        m_timeMainWindow->m_punchClockListToday->clear();
        expectedActions+=2;
        m_timeMainWindow->settings->readSettings(m_timeMainWindow->abtListToday, m_timeMainWindow->m_punchClockListToday);
    }

    m_timeMainWindow->abtList->clearKonten();
    m_timeMainWindow->m_punchClockList->clear();
    m_timeMainWindow->settings->readSettings(m_timeMainWindow->abtList, m_timeMainWindow->m_punchClockList);
}

void DateChanger::updatePunchClock()
{
    expectedActions--;
    if (expectedActions > 0)
        return;
    disconnect(lastconn);
    auto lastentry = std::prev(m_timeMainWindow->m_punchClockListToday->end());
    auto now = QDateTime::currentDateTime();
    int diff = now.time().msecsSinceStartOfDay() / 1000 - (lastentry->second);
    if ((diff <= 90) && (diff >= -90))
    {
        logError("change date in range");
        m_timeMainWindow->m_punchClockListToday->setCurrentEntry(lastentry);
    }
    else
    {
        logError("change date out of range");
        m_timeMainWindow->m_punchClockListToday->push_back(PunchClockEntry(now.time().msecsSinceStartOfDay() / 1000, now.time().msecsSinceStartOfDay() / 1000));
        m_timeMainWindow->m_punchClockListToday->setCurrentEntry(std::prev(m_timeMainWindow->m_punchClockListToday->end()));
    }
    m_timeMainWindow->abtListToday->setAsAktiv(abt,ko,uko,idx);
    emit finished(targetDate, changeVisible, changeToday, currentDateSel);
}