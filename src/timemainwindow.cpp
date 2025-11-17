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

#include "timemainwindow.h"

#include <QClipboard>
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QFont>
#include <QTimer>
#include <QMessageBox>
#include <QStringList>
#include <QDir>
#include <QPoint>
#include <QFile>
#include <QDebug>
#include <QToolBar>
#include <QColorDialog>
#include <QTextBrowser>
#include <QByteArray>
#include <QAction>
#include <QMutex>
#include <QLocale>
#include <QTextStream>
#include <QUrlQuery>
#include <QLocalServer>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQueue>
#include <QStatusBar>
#include <QGuiApplication>
#include <QScreen>
#include <QStringEncoder>
#include <QFileDialog>

#ifdef __EMSCRIPTEN__
#include <emscripten/val.h>
#include <emscripten.h>
#endif

#include "globals.h"
#include "time.h"
#include "preferencedialog.h"
#include "datedialog.h"
#include "findkontodialog.h"
#include "defaulttagreader.h"
#include "statusbar.h"
#include "kontotreeitem.h"
#include "bereitschaftsliste.h"
#include "bereitschaftsview.h"
#include "globals.h"
#include "timeedit.h"
#include "kontotreeview.h"
#include "unterkontodialog.h"
#include "kontotreeview.h"
#include "resthelper.h"
#include "defaultcommentreader.h"
#include "abteilungsliste.h"
#include "sctimexmlsettings.h"
#include "specialremunentryhelper.h"
#include "lock.h"
#include "datasource.h"
#ifndef DISABLE_PUNCHCLOCK
#include "punchclockdialog.h"
#endif
#include "setupdsm.h"
#include "specialremunerationsdialog.h"
#include "util.h"
#include "textviewerdialog.h"
#include "accountlistcommiter.h"
#include "punchclockchecker.h"
#include "pausedialog.h"
#include "datechanger.h"
#include "xmlwriter.h"
#include "xmlreader.h"
#include "oncalldialog.h"
#include "conflictdialog.h"
#ifdef RESTONLY
#ifndef __EMSCRIPTEN__
#include "logindialog.h"
#endif
#endif
#ifdef DOWNLOADDIALOG
#include "downloadshdialog.h"
#endif
#include "deletesettingsdialog.h"
#include "syncofflinehelper.h"
#include "datechangedialog.h"
#include "timetrackertasksreader.h"


QTreeWidget* TimeMainWindow::getKontoTree() { return kontoTree; }

static QString logTextLastLine(QObject::tr("-- Start --"));
static QString logText(logTextLastLine + "\n");
static QFile *logFile;
static QTextStream *logStream=NULL;
void trace(const QString &msg) {
  logError(msg);
}

void logError(const QString &msg) {
  logText.append(msg).append("\n");
  logTextLastLine = msg;
  if (logStream!=NULL) {
    (*logStream)<<msg<<Qt::endl;
  }
}

/** Erzeugt ein neues TimeMainWindow, das seine Daten aus abtlist bezieht. */
TimeMainWindow::TimeMainWindow(Lock* lock, QNetworkAccessManager *networkAccessManager, DSM* dsm, QString logfile):QMainWindow(), startTime(QDateTime::currentDateTime()),
     windowIcon(":/window_icon"), pausedWindowIcon(":/window_icon_paused"), networkAccessManager(networkAccessManager)  {
  initCompleted=false;
  if (!logfile.isNull() && !logfile.isEmpty()) {
    logFile = new QFile(logfile);
    if (logFile->open(QIODevice::ReadWrite)) {
       logStream=new QTextStream(logFile);
    } else {
      logStream = NULL;
    }
  }
  m_lock = lock;
  m_dsm=dsm;
  paused = false;
  entryBeingEdited = false;
  kontoTree =NULL;
  sekunden = 0;
  m_afterCommitMethodQueue = new QQueue<QueuedMethod*>();
  setObjectName(tr("sctime"));
  QDate heute;
  abtListToday=new AbteilungsListe(heute.currentDate(), zk);
  abtList=abtListToday;
  m_punchClockListToday=new PunchClockList();
  m_punchClockList=m_punchClockListToday;
  m_PCSToday=newPunchClockState();
  m_PCSYesterday=newPunchClockState();
  m_PCSToday->setDate(heute.currentDate());
  m_PCSYesterday->setDate(heute.currentDate().addDays(-1));
  pausedAbzur=false;
  inPersoenlicheKontenAllowed=true;
  powerToolBar = NULL;
  cantSaveDialog = NULL;
  m_dateChanger = NULL;

  statusBar = new StatusBar(this);
  setStatusBar(statusBar);
  connect(statusBar, &StatusBar::onlineStatusClicked, this, &TimeMainWindow::toggleOnlineStatus);
  std::vector<int> columnwidthlist;

#ifndef Q_OS_MAC
  setWindowIcon(windowIcon);
#endif

  toolBar   = new QToolBar(tr("Main toolbar"), this);
  toolBar->setIconSize(QSize(22,22));
  QMenu * kontomenu = menuBar()->addMenu(tr("&Account"));
  QMenu * zeitmenu = menuBar()->addMenu(tr("&Time"));
  QMenu * remunmenu = menuBar()->addMenu(tr("&Remuneration"));
  QMenu * settingsmenu = menuBar()->addMenu(tr("&Settings"));
  QMenu * hilfemenu = menuBar()->addMenu(tr("&Help"));

  auto now=QDateTime::currentDateTime();
  m_punchClockListToday->push_back(PunchClockEntry(now.time().msecsSinceStartOfDay()/1000,now.time().msecsSinceStartOfDay()/1000));
  m_punchClockListToday->setCurrentEntry(std::prev(m_punchClockListToday->end()));

  QAction* pauseAction = new QAction( QIcon(":/hi22_action_player_pause"), tr("&Pause"), this);
  pauseAction->setShortcut(Qt::CTRL|Qt::Key_P);
  connect(pauseAction, SIGNAL(triggered()), this, SLOT(pause()));

  QAction* pauseAbzurAction = new QAction( QIcon(":/hi22_action_player_pause_half"),
                                           tr("Pause &accountable time"), this);
  pauseAbzurAction->setShortcut(Qt::CTRL|Qt::Key_A);
  pauseAbzurAction->setCheckable(true);
  connect(pauseAbzurAction, SIGNAL(toggled(bool)), this, SLOT(pauseAbzur(bool)));

  QAction* saveAction = new QAction( QIcon(":/hi22_action_filesave" ), tr("&Save"), this);
  saveAction->setShortcut(Qt::CTRL|Qt::Key_S);
  connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

  copyAction = new QAction(tr("&Copy as text"), this);
  copyAction->setShortcut(Qt::CTRL|Qt::Key_C);
  connect(copyAction, SIGNAL(triggered()), this, SLOT(copyEntryAsText()));

  copyLinkAction = new QAction(tr("Copy as &link"), this);
  copyLinkAction->setShortcut(Qt::CTRL|Qt::Key_L);
  connect(copyLinkAction, SIGNAL(triggered()), this, SLOT(copyEntryAsLink()));

  QAction* pasteLinkAction = new QAction(tr("Paste link"), this);
  pasteLinkAction->setShortcut(Qt::CTRL|Qt::Key_V);
  connect(pasteLinkAction, SIGNAL(triggered()), this, SLOT(pasteEntryAsLink()));

  QAction* changeDateAction = new QAction(tr("C&hoose Date..."), this);
  changeDateAction->setShortcut(Qt::CTRL|Qt::Key_D);
  connect(changeDateAction, SIGNAL(triggered()), this, SLOT(callDateDialog()));

#ifndef DISABLE_PUNCHCLOCK
  QAction* punchClockAction = new QAction(tr("Punch Clock"), this);
  punchClockAction->setShortcut(Qt::CTRL|Qt::Key_O);
  connect(punchClockAction, SIGNAL(triggered()), this, SLOT(callPunchClockDialog()));
#endif

  QAction* resetAction = new QAction( tr("Re&set difference"), this);
  resetAction->setShortcut(Qt::CTRL|Qt::Key_N);
  connect(resetAction, SIGNAL(triggered()), this, SLOT(resetDiff()));

  inPersKontAction = new QAction( QIcon(":/hi22_action_attach"), tr("Select as personal &account"), this);
  inPersKontAction->setShortcut(Qt::CTRL|Qt::Key_K);
  inPersKontAction->setCheckable(true);
  connect(inPersKontAction, SIGNAL(toggled(bool)), this, SLOT(inPersoenlicheKonten(bool)));

  QAction* quitAction = new QAction(tr("&Quit"), this);
  // force this item to have the quit role so that Qt properly moves it into
  // the Mac application menu.
  // FIXME: This is a workaround. With proper translation, Qt's heuristic would
  // do this automatically based on the menu item title. Also, translation does
  // not seem to work as this time. The menu tetxts always end up English after
  // being merged into the application menu.
  quitAction->setMenuRole(QAction::QuitRole);
  quitAction->setShortcut(Qt::CTRL|Qt::Key_Q);
  connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

  QAction* findKontoAction = new QAction(tr("&Search account..."), this);
  findKontoAction->setShortcut(Qt::CTRL|Qt::Key_F);
  //findKontoAction->setStatusTip(tr("Konto suchen"));
  connect(findKontoAction, SIGNAL(triggered()), this, SLOT(callFindKontoDialog()));

  QAction* refreshAction = new QAction(tr("&Reread account list"), this);
  refreshAction->setShortcut(Qt::CTRL|Qt::Key_R);
  connect(refreshAction, SIGNAL(triggered()), this, SLOT(refreshKontoListe()));

  QAction* preferenceAction = new QAction(tr("&Settings..."),this);
  preferenceAction->setMenuRole(QAction::PreferencesRole);
  connect(preferenceAction, SIGNAL(triggered()), this, SLOT(callPreferenceDialog()));

  QAction* helpAction = new QAction(tr("&Manual..."), this);
  helpAction->setShortcut(Qt::Key_F1);
  connect(helpAction, SIGNAL(triggered()), this, SLOT(callHelpDialog()));  
  
  QAction* aboutAction = new QAction(tr("&About sctime..."), this);
  aboutAction->setMenuRole(QAction::AboutRole);
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(callAboutBox()));

  QAction* qtAction = new QAction(tr("About &Qt..."), this);
  qtAction->setMenuRole(QAction::AboutQtRole);
  connect(qtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  
  QAction* addlicAction = new QAction(tr("Additional &License Information..."), this);
  connect(addlicAction, SIGNAL(triggered()), this, SLOT(callAdditionalLicenseDialog()));

  QAction* logAction = new QAction(tr("&Messages..."), this);
  connect(logAction, SIGNAL(triggered()), this, SLOT(logDialog()));

  editUnterKontoAction = new QAction(QIcon(":/hi22_action_edit" ), tr("&Edit..."), this);
  editUnterKontoAction->setShortcuts({Qt::Key_Return, Qt::Key_Menu});
  connect(editUnterKontoAction, SIGNAL(triggered()), this, SLOT(editUnterKontoPressed()));

  QAction* eintragActivateAction = new QAction(tr("&Activate entry"), this);
  eintragActivateAction->setShortcut(Qt::CTRL|Qt::Key_X);
  connect(eintragActivateAction, SIGNAL(triggered()), this, SLOT(eintragAktivieren()));

  QAction* eintragAddAction = new QAction(QIcon(":/hi22_action_queue" ),
                                             tr("Add &entry"), this);
  eintragAddAction->setShortcut(Qt::CTRL|Qt::Key_Plus);
  connect(eintragAddAction, SIGNAL(triggered()), this, SLOT(eintragHinzufuegen()));

  eintragRemoveAction = new QAction(tr("&Delete entry"), this);
  eintragRemoveAction->setShortcut(Qt::Key_Delete);
  connect(eintragRemoveAction, SIGNAL(triggered()), this, SLOT(eintragEntfernen()));

  onCallAction = new QAction(QIcon(":/hi16_action_stamp" ),
                                            tr("Set &on-call times..."), this);
  onCallAction->setShortcut(Qt::CTRL|Qt::Key_B);
  connect(onCallAction, SIGNAL(triggered()), this, SLOT(editBereitschaftPressed()));
  
  specialRemunAction = new QAction(QIcon(":/hi16_moon" ),
                                            tr("Set special remuneration &times..."), this);
  specialRemunAction->setShortcut(Qt::CTRL|Qt::Key_T);
  connect(specialRemunAction, SIGNAL(triggered()), this, SLOT(specialRemunPressed()));

  bgColorChooseAction = new QAction(tr("Choose &background colour..."), this);
  bgColorChooseAction->setShortcut(Qt::CTRL|Qt::Key_G);
  bgColorRemoveAction = new QAction(tr("&Remove background colour"), this);

  QAction* downloadSHAction = new QAction(tr("Download sh files"), this);
  connect(downloadSHAction, SIGNAL(triggered()), this, SLOT(callDownloadSHDialog()));

  QAction* readSettingsFromFileAction = new QAction(tr("Import"), this);
  connect(readSettingsFromFileAction, SIGNAL(triggered()), this, SLOT(readSettingsFromFile()));

  QAction* readTimeTrackerTasksAction = new QAction(tr("Import time tracker tasks"), this);
  connect(readTimeTrackerTasksAction, SIGNAL(triggered()), this, SLOT(readTimeTrackerTasks()));

  QAction* deleteSettingsAction = new QAction(tr("Delete settings files"), this);
  connect(deleteSettingsAction, SIGNAL(triggered()), this, SLOT(callDeleteSettingsDialog()));

  /*QAction* syncAllAction = new QAction(tr("Sync All"), this);
  connect(syncAllAction, SIGNAL(triggered()), this, SLOT(syncAll()));*/

  jumpAction = new QAction(tr("S&how selected account in 'all accounts'"), this);

#ifndef WASMQUIRKS // leads to issues in WASM, disable for now
  pauseAbzurAction->setStatusTip(tr("Pause only tracking of accountable time"));
  copyAction->setStatusTip(tr("Copy infos about account and entry as text to clipboard"));
  copyLinkAction->setStatusTip(tr("Copy infos about account and entry as a link to clipboard"));
  pasteLinkAction->setStatusTip(tr("Open account from link from clipboard"));
  resetAction->setStatusTip(tr("Set active account's accountable time equal worked time"));
  quitAction->setStatusTip(tr("Quit program"));
  aboutAction->setStatusTip(tr("About sctime..."));
  editUnterKontoAction->setStatusTip(tr("Edit subaccount"));
#endif

  QAction* min5PlusAction = new QAction(QIcon(":/hi22_action_1uparrow" ),
                                          tr("Increase time"), this);
  QAction* min5MinusAction = new QAction(QIcon(":/hi22_action_1downarrow" ),
                                            tr("Decrease time"), this);

  QAction* fastPlusAction = new QAction(QIcon(":/hi22_action_2uparrow" ),
                                           tr("Increase time fast"), this);
  QAction* fastMinusAction = new QAction(QIcon(":/hi22_action_2downarrow" ),
                                            tr("Decrease time fast"), this);

  abzurMin5PlusAction = new QAction(QIcon(":/hi22_action_1uparrow_half" ),
                                      tr("Increase accountable time"), this);
  abzurMin5MinusAction = new QAction(QIcon(":/hi22_action_1downarrow_half" ),
                                       tr("Decrease accountable time"), this);

  fastAbzurPlusAction = new QAction(QIcon(":/hi22_action_2uparrow_half" ),
                                      tr("Increase accountable time fast"), this);
  fastAbzurMinusAction = new QAction(QIcon(":/hi22_action_2downarrow_half" ),
                                       tr("Decrease accountable time fast"), this);

  overtimeRegulatedModeAction = new QAction(tr("Toggle regulated overtime mode"), this);
  overtimeRegulatedModeAction->setCheckable(true);
  connect(overtimeRegulatedModeAction, SIGNAL(toggled(bool)), this, SLOT(switchOvertimeRegulatedMode(bool)));
  overtimeOtherModeAction = new QAction(tr("Toggle other overtime mode"), this);
  overtimeOtherModeAction->setCheckable(true);
  connect(overtimeOtherModeAction, SIGNAL(toggled(bool)), this, SLOT(switchOvertimeOtherMode(bool)));

  nightModeAction = new QAction(tr("Toggle night mode"), this);
  nightModeAction->setCheckable(true);
  connect(nightModeAction, SIGNAL(toggled(bool)), this, SLOT(switchNightMode(bool)));

  publicHolidayModeAction = new QAction(tr("Toggle public holiday mode"), this);
  publicHolidayModeAction->setCheckable(true);
  connect(publicHolidayModeAction, SIGNAL(toggled(bool)), this, SLOT(switchPublicHolidayMode(bool)));

  connect(min5PlusAction, SIGNAL(triggered()), this, SLOT(addTimeInc()));
  connect(min5MinusAction, SIGNAL(triggered()), this, SLOT(subTimeInc()));
  connect(fastPlusAction, SIGNAL(triggered()), this, SLOT(addFastTimeInc()));
  connect(fastMinusAction, SIGNAL(triggered()), this, SLOT(subFastTimeInc()));

  connect(bgColorChooseAction, SIGNAL(triggered()),this, SLOT(callColorDialog()));
  connect(bgColorRemoveAction, SIGNAL(triggered()),this, SLOT(removeBgColor()));
  connect(jumpAction, SIGNAL(triggered()),this, SLOT(jumpToAlleKonten()));

  connect(this,SIGNAL(eintragSelected(bool)), min5PlusAction, SLOT(setEnabled(bool)));
  connect(this,SIGNAL(eintragSelected(bool)), min5MinusAction, SLOT(setEnabled(bool)));
  connect(this,SIGNAL(eintragSelected(bool)), fastPlusAction, SLOT(setEnabled(bool)));
  connect(this,SIGNAL(eintragSelected(bool)), fastMinusAction, SLOT(setEnabled(bool)));
  connect(this,SIGNAL(augmentableItemSelected(bool)), eintragAddAction, SLOT(setEnabled(bool)));

  connect(this,SIGNAL(aktivierbarerEintragSelected(bool)), eintragActivateAction, SLOT(setEnabled(bool)));

  toolBar->addAction(editUnterKontoAction);
  toolBar->addAction(saveAction);
  toolBar->addAction(inPersKontAction);
  toolBar->addAction(eintragAddAction);
  toolBar->addAction(onCallAction);
  toolBar->addAction(specialRemunAction);
  toolBar->addAction(pauseAction);
  toolBar->addAction(min5PlusAction);
  toolBar->addAction(min5MinusAction);
  toolBar->addAction(fastPlusAction);
  toolBar->addAction(fastMinusAction);
  kontomenu->addAction(editUnterKontoAction);
  kontomenu->addAction(eintragActivateAction);
  kontomenu->addAction(eintragAddAction);
  kontomenu->addAction(eintragRemoveAction);
  kontomenu->addAction(saveAction);
  kontomenu->addAction(pauseAction);
  kontomenu->addAction(pauseAbzurAction);
  kontomenu->addAction(inPersKontAction);
  remunmenu->addAction(specialRemunAction);
  remunmenu->addAction(overtimeRegulatedModeAction);
  remunmenu->addAction(overtimeOtherModeAction);
  remunmenu->addAction(nightModeAction);
  remunmenu->addAction(publicHolidayModeAction);
  kontomenu->addSeparator();
  kontomenu->addAction(findKontoAction);
  kontomenu->addAction(jumpAction);
  kontomenu->addAction(copyAction);
  kontomenu->addAction(copyLinkAction);
  kontomenu->addAction(pasteLinkAction);
  kontomenu->addAction(refreshAction);
  kontomenu->addAction(readTimeTrackerTasksAction);
  settingsmenu->addAction(preferenceAction);
#ifdef DOWNLOADDIALOG
  kontomenu->addAction(downloadSHAction);
  settingsmenu->addAction(readSettingsFromFileAction);
  settingsmenu->addAction(deleteSettingsAction);
#endif
  kontomenu->addSeparator();
  kontomenu->addAction(bgColorChooseAction);
  kontomenu->addAction(bgColorRemoveAction);
  remunmenu->addSeparator();
  remunmenu->addAction(onCallAction);
  kontomenu->addSeparator();
  kontomenu->addAction(quitAction);
  zeitmenu->addAction(changeDateAction);
#ifndef DISABLE_PUNCHCLOCK
  zeitmenu->addAction(punchClockAction);
#endif
  zeitmenu->addAction(resetAction);
  //zeitmenu->addAction(syncAllAction);
  hilfemenu->addAction(helpAction);
  hilfemenu->addAction(aboutAction);
  hilfemenu->addAction(qtAction);
  QString overviewstr="/licenses/overview.html";
  additionalLicensesFile = QCoreApplication::applicationDirPath()+overviewstr;
  QFileInfo infofile(additionalLicensesFile);

  if (infofile.exists()) {
    hilfemenu->addAction(addlicAction);
  } else {
    additionalLicensesFile = ":/additional_legal"+overviewstr;
    if (QFile(additionalLicensesFile).exists()) {
       hilfemenu->addAction(addlicAction);
    }
  }

  hilfemenu->addSeparator();
  hilfemenu->addAction(logAction);

  addToolBar(toolBar);

  saveLaterTimer = NULL;

  settings=new SCTimeXMLSettings();
#if defined(RESTONLY) && !defined(__EMSCRIPTEN__)
  auto loginDialog = new LoginDialog(networkAccessManager, this);
  connect(loginDialog, &LoginDialog::finished, this, &TimeMainWindow::readInitialSetting);
  QTimer::singleShot(100, loginDialog, SLOT(open()));
#else
  QTimer::singleShot(100, this, SLOT(readInitialSetting()));
#endif
}

