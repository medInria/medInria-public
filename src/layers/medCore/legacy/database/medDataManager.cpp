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
#include <medDataHub.h>
#include <medGlobalDefs.h>
#include <medMessageController.h>
#include <medPluginManager.h>
#include <medSettingsManager.h>


class medDataManagerPrivate
{
public:
    medDataManagerPrivate(medDataManager *q) : q_ptr(q) {}

    Q_DECLARE_PUBLIC(medDataManager)

    medDataManager *const q_ptr;
    
};

// ------------------------- medDataManager -----------------------------------

std::unique_ptr<medDataManager> medDataManager::s_instance = nullptr;

medDataManager &medDataManager::instance()
{
    if(!s_instance)
    {
        s_instance = std::unique_ptr<medDataManager>(new medDataManager());
    }
    return *s_instance.get();
}

medAbstractData* medDataManager::retrieveData(const medDataIndex &index)
{
    Q_D(medDataManager);

    return medDataHub::instance()->getData(index);
}

QList<medAbstractData*> medDataManager::retrieveDataList(const medDataIndex &index)
{
    Q_D(medDataManager);

    return medDataHub::instance()->getDataList(index);
}

void medDataManager::loadData(const medDataIndex &index) {}

// ////////////////////////////////////////////////////////////////////////
// New behavior
QUuid medDataManager::importData(medAbstractData *data, bool persistent)
{
    Q_D(medDataManager);
    return medDataHub::instance()->writeResultsHackV3(*data, persistent); 
}


// ////////////////////////////////////////////////////////////////////////
// New behavior
QUuid medDataManager::importPath(const QString &dataPath, bool indexWithoutCopying, bool persistent)
{
    Q_D(medDataManager);
    if (!QFile::exists(dataPath))
        return QUuid();    

    QUuid uuid = QUuid::createUuid();   
    medDataHub::instance()->loadDataFromLocalFileSystem(dataPath, uuid);
    return uuid;
}


/** @brief return writers able to handle the data *Memory management is the
   responsability of the caller*
*/
QHash<QString, dtkAbstractDataWriter *> medDataManager::getPossibleWriters(medAbstractData *data)
{
    Q_D(medDataManager);
    QList<QString> allWriters = medAbstractDataFactory::instance()->writers();
    QHash<QString, dtkAbstractDataWriter *> possibleWriters;

    for (QString writerType : allWriters)
    {
        dtkAbstractDataWriter *writer = medAbstractDataFactory::instance()->writer(writerType);
        if (writer->handled().contains(data->identifier()))
            possibleWriters[writerType] = writer;
        else
            delete writer;
    }
    if (possibleWriters.isEmpty())
        medMessageController::instance().showError("Sorry, we have no exporter for this format.");

    return possibleWriters;
}

void medDataManager::exportData(dtkSmartPointer<medAbstractData> data) {}

void medDataManager::exportDataToPath(dtkSmartPointer<medAbstractData> data, const QString & filename, const QString & writer) {}

int medDataManager::getDataType(const medDataIndex & index)
{
    Q_D(medDataManager);
    return medDataHub::instance()->getDataType(index);
}

QList<medDataIndex> medDataManager::getSubData(const medDataIndex & index)
{
    Q_D(medDataManager);
    return medDataHub::instance()->getSubData(index);
}

QPixmap medDataManager::thumbnail(const medDataIndex & index)
{
    return QPixmap();
}

QList<medDataIndex> medDataManager::getSeriesListFromStudy(const medDataIndex &indexStudy)
{
    return QList<medDataIndex>();
}

QList<medDataIndex> medDataManager::moveStudy(const medDataIndex &indexStudy, const medDataIndex &toPatient)
{
    return QList<medDataIndex>();
}

medDataIndex medDataManager::moveSeries(const medDataIndex &indexSeries, const medDataIndex &toStudy)
{
    return medDataIndex();
}

void medDataManager::exportDialog_updateSuffix(int index)
{
    QComboBox *typesHandled = qobject_cast<QComboBox *>(sender());
    if (!typesHandled)
        return;

    QFileDialog *exportDialog = qobject_cast<QFileDialog *>(typesHandled->itemData(index, Qt::UserRole + 2).value<QObject *>());
    QString extension = typesHandled->itemData(index, Qt::UserRole + 1).toString();

    QString currentFilename = med::smartBaseName(exportDialog->selectedFiles().first());
    currentFilename += extension;
    exportDialog->selectFile(currentFilename);
}

medDataManager::medDataManager() : d_ptr(new medDataManagerPrivate(this))
{
    Q_D(medDataManager);
}

void medDataManager::medDataHubRelay(medDataIndex index, QUuid uuid)
{
    emit dataImported(index , uuid);
}

medDataManager::~medDataManager()
{
    delete d_ptr;
}
