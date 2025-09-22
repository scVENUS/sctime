/*

    Copyright (C) 2023 science+computing ag
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

#ifndef PUNCHCLOCKDIALOG_H
#define PUNCHCLOCKDIALOG_H

#include "ui_punchclockdialogbase.h"
#include <QDateTime>
#include <QTime>
#include <QDialog>
#include "punchclock.h"

class PunchClockStateBase;


 /**
  * Dialog to edit punch clock sheet
  */
class PunchClockDialog : public QDialog, private Ui::PunchClockDialogBase
{
  Q_OBJECT

public:
  PunchClockDialog(PunchClockList *pcl, PunchClockStateBase *pcs, QWidget* parent = 0);
  ~PunchClockDialog();
  /*$PUBLIC_FUNCTIONS$*/
  virtual void          fillFromList(PunchClockList *pcl);
  virtual void          copyToList(PunchClockList *pcl);

public slots:
  /*$PUBLIC_SLOTS$*/
  virtual void          insertEntry(int row, QTime begin, QTime end);

protected:
  /*$PROTECTED_FUNCTIONS$*/
  bool eventFilter(QObject *object, QEvent *event) override;

protected slots:
  /*$PROTECTED_SLOTS$*/
  virtual void          accept();
  virtual void          deleteEntry();
  virtual void          insertEntry();
  virtual void          updatePreview();

  private: 
    PunchClockStateBase* m_pcs;

};

#endif
