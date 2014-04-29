/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataManager.h>

#include <QDebug>

#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>

/* THESE CLASSES NEEDS TO BE THREAD-SAFE, don't forget to lock the mutex in the
 * methods below that access state.
 */

class medDataManagerPrivate {
public:
    medDataManagerPrivate(medDataManager * q)
        : q_ptr(q)
        , mutex(QMutex::Recursive)
    {
        dbController = medDatabaseController::instance();
        nonPersDbController = medDatabaseNonPersistentController::instance();

        if( ! dbController || ! nonPersDbController) {
            qCritical() << "One of the DB controllers could not be created !";
        }
    }
    medDataManager * const q_ptr;
    Q_DECLARE_PUBLIC(medDataManager)

    void cleanupTracker() {
        QMutexLocker lock(&mutex);
        foreach(const medDataIndex& i, loadedDataObjectTracker.keys()) {
            if (loadedDataObjectTracker.value(i).isNull()) {
                loadedDataObjectTracker.remove(i);
            }
        }
    }


    QMutex mutex;
    QHash<medDataIndex, QWeakPointer<medAbstractData> > loadedDataObjectTracker;
    medAbstractDbController * dbController;
    medAbstractDbController * nonPersDbController;
};


medDataManager *medDataManager::s_instance = NULL;

// Not thread-safe, but should only be called once, at application start-up
void medDataManager::initialize() {
    if ( ! s_instance) {
        s_instance = new medDataManager();
    }
}


medDataManager * medDataManager::instance() {
    return s_instance;
}


QSharedPointer<medAbstractData> medDataManager::retrieveData(const medDataIndex& index)
{
    Q_D(medDataManager);
    QMutexLocker locker(&(d->mutex));

    // If nothing in the tracker, we'll get a null weak pointer, thus a null shared pointer
    QSharedPointer<medAbstractData> dataObjRef = d->loadedDataObjectTracker.value(index).toStrongRef();

    if( ! dataObjRef.isNull()) {
        // we found an existing instance of that object
        return dataObjRef;
    }

    // No existing ref, we need to load from the file DB, then the non-persistent DB
    if (d->dbController->contains(index)) {
        dataObjRef = d->dbController->retrieve(index);
    } else if(d->nonPersDbController->contains(index)) {
        dataObjRef = d->nonPersDbController->retrieve(index);
    }

    if ( ! dataObjRef.isNull()) {
        dataObjRef->setDataIndex(index);
        d->loadedDataObjectTracker[index] = dataObjRef.toWeakRef();
        return dataObjRef;
    }
    // unlock mutex before emitting, as this could trigger code in others threads
    // which would try to access the data manager, and we don't want to deadlock.
    locker.unlock();

    emit retrievingFailed(index);
}


medDataManager::medDataManager()
    : d_ptr(new medDataManagerPrivate(this)) {

}

medDataManager::~medDataManager() {

}
