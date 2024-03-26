/*
    Copyright (C) 2023 science+computing ag
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

#ifndef DOWNLOADSHDIALOG_H
#define DOWNLOADSHDIALOG_H

#include "ui_downloadshdialogbase.h"

#include <QDateTime>

class SCTimeXMLSettings;
class AbteilungsListe;
class PunchClockList;
class QBuffer;
class QZipWriter;
class QNetworkAccessManager;

/**
  * Dialog to show when paused
  */
class DownloadSHDialog : public QDialog, private Ui::DownloadSHDialogBase
{
  Q_OBJECT

public:
  DownloadSHDialog(SCTimeXMLSettings* settings, QNetworkAccessManager *networkAccessManager, QWidget* parent = 0);
  virtual ~DownloadSHDialog();
public slots:
  /*$PUBLIC_SLOTS$*/

signals:
  void workflowFinished();

private slots:
  void saveZip();
  void addFile();

private:
  SCTimeXMLSettings *settings;
  AbteilungsListe* abtList;
  PunchClockList* pcl;
  QDate currentDate;
  QDate endDate;
  QDate startDate;
  QByteArray zipData;
  QBuffer* zipBuffer;
  QZipWriter* zipWriter;
  QNetworkAccessManager *networkAccessManager;
};

#endif
