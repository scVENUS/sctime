#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "ui_logindialogbase.h"
#include <QDialog>
#include <QNetworkCookie>

class QNetworkAccessManager;

class LoginDialog : public QDialog,Ui::LoginDialogBase
{
    Q_OBJECT

public:
    LoginDialog(QNetworkAccessManager *networkAccessManager ,QWidget *parent = nullptr);
    ~LoginDialog() {};

private slots:
    void addCookieToNetworkAccessManager(const QNetworkCookie &cookie);

private:
    QNetworkAccessManager *networkAccessManager;

};

#endif // LOGINDIALOG_H