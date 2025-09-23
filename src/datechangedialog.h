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

#pragma once
#include "datedialog.h"

class DateChangeDialog : public DateDialog
{
    Q_OBJECT
public:
    DateChangeDialog(const QDate& date, QWidget *parent = nullptr, QSet<QDate> conflictDates = QSet<QDate>());
    ~DateChangeDialog();

protected:
    void setSelectedDate(const QDate& date) override;
};