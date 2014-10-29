/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>

#include <medCoreExport.h>
#include <medDataIndex.h>

class medJobManagerPrivate;
class medAbstractJob;

/**
 * @class medJobManager
 * @author Michael Knopke
 * @brief Manages safe termination of medJobItems (QRunnables).
 *
 * All medJobItems that are registered here will receive a distributed cancel events when closing the application at will.
 *
 * The JobItems need to make sure to have implemented the Cancel() method.
 *
 * @see medAbstractJob
 */
class MEDCORE_EXPORT medJobManager : public QObject
{
    Q_OBJECT

public:
    static medJobManager *instance();

public:
    bool registerJob(medAbstractJob* job, QString jobName= "");
    bool unRegisterJob(medAbstractJob* job);

public:
    void dispatchGlobalCancelEvent(bool ignoreNewJobItems = true);

signals:
    void cancel(QObject*);
    void jobRegistered(medAbstractJob* job, QString jobName);

protected:
    medJobManager();
    ~medJobManager();

protected:
    static medJobManager *s_instance;

private:
    medJobManagerPrivate *d;
};


