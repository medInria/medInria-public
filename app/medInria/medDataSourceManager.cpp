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
#include <medPacsWidget.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>
#include <medMessageController.h>

#include <medFileSystemDataSource.h>
#include <medDatabaseDataSource.h>
#include <medPacsDataSource.h>
#include <medAbstractProcess.h>

#include <medApplication.h>

class medDataSourceManagerPrivate
{
public:
    QList <medAbstractDataSource*> dataSources;

    medDatabaseDataSource *dbSource;
    medFileSystemDataSource *fsSource;
    medPacsDataSource *pacsSource;

    // Fuse
    QList<medDataIndex> indexList;
    int numberOfPaths;
};

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

    foreach(QString dataSourceName, medAbstractDataSourceFactory::instance()->dataSourcePlugins())
    {
        qDebug()<< "factory creates dataSource:" << dataSourceName;
        medAbstractDataSource *dataSource = medAbstractDataSourceFactory::instance()->create(dataSourceName, 0);
        d->dataSources.push_back(dataSource);
        connectDataSource(dataSource);
    }

    connect(d->fsSource, SIGNAL(open(QString)),
            this, SLOT(openFromPath(QString)));
    connect(d->fsSource, SIGNAL(load(QString)),
            this, SLOT(loadFromPath(QString)));
    connect(d->fsSource, SIGNAL(fuse(QStringList)),
            this, SLOT(fuseFromPaths(QStringList)));
    connect(d->dbSource, SIGNAL(open(const medDataIndex&)),
            this, SLOT(openFromIndex(medDataIndex)));
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
    medDataManager::instance()->importPath(path, false);
}

void medDataSourceManager::fuseFromPaths(QStringList paths)
{
    d->indexList.clear();
    QList<QUuid> uuidList;
    d->numberOfPaths = paths.count();

    connect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex,QUuid)),
            this, SLOT(dataImported(medDataIndex,QUuid)));

    // Non persistent (temporary) loading
    foreach(QString path, paths)
    {
        uuidList.append(medDataManager::instance()->importPath(path, false));
    }

    // Wait for the end of loadings
    QEventLoop loop;
    connect(this, SIGNAL(loadingsFinished()), &loop, SLOT(quit()));
    loop.exec();

    disconnect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex,QUuid)),
               this, SLOT(dataImported(medDataIndex,QUuid)));

    dtkSmartPointer<medAbstractProcess> process =
            dtkAbstractProcessFactory::instance()->createSmartPointer("msc::MergeVolumes");

    if (process)
    {
        // Get back the successfully loaded medAbstractData and send them to the process
        QList<medAbstractData*> loadedDataList;
        foreach(medDataIndex dataIndex, d->indexList)
        {
            medAbstractData* data = medDataManager::instance()->retrieveData(dataIndex);
            loadedDataList.append(data);
            process->setInput(data);
        }

        // Fuse...
        process->setParameter(1); // fuse parameter
        process->update();

        // Save fused data
        if (process->output())
        {
            medDataManager::instance()->importData(process->output(), true);
        }
    }
    else
    {
        QString warning = "The Fuse process hasn't been found.";
        qDebug() << "medDataSourceManager::fuseFromPaths: " + warning;
        medMessageController::instance()->showInfo(warning, 6000);
    }

    // Remove temporary non-persistent data
    foreach(medDataIndex dataIndex, d->indexList)
    {
        medDataManager::instance()->removeData(dataIndex);
    }
}

void medDataSourceManager::dataImported(const medDataIndex& index, QUuid uuid)
{
    Q_UNUSED(uuid);

    d->indexList.append(index);

    // This was the last loading
    if (d->indexList.count() == d->numberOfPaths)
    {
        emit loadingsFinished();
    }
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

medDatabaseDataSource* medDataSourceManager::databaseDataSource()
{
    return d->dbSource;
}

medDataSourceManager *medDataSourceManager::s_instance = NULL;
