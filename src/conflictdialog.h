/*

    Copyright (C) 2024 science+computing ag
       Authors: Florian Schmitt, Marcus Camen et al.

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

#ifndef CONFLICTDIALOG_H
#define CONFLICTDIALOG_H

#include "ui_conflictdialogbase.h"
#include <QDate>
#include <QDomDocument>
#include <QMessageBox>

class SCTimeXMLSettings;
class AbteilungsListe;
class PunchClockList;
class TimeMainWindow;
class QNetworkAccessManager;

class ConflictDialog : public QDialog, private Ui::ConflictDialogBase
{
  Q_OBJECT
public:
  ConflictDialog(SCTimeXMLSettings* settings, QNetworkAccessManager* networkAccessManager, QDate targetdate, bool global, const QDomDocument docRemote, TimeMainWindow* tmw);
  ConflictDialog(SCTimeXMLSettings* settings, QNetworkAccessManager* networkAccessManager, QDate targetdate, bool global, const QDomDocument docRemote, TimeMainWindow* tmw, QDomDocument docLocal);
  virtual ~ConflictDialog() {};

public slots:
  void performMerge();
  void performReplace();
  void performClose();
  void performKeep();
  void errorDialog();

private:
  void mergeAbtList(AbteilungsListe* target, AbteilungsListe* other);
  TimeMainWindow* tmw;
  SCTimeXMLSettings* settings;
  bool global;
  AbteilungsListe* conflictedAbtList;
  PunchClockList * conflictedPunchClockList;
  QDomDocument docRemote;
  QDate targetdate;
  QDomDocument docLocal;
  QNetworkAccessManager* networkAccessManager;
};

class ConflictDialogSameBrowser: public QMessageBox
{
  Q_OBJECT
public:
  ConflictDialogSameBrowser(QWidget* parent):QMessageBox(QMessageBox::Warning,
            tr("sctime: conflict with session in same browser"),
            tr("There seems to be a conflict with another session in the same browser. Please use only one session at a time.\nHint: you may also get this message, if you have just restarted a running sctime in this browser."),
            QMessageBox::Ok, parent) {};
  virtual ~ConflictDialogSameBrowser() {};
};
#endif
