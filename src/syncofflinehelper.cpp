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

#include "syncofflinehelper.h"
#include <QFile>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "globals.h"
#include "resthelper.h"
#include "xmlreader.h"
#include "xmlwriter.h"
#include "abteilungsliste.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define LASTSYNCFN "lastsync.txt"

QDateTime SyncOfflineHelper::getLastSyncTime() {
    if (m_lastSyncTime.isValid()) {
        return m_lastSyncTime;
    }
    QFile file(configDir.absoluteFilePath(LASTSYNCFN));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line = in.readLine();
        file.close();
        if (!line.isEmpty()) {
            m_lastSyncTime = QDateTime::fromString(line, Qt::ISODate);
        }
    }
    if (!m_lastSyncTime.isValid()) {
        m_lastSyncTime=QDate::currentDate().addDays(-90).startOfDay(); // default to 90 days ago if no valid last sync time is found
    }
    return m_lastSyncTime;
}


void SyncOfflineHelper::setLastSyncTime(const QDateTime &time) {
    m_lastSyncTime = time;
    QFile file(configDir.absoluteFilePath(LASTSYNCFN));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << time.toString(Qt::ISODate);
        file.close();
    }
}

void SyncOfflineHelper::syncAll()
{
    QString baseurl = getRestBaseUrl();
    auto lastSyncTime = getLastSyncTime();
    auto dateFrom = QDate::currentDate().addDays(-90);
    auto dateTo = QDate::currentDate().addYears(1);
    auto request = QNetworkRequest(QUrl(baseurl + "/" + REST_LIST_SETTINGS_ENDPOINT +
        "?modifiedFrom=" + QUrl::toPercentEncoding(lastSyncTime.toString(Qt::ISODate)) +
        "&dateFrom=" + QUrl::toPercentEncoding(dateFrom.toString("yyyy-MM-dd")) + "&dateTo=" + QUrl::toPercentEncoding(dateTo.toString("yyyy-MM-dd"))));
    QNetworkReply *reply = networkAccessManager->get(request);
    networkAccessManager->get(QNetworkRequest());
    connect(reply, &QNetworkReply::finished, [=]()
            {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            QList<ServerFileStatus>* serverFileStatuses= new QList<ServerFileStatus>;
            if (jsonResponse.isObject()) {
                QJsonObject jsonObject = jsonResponse.object();
                // get timestamp from the response
                QJsonValue timestampValue = jsonObject.value("timestamp");
                if (timestampValue.isString()) {
                    QString timestamp = timestampValue.toString();
                    QDateTime syncTime = QDateTime::fromString(timestamp, Qt::ISODate);
                    if (syncTime.isValid()) {
                        setLastSyncTime(syncTime);
                        //trace("Last sync time updated to: " + syncTime.toString(Qt::ISODate));
                    } else {
                        //trace("Invalid timestamp format received: " + timestamp);
                    }
                } else {
                    //trace("Timestamp not found in the response.");
                }
                QJsonValue settingsFilesMetaValue = jsonObject.value("settingsfilesmeta");
                if (settingsFilesMetaValue.isArray()) {
                  QJsonArray jsonArray = settingsFilesMetaValue.toArray();
                  for (const QJsonValue &value : jsonArray) {
                    // fill serverfilestatus with the data from the response
                    ServerFileStatus serverFileStatus;
                    if (value.isObject()) {
                        QJsonObject fileObject = value.toObject();
                        serverFileStatus.lastModified = QDateTime::fromString(fileObject.value("modified").toString(), Qt::ISODate);
                        serverFileStatus.clientId = fileObject.value("client_id").toString();
                        serverFileStatus.clientInfo = fileObject.value("client_info").toString();
                        serverFileStatus.date = QDate::fromString(fileObject.value("date").toString(), "yyyy-MM-dd");
                        serverFileStatuses->append(serverFileStatus);
                        //trace("Received file status: " + serverFileStatus.clientInfo + " for date " + serverFileStatus.date.toString() + " with last modified time " + serverFileStatus.lastModified.toString(Qt::ISODate));
                      } else {
                        logError("Received JSON value is not an object.");
                      }
                  }
                }
            } else  {
               logError("Received JSON response is not an object.");
            }

            connect(this, &SyncOfflineHelper::finishedRemoteToLocal, [this, serverFileStatuses]() {
                connect(this, &SyncOfflineHelper::finishedLocalToRemote, [this, serverFileStatuses]() {
                  delete serverFileStatuses;
#ifdef __EMSCRIPTEN__
                  // sync previously written files in browser
                  EM_ASM(
                    FS.syncfs(function (err) {});
                 );
#endif
                  emit finished();
                });
                syncLocalToRemoteList(*serverFileStatuses);
            });
            syncRemoteToLocalList(*serverFileStatuses);
             
        } else {
            logError("Error syncing remote data: " + reply->errorString());
        }
        reply->deleteLater(); });
}

