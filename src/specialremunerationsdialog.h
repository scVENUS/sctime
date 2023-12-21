/*
    Copyright (C) 2018 science+computing ag
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

#ifndef SPECIALREMUNERATIONSDIALOG_H
#define SPECIALREMUNERATIONSDIALOG_H

#include "ui_specialremunerationdialogbase.h"
#include "unterkontoliste.h"
#include "eintragsliste.h"
class QDialog;
class AbteilungsListe;

 /**
  * Der Dialog zum Eingeben eines Datums
  */
class SpecialRemunerationsDialog : public QDialog, private Ui::SpecialRemunerationDialogBase
{
  Q_OBJECT
private:
  QString abteilung,konto,unterkonto;
  int eintragsIdx;
  UnterKontoListe::iterator ukiter;
  
public:
  SpecialRemunerationsDialog(AbteilungsListe* _abtlist, const QString& abt, const QString& ko, const QString& uko, int idx, QWidget* parent = 0);
  ~SpecialRemunerationsDialog();
  /*$PUBLIC_FUNCTIONS$*/

  static void fillListWidget(QListWidget* widget, AbteilungsListe* abtlist, EintragsListe* etl, int eintragsIdx, const QString& abteilung, const QString& konto,const QString& unterkonto);
  
protected:
  AbteilungsListe* abtlist;

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  virtual void          accept();
  virtual void postprocess();

signals:
  // signals
  void specialRemunerationsChanged(const QString&, const QString&, const QString&, int idx);
  void finishedWithInfo(QString abt, QString ko, QString uko);
};

#endif
