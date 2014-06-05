/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataManager.h>

#include <QDebug>

#include <medAbstractDataFactory.h>
#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseExporter.h>
#include <medMessageController.h>
#include <medJobManager.h>

/* THESE CLASSES NEEDS TO BE THREAD-SAFE, don't forget to lock the mutex in the
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

        if( ! dbController || ! nonPersDbController)
        {
            qCritical() << "One of the DB controllers could not be created !";
        }
    }
    medDataManager * const q_ptr;
    Q_DECLARE_PUBLIC(medDataManager)

    void cleanupTracker()
    {
        QMutexLocker lock(&mutex);
        foreach(const medDataIndex& i, loadedDataObjectTracker.keys())
            if (loadedDataObjectTracker.value(i).isNull())
                loadedDataObjectTracker.remove(i);
    }


    QMutex mutex;
    QHash<medDataIndex, dtkSmartPointer<medAbstractData> > loadedDataObjectTracker;
    medAbstractDbController * dbController;
    medAbstractDbController * nonPersDbController;
};


medDataManager *medDataManager::s_instance = NULL;

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

    foreach (medAbstractData* data, d->loadedDataObjectTracker.values())
        if(data->count() <= 1)
            d->loadedDataObjectTracker.remove(data->dataIndex());

    // No existing ref, we need to load from the file DB, then the non-persistent DB
    if (d->dbController->contains(index))
    {
        dataObjRef = d->dbController->retrieve(index);
    }
    else if(d->nonPersDbController->contains(index))
    {
        dataObjRef = d->nonPersDbController->retrieve(index);
    }

    if (dataObjRef)
    {
        dataObjRef->setDataIndex(index);
        d->loadedDataObjectTracker.insert(index, dataObjRef);
        return dataObjRef;
    }
    // unlock mutex before emitting, as this could trigger code in others threads
    // which would try to access the data manager, and we don't want to deadlock.
    locker.unlock();

    emit retrievingFailed(index);
}

void medDataManager::exportData(medAbstractData* data)
{
    if (!data)
        return;

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
    {
        medMessageController::instance()->showError("Sorry, we have no exporter for this format.");
        return;
    }

    QFileDialog * exportDialog = new QFileDialog(0, tr("Exporting : please choose a file name and directory"));
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

    if (gridbox)
    {
        gridbox->addWidget(new QLabel("Export format:", exportDialog), gridbox->rowCount()-1, 0);
        gridbox->addWidget(typesHandled, gridbox->rowCount()-1, 1);
    }

    exportDialog->setLayout(gridbox);

    // Set a default filename based on the series's description
    medAbstractDbController * dbController = controllerForDataSource(data->dataIndex().dataSourceId());
    if (dbController)
    {
        QString defaultName = dbController->metaData(data->dataIndex(), medMetaDataKeys::SeriesDescription);
        defaultName += typesHandled->itemData(typesHandled->currentIndex(), Qt::UserRole+1).toString();
        exportDialog->selectFile(defaultName);
    }

    if ( exportDialog->exec() )
    {
        QString chosenFormat = typesHandled->itemData(typesHandled->currentIndex()).toString();
        this->exportDataToFile(data, exportDialog->selectedFiles().first(), chosenFormat);
    }

    qDeleteAll(possibleWriters);
    delete exportDialog;
}


void medDataManager::exportDataToFile(medAbstractData *data, const QString & filename, const QString & writer)
{
    medDatabaseExporter *exporter = new medDatabaseExporter (data, filename, writer);
    QFileInfo info(filename);
    medMessageProgress *message = medMessageController::instance()->showProgress("Exporting data to " + info.baseName());

    connect(exporter, SIGNAL(progressed(int)), message, SLOT(setProgress(int)));
    connect(exporter, SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(exporter, SIGNAL(failure(QObject *)), message, SLOT(failure()));

    medJobManager::instance()->registerJobItem(exporter);
    QThreadPool::globalInstance()->start(exporter);
}

medDataManager::medDataManager()
    : d_ptr(new medDataManagerPrivate(this)) {

}

medDataManager::~medDataManager() {

}
