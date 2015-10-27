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
#include <QRunnable>

#include <medCoreExport.h>


class medAbstractJobPrivate;
class MEDCORE_EXPORT medAbstractJob: public QObject, public QRunnable
{
    Q_OBJECT
public:
    medAbstractJob(QObject *parent = NULL);
    virtual ~medAbstractJob();

    virtual QString caption() const = 0;

public:
    virtual void cancel() = 0;
    bool isRunning() const;

signals:
    void success();
    void failure();
    void progressed(float progression);

private slots:
    void _emitNotRunning();
    void _setIsRunning(bool isRunning);
signals:
    void _running(bool isRunning);

private:
    const QScopedPointer<medAbstractJobPrivate> d;

};
