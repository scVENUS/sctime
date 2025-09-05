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

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QString>
#include <QTranslator>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QLocalSocket>
#include <QUuid>

#ifdef __EMSCRIPTEN__
#include <emscripten/val.h>
#include <emscripten.h>
#endif

#ifndef WIN32
#include <assert.h>
#include <locale.h>
#endif

#include "lock.h"
#include "timemainwindow.h"
#include "globals.h"
#include "kontotreeview.h"
#include "datasource.h"
#include "sctimeapp.h"

#ifndef CONFIGDIR
#ifndef __EMSCRIPTEN__
#define CONFIGDIR "~/.sctime"
#else
#define CONFIGDIR "/sctime"
#endif
#endif

#ifdef __GNUC__
static void fatal(const QString& title, const QString& body) __attribute__ ((noreturn));
#else
static void fatal(const QString& title, const QString& body);
#endif

QDir configDir;
QString lockfilePath;
QString PERSOENLICHE_KONTEN_STRING;
QString ALLE_KONTEN_STRING;
QString SCTIME_IPC;
QString clientId;
QString clientinfo;

static void fatal(const QString& title, const QString& body) {
  QMessageBox *msgbox=new QMessageBox(QMessageBox::Critical, title, body, QMessageBox::Ok);
  QObject::connect(msgbox, &QMessageBox::finished, [=]()
  {
    msgbox->deleteLater();
    exit(1);
  });
  // with exec we start an event loop, because we do not have one during starup
  msgbox->exec();
}

static const QString help(QObject::tr(
"Available Options:\n"
"--configdir=DIR		location of the directory where your files will be placed\n"
"			(default: ~/.sctime)\n\n"
"--datasource=		(repeatable) use these data sources\n"
"			(default: 'QPSQL'',' 'QODBC'', 'command' and 'file'');\n"
"			overrides <backends/> in settings.xml\n\n"
"--zeitkontenfile=FILE	read the accounts list from file FILE\n"
"			(default: output of 'zeitkonten'). Obsolete.\n\n"
"--bereitschaftsfile=FILE	read the 'Bereitschaftsarten'' from file FILE\n"
"			(default: output of 'zeitbereitls'). Obsolete.\n\n"
"--specialremunfile=FILE       read the types of special remunerations from file FILE\n"
"			(default: output of 'sonderzeitls'. Obsolete.\n\n"
"--offlinefile=FILE		read all needed data from FILE which must be of json format\n"
"			overides --zeitkontenfile --bereitschaftsfile and --specialremunfile\n\n"
"--accountlink=URL		opens sctime with the given account selected. If an instance of sctime\n"
"\t\t\tis already running, the existing instance will be used for that.\n\n"
"Please see the Help menu for further information (F1)!"));

QString absolutePath(QString path) {
    if (path == "~" || path.startsWith("~/") || path.startsWith(QString("~") +
		QDir::separator())) {
#ifdef WIN32
      
      QString homedir = getenv("SCTIME_HOMEDRIVE");
	    if (homedir.isNull()) {
          homedir = getenv("HOMEDRIVE");
      }
	    // append a separator if only the homedir is requested so that the
	    // drive's root is addressed reliably
	    if (path == "~")
		    homedir.append(QDir::separator());
#else
	    QString homedir = QDir::homePath();
#endif /* WIN32 */
	    return path.replace(0, 1, homedir);
    }
    return QFileInfo(path).absoluteFilePath();
}

QString getMachineIdentifier() {
#ifdef __EMSCRIPTEN__ // we might have different browser on the same machine
  static QString machineId;
  QFile machineIdFile(configDir.absoluteFilePath("machineid.txt"));
  if (machineId.isEmpty())
  {
    if (!machineIdFile.exists())
    {
      machineId = QUuid::createUuid().toString(QUuid::WithoutBraces);
      machineIdFile.open(QIODevice::WriteOnly);
      machineIdFile.write(machineId.toUtf8());
      machineIdFile.close();
    }
    else
    {
      machineIdFile.open(QIODevice::ReadOnly);
      machineId = QString::fromUtf8(machineIdFile.readAll());
      machineIdFile.close();
    }
  }
  return QSysInfo::machineHostName()+ "-" + machineId;
#else
  return QSysInfo::machineHostName();
#endif
}

/** tries to open a link in an existing instance
 */