void TimeMainWindow::readSettingsFromFile() {
  QFileDialog::getOpenFileContent("Settings XML (*.xml)", [=](const QString &fileName, const QByteArray &fileContent){
    XMLReader *reader=new XMLReader(settings, networkAccessManager, true, false, false, abtList, m_punchClockList);
    QDomDocument *doc=new QDomDocument();
    doc->setContent(fileContent);
    connect(reader, &XMLReader::settingsPartRead, [=](){
      refreshKontoListe();
      reader->deleteLater();
      delete doc;
    });
    reader->fillSettingsFromDocument(*doc, settings);
    applySettings();
  });
}

void TimeMainWindow::readInitialSetting() {
    auto finish = [=](){
      XMLReader *reader=new XMLReader(settings, networkAccessManager ,true, false, false, abtList, m_punchClockList);
      connect(reader, &XMLReader::settingsRead, this, &TimeMainWindow::initialSettingsRead);
      connect(reader, &XMLReader::offlineSwitched, this, &TimeMainWindow::switchRestCurrentlyOffline);
      connect(reader, &XMLReader::settingsRead, reader, &XMLReader::deleteLater);
      connect(reader, &XMLReader::unauthorized, this, &TimeMainWindow::sessionInvalid);
      connect(reader, &XMLReader::conflictingClientRunning, this, &TimeMainWindow::readConflictDialog);
      connect(reader, &XMLReader::conflictedWithLocal, this, &TimeMainWindow::readConflictWithLocalDialog);
      reader->open();
    };
#ifdef __EMSCRIPTEN__
    static int retryCount = 0;
    int initdone = EM_ASM_INT({
        return sctimefsinitdone;
    });
    if (!initdone) {
      if (retryCount<100) {
        retryCount++;
        // try again later
        QTimer::singleShot(200, this, SLOT(readInitialSetting()));
      } else {
        // Filesystem wont come up, give up
        stopAppHard();
      }
      
      return;
    }
    // in wasm we have the permanent offline mode. We try to load a local file first, to have information initialized if we have to stay offline
    XMLReader *reader=new XMLReader(settings, networkAccessManager, true, true, false, abtList, m_punchClockList);
    connect(reader, &XMLReader::settingsRead, finish);
    connect(reader, &XMLReader::offlineSwitched, this, &TimeMainWindow::switchRestCurrentlyOffline);
    connect(reader, &XMLReader::settingsRead, reader, &XMLReader::deleteLater);
    connect(reader, &XMLReader::unauthorized, this, &TimeMainWindow::sessionInvalid);
    connect(reader, &XMLReader::conflictingClientRunning, this, &TimeMainWindow::readConflictDialog);
    connect(reader, &XMLReader::conflictedWithLocal, this, &TimeMainWindow::readConflictWithLocalDialog);
    reader->open();
#else
    finish();    
#endif
}

void TimeMainWindow::initialSettingsRead() {
  std::vector<int> columnwidthlist;
  settings->getColumnWidthList(columnwidthlist);
  

  QAction* min1MinusAction = new QAction(tr("Minimal decrease time"), this);
  min1MinusAction->setShortcut(Qt::CTRL|Qt::Key_Comma);
  connect(min1MinusAction, SIGNAL(triggered()), this, SLOT(subMinimalTimeInc()));

  kontoTree=new KontoTreeView(this, abtList, columnwidthlist, settings->defCommentDisplayMode(), settings->sortByCommentText());
  connect(kontoTree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem * )), this, SLOT(changeShortCutSettings(QTreeWidgetItem * ) ));
  kontoTree->showPersoenlicheKontenSummenzeit(settings->persoenlicheKontensumme());
  kontoTree->addAction(min1MinusAction);
#ifndef Q_OS_MAC
  setWindowIcon(QIcon(":/window_icon"));
#endif

  setCentralWidget(kontoTree);
  applySettings();  
  switchRestCurrentlyOffline(false);

  loadPCCData(settings->previousPCCData());
  loadPCCData(settings->currentPCCData());

  minutenTimer = new QTimer(this);
  connect( minutenTimer,SIGNAL(timeout()), this, SLOT(minuteHochzaehlen()));
  lastMinuteTick = startTime;
  minutenTimer->setInterval(60000); //Alle 60 Sekunden ticken
  minutenTimer->start();
  // Timer für "angebrochene" Minuten nach einer Pause
  restTimer = new QTimer(this);
  restTimer->setSingleShot(true);
  connect(restTimer, SIGNAL(timeout()), minutenTimer, SLOT(start()));
  connect(restTimer, SIGNAL(timeout()), this, SLOT(minuteHochzaehlen()));

  autosavetimer=new QTimer(this);
  connect( autosavetimer,SIGNAL(timeout()), this, SLOT(save()));
  autosavetimer->setInterval(300000); //save every 5 minutes
  
  zeitChanged();

  auto now=QDateTime::currentDateTime();
  if (now.date()==abtListToday->getDatum()) {
      auto currentEntry = m_punchClockListToday->currentEntry();
      if ((currentEntry!=m_punchClockListToday->end())&&(abs(currentEntry->second-now.time().msecsSinceStartOfDay()/1000)<120)) {
        // if we already have a matching entry, we do not add a new one
        currentEntry->second=now.time().msecsSinceStartOfDay()/1000;
      } else {
        m_punchClockListToday->push_back(PunchClockEntry(now.time().msecsSinceStartOfDay()/1000,now.time().msecsSinceStartOfDay()/1000));
        m_punchClockListToday->setCurrentEntry(std::prev(m_punchClockListToday->end()));
      }
  }

  changeShortCutSettings(NULL); // Unterkontenmenues deaktivieren...

  updateCaption();

  kontoTree->setContextMenuPolicy(Qt::CustomContextMenu);

  m_dsm->setup(settings, networkAccessManager);

  connect(m_dsm->kontenDSM, SIGNAL(finished(DSResult)), this, SLOT(commitKontenliste(DSResult)));
  connect(m_dsm->bereitDSM, SIGNAL(finished(DSResult)), this, SLOT(commitBereit(DSResult)));
  connect(m_dsm->specialRemunDSM, SIGNAL(finished(DSResult)), this, SLOT(commitSpecialRemun(DSResult)));
  connect(m_dsm->kontenDSM, SIGNAL(aborted()), this, SLOT(displayLastLogEntry()));
  connect(m_dsm->bereitDSM, SIGNAL(aborted()), this, SLOT(displayLastLogEntry()));
  connect(m_dsm->specialRemunDSM, SIGNAL(aborted()), this, SLOT(displayLastLogEntry()));
  connect(kontoTree, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));
  m_afterCommitMethodQueue->clear();
  if (m_dsm->bereitDSM!=m_dsm->kontenDSM) {
     m_afterCommitMethodQueue->enqueue(new QueuedMethod(m_dsm->bereitDSM, "start"));
  }
  if ((m_dsm->specialRemunDSM!=m_dsm->kontenDSM)&&(m_dsm->specialRemunDSM!=m_dsm->bereitDSM)) {
     m_afterCommitMethodQueue->enqueue(new QueuedMethod(m_dsm->specialRemunDSM, "start"));
  }
  QMetaObject::invokeMethod(this, "refreshKontoListe", Qt::QueuedConnection);
  

  specialRemunAction->setEnabled(false);

  m_ipcserver = new QLocalServer(this);
  connect(m_ipcserver, SIGNAL(newConnection()), this, SLOT(readIPCMessage()));
  if (!m_ipcserver->listen(SCTIME_IPC)) {
     trace(tr("cannot start ipc server"));
  }
  autosavetimer->start();
// enable this feature only for WASM for performance reasons
#ifdef __EMSCRIPTEN__
  saveLaterTimer = new QTimer();
  saveLaterTimer->setSingleShot(true);
  connect(saveLaterTimer, &QTimer::timeout, this, &TimeMainWindow::save);
#endif
}

void TimeMainWindow::applySettings() {
  kontoTree->setAcceptDrops(settings->dragNDrop());
  kontoTree->showAktivesProjekt();
  kontoTree->updateColumnWidth();
  //close the flagged items, needed if "Summe in pers. Konten" is 
  //selected
  kontoTree->closeFlaggedPersoenlicheItems();
  showAdditionalButtons(settings->powerUserView());
  std::vector<QString> xmlfilelist;
  settings->getDefaultCommentFiles(xmlfilelist);
  qtDefaultFont=QApplication::font();
  if (settings->useCustomFont())
  {
    QString custFont=settings->customFont();
    int custFontSize=settings->customFontSize();
    QApplication::setFont(QFont(custFont,custFontSize));
  }
  
  defaultCommentReader.read(abtList,xmlfilelist);
  DefaultTagReader defaulttagreader;
  defaulttagreader.read(&defaultTags);

  // restore size+position
  #ifndef RESTONLY
  QSize size = QSize(700,400);
  QPoint pos = QPoint(0,0);
  settings->getMainWindowGeometry(pos,size);
  resize(size);
  move(pos);
  #endif

  kontoTree->closeFlaggedPersoenlicheItems();

  if (!settings->showTypeColumn()) {
    kontoTree->hideColumn(KontoTreeItem::COL_TYPE);
  }
  if (!settings->showPSPColumn()) {
    kontoTree->hideColumn(KontoTreeItem::COL_PSP);
  }

  configClickMode(settings->singleClickActivation());
  updateSpecialModes(true);

}

void TimeMainWindow::displayLastLogEntry(){
#ifndef WASMQUIRKS
  statusBar->showMessage(logTextLastLine);
#endif
  QApplication::restoreOverrideCursor();
}

void TimeMainWindow::aktivesKontoPruefen(){
  QString a,k, u;
  int i;
  abtList->getAktiv(a, k, u, i);
  EintragsListe::iterator dummy;
  EintragsListe *dummy2;
  // if we are initalizing for the first time, this is the last step. Otherwise initCompleted should already be true.
  initCompleted=true;
  if (!abtList->findEintrag(dummy, dummy2, a, k, u, i)) {
    QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
          QObject::tr("sctime: accounting stopped"),
          tr("The last active account was %1/%2. It seems to have been closed or renamed. "
             "Please activate a new account to start time accounting!").arg(k,u),QMessageBox::NoButton,this);
    connect(msgbox, &QMessageBox::finished, msgbox, &QMessageBox::deleteLater);
    msgbox->open();
    msgbox->raise();
  }
}

void TimeMainWindow::closeEvent(QCloseEvent * event)
{
  save();
  QMainWindow::closeEvent(event);
}

/** Destruktor  */
TimeMainWindow::~TimeMainWindow()
{
   delete m_PCSToday;
   delete m_PCSYesterday;
   delete settings;
   if (abtList!=abtListToday)
     delete abtListToday;
   delete abtList;
}

