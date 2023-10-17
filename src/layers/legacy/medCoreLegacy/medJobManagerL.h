#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtCore/QObject>

#include <medCoreLegacyExport.h>
#include <medDataIndex.h>

#include <memory>

class medJobManagerLPrivate;
class medJobItemL;

/**
 * @class medJobManagerL
 * @author Michael Knopke
 * @brief Manages safe termination of medJobItemLs (QRunnables).
 *
 * All medJobItemLs that are registered here will receive a distributed cancel events when closing the application at will.
 *
 * The JobItems need to make sure to have implemented the Cancel() method.
 *
 * @see medJobItemL
 */
class MEDCORELEGACY_EXPORT medJobManagerL : public QObject
{
    Q_OBJECT

public:
    ~medJobManagerL();

    static medJobManagerL &instance();

    /**
    * registerJobItem - register a job item if you want that the manager sends cancel events to them (highly suggested!)
    * The manager will reject items if not active (see dispatchGlobalCancelEvent)
    * @param: const medJobItemL & item
    * @param: QString jobName short name that will be visible on the progression toolboxes
    * @return   bool
    */
    bool registerJobItem(medJobItemL* item, QString jobName= "");

    /**
    * unRegisterJobItem - remove the job from the hash
    * @param: const medJobItemL & item
    * @return   bool
    */
    bool unRegisterJobItem(medJobItemL* item);

    /**
    * dispatchGlobalCancelEvent - emits a cancel request to all registered items
    * @param bool ignoreNewJobItems - if set (default) the manager will not register any new items
    * @return   void
    */
    void dispatchGlobalCancelEvent(bool ignoreNewJobItems = true);

signals:
    void cancel(QObject*);

    void jobRegistered(medJobItemL* item, QString jobName);

protected:
    medJobManagerL();

    static std::unique_ptr<medJobManagerL> s_instance;

private:
    medJobManagerLPrivate *d;
};