bool openLinkInExistingInstance(QString accountlink) {
  QLocalSocket ls;
  ls.connectToServer(SCTIME_IPC, QIODevice::WriteOnly);
  if (!ls.waitForConnected(3000))
  {
     return false; // existing instance could not be reached
  }
  ls.write(("{\"type\":\"accountlink\", \"link\":\""+accountlink+"\"}").toUtf8());
  if (!ls.waitForBytesWritten(5000))
  {
     fatal(QObject::tr("Error on connecting to sctime"), ls.errorString().toUtf8());
  }
  ls.disconnectFromServer();
  return true;
}

void stopAppHard() {
  qApp->exit();
  #ifdef __EMSCRIPTEN__
      emscripten_run_script(R"(
          // Clear all timers
          var highestTimeoutId = setTimeout(";", 0);
          for (var i = 0; i < highestTimeoutId; i++) {
              clearTimeout(i);
          }
          var highestIntervalId = setInterval(";", 9999);
          for (var i = 0; i < highestIntervalId; i++) {
              clearInterval(i);
          }
          // Set custom content
          document.documentElement.innerHTML = "<html><head><title>sctime - session closed</title></head><body><h1>Session Closed</h1><p>This sctime session has been closed.</p><button onclick='window.location.reload()'>Restart</button></body></html>";
      )");
  #else
      exit(0);
  #endif
}

/** main: hier wird ueberprueft, ob die Applikation ueberhaupt starten soll
 * (Lockfiles,...), und falls ja, wird SCTimeApp initialisiert und
 ausgefuehrt */