void TimeMainWindow::showAdditionalButtons(bool show)
{
   if (show) {
      if (powerToolBar!=NULL) return;
      powerToolBar = addToolBar(tr("Power Buttons"));
      powerToolBar->setIconSize(toolBar->iconSize());
      powerToolBar->addAction(abzurMin5PlusAction);
      powerToolBar->addAction(abzurMin5MinusAction);
      powerToolBar->addAction(fastAbzurPlusAction);
      powerToolBar->addAction(fastAbzurMinusAction);
      connect(abzurMin5PlusAction, SIGNAL(triggered()), this, SLOT(addAbzurTimeInc()));
      connect(abzurMin5MinusAction, SIGNAL(triggered()), this, SLOT(subAbzurTimeInc()));
      connect(fastAbzurPlusAction, SIGNAL(triggered()), this, SLOT(addFastAbzurTimeInc()));
      connect(fastAbzurMinusAction, SIGNAL(triggered()), this, SLOT(subFastAbzurTimeInc()));
      connect(this,SIGNAL(eintragSelected(bool)), abzurMin5PlusAction, SLOT(setEnabled(bool)));
      connect(this,SIGNAL(eintragSelected(bool)), abzurMin5MinusAction, SLOT(setEnabled(bool)));
      connect(this,SIGNAL(eintragSelected(bool)), fastAbzurPlusAction, SLOT(setEnabled(bool)));
      connect(this,SIGNAL(eintragSelected(bool)), fastAbzurMinusAction, SLOT(setEnabled(bool)));
   } else {
      if (powerToolBar==NULL) return;
      delete(powerToolBar);
      powerToolBar = NULL;
   }
}

void TimeMainWindow::configClickMode(bool singleClickActivation)
{
    disconnect(kontoTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int )),
               this, SLOT(callUnterKontoDialog(QTreeWidgetItem *)) );
    disconnect(kontoTree, SIGNAL(itemRightClicked(QTreeWidgetItem *)),
               this, SLOT(callUnterKontoDialog(QTreeWidgetItem *)));
    disconnect(kontoTree, SIGNAL(itemClicked(QTreeWidgetItem *, int )),
               this, SLOT(setAktivesProjekt(QTreeWidgetItem *)));
    disconnect(kontoTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int )),
               this, SLOT(setAktivesProjekt(QTreeWidgetItem *)));

    if (!singleClickActivation) {
      connect(kontoTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int )),
              this, SLOT(setAktivesProjekt(QTreeWidgetItem *)));
      connect(kontoTree, SIGNAL(itemRightClicked(QTreeWidgetItem *)),
              this, SLOT(callUnterKontoDialog(QTreeWidgetItem *)) );
    } else {
      connect(kontoTree, SIGNAL(itemClicked ( QTreeWidgetItem * , int )),
              this, SLOT(setAktivesProjekt(QTreeWidgetItem *)));
      connect(kontoTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int )),
              this, SLOT(callUnterKontoDialog(QTreeWidgetItem *)) );
    }
}

void TimeMainWindow::suspend() {
    // sonst könnten die anderen Timer früher als das Resume-Ereignis eintreffen
    stopTimers(tr("suspend"));
}

void TimeMainWindow::resume() {
  QDateTime now(QDateTime::currentDateTime()), before(lastMinuteTick);
  if (! paused) {
      minutenTimer->start();
      autosavetimer->start();
  }
  int pauseSecs = before.secsTo(now);
#ifndef WASMQUIRKS
  statusBar->showMessage(tr("resume"), 3000);
#endif
  trace(tr("resume %2; suspend was %1").arg(lastMinuteTick.toString(), now.toString()));
  if (pauseSecs < 60) return;
  sekunden += pauseSecs; // damit  sich driftKorrektur() nicht beschwert
  lastMinuteTick = now;
  if (pauseSecs > 12 * 3600 * 3600) {
    QMessageBox *msgboxinfo=new QMessageBox(QMessageBox::Information,
          tr("sctime: resume"),
          tr("The machine was suspended from %1 until %2. Please check and adjust accounted time if necessary!")
          .arg(before.toString(), now.toString()), QMessageBox::NoButton, this);
    connect(msgboxinfo, &QMessageBox::finished, msgboxinfo, &QMessageBox::deleteLater);
    msgboxinfo->open();
    msgboxinfo->raise();
    return;
  }
  auto msgbox=new QMessageBox(QMessageBox::Question, tr("sctime: resume"),
        tr("The machine was suspended from %1 until %2. Should this time be added to the active account?")
        .arg(before.toString(), now.toString()),
	QMessageBox::Yes| QMessageBox::No, this);
  connect(msgbox, &QMessageBox::finished,
  [=](){
    if (msgbox->result() == QMessageBox::Yes) {
       zeitKorrektur(pauseSecs);
       saveLater();
    }
    msgbox->deleteLater();
   });
   msgbox->open();
   msgbox->raise();
}

/**
 * @brief Adjusts the time for the currently active entry in today's list.
 *
 * This function retrieves the currently active entry from `abtListToday` and applies a time correction
 * by the specified delta. The correction can be either regular or manual, as indicated by the `regular` flag.
 * After updating the time, the corresponding item in `kontoTree` is refreshed and a notification of the time change is triggered.
 *
 * @param delta The amount of time to adjust (in seconds or appropriate units).
 * @param regular Indicates whether the time correction is regular (true) or manual (false).
 */
void TimeMainWindow::zeitKorrektur(int delta, bool regular) {
  QString abt,ko,uko;
  int idx;
  abtListToday->getAktiv(abt,ko,uko,idx);
  abtListToday->changeZeit(abt, ko, uko, idx, delta, regular, pausedAbzur);
  kontoTree->refreshItem(abt,ko,uko,idx);
  zeitChanged();
}

void TimeMainWindow::copyNameToClipboard()
 {
    QClipboard *cb = QApplication::clipboard();
    cb->setText( kontoTree->currentItem()->text(KontoTreeItem::COL_ACCOUNTS), QClipboard::Clipboard );
}

void TimeMainWindow::driftKorrektur() {
  int drift = startTime.secsTo(lastMinuteTick) - sekunden;
  if (abs(drift) < 60) return;
  QString msg = tr("Drift is %2s (%1)").arg(lastMinuteTick.toString()).arg(drift);
  logError(msg);
  sekunden += drift;
  {
    QFile logFile(configDir.filePath("sctime.log"));
    if (logFile.open(QIODevice::Append)) {
      QTextStream stream(&logFile);
      stream<< msg << Qt::endl;
    }
  }
  QMessageBox* msgbox;
  if (drift>0) {
     msgbox=new QMessageBox(QMessageBox::Question, tr("sctime: Programm was frozen"),
         tr("The program (or whole system) seems to have hung for %1min or system time was changed.\n"
                     "Should the time difference be added to the active account?\n"
                     "(current system time: %2)").arg(drift/60).arg(lastMinuteTick.toString()),
	       QMessageBox::Yes| QMessageBox::No, this);
  } else {
    msgbox=new QMessageBox(QMessageBox::Question, tr("sctime: system time set back"),
        tr("The system's time has been set back by %1min to %2."
                   "Should this time be subtracted from the active account?\n").arg(drift/60).arg(lastMinuteTick.toString()),
	       QMessageBox::Yes| QMessageBox::No, this);
  }
  connect(msgbox, &QMessageBox::finished,
  [=](){
    if (msgbox->result() == QMessageBox::Yes) {
       zeitKorrektur(drift, true);
       saveLater();
    }
    msgbox->deleteLater();
   });
  msgbox->open();
  msgbox->raise();     
}

/* Wird durch einen Timer einmal pro Minute aufgerufen,
und sorgt fuer die korrekte Aktualisierung der Objekte.
Da der Timer weiter laufen soll, muss sich diese Methode
beenden ohne zu blockieren.
*/
void TimeMainWindow::minuteHochzaehlen() {
  sekunden += 60;
  QString abt,ko,uko;
  int idx;
  QDateTime now = QDateTime::currentDateTime();
  int delta = lastMinuteTick.secsTo(now) - 60; // Abweichung seit letzter Minute
  lastMinuteTick = now;
  abtListToday->minuteVergangen(!pausedAbzur);
  // -> notwendig?
  abtListToday->getAktiv(abt,ko,uko,idx);
  kontoTree->refreshItem(abt,ko,uko,idx);
  auto pce=m_punchClockListToday->currentEntry();
  if (abtListToday->getDatum()==now.date()) {
     if (pce==m_punchClockListToday->end()) {
        pce=m_punchClockListToday->findEntryWithEnding(now.time().msecsSinceStartOfDay()/1000-120,now.time().msecsSinceStartOfDay()/1000);
     }
     if (pce!=m_punchClockListToday->end()) {
        pce->second=now.time().msecsSinceStartOfDay()/1000;
     } else {
        m_punchClockListToday->push_back(PunchClockEntry(now.time().msecsSinceStartOfDay()/1000,now.time().msecsSinceStartOfDay()/1000));
        m_punchClockListToday->setCurrentEntry(std::prev(m_punchClockListToday->end()));
     }
  }
  zeitChanged();
  // <- notwendig?
  if (lastMinuteTick.time().secsTo(QTime(0,2)) > 0) {
    tageswechsel();
  }
  if (abs(delta) >= 5)
      logError(tr("Minute-signal %1s arrived late (%2)").arg(delta).arg(now.toString()));
  QMetaObject::invokeMethod(this, "driftKorrektur", Qt::QueuedConnection);
}

void TimeMainWindow::tageswechsel() {
  if (abtList->getDatum().daysTo(lastMinuteTick.date()))
    emit changeTodaysDate(QDate::currentDate());
}

/**
  * Addiert timeIncrement auf die Zeiten des selktierten Unterkontos.
  */
void TimeMainWindow::addTimeInc()
{
  addDeltaToZeit(settings->timeIncrement());
}


/**
  * Subtrahiert timeIncrement von den Zeiten des selktierten Unterkontos.
  */
void TimeMainWindow::subTimeInc()
{
  addDeltaToZeit(-settings->timeIncrement());
}

/** subtracts one minute from the selected entry */
void TimeMainWindow::subMinimalTimeInc()
{
  addDeltaToZeit(-60);
}

/**
  * Addiert fastTimeIncrement auf die Zeiten des selktierten Unterkontos.
  */
void TimeMainWindow::addFastTimeInc()
{
  addDeltaToZeit(settings->fastTimeIncrement());
}


/**
  * Subtrahiert timeIncrement von den Zeiten des selktierten Unterkontos.
  */
void TimeMainWindow::subFastTimeInc()
{
  addDeltaToZeit(-settings->fastTimeIncrement());
}

/**
  * Addiert timeIncrement auf die Zeiten des selktierten Unterkontos.
  */
void TimeMainWindow::addAbzurTimeInc()
{
  addDeltaToZeit(settings->timeIncrement(), true);
}


/**
  * Subtrahiert timeIncrement von den Zeiten des selktierten Unterkontos.
  */
void TimeMainWindow::subAbzurTimeInc()
{
  addDeltaToZeit(-settings->timeIncrement(), true);
}


/**
  * Addiert fastTimeIncrement auf die Zeiten des selktierten Unterkontos.
  */
void TimeMainWindow::addFastAbzurTimeInc()
{
  addDeltaToZeit(settings->fastTimeIncrement(), true);
}

/**
  * Subtrahiert timeIncrement von den Zeiten des selktierten Unterkontos.
  */
void TimeMainWindow::subFastAbzurTimeInc()
{
  addDeltaToZeit(-settings->fastTimeIncrement(), true);
}

/**
  *  Addiert Delta Sekunden auf die Zeiten des selektierten Unterkontos.
  */
void TimeMainWindow::addDeltaToZeit(int delta, bool abzurOnly)
{
  QTreeWidgetItem * item=kontoTree->currentItem();

  if (!kontoTree->isEintragsItem(item)) return;

  QString uko,ko,abt,top;
  int idx;

  kontoTree->itemInfo(item,top,abt,ko,uko,idx);

  abtList->changeZeit(abt, ko, uko, idx, delta, abzurOnly);
  kontoTree->refreshItem(abt, ko, uko, idx);
  zeitChanged();
}

void TimeMainWindow::updateBreakTime()
{
  if (m_PCSToday->getDate()==abtList->getDatum()) {
     statusBar->setBreakTime(m_PCSToday->getBreaktimeThisWorkday());
  } else {
     statusBar->setBreakTime(-1);
  }
}

/**
 *  Bestimmt die veraenderte Gesamtzeit und loest die Signale gesamtZeitChanged und
 *  gesamtZeitAbzurChanged aus.
 */
void TimeMainWindow::zeitChanged()
{
  static PUNCHWARN lastwarn=PW_NONE;
  static QTime lastclocktime = QTime::currentTime();
  int zeitAbzur, workedtime;
  //int max_working_time=settings->maxWorkingTime();
  QTime clocktime = QTime::currentTime();
  abtList->getGesamtZeit(workedtime, zeitAbzur);
  statusBar->setDiff(abtList->getZeitDifferenz());
  emit gesamtZeitChanged(workedtime);
  emit gesamtZeitAbzurChanged(zeitAbzur);

  // remember the last clocktime for the next call, which might happen while
  // we do further stuff inside this function. Also remember the old lastworkedtime
  // in the local variable oldlast
  PUNCHWARN oldlastwarn=lastwarn;
  m_PCSToday->check(m_punchClockListToday, clocktime.msecsSinceStartOfDay()/1000, m_PCSYesterday);
  lastwarn=m_PCSToday->warnId;
  m_PCSToday->setDate(abtListToday->getDatum());
  updateBreakTime();

  settings->setCurrentPCCData(m_PCSToday->serialize());
  settings->setPreviousPCCData(m_PCSYesterday->serialize());

  // do the same for clocktime
  QTime oldlastclocktime = lastclocktime;
  lastclocktime = clocktime;

  // at first occurrence of a warning
  if ((lastwarn!=PW_NONE)&&(oldlastwarn!=lastwarn)) {
    // OK, QTimer erwartet nun, dass der letzte aufruf zurueckgekehrt ist, bevor
    // der nächste kommen kann. Da wir ueber einen QTimer aufgerufen wurden,
    // und wir weiter Tick-Events bekommen muessen, muessen wir den Arbeitszeitdialog asynchron starten.
    // Das tun wir ueber einen weiteren QTimer (das klappt, weil wir hier einen Wegwerftimer benutzen.
    QTimer::singleShot(3000, this, SLOT(showWorkdayWarning()));
  }
  if ((clocktime>settings->nightModeBegin())&&(oldlastclocktime<=settings->nightModeBegin())) {
    QTimer::singleShot(3000, this, SLOT(callNightTimeBeginDialog()));
  } else {
    foreach (QTime t, settings->nightModeAdditionalDialogTimes()) {
      if ((clocktime>t) && (oldlastclocktime<=t)) {
        QTimer::singleShot(5000, this, SLOT(callNightTimeBeginDialog()));
        break;
      }
    }
  }

  if ((clocktime>settings->nightModeEnd())&&(oldlastclocktime<=settings->nightModeEnd())) {
    QTimer::singleShot(3000, this, SLOT(callNightTimeEndDialog()));
  }

  saveLater();
    
#ifdef WIN32
	updateTaskbarTitle(workedtime);
#endif
}

#ifdef WIN32
/**
 * On OS Windows the taskbartitle will show the amount of workingtime
 * if sctime is minimized.
 * */
void TimeMainWindow::updateTaskbarTitle(int zeit)
{
	TimeCounter tc(zeit);
  QString text;
  text=tc.toString();
	setWindowIconText("sctime - " + text);
}
#endif

void TimeMainWindow::showWorkdayWarning() {
  QString warning=m_PCSToday->currentWarning;
  if ((warning!="")&&(settings->workingTimeWarnings())) {
     QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
          tr("Warning"),warning,QMessageBox::NoButton, this);
     connect(msgbox, &QMessageBox::finished, msgbox, &QMessageBox::deleteLater);
     msgbox->open();
     msgbox->raise();
  }
}

int TimeMainWindow::stopTimers(const QString& grund) {
    minutenTimer->stop();
    autosavetimer->stop();
    restTimer->stop();
    // die Sekunden seit dem letzten Tick speichern
    QDateTime now = QDateTime::currentDateTime();
    int secSeitTick = lastMinuteTick.secsTo(now);
    lastMinuteTick = now;
    emit save();
    trace(tr("%1: Accounting stopped (%2, +%3s)").arg(grund, now.toString()).arg(secSeitTick));
    return secSeitTick;
}


/** Ruft einen modalen Dialog auf, der eine Pause anzeigt, und setzt gleichzeitig
  *  paused auf true, um die Zeiten anzuhalten
  */
