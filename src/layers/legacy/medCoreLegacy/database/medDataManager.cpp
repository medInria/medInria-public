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
#include <medDatabaseNonPersistentController.h>
#include <medDataPacsController.h>
#include <medGlobalDefs.h>
#include <medJobManagerL.h>
#include <medLocalDbController.h>
#include <medMessageController.h>
#include <medPluginManager.h>
#include <medSettingsManager.h>
#include <medStorage.h>

#include <QDebug>

/* THESE CLASSES NEED TO BE THREAD-SAFE, don't forget to lock the mutex in the
 * methods below that access state.
 */

class medDataManagerPrivate
{
public:
    medDataManagerPrivate(medDataManager *q)
        : q_ptr(q), mutex(QMutex::Recursive) {}

    void cleanupTracker()
    {
        QMutexLocker lock(&mutex);
        for (const medDataIndex &i : loadedDataObjectTracker.keys())
        {
            if (loadedDataObjectTracker.value(i).isNull())
            {
                loadedDataObjectTracker.remove(i);
            }
        }
    }

    medAbstractDbController *controllerForDataSource(int id)
    {
        if (dbController->dataSourceId() == id)
            return dbController;
        else if (nonPersDbController->dataSourceId() == id)
        {
            return nonPersDbController;
        }
        else if (pacsController->dataSourceId() == id)
        {
            return pacsController;
        }
        else
        {
            return nullptr;
        }
    }

    Q_DECLARE_PUBLIC(medDataManager)

    medDataManager *const q_ptr;
    QMutex mutex;
    QHash<medDataIndex, dtkSmartPointer<medAbstractData> >
        loadedDataObjectTracker;
    medDatabasePersistentController *dbController;
    medAbstractDbController *nonPersDbController;
    medAbstractDbController *pacsController;
    QTimer timer;
    QHash<QUuid, medDataIndex> makePersistentJobs;
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

medAbstractData* medDataManager::retrieveData(const medDataIndex& index)
{
    return retrieveData(index, true);
}

const QMetaObject* medDataManager::getDataType(const medDataIndex& index)
{
    medAbstractData* data = retrieveData(index, false);

    return data ? data->metaObject() : nullptr;
}

medAbstractData* medDataManager::retrieveData(const medDataIndex& index, bool readFullData)
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
        dataObjRef = d->dbController->retrieve(index, readFullData);
    } else if(d->nonPersDbController->contains(index)) {
        dataObjRef = d->nonPersDbController->retrieve(index, readFullData);
    }
    else if (d->pacsController->contains(index))
    {
        if (d->pacsController->loadData(index))
        {
            dataObjRef = d->pacsController->retrieve(index);
        }
    }

    if (dataObjRef) {
        dataObjRef->setDataIndex(index);

        if (readFullData)
        {
            d->loadedDataObjectTracker.insert(index, dataObjRef);
        }
        return dataObjRef;
    }
    return nullptr;
}

void medDataManager::loadData(const medDataIndex &index)
{
    Q_D(medDataManager);
    QMutexLocker locker(&(d->mutex));

    if (d->pacsController->contains(index))
    {

        if (index.isValidForSeries())
        {
            d->pacsController->loadData(index);
        }
        else if (index.isValidForStudy())
        {
            for (medDataIndex id : d->pacsController->series(index))
            {
                loadData(id);
            }
        }
        else if (index.isValidForPatient())
        {
            for (medDataIndex id : d->pacsController->studies(index))
            {
                loadData(id);
            }
        }
    }
    return;
}

QUuid medDataManager::importData(medAbstractData *data, bool persistent, bool allowDuplicateSeriesName)
{
    if (!data)
        return QUuid();

    Q_D(medDataManager);
    QUuid uuid = QUuid::createUuid();
    medAbstractDbController *controller =
        persistent ? d->dbController : d->nonPersDbController;
    qDebug() << "generated uuid " << uuid.toString();

    if (persistent)
    {
        d->dbController->importData(data, uuid, allowDuplicateSeriesName);
    }
    else
    {
        controller->importData(data, uuid);
    }
    return uuid;
}

