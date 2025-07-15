/*

    Copyright (C) 2025 science+computing ag
       Authors: Florian Schmitt

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

#include "conflictcalendarwidget.h"
#include <QPainter>
#include <QWidget>

ConflictCalendarWidget::~ConflictCalendarWidget() {
}

ConflictCalendarWidget::ConflictCalendarWidget(QWidget *parent, QSet<QDate> conflictDates)
    : QCalendarWidget(parent), m_conflictDates(conflictDates)
{
}

void ConflictCalendarWidget::paintCell(QPainter *painter, const QRect &rect, QDate date) const {
    painter->save();
    if (m_conflictDates.contains(date)) {
       painter->fillRect(rect, QColor(255, 240, 200));
       painter->setCompositionMode(QPainter::CompositionMode_Darken);
    }
    QCalendarWidget::paintCell(painter, rect, date);
    painter->restore();
}

void ConflictCalendarWidget::setConflictDates(const QSet<QDate> &conflictDates) {
    m_conflictDates = conflictDates;
    update(); // Trigger a repaint to reflect the new conflict dates
}