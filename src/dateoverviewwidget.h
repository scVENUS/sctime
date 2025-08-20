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

#ifndef DATEOVERVIEWWIDGET_H
#define DATEOVERVIEWWIDGET_H

#include "ui_dateoverviewwidgetbase.h"
#include "abteilungsliste.h"
#include "punchclock.h"
#include <QDate>

class SCTimeXMLSettings;

class DateOverviewWidget : public QWidget, private Ui::DateOverviewWidgetBase
{
    Q_OBJECT
public:
    DateOverviewWidget(QWidget *parent = nullptr);
    virtual ~DateOverviewWidget();

public slots:
    void loadDate(AbteilungsListe* abtList, PunchClockList* pcl, SCTimeXMLSettings* settings);
};

#endif // DATEOVERVIEWWIDGET_H
