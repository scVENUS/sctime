/*
    Copyright (C) 2018 science+computing ag
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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>
#include <QDir>
class DatasourceManager;
class Lock;
class QNetworkReply;
extern QDir configDir;
extern QString lockfilePath;
extern QString SCTIME_IPC;
extern QString clientId;
extern QString clientinfo;
void logError(const QString& msg);
void trace(const QString& msg);
QString absolutePath(QString);
QString getMachineIdentifier();

#define REST_SETTINGS_ENDPOINT "sctimegui/v1/settingsdata"
#define REST_LIST_SETTINGS_ENDPOINT "sctimegui/v1/listsettingsdata"
#define REST_COMMITED_ENDPOINT "sctimegui/v1/commiteddata"
#define REST_ACCOUNTINGMETA_ENDPOINT "sctimegui/v1/accountingmetadata"
#define STATIC_URL "static/"
#define REFRESH_URL_PART "refresh.html"

#endif
