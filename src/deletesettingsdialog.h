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

#ifndef DELETESETTINGSDIALOG_H
#define DELETESETTINGSDIALOG_H

#include "ui_deletesettingsdialogbase.h"

class QNetworkAccessManager;

/**
  * Dialog to reset/delete settings
  */
class DeleteSettingsDialog : public QDialog, private Ui::DeleteSettingsDialogBase
{
  Q_OBJECT

signals:
  void processingDone();

public:
  DeleteSettingsDialog(QWidget* parent, QNetworkAccessManager* nam);
  virtual ~DeleteSettingsDialog();
  static void stopApp();
public slots:
  /*$PUBLIC_SLOTS$*/
  virtual void checkInput();
  virtual void performDelete();

private:
  QNetworkAccessManager* networkAccessManager;
};

#endif