/*
    Copyright (C) 2025 science+computing ag
       Authors: Florian Schmitt et al.

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

#ifndef TIMETRACKERREADER_H
#define TIMETRACKERREADER_H

#include <QObject>
#include <QDate>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

/**
 * Represents a single sctimeTracker task
 */
struct TimeTrackerTask {
    QDate date;
    QTime startTime;
    QTime endTime;
    QString label;
    QString comment;
    int durationSeconds;
    
    TimeTrackerTask(): date(), startTime(), endTime(), label(), comment(), durationSeconds(0) {}
    TimeTrackerTask(const QDate& _date, const QTime& _startTime, const QTime& _endTime, 
                    const QString& _label, const QString& _comment, int _durationSeconds)
        : date(_date), startTime(_startTime), endTime(_endTime), label(_label), comment(_comment), durationSeconds(_durationSeconds) {}
};

/**
 * TimeTrackerTasksReader reads sctimeTracker tasks from the /sctt_tasks REST endpoint
 * 
 * Usage:
 *   auto reader = new TimeTrackerTasksReader(networkAccessManager, dateFrom, dateTo);
 *   connect(reader, &TimeTrackerTasksReader::tasksReceived, this, &MyClass::handleTasks);
 *   connect(reader, &TimeTrackerTasksReader::failed, this, &MyClass::handleError);
 *   reader->fetchTasks();
 */
class TimeTrackerTasksReader: public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param networkAccessManager Network access manager for making HTTP requests
     * @param dateFrom The earliest date to include in the list (required)
     * @param dateTo The last date to include in the list (optional, uses current date if not set)
     * @param parent Parent QObject
     */
    TimeTrackerTasksReader(QNetworkAccessManager* networkAccessManager, 
                           const QDate& dateFrom, 
                           const QDate& dateTo = QDate(),
                           QObject* parent = nullptr);
    
    virtual ~TimeTrackerTasksReader();
    
    /**
     * Fetches tasks from the server and marks them as processed
     * @param fetchAll If true, returns all tasks; if false, returns only new tasks (default: false)
     */
    void fetchTasks(bool fetchAll = false);
    
signals:
    /**
     * Emitted when tasks are successfully received from the server
     * @param tasks List of TimeTrackerTask objects
     */
    void tasksReceived(const QList<TimeTrackerTask>& tasks);
    
    /**
     * Emitted when an error occurs
     * @param errorMessage Description of the error
     */
    void failed(const QString& errorMessage);
    
private slots:
    void handleReply();
    
private:
    void processJsonResponse(QNetworkReply* reply);
    QList<TimeTrackerTask> parseTasksFromJson(const QJsonDocument& doc);
    QString buildUrl(bool fetchAll) const;
    
    QNetworkAccessManager* networkAccessManager;
    QDate dateFrom;
    QDate dateTo;
};

#endif // TIMETRACKERREADER_H
