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

#include "sctimeapp.h"
#include "timemainwindow.h"
#include "lock.h"
#include "sctimexmlsettings.h"
#include "setupdsm.h"
#include <QUrl>

#ifndef WIN32
#include <signal.h>
#include "unix/signalhandler.h"
#endif

// transform the value of a #define into a string in a portable way
#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)


SctimeApp::SctimeApp(int &argc, char **argv):QApplication(argc, argv) {
    mainWindow=NULL;
    m_lock=NULL;
#ifndef WIN32
    term = NULL;
    hup = NULL;
    int_ = NULL;
    cont = NULL;
#endif
    // necessary for XSM support
    setObjectName("sctime");
    setApplicationName("sctime");
    setApplicationVersion(QUOTE(APP_VERSION));
}

#ifdef WIN32
// catch suspend and resume events and handle them by calling "suspend()" or "resume()"
bool SctimeApp::winEventFilter(MSG * msg, long * result) {
  if (msg->message == WM_POWERBROADCAST && mainWindow && msg->hwnd == (HWND)mainWindow->winId()) {
    if (msg->wParam == PBT_APMRESUMEAUTOMATIC)
      QMetaObject::invokeMethod(mainWindow, "resume", Qt::QueuedConnection);
    else if (msg->wParam == PBT_APMSUSPEND)
      QMetaObject::invokeMethod(mainWindow, "suspend", Qt::QueuedConnection);
    else return false;
    *result = TRUE;
    return true;
  }
  return false;
}
#endif

void SctimeApp::init(Lock* lock, QStringList& dataSourceNames, const QString& zeitkontenfile,
 const QString& bereitschaftsfile, const QString& specialremunfile, const QString& offlinefile, const QString& logfile, const QString& accountlink)
{
  m_lock=lock;
  m_accountLink=accountlink;

  DSM* dsm=new DSM(dataSourceNames, zeitkontenfile, bereitschaftsfile, specialremunfile,offlinefile);
  QNetworkAccessManager *networkAccessManager=new QNetworkAccessManager();
  // do not redirect for REST Requests, to avoid getting send to login pages, etc, which will not work anyways in REST.
  networkAccessManager->setRedirectPolicy(QNetworkRequest::ManualRedirectPolicy);

  mainWindow = new TimeMainWindow(m_lock, networkAccessManager, dsm, logfile);
#ifndef WIN32
  term = new SignalHandler(SIGTERM);
  connect(term, SIGNAL(received()), this, SLOT(closeAllWindows()));
  hup = new SignalHandler(SIGHUP);
  connect(hup, SIGNAL(received()), this, SLOT(closeAllWindows()));
  int_ = new SignalHandler(SIGINT);
  connect(int_, SIGNAL(received()), this, SLOT(closeAllWindows()));
  cont = new SignalHandler(SIGCONT);
  connect(cont, SIGNAL(received()), mainWindow, SLOT(resume()));
#endif
  connect(this, SIGNAL(aboutToQuit()), this, SLOT(cleanup()));
  if (accountlink!="") {
    m_openLinkConnect=connect(mainWindow, SIGNAL(accountListRead()), this, SLOT(openLink()));
  }
  mainWindow->show();
}

/* opens the link once after the application has fully started up */
void SctimeApp::openLink() {
   disconnect(m_openLinkConnect);
   mainWindow->openEntryLink(QUrl(m_accountLink));
}

void SctimeApp::cleanup() {
  m_lock->release();
}

SctimeApp::~SctimeApp() {
  delete mainWindow;
#ifndef WIN32
  delete term;
  delete hup;
  delete int_;
  delete cont;
#endif
}