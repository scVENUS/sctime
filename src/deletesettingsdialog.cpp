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

#include "deletesettingsdialog.h"
#include "globals.h"
#include "resthelper.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSharedPointer>
#include <QTimer>
#include <functional>

#ifdef __EMSCRIPTEN__
#include <emscripten/val.h>
#include <emscripten.h>
#endif

DeleteSettingsDialog::DeleteSettingsDialog(QWidget *parent, QNetworkAccessManager* nam)
: QDialog(parent), networkAccessManager(nam) 
{
  setupUi(this);
  this->setModal(true);
  fromDateEdit->setDisplayFormat("yyyy-MM-dd");
  toDateEdit->setDisplayFormat("yyyy-MM-dd");
  fromDateEdit->setCalendarPopup(true);
  toDateEdit->setCalendarPopup(true);
  fromDateEdit->setDate(QDate::currentDate());
  toDateEdit->setDate(QDate::currentDate());
  connect(this,&DeleteSettingsDialog::finished, this, &DeleteSettingsDialog::checkInput);
}

DeleteSettingsDialog::~DeleteSettingsDialog() {
};

void DeleteSettingsDialog::accept() {
    if (fromDateEdit->date() > toDateEdit->date()) {
        QMessageBox::warning(this, tr("sctime: invalid date range"),
            tr("The \"From\" date must not be after the \"To\" date."));
        return;
    }
    QDialog::accept();
}

void DeleteSettingsDialog::checkInput() {
    if (result()!=QDialog::Accepted) {
      return;
    }
    bool showWarn=dailyLocalSettingsCheckbox->isChecked() || dailyServerSettingsCheckbox->isChecked();
    if (showWarn) {
        QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
            QObject::tr("sctime: deleting data"),
            tr("This operation deletes all your working times that you have not checked in.\nProceed?"),QMessageBox::Yes|QMessageBox::No,dynamic_cast<QWidget*>(this->parent()));
        connect(msgbox, &QMessageBox::finished, msgbox, &QMessageBox::deleteLater);
        connect(msgbox, &QMessageBox::accepted, this, &DeleteSettingsDialog::performDelete);
        msgbox->open();
        msgbox->raise();
    } else {
        performDelete();
    }
}

