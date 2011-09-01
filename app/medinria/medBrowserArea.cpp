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

#include <medMessageController.h>
#include <medJobManager.h>
#include <medDataManager.h>
#include <medAbstractDataSource.h>
#include <medAbstractDataSourceFactory.h>
#include <medStorage.h>

#include <medDatabaseController.h>
#include <medDatabaseExporter.h>
#include <medDatabaseImporter.h>

#include <medProgressionStack.h>
#include <medToolBox.h>
#include <medToolBoxFactory.h>
#include <medToolBoxContainer.h>
#include <medBrowserToolBoxJobs.h>
#include <medPacsMover.h>
#include <medPacsWidget.h>
#include <medToolBoxCompositeDataSetImporter.h>

class medBrowserAreaPrivate
{
public:
    medDatabaseDataSource* dbSource;
    medFileSystemDataSource* fsSource;
    medPacsDataSource* pacsSource;

    medToolBoxContainer *toolbox_container;
    medBrowserToolBoxJobs *toolbox_jobs;
    medBrowserToolBoxSource *toolbox_source;
    medToolBoxCompositeDataSetImporter *toolbox_compositeimporter;

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

    d->dbSource = new medDatabaseDataSource();
    addDataSource(d->dbSource);
    connect(d->dbSource, SIGNAL(open(const medDataIndex&)), this,SIGNAL(open(const medDataIndex&)));
    connect(medDatabaseController::instance(), SIGNAL(updated(const medDataIndex &)),d->dbSource,SLOT(update(const medDataIndex&)));
    connect(medDatabaseController::instance(), SIGNAL(displayJobItem(medJobItem *, QString)),this,SLOT(displayJobItem(medJobItem *, QString)));

    d->fsSource = new medFileSystemDataSource(this);
    addDataSource(d->fsSource);
    connect(d->fsSource, SIGNAL(open(QString)), this, SIGNAL(open(QString)));
    connect(d->fsSource, SIGNAL(load(QString)), this, SIGNAL(load(QString)));

    d->pacsSource = new medPacsDataSource(this);

    medPacsWidget * mainPacsWidget = dynamic_cast<medPacsWidget *> (d->pacsSource->mainViewWidget());
    if (mainPacsWidget->isServerFunctional())
        addDataSource(d->pacsSource);

    // dynamic data sources (from plugins) ////////////////
    foreach(QString dataSourceName, medAbstractDataSourceFactory::instance()->dataSourcePlugins()) {
        qDebug()<< "factory creates dataSource:" << dataSourceName;
        medAbstractDataSource *dataSource = medAbstractDataSourceFactory::instance()->create(dataSourceName,this);
        addDataSource(dataSource);
    }

    // Jobs should be added as the last item so that they appear at the bottom
    d->toolbox_container->addToolBox(d->toolbox_jobs);

    connect(this,SIGNAL(showError(QObject*,const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(QObject*,const QString&,unsigned int)));

    d->toolbox_compositeimporter = new medToolBoxCompositeDataSetImporter(this);
    d->toolbox_compositeimporter->setVisible(true);
    d->toolbox_container->addToolBox(d->toolbox_compositeimporter);

    // Layout /////////////////////////////////////////////
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->stack);
    layout->addWidget(d->toolbox_container);

    // make toolboxes visible
    onSourceIndexChanged(d->stack->currentIndex());

    connect(medDataManager::instance(), SIGNAL(failedToOpen(const medDataIndex&)), this, SLOT(onOpeningFailed(const medDataIndex&)));
 }

medBrowserArea::~medBrowserArea(void)
{
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
    medDatabaseController::instance()->import(path,false);
}

void medBrowserArea::onFileIndex(QString path)
{
     medDatabaseController::instance()->import(path,true);
}

void medBrowserArea::onPartialImportAttempted(const QString& message)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Import warning");
    msgBox.setText(message);
    msgBox.exec();
}

void medBrowserArea::onOpeningFailed(const medDataIndex& index)
{
    d->dbSource->onOpeningFailed(index);
}

void medBrowserArea::displayJobItem(medJobItem *importer, QString infoBaseName)
{
    d->toolbox_jobs->stack()->addJobItem(importer, infoBaseName);
}

void medBrowserArea::onDataImport(dtkAbstractData *data)
{
    QString patientName = data->metaDataValues(medMetaDataKeys::PatientName.key())[0];
    QString studyName   = data->metaDataValues(medMetaDataKeys::StudyDescription.key())[0];
    QString seriesName  = data->metaDataValues(medMetaDataKeys::SeriesDescription.key())[0];

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

    medDatabaseController::instance()->import(data);
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

    connect(dataSource,SIGNAL(dataToImportReceived(QString)),this,SLOT(onFileImport(QString)));
    connect(dataSource,SIGNAL(dataToIndexReceived(QString)),this,SLOT(onFileIndex(QString)));
    connect(dataSource,SIGNAL(dataReceived(dtkAbstractData *)),this,SLOT(onDataImport(dtkAbstractData *)));
    connect(dataSource,SIGNAL(dataReceivingFailed(QString)), this, SLOT(onDataReceivingFailed(QString)));
    connect(dataSource, SIGNAL(exportData(const medDataIndex&)), this, SLOT(onExportData(const medDataIndex&)));
    connect(dataSource, SIGNAL(dataRemoved(const medDataIndex&)), this, SLOT(onDataRemoved(const medDataIndex&)));
}

void medBrowserArea::onExportData(const medDataIndex &index)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), "", "*.*");

    if (fileName.isEmpty())
        return;

    dtkSmartPointer<dtkAbstractData> data = medDataManager::instance()->data(index);

    if (!data)
        return;

    medDatabaseExporter *exporter = new medDatabaseExporter (data, fileName);

    connect(exporter, SIGNAL(progressed(QObject*,int)), d->toolbox_jobs->stack(), SLOT(setProgress(QObject*,int)));

    QThreadPool::globalInstance()->start(exporter);
}

void medBrowserArea::addToolBox(medToolBox *toolbox)
{
    d->toolbox_container->addToolBox(toolbox);
}

void medBrowserArea::removeToolBox(medToolBox *toolbox)
{
    d->toolbox_container->removeToolBox(toolbox);
}

void medBrowserArea::onDataRemoved( const medDataIndex &index )
{
    d->dbSource->update(index);
}
