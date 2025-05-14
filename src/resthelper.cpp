/*
    Copyright (C) 2025 science+computing ag
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

#include "resthelper.h"
#include "globals.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/val.h>
#include <emscripten.h>
#endif

#include <QProcessEnvironment>
#include <QRegularExpression>
#include <QNetworkReply>
#include <QUrl>

QString getRestBaseUrl() {
  auto env = QProcessEnvironment::systemEnvironment();
  QString baseurl = env.value("SCTIME_BASE_URL");
#ifdef __EMSCRIPTEN__
  if (baseurl.isNull()||baseurl.isEmpty()) {
     auto location = emscripten::val::global("location");
     auto href = QString::fromStdString(location["href"].as<std::string>());
     int lastSlash=href.lastIndexOf("/");
     baseurl = href.replace(lastSlash,href.length(),"/../");
  }
#endif
  return baseurl;
}

// returns the directory URL for static files
QString getStaticUrl() {
  auto env = QProcessEnvironment::systemEnvironment();
  QString staticurl = env.value("SCTIME_STATIC_URL");
  if (staticurl.isNull()||staticurl.isEmpty()) {
     staticurl=getRestBaseUrl()+STATIC_URL;
  }
  return staticurl;
}

QStringList splitIgnoringQuotes(const QString &input) {
  // split on semicolons, but not inside quotes
  QRegularExpression regex(R"((?:[^";]|"(?:\\.|[^"])*")+)");
  QStringList result;
  auto it = regex.globalMatch(input);
  while (it.hasNext()) {
      QRegularExpressionMatch match = it.next();
      result.append(match.captured(0).trimmed());
  }
  return result;
}

QString unRFC8187(const QString& input) {
  // unRFC8187: remove the RFC 8187 encoding
  QRegularExpression regex(R"(^utf-8\'.*\')",  QRegularExpression::CaseInsensitiveOption);
  QString result=input;
  result.replace(regex, "");
  result=QUrl::fromPercentEncoding(result.toUtf8());
  return result;
}

QString unquote(const QString& input) {
  // unquote: remove the quotes
  QRegularExpression regex(R"(^\"|\"$)");
  QString result=input;
  result.replace(regex, "");
  return result;
}

QString getRestHeader(const QNetworkReply* reply, const QString& name) {
   QByteArray uheader=name.toUtf8();
   if (name.startsWith("sctime-") && !reply->hasRawHeader(uheader)) {
      QString ctheader=reply->header(QNetworkRequest::ContentTypeHeader).toString();
      QStringList parts = splitIgnoringQuotes(ctheader);
      foreach (const QString &part, parts) {
          if (part.startsWith(name+"=")) {
              QString value = part.section("=", 1);
              value =unquote(value);
              trace("unquote value="+value);
              return value;
          } else if (part.startsWith(name+"*=")) {
              QString value = part.section("=", 1);
              value = unRFC8187(value);
              trace("unRFC8187 value="+value);
              return value;
          } 
      }
   }
   return reply->rawHeader(uheader);
}