void TimeMainWindow::pause() {
    int drift =  sekunden - startTime.secsTo(lastMinuteTick);
    paused = true;
    int secSinceTick = stopTimers(tr("Pause"));
    if (secSinceTick > 60 || secSinceTick < 0) {
        trace(tr("ERROR: seconds since tick: %1").arg(secSinceTick));
        secSinceTick = 60;
    }
    settings->setLastRecordedTimestamp(lastMinuteTick);
    qApp->setWindowIcon(pausedWindowIcon);
    setWindowIcon(pausedWindowIcon);
    QDateTime now = QDateTime::currentDateTime();
    QString currtime= QLocale().toString(now.time(), QLocale::ShortFormat);
    auto pce=m_punchClockListToday->currentEntry();
    if ((now.date()==abtListToday->getDatum()) && (pce!=m_punchClockListToday->end())) {
      pce->second=now.time().msecsSinceStartOfDay()/1000;
    }
    PauseDialog* pd = new PauseDialog(now, drift, secSinceTick, this);
    
    connect(pd,&PauseDialog::pauseHasEnded, this, &TimeMainWindow::continueAfterPause);
    connect(pd, &PauseDialog::updateEvent,this, &TimeMainWindow::updateBreakTime);
    connect(pd, &PauseDialog::updateEvent,this, &TimeMainWindow::keepAlive);
    saveLater();
    pd->startPause();
}

void TimeMainWindow::keepAlive() {
#ifdef __EMSCRIPTEN__
    static int counter=0;
    counter++;
    if (counter > 60) {
        counter = 0;
        // send a get request to the server to keep the session alive
        QNetworkRequest request(QUrl(getStaticUrl() + "/"+KEEPALIVE_URL_PART));
        networkAccessManager->get(request);
    }
#endif
}

void TimeMainWindow::continueAfterPause(int drift, int secSinceTick) {
    sender()->deleteLater();
    paused = false;
    QDateTime now = QDateTime::currentDateTime();
    sekunden = drift;
    trace(tr("End of break: ") +now.toString());

    if (now.date()==abtListToday->getDatum()) {
      m_punchClockListToday->push_back(PunchClockEntry(now.time().msecsSinceStartOfDay()/1000,now.time().msecsSinceStartOfDay()/1000));
      m_punchClockListToday->setCurrentEntry(std::prev(m_punchClockListToday->end()));
    }

    qApp->setWindowIcon(windowIcon);
    setWindowIcon(windowIcon);
    autosavetimer->start();
    // Tricks wegen der vor der Pause angebrochenen Minute
    restTimer->start((60 - secSinceTick) * 1000);
    startTime = now.addSecs(-secSinceTick);
    lastMinuteTick = startTime;
    tageswechsel();
    updateSpecialModes(true);
}


/**
 * Setzt, ob die abzurechnende Zeit pausiert werden soll.
 */
void TimeMainWindow::pauseAbzur(bool on)
{
  pausedAbzur=on;
}


/**
 * Speichert die aktuellen Zeiten und Einstellungen
 */
void TimeMainWindow::saveWithTimeout(int conflicttimeout)
{
  if (kontoTree!=NULL) {
    kontoTree->flagClosedPersoenlicheItems();
    std::vector<int> columnwidthlist;
    kontoTree->getColumnWidthList(columnwidthlist);
    settings->setColumnWidthList(columnwidthlist);
  }
  settings->setLastRecordedTimestamp(lastMinuteTick);
  m_PCSToday->check(m_punchClockListToday, QTime::currentTime().msecsSinceStartOfDay()/1000, m_PCSYesterday);
  m_PCSToday->setDate(abtListToday->getDatum());
  settings->setCurrentPCCData(m_PCSToday->serialize());
  settings->setPreviousPCCData(m_PCSYesterday->serialize());
  settings->setMainWindowGeometry(pos(),size());
  if (checkConfigDir()) {
    checkLock();
    XMLWriter* writer=new XMLWriter(settings, networkAccessManager, abtListToday, m_punchClockListToday, conflicttimeout);
    connect(writer, &XMLWriter::settingsWritten, writer, &XMLWriter::deleteLater);
    connect(writer, &XMLWriter::settingsWriteFailed, writer, &XMLWriter::deleteLater);
    connect(writer, &XMLWriter::offlineSwitched, this, &TimeMainWindow::switchRestCurrentlyOffline);
    connect(writer, &XMLWriter::unauthorized, this, &TimeMainWindow::sessionInvalid);
    connect(writer, &XMLWriter::conflicted, this, &TimeMainWindow::writeConflictDialog, Qt::QueuedConnection);
    settings->writeShellSkript(abtListToday, m_punchClockListToday);
    writer->writeAllSettings();
    if (abtList!=abtListToday) {
      writer=new XMLWriter(settings, networkAccessManager, abtList, m_punchClockList, conflicttimeout);
      connect(writer, &XMLWriter::settingsWritten, writer, &XMLWriter::deleteLater);
      connect(writer, &XMLWriter::offlineSwitched, this, &TimeMainWindow::switchRestCurrentlyOffline);
      connect(writer, &XMLWriter::settingsWriteFailed, writer, &XMLWriter::deleteLater);
      connect(writer, &XMLWriter::unauthorized, this, &TimeMainWindow::sessionInvalid);
      connect(writer, &XMLWriter::conflicted, this, &TimeMainWindow::writeConflictDialog, Qt::QueuedConnection);
      settings->writeShellSkript(abtList, m_punchClockList);
      writer->writeAllSettings();
    }
  }
}

void TimeMainWindow::save() {
  saveWithTimeout(150);
// for wasm also sync
#ifdef __EMSCRIPTEN__
  if (!settings->restCurrentlyOffline()&&!settings->restSaveOffline()&&m_conflictDialogOpenForDates.isEmpty()) {
    syncAll();
  }
#endif
}

bool TimeMainWindow::checkConfigDir() {
  bool result = configDir.exists();
  if (!result) {
    QTimer::singleShot(0, this, SLOT(callCantSaveDialog()));
  }
  return result;
}

void TimeMainWindow::callCantSaveDialog() {
  if (cantSaveDialog==NULL) {
    QMessageBox* msg = new QMessageBox(this);
    cantSaveDialog=msg;
    msg->setWindowTitle("Error on saving");
    msg->setText(tr("An error occured when saving data. Please check permissions and connectivity of your target directory. If this error persists and you close sctime, you will loose all changes since the last successful save (an automatic save should occur every 5 minutes)."));
    msg->setIcon(QMessageBox::Warning);
    msg->setStandardButtons(QMessageBox::Close);
    msg->setModal(true);
    qDebug() << tr("An error occured when saving data.");
    connect(msg, &QMessageBox::finished, this, &TimeMainWindow::finishCantSaveDialog);
    msg->open();
    msg->raise();
  }
}

void TimeMainWindow::finishCantSaveDialog() {
  cantSaveDialog->deleteLater();
  cantSaveDialog=NULL;
}

void TimeMainWindow::checkLock() {
  if (m_lock->check()==LS_CONFLICT) {
    QMessageBox* msg = new QMessageBox(this);
    msg->setText(tr("The program will quit in a few seconds without saving."));
    msg->setInformativeText(m_lock->errorString());
    msg->setModal(true);
    qDebug() << tr("The program will now quit without saving.") << m_lock->errorString();
    QTimer::singleShot(10000, this, SLOT(quit()));
    connect(msg, &QMessageBox::finished, msg, &QMessageBox::deleteLater);
    msg->open();
    msg->raise();
    return;
  }
  if (m_lock->check()!=LS_OK) {
    QMessageBox* msg = new QMessageBox(this);
    msg->setText(tr("Unclear state of Lockfile. Please check that there is no other instance of sctime running and that you have access to the sctime config directory. Otherwise loss of data may occur."));
    msg->setInformativeText(m_lock->errorString());
    msg->setModal(true);
    connect(msg, &QMessageBox::finished, msg, &QMessageBox::deleteLater);
    qDebug() << tr("Unkown state of lockfile.") << m_lock->errorString();
    msg->open();
    msg->raise();
  }
}

void TimeMainWindow::quit() {
  exit(1);
}

/**
 * Loest ein callUnterKontoDialog Signal mit dem selektierten Item auf
 */
void TimeMainWindow::editUnterKontoPressed()
{
  emit callUnterKontoDialog(kontoTree->currentItem());
}

void TimeMainWindow::editBereitschaftPressed()
{
  emit callBereitschaftsDialog(kontoTree->currentItem());
}

void TimeMainWindow::specialRemunPressed()
{
  emit callSpecialRemunerationsDialog(kontoTree->currentItem());
}

/**
 * Copies a new Entry as text to the clipboard
 */
void TimeMainWindow::copyEntryAsText()
{
  QTreeWidgetItem * item=kontoTree->currentItem();
  QString top,uko,ko,abt;
  int idx;
  kontoTree->itemInfo(item,top,abt,ko,uko,idx);
  QString text;
  text=abt+"|"+ko+"|"+uko+"|";
  if (kontoTree->isEintragsItem(item)) {
        UnterKontoEintrag eintrag;
        abtList->getEintrag(eintrag,abt,ko,uko,idx);
        QTextStream(&text) << roundTo(1.0/3600*eintrag.sekunden,0.01) << "|" << roundTo(1.0/3600*eintrag.sekundenAbzur,0.01) << "|" << eintrag.kommentar ;
  }
  QApplication::clipboard()->setText(text, QClipboard::Clipboard);
}

/**
 * Copies a new Entry as link to the clipboard
 */
void TimeMainWindow::copyEntryAsLink()
{
  QTreeWidgetItem * item=kontoTree->currentItem();
  QString top,uko,ko,abt;
  int idx;
  kontoTree->itemInfo(item,top,abt,ko,uko,idx);
  QString text;
  text="sctime://local/"+QUrl::toPercentEncoding(abt)+"/"+QUrl::toPercentEncoding(ko)+"/"+QUrl::toPercentEncoding(uko);
  if (kontoTree->isEintragsItem(item)) {
        UnterKontoEintrag eintrag;
        abtList->getEintrag(eintrag,abt,ko,uko,idx);
        if (eintrag.kommentar!="") {
            QTextStream(&text) << "?comment=" + QUrl::toPercentEncoding(eintrag.kommentar);
        }
  }
  QApplication::clipboard()->setText(text, QClipboard::Clipboard);
}

/**
 * Copies a link from the clipboard and opens it
 */
void TimeMainWindow::pasteEntryAsLink()
{
  QString text=QApplication::clipboard()->text(QClipboard::Clipboard);
  QUrl url=QUrl(text);
  openEntryLink(QUrl(text));
}

/**
 * opens a link to an entry
 */
void TimeMainWindow::openEntryLink(const QUrl& url)
{
  if (url.scheme()=="sctime") {
     QStringList pathlist=url.path().split("/");
     if (pathlist.size()==0) {
      return;
     }
     pathlist[0]=ALLE_KONTEN_STRING;
     if (url.hasQuery()) {
       QString comment=QUrlQuery(url.query()).queryItemValue("comment",QUrl::FullyDecoded);
       if (comment!="") {
           pathlist<<comment;
       }
     }
     openItemFromPathList(pathlist);
  }
}

/**
 * Aktiviert einen Eintrag
 */
void TimeMainWindow::eintragAktivieren()
{
  QTreeWidgetItem * item=kontoTree->currentItem();
  setAktivesProjekt(item);
}


/**
 * Fuegt einen Eintrag zum selektierten Unterkonto hinzu.
 */
void TimeMainWindow::eintragHinzufuegen()
{
  QTreeWidgetItem * item=kontoTree->currentItem();

  if ((!kontoTree->isEintragsItem(item))&&(!kontoTree->isUnterkontoItem(item))) return;

  QString top,uko,ko,abt;
  int oldidx;

  kontoTree->itemInfo(item,top,abt,ko,uko,oldidx);

  int idx=abtList->insertEintrag(abt,ko,uko);

  abtList->setEintragFlags(abt,ko,uko,idx,abtList->getEintragFlags(abt,ko,uko,oldidx));

  kontoTree->refreshAllItemsInUnterkonto(abt,ko,uko);
  changeShortCutSettings(item);
  saveLater();
}

/**
 * Entfernt einen Eintrag.
 */
void TimeMainWindow::eintragEntfernen()
{
  QTreeWidgetItem * item=kontoTree->currentItem();

  if ((!item)||(kontoTree->getItemDepth(item)!=4)) return;

  QString top,uko,ko,abt;
  int idx;

  kontoTree->itemInfo(item,top,abt,ko,uko,idx);

  KontoTreeItem *topi, *abti, *koi, *ukoi, *eti;

  if (abtList->isAktiv(abt,ko,uko,idx)) {
      QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
         tr("Warning"), tr("Cannot delete active entry"),
         QMessageBox::Ok, this);
      connect(msgbox, &QMessageBox::finished, msgbox, &QMessageBox::deleteLater);
      msgbox->open();
      msgbox->raise();
      return;
  }

  abtList->setSekunden(abt,ko,uko,idx,0); // Explizit vorher auf Null setzen, um die Gesamtzeit
                                          // nicht zu verwirren.
  abtList->deleteEintrag(abt,ko,uko,idx);

  kontoTree->sucheItem(PERSOENLICHE_KONTEN_STRING,abt,ko,uko,idx,topi,abti,koi,ukoi,eti);
  delete ukoi;

  kontoTree->sucheItem(ALLE_KONTEN_STRING,abt,ko,uko,idx,topi,abti,koi,ukoi,eti);
  delete ukoi;

  kontoTree->refreshAllItemsInUnterkonto(abt,ko,uko);
  int etiCurrent = 0;
  if (kontoTree->sucheItem(top,abt,ko,uko,idx,topi,abti,koi,ukoi,eti)) {
      for (eti=(KontoTreeItem*)(ukoi->child(0));
           (eti!=NULL)&&(eti->text(KontoTreeItem::COL_ACCOUNTS).toInt()<=idx);
           eti=(KontoTreeItem*)(eti->child(etiCurrent+=1))) ;
      if (eti!=NULL)
          kontoTree->setCurrentItem(eti);
      else
          kontoTree->setCurrentItem(ukoi);
  }
  zeitChanged();
}

void TimeMainWindow::callSwitchDateErrorDialog()
{
    QMessageBox *msg=new QMessageBox(this);
    QString msgtext = tr("Could not switch day due to problems with saving. ATTENTION: that also means that the clock might be running on the wrong day. Please fix the problem with saving and switch manually to the current date afterwards.");
    msg->setText(msgtext);
    qDebug() << msgtext;
    connect(msg, &QMessageBox::finished, msg, &QMessageBox::deleteLater);
    msg->open();
    msg->raise();
}

void TimeMainWindow::loadPCCData(const QString& pccdata) {
#if PUNCHCLOCKDE23
   PunchClockStateDE23 pcs;
#else
   PunchClockStateNoop pcs;
#endif
   pcs.deserialize(pccdata);
   if (pcs.getDate()==abtListToday->getDatum()) {
      m_PCSToday->copyFrom(&pcs);
   } else if (pcs.getDate()==abtListToday->getDatum().addDays(-1)) {
      m_PCSYesterday->copyFrom(&pcs);
   }
}

/**
 * Aendert das Datum: dazu werden zuerst die aktuellen Zeiten und Einstellungen gespeichert,
 * sodann die Daten fuer das angegebene Datum neu eingelesen.
 */
void TimeMainWindow::changeDate(QDate datum, bool changeVisible, bool changeToday)
{
  if (checkConfigDir()||m_dateChanger!=NULL)
  {
    checkLock();

    kontoTree->flagClosedPersoenlicheItems();
    std::vector<int> columnwidthlist;
    kontoTree->getColumnWidthList(columnwidthlist);
    settings->setColumnWidthList(columnwidthlist);
    m_dateChanger = new DateChanger(this, networkAccessManager, datum, changeVisible, changeToday);
    connect(m_dateChanger, &DateChanger::finished, this, &TimeMainWindow::changeDateFinished);
    connect(m_dateChanger, &DateChanger::offlineSwitched, this, &TimeMainWindow::switchRestCurrentlyOffline);
    m_dateChanger->start();
  }
  else
  {
    callSwitchDateErrorDialog();
  }
}

