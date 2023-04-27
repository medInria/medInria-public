/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractDataFactory.h>
#include <medDataManager.h>
#include <medGlobalDefs.h>
#include <medJobManagerL.h>
#include <medMessageController.h>
#include <medPluginManager.h>
#include <medSettingsManager.h>

#include <QDebug>

/* THESE CLASSES NEED TO BE THREAD-SAFE, don't forget to lock the mutex in the
 * methods below that access state.
 */

class medDataManagerPrivate
{
public:
    medDataManagerPrivate(medDataManager *q)
        : q_ptr(q), mutex(QMutex::Recursive) {}

    void cleanupTracker()
    {
        QMutexLocker lock(&mutex);
        for (const medDataIndex &i : loadedDataObjectTracker.keys())
        {
            if (loadedDataObjectTracker.value(i).isNull())
            {
                loadedDataObjectTracker.remove(i);
            }
        }
    }

    Q_DECLARE_PUBLIC(medDataManager)

    medDataManager *const q_ptr;
    QMutex mutex;
    QHash<medDataIndex, dtkSmartPointer<medAbstractData> > loadedDataObjectTracker;

    QTimer timer;
    QHash<QUuid, medDataIndex> makePersistentJobs;

    medAbstractData *(*f)(medDataIndex const &); //TODO Remove ok c'est le truc le moins classe du monde (Part1)
    QUuid (*f2)(medAbstractData &, bool); //TODO Remove ok c'est le truc le moins classe du monde (Part1)
    void(*f3)(QString const &, QUuid); //TODO Remove ok c'est le truc le moins classe du monde (Part1)
};

// ------------------------- medDataManager -----------------------------------

medDataManager *medDataManager::s_instance = nullptr;

medDataManager *medDataManager::instance()
{
    if (!s_instance)
    {
        s_instance = new medDataManager();
    }
    return s_instance;
}

void medDataManager::setIndexV2Handler(medAbstractData *(*f)(medDataIndex const &), QUuid (*f2)(medAbstractData &, bool), void(*f3)(QString const &, QUuid))
{
    d_ptr->f = f;
    d_ptr->f2 = f2;
    d_ptr->f3 = f3;
}

medAbstractData *medDataManager::retrieveData(const medDataIndex &index)
{
    Q_D(medDataManager);

    if (index.isV2())
    {
        medAbstractData * dataTmp = nullptr; // d->loadedDataObjectTracker.value(index);;
        if (dataTmp == nullptr)
        {
            dataTmp = d->f(index); //TODO Remove ok c'est le truc le moins classe du monde (Part2)
        }
        return dataTmp;
    }

    return nullptr;
}

void medDataManager::loadData(const medDataIndex &index)
{
}

// ////////////////////////////////////////////////////////////////////////
// New behavior
QUuid medDataManager::importData(medAbstractData *data, bool persistent)
{
    QUuid uuid;
    if (!data)
        return QUuid();
    Q_D(medDataManager);

    uuid = d->f2(*data, persistent);

    return uuid;
}


// ////////////////////////////////////////////////////////////////////////
// New behavior
QUuid medDataManager::importPath(const QString &dataPath, bool indexWithoutCopying, bool persistent)
{
    if (!QFile::exists(dataPath))
        return QUuid();
    
    Q_D(medDataManager);

    QUuid uuid = QUuid::createUuid();
    
    d->f3(dataPath, uuid);

    return uuid;
}


/** @brief return writers able to handle the data *Memory management is the
   responsability of the caller*

*/
QHash<QString, dtkAbstractDataWriter *> medDataManager::getPossibleWriters(
    medAbstractData *data)
{
    Q_D(medDataManager);
    QList<QString> allWriters = medAbstractDataFactory::instance()->writers();
    QHash<QString, dtkAbstractDataWriter *> possibleWriters;

    for (QString writerType : allWriters)
    {
        dtkAbstractDataWriter *writer =
            medAbstractDataFactory::instance()->writer(writerType);
        if (writer->handled().contains(data->identifier()))
            possibleWriters[writerType] = writer;
        else
            delete writer;
    }
    if (possibleWriters.isEmpty())
        medMessageController::instance()->showError(
            "Sorry, we have no exporter for this format.");

    return possibleWriters;
}

void medDataManager::exportData(dtkSmartPointer<medAbstractData> data)
{
  
}

void medDataManager::exportDataToPath(dtkSmartPointer<medAbstractData> data, const QString & filename, const QString & writer)
{
 
}

QPixmap medDataManager::thumbnail(const medDataIndex & index)
{
    return QPixmap();
}

QList<medDataIndex> medDataManager::getSeriesListFromStudy(const medDataIndex &indexStudy)
{
    Q_D(medDataManager);
    QList<medDataIndex> indexList;
    return indexList;
}

QList<medDataIndex> medDataManager::moveStudy(const medDataIndex &indexStudy,
                                              const medDataIndex &toPatient)
{
    Q_D(medDataManager);
    QList<medDataIndex> newIndexList;
    return newIndexList;
}

medDataIndex medDataManager::moveSeries(const medDataIndex &indexSeries,
                                        const medDataIndex &toStudy)
{
    medDataIndex newIndex;
    return newIndex;
}

void medDataManager::exportDialog_updateSuffix(int index)
{
    QComboBox *typesHandled = qobject_cast<QComboBox *>(sender());
    if (!typesHandled)
        return;

    QFileDialog *exportDialog = qobject_cast<QFileDialog *>(
        typesHandled->itemData(index, Qt::UserRole + 2).value<QObject *>());
    QString extension =
        typesHandled->itemData(index, Qt::UserRole + 1).toString();

    QString currentFilename =
        med::smartBaseName(exportDialog->selectedFiles().first());
    currentFilename += extension;
    exportDialog->selectFile(currentFilename);
}

void medDataManager::garbageCollect()
{
}

void medDataManager::setWriterPriorities()
{
}

medDataManager::medDataManager() : d_ptr(new medDataManagerPrivate(this))
{
    Q_D(medDataManager);

    connect(medPluginManager::instance(), SIGNAL(allPluginsLoaded()), this, SLOT(setWriterPriorities()));
    connect(this, SIGNAL(dataImported(medDataIndex, QUuid)), this, SLOT(removeFromNonPersistent(medDataIndex, QUuid)));
}

void medDataManager::medDataHubRelay(medDataIndex index, QUuid uuid)
{
    emit dataImported(index , uuid);
}


medDataManager::~medDataManager() {}
