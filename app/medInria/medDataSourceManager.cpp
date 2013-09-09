/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDataSourceManager.h"

#include <QList>
#include <medAbstractDataSource.h>
#include <medAbstractDataSourceFactory.h>
#include <medDataManager.h>
#include <medPacsWidget.h>

#include "medFileSystemDataSource.h"
#include "medDatabaseDataSource.h"
#include "medPacsDataSource.h"

class medDataSourceManagerPrivate
{
public:
    QList <medAbstractDataSource*> dataSources;
    
    medDatabaseDataSource *dbSource;
    medFileSystemDataSource *fsSource;
    medPacsDataSource *pacsSource;
};

medDataSourceManager::medDataSourceManager(): d(new medDataSourceManagerPrivate)
{

}


void medDataSourceManager::ceateDataSource()
{
    // Data base data source
    
    d->dbSource = new medDatabaseDataSource(this);
    d->dataSources.push_back(d->dbSource);
    emit registered(d->dbSource);
    
    // File system data source
    
    d->fsSource = new medFileSystemDataSource(this);
    d->dataSources.push_back(d->fsSource);
    emit registered(d->fsSource);    
    
    // Pacs data source
    
    
    medPacsDataSource *pacsDataSource = new medPacsDataSource;
    medPacsWidget * mainPacsWidget = qobject_cast<medPacsWidget*> (pacsDataSource->largeViewWidget());
    //make the widget hide if not functional (otehrwise it flickers in and out).
    mainPacsWidget->hide();
    if (mainPacsWidget->isServerFunctional())
    {
        d->pacsSource = new medPacsDataSource(this);
        d->dataSources.push_back(d->pacsSource);
        emit registered(d->pacsSource); 
    }
    else mainPacsWidget->deleteLater();
    
    // dynamic data sources (from plugins) 
    
    foreach(QString dataSourceName, medAbstractDataSourceFactory::instance()->dataSourcePlugins()) 
    {
        qDebug()<< "factory creates dataSource:" << dataSourceName;
        medAbstractDataSource *dataSource = medAbstractDataSourceFactory::instance()->create(dataSourceName, this);
        d->dataSources.push_back(dataSource);       
        emit registered(dataSource);
    }
        

    connect(medDataManager::instance(), SIGNAL(dataAdded(const medDataIndex &)),
            d->dbSource, SLOT(update(const medDataIndex&)));
    connect(medDataManager::instance(), SIGNAL(dataRemoved(const medDataIndex &)),
            d->dbSource, SLOT(update(const medDataIndex&)));
    connect(medDataManager::instance(), SIGNAL(failedToOpen(const medDataIndex&)), 
            d->dbSource , SLOT(onOpeningFailed(const medDataIndex&)));
   
    connect(d->fsSource, SIGNAL(open(QString)), 
            this, SIGNAL(open(QString)));
    connect(d->fsSource, SIGNAL(load(QString)), 
            this, SIGNAL(load(QString)));
    connect(d->dbSource, SIGNAL(open(const medDataIndex&)), 
            this, SIGNAL(open(const medDataIndex&)));    
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

void medDataSourceManager::destroy( void )
{
    if (s_instance) {
        delete s_instance;
        s_instance = 0;
    }
}

medDataSourceManager *medDataSourceManager::s_instance = NULL;
