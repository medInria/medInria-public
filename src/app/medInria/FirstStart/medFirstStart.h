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
#include <QUrl>

class medFirstStartPrivate;
class medFirstStart : public QObject
{
public:
    medFirstStart();
    ~medFirstStart();

    void pushPathToCheck(QString pathToCheck, QString pathRessourceBase, QUrl urlDL);
    void checkAndUpdate();

private:
    void askNewVersion();
    bool readUpdatedFile();
    bool writeUpdatedFile();
    bool copy(QString const & pathSource, QString const & pathDest);

private:
    medFirstStartPrivate *d;
};