void TimeMainWindow::changeDateFinished(const QDate &date, bool changeVisible, bool changeToday, bool currentDateSel)
{
  kontoTree->load(abtList);
  kontoTree->closeFlaggedPersoenlicheItems();
  kontoTree->showAktivesProjekt();
  if (changeToday)
  {
    // reset data for yesterday - this ensures that we do not use old data even if we cannot load PCDAta for this day
    m_PCSYesterday->reset();
    // load all existing pcc data. This may or may not contain today or yesterday's data.
    loadPCCData(settings->previousPCCData());
    loadPCCData(settings->currentPCCData());
    // update today's PCC data
    m_PCSToday->check(m_punchClockListToday, QTime::currentTime().msecsSinceStartOfDay() / 1000, m_PCSYesterday);
    m_PCSToday->setDate(abtListToday->getDatum());

    updateSpecialModes(false);
  }
  zeitChanged();
  emit(currentDateSelected(currentDateSel));
  statusBar->dateWarning((abtList != abtListToday), abtList->getDatum());
  if (changeVisible)
  {
    trace(tr("Visible day set to: ") + date.toString());
  }
  if (changeToday)
  {
    trace(tr("Today is now: ") + date.toString());
  }
  // Append Warning if current file is checked in
  if (!currentDateSel)
  {
    if (abtList->checkInState())
    {
      statusBar->appendWarning(!currentDateSel, tr(" -- This day has already been checked in!"));
    }
  }
  m_dateChanger->deleteLater();
  m_dateChanger=NULL;
}

// changes the visible date and updates todays date if we are not on the current date
void TimeMainWindow::changeVisibleDate(const QDate &date) {
  changeDate(date, true, true);
}

/* changes today's date (because e.g. we are past midnight)
   implictly changes the visible date, iff it happens to be the same as the previous todays date, and no one is editig something at the moment*/
void TimeMainWindow::changeTodaysDate(const QDate &date) {
  changeDate(date, false, true);
}

void TimeMainWindow::refreshKontoListe() {
#ifndef WASMQUIRKS
  statusBar->showMessage(tr("Reading account list..."));
#endif
  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
  QTimer::singleShot(100, m_dsm->kontenDSM, SLOT(start()));
}

void TimeMainWindow::commitKontenliste(DSResult data) {
#ifndef WASMQUIRKS
  statusBar->showMessage(tr("Commiting account list..."));
#endif
  auto commiter=new AccountListCommiter(this, networkAccessManager,data, settings,kontoTree,abtList,abtListToday, m_punchClockList, !initCompleted);
  
  if (checkConfigDir()) {
    checkLock();
    connect(commiter, &AccountListCommiter::finished, this, &TimeMainWindow::commitKontenlisteFinished );
    connect(commiter, &AccountListCommiter::finished, commiter, &AccountListCommiter::deleteLater);
    commiter->start();
  } else {
    commiter->deleteLater();
    QApplication::restoreOverrideCursor();
  }
}

void TimeMainWindow::commitKontenlisteFinished() {
#ifndef WASMQUIRKS
  statusBar->showMessage(tr("Account list successfully read."), 2000);
#endif
  QApplication::restoreOverrideCursor();
  QMetaObject::invokeMethod(this, "aktivesKontoPruefen", Qt::QueuedConnection);
  emit accountListRead();
  if (!m_afterCommitMethodQueue->isEmpty()) {
     auto method = m_afterCommitMethodQueue->dequeue();
     QMetaObject::invokeMethod(method->obj, method->method, Qt::QueuedConnection);
     delete method;
  }
}


/**
 * Fuegt das aktuell selektierte Unterkonto den Persoenlichen Konten hinzu.
 * Falls kein Unterkonto selektiert oder inPersoenlicheKonten==false ist, passiert nichts.
 */
void TimeMainWindow::inPersoenlicheKonten(bool hinzufuegen)
{

  if (!inPersoenlicheKontenAllowed) return;

  QTreeWidgetItem * item=kontoTree->currentItem();

  if (!item) return;

  QString uko,ko,abt,top;
  int idx;

  kontoTree->itemInfo(item,top,abt,ko,uko,idx);
  int itemDepth=kontoTree->getItemDepth(item);

  auto doChange = [=](){
    if (itemDepth == 2)
    {
      abtList->moveKontoPersoenlich(abt, ko, hinzufuegen);
      kontoTree->refreshAllItemsInKonto(abt, ko);
      return;
    }
    else
    {
      if (itemDepth == 3)
      {
        abtList->moveUnterKontoPersoenlich(abt, ko, uko, hinzufuegen);
        kontoTree->refreshAllItemsInUnterkonto(abt, ko, uko);
        return;
      }
    }

    abtList->moveEintragPersoenlich(abt, ko, uko, idx, hinzufuegen);
    kontoTree->refreshItem(abt, ko, uko, idx);
    saveLater();
  };

  if (!hinzufuegen) {
    auto msgbox=new QMessageBox(QMessageBox::Question, tr("Remove from personal accounts"),
        tr("Do you really want to remove this item from your personal accounts?"),
	      QMessageBox::Yes| QMessageBox::No, this);
    connect(msgbox, &QMessageBox::finished,
       [=](){
         if (msgbox->result() != QMessageBox::Yes)
            flagsChanged(abt,ko,uko,idx);
         else
            doChange();
         msgbox->deleteLater();
       });
    msgbox->open();
    msgbox->raise();
  } else {
    doChange();
  }
}


/**
 * Aendert die Einstellungen fuer die Menueshortcuts entsprechend dem selektierten Item
 */
void TimeMainWindow::changeShortCutSettings(QTreeWidgetItem * item)
{
  bool iseintragsitem=kontoTree->isEintragsItem(item);
  bool isUnterkontoItem=kontoTree->isUnterkontoItem(item);
  inPersoenlicheKontenAllowed=false; //Vorsorglich disablen, sonst Seiteneffekte mit flagsChanged.
  inPersKontAction->setEnabled(false);

  QString uko,ko,abt;
  QString top=""; // top wird weiter unten ausgelesen, und es ist nicht sicher, ob es initialisiert wurde.
  int idx;

  if (item) kontoTree->itemInfo(item,top,abt,ko,uko,idx);

  emit unterkontoSelected(isUnterkontoItem);

  int depth=-1;
  if (item)
    depth=kontoTree->getItemDepth(item);

  onCallAction->setEnabled(isUnterkontoItem && !abtList->checkInState() && abtList->getDescription(abt,ko,uko).supportsOnCallTimes());

  if (iseintragsitem) {

    if ((depth<=3)||(item->parent()->childCount()<=1))
       eintragRemoveAction->setEnabled(false);
    else {
       eintragRemoveAction->setEnabled(true);
    }

    flagsChanged(abt,ko,uko,idx);
    inPersKontAction->setEnabled(!abtList->checkInState());
    editUnterKontoAction->setEnabled(!abtList->checkInState());

    specialRemunAction->setEnabled(!abtList->checkInState() && abtList->getDescription(abt,ko,uko).supportsSpecialRemuneration());
    /* Eigentlich sollte das Signal in editierbarerEintragSelected umbenannt werden... */
    emit eintragSelected(!abtList->checkInState());
    emit augmentableItemSelected(!abtList->checkInState());
    if (abtListToday==abtList)
      emit aktivierbarerEintragSelected(!abtList->checkInState());
  }
  else {
    // Auch bei Konten und Unterkonten in Pers. Konten PersKontAction auf On stellen.
    inPersKontAction->setChecked((item&&(top==PERSOENLICHE_KONTEN_STRING)&&(kontoTree->getItemDepth(item)>=2)&&(kontoTree->getItemDepth(item)<=3)));
    inPersKontAction->setEnabled((!abtList->checkInState())&&(item&&(kontoTree->getItemDepth(item)>=2)&&(kontoTree->getItemDepth(item)<=3)));
    editUnterKontoAction->setEnabled(false);
    specialRemunAction->setEnabled(false);
    emit eintragSelected(false);
    emit augmentableItemSelected(!abtList->checkInState()&&isUnterkontoItem);
    emit aktivierbarerEintragSelected(false);
    eintragRemoveAction->setEnabled(false);
  }
  bool customColorIsAllowed = (depth>=1)&&(depth<=3);
  bgColorChooseAction->setEnabled(customColorIsAllowed);
  bgColorRemoveAction->setEnabled(customColorIsAllowed);
  jumpAction->setEnabled((top!=ALLE_KONTEN_STRING)&&(depth>=1));
  inPersoenlicheKontenAllowed=true; // Wieder enablen.
}

void TimeMainWindow::updateCaption()
{
   QString abt, ko, uko;
   int idx;
   abtList->getAktiv(abt,ko,uko,idx);
   setWindowTitle(tr("sctime - ")+ abt+"/"+ko+"/"+uko);
}

void TimeMainWindow::resetDiff()
{
#ifndef DISABLE_PUNCHCLOCK
  int diff = abtList->getZeitDifferenz();
  int currentintlen = m_punchClockListToday->currentEntry()->second - m_punchClockListToday->currentEntry()->first;
  if ((diff == 0) || (abtListToday != abtList) || (-diff > currentintlen)) {
#endif
    abtList->setZeitDifferenz(0);
    zeitChanged();
#ifndef DISABLE_PUNCHCLOCK
  } else {
    QString msg;
    if (diff > 0)
    {
      msg = tr("Do you also want to move the begin of the current working intervall by %1 minutes?").arg(abs(diff / 60));
    }
    else
    {
      msg = tr("Do you also want to add a pause of %1 minutes at the end of the current working interval?").arg(abs(diff / 60));
    }
    auto msgbox = new QMessageBox(
        QMessageBox::Question,
        tr("Also adapt punch clock?"),
        msg, QMessageBox::Yes | QMessageBox::No, this);
    connect(msgbox, &QMessageBox::finished,
            [=]()
            {
              auto reply = msgbox->result();
              if (reply == QMessageBox::Yes)
              {
                auto currententry = m_punchClockListToday->currentEntry();
                if (diff > 0)
                {
                  int newbegin = currententry->first - diff;
                  if (newbegin < 0)
                  {
                    newbegin = 0;
                  }
                  currententry->first = newbegin;
                }
                else
                {
                  int newend = currententry->second + diff;
                  if (newend < currententry->first)
                  {
                    newend = currententry->first;
                  }
                  auto now = QTime::currentTime();
                  m_punchClockListToday->push_back(PunchClockEntry(currententry->second, std::max(now.msecsSinceStartOfDay() / 1000, currententry->second)));
                  m_punchClockListToday->setCurrentEntry(std::prev(m_punchClockListToday->end()));
                  currententry->second = newend;
                }
              }
              abtList->setZeitDifferenz(0);
              zeitChanged();
              msgbox->deleteLater();
            });
    msgbox->open();
    msgbox->raise();
  }
#endif
}

/**
 * Sollte aufgerufen werden, sobald sich die Einstellungen fuer ein Konto aendern.
 * Toggelt zB inPersKontAction.
 */
void TimeMainWindow::flagsChanged(const QString& abt, const QString& ko, const QString& uko, int idx)
{

  QTreeWidgetItem * item=kontoTree->currentItem();

  if (!item) return;

  QString selecteduko,selectedko,selectedabt,selectedtop;
  int selectedidx;

  kontoTree->itemInfo(item,selectedtop,selectedabt,selectedko,selecteduko,selectedidx);
  if ((selectedabt==abt)&&(selectedko==ko)&&(selecteduko==uko)&&(selectedidx==idx)) {
    inPersKontAction->setChecked((abtList->getEintragFlags(abt,ko,uko,idx)&UK_PERSOENLICH)&&(!abtList->checkInState()));
  }

  updateCaption();
}

void TimeMainWindow::resizeToIfSensible(QDialog* dialog, const QPoint& pos, const QSize& size)
{
  if (size.isNull())
  {
    return;
  }
  QRect screenGeometry(QGuiApplication::primaryScreen()->availableGeometry());
  QPoint pos2=QPoint(pos.x()+size.width(), pos.y()+size.height());
  if (screenGeometry.contains(pos)&&screenGeometry.contains(pos2))
  {
    dialog->resize(size);
    dialog->move(pos);
  } 
}

void TimeMainWindow::showContextMenu(const QPoint &pos)
{
      // no context menu in single click activation mode
      if (settings->singleClickActivation()) {
         return;
      }
      QTreeWidgetItem *item = kontoTree->itemAt(pos);
      if (!item || kontoTree->isEintragsItem(item))
         return;
 
      int col=kontoTree->columnAt(pos.x());
      int d=kontoTree->getItemDepth(item);

      if ((d==2)&&(col==KontoTreeItem::COL_ACCOUNTS)) {
         QMenu* menu =new QMenu(tr("Account menu"), this);
         menu->addAction(copyAction);
         menu->addAction(copyLinkAction);
         menu->addAction(inPersKontAction);
         connect(menu,&QMenu::aboutToHide, menu, &QMenu::deleteLater);
         menu->popup(kontoTree->viewport()->mapToGlobal(pos));
      }
}

/**
 * Erzeugt einen UnterkontoDialog fuer item.
 */
void TimeMainWindow::callUnterKontoDialog(QTreeWidgetItem * item)
{
  if ((!kontoTree->isEintragsItem(item)))
    return;
  entryBeingEdited = true;

  QString top,uko,ko,abt;

  int idx;

  kontoTree->itemInfo(item,top,abt,ko,uko,idx);

  m_unterKontoDialog=new UnterKontoDialog(abt,ko,uko,idx,abtList,&defaultTags, true, settings, this, abtList->checkInState());
  connect(m_unterKontoDialog, SIGNAL(entryChanged(const QString&, const QString&, const QString&, int )), kontoTree,
  SLOT(refreshItem(const QString&, const QString&, const QString&,int )));
  connect(m_unterKontoDialog, SIGNAL(entryChanged(const QString&, const QString&, const QString&, int )), this,
  SLOT(checkComment(const QString&, const QString&, const QString&,int )));
  connect(m_unterKontoDialog, SIGNAL(entryChanged(const QString&, const QString&, const QString&, int)), this, SLOT(zeitChanged()));
  connect(m_unterKontoDialog, SIGNAL(entryChanged(const QString&, const QString&, const QString&, int)),
           this, SLOT(flagsChanged(const QString&, const QString&, const QString&,int)));
  connect(m_unterKontoDialog, SIGNAL(entryActivated()), this, SLOT(eintragAktivieren()));
  connect(m_unterKontoDialog, SIGNAL(bereitschaftChanged(const QString&, const QString&, const QString&)),
          kontoTree, SLOT(refreshAllItemsInUnterkonto(const QString&, const QString&, const QString&)));
  if (abtList->isAktiv(abt,ko,uko,idx) && (abtList->getDatum()==QDate::currentDate()))
    m_unterKontoDialogTimerConnection=connect(minutenTimer, SIGNAL(timeout()),m_unterKontoDialog->getZeitBox(),SLOT(incrMin()));
  else
    m_unterKontoDialogTimerConnection=QMetaObject::Connection();

  QPoint pos;
  QSize size;
  settings->getUnterKontoWindowGeometry(pos, size);
  resizeToIfSensible(m_unterKontoDialog,pos,size);
  connect(m_unterKontoDialog, SIGNAL(finished(int)), this, SLOT(cleanupUnterKontoDialog(int)));
  m_unterKontoDialog->open();
  m_unterKontoDialog->raise();
  m_unterKontoDialog->commentwidget->setFocus();
}

void TimeMainWindow::cleanupUnterKontoDialog(int result)
{
  if (result==QDialog::Accepted) {
    saveLater();
  }
  QObject::disconnect(m_unterKontoDialogTimerConnection);
  m_unterKontoDialog->deleteLater();
  m_unterKontoDialog=NULL;
  entryBeingEdited = false;
}

void TimeMainWindow::openItemFromPathList(QStringList pathlist)
{
  if (pathlist.size() > 0)
  {
    // Konto was searched
    if (pathlist.size() == 3)
    {
      QTreeWidgetItem *item = kontoTree->sucheKontoItem(pathlist.at(0),
                                                        pathlist.at(1), pathlist.at(2));
      openItem(item);
    }
    // Unterkonto was searched
    if (pathlist.size() == 4)
    {
      QTreeWidgetItem *item = kontoTree->sucheUnterKontoItem(
          pathlist.at(0), pathlist.at(1), pathlist.at(2), pathlist.at(3));
      openItem(item);
    }
    // Kommentar was searched
    if (pathlist.size() == 5)
    {
      QTreeWidgetItem *item = kontoTree->sucheKommentarItem(
          pathlist.at(0), pathlist.at(1), pathlist.at(2),
          pathlist.at(3), pathlist.at(4));
      // in case we didnt find the comment (e.g. because it was just a default comment), at least open the subaccount
      if (item == NULL)
      {
        item = kontoTree->sucheUnterKontoItem(
            pathlist.at(0), pathlist.at(1), pathlist.at(2), pathlist.at(3));
      }
      openItem(item);
    }
  }
}