void DeleteSettingsDialog::performDelete() {
  emit deletionStarted();
  if (globalLocalSettingsCheckBox->isChecked()) {
    QString filepath=configDir.filePath("settings.xml");
    bool result = QFile::remove(filepath);
    if (!result) {
        trace(QString("File not deleted: %1").arg(filepath));
    }
    filepath=configDir.filePath("settings.xml.bak");
    result = QFile::remove(filepath);
    if (!result) {
        trace(QString("File not deleted: %1").arg(filepath));
    }
  }
  QDate fromDate=fromDateEdit->date();
  QDate toDate=toDateEdit->date();
  if (dailyLocalSettingsCheckbox->isChecked()) {
    trace(QString("performDelete: deleting local daily settings from %1 to %2").arg(fromDate.toString("yyyy-MM-dd"), toDate.toString("yyyy-MM-dd")));
    QStringList filters;
    auto dir = configDir;
    filters << "zeit-*.xml" << "zeit-*.sh" << "zeit-*.xml.unmerged" << "zeit-*.needssync";
    dir.setNameFilters(filters);
    for (const QString &entry : dir.entryList()) {
        // filenames are "zeit-yyyy-MM-dd<suffix>" (.xml, .sh, .xml.unmerged, .needssync, ...);
        QDate fileDate;
        if (entry.startsWith("zeit-")) {
            fileDate=QDate::fromString(entry.mid(5, 10), "yyyy-MM-dd");
        }
        if (!fileDate.isValid()) {
            // can't confirm it's in range, so don't delete it rather than risk deleting outside the requested range
            logError(QString("performDelete: could not determine date of %1, skipping").arg(entry));
            continue;
        }
        if (fileDate<fromDate || fileDate>toDate) {
            continue;
        }
        trace(QString("File to delete: %1").arg(entry));
        bool result=QFile::remove(dir.filePath(entry));
        if (!result) {
            trace(QString("File not deleted: %1").arg(entry));
        }
    }
  }
  bool doStop=stopAppCheckbox->isChecked();
  QWidget* parent=dynamic_cast<QWidget*>(this->parent());

  auto finalize=[this,doStop](){
      if (doStop) {
          DeleteSettingsDialog::stopApp();
      }
      emit processingDone(doStop);
  };

#ifdef __EMSCRIPTEN__
  // FS.syncfs() persists the deletions above to IndexedDB asynchronously. stopApp() clears all
  // JS timers/intervals, which can abort that persist if it hasn't finished yet, silently
  // reverting the deletion on next reload. So wait (with a bounded retry) until it is done.
  trace("performDelete: persisting local filesystem changes");
  EM_ASM(
     window.sctimeDeleteFsSynced = false;
     FS.syncfs(false, function (err) {
         if (err) { console.error("sctime: FS.syncfs after delete failed: " + err); }
         window.sctimeDeleteFsSynced = true;
     });
  );
  QSharedPointer<std::function<void()>> waitForFsSync(new std::function<void()>());
  QSharedPointer<int> fsyncRetries(new int(0));
  *waitForFsSync = [this,finalize,waitForFsSync,fsyncRetries](){
      int synced=EM_ASM_INT({ return window.sctimeDeleteFsSynced ? 1 : 0; });
      if (synced) {
          trace("performDelete: local filesystem changes persisted");
          finalize();
          return;
      }
      if (++(*fsyncRetries)>=50) {
          logError("performDelete: gave up waiting for filesystem sync to persist deletions");
          finalize();
          return;
      }
      QTimer::singleShot(100, this, [waitForFsSync](){ (*waitForFsSync)(); });
  };
  auto finalizeAfterFsSync=[waitForFsSync](){ (*waitForFsSync)(); };
#else
  auto finalizeAfterFsSync=finalize;
#endif

  int pendingOps=0;
  if (globalServerSettingsCheckBox->isChecked()) pendingOps++;
  if (dailyServerSettingsCheckbox->isChecked()) pendingOps++;

  if (pendingOps==0) {
    finalizeAfterFsSync();
    return;
  }

  QSharedPointer<int> remainingOps(new int(pendingOps));
  auto finalizeOp=[remainingOps,finalizeAfterFsSync](){
      if (--(*remainingOps)<=0) {
          finalizeAfterFsSync();
      }
  };

  if (globalServerSettingsCheckBox->isChecked()) {
    QString baseurl=getRestBaseUrl();
    auto url=QUrl(baseurl + "/" + REST_SETTINGS_ENDPOINT);
    QNetworkRequest request(url);
    QNetworkReply *reply = networkAccessManager->deleteResource(request);
    connect(reply, &QNetworkReply::finished, [reply, parent, finalizeOp](){
        if (reply->error()!=0) {
            QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
                QObject::tr("sctime: error on deleting file on server"),
                tr("The file could not be deleted on the server. Error code is %1").arg(reply->error()),QMessageBox::NoButton,parent);
          connect(msgbox, &QMessageBox::finished, msgbox, &QMessageBox::deleteLater);
          msgbox->open();
          msgbox->raise();
        }
        reply->deleteLater();
        finalizeOp();
    });
  }

  if (dailyServerSettingsCheckbox->isChecked()) {
    QString baseurl=getRestBaseUrl();
    auto listUrl=QUrl(baseurl + "/" + REST_LIST_SETTINGS_ENDPOINT +
        "?dateFrom=" + QUrl::toPercentEncoding(fromDate.toString("yyyy-MM-dd")) +
        "&dateTo=" + QUrl::toPercentEncoding(toDate.toString("yyyy-MM-dd")) +
        "&modifiedFrom=" + QUrl::toPercentEncoding("1900-01-01T00:00:00Z"));
    QNetworkRequest listRequest(listUrl);
    QNetworkReply *listReply = networkAccessManager->get(listRequest);
    connect(listReply, &QNetworkReply::finished, [this,listReply,parent,baseurl,finalizeOp](){
        if (listReply->error()!=0) {
            QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
                QObject::tr("sctime: error on listing settings on server"),
                tr("The list of daily settings could not be retrieved from the server. Error code is %1").arg(listReply->error()),QMessageBox::NoButton,parent);
            connect(msgbox, &QMessageBox::finished, msgbox, &QMessageBox::deleteLater);
            msgbox->open();
            msgbox->raise();
            listReply->deleteLater();
            finalizeOp();
            return;
        }
        QJsonDocument jsonResponse=QJsonDocument::fromJson(listReply->readAll());
        listReply->deleteLater();
        QStringList dates;
        if (jsonResponse.isObject()) {
            QJsonValue metaValue=jsonResponse.object().value("settingsfilesmeta");
            if (metaValue.isArray()) {
                for (const QJsonValue &value : metaValue.toArray()) {
                    if (value.isObject()) {
                        QString dateStr=value.toObject().value("date").toString();
                        if (!dateStr.isEmpty()) {
                            dates.append(dateStr);
                        }
                    }
                }
            }
        }
        trace(QString("performDelete: server listed %1 daily settings file(s) to delete").arg(dates.size()));
        if (dates.isEmpty()) {
            finalizeOp();
            return;
        }
        QSharedPointer<int> remainingDeletes(new int(dates.size()));
        for (const QString &dateStr : dates) {
            auto deleteUrl=QUrl(baseurl + "/" + REST_SETTINGS_ENDPOINT + "?date=" + QUrl::toPercentEncoding(dateStr));
            QNetworkRequest deleteRequest(deleteUrl);
            QNetworkReply *deleteReply = networkAccessManager->deleteResource(deleteRequest);
            connect(deleteReply, &QNetworkReply::finished, [deleteReply,parent,dateStr,remainingDeletes,finalizeOp](){
                if (deleteReply->error()!=0) {
                    QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
                        QObject::tr("sctime: error on deleting file on server"),
                        tr("The settings for %1 could not be deleted on the server. Error code is %2").arg(dateStr).arg(deleteReply->error()),QMessageBox::NoButton,parent);
                    connect(msgbox, &QMessageBox::finished, msgbox, &QMessageBox::deleteLater);
                    msgbox->open();
                    msgbox->raise();
                } else {
                    trace(QString("performDelete: deleted settings on server for %1").arg(dateStr));
                }
                deleteReply->deleteLater();
                if (--(*remainingDeletes)<=0) {
                    finalizeOp();
                }
            });
        }
    });
  }
}

void DeleteSettingsDialog::stopApp() {
   stopAppHard();
}