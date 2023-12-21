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

#include "datasource.h"

#include "JSONReader.h"
#include "globals.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QTextStream>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QThread>
#include <QEventLoop>
#include <QApplication>


JSONSource::JSONSource(JSONReaderBase *jsonreader)
  :Datasource(), currentversion(JSONReaderBase::INVALIDDATA), jsonreader(jsonreader) {
          connect(this->jsonreader, SIGNAL(aborted()), this, SLOT(jsonfailed()));
          connect(this->jsonreader, SIGNAL(finished()), this, SLOT(jsonreceived()));
  }

void JSONSource::appendStringToRow(QStringList& row, const QJsonObject& object, const QString& field) {
  row.append(object[field].toString());
}

void JSONSource::start() {
  if (broken) {
     emit failed();
     return;
  }
  jsonreader->loadDataNewerThan(currentversion);
}

void JSONSource::jsonreceived() {
   DSResult result;
   if (convertData(&result)) {
      emit finished(result);
   } else {
      emit failed();
   }
}
  void JSONSource::jsonfailed() {
     emit failed();
  }


/*
 * creates a new JSONSource-Object. JSONSource reads from aJSONReader Object and creates a flat table,
 * with the following columns:
 * tbd
 * 
 * this is neither efficient nor very elegant, but it fits into the existing mechanism. Perhaps it will
 * be refactored one day.
 * 
 */
JSONAccountSource::JSONAccountSource(JSONReaderBase *jsonreader)
  :JSONSource(jsonreader) {}
  
bool JSONAccountSource::convertData(DSResult* const result) {
  QJsonDocument doc=jsonreader->getData();
  QJsonObject data=doc.object()["AccountTree"].toObject();
  QJsonArray departments=data["Departments"].toArray();
  foreach (auto departmentVal, departments) {
    QJsonObject department=departmentVal.toObject();
    QJsonArray accounts=department["Accounts"].toArray();
    foreach (auto accountVal, accounts) {
      QJsonObject account=accountVal.toObject();
      QString respaccount1="";
      QString respaccount2="";
      QJsonArray resppers=account["ResponsiblePersons"].toArray();
      if (!resppers.isEmpty()) {
        respaccount1=resppers.takeAt(0).toString();
        if (!resppers.isEmpty()) {
          respaccount2=resppers.takeAt(0).toString();
        }
      }
      QJsonArray subaccounts=account["SubAccounts"].toArray();
      foreach (auto subaccountVal, subaccounts) {
        QJsonObject subaccount=subaccountVal.toObject();
        QStringList row;
        appendStringToRow(row,department,"Name");
        appendStringToRow(row,account,"CostCenter");
        appendStringToRow(row,account,"Name");
        row.append(respaccount1);
        row.append(respaccount2);
        appendStringToRow(row,account,"InvoicedUntil");
        appendStringToRow(row,account,"NoEntriesBefore");
        appendStringToRow(row,subaccount,"Name");
        resppers=subaccount["ResponsiblePersons"].toArray();
        if (!resppers.isEmpty()) {
          row.append(resppers.takeAt(0).toString());
          if (!resppers.isEmpty()) {
            row.append(resppers.takeAt(0).toString());
          }
          else {
            row.append("");
          }
        }
        else {
          row.append("");
	  row.append("");
        }
        appendStringToRow(row,subaccount,"Category");
        appendStringToRow(row,subaccount,"Description");
        appendStringToRow(row,subaccount,"PSP");
        QJsonArray specialremuns=subaccount["SpecialRemunerations"].toArray();
        QStringList srlist;
        foreach (auto specialremunVal, specialremuns) {
          srlist.append(specialremunVal.toString());
        }
        row.append(srlist.join(","));
        QJsonArray microaccounts=subaccount["MicroAccounts"].toArray();
        row.append("");
        if (microaccounts.size()>0) {
          foreach (auto microaccountVal, microaccounts) {
            QString microaccount=microaccountVal.toString();
            row[row.size()-1]=microaccount; // we need a row for each microaccount, so replace it on the row
            result->append(row);            // and insert the new row to the result
          }
        } else {
            result->append(row); // if there are no microaccounts insert row anyway.
        }      
      }
    }
  }
  return true;
}

