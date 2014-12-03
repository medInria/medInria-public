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


QList <medAbstractDataSource*> medDataSourceManager::dataSources()
{
    return d->dataSources;
}

void medDataSourceManager::connectDataSource(medAbstractDataSource *dataSource)
{
    connect(dataSource, SIGNAL(openRequest(const medDataIndex&)),
            this, SIGNAL(openRequest(const medDataIndex&)));
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
    if (s_instance)
    {
        delete s_instance;
        s_instance = 0;
    }
}


medDataSourceManager *medDataSourceManager::s_instance = NULL;
