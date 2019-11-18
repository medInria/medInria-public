/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QDebug>

#include <medAbstractDataFactory.h>
#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDataManager.h>
#include <medGlobalDefs.h>
#include <medJobManagerL.h>
#include <medMessageController.h>
#include <medPluginManager.h>

/* THESE CLASSES NEED TO BE THREAD-SAFE, don't forget to lock the mutex in the
 * methods below that access state.
 */

class medDataManagerPrivate
{
public:
    medDataManagerPrivate(medDataManager * q)
        : q_ptr(q)
        , mutex(QMutex::Recursive)
    {
        dbController = medDatabaseController::instance();
        nonPersDbController = medDatabaseNonPersistentController::instance();

        if( ! dbController || ! nonPersDbController) {
            qCritical() << "One of the DB controllers could not be created !";
        }
    }

    void cleanupTracker()
    {
        QMutexLocker lock(&mutex);
        foreach(const medDataIndex& i, loadedDataObjectTracker.keys())
            if (loadedDataObjectTracker.value(i).isNull())
                loadedDataObjectTracker.remove(i);
    }

    medAbstractDbController* controllerForDataSource(int id) {
        if (dbController->dataSourceId() == id)
            return dbController;
        else if (nonPersDbController->dataSourceId() == id)
            return nonPersDbController;
        else
            return NULL;
    }

    Q_DECLARE_PUBLIC(medDataManager)

    medDataManager * const q_ptr;
    QMutex mutex;
    QHash<medDataIndex, dtkSmartPointer<medAbstractData> > loadedDataObjectTracker;
    medAbstractDbController * dbController;
    medAbstractDbController * nonPersDbController;
    QTimer timer;
    QHash<QUuid, medDataIndex> makePersistentJobs;
};

// ------------------------- medDataManager -----------------------------------

medDataManager * medDataManager::s_instance = NULL;

// Not thread-safe, but should only be called once, at application start-up
void medDataManager::initialize()
{
    if ( ! s_instance) {
        s_instance = new medDataManager();
    }
}

medDataManager * medDataManager::instance()
{
    return s_instance;
}

medAbstractData* medDataManager::retrieveData(const medDataIndex& index)
{
    Q_D(medDataManager);
    QMutexLocker locker(&(d->mutex));

    // If nothing in the tracker, we'll get a null weak pointer, thus a null shared pointer
    medAbstractData *dataObjRef = d->loadedDataObjectTracker.value(index);

    if(dataObjRef)
    {
        // we found an existing instance of that object
        return dataObjRef;
    }

    // No existing ref, we need to load from the file DB, then the non-persistent DB
    if (d->dbController->contains(index)) {
        dataObjRef = d->dbController->retrieve(index);
    } else if(d->nonPersDbController->contains(index)) {
        dataObjRef = d->nonPersDbController->retrieve(index);
    }

    if (dataObjRef) {
        dataObjRef->setDataIndex(index);

        d->loadedDataObjectTracker.insert(index, dataObjRef);
        return dataObjRef;
    }
    return NULL;
}

QUuid medDataManager::importData(medAbstractData *data, bool persistent)
{
    if (!data)
        return QUuid();

    Q_D(medDataManager);
    QUuid uuid = QUuid::createUuid();
    medAbstractDbController * controller = persistent ?  d->dbController : d->nonPersDbController;
    controller->importData(data, uuid);
    return uuid;
}

QUuid medDataManager::importPath(const QString& dataPath, bool indexWithoutCopying, bool persistent)
{
    if ( ! QFile::exists(dataPath))
        return QUuid();

    Q_D(medDataManager);
    QUuid uuid = QUuid::createUuid();
    medAbstractDbController * controller = persistent ?  d->dbController : d->nonPersDbController;
    controller->importPath(dataPath, uuid, indexWithoutCopying);
    return uuid;
}
/** @brief return writers able to handle the data *Memory management is the responsability of the caller*

*/
QHash<QString, dtkAbstractDataWriter*> medDataManager::getPossibleWriters(medAbstractData* data)
{
    Q_D(medDataManager);
    QList<QString> allWriters = medAbstractDataFactory::instance()->writers();
    QHash<QString, dtkAbstractDataWriter*> possibleWriters;

    foreach(QString writerType, allWriters)
    {
        dtkAbstractDataWriter * writer = medAbstractDataFactory::instance()->writer(writerType);
        if (writer->handled().contains(data->identifier()))
            possibleWriters[writerType] = writer;
        else
            delete writer;
    }
    if (possibleWriters.isEmpty())
        medMessageController::instance()->showError("Sorry, we have no exporter for this format.");

    return possibleWriters;
}