JSONReaderBase::JSONReaderBase()
  : currentversion(INVALIDDATA) {
  }
  
QJsonDocument &JSONReaderBase::getData()
{
  return data;
}



void JSONReaderBase::loadDataNewerThan(int version)
{
  currentversion=version+1;
  /*QByteArray byteData;
  try {
     byteData = getByteArray();
  } catch (JSONReaderException *e) {
    return INVALIDDATA;
  }
  */
  requestData();
}

void JSONReaderBase::processByteArray(QByteArray byteData) {
   data=QJsonDocument::fromJson(byteData);
}

JSONOnCallSource::JSONOnCallSource(JSONReaderBase *jsonreader)
  :JSONSource(jsonreader) {}
  
bool JSONOnCallSource::convertData(DSResult* const result) {
  QJsonDocument doc=jsonreader->getData();
  QJsonArray oncalltimes=doc.object()["OnCallTimes"].toArray();
  foreach (auto oncalltimeVal, oncalltimes) {
    QJsonObject oncalltime=oncalltimeVal.toObject();
    QStringList row;
    appendStringToRow(row,oncalltime,"Category");
    appendStringToRow(row,oncalltime,"Description");
    result->append(row);
  }
  return true;
}

JSONSpecialRemunSource::JSONSpecialRemunSource(JSONReaderBase *jsonreader)
  :JSONSource(jsonreader) {}
  
bool JSONSpecialRemunSource::convertData(DSResult* const result) {
  QJsonDocument doc=jsonreader->getData();
  QJsonArray specialremuns=doc.object()["SpecialRemunerations"].toArray();
  foreach (auto specialremunVal, specialremuns) {
    QJsonObject specialremun=specialremunVal.toObject();
    QStringList row;
    appendStringToRow(row,specialremun,"Category");
    appendStringToRow(row,specialremun,"Description");
    if (specialremun["IsGlobal"].isString()) {
      row.append(specialremun["IsGlobal"].toString());
    } else {
      if (specialremun["IsGlobal"].toInt()) {
        row.append("1");
      } else {
        row.append("0");
      }
    }
    result->append(row);
  }
  return true;
}

void JSONReaderUrl::requestData()
{
  auto request = QNetworkRequest(QUrl(uri));
  QNetworkReply *reply = networkAccessManager.get(request);
  connect(reply, &QNetworkReply::finished, this, &JSONReaderUrl::gotReply);
  // for compatibility - use errorOccurred slot instead in future
  connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
        this, &JSONReaderUrl::onErrCompat);
}

void JSONReaderUrl::receiveData(QNetworkReply *reply)
{
  reply->deleteLater();
  auto err=reply->error();
  if (err!=QNetworkReply::NoError) {
        trace(tr("Couldn't open json from uri %1.").arg(uri));
        emit aborted();
        return;
  }
  QByteArray byteData = reply->readAll();
  processByteArray(byteData);
  emit finished();
}

JSONReaderUrl::JSONReaderUrl(const QString& _uri): JSONReaderBase(), uri(_uri) {
        
};

// we need this for compatibility with old QT.
void JSONReaderUrl::onErrCompat(QNetworkReply::NetworkError code) {
    auto obj=sender();
    receiveData((QNetworkReply*)obj);
}

void JSONReaderUrl::gotReply() {
    auto obj=sender();
    receiveData((QNetworkReply*)obj);
}

#ifndef RESTONLY
void JSONReaderCommand::requestData()
{
  QProcess* process = new QProcess(parent);
  process->start(command,QIODevice::ReadOnly);
  trace(QObject::tr("Running command: ") + command);
  if (!process->waitForFinished(-1)) {
    logError(QObject::tr("Cannot run command '%1': %2").arg(command).arg(process->error()));
    delete process;
    emit aborted();
    return;
  }
  if (process->exitCode()) {
    logError(QObject::tr("Command '%1' has non-zero exitcode: %s2").arg(command, process->exitCode()));
    delete process;
    emit aborted();
    return;
  }
  QByteArray byteData = process->readAllStandardOutput();
  delete process;

  processByteArray(byteData);
  emit finished();
}

JSONReaderCommand::JSONReaderCommand(const QString& _command, QObject* _parent): JSONReaderBase(), command(_command), parent(_parent) {};
#endif
