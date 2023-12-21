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

#ifndef ONCALLDIALOG_H
#define ONCALLDIALOG_H

#include <QDialog>
#include <QStringList>

class  BereitschaftsView;

/**
  * Dialog to show when paused
  */
class OnCallDialog : public QDialog
{
  Q_OBJECT

public:
  OnCallDialog(QString abt, QString ko, QString uko, QStringList onCallCategories, QWidget* parent = 0);
  virtual ~OnCallDialog() {};
public slots:
  /*$PUBLIC_SLOTS$*/
private slots:
  virtual void postprocess();

signals:
  void finishedWithInfo(QString abt, QString ko, QString uko, QStringList onCallCategoriesOrig, QStringList onCallCategoriesNew);

private:
  BereitschaftsView* onCallView;
  QStringList onCallCategoriesOrig;
  QString abt;
  QString ko;
  QString uko;
};

#endif