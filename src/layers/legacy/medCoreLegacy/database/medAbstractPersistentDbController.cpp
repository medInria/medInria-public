/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractPersistentDbController.h"
#include "medDatabaseImporter.h"
#include "medDatabaseReader.h"
#include "medDatabaseRemover.h"
#include "medStorage.h"

#include <medJobManagerL.h>
#include <medMessageController.h>

/**
* Import data into the db read from file
* @param const QString & file The file containing the data
* @param bool indexWithoutCopying true if the file must only be indexed by its current path,
* false if the file will be imported (copied or converted to the internal storage format)
*/
void medAbstractPersistentDbController::importPath(const QString& file, const QUuid &importUuid, bool indexWithoutCopying)
{
    QFileInfo info(file);
    medDatabaseImporter *importer = new medDatabaseImporter(info.absoluteFilePath(),importUuid, indexWithoutCopying);
    medMessageProgress *message = medMessageController::instance()->showProgress("Importing " + info.fileName());

    connect(importer, SIGNAL(progressed(int)),    message, SLOT(setProgress(int)));
    connect(importer, SIGNAL(dataImported(medDataIndex,QUuid)), this, SIGNAL(dataImported(medDataIndex,QUuid)));

    connect(importer, SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(importer, SIGNAL(failure(QObject *)), message, SLOT(failure()));
    connect(importer,SIGNAL(showError(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(const QString&,unsigned int)));

    medJobManagerL::instance()->registerJobItem(importer);
    QThreadPool::globalInstance()->start(importer);
}

/**
* Import data into the db read from memory
* @param medAbstractData * data dataObject
*/
void medAbstractPersistentDbController::importData( medAbstractData *data, const QUuid & importUuid)
{
    medDatabaseImporter *importer = new medDatabaseImporter(data, importUuid);
    medMessageProgress *message = medMessageController::instance()->showProgress("Saving database item");

    connect(importer, SIGNAL(progressed(int)),    message, SLOT(setProgress(int)));
    connect(importer, SIGNAL(dataImported(medDataIndex,QUuid)), this, SIGNAL(dataImported(medDataIndex,QUuid)));

    connect(importer, SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(importer, SIGNAL(failure(QObject *)), message, SLOT(failure()));
    connect(importer,SIGNAL(showError(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(const QString&,unsigned int)));

    medJobManagerL::instance()->registerJobItem(importer);
    QThreadPool::globalInstance()->start(importer);
}

void medAbstractPersistentDbController::showOpeningError(QObject *sender)
{
    medMessageController::instance()->showError("Opening item failed.", 3000);
}

/**
* Change the storage location of the database by copy, verify, delete
* @param QString newLocation path of new storage location, must be empty
* @return bool true on success
*/
bool medAbstractPersistentDbController::moveDatabase( QString newLocation)
{
    // close connection if necessary
    if (this->isConnected())
    {
        this->closeConnection();
    }
    // now update the datastorage path and make sure to reconnect
    medStorage::setDataLocation(newLocation);

    qDebug() << "Restarting connection...";
    this->createConnection();

    return true;
}

/** override base class */
void medAbstractPersistentDbController::remove( const medDataIndex& index )
{
    medDatabaseRemover *remover = new medDatabaseRemover(index);
    medMessageProgress *message = medMessageController::instance()->showProgress("Removing item");

    connect(remover, SIGNAL(progressed(int)),    message, SLOT(setProgress(int)));
    connect(remover, SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(remover, SIGNAL(failure(QObject *)), message, SLOT(failure()));
    connect(remover, SIGNAL(removed(const medDataIndex &)), this, SIGNAL(dataRemoved(medDataIndex)));

    medJobManagerL::instance()->registerJobItem(remover);
    QThreadPool::globalInstance()->start(remover);
}

/** Implement base class */
int medAbstractPersistentDbController::dataSourceId() const
{
    return 1;
}

QPixmap medAbstractPersistentDbController::thumbnail(const medDataIndex &index) const
{
    QString thumbpath = this->metaData(index, medMetaDataKeys::ThumbnailPath.key());

    QFileInfo fileInfo(thumbpath);
    if ( fileInfo.exists() )
    {
        return QPixmap(thumbpath);
    }
    return QPixmap();
}

/** Implement base class */
bool medAbstractPersistentDbController::isPersistent(  ) const
{
    return true;
}

medAbstractData* medAbstractPersistentDbController::retrieve(const medDataIndex &index) const
{
    QScopedPointer<medDatabaseReader> reader(new medDatabaseReader(index));
    medMessageProgress *message = medMessageController::instance()->showProgress("Opening database item");

    connect(reader.data(), SIGNAL(progressed(int)), message, SLOT(setProgress(int)));
    connect(reader.data(), SIGNAL(success(QObject *)), message, SLOT(success()));
    connect(reader.data(), SIGNAL(failure(QObject *)), message, SLOT(failure()));

    connect(reader.data(), SIGNAL(failure(QObject *)), this, SLOT(showOpeningError(QObject *)));

    medAbstractData* data;
    data = reader->run();
    return data;
}

void medAbstractPersistentDbController::removeAll()
{
    qWarning()<< "Attempt to remove all item from PERSISTENT dataBase";
}