void medDataManager::exportData(medAbstractData* data)
{
    if (!data)
        return;

    Q_D(medDataManager);
    QList<QString> allWriters = medAbstractDataFactory::instance()->writers();
    QHash<QString, dtkAbstractDataWriter*> possibleWriters=getPossibleWriters(data);

    QFileDialog * exportDialog = new QFileDialog(0, tr("Exporting: please choose a file name and directory"));
    exportDialog->setOption(QFileDialog::DontUseNativeDialog);
    exportDialog->setAcceptMode(QFileDialog::AcceptSave);

    QComboBox* typesHandled = new QComboBox(exportDialog);
    // we use allWriters as the list of keys to make sure we traverse possibleWriters
    // in the order specified by the writers priorities.
    foreach(QString type, allWriters)
    {
        if (!possibleWriters.contains(type))
            continue;

        QStringList extensionList = possibleWriters[type]->supportedFileExtensions();
        QString label = possibleWriters[type]->description() + " (" + extensionList.join(", ") + ")";
        QString extension = (extensionList.isEmpty()) ? QString() : extensionList.first();
        typesHandled->addItem(label, type);
        typesHandled->setItemData(typesHandled->count()-1, extension, Qt::UserRole+1);
        typesHandled->setItemData(typesHandled->count()-1, QVariant::fromValue<QObject*>(exportDialog), Qt::UserRole+2);
    }
    connect(typesHandled, SIGNAL(currentIndexChanged(int)), this, SLOT(exportDialog_updateSuffix(int)));

    QLayout* layout = exportDialog->layout();
    QGridLayout* gridbox = qobject_cast<QGridLayout*>(layout);

    // nasty hack to hide the filter list
    QWidget * filtersLabel = gridbox->itemAtPosition(gridbox->rowCount()-1, 0)->widget();
    QWidget * filtersList = gridbox->itemAtPosition(gridbox->rowCount()-1, 1)->widget();
    filtersLabel->hide(); filtersList->hide();

    if (gridbox) {
        gridbox->addWidget(new QLabel("Export format:", exportDialog), gridbox->rowCount()-1, 0);
        gridbox->addWidget(typesHandled, gridbox->rowCount()-1, 1);
    }

    exportDialog->setLayout(gridbox);

    // Set a default filename based on the series's description
    medAbstractDbController * dbController = d->controllerForDataSource(data->dataIndex().dataSourceId());
    if (dbController) {
        QString defaultName = dbController->metaData(data->dataIndex(), medMetaDataKeys::SeriesDescription);
        defaultName += typesHandled->itemData(typesHandled->currentIndex(), Qt::UserRole+1).toString();
        exportDialog->selectFile(defaultName);
    }

    if ( exportDialog->exec() )
    {
        // Chosen format in combobox. Ex. "vtkDataMesh"
        QString chosenFormat = typesHandled->itemData(typesHandled->currentIndex()).toString();

        // Combobox extension. Ex. ".vtk"
        QString comboExtension = typesHandled->itemData(typesHandled->currentIndex(), Qt::UserRole+1).toString();

        // Chosen extension in filename. Ex. "vtk"
        QString finalFilename = exportDialog->selectedFiles().first().toUtf8();
        QString userExtension = QFileInfo(finalFilename).suffix();

        // Some extensions are linked to several formats:
        // if the combobox and filename extensions are equal, no need to enter here.
        if (!userExtension.isEmpty() && (comboExtension != ("."+userExtension)))
        {
            foreach(QString type, allWriters)
            {
                if (possibleWriters.contains(type))
                {
                    QStringList extensionList = possibleWriters[type]->supportedFileExtensions();

                    if (extensionList.contains("." + userExtension))
                    {
                        // User has the last word about the file format, if it's a known format
                        chosenFormat = type;
                        break;
                    }
                }
            }
        }

        // Send final type to export data
        this->exportDataToPath(data, finalFilename, chosenFormat);
    }

    qDeleteAll(possibleWriters);
    delete exportDialog;
}

