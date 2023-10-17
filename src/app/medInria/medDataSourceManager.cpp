/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataSourceManager.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <QList>
#include <QUuid>
#include <medAbstractDataSource.h>
#include <medAbstractDataSourceFactory.h>
#include <medDataManager.h>
#include <medPacsWidget.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>
#include <medMessageController.h>

#include <medFileSystemDataSource.h>
#include <medDatabaseDataSource.h>
#include <medPacsDataSource.h>

#include <medApplication.h>

class medDataSourceManagerPrivate
{
public:
    QList <medAbstractDataSource*> dataSources;

    medDatabaseDataSource *dbSource;
    medFileSystemDataSource *fsSource;
    medPacsDataSource *pacsSource;
};

std::unique_ptr<medDataSourceManager> medDataSourceManager::s_instance = nullptr;

medDataSourceManager &medDataSourceManager::instance()
{
    if(!s_instance)
    {
        s_instance = std::unique_ptr<medDataSourceManager>(new medDataSourceManager());
    }
    return *s_instance.get();
}

medDataSourceManager::medDataSourceManager(): d(new medDataSourceManagerPrivate)
{
    // Data base data source
    d->dbSource = new medDatabaseDataSource();
    d->dataSources.push_back(d->dbSource);
    connectDataSource(d->dbSource);

    // File system data source
    d->fsSource = new medFileSystemDataSource();
    d->dataSources.push_back(d->fsSource);
    connectDataSource(d->fsSource);

    // Pacs data source
    medPacsDataSource *pacsDataSource = new medPacsDataSource;
    medPacsWidget * mainPacsWidget = qobject_cast<medPacsWidget*> (pacsDataSource->mainViewWidget());
    //make the widget hide if not functional (otehrwise it flickers in and out).
    mainPacsWidget->hide();
    if (mainPacsWidget->isServerFunctional())
    {
        d->pacsSource = new medPacsDataSource();
        d->dataSources.push_back(d->pacsSource);
        connectDataSource(d->pacsSource);
    }
    else mainPacsWidget->deleteLater();

    // dynamic data sources (from plugins)

    for(QString dataSourceName : medAbstractDataSourceFactory::instance()->dataSourcePlugins())
    {
        dtkDebug()<< "factory creates dataSource:" << dataSourceName;
        medAbstractDataSource *dataSource = medAbstractDataSourceFactory::instance()->create(dataSourceName, 0);
        d->dataSources.push_back(dataSource);
        connectDataSource(dataSource);
    }

    connect(d->fsSource, SIGNAL(open(QString)),
            this, SLOT(openFromPath(QString)));
    connect(d->fsSource, SIGNAL(load(QString)),
            this, SLOT(loadFromPath(QString)));
    connect(d->dbSource, SIGNAL(open(const medDataIndex &)),
            this, SLOT(openFromIndex(medDataIndex)));
}

void medDataSourceManager::connectDataSource(medAbstractDataSource *dataSource)
{
    connect(dataSource, SIGNAL(exportData(const medDataIndex &)),
            this, SLOT(exportData(const medDataIndex &)));

    connect(dataSource, SIGNAL(dataReceived(medAbstractData *)),
            this, SLOT(importData(medAbstractData *)));

    connect(dataSource, SIGNAL(dataReceivingFailed(QString)),
            this, SLOT(emitDataReceivingFailed(QString)));

    connect(dataSource, SIGNAL(dataToImportReceived(QString)),
            this, SLOT(importFile(QString)));

    connect(dataSource, SIGNAL(dataToFetchReceived(QHash<QString, QHash<QString, QVariant> >, QHash<QString, QHash<QString, QVariant> >)),
            this, SLOT(fetchData(QHash<QString, QHash<QString, QVariant> >, QHash<QString, QHash<QString, QVariant> >)));

    connect(dataSource, SIGNAL(updateProgress(int)), &medDataManager::instance(), SIGNAL(updateProgress(int)));
    connect(dataSource, SIGNAL(moveState(int, const QString &)), &medDataManager::instance(), SIGNAL(moveState(int, const QString &)));
    connect(&medDataManager::instance(), SIGNAL(moveRequested(const QString &, const QString &)), dataSource, SIGNAL(moveRequested(const QString &, const QString &)));
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
        dtkDebug() << "Cannot create directory: " << fileInfo.dir().path();
        return;
    }

    medDataManager::instance().importData(data, true);
}

void medDataSourceManager::exportData(const medDataIndex &index)
{
    //TODO did it all from the medDataManager ? - RDE
    dtkSmartPointer<medAbstractData> data = medDataManager::instance().retrieveData(index);
    medDataManager::instance().exportData(data);
}

void medDataSourceManager::importFile(QString path)
{
    medDataManager::instance().importPath(path, false, true);
}

void medDataSourceManager::fetchData(QHash<QString, QHash<QString, QVariant> > pData, QHash<QString, QHash<QString, QVariant> > sData)
{
    medDataManager::instance().fetchData(pData, sData);
}

void medDataSourceManager::emitDataReceivingFailed(QString fileName)
{
    medMessageController::instance().showError(tr("Unable to get from source the data named ") + fileName, 3000);
}

medDataSourceManager::~medDataSourceManager()
{
    delete d;
    d = nullptr;
}

/**
 * @brief medDataSourceManager::openFromPath temporary import, switch to
 * Visualization Workspace and display a data
 * @param path of the data to display
 */
void medDataSourceManager::openFromPath(QString path)
{
    qobject_cast<medApplication*>(qApp)->open(path);
}

void medDataSourceManager::openFromIndex(medDataIndex index)
{
    qobject_cast<medApplication*>(qApp)->open(index);
}

void medDataSourceManager::loadFromPath(QString path)
{
    medDataManager::instance().importPath(path, false);
}

QList <medAbstractDataSource*> medDataSourceManager::dataSources()
{
    return d->dataSources;
}

medDatabaseDataSource* medDataSourceManager::databaseDataSource()
{
    return d->dbSource;
}
