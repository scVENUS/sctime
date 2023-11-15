/*
    Copyright (C) 2018 science+computing ag
       Authors: Johannes Abt et al.

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

#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QString>
#include <QList>
#include <QObject>
#ifndef RESTONLY
#include <QSqlDatabase>
#endif
class QStringList;

typedef QList<QStringList> DSResult;

/* represents a way of reading data (a list of a list of strings); the base class of all data sources. */
class Datasource: public QObject
{
  Q_OBJECT
public:
  virtual ~Datasource() {};
  virtual QString toString()=0;
public slots:
  virtual void start()=0;
signals:
  void finished(const DSResult& data);
  void failed();
protected:
  /* the datasource detected a permanent error */
  bool broken;
  Datasource(QObject* parent=NULL): QObject(parent) {broken=false;};
};

class DatasourceManager:public QObject
{
  Q_OBJECT
public:
  /* name: identifier for message */
  DatasourceManager(const QString& name, QList<Datasource*>* sources);
  const QString name;
  virtual ~DatasourceManager();
public slots:
  virtual void start();
  virtual void dsfinished(const DSResult& data);
  virtual void lastdsnoresult();
signals:
  /* successfully finished reading */
  void finished(const DSResult& data);
  /* no datasource provided data */
  void aborted();
private:
  int lastidx;
  QList<Datasource*> *sources;
};

#ifndef RESTONLY

class FileReader : public Datasource
{
  Q_OBJECT
public:
  FileReader(const QString &path, const QString& columnSeparator, int columns);
  virtual ~FileReader() {};
  const QString path, sep;
  const int columns;
  virtual QString toString() {return "FileReader_"+path;};
public slots:
  virtual void start();
private:
  bool read(DSResult* const result);
  };

class SqlReader : public Datasource
{
  Q_OBJECT
public:
  SqlReader(QSqlDatabase db, const QString &cmd);
  virtual ~SqlReader() {};
  const QString cmd;
  QSqlDatabase db;
  virtual QString toString() {return "SqlReader_"+cmd;};
public slots:
  virtual void start();
private:
  bool read(DSResult* const result);
};


#ifndef WIN32
class CommandReader : public Datasource
{
  Q_OBJECT
public:
  CommandReader(const QString &command, const QString& columnSeparator, int columns);
  virtual ~CommandReader() {};
  const QString command, sep;
  const int columns;
  virtual QString toString() {return "CommandReader_"+command;};
public slots:
  virtual void start();
private:
  bool read(DSResult* const result);
};
#endif
#endif

#endif // DATASOURCE_H