void medDataManager::exportDataToPath(medAbstractData *data, const QString & filename, const QString & writer)
{
    medDatabaseExporter *exporter = new medDatabaseExporter (data, filename, writer);
    launchExporter(exporter, filename);
}

void medDataManager::launchExporter(medDatabaseExporter* exporter, const QString & filename)
{
    QFileInfo info(filename);
    medMessageProgress *message = medMessageController::instance()->showProgress("Exporting data to " + info.baseName());

    connect(exporter, SIGNAL(progressed(int)), message, SLOT(setProgress(int)));
    connect(exporter, SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(exporter, SIGNAL(failure(QObject *)), message, SLOT(failure()));
    connect(exporter, SIGNAL(success(QObject *)), this, SIGNAL(exportFinished()));
    connect(exporter, SIGNAL(failure(QObject *)), this, SIGNAL(exportFinished()));

    medJobManagerL::instance()->registerJobItem(exporter);
    QThreadPool::globalInstance()->start(exporter);
}

QList<medDataIndex> medDataManager::getSeriesListFromStudy(const medDataIndex& indexStudy)
{
    Q_D(medDataManager);
    QList<medDataIndex> indexList;

    medAbstractDbController * dbc = d->controllerForDataSource(indexStudy.dataSourceId());

    if (dbc)
    {
        // Get the list of each series from that study index
        indexList = dbc->series(indexStudy);
    }

    return indexList;
}

QList<medDataIndex> medDataManager::moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient)
{
    Q_D(medDataManager);
    QList<medDataIndex> newIndexList;
    medAbstractDbController * dbc = d->controllerForDataSource(indexStudy.dataSourceId());
    if (!dbc) {
        return newIndexList;
    }

    if(dbc->dataSourceId() != toPatient.dataSourceId()) {
        qWarning() << "medDataManager: Moving data accross controllers is not supported.";
    } else {
        newIndexList = dbc->moveStudy(indexStudy,toPatient);
    }

    return newIndexList;
}

medDataIndex medDataManager::moveSeries(const medDataIndex& indexSeries, const medDataIndex& toStudy)
{
    Q_D(medDataManager);
    medAbstractDbController * dbc = d->controllerForDataSource(indexSeries.dataSourceId());
    if (!dbc) {
        return medDataIndex();
    }

    medDataIndex newIndex;

    if(dbc->dataSourceId() != toStudy.dataSourceId()) {
        qWarning() << "medDataManager: Moving data accross controllers is not supported.";
    } else {
        newIndex = dbc->moveSeries(indexSeries,toStudy);
    }

    return newIndex;
}

medAbstractDbController *medDataManager::controllerForDataSource(int dataSourceId)
{
    Q_D(medDataManager);
    return d->controllerForDataSource(dataSourceId);
}

void medDataManager::exportDialog_updateSuffix(int index)
{
    QComboBox * typesHandled = qobject_cast<QComboBox*>(sender());
    if (! typesHandled) return;

    QFileDialog * exportDialog = qobject_cast<QFileDialog*>(typesHandled->itemData(index, Qt::UserRole+2).value<QObject*>());
    QString extension = typesHandled->itemData(index, Qt::UserRole+1).toString();

    QString currentFilename = med::smartBaseName(exportDialog->selectedFiles().first());
    currentFilename += extension;
    exportDialog->selectFile(currentFilename);
}

void medDataManager::garbageCollect()
{
    Q_D(medDataManager);
    QMutexLocker locker(&(d->mutex));

    // garbage collect data that are only referenced by the manager
    QMutableHashIterator <medDataIndex, dtkSmartPointer<medAbstractData> > it(d->loadedDataObjectTracker);
    while(it.hasNext()) {
        it.next();
        medAbstractData *data = it.value();
        if(data->count() <= 1) {
            qDebug()<<"medDataManager garbage collected " << data->dataIndex();
            it.remove();
        }
    }
}

