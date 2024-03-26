#include "datechanger.h"

#include <QDate>
#include "abteilungsliste.h"
#include "sctimexmlsettings.h"
#include "xmlwriter.h"
#include "xmlreader.h"
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
        expectedActions = 2;
        
        // TODO: shell scripts are not written asynchronously
        m_timeMainWindow->settings->writeShellSkript(m_timeMainWindow->abtListToday, m_timeMainWindow->m_punchClockListToday);
        m_timeMainWindow->settings->writeShellSkript(m_timeMainWindow->abtList, m_timeMainWindow->m_punchClockList);
        write(m_timeMainWindow->abtListToday, m_timeMainWindow->m_punchClockListToday);
        write(m_timeMainWindow->abtList, m_timeMainWindow->m_punchClockList);
    }
    else
    {
        expectedActions = 1;
        m_timeMainWindow->settings->writeShellSkript(m_timeMainWindow->abtList, m_timeMainWindow->m_punchClockList);
        write(m_timeMainWindow->abtList, m_timeMainWindow->m_punchClockList);
    }
}

void DateChanger::write(AbteilungsListe* abtlist, PunchClockList* pcl) {
    XMLWriter *writer=new XMLWriter(m_timeMainWindow->settings, networkAccessManager, abtlist, pcl);
    connect(writer, &XMLWriter::settingsWritten, this, &DateChanger::resetLists, Qt::QueuedConnection);
    connect(writer, &XMLWriter::settingsWritten, writer, &XMLWriter::deleteLater, Qt::QueuedConnection);
    connect(writer, &XMLWriter::settingsWriteFailed, writer, &XMLWriter::deleteLater, Qt::QueuedConnection);
    connect(writer, &XMLWriter::offlineSwitched, [this](bool offline){emit offlineSwitched(offline);});
    writer->writeAllSettings();
}

void DateChanger::resetLists()
{
    expectedActions--;
    if (expectedActions > 0)
        return;
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
    expectedActions = 1;
    
    if (changeToday && (m_timeMainWindow->abtListToday != m_timeMainWindow->abtList))
    {
        m_timeMainWindow->abtListToday->clearKonten();
        m_timeMainWindow->m_punchClockListToday->clear();
        expectedActions++;
        XMLReader *reader=new XMLReader(m_timeMainWindow->settings, networkAccessManager,true, false, true, m_timeMainWindow->abtListToday, m_timeMainWindow->m_punchClockListToday);
        connect(reader, &XMLReader::settingsRead, this, &DateChanger::updatePunchClock, Qt::QueuedConnection);
        connect(reader, &XMLReader::settingsRead, reader, &XMLWriter::deleteLater, Qt::QueuedConnection);
        connect(reader, &XMLReader::offlineSwitched, [this](bool offline){emit offlineSwitched(offline);});
        reader->open();
    }

    m_timeMainWindow->abtList->clearKonten();
    m_timeMainWindow->m_punchClockList->clear();
    XMLReader *reader=new XMLReader(m_timeMainWindow->settings, networkAccessManager, true, false, true, m_timeMainWindow->abtList, m_timeMainWindow->m_punchClockList);
    connect(reader, &XMLReader::settingsRead, this, &DateChanger::updatePunchClock, Qt::QueuedConnection);
    connect(reader, &XMLReader::settingsRead, reader, &XMLWriter::deleteLater, Qt::QueuedConnection);
    connect(reader, &XMLReader::offlineSwitched, [this](bool offline){emit offlineSwitched(offline);});
    reader->open();
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