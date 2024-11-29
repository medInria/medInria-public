#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <QString>

class QNetworkAccessManager;
class medFirstStartClientPrivate;
class medFirstStartClient : public QObject
{
public:
    medFirstStartClient(QString featureName, QString fileName, QString uri, QNetworkAccessManager *qnam);
    ~medFirstStartClient();

    bool updateReferenceFile ();

private:
    QString getNextArchiveFolderName();

private:
    medFirstStartClientPrivate *d;
};

