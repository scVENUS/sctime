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

#ifndef CONFLICTCALENDARWIDGET_H
#define CONFLICTCALENDARWIDGET_H


#include "QCalendarWidget"

class ConflictCalendarWidget : public QCalendarWidget
{
  Q_OBJECT
  public:
    explicit ConflictCalendarWidget(QWidget *parent = nullptr, QSet<QDate> conflictDates = QSet<QDate>());
    ~ConflictCalendarWidget() override;
    virtual void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;
    void setConflictDates(const QSet<QDate> &conflictDates); 
  private:
    QSet<QDate> m_conflictDates;
};

#endif // CONFLICTCALENDARWIDGET_H