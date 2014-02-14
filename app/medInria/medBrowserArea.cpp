/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medBrowserArea.h"

#include <medBrowserSourceSelectorToolBox.h>
#include <medPacsDataSource.h>
#include <medDatabaseDataSource.h>
#include <medFileSystemDataSource.h>

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
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseExporter.h>
#include <medDatabaseImporter.h>

#include <medProgressionStack.h>
#include <medToolBox.h>
#include <medToolBoxFactory.h>
#include <medToolBoxContainer.h>
#include <medBrowserJobsToolBox.h>
#include <medPacsMover.h>
#include <medPacsWidget.h>
#include <medCompositeDataSetImporterSelectorToolBox.h>

class medBrowserAreaPrivate
{
public:
    medDatabaseDataSource* dbSource;
    medFileSystemDataSource* fsSource;
    medPacsDataSource* pacsSource;

    medToolBoxContainer *toolboxContainer;
    medBrowserJobsToolBox *jobsToolBox;
    medBrowserSourceSelectorToolBox *sourceSelectorToolBox;

    QList <medAbstractDataSource *> dataSources;

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

    d->sourceSelectorToolBox = new medBrowserSourceSelectorToolBox(this);
    connect(d->sourceSelectorToolBox, SIGNAL(indexChanged(int)), this, SLOT(onSourceIndexChanged(int)));

    // Jobs //////////////////////////////////////////

    d->jobsToolBox = new medBrowserJobsToolBox(this);
    d->jobsToolBox->setVisible(false);
    // connect the job-manager with the visual representation
    connect(medJobManager::instance(), SIGNAL(jobRegistered(medJobItem*, QString)),
        d->jobsToolBox->stack(),SLOT(addJobItem(medJobItem*, QString)));

    // Toolbox container /////////////////////////////////////////////

    d->toolboxContainer = new medToolBoxContainer(this);
    d->toolboxContainer->setObjectName("browserContainerToolbox");
    d->toolboxContainer->setFixedWidth(340);
    d->toolboxContainer->addToolBox(d->sourceSelectorToolBox);

    // static data sources ////////////////

    d->dbSource = new medDatabaseDataSource(this);
    addDataSource(d->dbSource);
    connect(d->dbSource, SIGNAL(open(const medDataIndex&)), this, SIGNAL(open(const medDataIndex&)));

    connect(medDataManager::instance(), SIGNAL(dataAdded(const medDataIndex &)),d->dbSource,SLOT(update(const medDataIndex&)));
    connect(medDataManager::instance(), SIGNAL(dataRemoved(const medDataIndex &)),d->dbSource,SLOT(update(const medDataIndex&)));
    connect(medDataManager::instance(), SIGNAL(openRequested(const medDataIndex &, int)), this, SIGNAL(openRequested(const medDataIndex&, int)));
    // This remains to be checked
    connect(medDatabaseController::instance(), SIGNAL(jobStarted(medJobItem*,QString)),this,SLOT(displayJobItem(medJobItem *, QString)));
    connect(medDatabaseNonPersistentController::instance(), SIGNAL(jobStarted(medJobItem*,QString)),this,SLOT(displayJobItem(medJobItem *, QString)));
    
    d->fsSource = new medFileSystemDataSource(this);
    addDataSource(d->fsSource);
    connect(d->fsSource, SIGNAL(open(QString)), this, SIGNAL(open(QString)));
    connect(d->fsSource, SIGNAL(load(QString)), this, SIGNAL(load(QString)));

    medPacsDataSource* pacsDataSource = new medPacsDataSource(this);

    medPacsWidget * mainPacsWidget = qobject_cast<medPacsWidget *> (pacsDataSource->mainViewWidget());
    //make the widget hide if not functional (otehrwise it flickers in and out).
    mainPacsWidget->hide();
    if (mainPacsWidget->isServerFunctional())
    {
        d->pacsSource = pacsDataSource;
        addDataSource(d->pacsSource);
    }
    else
    {
        d->pacsSource = NULL;
        pacsDataSource->deleteLater();
    }

