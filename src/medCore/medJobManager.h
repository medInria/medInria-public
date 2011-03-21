#ifndef medJobManager_h__
#define medJobManager_h__

#include <QtCore/QObject>

#include "medCoreExport.h"
#include "medDataIndex.h"

class medJobManagerPrivate;
class medJobItem;

/**
 * @class medJobManager
 * @author Michael Knopke
 * @brief Manages safe termination of medJobItems (QRunnables) 
 * All medJobItems that are registered here will receive a distributed cancel events when closing the application at will.
 * The JobItems need to make sure to have implemented the Cancel() method
 */
class MEDCORE_EXPORT medJobManager : public QObject
{
    Q_OBJECT

public:
    static medJobManager *instance(void);

    /**
    * registerJobItem - register a job item if you want that the manager sends cancel events to them (highly suggested!)
    * @params: const medJobItem & item
    * @return   bool
    */
    bool registerJobItem(medJobItem* item);

    /**
    * unRegisterJobItem - remove the job from the hash
    * @params: const medJobItem & item
    * @return   bool
    */
    bool unRegisterJobItem(medJobItem* item);

    /**
    * dispatchGlobalCancelEvent - emits a cancel request to all registered items
    * @return   void
    */
    void dispatchGlobalCancelEvent();

signals:
    void cancel(QObject*);
 
protected:
    medJobManager(void);
    ~medJobManager(void);

protected:
    static medJobManager *s_instance;

private:
    medJobManagerPrivate *d;
};

#endif // medJobManager_h__