/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>

#include <medCoreExport.h>

class medAbstractJob;

class medJobManagerPrivate;
class MEDCORE_EXPORT medJobManager: public QObject
{

    Q_OBJECT

private:
    medJobManager(QObject *parent = NULL);
    ~medJobManager();
    static medJobManager *s_instance;

public:
    static medJobManager *instance();

public:
    void registerJob(medAbstractJob *job);
    void unregisterJob(medAbstractJob *job);
    QList<medAbstractJob *> jobs() const;

    void cancelAll();

public:
    static void startJobInThread(medAbstractJob* job, int priority = 0);

private:
    const QScopedPointer<medJobManagerPrivate> d;
};
