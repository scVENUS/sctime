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

class SCTimeXMLSettings;
class AbteilungsListe;
class PunchClockList;
class TimeMainWindow;
class QNetworkAccessManager;

class ConflictDialog : public QDialog, private Ui::ConflictDialogBase
{
  Q_OBJECT
public:
  ConflictDialog(SCTimeXMLSettings* settings, QNetworkAccessManager* networkAccessManager, QDate targetdate, bool global, const QByteArray remoteBA, TimeMainWindow* tmw);
  ConflictDialog(SCTimeXMLSettings* settings, QNetworkAccessManager* networkAccessManager, QDate targetdate, bool global, const QByteArray remoteBA, TimeMainWindow* tmw, QDomDocument docLocal);
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
  QByteArray remoteBA;
  QDate targetdate;
  QDomDocument docLocal;
  QNetworkAccessManager* networkAccessManager;
};

#endif