/**
 * Baut den Kontosuchdialog auf, und zeigt das Such-Ergebnis an.
 */
void TimeMainWindow::callFindKontoDialog()
{
  QString konto;

  FindKontoDialog* findKontoDialog=new FindKontoDialog(abtList,this);
  connect(findKontoDialog,&FindKontoDialog::finished, this, &TimeMainWindow::finishFindKontoDialog);
  findKontoDialog->open();
  findKontoDialog->raise();
}

void TimeMainWindow::finishFindKontoDialog() 
{
  auto findKontoDialog = (FindKontoDialog*)(sender());
  int rcFindDialog = findKontoDialog->result();
  if( rcFindDialog == QDialog::Rejected )
  {
  } else if( rcFindDialog == QDialog::Accepted )
  {
    QStringList items = findKontoDialog->getSelectedItems();
    openItemFromPathList(items);
  }
  findKontoDialog->deleteLater();
}

void TimeMainWindow::openItem( QTreeWidgetItem *item )
{
  if (item)
  {
    kontoTree->setCurrentItem(item);
  }
}

void TimeMainWindow::callPreferenceDialog()
{
  bool oldshowtypecolumn = settings->showTypeColumn();
  bool oldshowpspcolumn = settings->showPSPColumn();
  SCTimeXMLSettings::DefCommentDisplayModeEnum olddisplaymode = settings->defCommentDisplayMode();

  /* FIXME: no idea, why we need to do that, but otherwise QLabels and
   * QPushButtons in the preference dialog won't use the custom font (at least
   * on the Mac with Qt 4.8.0) */
  if (settings->useCustomFont())
  {
    QString custFont=settings->customFont();
    int custFontSize=settings->customFontSize();
    QApplication::setFont(QFont(custFont,custFontSize));
  }

  PreferenceDialog *preferenceDialog=new PreferenceDialog(settings, oldshowtypecolumn, oldshowpspcolumn, olddisplaymode, this);
  connect(preferenceDialog,&PreferenceDialog::finishedWithInfo, this, &TimeMainWindow::finishPreferenceDialog);
  preferenceDialog->open();
  preferenceDialog->raise();
}

void TimeMainWindow::finishPreferenceDialog(int oldshowtypecolumn, int oldshowpspcolumn, int olddisplaymode) {
  sender()->deleteLater();
  showAdditionalButtons(settings->powerUserView());
  configClickMode(settings->singleClickActivation());
  switchRestCurrentlyOffline(settings->restCurrentlyOffline());
  kontoTree->setAcceptDrops(settings->dragNDrop());
  if (settings->useCustomFont()) {
    QApplication::setFont(QFont(settings->customFont(),settings->customFontSize()));
  }
  else
  {
    QApplication::setFont(qtDefaultFont);
  }
  kontoTree->showPersoenlicheKontenSummenzeit(settings->persoenlicheKontensumme());
  if (!settings->showTypeColumn()) {
    kontoTree->hideColumn(KontoTreeItem::COL_TYPE);
  } else
  {
    if (!oldshowtypecolumn) {
      kontoTree->showColumn(KontoTreeItem::COL_TYPE);
      kontoTree->resizeColumnToContents(KontoTreeItem::COL_TYPE);
    }
  }
  if (!settings->showPSPColumn()) {
    kontoTree->hideColumn(KontoTreeItem::COL_PSP);
  } else
  {
    if (!oldshowpspcolumn) {
      kontoTree->showColumn(KontoTreeItem::COL_PSP);
      kontoTree->resizeColumnToContents(KontoTreeItem::COL_PSP);
    }
  }
  kontoTree->flagClosedPersoenlicheItems();
  if (settings->defCommentDisplayMode()!=olddisplaymode) {
      kontoTree->setDisplayMode(settings->defCommentDisplayMode());
  }
  kontoTree->setSortByCommentText(settings->sortByCommentText());
  kontoTree->closeFlaggedPersoenlicheItems();
  saveLater();
  
}

/**
 * Setzt das zu Item gehoerende Unterkonto als aktiv.
 */
void TimeMainWindow::setAktivesProjekt(QTreeWidgetItem * item)
{
  bool currentDateSel = (abtList->getDatum()==QDate::currentDate());
  if (!kontoTree->isEintragsItem(item)) return;
  if (!currentDateSel) {
    abtList->setAsAktiv("","","",0);
    return;
  }

  QString uko,ko,abt,top ;
  int idx;

  kontoTree->itemInfo(item,top,abt,ko,uko,idx);

  QString oldabt, oldko, olduk;
  int oldidx;
  abtList->getAktiv(oldabt, oldko, olduk,oldidx);

  auto doSetActiveProject=[=](int idx){
    abtList->setAsAktiv(abt,ko,uko,idx);
    kontoTree->refreshItem(oldabt,oldko,olduk,oldidx);
    kontoTree->refreshItem(abt,ko,uko,idx);
    kontoTree->setCurrentItem(item);
    if (item->text(KontoTreeItem::COL_TYPE) == "tageweise abrechnen (kd)" && abtList->ukHatMehrereEintrage(abt, ko, uko, idx)) {
#ifndef WASMQUIRKS
      statusBar->showMessage(tr("Please specify only one entry for accounts of type \"%1\"!").arg(item->text(KontoTreeItem::COL_TYPE)), 10000);
#endif
      QApplication::beep();
    }
    updateCaption();
  };

  if (abtList->overTimeModeActive()) {
    UnterKontoEintrag entry;
    abtList->getEintrag(entry,abt,ko,uko,idx);
    QSet<QString> existingRS = entry.getAchievedSpecialRemunSet();
    QSet<QString> otmRS = abtList->getActiveOverTimeModes();
    if (!existingRS.contains(otmRS)) {
       existingRS.unite(otmRS);
       auto helper=new SpecialRemunEntryHelper();
       connect(helper, &SpecialRemunEntryHelper::checked, [=](bool reuse, int correctidx) {
         int newidx=correctidx;
         if (!reuse) {
           auto newentry=entry;
           newidx=idx;
           if (entry.sekunden!=0 || entry.sekundenAbzur!=0) {
             newidx=abtList->insertEintrag(abt,ko,uko);
             newentry.sekunden=0;
             newentry.sekundenAbzur=0;
           }
           newentry.setAchievedSpecialRemunSet(existingRS);
           abtList->setEintrag(abt, ko, uko, newidx, newentry);
         }
         kontoTree->refreshAllItemsInUnterkonto(abt,ko,uko);
         helper->deleteLater();
         doSetActiveProject(newidx);   
      });
      helper->checkSREntry(abtListToday,idx,abt,ko,uko,existingRS);
      return;
    }
  } 
  doSetActiveProject(idx);
  saveLater();
}

/**
 * Erzeugt einen DatumsDialog zum aendern des aktuellen Datums.
 */
void TimeMainWindow::callDateDialog()
{
  /* FIXME: no idea, why we need to do that, but otherwise QLabels and
   * QPushButtons in the preference dialog won't use the custom font (at least
   * on the Mac with Qt 4.8.0) */
  if (settings->useCustomFont())
  {
    QString custFont=settings->customFont();
    int custFontSize=settings->customFontSize();
    QApplication::setFont(QFont(custFont,custFontSize));
  }
  QWidget* ddparent=NULL;
  QSet<QDate> unclean;
  #ifdef __EMSCRIPTEN__
  // in WASM we need to set the dialog modal as a workaround because we have no full window management. In all other cases we want
  // to have the dialog non-modal, so that we can interact with the main window.
  ddparent=this;
  unclean=SyncOfflineHelper::findAllUnmergedDates();
  #endif
  DateChangeDialog * dateDialog=new DateChangeDialog(abtList->getDatum(), ddparent, unclean);
  connect(dateDialog, SIGNAL(dateChanged(const QDate&)), this, SLOT(changeVisibleDate(const QDate&)));
  dateDialog->setAttribute(Qt::WA_DeleteOnClose);
  dateDialog->open();
  dateDialog->raise();
}

void TimeMainWindow::infoDialog(TextViewerDialog *&dialog, const QString& title, const QString& name, int x, int y, bool plaintext_links) {
  dialog = new TextViewerDialog(this,title,name, plaintext_links);
  dialog->resize(x, y);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->open();
  dialog->raise();
}

void TimeMainWindow::callHelpDialog() {
  TextViewerDialog* dialog;
  infoDialog(dialog, tr("sctime: Help"), tr("sctime help"), 600, 450);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->browser()->setSource(QUrl("qrc:/help"));
}


void TimeMainWindow::callAboutBox() {
  TextViewerDialog* dialog;
  infoDialog(dialog, tr("About sctime"), tr("sctime about"), 400, 350);
  QString clientidline="";
#ifdef __EMSCRIPTEN__
  clientidline="<tr><td>ClientID:</td><td>"+clientId+"</td></tr>";
#endif
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->browser()->setHtml(tr(
        "<h1><img src=':/scLogo_15Farben' />sctime</h1>"
        "<table><tr><td>Version:</td><td>%1</td></tr>%4"
        "<tr><td>Qt-Version:</td><td>%2 (development)</td></tr>"
        "<tr><td></td><td>%3 (runtime)</td></tr>"
        "<tr><td>Developers:</td><td>Johannes Abt, Alexander Wuetz, Florian Schmitt</td></tr>"
        "<tr><td>Patches:</td><td>Marcus Camen</td></tr>"
        "<tr><td>Mac:</td><td>Michael Weiser</td></tr>"
        "<tr><td>New icons:</td><td>Mayra Delgado</td></tr>"
        "<tr><td>RT:</td><td><a href='mailto:zeittools-rt@science-computing.de'>zeittools-rt@science-computing.de</a></td></tr>"
        "<tr><td>Project page:</td><td><a href='http://github.com/scVENUS/sctime/'>http://github.com/scVENUS/sctime/</a></td></tr>"
        "</table><p>This program is licensed under the GNU Public License v3.</p>")
                   .arg(qApp->applicationVersion(), QT_VERSION_STR, qVersion(), clientidline));
}

void TimeMainWindow::logDialog() {
  TextViewerDialog* dialog;
  infoDialog(dialog, tr("sctime: Messages"), tr("sctime message log"), 700, 300);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->browser()->setPlainText(logText);
}

void TimeMainWindow::callBereitschaftsDialog(QTreeWidgetItem * item) {
  if ((!kontoTree->isUnterkontoItem(item))||(abtList->checkInState()))
    return;
  QString top,uko,ko,abt;
  int idx;
  kontoTree->itemInfo(item,top,abt,ko,uko,idx);
  UnterKontoListe *ukl;
  UnterKontoListe::iterator ukiter;

  if (!abtList->findUnterKonto(ukiter, ukl, abt, ko, uko)) {
    QMessageBox *msgbox=new QMessageBox(QMessageBox::Critical,
          tr("sctime: On-call times"), tr("subaccount not found!"), QMessageBox::NoButton, this);
    connect(msgbox, &QMessageBox::finished, msgbox, &QMessageBox::deleteLater);
    msgbox->open();
    msgbox->raise();
    return;
  }  

  QStringList bereitschaften = ukiter->second.getBereitschaft();

  auto oncalldialog=new OnCallDialog(abt,ko,uko,bereitschaften,this);
  connect(oncalldialog, &OnCallDialog::finishedWithInfo, this, &TimeMainWindow::finishBereitschaftsDialog);

  oncalldialog->open();
  oncalldialog->raise();
}

void TimeMainWindow::finishBereitschaftsDialog(QString abt, QString ko, QString uko, QStringList bereitschaften, QStringList bereitschaftenNeu) {
  auto dialog = (QDialog*)(sender());
  if (dialog->result())
  {
    if (bereitschaften!=bereitschaftenNeu) {
      UnterKontoListe *ukl;
      UnterKontoListe::iterator ukiter;

      if (!abtList->findUnterKonto(ukiter, ukl, abt, ko, uko)) {
        QMessageBox *msgbox=new QMessageBox(QMessageBox::Critical,
        tr("sctime: On-call times"), tr("subaccount not found!"), QMessageBox::NoButton, this);
        connect(msgbox, &QMessageBox::finished, msgbox, &QMessageBox::deleteLater);
        msgbox->open();
        msgbox->raise();
        return;
      }
      ukiter->second.setBereitschaft(bereitschaftenNeu);
      kontoTree->refreshAllItemsInUnterkonto(abt,ko,uko);
      saveLater();
    }
  }
  dialog->deleteLater();
}

void TimeMainWindow::callSpecialRemunerationsDialog(QTreeWidgetItem * item) {
  QString top,uko,ko,abt;
  int idx;
  kontoTree->itemInfo(item,top,abt,ko,uko,idx);
  SpecialRemunerationsDialog* srDialog= new SpecialRemunerationsDialog(abtList, abt,ko,uko,idx, this);
  connect(srDialog, &SpecialRemunerationsDialog::finishedWithInfo, this, &TimeMainWindow::finishSpecialRemunerationsDialog);
  srDialog->open();
  srDialog->raise();
}

void TimeMainWindow::finishSpecialRemunerationsDialog(QString abt, QString ko, QString uko) {
  auto srDialog=(SpecialRemunerationsDialog*)(sender());
  if (srDialog->result())
  {
      kontoTree->refreshAllItemsInUnterkonto(abt,ko,uko);
      saveLater();
  }
  srDialog->deleteLater();
}


void TimeMainWindow::refreshAfterColorChange(const QString& abt, const QString& ko, const QString& uko) {
	if (ko != "") {
		if(uko != "") {
			kontoTree->refreshAllItemsInUnterkonto(abt,ko,uko);
		} else {
			kontoTree->refreshAllItemsInKonto(abt,ko);
		}
	} else {
		kontoTree->refreshAllItemsInDepartment(abt);
	}
}

void TimeMainWindow::callColorDialog()
{
   QTreeWidgetItem * item=kontoTree->currentItem();
   // when first started there is no selected item, so we cannot select a color
   // for that
   if (!item)
     return;

   QString top,uko,ko,abt;
   int idx;
   kontoTree->itemInfo(item,top,abt,ko,uko,idx);

   QColor color, initial = QColor();
   if (abtList->hasBgColor(abt,ko,uko))
     initial = abtList->getBgColor(abt,ko,uko);
   auto colordialog = new QColorDialog(initial, this);
   connect(colordialog, &QColorDialog::finished, [=](){
     if (colordialog->result()==QDialog::Accepted) {
       auto color=colordialog->selectedColor();
       if (color.isValid()) {
         abtList->setBgColor(color, abt,ko,uko);
         refreshAfterColorChange(abt, ko, uko);
       }
     }
     colordialog->deleteLater();
   });
   colordialog->open();
   colordialog->raise();
}

void TimeMainWindow::removeBgColor() {
   QTreeWidgetItem * item=kontoTree->currentItem();
   QString top,uko,ko,abt;
   int idx;
   kontoTree->itemInfo(item,top,abt,ko,uko,idx);
   abtList->unsetBgColor(abt,ko,uko);
   refreshAfterColorChange(abt, ko, uko);
}

void TimeMainWindow::jumpToAlleKonten()
{
   QTreeWidgetItem * item=kontoTree->currentItem();
   QString top,uko,konto,abt;
   int idx;
   kontoTree->itemInfo(item,top,abt,konto,uko,idx);
   QTreeWidgetItem *newitem = kontoTree->sucheKontoItem(ALLE_KONTEN_STRING,abt,konto);
   if (newitem) {
      kontoTree->setCurrentItem(newitem);
      //kontoTree->currentItem(newitem);
   }
}