QUuid medDataManager::makePersistent(medAbstractData* data)
{
    if (!data)
    {
        return QUuid();
    }

    Q_D(medDataManager);

    // If already persistent
    if (data->dataIndex().dataSourceId() == d->dbController->dataSourceId())
    {
        return QUuid();
    }

    QUuid jobUuid;

    if(data->dataIndex().isValidForSeries())
    {
        jobUuid = this->importData(data, true);
        d->makePersistentJobs.insert(jobUuid, data->dataIndex());
    }
    else if( data->dataIndex().isValidForStudy() )
    {
        foreach(medDataIndex index, d->nonPersDbController->series(data->dataIndex()))
            jobUuid = makePersistent(this->retrieveData(index));

    }
    else if( data->dataIndex().isValidForPatient())
    {
        foreach(medDataIndex index, d->nonPersDbController->studies(data->dataIndex()))
            jobUuid = makePersistent(this->retrieveData(index));
    }

    return jobUuid;
}

QString medDataManager::getMetaData(const medDataIndex& index, const QString& key)
{
    Q_D(medDataManager);
    medAbstractDbController* dbc = d->controllerForDataSource(index.dataSourceId());

    if (dbc != nullptr)
    {
        return dbc->metaData(index, key);
    }
    else
    {
        return QString();
    }
}

bool medDataManager::setMetadata(const medDataIndex& index, const QString& key, const QString& value)
{
    Q_D(medDataManager);
    medAbstractDbController * dbc = d->controllerForDataSource( index.dataSourceId() );

    if((dbc != nullptr) && (dbc->setMetaData(index, key, value)))
    {
        emit metadataModified(index, key, value);
        return true;
    }

    return false;
}

void medDataManager::removeData(const medDataIndex& index)
{
    Q_D(medDataManager);
    medAbstractDbController * dbc = d->controllerForDataSource(index.dataSourceId());
    if (dbc) {
        dbc->remove(index);
    }
}

void medDataManager::removeFromNonPersistent(medDataIndex indexImported, QUuid uuid)
{
    Q_D(medDataManager);
    if(!d->makePersistentJobs.contains(uuid))
        return;

    this->removeData(d->makePersistentJobs.value(uuid));
    d->makePersistentJobs.remove(uuid);
}

QPixmap medDataManager::thumbnail(const medDataIndex & index)
{
    Q_D(medDataManager);
    medAbstractDbController* dbc = d->controllerForDataSource(index.dataSourceId());

    QPixmap pix;
    // dbc is NULL when called from the importer, as data is not imported yet
    if (dbc)
    {
        pix = dbc->thumbnail(index);
    }

    return pix.isNull() ? QPixmap(":/pixmaps/default_thumbnail.png") : pix;
}

void medDataManager::setWriterPriorities()
{
    QList<QString> writers = medAbstractDataFactory::instance()->writers();
    QMap<int, QString> writerPriorites;

    // set vtkDataMeshWriter as a top priority writer
    if(writers.contains("vtkDataMeshWriter"))
    {
        writers.move(writers.indexOf("vtkDataMeshWriter"),0);
    }

    // set itkMetaDataImageWriter as a top priority writer
    if(writers.contains("itkMetaDataImageWriter"))
    {
        writers.move(writers.indexOf("itkMetaDataImageWriter"),1);
    }

    for ( int i=0; i<writers.size(); i++ )
    {
        writerPriorites.insert(i, writers[i]);
    }

    medAbstractDataFactory::instance()->setWriterPriorities(writerPriorites);
}

medDataManager::medDataManager() : d_ptr(new medDataManagerPrivate(this))
{
    Q_D(medDataManager);
    QList<medAbstractDbController*> controllers;
    controllers << d->dbController << d->nonPersDbController;
    foreach(medAbstractDbController* controller, controllers) {
        connect(controller, SIGNAL(dataImported(medDataIndex,QUuid)), this, SIGNAL(dataImported(medDataIndex,QUuid)));
        connect(controller, SIGNAL(dataRemoved(medDataIndex)), this, SIGNAL(dataRemoved(medDataIndex)));
        connect(controller, SIGNAL(metadataModified(medDataIndex,QString,QString)), this, SIGNAL(metadataModified(medDataIndex,QString,QString)));
    }

    connect(&(d->timer), SIGNAL(timeout()), this, SLOT(garbageCollect()));
    d->timer.start(5*1000);

    connect(medPluginManager::instance(), SIGNAL(allPluginsLoaded()), this, SLOT(setWriterPriorities()));
    connect(this, SIGNAL(dataImported(medDataIndex,QUuid)), this, SLOT(removeFromNonPersistent(medDataIndex,QUuid)));
}

medDataManager::~medDataManager()
{
}