    // dynamic data sources (from plugins) ////////////////
    foreach(QString dataSourceName, medAbstractDataSourceFactory::instance()->dataSourcePlugins()) {
        qDebug()<< "factory creates dataSource:" << dataSourceName;
        medAbstractDataSource *dataSource = medAbstractDataSourceFactory::instance()->create(dataSourceName,this);
        addDataSource(dataSource);
    }

    // Jobs should be added as the last item so that they appear at the bottom
    d->toolboxContainer->addToolBox(d->jobsToolBox);

    connect(this,SIGNAL(showError(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(const QString&,unsigned int)));

    // Layout /////////////////////////////////////////////
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->toolboxContainer);
    layout->addWidget(d->stack);

    // make toolboxes visible
    onSourceIndexChanged(d->stack->currentIndex());

    connect(medDataManager::instance(), SIGNAL(failedToOpen(const medDataIndex&)), this, SLOT(onOpeningFailed(const medDataIndex&)));

    medDatabaseControllerImpl* dbcont = medDatabaseController::instance().data();
    connect(dbcont, SIGNAL(partialImportAttempted ( const QString& )), this, SLOT(onPartialImportAttempted ( const QString& )));

    //Check if there are already item in the database, otherwise, switch to File system datasource
    QList<medDataIndex> indexes = medDatabaseNonPersistentController::instance()->availableItems();
    QList<medDataIndex> patients = medDatabaseController::instance()->patients();
    if (indexes.isEmpty() && patients.isEmpty())
    {
        d->sourceSelectorToolBox->setCurrentTab(1);
    }
 }

medBrowserArea::~medBrowserArea(void)
{
    foreach(medAbstractDataSource* source, d->dataSources)
    {
        source->deleteLater();
    }

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
    medDataManager::instance()->import(path,false);
}

void medBrowserArea::onFileIndex(QString path)
{
    medDataManager::instance()->import(path,true);

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
    d->jobsToolBox->stack()->addJobItem(importer, infoBaseName);
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

    dtkSmartPointer<dtkAbstractData> data_smart(data);

    medDataManager::instance()->import(data_smart);
}

void medBrowserArea::onDataReceivingFailed(QString fileName)
{
  emit(showError(tr("Unable to get from source the data named ") + fileName,3000));
}

void medBrowserArea::onSourceIndexChanged(int index)
{
    setToolBoxesVisible(d->stack->currentIndex(),false);
    setToolBoxesVisible(index, true);
    d->stack->setCurrentIndex(index);
}

void medBrowserArea::setToolBoxesVisible(int index, bool visible )
{
    QList<medToolBox*> toolBoxes = d->dataSources[index]->getToolBoxes();
    foreach(medToolBox* toolBox, toolBoxes)
        toolBox->setVisible(visible);
}

void medBrowserArea::addDataSource( medAbstractDataSource* dataSource )
{
    d->dataSources.push_back(dataSource);
    d->stack->addWidget(dataSource->mainViewWidget());
    d->sourceSelectorToolBox->addTab(dataSource->tabName(),dataSource->sourceSelectorWidget(),dataSource->description());
    QList<medToolBox*> toolBoxes = dataSource->getToolBoxes();
    foreach(medToolBox* toolBox, toolBoxes) {
        toolBox->setVisible(false);
        d->toolboxContainer->addToolBox(toolBox);
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
    dtkSmartPointer<dtkAbstractData> data = medDataManager::instance()->data(index);
    medDataManager::instance()->exportDataToFile(data);
}

void medBrowserArea::addToolBox(medToolBox *toolbox)
{
    d->toolboxContainer->addToolBox(toolbox);
}

void medBrowserArea::removeToolBox(medToolBox *toolbox)
{
    d->toolboxContainer->removeToolBox(toolbox);
}

void medBrowserArea::onDataRemoved( const medDataIndex &index )
{
    d->dbSource->update(index);
}
