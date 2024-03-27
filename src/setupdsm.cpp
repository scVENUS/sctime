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


#include "setupdsm.h"

#ifdef WIN32
# include <windows.h>
# include <lmcons.h> // UNLEN
#else
# include <unistd.h>
#endif
#include <QProcessEnvironment>
#ifndef RESTONLY
#include <QSqlDatabase>
#include <QSqlError>
#endif
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include "JSONReader.h"
#include "sctimexmlsettings.h"
#include "datasource.h"
#include "globals.h"


QString DSM::kontenQuery(
  "Select  "
  "   gb.name, " // 0
  "   team.kostenstelle, "
  "   konto.name,  "
  "   f_username(konto.verantwortlich), " // 3
  "   f_username(coalesce(konto.stellvertreter, konto.verantwortlich)), "
  "   konto.abgerechnet_bis, "
  "   konto.zeitlimit, "  // 6
  "   u.name, "
  "   f_username(coalesce(u.verantwortlich, konto.verantwortlich)), "
  "   f_username(coalesce(u.stellvertreter, u.verantwortlich, konto.verantwortlich)), " // 9
  "   coalesce(unterkonto_art.name || ' (' || u.art || ')', u.art), "
  "   coalesce(u.beschreibung, '') || coalesce('; noch nicht abgerechnet: ' || (get_budget_saldo(u.unterkonto_id)::numeric(8,2)), ''), "
  "   coalesce(u.intercompany_id, '(keine PSP)'), "
  "   (select coalesce(string_list(sz.kategorie),'') from t_sonderzeiten_unterkonto szu join t_sonderzeiten sz on (szu.id_sonderzeiten=sz.id) where szu.id_unterkonto=u.unterkonto_id), "
  "   coalesce(uk.kommentar, '') " // 15
  "From "
  "  gb "
  "  join konto on (gb.gb_id = konto.gb_id) "
  "  join team on (team.team_id = konto.team_id)  "
  "  join unterkonto u on (u.konto_id = konto.konto_id) "
  "  join unterkonto_art on (u.art = unterkonto_art.art) "
  "  left join unterkonto_kommentar uk on (u.unterkonto_id = uk.unterkonto_id) "
  "Where "
  " u.eintragbar "
  "Order By gb.name, konto.name, u.name, uk.kommentar ");

const QString DSM::bereitQuery("SELECT kategorie, beschreibung FROM v_bereitschaft_sctime");

const QString DSM::specialRemunQuery(
  "Select "
  "    sz.kategorie, "
  "    sz.beschreibung, "
  "    sz.isglobal "
  "From "
  "    v_sonderzeiten_sctime sz "
  "    order by sz.kategorie");

QString DSM::username() {
  static QString result;
  if (!result.isNull())
    return result;
#ifdef WIN32
    char winUserName[UNLEN + 1];
    DWORD winUserNameSize = sizeof(winUserName);
    if (GetUserNameA(winUserName, &winUserNameSize))
      result = QString::fromLocal8Bit(winUserName);
#else
    char *login = getlogin();
    if (login)
      result = QString::fromLocal8Bit(login);
#endif
    if (result.isEmpty()) {
      result = "";
      logError(QObject::tr("user name cannot be determined."));
    }
    return result;
}

QString DSM::password() {
  static QString result;
  if (!result.isNull())
    return result;
  result = username(); // the username is the default password
  // try to read password from a file
  QList<QString> pwdfilepaths;
  pwdfilepaths << absolutePath("~/.Zeit");
  QString p;
  foreach (p, pwdfilepaths) {
    QFile pwdfile(p);
    if (pwdfile.open(QIODevice::ReadOnly)) {
      QTextStream qs(&pwdfile);
      result = qs.readLine();
      break;
    } else
      logError(QObject::tr("Error when reading from file %1: %2").arg(p, pwdfile.errorString()));
  }
  return result;
}

