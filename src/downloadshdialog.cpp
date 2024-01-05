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

#include "downloadshdialog.h"
#include "sctimexmlsettings.h"
#include "xmlreader.h"
#include <private/qzipwriter_p.h>
#include <QBuffer>
#include <QFileDialog>

DownloadSHDialog::DownloadSHDialog(SCTimeXMLSettings *settings, QWidget* parent): QDialog(parent), settings(settings) {
    setupUi(this);
    zipBuffer=NULL;
    zipWriter=NULL;
    startDateEdit->setDate(QDate::currentDate().addDays(-1));
    endDateEdit->setDate(QDate::currentDate().addDays(-1));
    connect(this,&DownloadSHDialog::finished, this, &DownloadSHDialog::saveZip);
}

DownloadSHDialog::~DownloadSHDialog()
{
}

void DownloadSHDialog::saveZip()
{
  startDate = startDateEdit->date();
  endDate = endDateEdit->date();
  zipBuffer=new QBuffer(&zipData);
  zipWriter=new QZipWriter(zipBuffer);
  currentDate=startDate.addDays(-1);
  addFile();
}

void DownloadSHDialog::addFile() {
  // Todo: we also come here if the user has canceled
  currentDate = currentDate.addDays(1);
  if (startDate!=currentDate) {
    QByteArray filecontent;
    QTextStream stream(&filecontent);
    settings->writeShellSkriptToStream(stream, abtList, pcl);
    zipWriter->addFile("zeit-"+abtList->getDatum().toString("yyyy-MM-dd")+".sh", filecontent);
  }
  if (currentDate <= endDate){
    // todo delete later
    abtList=new AbteilungsListe(currentDate, (KontoDatenInfo*)NULL);
    pcl=new PunchClockList();
    XMLReader* reader = new XMLReader(settings,false,abtList,pcl);
    connect(reader,&XMLReader::settingsRead,this,&DownloadSHDialog::addFile, Qt::QueuedConnection);
    connect(reader,&XMLReader::settingsRead,reader,&XMLReader::deleteLater);
    reader->open();
  } else {
    zipWriter->close();
    QFileDialog::saveFileContent(zipData, "sctimesave.zip");
    emit workflowFinished();
  }
}