QUuid medDataManager::importPath(const QString &dataPath,
                                 bool indexWithoutCopying, bool persistent)
{
    if (!QFile::exists(dataPath))
        return QUuid();

    Q_D(medDataManager);
    QUuid uuid = QUuid::createUuid();
    medAbstractDbController *controller =
        persistent ? d->dbController : d->nonPersDbController;
    controller->importPath(dataPath, uuid, indexWithoutCopying);
    return uuid;
}

void medDataManager::fetchData(const QHash<QString, QHash<QString, QVariant> > &pData,
                               const QHash<QString, QHash<QString, QVariant> > &sData)
{
    Q_D(medDataManager);
    d->pacsController->importMetaDataFromPacs(pData, sData);
}

/** @brief return writers able to handle the data *Memory management is the
   responsability of the caller*

*/
QHash<QString, dtkAbstractDataWriter *> medDataManager::getPossibleWriters(
    medAbstractData *data)
{
    Q_D(medDataManager);
    QList<QString> allWriters = medAbstractDataFactory::instance()->writers();
    QHash<QString, dtkAbstractDataWriter *> possibleWriters;

    for (QString writerType : allWriters)
    {
        dtkAbstractDataWriter *writer =
            medAbstractDataFactory::instance()->writer(writerType);
        if (writer->handled().contains(data->identifier()))
            possibleWriters[writerType] = writer;
        else
            delete writer;
    }
    if (possibleWriters.isEmpty())
        medMessageController::instance().showError(
            "Sorry, we have no exporter for this format.");

    return possibleWriters;
}

void medDataManager::exportData(dtkSmartPointer<medAbstractData> data)
{
    if (!data)
        return;

    Q_D(medDataManager);
    QList<QString> allWriters = medAbstractDataFactory::instance()->writers();
    QHash<QString, dtkAbstractDataWriter *> possibleWriters =
        getPossibleWriters(data);

    QFileDialog *exportDialog = new QFileDialog(
        0, tr("Exporting: please choose a file name and directory"));
    exportDialog->setOption(QFileDialog::DontUseNativeDialog);
    exportDialog->setAcceptMode(QFileDialog::AcceptSave);

    QComboBox *typesHandled = new QComboBox(exportDialog);
    // we use allWriters as the list of keys to make sure we traverse
    // possibleWriters in the order specified by the writers priorities.
    for (QString type : allWriters)
    {
        if (!possibleWriters.contains(type))
            continue;

        QStringList extensionList =
            possibleWriters[type]->supportedFileExtensions();
        QString label = possibleWriters[type]->description() + " (" +
                        extensionList.join(", ") + ")";
        QString extension =
            (extensionList.isEmpty()) ? QString() : extensionList.first();
        typesHandled->addItem(label, type);
        typesHandled->setItemData(typesHandled->count() - 1, extension,
                                  Qt::UserRole + 1);
        typesHandled->setItemData(typesHandled->count() - 1,
                                  QVariant::fromValue<QObject *>(exportDialog),
                                  Qt::UserRole + 2);
    }
    connect(typesHandled, SIGNAL(currentIndexChanged(int)), this,
            SLOT(exportDialog_updateSuffix(int)));

    QLayout *layout = exportDialog->layout();
    QGridLayout *gridbox = qobject_cast<QGridLayout *>(layout);

    // nasty hack to hide the filter list
    QWidget *filtersLabel =
        gridbox->itemAtPosition(gridbox->rowCount() - 1, 0)->widget();
    QWidget *filtersList =
        gridbox->itemAtPosition(gridbox->rowCount() - 1, 1)->widget();
    filtersLabel->hide();
    filtersList->hide();

    if (gridbox)
    {
        gridbox->addWidget(new QLabel("Export format:", exportDialog),
                           gridbox->rowCount() - 1, 0);
        gridbox->addWidget(typesHandled, gridbox->rowCount() - 1, 1);
    }

    exportDialog->setLayout(gridbox);

    // Set a default filename based on the series's description
    medAbstractDbController *dbController =
        d->controllerForDataSource(data->dataIndex().dataSourceId());
    if (dbController)
    {
        QString defaultName = dbController->metaData(
            data->dataIndex(), medMetaDataKeys::SeriesDescription);
        defaultName +=
            typesHandled->itemData(typesHandled->currentIndex(), Qt::UserRole + 1)
                .toString();
        exportDialog->selectFile(defaultName);
    }

    if (exportDialog->exec())
    {
        // Chosen format in combobox. Ex. "vtkDataMesh"
        QString chosenFormat =
            typesHandled->itemData(typesHandled->currentIndex()).toString();

        // Combobox extension. Ex. ".vtk"
        QString comboExtension =
            typesHandled->itemData(typesHandled->currentIndex(), Qt::UserRole + 1)
                .toString();

        // Chosen extension in filename. Ex. "vtk"
        QString finalFilename = exportDialog->selectedFiles().first().toUtf8();
        QString userExtension = QFileInfo(finalFilename).suffix();

        // Some extensions are linked to several formats:
        // if the combobox and filename extensions are equal, no need to enter here.
        if (!userExtension.isEmpty() && (comboExtension != ("." + userExtension)))
        {
            for (QString type : allWriters)
            {
                if (possibleWriters.contains(type))
                {
                    QStringList extensionList =
                        possibleWriters[type]->supportedFileExtensions();

                    if (extensionList.contains("." + userExtension))
                    {
                        // User has the last word about the file format, if it's a known
                        // format
                        chosenFormat = type;
                        break;
                    }
                }
            }
        }
        // Send final type to export data
        this->exportDataToPath(data, finalFilename, chosenFormat);
        // Save json file associated to data (APHP/incepto requirement)
        QString jsonPath = dbController->attachedMetadataFileExists(data->dataIndex());
        QString exportPath = finalFilename.left(finalFilename.lastIndexOf("."));

        if (data->metadata(medMetaDataKeys::Toolbox.key())=="PolygonROI")
        {
            saveAttachedMetadataToFile(data, exportPath);
        }
        else if (!jsonPath.isEmpty())
        {
            jsonPath.prepend(medSettingsManager::instance().value("database", "actual_database_location").toString());
            if (!QDir(exportPath).exists())
            {
                QDir().mkpath(exportPath);
            }
            QString jsonFileName = exportPath + QDir::separator() + data->metadata(medMetaDataKeys::SeriesInstanceUID.key()) + ".json";
            QFile::copy(jsonPath, jsonFileName);
        }
    }

    qDeleteAll(possibleWriters);
    delete exportDialog;
}

