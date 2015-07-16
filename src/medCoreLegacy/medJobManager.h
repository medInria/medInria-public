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
class medJobItem;

/**
 * @class medJobManager
 * @author Michael Knopke
 * @brief Manages safe termination of medJobItems (QRunnables).
 *
 * All medJobItems that are registered here will receive a distributed cancel events when closing the application at will.
 *
 * The JobItems need to make sure to have implemented the Cancel() method.
 *
 * @see medJobItem
 */
class MEDCORE_EXPORT medJobManager : public QObject
{
    Q_OBJECT

public:
    static medJobManager *instance();

    /**
    * registerJobItem - register a job item if you want that the manager sends cancel events to them (highly suggested!)
    * The manager will reject items if not active (see dispatchGlobalCancelEvent)
    * @param: const medJobItem & item
    * @param: QString jobName short name that will be visible on the progression toolboxes
    * @return   bool
    */
    bool registerJobItem(medJobItem* item, QString jobName= "");

    /**
    * unRegisterJobItem - remove the job from the hash
    * @param: const medJobItem & item
    * @return   bool
    */
    bool unRegisterJobItem(medJobItem* item);

    /**
    * dispatchGlobalCancelEvent - emits a cancel request to all registered items
    * @param bool ignoreNewJobItems - if set (default) the manager will not register any new items
    * @return   void
    */
    void dispatchGlobalCancelEvent(bool ignoreNewJobItems = true);

signals:
    void cancel(QObject*);

    void jobRegistered(medJobItem* item, QString jobName);

protected:
    medJobManager();
    ~medJobManager();

protected:
    static medJobManager *s_instance;

private:
    medJobManagerPrivate *d;
};


