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

#ifdef __EMSCRIPTEN__
#include <emscripten/val.h>
#include <emscripten.h>
#endif

DeleteSettingsDialog::DeleteSettingsDialog(QWidget *parent, QNetworkAccessManager* nam)
: QDialog(parent), networkAccessManager(nam) 
{
  setupUi(this);
  this->setModal(true);
  connect(this,&DeleteSettingsDialog::finished, this, &DeleteSettingsDialog::checkInput);
}

DeleteSettingsDialog::~DeleteSettingsDialog() {
};

void DeleteSettingsDialog::checkInput() {
    if (result()!=QDialog::Accepted) {
      return;
    }
    bool showWarn=dailyLocalSettingsCheckbox->isChecked();
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
  if (dailyLocalSettingsCheckbox->isChecked()) {
    QStringList filters;
    auto dir = configDir;
    filters << "zeit-*.xml" << "zeit-*.sh";
    dir.setNameFilters(filters);
    for (const QString &entry : dir.entryList()) {
        trace(QString("File to delete: %1").arg(entry));
        bool result=QFile::remove(dir.filePath(entry));
        if (!result) {
            trace(QString("File not deleted: %1").arg(entry));
        }
    }
  }
  #ifdef __EMSCRIPTEN__
  EM_ASM(
     FS.syncfs(false, function (err) {});
  );
  #endif
  bool doStop=stopAppCheckbox->isChecked();
  if (globalServerSettingsCheckBox->isChecked()) {
    QString baseurl=getRestBaseUrl();
    auto url=QUrl(baseurl + "/" + REST_SETTINGS_ENDPOINT);
    QNetworkRequest request(url);
    QNetworkReply *reply = networkAccessManager->deleteResource(request);
    QWidget* parent=dynamic_cast<QWidget*>(this->parent());
    connect(reply, &QNetworkReply::finished, [reply,doStop, parent, this](){
        if (reply->error()!=0) {
            QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
                QObject::tr("sctime: error on deleting file on server"),
                tr("The file could not be deleted on the server. Error code is %1").arg(reply->error()),QMessageBox::NoButton,parent);
          connect(msgbox, &QMessageBox::finished, msgbox, &QMessageBox::deleteLater);
          msgbox->open();
          msgbox->raise();
        } else 
        if (doStop) {
          DeleteSettingsDialog::stopApp();
        }
        reply->deleteLater();
        emit processingDone();
    });
  } else {
    if (doStop) {
        DeleteSettingsDialog::stopApp();
    }
    emit processingDone();
  }
}

void DeleteSettingsDialog::stopApp() {
  qApp->exit();
  #ifdef __EMSCRIPTEN__
      emscripten_run_script(QString(" document.documentElement.innerHTML = \"<html><head><title>sctime - session closed</title></head><body>This session has been closed.</body></html>\"").toUtf8().data());
  #else
      exit(0);
  #endif
}