void medDataManager::saveAttachedMetadataToFile(const dtkSmartPointer<medAbstractData> &data, const QString &exportPath) const
{
    QString id = data->metadata(medMetaDataKeys::SeriesInstanceUID.key());

    if (!QDir(exportPath).exists())
    {
        QDir().mkpath(exportPath);
    }
    QString jsonFileName = exportPath + QDir::separator() + id + ".json";
    QFile jsonFile(jsonFileName);
    jsonFile.open(QIODevice::WriteOnly);
    QJsonObject mainObject;

    QJsonObject inceptoObject;
    inceptoObject.insert("id", id);

    QString date =
            QDate::currentDate().toString("yyyy-MM-dd")
            +"T"
            + QTime::currentTime().toString("hh:mm:ss.zzz");
    inceptoObject.insert("createdDate", date);
    inceptoObject.insert("modifiedDate", date);

    inceptoObject.insert("userId", "");

    QJsonObject valueObject;
    QString reference = data->metadata(medMetaDataKeys::SeriesDescription.key());
    int start = reference.indexOf("mask");
    start += QString("mask ").size();
    int end = reference.lastIndexOf("(") - start - 1;
    reference = reference.mid(start, end);
    reference.prepend(QString("Daicap_Prostate_"));
    reference.append(QString("_Segmentation"));
    reference.replace(" ", "_");

    valueObject.insert("reference",  reference);
    valueObject.insert("mha_filename",  QString(id+".mha"));
    QJsonObject caracteristique;

    QString sequence = data->metadata(medMetaDataKeys::SeriesDescription.key());
    start = sequence.lastIndexOf("(") + 1;
    end = sequence.lastIndexOf(")") - start;
    sequence = sequence.mid(start, end);
    caracteristique.insert("sequence", sequence);

    if (reference.contains("Target"))
    {
        start = reference.indexOf("Target");
        start += QString("Target").size()+1;
        QString targetId = reference.mid(start, 1);
        caracteristique.insert("targetId", targetId);

    }
    if (reference.contains("PIRADS"))
    {
        start = reference.indexOf("PIRADS");
        start += QString("PIRADS").size();
        QString pirads = reference.mid(start, 1);
        caracteristique.insert("pirads", pirads);
    }
    if (reference.contains(QRegExp("_Z[T|P]{1}_")))
    {
        start = reference.indexOf(QRegExp("Z[T|P]{1}"));
        QString zone = reference.mid(start, 2);
        caracteristique.insert("zone", zone);
    }

    valueObject.insert("caracteristique", caracteristique);

    inceptoObject.insert("value", valueObject);

    QJsonObject medInriaObject;
    medInriaObject.insert("patientId", data->metadata(medMetaDataKeys::PatientID.key()));
    medInriaObject.insert("patientName", data->metadata(medMetaDataKeys::PatientName.key()));
    medInriaObject.insert("studyInstanceUID", data->metadata(medMetaDataKeys::StudyInstanceUID.key()));
    medInriaObject.insert("studyDescription", data->metadata(medMetaDataKeys::StudyDescription.key()));
    medInriaObject.insert("seriesInstanceUID", data->metadata(medMetaDataKeys::OriginalDataUID.key()));
    medInriaObject.insert("seriesDescription", data->metadata(medMetaDataKeys::OriginalDataDesc.key()));

    mainObject.insert("originalMetaData", medInriaObject);
    mainObject.insert("incepto", inceptoObject);


    QJsonDocument doc(mainObject);
    if (jsonFile.write(doc.toJson()) != -1)
    {
        int pos = jsonFileName.size() - jsonFileName.indexOf(data->metadata(medMetaDataKeys::PatientID.key())) + 1;
        QString suffixPath = jsonFileName.right(pos);
        data->setMetaData(medMetaDataKeys::FileMetadataPath.key(), suffixPath);
        jsonFile.close();
    }
}

