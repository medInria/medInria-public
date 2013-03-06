/* medStorage.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 17 18:45:22 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 17 18:45:41 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 4
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include <QtCore>
#include <QString>

#include "medCoreExport.h"

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
     medStorage(void);
    ~medStorage(void);

    static bool mkpath(const QString& dirPath);
    static bool rmpath(const QString& dirPath);

    static QString   dataLocation(void);
    static QString configLocation(void);

    static void setDataLocation(QString);

    static bool copyFiles(QStringList sourceList, QStringList destList, SigEmitter* emitter = NULL);
    static bool createDestination(QStringList sourceList, QStringList& destList, QString sourceDir, QString destDir);
    static void recurseAddDir(QDir d, QStringList & list);
    static bool removeDir(QString dirName, SigEmitter* emitter = NULL);

private:
    static QString m_dataLocation;

};


