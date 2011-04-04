
#ifndef medJobItem_h__
#define medJobItem_h__

#include <QtCore>

#include "medCoreExport.h"

/**
 * @class medJobItem
 * @author Michael Knopke
 * @brief Abstract base class for all QRunnables that should appear in the Job Toolbox
 * The base class should use the signals defined here, where sender is the pointer to the object.
 * The pointer is used to track the item in the ProgressionStack, it is necessary due to the non-blocking connection of Signals and Slots between threads.
 * A non-blocking connection is needed for QThreadPool::waitForDone()
 */
class MEDCORE_EXPORT medJobItem :  public QObject, public QRunnable
{
    Q_OBJECT

public:
             medJobItem();
    virtual ~medJobItem();
    
signals:
    void progressed(QObject* sender, int progress);
    void success (QObject* sender);
    void failure (QObject* sender);
    void showError(QObject* sender,const QString&,unsigned int timeout);
    void cancelled(QObject* sender);

public slots:

    /**
    * onCancel - Re-implement this, if your job is able to cancel itself (recommended). 
    * It should then emit cancelled(sender) to give a status to the ProgressionStack
    * @params: QObject *sender 
    * @return   void
    */
    virtual void onCancel(QObject*);

};

#endif // medJobItem_h__