void medDataManager::exportDataToPath(dtkSmartPointer<medAbstractData> data, const QString & filename, const QString & writer)
{
    medDatabaseExporter *exporter =
        new medDatabaseExporter(data, filename, writer);
    launchExporter(exporter, filename);
}

void medDataManager::exportDataToPath(QList<medAbstractData*> dataList, const QString & filename, const QString & writer)
{
    auto exporter = new medDatabaseExporter(dataList, filename, writer);
    launchExporter(exporter, filename);
}

void medDataManager::launchExporter(medDatabaseExporter *exporter,
                                    const QString &filename)
{
    QFileInfo info(filename);
    medMessageProgress *message = medMessageController::instance().showProgress(
        "Exporting data to " + info.baseName());

    connect(exporter, SIGNAL(progressed(int)), message, SLOT(setProgress(int)));
    connect(exporter, SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(exporter, SIGNAL(failure(QObject *)), message, SLOT(failure()));
    connect(exporter, SIGNAL(success(QObject *)), this, SIGNAL(exportFinished()));
    connect(exporter, SIGNAL(failure(QObject *)), this, SIGNAL(exportFinished()));

    medJobManagerL::instance().registerJobItem(exporter);
    QThreadPool::globalInstance()->start(exporter);
}

QList<medDataIndex> medDataManager::getSeriesListFromStudy(
    const medDataIndex &indexStudy)
{
    Q_D(medDataManager);
    QList<medDataIndex> indexList;

    medAbstractDbController *dbc =
        d->controllerForDataSource(indexStudy.dataSourceId());

    if (dbc)
    {
        // Get the list of each series from that study index
        indexList = dbc->series(indexStudy);
    }

    return indexList;
}

QList<medDataIndex> medDataManager::moveStudy(const medDataIndex &indexStudy,
                                              const medDataIndex &toPatient)
{
    Q_D(medDataManager);
    QList<medDataIndex> newIndexList;
    medAbstractDbController *dbc =
        d->controllerForDataSource(indexStudy.dataSourceId());
    if (!dbc)
    {
        return newIndexList;
    }

    if (dbc->dataSourceId() != toPatient.dataSourceId())
    {
        qWarning()
            << "medDataManager: Moving data accross controllers is not supported.";
    }
    else
    {
        newIndexList = dbc->moveStudy(indexStudy, toPatient);
    }

    return newIndexList;
}

medDataIndex medDataManager::moveSeries(const medDataIndex &indexSeries,
                                        const medDataIndex &toStudy)
{
    Q_D(medDataManager);
    medAbstractDbController *dbc =
        d->controllerForDataSource(indexSeries.dataSourceId());
    if (!dbc)
    {
        return medDataIndex();
    }

    medDataIndex newIndex;

    if (dbc->dataSourceId() != toStudy.dataSourceId())
    {
        qWarning()
            << "medDataManager: Moving data accross controllers is not supported.";
    }
    else
    {
        newIndex = dbc->moveSeries(indexSeries, toStudy);
    }

    return newIndex;
}

medAbstractDbController *medDataManager::controllerForDataSource(
    int dataSourceId)
{
    Q_D(medDataManager);
    return d->controllerForDataSource(dataSourceId);
}

medDatabasePersistentController *medDataManager::controller()
{
    Q_D(medDataManager);
    return d->dbController;
}

void medDataManager::exportDialog_updateSuffix(int index)
{
    QComboBox *typesHandled = qobject_cast<QComboBox *>(sender());
    if (!typesHandled)
        return;

    QFileDialog *exportDialog = qobject_cast<QFileDialog *>(
        typesHandled->itemData(index, Qt::UserRole + 2).value<QObject *>());
    QString extension =
        typesHandled->itemData(index, Qt::UserRole + 1).toString();

    QString currentFilename =
        med::smartBaseName(exportDialog->selectedFiles().first());
    currentFilename += extension;
    exportDialog->selectFile(currentFilename);
}

void medDataManager::garbageCollect()
{
    Q_D(medDataManager);
    QMutexLocker locker(&(d->mutex));

    // garbage collect data that are only referenced by the manager
    QMutableHashIterator<medDataIndex, dtkSmartPointer<medAbstractData>> it(
        d->loadedDataObjectTracker);
    while (it.hasNext())
    {
        it.next();
        medAbstractData *data = it.value();
        if (data->count() <= 1)
        {
            qDebug() << "medDataManager garbage collected " << data->dataIndex();
            it.remove();
        }
    }
}

QUuid medDataManager::makePersistent(medDataIndex index)
{
    if (!index.isValid())
    {
        return QUuid();
    }

    Q_D(medDataManager);

    // If already persistent
    if (index.dataSourceId() == d->dbController->dataSourceId())
    {
        return QUuid();
    }

    QUuid jobUuid;

    medAbstractDbController *dbc =
        d->controllerForDataSource(index.dataSourceId());

    if (index.isValidForSeries())
    {
        medAbstractData *data =  this->retrieveData(index);
        jobUuid = this->importData(data, true);
        d->makePersistentJobs.insert(jobUuid, index);

        if (data->metadata(medMetaDataKeys::Toolbox.key())=="PolygonROI")
        {
            // Save json file associated to data (APHP/incepto requirement)
            QString path = medSettingsManager::instance().value("database", "actual_database_location").toString();
            path += QDir::separator() + data->metadata(medMetaDataKeys::PatientID.key()) +
                    QDir::separator() + data->metadata(medMetaDataKeys::SeriesID.key()) ;
            saveAttachedMetadataToFile(data, path);
        }

    }
    else if (index.isValidForStudy())
    {
        for (medDataIndex id : dbc->series(index))
        {
            jobUuid = makePersistent(id);
        }
    }
    else if (index.isValidForPatient())
    {
        for (medDataIndex id : dbc->studies(index))
        {
            jobUuid = makePersistent(id);
        }
    }

    return jobUuid;
}

QString medDataManager::getMetaData(const medDataIndex &index,
                                    const QString &key)
{
    Q_D(medDataManager);
    medAbstractDbController *dbc =
        d->controllerForDataSource(index.dataSourceId());

    if (dbc != nullptr)
    {
        return dbc->metaData(index, key);
    }
    else
    {
        return QString();
    }
}

bool medDataManager::setMetadata(const medDataIndex &index, const QString &key,
                                 const QString &value)
{
    Q_D(medDataManager);
    medAbstractDbController *dbc =
        d->controllerForDataSource(index.dataSourceId());

    if ((dbc != nullptr) && (dbc->setMetaData(index, key, value)))
    {
        emit metadataModified(index, key, value);
        return true;
    }

    return false;
}

void medDataManager::removeData(const medDataIndex &index)
{
    Q_D(medDataManager);
    medAbstractDbController *dbc =
        d->controllerForDataSource(index.dataSourceId());
    if (dbc)
    {
        dbc->remove(index);
    }
}

void medDataManager::removeFromNonPersistent(medDataIndex indexImported,
                                             QUuid uuid)
{
    Q_UNUSED(indexImported);
    Q_D(medDataManager);
    if (!d->makePersistentJobs.contains(uuid))
        return;

    this->removeData(d->makePersistentJobs.value(uuid));
    d->makePersistentJobs.remove(uuid);
}

QPixmap medDataManager::thumbnail(const medDataIndex &index)
{
    Q_D(medDataManager);
    medAbstractDbController *dbc =
        d->controllerForDataSource(index.dataSourceId());

    QPixmap pix;
    // dbc is null when called from the importer, as data is not imported yet
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
    if (writers.contains("vtkDataMeshWriter"))
    {
        writers.move(writers.indexOf("vtkDataMeshWriter"), 0);
    }

    // set itkMetaDataImageWriter as a top priority writer
    if (writers.contains("itkMetaDataImageWriter"))
    {
        writers.move(writers.indexOf("itkMetaDataImageWriter"), 1);
    }

    for (int i = 0; i < writers.size(); i++)
    {
        writerPriorites.insert(i, writers[i]);
    }

    medAbstractDataFactory::instance()->setWriterPriorities(writerPriorites);
}

medDataManager::medDataManager() : d_ptr(new medDataManagerPrivate(this))
{
    Q_D(medDataManager);

    d->nonPersDbController = &medDatabaseNonPersistentController::instance();
    d->pacsController = &medDataPacsController::instance();
    // Setting up database connection
    d->dbController = &medLocalDbController::instance();

    if (!d->dbController->createConnection())
    {
        qDebug() << "Unable to create a connection to the database";
    }

    QList<medAbstractDbController *> controllers;
    controllers << d->dbController << d->nonPersDbController << d->pacsController;
    for (medAbstractDbController *controller : controllers)
    {
        connect(controller, SIGNAL(dataImported(medDataIndex, QUuid)), this,
                SIGNAL(dataImported(medDataIndex, QUuid)));
        connect(controller, SIGNAL(dataRemoved(medDataIndex)), this,
                SIGNAL(dataRemoved(medDataIndex)));
        connect(controller,
                SIGNAL(metadataModified(medDataIndex, QString, QString)), this,
                SIGNAL(metadataModified(medDataIndex, QString, QString)));
        connect(controller, SIGNAL(moveRequested(const QString &, const QString &)), this,
                SIGNAL(moveRequested(const QString &, const QString &)));
    }

    connect(&(d->timer), SIGNAL(timeout()), this, SLOT(garbageCollect()));
    d->timer.start(5 * 1000);

    connect(&medPluginManager::instance(), SIGNAL(allPluginsLoaded()), this,
            SLOT(setWriterPriorities()));
    connect(this, SIGNAL(dataImported(medDataIndex, QUuid)), this,
            SLOT(removeFromNonPersistent(medDataIndex, QUuid)));
}

void medDataManager::setDatabaseLocation()
{
    Q_D(medDataManager);
    QString currentLocation = medStorage::dataLocation();

    //  If the user configured a new location for the database in the settings
    //  editor, we'll need to move it

    medSettingsManager *mnger = &medSettingsManager::instance();
    QString newLocation =
        mnger->value("medDatabaseSettingsWidget", "new_database_location")
            .toString();
    if (!newLocation.isEmpty())
    {
        //  If the locations are different we need to move the db to the new
        //  location

        if (currentLocation.compare(newLocation) != 0)
        {
            if (!d->dbController->moveDatabase(newLocation))
            {
                qDebug() << "Failed to move the database from " << currentLocation
                         << " to " << newLocation;
                //  The new location is invalid so set it to zero
                newLocation = "";
            }
            mnger->setValue("medDatabaseSettingsWidget", "actual_database_location",
                            newLocation);

            //  We need to reset the new Location to prevent doing it all the time

            mnger->setValue("medDatabaseSettingsWidget", "new_database_location", "");
        }
    }
    // END OF DATABASE INITIALISATION
}

medDataManager::~medDataManager()
{
    delete d_ptr;
}