void SyncOfflineHelper::nextStepRemoteToLocal() {
    partstodo--;
    if (partstodo <= 0) {
        partstodo=0;
        emit finishedRemoteToLocal();
    }
}

void SyncOfflineHelper::nextStepLocalToRemote() {
    partstodo--;
    if (partstodo <= 0) {
        partstodo=0;
        emit finishedLocalToRemote();
    }
}

void SyncOfflineHelper::syncRemoteToLocalList(QList<ServerFileStatus> &list) {
    partstodo++;
    for (const ServerFileStatus &fileStatus : list) {
        QString *filename = new QString("zeit-" + fileStatus.date.toString("yyyy-MM-dd") + ".xml");
        QFileInfo fileInfo(configDir.absoluteFilePath(*filename));
        bool fileExists = fileInfo.exists();
        // Check if the file is already up to date - the remote file cannot be newer than the local file, as it is written with the same client, but
        // be paranoid and also check for date with a small buffer
        if (fileStatus.clientId==clientId && fileExists && fileInfo.lastModified().addSecs(30) >= fileStatus.lastModified) {
            //trace("Skipping file " + *filename + " as it is already up to date.");
            continue;
        }
        if (fileStatus.date == tmw->getOpenDate() || fileStatus.date == tmw->getOpenCurrentDate()) {
            //trace("Skipping file " + *filename + " as it is currently open.");
            continue;
        }
        AbteilungsListe *abtList=tmw->getEmptyAbtList(fileStatus.date);
        PunchClockList *pcl=new PunchClockList();
        XMLReader* xmlReader= new XMLReader(settings, networkAccessManager, false, false, true, abtList, pcl);
        QDateTime* fileModified = new QDateTime(fileStatus.lastModified);
        QDate* date = new QDate(fileStatus.date);
        trace("Syncing remote file " + *filename + " for date " + fileStatus.date.toString("yyyy-MM-dd") + " with last modified time " + fileStatus.lastModified.toString(Qt::ISODate) + " and client ID " + fileStatus.clientId);
        partstodo++;
        connect(xmlReader, &XMLReader::settingsRead, [fileExists, filename, this, fileModified, abtList, pcl, xmlReader, date]() {
            QString targetFilename;
            QDateTime remoteDate=xmlReader->lastRemoteSaveTime();
            QString remoteID=xmlReader->lastRemoteID();
            QDateTime localDate;
            QString localID;
            if (fileExists) {
                targetFilename = configDir.absoluteFilePath(*filename)+".unmerged";
                QFile localFile(configDir.absoluteFilePath(*filename));
                QDomDocument localDoc;
                bool err = false;
                if (localFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                  if (!localDoc.setContent(&localFile)) {
                    logError("Failed to parse XML content in " + localFile.fileName());
                    err=true;
                  }
                  localFile.close();
                } else {
                  logError("Failed to open file " + localFile.fileName() + ": " + localFile.errorString());
                  err=true;
                }
                if (!err) {
                  AbteilungsListe *abtList=tmw->getEmptyAbtList(*date);
                  PunchClockList *pcl=new PunchClockList();
                  XMLReader localXmlReader(settings, networkAccessManager, false, true, true, abtList, pcl);
                  localXmlReader.fillSettingsFromDocument(localDoc, settings);
                  localID= localXmlReader.lastRemoteID();
                  localDate = localXmlReader.lastRemoteSaveTime();
                  delete abtList;
                  delete pcl;
                }
            } else {
                targetFilename = configDir.absoluteFilePath(*filename);
            }

            if (remoteDate==localDate && remoteID==localID) {
                trace("Remote file " + *filename + " is already up to date, skipping.");
            } else {
              if (remoteDate.isValid()) {
                trace("Remote file " + *filename + " has last modified time " + remoteDate.toString(Qt::ISODate) + " and client ID " + remoteID);
              } else {
                trace("Remote file " + *filename + " has no valid last modified time");
              }
              if (localDate.isValid()) {
                trace("Local file " + *filename + " has last modified time " + localDate.toString(Qt::ISODate) + " and client ID " + localID);
              } else {
                trace("Local file " + *filename + " has no valid last modified time");
              }
              trace("RemoteID is " + remoteID + " and localID is " + localID);
              if (fileExists) {
                uncleanDates.insert(*date);
              }

              XMLWriter xmlWriter(settings, networkAccessManager, abtList,pcl);
              // ensure the XMLWriter has the correct metadata
              xmlWriter.setSavetime(remoteDate);
              xmlWriter.setIdentifier(remoteID);


              QDomDocument doc = xmlWriter.settings2Doc(false);
              QFile file(targetFilename);
              if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out.setEncoding(QStringConverter::Utf8);
                out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
                out << doc.toString();
                file.setFileTime(*fileModified, QFileDevice::FileModificationTime);
                file.close();
              } else {
                  logError("Failed to write settings to " + targetFilename + ": " + file.errorString());
              }
            }
            xmlReader->deleteLater();
            delete abtList;
            delete filename;
            delete fileModified;
            delete pcl;
            delete date;
            nextStepRemoteToLocal();
        });
        xmlReader->open();
    }
    nextStepRemoteToLocal();
}

