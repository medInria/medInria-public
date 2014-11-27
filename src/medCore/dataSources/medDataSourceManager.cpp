/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataSourceManager.h>
#include <dtkCore>

#include <QList>
#include <QUuid>
#include <medAbstractDataSource.h>
#include <medAbstractDataSourceFactory.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>
#include <medMessageController.h>

class medDataSourceManagerPrivate
{
public:
    QList <medAbstractDataSource*> dataSources;


};

medDataSourceManager::medDataSourceManager(): d(new medDataSourceManagerPrivate)
{
    foreach(QString dataSourceName, medAbstractDataSourceFactory::instance()->dataSources())
    {
        qDebug()<< "factory creates dataSource:" << dataSourceName;
        medAbstractDataSource *dataSource = medAbstractDataSourceFactory::instance()->create(dataSourceName, 0);
        d->dataSources.push_back(dataSource);
        connectDataSource(dataSource);
    }
}


void medDataSourceManager::connectDataSource(medAbstractDataSource *dataSource)
{
    connect(dataSource, SIGNAL(exportData(const medDataIndex&)),
            this, SLOT(exportData(const medDataIndex&)));

    connect(dataSource, SIGNAL(dataReceived(medAbstractData*)),
            this, SLOT(importData(medAbstractData*)));

    connect(dataSource, SIGNAL(dataReceivingFailed(QString)),
            this, SLOT(emitDataReceivingFailed(QString)));

    connect(dataSource, SIGNAL(dataToImportReceived(QString)),
            this, SLOT(importFile(QString)));

    connect(dataSource, SIGNAL(dataToIndexReceived(QString)),
            this, SLOT(indexFile(QString)));

    connect(dataSource, SIGNAL(open(QString)),
            this, SIGNAL(openFromPathRequest(QString)));
    connect(dataSource, SIGNAL(load(QString)),
            this, SLOT(loadFromPath(QString)));
    connect(dataSource, SIGNAL(open(const medDataIndex&)),
            this, SIGNAL(openFromIndexRequest(medDataIndex)));
}

//TODO: Maybe it is not the best place to put it (medDataManager?)
void medDataSourceManager::importData(medAbstractData *data)
{
    QString patientName = data->metaDataValues(medMetaDataKeys::PatientName.key())[0];
    QString studyName = data->metaDataValues(medMetaDataKeys::StudyDescription.key())[0];
    QString seriesName = data->metaDataValues(medMetaDataKeys::SeriesDescription.key())[0];

    QString s_patientName = patientName.simplified();
    QString s_studyName = studyName.simplified();
    QString s_seriesName = seriesName.simplified();

    if ((s_patientName == "")||(s_studyName == "")||(s_seriesName == ""))
        return;

    QFileInfo fileInfo (medStorage::dataLocation() + "/" + s_patientName + "/" + s_studyName + "/");

    if (!fileInfo.dir().exists() && !medStorage::mkpath (fileInfo.dir().path()))
    {
        qDebug() << "Cannot create directory: " << fileInfo.dir().path();
        return;
    }

    medDataManager::instance()->importData(data, true);
}

void medDataSourceManager::exportData(const medDataIndex &index)
{
    //TODO did it all from the medDataManager ? - RDE
    dtkSmartPointer<medAbstractData> data = medDataManager::instance()->retrieveData(index);
    medDataManager::instance()->exportData(data);
}

void medDataSourceManager::importFile(QString path)
{
    medDataManager::instance()->importPath(path, false, true);
}

void medDataSourceManager::indexFile(QString path)
{
    medDataManager::instance()->importPath(path, true, true);
}


void medDataSourceManager::emitDataReceivingFailed(QString fileName)
{
    medMessageController::instance()->showError(tr("Unable to get from source the data named ") + fileName, 3000);
}

medDataSourceManager * medDataSourceManager::instance( void )
{
    if(!s_instance)
        s_instance = new medDataSourceManager;
    return s_instance;
}

medDataSourceManager::~medDataSourceManager( void )
{
    delete d;
    d = NULL;
}

void medDataSourceManager::loadFromPath(QString path)
{
    medDataManager::instance()->importPath(path, false);
}

void medDataSourceManager::destroy( void )
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = 0;
    }
}

QList <medAbstractDataSource*> medDataSourceManager::dataSources()
{
    return d->dataSources;
}

medDataSourceManager *medDataSourceManager::s_instance = NULL;
