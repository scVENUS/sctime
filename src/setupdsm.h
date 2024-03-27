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


#ifndef SETUPDSM_H
#define SETUPDSM_H

#include <QString>
#include <QStringList>

class SCTimeXMLSettings;
class DatasourceManager;
class QNetworkAccessManager;

class DSM {
public:
   DSM(QStringList& userDataSourceNames, const QString &kontenPath, const QString &bereitPath, const QString& specialremunPath, const QString& jsonPath): 
      userDataSourceNames(userDataSourceNames), kontenPath(kontenPath),bereitPath(bereitPath), specialremunPath(specialremunPath), jsonPath(jsonPath) {   
   }

   void setup(SCTimeXMLSettings* settings, QNetworkAccessManager* networkAccessManager);
   DatasourceManager* kontenDSM;
   DatasourceManager* bereitDSM;
   DatasourceManager* specialRemunDSM;
private:
   QStringList userDataSourceNames;
   QString kontenPath;
   QString bereitPath;
   QString specialremunPath;
   QString jsonPath;

   static QString kontenQuery;
   static const QString bereitQuery;
   static const QString specialRemunQuery;
   static QString username();
   static QString password();
};

#endif // SETUPDSM_H
