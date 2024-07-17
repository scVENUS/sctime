/*
    Copyright (C) 2024 science+computing ag
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

#ifndef CHECKINDIALOG_H
#define CHECKINDIALOG_H

#include "ui_checkindialogbase.h"
#include "abteilungsliste.h"
#include "punchclock.h"
#include <QDate>
#include <QList>
#include <QStringList>
#include <QNetworkAccessManager>

class SCTimeXMLSettings;

class CheckinDialog : public QDialog, private Ui::CheckinDialogBase
{
    Q_OBJECT
public:
    CheckinDialog(SCTimeXMLSettings* settings, QNetworkAccessManager* networkAccessManager, QWidget *parent = nullptr);
    ~CheckinDialog();

private slots:
    void checkin();
    void skip();
    void openDate(const QDate& date);
    void openDateForItem(QListWidgetItem *current, QListWidgetItem *previous=NULL);
    void loadDate();
private:
    QStringList listNotCheckedIn();
    AbteilungsListe* abtList;
    PunchClockList* pcl;
    SCTimeXMLSettings* settings;
    QNetworkAccessManager* networkAccessManager;
};

#endif // CHECKINDIALOG_H
