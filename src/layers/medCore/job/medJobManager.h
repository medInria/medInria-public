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
#include <QRunnable>

#include <medCoreExport.h>

class medAbstractJob;

class medJobManagerPrivate;
class MEDCORE_EXPORT medJobManager: public QObject
{

    Q_OBJECT

private:
    medJobManager(QObject *parent = nullptr);
    ~medJobManager();
    static medJobManager *s_instance;

public:
    static medJobManager *instance();

public:
    void registerJob(medAbstractJob *job);
    void unregisterJob(medAbstractJob *job);
    QList<medAbstractJob *> jobs() const;

public:
     void startJobInThread(medAbstractJob* job);

private:
    const QScopedPointer<medJobManagerPrivate> d;
};

class medJobRunner: public QObject, public QRunnable
{
    Q_OBJECT
public:
    medJobRunner(medAbstractJob *parent);
    virtual void run();

signals:
    void exceptionCaught(QString const& message);

private:
    medAbstractJob *m_job;
};
