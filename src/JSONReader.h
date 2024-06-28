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

#ifndef JSONREADER_H
#define JSONREADER_H

#include <QString>

#include <QJsonDocument>
#include <QNetworkReply>
#include "datasource.h"

class JSONReaderBase: public QObject
{
  Q_OBJECT
public:
  const static int INVALIDDATA=-1;
  virtual ~JSONReaderBase() {};
  virtual void loadDataNewerThan(int version);
  virtual void requestData()=0;
  virtual QJsonDocument& getData();
  virtual void processByteArray(QByteArray byteData);
signals:
  void aborted();
  void finished();
private:
  QJsonDocument data;
  int currentversion;
protected:
  JSONReaderBase();
};

class JSONReaderUrl: public JSONReaderBase
{
Q_OBJECT
public:
  JSONReaderUrl(QNetworkAccessManager* networkAccessManager, const QString& uri);
  virtual void requestData();
  virtual ~JSONReaderUrl() {};
public slots:
  virtual void receiveData(QNetworkReply *reply);
  virtual void gotReply();
private:
  const QString uri;
  QNetworkAccessManager* networkAccessManager;
};

#ifndef RESTONLY
class JSONReaderCommand: public JSONReaderBase
{
public:
  const static int INVALIDDATA=-1;
  JSONReaderCommand(const QString& command, QObject* parent);
  virtual void requestData();
  virtual ~JSONReaderCommand() {};
private:
  const QString command;
  QObject* parent;
};
#endif //RESTONLY

class JSONSource: public Datasource
{
  Q_OBJECT;
public:
  JSONSource(JSONReaderBase *jsonreader);
  virtual ~JSONSource() {};
  virtual QString toString() {return "JSONSource";};
public slots:
  virtual void start();
  virtual void jsonreceived();
  virtual void jsonfailed();
private: 
  int currentversion;
protected:
  JSONReaderBase* jsonreader;
  void appendStringToRow(QStringList& row, const QJsonObject& object, const QString& field, const QString& defaultValue="");
  virtual bool convertData(DSResult* const result)=0;
};

class JSONAccountSource: public JSONSource
{
public:
  JSONAccountSource(JSONReaderBase *jsonreader);
  virtual ~JSONAccountSource() {};
protected:
  virtual bool convertData(DSResult* const result);
};

class JSONSpecialRemunSource: public JSONSource
{
public:
  JSONSpecialRemunSource(JSONReaderBase *jsonreader);
  virtual ~JSONSpecialRemunSource() {};
protected:
  virtual bool convertData(DSResult* const result);
};

class JSONOnCallSource: public JSONSource
{
public:
  JSONOnCallSource(JSONReaderBase *jsonreader);
  virtual ~JSONOnCallSource() {};
protected:
  virtual bool convertData(DSResult* const result);
};

#endif // JSONREADER_H
