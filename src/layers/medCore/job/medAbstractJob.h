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

enum medJobExitStatus
{
    MED_JOB_EXIT_FAILURE,
    MED_JOB_EXIT_CANCELLED,
    MED_JOB_EXIT_SUCCES
};

Q_DECLARE_METATYPE(medJobExitStatus)

class medDoubleParameter;

class medAbstractJobPrivate;
class MEDCORE_EXPORT medAbstractJob: public QObject
{
    Q_OBJECT
public:
    medAbstractJob(QObject *parent = NULL);
    virtual ~medAbstractJob();

    virtual QString caption() const = 0;

public:
    virtual medJobExitStatus run() = 0;
    virtual void cancel() = 0;
    bool isRunning() const;

    medDoubleParameter* progression() const;

signals:
    void finished(medJobExitStatus exitStatus);

private slots:
    void _setIsRunning(bool isRunning);

signals:
    void running(bool isRunning);

private:
    const QScopedPointer<medAbstractJobPrivate> d;

};
