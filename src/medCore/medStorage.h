/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>
#include <QString>

#include <medCoreExport.h>

class MEDCORE_EXPORT SigEmitter: public QObject
{
    Q_OBJECT

public:
    void doEmit(QString msg)
    {
        emit message(msg);
    }

signals:
        void message(QString);
};

class MEDCORE_EXPORT medStorage
{
public:
     medStorage();
    ~medStorage();

    static bool mkpath(const QString& dirPath);
    static bool rmpath(const QString& dirPath);

    static QString   dataLocation();
    static QString configLocation();

    static void setDataLocation(QString);

    static bool copyFiles(QStringList sourceList, QStringList destList, SigEmitter* emitter = NULL);
    static bool createDestination(QStringList sourceList, QStringList& destList, QString sourceDir, QString destDir);
    static void recurseAddDir(QDir d, QStringList & list);
    static bool removeDir(QString dirName, SigEmitter* emitter = NULL);

private:
    static QString m_dataLocation;

};


