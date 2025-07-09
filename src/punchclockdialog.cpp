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

#include "punchclockdialog.h"

#include <QPushButton>
#include <QDir>
#include <QObject>
#include <QStringList>
#include <QTextCharFormat>
#include <QDateTimeEdit>
#include "globals.h"
#include "punchclockchecker.h"

PunchClockDialog::PunchClockDialog(PunchClockList *pcl, PunchClockStateBase *pcs, QWidget *parent)
: QDialog(parent)
{
  m_pcs=pcs;
  setupUi(this);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(insertButton, SIGNAL(clicked()), this, SLOT(insertEntry()));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteEntry()));
  if (pcl!=NULL) {
    fillFromList(pcl);
  }
  punchClockTable->setHorizontalHeaderLabels(QStringList()<<QObject::tr("Begin time")<<QObject::tr("End time")<<"");
  
  if (m_pcs==NULL) {
    previewBrowser->hide();
  }
  else {
    QString consolidated=m_pcs->getConsolidatedIntervalString(pcl);
    previewBrowser->setText(consolidated);
  }
  punchClockTable->setSelectionMode(QAbstractItemView::SingleSelection);
  punchClockTable->setSelectionBehavior(QAbstractItemView::SelectRows);
}

PunchClockDialog::~PunchClockDialog()
{
}

void PunchClockDialog::fillFromList(PunchClockList *pcl) {
  punchClockTable->clear();
  int row=0;
  for (auto entry = pcl->begin(); entry != pcl->end(); ++entry) {
    insertEntry(row, QTime::fromMSecsSinceStartOfDay(entry->first*1000), QTime::fromMSecsSinceStartOfDay(entry->second*1000));
    auto label=(QLabel*)punchClockTable->cellWidget(row,2);
    if (entry==pcl->currentEntry()) {
      punchClockTable->cellWidget(row,1)->setEnabled(false);
      label->setText(QObject::tr("Current"));
    } 
    row++;
  }
}

void PunchClockDialog::copyToList(PunchClockList *pcl) {
  pcl->clear();
  for (int row=0; row<punchClockTable->rowCount(); row++) {
    auto beginCell=(QDateTimeEdit*)punchClockTable->cellWidget(row,0);
    auto endCell=(QDateTimeEdit*)punchClockTable->cellWidget(row,1);
    int begin=(int)beginCell->time().msecsSinceStartOfDay()/1000;
    int end=(int)endCell->time().msecsSinceStartOfDay()/1000;
    if (begin>end) {
      // heuristics, if someone probably meant to end at 23:59, 0:00 would already be the next day
      if ((begin>12*60*60)&&(end<=60)) {
        end=23*60*60+59*60;
      } else {
        int t=begin;
        begin=end;
        end=t;
      }
    }
    bool active=!endCell->isEnabled();
    pcl->push_back(PunchClockEntry(begin, end));
    if (active) {
       pcl->setCurrentEntry(std::prev(pcl->end()));
    }
  }
}

void PunchClockDialog::deleteEntry()
{
  punchClockTable->removeRow(punchClockTable->currentRow());
  updatePreview();
}

void PunchClockDialog::insertEntry()
{
  int row=punchClockTable->currentRow()+1;
  insertEntry(row, QTime::fromString("0:0", "H:m"), QTime::fromString("0:0", "H:m"));
  updatePreview();
}

void PunchClockDialog::insertEntry(int row, QTime begin, QTime end)
{
  punchClockTable->insertRow(row);
  auto dateTime = new QDateTimeEdit(this);
  dateTime->setDisplayFormat("H:mm");
  dateTime->setFrame(false);
  dateTime->setTime(begin);
  punchClockTable->setCellWidget(row, 0, dateTime);
  connect(dateTime, SIGNAL(timeChanged(const QTime&)), this, SLOT(updatePreview()));
  dateTime = new QDateTimeEdit(this);
  dateTime->setDisplayFormat("H:mm");
  dateTime->setFrame(false);
  dateTime->setTime(end);
  punchClockTable->setCellWidget(row, 1, dateTime);
  connect(dateTime, SIGNAL(timeChanged(const QTime&)), this, SLOT(updatePreview()));
  auto label =new QLabel(this);
  punchClockTable->setCellWidget(row, 2, label);
}

void PunchClockDialog::accept()
{
  QDialog::accept();
}

void PunchClockDialog::updatePreview()
{
  if (m_pcs!=NULL) {
    PunchClockList pcl;
    copyToList(&pcl);
    QString consolidated = m_pcs->getConsolidatedIntervalString(&pcl);
    previewBrowser->setText(consolidated);
  }
}