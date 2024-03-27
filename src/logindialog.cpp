#include "logindialog.h"

#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include <QNetworkCookie>
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>

LoginDialog::LoginDialog(QNetworkAccessManager* networkAccessManager, QWidget *parent)
    : networkAccessManager(networkAccessManager), QDialog(parent)
{
    setupUi(this);
    this->setWindowTitle(tr("sctime: Login"));
    this->setModal(true);
    webEngineView->setUrl(QUrl("http://127.0.0.1:8888/static/"));
    auto page=webEngineView->page();
    auto profile=page->profile();
    auto cookieStore=profile->cookieStore();
    connect(cookieStore, &QWebEngineCookieStore::cookieAdded, this, &LoginDialog::addCookieToNetworkAccessManager);
}

void LoginDialog::addCookieToNetworkAccessManager(const QNetworkCookie &cookie)
{
   networkAccessManager->cookieJar()->insertCookie(cookie);
}