void TimeMainWindow::checkComment(const QString& abt, const QString& ko , const QString& uko,int idx) {
  UnterKontoEintrag eintrag;
  if ((settings->warnISO8859())&&(abtList->getEintrag(eintrag, abt, ko, uko, idx))) {
    QStringEncoder encoder(QStringConverter::Latin1);
    encoder(eintrag.kommentar);
    if (encoder.hasError()) {
      QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
            tr("Warning"),
            tr("Warning: The entered comment contains a character that is not part of "
               "ISO-8859-1 and might not render correctly on some platforms. "
               "This may cause problems with custom reporting scripts."), QMessageBox::NoButton, this);
      connect(msgbox, &QMessageBox::finished, msgbox, &QMessageBox::deleteLater);
      msgbox->open();
      msgbox->raise();
    }
  }
}

void TimeMainWindow::moveEvent(QMoveEvent *) {
  settings->setMainWindowGeometry(pos(),size());
}

void TimeMainWindow::commitBereit(DSResult data) {
  BereitschaftsListe *berListe = BereitschaftsListe::getInstance();
  QStringList ql;
  foreach (ql, data){
    if (ql.isEmpty()) continue;
    QString name = ql[0].simplified();
    QString beschreibung = ql[1].simplified();
    if (beschreibung.isEmpty()) beschreibung = ""; // Leerer String, falls keine Beschr. vorhanden. //FIXME: notwendig?
    berListe->insertEintrag(name, beschreibung);
  }
  if (!m_afterCommitMethodQueue->isEmpty()) {
     auto method = m_afterCommitMethodQueue->dequeue();
     QMetaObject::invokeMethod(method->obj, method->method, Qt::QueuedConnection);
     delete method;
  }
}

void TimeMainWindow::commitSpecialRemun(DSResult data) {
  SpecialRemunTypeMap srmap;
  QList<QString> global_srnames;
  QStringList ql;
  foreach (ql, data){
    if (ql.isEmpty()) continue;
    QString category = ql[0].simplified();
    QString description = ql[1].simplified();
    srmap[category]=SpecialRemunerationType(description);
    if ((QString("1").compare(ql[2])==0)||(QString("true").compare(ql[2])==0)) {
      global_srnames.push_back(category);
    }
  }
  abtList->setSpecialRemunTypeMap(srmap);
  abtList->setGlobalSpecialRemunNames(global_srnames);
  if (!m_afterCommitMethodQueue->isEmpty()) {
     auto method = m_afterCommitMethodQueue->dequeue();
     QMetaObject::invokeMethod(method->obj, method->method, Qt::QueuedConnection);
     delete method;
  }
}

void TimeMainWindow::callAdditionalLicenseDialog() {
  TextViewerDialog* dialog;
  infoDialog(dialog, tr("sctime: Additional Information about Licensing"), tr("sctime licensing"), 600, 450, true);
  dialog->browser()->setSource(QUrl::fromLocalFile(additionalLicensesFile));
}

/**
 * switches the overtimemode identified by the given special remun to the given state*/
void TimeMainWindow::switchOvertimeMode(bool enabled, QString specialremun) {
  abtListToday->setOverTimeModeState(enabled,specialremun);
  QString abt, ko, uko;
  int oldidx;
  abtListToday->getAktiv(abt, ko, uko,oldidx);
  UnterKontoEintrag entry;
  abtListToday->getEintrag(entry,abt,ko,uko,oldidx);
  QSet<QString> existingRS = entry.getAchievedSpecialRemunSet();
  if (existingRS.contains(specialremun)!=enabled) {
    QSet<QString> desiredRemuns = existingRS;
    if (enabled) {
       desiredRemuns.insert(specialremun);
    } else {
       desiredRemuns.remove(specialremun);
    }
    auto helper=new SpecialRemunEntryHelper();
    connect(helper, &SpecialRemunEntryHelper::checked, [=](bool reuse, int correctidx) {
      int newidx=correctidx;
      auto newentry=entry;
      if (!reuse) {
        newidx=oldidx;
        if (entry.sekunden!=0 || entry.sekundenAbzur!=0) {
          newidx=abtListToday->insertEintrag(abt,ko,uko);
          newentry.sekunden=0;
          newentry.sekundenAbzur=0;
        }
      
        newentry.setAchievedSpecialRemunSet(desiredRemuns);
        abtListToday->setEintrag(abt, ko, uko, newidx, newentry);
      }
      abtListToday->setAsAktiv(abt,ko,uko,newidx);
      kontoTree->refreshAllItemsInUnterkonto(abt,ko,uko);
      helper->deleteLater();
      saveLater();
    });
    helper->checkSREntry(abtListToday,oldidx,abt,ko,uko,desiredRemuns);
  }
}

/**
 * switches the regulated overtime mode to the given state*/
void TimeMainWindow::switchOvertimeRegulatedMode(bool enabled) {
    settings->setOvertimeRegulatedModeActive(enabled);
    if (enabled) {
      settings->setOvertimeOtherModeActive(false);
      overtimeOtherModeAction->setChecked(false);
      statusBar->setMode(tr("Unregulated OT"),false);
    }
    statusBar->setMode(tr("Regulated OT"),enabled);
    switchOvertimeMode(enabled, settings->overtimeRegulatedSR());
}

/**
 * switches the other overtime mode to the given state*/
void TimeMainWindow::switchOvertimeOtherMode(bool enabled) {
    settings->setOvertimeOtherModeActive(enabled);
    if (enabled) {
      settings->setOvertimeRegulatedModeActive(false);
      overtimeRegulatedModeAction->setChecked(false);
      statusBar->setMode(tr("Regulated OT"),false);
    }
    statusBar->setMode(tr("Unregulated OT"),enabled);
    switchOvertimeMode(enabled, settings->overtimeOtherSR());
}

/**
 * switches the public holiday mode to the given state*/
void TimeMainWindow::switchPublicHolidayMode(bool enabled) {
    settings->setPublicHolidayModeActive(enabled);
    statusBar->setMode(tr("Holiday"),enabled);
    switchOvertimeMode(enabled, settings->publicHolidaySR());
}

/**
 * switches the night mode to the given state*/
void TimeMainWindow::switchNightMode(bool enabled) {
    settings->setNightModeActive(enabled);
    statusBar->setMode(tr("Night"),enabled);
    switchOvertimeMode(enabled, settings->nightSR());
}

/**
 * opens a dialog asking if nightmode should be switched on, or off, depending
 * on the given parameter and the current state of the mode (isnight=true means
 * nighttime should be enabled, if that is not already the case)*/
void TimeMainWindow::callNightTimeDialog(bool isnight) 
{
    static QMutex mutex;
    // we only want one instamnce of this dialog
    if (!mutex.tryLock()) {
      return;
    }
    if (isnight==settings->nightModeActive()) {
      mutex.unlock();
      return;
    }
    QDateTime beforeOpen=QDateTime::currentDateTime();
    QMessageBox* msgbox;
    if (isnight) {
       msgbox=new QMessageBox(QMessageBox::Question,
            tr("sctime: switch nightmode on?"),
            tr("It is %1. Should I switch to night mode, so you get special "
            "remuneration for working late? Please also check your companies "
            "regulations before enabling nightmode.").arg(beforeOpen.time().toString("hh:mm")),
            QMessageBox::Yes|QMessageBox::No, this);
    } else {
       msgbox=new QMessageBox(QMessageBox::Question,
            tr("sctime: switch nightmode off?"),
            tr("It is %1. Should I switch night mode off? "
            "Otherwise you apply for further special remuneration. Please also check your companies "
            "regulations when keeping nightmode enabled.").arg(beforeOpen.time().toString("hh:mm")),
            QMessageBox::Yes|QMessageBox::No, this);
    }
    connect(msgbox, &QMessageBox::finished,
    [=](){
      if (msgbox->result() == QMessageBox::Yes) {
        QString abt, ko, uko;
        int oldidx;
        abtListToday->getAktiv(abt, ko, uko,oldidx);
        // this might change the current entry as a side effect
        nightModeAction->setChecked(isnight);
        switchNightMode(isnight);
        int newidx;
        abtListToday->getAktiv(abt, ko, uko,newidx);
        // its probable that the user noticed the previous dialog only after some minutes.
        // we give him a chance to move the accrued times to the correct entry
        auto delta=beforeOpen.secsTo(QDateTime::currentDateTime());
        if (beforeOpen.daysTo(QDateTime::currentDateTime())>=1) {
          cantMoveTimeDialog(delta);
          mutex.unlock();
          return;
        }
        if ((!paused)&&(newidx!=oldidx)&&(delta>=60)) {
          auto questionBox=new QMessageBox(QMessageBox::Question,
                  tr("sctime: move worked time to new entry"),
                  tr("Should %1 minutes be moved to the new selected entry?").arg(int(delta/60)),
                  QMessageBox::Yes|QMessageBox::No, this);
          connect(questionBox, &QMessageBox::finished, [=]() {
            if (questionBox->result()==QMessageBox::Yes) {
              if (beforeOpen.daysTo(QDateTime::currentDateTime())>=1) {
                cantMoveTimeDialog(delta);
                mutex.unlock();
                return;
              }
              abtListToday->changeZeit(abt, ko, uko, oldidx, -delta, false, true, pausedAbzur);
              abtListToday->changeZeit(abt, ko, uko, newidx, delta, false, true, pausedAbzur);
            
              kontoTree->refreshAllItemsInUnterkonto(abt,ko,uko);
            }
            saveLater();
            questionBox->deleteLater();
          });
          questionBox->open();
          questionBox->raise();

        }
      }
      mutex.unlock();
      msgbox->deleteLater();
   });
   msgbox->open();
   msgbox->raise();
}

/** shows an error message if worked times could not be moved to another entry */
void TimeMainWindow::cantMoveTimeDialog(int delta)
{
  QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
            tr("sctime: could not move worked time to new entry"),
            tr("A date change has occurrred - therefore %1 minutes of work time won't be moved automatically to the new entry. Please check your entries manually.").arg(int(delta/60)),
            QMessageBox::Ok, this);
  connect(msgbox, &QMessageBox::finished, msgbox, &QMessageBox::deleteLater);
  msgbox->open();
  msgbox->raise();
}

/**
 * checks and updates overtime modes after a pause (they might not be valid anymore)*/
void TimeMainWindow::updateSpecialModes(bool afterPause) {
  QDateTime timestamp = QDateTime::currentDateTime();
  QDateTime lastts = settings->lastRecordedTimestamp();
  if (lastts.isValid()) {
    bool sameday=(lastts.date()==timestamp.date());
    switchOvertimeOtherMode(settings->overtimeOtherModeActive() && sameday);
    switchOvertimeRegulatedMode(settings->overtimeRegulatedModeActive() && sameday);
    switchPublicHolidayMode(settings->publicHolidayModeActive() && sameday);
    if ((lastts.secsTo(timestamp)>60*60*12)) {
      switchNightMode(false);
    } else {
      switchNightMode(settings->nightModeActive());
    }
    overtimeRegulatedModeAction->setChecked(settings->overtimeRegulatedModeActive());
    overtimeOtherModeAction->setChecked(settings->overtimeOtherModeActive());
    publicHolidayModeAction->setChecked(settings->publicHolidayModeActive());
    nightModeAction->setChecked(settings->nightModeActive());
  }
  if (!settings->nightModeBegin().isNull() && !settings->nightModeEnd().isNull()) {
    if (afterPause) {
        if (timestamp.time()>settings->nightModeBegin()||timestamp.time()<settings->nightModeEnd()) {
           QTimer::singleShot(3000, this, SLOT(callNightTimeBeginDialog()));
        } else {
           QTimer::singleShot(3000, this, SLOT(callNightTimeEndDialog()));
        }
    }
  }
  saveLater();
}

/**
 * slot to open a dialog asking to switch nightmode on, if necessary */
void TimeMainWindow::callNightTimeBeginDialog(){
  callNightTimeDialog(true);
}

/**
 * slot to open a dialog asking to switch nightmode off, if necessary */
void TimeMainWindow::callNightTimeEndDialog(){
  callNightTimeDialog(false);
}

void TimeMainWindow::readIPCMessage() {
  QLocalSocket *socket = m_ipcserver->nextPendingConnection();
  if (!socket->waitForReadyRead(3000))
  {
     trace(socket->errorString());
     socket->close();
     delete socket;
     return;
  }
  QByteArray ba = socket->readAll();
  QJsonDocument msg =QJsonDocument::fromJson(ba);
  if (msg.isNull()) {
     socket->close();
     delete socket;
     return;
  }
  auto obj=msg.object();
  auto type = obj.value("type").toString();
  if (type=="accountlink") {
    auto link=obj.value("link").toString();
    openEntryLink(QUrl(link));
  }
  socket->close();
  delete socket;
}

void TimeMainWindow::callPunchClockDialog() {
#ifndef DISABLE_PUNCHCLOCK
  PunchClockDialog* pcDialog=new PunchClockDialog(m_punchClockList, m_PCSToday, this);
  connect(pcDialog, &PunchClockDialog::finished, this, &TimeMainWindow::finishPunchClockDialog);
  pcDialog->setModal(true);
  pcDialog->open();
  pcDialog->raise();
#endif
}

void TimeMainWindow::finishPunchClockDialog() {
#ifndef DISABLE_PUNCHCLOCK
  auto pcDialog=(PunchClockDialog*)(sender());
  auto result=pcDialog->result();
  if (result==QDialog::Accepted) {
    pcDialog->copyToList(m_punchClockList);
    auto now=QDateTime::currentDateTime();
    if (m_punchClockList==m_punchClockListToday) {
      auto pce=m_punchClockListToday->currentEntry();
      if (pce!=m_punchClockListToday->end()) {
        pce->second=now.time().msecsSinceStartOfDay()/1000;
      } 
      m_PCSToday->check(m_punchClockList,0, m_PCSYesterday);
    } else if (abtList->getDatum()==QDate::currentDate().addDays(-1)) {
      m_PCSYesterday->setDate(abtList->getDatum());
      m_PCSYesterday->check(m_punchClockList,0,NULL);
      settings->setPreviousPCCData(m_PCSYesterday->serialize());
    }
    saveLater();
  }
  pcDialog->deleteLater();
  updateBreakTime();
#endif
}

void TimeMainWindow::switchRestCurrentlyOffline(bool offline) {
   settings->setRestCurrentlyOffline(offline);
   if (settings->restSaveOffline()) {
      statusBar->setOnlineStatus(tr("permanently offline"));
   } else {
     if (offline) {
       statusBar->setOnlineStatus(tr("offline"));
     } else {
       statusBar->setOnlineStatus(tr("online"));
     }
   }
 }

void TimeMainWindow::callDownloadSHDialog() {
#ifdef DOWNLOADDIALOG
   auto dialog=new DownloadSHDialog(settings, networkAccessManager, this);
   connect(dialog, &DownloadSHDialog::workflowFinished, dialog, &DownloadSHDialog::deleteLater);
   dialog->open();
   dialog->raise();
#endif
}

void TimeMainWindow::sessionInvalid() {
// for now, only implemented in WASM case
#ifdef __EMSCRIPTEN__
   static bool dialogopen = false;
   if (dialogopen) {
     return;
   }
   dialogopen = true;
   QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
            tr("sctime: invalid session"),
            tr("Your session seems to be invalid. Press OK to open a new window to refresh it. Please provide your credentials there if your browser asks for them. Alternativly you can go permanently offline."),
            QMessageBox::Ok, this);
   msgbox->addButton(tr("Go Offline"), QMessageBox::NoRole);
   connect(msgbox, &QMessageBox::finished,
   [=](){
      if (msgbox->result()!=QMessageBox::Ok) {
        bool offline=settings->restSaveOffline();
        if (offline==false) {
          toggleOnlineStatus();
        }
      } else {
        emscripten_run_script(QString("window.open('%1', '%2');").arg(getStaticUrl()+REFRESH_URL_PART).arg(tr("Refresh Session")).toUtf8().data());
      }
      dialogopen=false;
      msgbox->deleteLater();
   });
   msgbox->open();
   msgbox->raise();
#endif
}

void TimeMainWindow::saveLater() {
   if (saveLaterTimer) {
      saveLaterTimer->start(2000);
   }
}

