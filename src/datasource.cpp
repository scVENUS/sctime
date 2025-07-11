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

#include "datasource.h"

#include <QTextStream>
#include <QStringList>
#include <QStringConverter>
#include <QFile>
#ifndef RESTONLY
#ifdef WIN32
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#endif
#endif
#include <QVariant>
#include "globals.h"

static bool readFile(DSResult* const result, QTextStream &ts, const QString &sep, int columns, const QString &path);

DatasourceManager::DatasourceManager(const QString& name, QList<Datasource*>* sources):name(name) {
   lastidx=0;
   this->sources=sources;
   if (sources->length()==0) {
           return;
   }
   QObject::connect((*sources)[0], &Datasource::finished, this, &DatasourceManager::dsfinished);
   for (int i=1; i<sources->length(); i++) {
      QObject::connect((*sources)[i], &Datasource::finished, this, &DatasourceManager::dsfinished);
      QObject::connect((*sources)[i-1], &Datasource::failed, (*sources)[i], &Datasource::start);
   }
   QObject::connect((*sources)[sources->length()-1], &Datasource::failed, this, &DatasourceManager::lastdsnoresult);
}

DatasourceManager::~DatasourceManager() {
  Datasource *ds;
  foreach (ds, *sources) delete ds;
  delete sources;
}

void DatasourceManager::start() {
  Datasource *ds;
  if (sources->length()==0) {
    logError(QObject::tr("%1: no data source available").arg(name));
    emit aborted();
    return;
  }
  ds=(*sources)[0];
  ds->start();
}

void DatasourceManager::dsfinished(const DSResult& data) {
  emit finished(data);
}

void DatasourceManager::lastdsnoresult() {
  logError(QObject::tr("%1: no data source available").arg(name));
  emit aborted();
}


#ifndef RESTONLY

FileReader::FileReader(const QString &path, const QString&  columnSeparator, int columns)
  :Datasource(), path(path), sep(columnSeparator), columns(columns) {}

bool FileReader::read(DSResult* const result) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) {
    logError(QObject::tr("file '%1' cannot be opened: %2").arg(path, file.errorString()));
    broken = true;
    return false;
  }
  trace(QObject::tr("Reading ") + path);
  QTextStream ts(&file);
  return readFile(result, ts, sep, columns, path);
}

void FileReader::start() {
   DSResult result;
   if (read(&result)) {
      emit finished(result);
   } else {
      emit failed();
   }
}

static bool readFile(DSResult* const result, QTextStream &ts, const QString& sep, int columns, const QString &path) {
  int lines = 0;
  while (!ts.atEnd()) {
    QString l = ts.readLine();
    lines++;
    if (l.isEmpty()) continue;
    QStringList vl;
    int start = 0;
    // Der Separator soll in der letzten Spalte als einfaches Zeichen behandelt werden soll.
    // Dafuer gibt es keine Methode in QString. Deswegen hier per Hand:
    for (int i = 0; i < columns - 1; i++) {
      int end = l.indexOf(sep, start);
      if (end == -1) {
        logError(QObject::tr("Line %1 of '%2' has only %3 columns instead of %4").arg(lines).arg(path).arg(i + 1).arg(columns));
        return false;
      }
      vl << l.mid(start, end - start);
      start = end + sep.length();
    }
    vl << l.mid(start); // das letzte Element enthaelt den ganzen Rest
    result->append(vl);
  }
  return true;
}

#ifdef WIN32
SqlReader::SqlReader(QSqlDatabase db, const QString &cmd):cmd(cmd),db(db) {}

bool  SqlReader::read(DSResult* const result) {
  logError(QObject::tr("Connecting to database %1 on %2 with driver %3 as user %4")
           .arg(db.databaseName(), db.hostName(), db.driverName(), db.userName()));
  if (!db.open()) {
    logError(QObject::tr("connection failed: ") + db.lastError().databaseText());
    return false;
  }
  QSqlQuery query(cmd, db);
  if (!query.isActive()) {
    logError(QObject::tr("Error ('%1') when executing query: %2").arg(db.lastError().databaseText()).arg(cmd));
    broken = true;
    db.close();
    return false;
  }
  int cols = query.record().count();
  while (query.next()) {
    QStringList row;
    for (int i = 0; i < cols; i++)
      row.append(query.value(i).toString());
    result->append(row);
  }
  db.close();
  return true;
}

void SqlReader::start() {
   DSResult result;
   if (read(&result)) {
      emit finished(result);
   } else {
      emit failed();
   }
}
#endif

#ifndef WIN32
#include <stdlib.h>
#include <errno.h>
#include <langinfo.h>
CommandReader::CommandReader(const QString &command, const QString& columnSeparator, int columns)
  :Datasource(), command(command), sep(columnSeparator), columns(columns) {}

bool CommandReader::read(DSResult* const result) {
  FILE *file = popen(command.toLocal8Bit(), "r");
  if (!file) {
    logError(QObject::tr("Cannot run command '%1': %s2").arg(command, strerror(errno)));
    broken = true;
    return false;
  }
  trace(QObject::tr("Running command: ") + command);
  QTextStream ts(file, QIODevice::ReadOnly);
  QString codeset = QString(nl_langinfo(CODESET)).toUpper();
  // FIXME: this heuristics might go wrong in some special cases (for example euro sign and iso-8859-15 encoding), but there does 
  // not seem to be a better way using QT that works with all QT6 versions
  // the best way to fix encoding issues would probably be to have the command always output utf8, which would reduce complexity here
  if ((codeset!="UTF-8") || (codeset!="UTF8")) {
    ts.setEncoding(QStringConverter::Latin1);
  }
  bool ok = readFile(result, ts, sep, columns, command);
  int rc = pclose(file);
  if (rc == -1 || !WIFEXITED(rc) || WEXITSTATUS(rc)) {
    logError(QObject::tr("Error when executing command '%1': %2").arg(command).arg(rc == -1 ? strerror(errno) : QObject::tr("abnormal termination")));
    broken = true;
    return false;
  }
  return ok;
}

void CommandReader::start() {
   DSResult result;
   if (read(&result)) {
      emit finished(result);
   } else {
      emit failed();
   }
}

#endif // RESTONLY

#endif
