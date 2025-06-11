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

#ifndef SYNCOFFLINEHELPER
#define SYNCOFFLINEHELPER

#include <QObject>
#include <QDateTime>
#include <QStringList>
#include "timemainwindow.h"

class QNetworkAccessManager;

class ServerFileStatus {
public:
  QString clientInfo;
  QDateTime lastModified;
  QString clientId;
  QDate date;
};

class SyncOfflineHelper: public QObject {
        Q_OBJECT;
public:
       SyncOfflineHelper(SCTimeXMLSettings* settings, QNetworkAccessManager *networkAccessManager, TimeMainWindow* tmw):  QObject(tmw), networkAccessManager(networkAccessManager),settings(settings),tmw(tmw)  {
          partstodo=0;
       };
       static void setNeedSyncMark(QDate date, bool needsync);
       static void removeUnmergedData(QDate date);
       QSet<QDate> getUncleanDates();
public slots:
       void syncAll();
private slots:
       QDateTime getLastSyncTime();
       void setLastSyncTime(const QDateTime &time);
       void nextStepRemoteToLocal();
       void nextStepLocalToRemote();

signals:
       void finishedRemoteToLocal();
       void finishedLocalToRemote();
       void finished();

private:
       QNetworkAccessManager *networkAccessManager;
       QDateTime m_lastSyncTime;
       void syncRemoteToLocalList(QList<ServerFileStatus> &list);
       void syncLocalToRemoteList(QList<ServerFileStatus> &list);
       SCTimeXMLSettings* settings;
       TimeMainWindow* tmw;
       int partstodo;
       QSet<QDate> uncleanDates;
};

#endif