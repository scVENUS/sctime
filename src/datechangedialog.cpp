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

#include "datechangedialog.h"
#include <QDir>
#include <QTextCharFormat>
#include <QBrush>
#include "globals.h"

DateChangeDialog::DateChangeDialog(const QDate& date, QWidget *parent, QSet<QDate> conflictDates)
    : DateDialog(date, parent, true)
{
    datePicker->setConflictDates(conflictDates);
}

DateChangeDialog::~DateChangeDialog()
{
}

void DateChangeDialog::setSelectedDate(const QDate& date)
{
    // Only highlight if month/year changed
    if (date.month() != selectedDate.month() || date.year() != selectedDate.year()) {
        QDir checkedinDir(configDir.filePath("checkedin"));
        QString pattern = QString("zeit-%1-%2-*.xml")
            .arg(date.year())
            .arg(QString("%1").arg(date.month(), 2, 10, QChar('0')));
        QStringList files = checkedinDir.entryList(QStringList() << pattern, QDir::Files);

        for (const QString& filename : files) {
            QStringList parts = filename.split('-');
            if (parts.size() >= 4) {
                QString dayPart = parts[3].split('.').first();
                bool validDay = false;
                int dayNum = dayPart.toInt(&validDay);
                if (validDay) {
                    QDate highlightDate(date.year(), date.month(), dayNum);
                    QTextCharFormat fmt = datePicker->dateTextFormat(highlightDate);
                    fmt.setForeground(QBrush(Qt::red));
                    fmt.setBackground(QBrush(Qt::red));
                    datePicker->setDateTextFormat(highlightDate, fmt);
                }
            }
        }
    }
    // Call base implementation for the rest
    DateDialog::setSelectedDate(date);
}