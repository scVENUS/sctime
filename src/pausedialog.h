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

#ifndef PAUSEDIALOG_H
#define PAUSEDIALOG_H

#include "ui_pausedialogbase.h"

#include <QDateTime>

/**
  * Dialog to show when paused
  */
class PauseDialog : public QDialog, private Ui::PauseDialogBase
{
  Q_OBJECT

public:
  PauseDialog(QDateTime starttime, QWidget* parent = 0);
  virtual ~PauseDialog();
public slots:
  /*$PUBLIC_SLOTS$*/
  virtual void          updateTime();

private:
  QDateTime starttime;
  QString text;
};

#endif