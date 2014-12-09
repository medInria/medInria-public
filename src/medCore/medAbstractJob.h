/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>

#include <medCoreExport.h>

/**
 * @class medAbstractJob
 * @brief Abstract base class for all QRunnables that should appear in the Job Widget.
 *
 * An example of use is the following:
 *  @code
 *   medRunnableProcess *runProcess = new medRunnableProcess(process);
 *   runProcess->start();
 */
class MEDCORE_EXPORT medAbstractJob :  public QObject, public QRunnable
{
    Q_OBJECT

public:
             medAbstractJob(QString name = "Unknown job");
    virtual ~medAbstractJob();

public:
    void start();
    bool isCancelled() const;

private:
    virtual void run();

protected:
    virtual void internalRun() = 0;

signals:
    void progressed(int progress);
    void success();
    void failure();
    void cancelled();
    void showError(const QString& message, unsigned int timeout = 5000);
    /**
    * This signal is emitted when the process cannot be cancelled anymore.
    * @param QObject *sender
    * @return void
    */
    void disableCancel();

public slots:
    virtual void cancel();

private:
    bool m_cancel;
};


