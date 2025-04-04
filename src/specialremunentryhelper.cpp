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

#include "specialremunentryhelper.h"
#include "unterkontoeintrag.h"
#include "abteilungsliste.h"

#include <QMessageBox>


void SpecialRemunEntryHelper::checkSREntry(AbteilungsListe *abtListToday, int idx, const QString& abt, const QString& ko , const QString& uko, const QSet<QString>& specialRemuns) {
  UnterKontoEintrag entry;
  abtListToday->getEintrag(entry,abt,ko,uko,idx);

  if (entry.getAchievedSpecialRemunSet()!=specialRemuns) {
     int correctidx=idx;
     EintragsListe::iterator itEt;
     EintragsListe* entrylist;
  
     if (abtListToday->findEntryWithSpecialRemunsAndComment(itEt, entrylist, correctidx, abt, ko, uko, entry.kommentar, specialRemuns)) {
        auto msgbox = new QMessageBox(
                  QMessageBox::Question, tr("sctime: wrong special remunerations"),
                  tr("There is another entry with the same comment and the correct special remunerations. Do you want to switch to this entry? "
                     "Otherwise a new entry will be created."),
                  QMessageBox::Yes|QMessageBox::No, dynamic_cast<QWidget*>(this->parent()));
        connect(msgbox, &QMessageBox::finished, [=](){
           if (msgbox->result()==QMessageBox::Yes) {
              emit checked(true,correctidx);
           } else {
              emit checked(false,0);
           }
           msgbox->deleteLater();
        });
        msgbox->open();
     } else {
        emit checked(false,0);
     }
  } else {
    emit checked (true, idx);
  }
}