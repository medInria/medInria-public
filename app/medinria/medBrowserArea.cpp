/* medBrowserArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 25 12:23:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 11:15:55 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 461
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medBrowserArea.h"

#include "medBrowserToolBoxSource.h"
#include "medPacsDataSource.h"
#include "medDatabaseDataSource.h"
#include "medFileSystemDataSource.h"

#include <QtGui>

#include <dtkCore/dtkGlobal.h>
#include <dtkGui/dtkFinder.h>

#include <medCore/medMessageController.h>
#include <medCore/medJobManager.h>
#include <medCore/medDataManager.h>
#include <medCore/medAbstractDataSource.h>
#include <medCore/medAbstractDataSourceFactory.h>
#include <medCore/medStorage.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseExporter.h>
#include <medSql/medDatabaseImporter.h>

#include <medGui/medProgressionStack.h>
#include <medGui/medToolBox.h>
#include <medGui/medToolBoxFactory.h>
#include <medGui/medToolBoxContainer.h>
#include <medGui/medBrowserToolBoxJobs.h>

#include <medPacs/medPacsMover.h>
#include <medPacs/medPacsWidget.h>

class medBrowserAreaPrivate
{
public:
    medDatabaseDataSource* dbSource;
    medFileSystemDataSource* fsSource;
    medPacsDataSource* pacsSource;

    medToolBoxContainer *toolbox_container;
    medBrowserToolBoxJobs *toolbox_jobs;
    medBrowserToolBoxSource *toolbox_source;

    QList <medAbstractDataSource *> data_sources;

    QStackedWidget *stack;
    QStatusBar *status;
};

// /////////////////////////////////////////////////////////////////
// medBrowserArea
// /////////////////////////////////////////////////////////////////

medBrowserArea::medBrowserArea(QWidget *parent) : QWidget(parent), d(new medBrowserAreaPrivate)
{
    // /////////////////////////////////////////////////////////////////

    d->stack = new QStackedWidget(this);

    // Source toolbox ///////////////////////////////////////////////

    d->toolbox_source = new medBrowserToolBoxSource(this);
    connect(d->toolbox_source, SIGNAL(indexChanged(int)), this, SLOT(onSourceIndexChanged(int)));

    // Jobs //////////////////////////////////////////

    d->toolbox_jobs = new medBrowserToolBoxJobs(this);
    d->toolbox_jobs->setVisible(false);
    // connect the job-manager with the visual representation
    connect(medJobManager::instance(), SIGNAL(jobRegistered(medJobItem*, QString)),
        d->toolbox_jobs->stack(),SLOT(addJobItem(medJobItem*, QString)));

    // Toolbox container /////////////////////////////////////////////

    d->toolbox_container = new medToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(300);
    d->toolbox_container->addToolBox(d->toolbox_source);

    // static data sources ////////////////

    d->dbSource = new medDatabaseDataSource(this);
    addDataSource(d->dbSource);    
    connect(d->dbSource, SIGNAL(open(const medDataIndex&)), this,SIGNAL(open(const medDataIndex&)));

    d->fsSource = new medFileSystemDataSource(this);
    addDataSource(d->fsSource);
    connect(d->fsSource, SIGNAL(open(QString)), this,SIGNAL(open(QString)));

    d->pacsSource = new medPacsDataSource();
    
    medPacsWidget * mainPacsWidget = dynamic_cast<medPacsWidget *> (d->pacsSource->mainViewWidget());
    if (mainPacsWidget->isServerFunctional())
        addDataSource(d->pacsSource);

    // dynamic data sources (from plugins) ////////////////
    foreach(QString dataSourceName, medAbstractDataSourceFactory::instance()->dataSourcePlugins()) {
        medAbstractDataSource *dataSource = medAbstractDataSourceFactory::instance()->create(dataSourceName);
        addDataSource(dataSource);
    }
  
    // Jobs should be added as the last item so that they appear at the bottom
    d->toolbox_container->addToolBox(d->toolbox_jobs);
  
    connect(this,SIGNAL(showError(QObject*,const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(QObject*,const QString&,unsigned int)));

    // Layout /////////////////////////////////////////////

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->stack);
    layout->addWidget(d->toolbox_container);

    // make toolboxes visible
    onSourceIndexChanged(d->stack->currentIndex());

 }

medBrowserArea::~medBrowserArea(void)
{
    d->pacsSource->deleteLater();
    delete d;
    d = NULL;
}

void medBrowserArea::setup(QStatusBar *status)
{
    d->status = status;
}

void medBrowserArea::setdw(QStatusBar *status)
{
    d->status = status;
}

void medBrowserArea::onFileImport(QString path)
{
    QFileInfo info(path);
    medDatabaseImporter *importer = new medDatabaseImporter(info.absoluteFilePath());
    connect(importer, SIGNAL(success(QObject*)), this, SLOT(onFileImported()), Qt::QueuedConnection);
    connect(importer, SIGNAL(failure(QObject*)), this, SLOT(onFileImported()), Qt::QueuedConnection);
    d->toolbox_jobs->stack()->addJobItem(importer, info.baseName());
    medJobManager::instance()->registerJobItem(importer);
    QThreadPool::globalInstance()->start(importer);
    
}

void medBrowserArea::onDataImport(dtkAbstractData *data)
{
    QString patientName = data->metaDataValues(tr("PatientName"))[0];
    QString studyName   = data->metaDataValues(tr("StudyDescription"))[0];
    QString seriesName  = data->metaDataValues(tr("SeriesDescription"))[0];
    
    QString s_patientName = patientName.simplified();
    QString s_studyName   = studyName.simplified();
    QString s_seriesName  = seriesName.simplified();  
    
    if ((s_patientName == "")||(s_studyName == "")||(s_seriesName == ""))
        return;
    
    QFileInfo fileInfo (medStorage::dataLocation() + "/" + s_patientName + "/" + s_studyName   + "/");
    
    if (!fileInfo.dir().exists() && !medStorage::mkpath (fileInfo.dir().path()))
    {
        qDebug() << "Cannot create directory: " << fileInfo.dir().path();
        return;
    }  
    
    medDataIndex importIndex = medDataManager::instance()->importNonPersistent(data);
    medDataManager::instance()->storeNonPersistentSingleDataToDatabase(importIndex);
    
    this->onFileImported();
}

void medBrowserArea::onFileImported(void)
{
    medDatabaseController::instance()->import("");
    d->dbSource->update();
}

void medBrowserArea::onDataReceivingFailed(QString fileName)
{
  emit(showError(this, tr("Unable to get from source the data named ") + fileName,3000));
}

void medBrowserArea::onSourceIndexChanged(int index)
{
    setToolBoxesVisible(d->stack->currentIndex(),false);
    setToolBoxesVisible(index, true);
    d->stack->setCurrentIndex(index);
}

void medBrowserArea::setToolBoxesVisible(int index, bool visible )
{
    QList<medToolBox*> toolBoxes = d->data_sources[index]->getToolboxes();
    foreach(medToolBox* toolBox, toolBoxes)
        toolBox->setVisible(visible);
}

void medBrowserArea::addDataSource( medAbstractDataSource* dataSource )
{
    d->data_sources.push_back(dataSource);
    d->stack->addWidget(dataSource->mainViewWidget());
    d->toolbox_source->addTab(dataSource->tabName(),dataSource->sourceSelectorWidget());
    QList<medToolBox*> toolBoxes = dataSource->getToolboxes();
    foreach(medToolBox* toolBox, toolBoxes) {
        toolBox->setVisible(false);
        d->toolbox_container->addToolBox(toolBox);
    }

    connect(dataSource,SIGNAL(dataReceived(QString)),this,SLOT(onFileImport(QString)));
    connect(dataSource,SIGNAL(dataReceived(dtkAbstractData *)),this,SLOT(onDataImport(dtkAbstractData *)));
    connect(dataSource,SIGNAL(dataReceivingFailed(QString)), this, SLOT(onDataReceivingFailed(QString)));
    connect(dataSource, SIGNAL(exportData(const medDataIndex&)), this, SLOT(onExportData(const medDataIndex&)));
}

void medBrowserArea::onExportData(const medDataIndex &index)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), "", "*.*");

    if (fileName.isEmpty())
        return;

    dtkAbstractData *data = medDataManager::instance()->data(index).data();

    if (!data)
        return;

    medDatabaseExporter *exporter = new medDatabaseExporter (data, fileName);
    
    connect(exporter, SIGNAL(progressed(QObject*,int)), d->toolbox_jobs->stack(), SLOT(setProgress(QObject*,int)));

    QThreadPool::globalInstance()->start(exporter);
}