void SyncOfflineHelper::syncLocalToRemoteList(QList<ServerFileStatus> &list) {
    partstodo++;
    // loop over all files with extension .needssync in configDir
    QDir dir(configDir);
    QMap<QString, ServerFileStatus> serverFileStatusMap;
    for (const ServerFileStatus &fileStatus : list) {
        serverFileStatusMap.insert("zeit-"+fileStatus.date.toString("yyyy-MM-dd")+".needssync", fileStatus);
    }
    QStringList needSyncFiles = dir.entryList(QStringList() << "*.needssync", QDir::Files);
    for (const QString &fileNameNS : needSyncFiles) {
        QString filePathNS = dir.absoluteFilePath(fileNameNS);
        QString fileName = fileNameNS;
        fileName.replace(".needssync", ".xml");
        if (serverFileStatusMap.contains(fileNameNS)) {
            QFileInfo fileInfo(configDir.absoluteFilePath(fileNameNS));
            auto fs = serverFileStatusMap.value(fileNameNS);
            if (fs.lastModified>fileInfo.lastModified()) {
                // trace("Skipping file " + fileName + " as it is already up to date.");
                continue;
            }
        }
        
        QDate date = QDate::fromString(fileName.replace("zeit-", "").replace(".xml", ""), "yyyy-MM-dd");
        if (!date.isValid()) {
            logError("Invalid date in file name " + fileName + ": " + fileName.replace("zeit-", "").replace(".xml", ""));
            continue;
        }
        trace("Syncing local file " + fileName + " for date " + date.toString("yyyy-MM-dd") + " from " + filePathNS);
        QString filePath = filePathNS;
        filePath.replace(".needssync", ".xml");

        if (QFile::exists(filePath+".unmerged")) {
            logError("File " + filePath + " already exists as unmerged, skipping.");
            uncleanDates.insert(date);
            continue;
        }

        QFile file(filePath);
        QDomDocument doc;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            if (!doc.setContent(&file)) {
                logError("Failed to parse XML content in " + filePath);
                file.close();
                continue;
            }
            file.close();
        } else {
            logError("Failed to open file " + filePath + ": " + file.errorString());
            continue;
        }
        
        AbteilungsListe *abtList=tmw->getEmptyAbtList(date);
        PunchClockList *pcl=new PunchClockList();
        XMLReader xmlReader(settings, networkAccessManager, false, true, true, abtList, pcl);
        xmlReader.fillSettingsFromDocument(doc, settings);
        XMLWriter *xmlWriter=new XMLWriter(settings, networkAccessManager, abtList, pcl);
        QString* filePathNSPtr = new QString(filePathNS);
        partstodo++;
        connect(xmlWriter, &XMLWriter::settingsWritten, [=]() {
            QFile::remove(*filePathNSPtr);
            
            xmlWriter->deleteLater();

            delete abtList;
            delete pcl;
            delete filePathNSPtr;
            nextStepLocalToRemote();
        });
        xmlWriter->writeSettings(false);

    }
    nextStepLocalToRemote();

}

void SyncOfflineHelper::setNeedSyncMark(QDate date, bool needsync) {
    QString filename = "zeit-" + date.toString("yyyy-MM-dd") + ".needssync";
    QFile file(configDir.absoluteFilePath(filename));
    if (needsync) {
        if (!file.exists()) {
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file.close();
            } else {
                logError("Failed to create sync marker file: " + filename + ": " + file.errorString());
            }
        } 
    } else {
        if (file.exists()) {
            if (!file.remove()) {
               logError("Failed to remove sync marker file: " + filename + ": " + file.errorString());
            }
        } 
    }
}

void SyncOfflineHelper::removeUnmergedData(QDate date) {
    QString filename = "zeit-" + date.toString("yyyy-MM-dd") + ".xml.unmerged";
    //trace("Removing unmerged data file: " + filename);
    QFile file(configDir.absoluteFilePath(filename));
    if (file.exists()) {
       if (!file.remove()) {
          logError("Failed to remove unmerged data file: " + filename + ": " + file.errorString());
       } 
    }
}

QSet<QDate> SyncOfflineHelper::getLastUncleanDates()
{
    return uncleanDates;
}

QSet<QDate> SyncOfflineHelper::findAllUnmergedDates()
{
    QSet<QDate> unmergedDates;
    QDir dir(configDir);
    QStringList unmergedFiles = dir.entryList(QStringList() << "*.xml.unmerged", QDir::Files);
    for (QString fileName : unmergedFiles) {
        QDate date = QDate::fromString(fileName.replace("zeit-", "").replace(".xml.unmerged", ""), "yyyy-MM-dd");
        if (date.isValid()) {
            unmergedDates.insert(date);
        } else {
            logError("Invalid date in unmerged file name: " + fileName);
        }
    }
    return unmergedDates;
}