int main(int argc, char **argv ) {
#ifdef __EMSCRIPTEN__
  // for WASM: mount persistent filesytem
  EM_ASM(
     FS.mkdir('/sctime');
     FS.mount(IDBFS, {}, '/sctime');
     sctimefsinitdone=false;
     FS.syncfs(true, function (err) {sctimefsinitdone=true;});
  );
#endif
#ifdef WIN32
  // darkmode needs more testing
  qputenv("QT_QPA_PLATFORM","windows:darkmode=0");
#endif
  SctimeApp* app = new SctimeApp(argc, argv);  // Qt initialization
  // load translations
  QTranslator qtTranslator;
  qtTranslator.load("qt_" + QLocale::system().name(),
          QLibraryInfo::path(QLibraryInfo::TranslationsPath));
  app->installTranslator(&qtTranslator);
  QTranslator sctimeTranslator;
  sctimeTranslator.load(":/translations/sctime");
  app->installTranslator(&sctimeTranslator);

  PERSOENLICHE_KONTEN_STRING = QObject::tr("Personal accounts");
  ALLE_KONTEN_STRING = QObject::tr("All accounts");

  // FIXME: use QCommandLineParser for display of help too
  if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1],"--help") == 0 || strcmp(argv[1], "/h") == 0||strcmp(argv[1],"/help") == 0)) {
      QMessageBox mb(
          QMessageBox::Information,
          QObject::tr("sctime ") + qApp->applicationVersion(),
          "<pre>"+help.toHtmlEscaped()+"</pre>",
          QMessageBox::Ok,
          NULL);
      mb.exec();
      exit(0);
  }
  
  QCommandLineParser parser;

  QCommandLineOption configdiropt("configdir","",QObject::tr("directory"));
  parser.addOption(configdiropt);
  QCommandLineOption zeitkontenfileopt("zeitkontenfile","",QObject::tr("file"));
  parser.addOption(zeitkontenfileopt);
  QCommandLineOption bereitschaftsfileopt("bereitschaftsfile","",QObject::tr("file"));
  parser.addOption(bereitschaftsfileopt);
  QCommandLineOption specialremunfileopt("specialremunfile","",QObject::tr("file"));
  parser.addOption(specialremunfileopt);
  QCommandLineOption offlinefileopt("offlinefile","",QObject::tr("file"));
  parser.addOption(offlinefileopt);
  QCommandLineOption datasourceopt("datasource","", QObject::tr("source"));
  parser.addOption(datasourceopt);
  QCommandLineOption logfileopt("logfile","",QObject::tr("file"));
  parser.addOption(logfileopt);
  QCommandLineOption accountlinkopt("accountlink","",QObject::tr("link"));
  parser.addOption(accountlinkopt);
  parser.process(*app);
  
  QString configdirstring=parser.value(configdiropt);
  QString zeitkontenfile=parser.value(zeitkontenfileopt);
  QString bereitschaftsfile=parser.value(bereitschaftsfileopt);
  QString specialremunfile=parser.value(specialremunfileopt);
  QString offlinefile=parser.value(offlinefileopt);
  QString logfile=parser.value(logfileopt);
  QString accountlink=parser.value(accountlinkopt);
  QStringList dataSourceNames=parser.values(datasourceopt);

  if (configdirstring.isEmpty()) {
    char *envpointer = getenv("SCTIME_CONFIG_DIR");
    configdirstring = envpointer ? envpointer : CONFIGDIR; // default Configdir
  }

  // configdirstring can no longer be empty now but still may be relative or
  // reference home dir by ~
  configdirstring = absolutePath(configdirstring);
  QDir directory;
  if (!directory.cd(configdirstring)) {
    directory.mkdir(configdirstring);
    if (!directory.cd(configdirstring))
      fatal(QObject::tr("sctime: Configuration problem"),
        QObject::tr("Cannot access configration directory %1.").arg(configdirstring));
  }
  configDir.setPath(directory.path());

  SCTIME_IPC=configdirstring+"/SCTIME_IPC";

  if ((!accountlink.isEmpty())&&(openLinkInExistingInstance(accountlink))) {
     return 0;
  }

  if (!zeitkontenfile.isEmpty())
      zeitkontenfile=absolutePath(zeitkontenfile);
  if (!bereitschaftsfile.isEmpty())
      bereitschaftsfile=absolutePath(bereitschaftsfile);
  if (!specialremunfile.isEmpty())
      specialremunfile=absolutePath(specialremunfile);
  if (!offlinefile.isEmpty())
      offlinefile=absolutePath(offlinefile);
  if (!logfile.isEmpty())
      logfile=absolutePath(logfile);

      
  // Locking: nur eine Instanz von sctime soll laufen

  // Ich lege eine lokale Sperre an, die vom Betriebssystem zuverlässig auch
  // nach einem Absturz aufgegeben wird. Außerdem lege ich noch ein globales
  // Lock mit dem Rechnernamen an.
  // Gründe:
  // - Es gibt keine globalen Sperren (für SMB und NFS hinweg sichtbar), die
  //   nach einem Absturz automatisch entfernt werden.
  // - Das Programm kann ausgeben, auf welchem Rechner sctime noch läuft.
  // - Nach einem Absturz kann ich zumindest auf dem gleichen Rechner neu starten,
  //   ohne den Benutzer mit Warnungen wegen alter Sperren zu belästigen.
  LockLocal local("sctime", true);
  bool oldgloballockexists=configDir.exists("LOCK");
  Lockfile *global = new Lockfile(configDir.filePath("LOCK"), true);
  local.setNext(global);
  if (!local.acquire()) fatal(QObject::tr("sctime: Cannot start"), local.errorString());
  if (oldgloballockexists) {
      QFileInfo info(configDir,"settings.xml");
      if (info.exists()) {
        QDateTime lasttime=info.lastModified();
        QMessageBox *msgbox=new QMessageBox(QMessageBox::Critical, QObject::tr("Unclean state"), QObject::tr("It looks like the last instance of sctime might have crashed, probably at %1. Please check if the recorded times of that date are correct.").arg(lasttime.toLocalTime().toString()), QMessageBox::Ok);
        QObject::connect(msgbox, &QMessageBox::finished, msgbox, &QMessageBox::deleteLater);
        // with exec we start an event loop, because we do not have one during starup
        msgbox->exec();
      }
  }
  // the more elegant version does not work for older QT (for example 5.7)
  // clientId=QUuid::createUuid().toString(QUuid::WithoutBraces);
  QString uuid=QUuid::createUuid().toString();
  clientId=uuid.mid(1,uuid.length()-2); // remove curly braces
  clientinfo="sctime-"+app->applicationVersion();
  #ifdef __EMSCRIPTEN__
  char *str = (char*)EM_ASM_PTR({
      return stringToNewUTF8(navigator.userAgent);
  });
  clientinfo += " on " + QString(str);
  free(str); // Each call to _malloc() must be paired with free(), or heap memory will leak!(jsString);
  #endif
  app->init(&local, dataSourceNames, zeitkontenfile, bereitschaftsfile, specialremunfile, offlinefile, logfile, accountlink);
  app->exec();
  
  // warning: dont rely on anything being executed beyond that point

  #ifndef __linux__ // do not delete app on linux as this triggers some obscure hang in ~QXcbConnection with certain x2go settings
  delete app;
  #endif
  delete global;
  return 0;
}