void TimeMainWindow::writeConflictDialog(QDate targetdate, bool global, const QByteArray ba) {
  if (global) {
    // Do nothing for now, just keep the current settings and dont bother the user
    return;
  }
  if (m_conflictDialogOpenForDates.contains(targetdate)) {
     ConflictDialog *dialog=dynamic_cast<ConflictDialog*>(m_conflictDialogOpenForDates[targetdate]);
     if (dialog) {
       QDomDocument doc("settings");
       QString errMsg;
       QString resname;
       int errLine, errCol;
       doc.setContent(qUncompress(ba), &errMsg, &errLine, &errCol);
       dialog->updateRemoteDocument(doc);
     }
     return;
  }
  if (abtList->getDatum()==targetdate) {
  } else if (abtListToday->getDatum()==targetdate) {
  } else {
    // this should not happen, but handle anyways, just in case...
    QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
            tr("sctime: unresolvable conflict"),
            tr("There seems to be a conflict with another session that could not be resolved. Please check your entries."),
            QMessageBox::Ok, this);
    m_conflictDialogOpenForDates[targetdate]=msgbox;
    connect(msgbox, &QMessageBox::finished,
    [=](){
      m_conflictDialogOpenForDates.remove(targetdate);
      msgbox->deleteLater();
    });
    msgbox->open();
    msgbox->raise();
    return;
  }
  QDomDocument doc("settings");
  QString errMsg;
  QString resname;
  int errLine, errCol;
  bool success = doc.setContent(qUncompress(ba), &errMsg, &errLine, &errCol);
  if (!success) {
        QMessageBox *msgbox=new QMessageBox(QMessageBox::Critical,
                              QObject::tr("sctime: reading configuration file"),
                              QObject::tr("error in %1, line %2, column %3: %4.").arg(resname).arg(errLine).arg(errCol).arg(errMsg), QMessageBox::NoButton, dynamic_cast<QWidget*>(this->parent()));
        m_conflictDialogOpenForDates[targetdate]=msgbox;
        connect(msgbox, &QMessageBox::finished,
        [=](){
          m_conflictDialogOpenForDates.remove(targetdate);
          msgbox->deleteLater();
        });
        msgbox->open();
        msgbox->raise();
        return;
  }
  QString remoteID=doc.documentElement().attribute("identifier");
  if (remoteID==getMachineIdentifier()) {
    QMessageBox *msgbox=new ConflictDialogSameBrowser(this);
    m_conflictDialogOpenForDates[targetdate]=msgbox;
    connect(msgbox, &QMessageBox::finished,
    [=](){
      m_conflictDialogOpenForDates.remove(targetdate);
      msgbox->deleteLater();
      SyncOfflineHelper::removeUnmergedData(targetdate);
      
      refreshKontoListe();
      saveWithTimeout(0);
    });
    msgbox->open();
    msgbox->raise();
    return;
  }

  ConflictDialog *dialog=new ConflictDialog(settings, networkAccessManager, targetdate, global, doc, this);
  m_conflictDialogOpenForDates[targetdate]=dialog;
  connect(dialog, &QMessageBox::finished,
    [=](){
      m_conflictDialogOpenForDates.remove(targetdate);
      dialog->deleteLater();
      SyncOfflineHelper::removeUnmergedData(targetdate);
      
      refreshKontoListe();
      saveWithTimeout(0);
  });
  dialog->open();
  dialog->adjustSize();
  dialog->raise();
}

void TimeMainWindow::readConflictDialog(QDate targetdate, bool global, QDomDocument remotesettings) {
  auto reader=dynamic_cast<XMLReader*>(sender());
  if (global) {
    // Do nothing for now, just keep the current settings and dont bother the user
    reader->ignoreConflict();
    return;
  }
  if (m_conflictDialogOpenForDates.contains(targetdate)) {
     ConflictDialog *dialog=dynamic_cast<ConflictDialog*>(m_conflictDialogOpenForDates[targetdate]);
     if (dialog) {
       dialog->updateRemoteDocument(remotesettings);
     }
     return;
  }
  bool istoday=false;
  if (abtList->getDatum()==targetdate) {
  } else if (abtListToday->getDatum()==targetdate) {
    istoday=true;
  } else {
    // this should not happen, but handle anyways, just in case...
    QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
            tr("sctime: unresolvable conflict"),
            tr("There seems to be a conflict with another session that could not be resolved. Please check your entries."),
            QMessageBox::Ok,this);
    m_conflictDialogOpenForDates[targetdate]=msgbox;
    connect(msgbox, &QMessageBox::finished,
    [=](){
      m_conflictDialogOpenForDates.remove(targetdate);
      msgbox->deleteLater();
    });
    msgbox->open();
    msgbox->raise();
    return;
  }
  QString remoteID=remotesettings.documentElement().attribute("identifier");
  if (remoteID==getMachineIdentifier()) {
    QMessageBox *msgbox=new ConflictDialogSameBrowser(this);
    m_conflictDialogOpenForDates[targetdate]=msgbox;
    connect(msgbox, &QMessageBox::finished,
    [=](){
      m_conflictDialogOpenForDates.remove(targetdate);
      msgbox->deleteLater();
      SyncOfflineHelper::removeUnmergedData(targetdate);
      if (reader!=NULL) {
        if (istoday) {
           XMLReader lreader(settings, networkAccessManager, global, false, true, abtListToday, m_punchClockListToday);
           lreader.fillSettingsFromDocument(remotesettings, settings);
           emit reader->settingsPartRead(global, abtListToday, m_punchClockListToday, true, "");
        } else {
           XMLReader lreader(settings, networkAccessManager, global, false, true, abtList, m_punchClockList);
           lreader.fillSettingsFromDocument(remotesettings, settings);
           emit reader->settingsPartRead(global, abtList, m_punchClockList, true, "");
        }
      }
      
      saveWithTimeout(0);
    });
    msgbox->open();
    msgbox->raise();
    return;
  }

  ConflictDialog *dialog=new ConflictDialog(settings, networkAccessManager, targetdate, global, remotesettings, this);
  m_conflictDialogOpenForDates[targetdate]=dialog;
  connect(dialog, &QMessageBox::finished,
    [=](){
      if (reader!=NULL) {
        if (istoday) {
           emit reader->settingsPartRead(global, abtListToday, m_punchClockListToday, true, "");
        } else {
           emit reader->settingsPartRead(global, abtList, m_punchClockList, true, "");
        }
      }
      dialog->deleteLater();
      m_conflictDialogOpenForDates.remove(targetdate);
      SyncOfflineHelper::removeUnmergedData(targetdate);
      saveWithTimeout(0);
      
  });
  dialog->open();
  dialog->adjustSize();
  dialog->raise();
}

void TimeMainWindow::readConflictWithLocalDialog(QDate targetdate, bool global, QDomDocument localsettings, QDomDocument remotesettings) {
  auto reader=dynamic_cast<XMLReader*>(sender());
  if (global) {
    // Do nothing for now, just keep the current settings and dont bother the user
    reader->ignoreConflict();
    return;
  }
  // This dialog needs to refresh data manually, because local data was not loaded, and without it we do not trigger a autmatic save, which in turn would trigger this dialog
  std::function<void()> refreshHandler;
  refreshHandler= [=](){
     if (m_conflictDialogOpenForDates.contains(targetdate)) {
       auto remoteAbtList=new AbteilungsListe(targetdate, abtList);
       auto remoteSettings=new SCTimeXMLSettings(*settings);
       PunchClockList* remotePunchClockList=new PunchClockList();
       XMLReader *reader=new XMLReader(settings, networkAccessManager,false, false, false, remoteAbtList, remotePunchClockList);
       auto updater=[=](){
         if (m_conflictDialogOpenForDates.contains(targetdate)) {
           auto dialog=dynamic_cast<ConflictDialog*>(m_conflictDialogOpenForDates[targetdate]);
           if (dialog) {
             dialog->updateRemoteDocument(remotesettings);
             QTimer::singleShot(60000, refreshHandler);
           }
         }
         delete reader;
         delete remoteAbtList;
         delete remoteSettings;
         delete remotePunchClockList;
       };
       connect(reader, &XMLReader::settingsRead, updater);
       connect(reader, &XMLReader::conflictingClientRunning, this, &TimeMainWindow::readConflictDialog);
       connect(reader, &XMLReader::conflictedWithLocal, this, &TimeMainWindow::readConflictWithLocalDialog);
       reader->open();
     }
  };
  QTimer::singleShot(60000, refreshHandler);
  if (m_conflictDialogOpenForDates.contains(targetdate)) {
     ConflictDialog *dialog=dynamic_cast<ConflictDialog*>(m_conflictDialogOpenForDates[targetdate]);
     if (dialog) {
       dialog->updateRemoteDocument(remotesettings);
     }
     return;
  }
  bool istoday=false;
  if (abtList->getDatum()==targetdate) {
  } else if (abtListToday->getDatum()==targetdate) {
    istoday=true;
  } else {
    // this should not happen, but handle anyways, just in case...
    QMessageBox *msgbox=new QMessageBox(QMessageBox::Warning,
            tr("sctime: unresolvable conflict"),
            tr("There seems to be a conflict with another session that could not be resolved. Please check your entries."),
            QMessageBox::Ok,this);
    m_conflictDialogOpenForDates[targetdate]=msgbox;
    connect(msgbox, &QMessageBox::finished,
    [=](){
      m_conflictDialogOpenForDates.remove(targetdate);
      msgbox->deleteLater();
    });
    msgbox->open();
    msgbox->raise();
    return;
  }
  QString remoteID=remotesettings.documentElement().attribute("identifier");
  if (remoteID==getMachineIdentifier()) {
    QMessageBox *msgbox=new ConflictDialogSameBrowser(this);
    m_conflictDialogOpenForDates[targetdate]=msgbox;
    connect(msgbox, &QMessageBox::finished,
    [=](){
      m_conflictDialogOpenForDates.remove(targetdate);
      msgbox->deleteLater();
      SyncOfflineHelper::removeUnmergedData(targetdate);
      if (reader!=NULL) {
        if (istoday) {
           XMLReader lreader(settings, networkAccessManager, global, false, true, abtListToday, m_punchClockListToday);
           lreader.fillSettingsFromDocument(remotesettings, settings);
           emit reader->settingsPartRead(global, abtListToday, m_punchClockListToday, true, "");
        } else {
           XMLReader lreader(settings, networkAccessManager, global, false, true, abtList, m_punchClockList);
           lreader.fillSettingsFromDocument(remotesettings, settings);
           emit reader->settingsPartRead(global, abtList, m_punchClockList, true, "");
        }
      }
      
      saveWithTimeout(0);
    });
    msgbox->open();
    msgbox->raise();
    return;
  }
  ConflictDialog *dialog=new ConflictDialog(settings, networkAccessManager, targetdate, global, remotesettings, this, localsettings);
  m_conflictDialogOpenForDates[targetdate]=dialog;
  connect(dialog, &QMessageBox::finished,
    [=](){
      if (reader!=NULL) {
        if (istoday) {
           emit reader->settingsPartRead(global, abtListToday, m_punchClockListToday, true, "");
        } else {
           emit reader->settingsPartRead(global, abtList, m_punchClockList, true, "");
        }
      }
      dialog->deleteLater();
      m_conflictDialogOpenForDates.remove(targetdate);
      SyncOfflineHelper::removeUnmergedData(targetdate);
      saveWithTimeout(0);
      
  });
  dialog->open();
  dialog->adjustSize();
  dialog->raise();
}

void TimeMainWindow::callDeleteSettingsDialog() {
  DeleteSettingsDialog *dialog=new DeleteSettingsDialog(this, networkAccessManager);
  connect(dialog, &DeleteSettingsDialog::processingDone,
    [=](){
      dialog->deleteLater();
  });
  dialog->open();
  dialog->adjustSize();
  dialog->raise();
}

AbteilungsListe* TimeMainWindow::getEmptyAbtList(QDate date) {
  return new AbteilungsListe(date, abtListToday);
}

void TimeMainWindow::syncAll() {
  SyncOfflineHelper *helper=new SyncOfflineHelper(settings, networkAccessManager, this);
  connect(helper, &SyncOfflineHelper::finished, [=](){
    QList<QDate> uncleanlist;
    QSet unclean=helper->getLastUncleanDates();
    m_lastUncleanDates=unclean; // store the unclean dates for later use
    //build a sorted unclean list
    if (!unclean.isEmpty()) {
      uncleanlist=unclean.values();
      std::sort(uncleanlist.begin(), uncleanlist.end());
    }
    QStringList attentionlist;
    for (const auto& date : uncleanlist) {
      attentionlist.append(date.toString(Qt::ISODate));
    }

    if (!attentionlist.isEmpty()) {
      QString message=tr("There were some conflicts during sync. Please check and save the data for the following dates:\n\n  %1").arg(attentionlist.join("\n  "));
      trace(message);
      static bool dialogopen = false;
      if (!dialogopen) {        
        dialogopen = true;
        QMessageBox *msgbox=new QMessageBox(QMessageBox::Critical,
            tr("sctime: Conflicts during sync"),
            message,
            QMessageBox::Ok, this);
        connect(msgbox, &QMessageBox::finished, [=]() {
          msgbox->deleteLater();
          dialogopen = false;
        });
        msgbox->open();
        msgbox->raise();
      }
    } else {
      statusBar->showMessage(tr("Sync finished successfully"), 5000);
    }
    helper->deleteLater();
  });
  helper->syncAll();
}

QDate TimeMainWindow::getOpenCurrentDate()
{
  return abtListToday->getDatum();
}

QDate TimeMainWindow::getOpenDate()
{
  return abtList->getDatum();
}

void TimeMainWindow::toggleOnlineStatus() {
    bool offline=settings->restSaveOffline();
    if (offline==false) {
        QMessageBox *msgbox=new QMessageBox(QMessageBox::Critical,
                QObject::tr("sctime: going permanently offline"),
                QObject::tr("Do you really want to go permanently offline? Your data will not be synced to the cloud while you are permanently offline."), QMessageBox::Ok|QMessageBox::Cancel, this);
        connect(msgbox, &QMessageBox::finished,
        [=](){
          if (msgbox->result() == QMessageBox::Ok) {
              settings->setRestSaveOffline(true);
              switchRestCurrentlyOffline(true);
          }
          msgbox->deleteLater();
        });
        msgbox->open();
        msgbox->raise();
        return;
    } else {
       settings->setRestSaveOffline(false);
       switchRestCurrentlyOffline(false);
    }
}

void TimeMainWindow::readTimeTrackerTasks() {
    QDate dateFrom = abtList->getDatum();
    QDate dateTo = dateFrom;

    auto reader = new TimeTrackerTasksReader(networkAccessManager, dateFrom, dateTo, this);

    connect(reader, &TimeTrackerTasksReader::tasksReceived, this, [=](const QList<TimeTrackerTask>& tasks) {
        trace(tr("Received %1 sctimeTracker tasks").arg(tasks.size()));

        QString abteilungstr = "_SctimeTracker_";
        QString kontostr = "Tasks";

        bool kontopers = abtList->getKontoFlags(abteilungstr, kontostr) & UK_PERSOENLICH;;

        // Process the tasks
        for (const auto& task : tasks) {
            QString unterkontostr = task.label;
            if (task.durationSeconds>=60) {

              int idx=abtList->insertEintrag(abteilungstr, kontostr, unterkontostr);

              UnterKontoEintrag entry;
              entry.kommentar = task.comment;
              entry.sekunden = task.durationSeconds;
              entry.sekundenAbzur = task.durationSeconds;
              if (kontopers) {
                entry.flags = UK_PERSOENLICH;
              } else {
                entry.flags=0;
              }

              abtList->setEintrag(abteilungstr, kontostr, unterkontostr, idx, entry);
              abtList->setUnterKontoFlags(abteilungstr, kontostr, unterkontostr,IS_DISABLED, FLAG_MODE_NAND);
              if (kontopers) {
                abtList->setUnterKontoFlags(abteilungstr, kontostr, unterkontostr, UK_PERSOENLICH, FLAG_MODE_OR);
              }
            }
        }

        kontoTree->refreshAllItemsInKonto(abteilungstr, kontostr);

        saveLater();

        reader->deleteLater();
    });

    connect(reader, &TimeTrackerTasksReader::failed, this, [=](const QString& errorMessage) {
        trace(tr("Failed to fetch sctimeTracker tasks: %1").arg(errorMessage));
        QMessageBox *msgbox=new QMessageBox(QMessageBox::Critical,
            tr("sctime: Could not get tasks"),
            errorMessage,
            QMessageBox::Ok, this);
        connect(msgbox, &QMessageBox::finished, &QMessageBox::deleteLater);
        msgbox->open();
        msgbox->raise();
        reader->deleteLater();
    });

    reader->fetchTasks(false);
}