void DSM::setup(SCTimeXMLSettings* settings, QNetworkAccessManager* networkAccessManager)
{
  //FIXME: memory leak?
  QList<Datasource*> *kontensources=new QList<Datasource*>();
  QList<Datasource*> *bereitsources=new QList<Datasource*>();;
  QList<Datasource*> *specialremunsources=new QList<Datasource*>();

  QStringList dataSourceNames;
  
  if (userDataSourceNames.isEmpty()) {
    dataSourceNames = settings->backends.split(" "); 
  } else {
    dataSourceNames = userDataSourceNames;
  }

  JSONReaderBase *jsonreader=NULL;
#ifndef RESTONLY
  trace(QObject::tr("available database drivers: %1.").arg(QSqlDatabase::drivers().join(", ")));
  if (!kontenPath.isEmpty())
    kontensources->append(new FileReader(kontenPath, "|", 15));
  if (!bereitPath.isEmpty())
    bereitsources->append(new FileReader(bereitPath, "|", 2));
  if (!specialremunPath.isEmpty())
    specialremunsources->append(new FileReader(specialremunPath, "|", 2));
  if (!jsonPath.isEmpty()) {
    trace(QObject::tr("adding jsonreader: %1.").arg(jsonPath));
    jsonreader=new JSONReaderUrl(networkAccessManager, "file://"+jsonPath);
    kontensources->append(new JSONAccountSource(jsonreader));
    jsonreader=new JSONReaderUrl(networkAccessManager, "file://"+jsonPath);
    bereitsources->append(new JSONOnCallSource(jsonreader));
    jsonreader=new JSONReaderUrl(networkAccessManager, "file://"+jsonPath);
    specialremunsources->append(new JSONSpecialRemunSource(jsonreader));
  }
  QString dsname;
  foreach (dsname, dataSourceNames) {
    if (dsname.compare("json") == 0) {
      jsonreader=new JSONReaderUrl(networkAccessManager, "file://"+configDir.filePath("sctime-offline.json"));
      kontensources->append(new JSONAccountSource(jsonreader));
     jsonreader=new JSONReaderUrl(networkAccessManager, "file://"+configDir.filePath("sctime-offline.json"));
      bereitsources->append(new JSONOnCallSource(jsonreader));
      jsonreader=new JSONReaderUrl(networkAccessManager, "file://"+configDir.filePath("sctime-offline.json"));
      specialremunsources->append(new JSONSpecialRemunSource(jsonreader));
    } else
    if (dsname.compare("file") == 0) {
      kontensources->append(new FileReader(configDir.filePath("zeitkonten.txt"), "|", 15));
      bereitsources->append(new FileReader(configDir.filePath("zeitbereitls.txt"), "|", 2));
      specialremunsources->append(new FileReader(configDir.filePath("sonderzeitls.txt"), "|", 3));
    } 
    else if (dsname.compare("command") == 0) {
#ifdef WIN32
      logError(QObject::tr("data source 'command' is not available on Windows"));
#else
#ifdef DEPRECATED_CMDS
      kontensources.append(new CommandReader("zeitkonten --mikrokonten --psp --sonderzeiten --separator='|'", "|", 15));
      bereitsources.append(new CommandReader("zeitbereitls --separator='|'", "|", 2));
      specialRemunsources.append(new CommandReader("sonderzeitls --separator='|'", "|", 3));
#else
      jsonreader=new JSONReaderCommand("zeit-sctime-offline", NULL);
      kontensources->append(new JSONAccountSource(jsonreader));
      jsonreader=new JSONReaderCommand("zeit-sctime-offline", NULL);
      bereitsources->append(new JSONOnCallSource(jsonreader));
      jsonreader=new JSONReaderCommand("zeit-sctime-offline", NULL);
      specialremunsources->append(new JSONSpecialRemunSource(jsonreader));
#endif
#endif
    } 
    else {
      if (!QSqlDatabase::drivers().contains(dsname)) {
        logError(QObject::tr("database driver or data source not available: ") + dsname);
        continue;
      }
      QSqlDatabase db = QSqlDatabase::addDatabase(dsname, dsname);
      if (!db.isValid() || db.isOpenError()) {
        logError(QObject::tr("data source '%1' not working: %2").arg(dsname, db.lastError().driverText()));
        continue;
      }
      db.setDatabaseName(
	  dsname.startsWith("QODBC") 
	    ? "DSN=Postgres_Zeit;DRIVER=PostgreSQL UNICODE" 
	    : settings->database);
      db.setHostName(settings->databaseserver);

      QString un = settings->databaseuser;
      if (un.isEmpty())
	un = username();
      db.setUserName(un);

      QString pw = settings->databasepassword;
      if (pw.isEmpty())
	pw = password();
      db.setPassword(pw);

      kontensources->append(new SqlReader(db, kontenQuery));
      bereitsources->append(new SqlReader(db, bereitQuery));
      specialremunsources->append(new SqlReader(db, specialRemunQuery));
    }
  }
  if (dsname.compare("rest") == 0) {
#endif // RESTONLY
    QString baseurl=getRestBaseUrl();
    jsonreader=new JSONReaderUrl(networkAccessManager, baseurl+"/"+REST_ACCOUNTINGMETA_ENDPOINT);
    kontensources->append(new JSONAccountSource(jsonreader));
    jsonreader=new JSONReaderUrl(networkAccessManager, baseurl+"/"+REST_ACCOUNTINGMETA_ENDPOINT);
    bereitsources->append(new JSONOnCallSource(jsonreader));
    jsonreader=new JSONReaderUrl(networkAccessManager, baseurl+"/"+REST_ACCOUNTINGMETA_ENDPOINT);
    specialremunsources->append(new JSONSpecialRemunSource(jsonreader));
#ifndef RESTONLY
  }
#endif
  kontenDSM = new DatasourceManager(QObject::tr("Accounts"), kontensources);
  bereitDSM = new DatasourceManager(QObject::tr("On-call categories"), bereitsources);
  specialRemunDSM = new DatasourceManager(QObject::tr("Special Remunerations"), specialremunsources);
}
