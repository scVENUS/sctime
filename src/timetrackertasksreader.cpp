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

#include "timetrackertasksreader.h"
#include "resthelper.h"
#include "globals.h"

#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

TimeTrackerTasksReader::TimeTrackerTasksReader(QNetworkAccessManager* networkAccessManager,
                                               const QDate& dateFrom,
                                               const QDate& dateTo,
                                               QObject* parent)
    : QObject(parent)
    , networkAccessManager(networkAccessManager)
    , dateFrom(dateFrom)
    , dateTo(dateTo.isValid() ? dateTo : QDate::currentDate())
{
}

TimeTrackerTasksReader::~TimeTrackerTasksReader()
{
}

QString TimeTrackerTasksReader::buildUrl(bool fetchAll) const
{
    QString baseUrl = getRestBaseUrl();
    QString endpoint = baseUrl + "/sctimegui/v1/sctt_tasks";
    
    QUrl url(endpoint);
    QUrlQuery query;
    
    // dateFrom is required
    query.addQueryItem("dateFrom", dateFrom.toString(Qt::ISODate));
    
    // dateTo is optional
    if (dateTo.isValid()) {
        query.addQueryItem("dateTo", dateTo.toString(Qt::ISODate));
    }
    
    // all parameter is optional
    if (fetchAll) {
        query.addQueryItem("all", "true");
    }
    
    url.setQuery(query);
    return url.toString();
}

void TimeTrackerTasksReader::fetchTasks(bool fetchAll)
{
    QString urlString = buildUrl(fetchAll);
    QUrl url(urlString);
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply* reply = networkAccessManager->sendCustomRequest(request, "PATCH");
    connect(reply, &QNetworkReply::finished, this, &TimeTrackerTasksReader::handleReply);
}

void TimeTrackerTasksReader::handleReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        emit failed(tr("Invalid reply object"));
        return;
    }
    
    processJsonResponse(reply);
}

void TimeTrackerTasksReader::processJsonResponse(QNetworkReply* reply)
{
    reply->deleteLater();
    
    // Check for network errors
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = tr("Network error while fetching tasks: %1").arg(reply->errorString());
        trace(errorMsg);
        emit failed(errorMsg);
        return;
    }
    
#ifdef ENSURE_REST_HEADER
    // Verify the sctime-rest-response header
    auto scheme = reply->url().scheme();
    if (scheme == "https" || scheme == "http") {
        auto sctimeRestResponse = getRestHeader(reply, "sctime-rest-response");
        if (QString(sctimeRestResponse) != "true") {
            QString errorMsg = tr("Invalid response from server: missing sctime-rest-response header");
            trace(errorMsg);
            emit failed(errorMsg);
            return;
        }
    }
#endif
    
    // Read response data
    QByteArray responseData = reply->readAll();
    
    // Parse JSON
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        QString errorMsg = tr("JSON parse error: %1").arg(parseError.errorString());
        trace(errorMsg);
        emit failed(errorMsg);
        return;
    }
    
    // Parse tasks from JSON
    QList<TimeTrackerTask> tasks = parseTasksFromJson(doc);
    
    emit tasksReceived(tasks);
}

QList<TimeTrackerTask> TimeTrackerTasksReader::parseTasksFromJson(const QJsonDocument& doc)
{
    QList<TimeTrackerTask> tasks;
    
    if (!doc.isArray()) {
        QString errorMsg = tr("Expected JSON array but got different type");
        trace(errorMsg);
        emit failed(errorMsg);
        return tasks;
    }
    
    QJsonArray tasksArray = doc.array();
    
    for (const QJsonValue& taskValue : tasksArray) {
        if (!taskValue.isObject()) {
            continue;
        }
        
        QJsonObject taskObj = taskValue.toObject();
        
        // Parse time_span object
        QJsonObject timeSpan = taskObj.value("time_span").toObject();
        QString startDateTimeStr = timeSpan.value("start").toString();
        QString endDateTimeStr = timeSpan.value("end").toString();
        
        // Parse start and end as ISO 8601 date-time strings
        QDateTime startDateTime = QDateTime::fromString(startDateTimeStr, Qt::ISODate);
        QDateTime endDateTime = QDateTime::fromString(endDateTimeStr, Qt::ISODate);
        
        if (!startDateTime.isValid() || !endDateTime.isValid() || startDateTime > endDateTime || startDateTime.date() != endDateTime.date()) {
            // TODO: do we want to create an error message box here?
            trace(tr("Invalid time_span format for task"));
            continue;
        }
        
        // Get task_data object
        QJsonObject taskData = taskObj.value("task_data").toObject();
        
        // Extract label and comment from task_data
        QString label = taskData.value("label").toString();
        QString comment = taskData.value("comment").toString();
        int seconds= (endDateTime.toSecsSinceEpoch() - startDateTime.toSecsSinceEpoch());
        
        // Create task object using date from start time
        TimeTrackerTask task(startDateTime.date(), startDateTime.time(), endDateTime.time(), label, comment, seconds);
        
        tasks.append(task);
    }
    
    return tasks;
}
