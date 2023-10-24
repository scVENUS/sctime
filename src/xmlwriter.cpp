#include "xmlwriter.h"
#include <QString>
#include <QFile>
#include <QFileInfo>
#include "sctimexmlsettings.h"

void XMLWriter::checkReply(QNetworkReply* input) {
   SCTimeXMLSettings *settings=(SCTimeXMLSettings*)(parent());
   emit settings->settingsPartWritten(global, abtList, pcl);
}

void XMLWriter::writeBytes(QUrl url, QByteArray ba) {
  auto request = QNetworkRequest(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
#ifdef RESTONLY
  networkAccessManager.put(request, ba);
#else
  //TODO
#endif
}