/* medDatabaseNonPersitentController.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Jun 27 17:58:04 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 16:30:50 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 99
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medDatabaseNonPersistentItem.h"
#include "medDatabaseNonPersistentControllerImpl.h"
#include "medDatabaseNonPersistentReader.h"
#include "medDatabaseNonPersistentImporter.h"

#include <medDataIndex.h>
#include <medMessageController.h>
#include <medJobManager.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>

#include <QtCore/QHash>

// /////////////////////////////////////////////////////////////////
// medDatabaseNonPersitentControllerPrivate
// /////////////////////////////////////////////////////////////////

class medDatabaseNonPersistentControllerImplPrivate
{
public:
    int pt_index;
    int st_index;
    int se_index;
    int im_index;
    typedef QMap<medDataIndex, medDatabaseNonPersistentItem *> DataHashMapType;
    DataHashMapType items;
};

// /////////////////////////////////////////////////////////////////
// medDatabaseNonPersitentController
// /////////////////////////////////////////////////////////////////

int medDatabaseNonPersistentControllerImpl::patientId(bool increment)
{
    if (increment)
        return d->pt_index++;
    else
        return d->pt_index;
}

int medDatabaseNonPersistentControllerImpl::studyId(bool increment)
{
    if (increment)
        return d->st_index++;
    else
        return d->st_index;
}

int medDatabaseNonPersistentControllerImpl::seriesId(bool increment)
{
    if (increment)
        return d->se_index++;
    else
        return d->se_index;
}

int medDatabaseNonPersistentControllerImpl::imageId(bool increment)
{
    if (increment)
        return d->im_index++;
    else
        return d->im_index;
}

QList<medDatabaseNonPersistentItem *> medDatabaseNonPersistentControllerImpl::items(void)
{
    return d->items.values();
}

void medDatabaseNonPersistentControllerImpl::insert(medDataIndex index, medDatabaseNonPersistentItem *item)
{
    d->items.insert(index, item);
}

void medDatabaseNonPersistentControllerImpl::import(const QString& file,const QString& importUuid)
{
    medDatabaseNonPersistentReader *reader =
            new medDatabaseNonPersistentReader(file,importUuid);

    connect(reader, SIGNAL(progressed(int)),
            medMessageController::instance(), SLOT(setProgress(int)));
    connect(reader, SIGNAL(nonPersistentRead(const medDataIndex &,const QString &)),
            this, SIGNAL(updated(const medDataIndex &, const QString&)));
    connect(reader, SIGNAL(success(QObject *)),
            medMessageController::instance(), SLOT(remove(QObject *)));
    connect(reader, SIGNAL(failure(QObject *)),
            medMessageController::instance(), SLOT(remove(QObject *)));
    connect(reader, SIGNAL(success(QObject *)), reader, SLOT(deleteLater()));
    connect(reader, SIGNAL(failure(QObject *)), reader, SLOT(deleteLater()));

    medMessageController::instance()->showProgress(reader, "Opening file item");

    medJobManager::instance()->registerJobItem(reader);
    QThreadPool::globalInstance()->start(reader);
}

dtkSmartPointer<dtkAbstractData> medDatabaseNonPersistentControllerImpl::read( const medDataIndex& index ) const
{
    // Lookup item in hash table.
    medDatabaseNonPersistentControllerImplPrivate::DataHashMapType::const_iterator it( d->items.find(index) );

    // Is item in our table ? if not, return null.
    if ( it == d->items.end() )
        return dtkSmartPointer<dtkAbstractData> ();

    dtkSmartPointer<dtkAbstractData> ret(it.value()->data());

    return ret;
}

int medDatabaseNonPersistentControllerImpl::nonPersistentDataStartingIndex(void) const
{
    return 100000000;
}

medDatabaseNonPersistentControllerImpl::medDatabaseNonPersistentControllerImpl(void): d(new medDatabaseNonPersistentControllerImplPrivate)
{
    d->pt_index = nonPersistentDataStartingIndex();
    d->st_index = nonPersistentDataStartingIndex();
    d->se_index = nonPersistentDataStartingIndex();
    d->im_index = nonPersistentDataStartingIndex();
}

medDatabaseNonPersistentControllerImpl::~medDatabaseNonPersistentControllerImpl(void)
{
    foreach(medDatabaseNonPersistentItem *item, d->items)
        delete item;

    delete d;

    d = NULL;
}

bool medDatabaseNonPersistentControllerImpl::isConnected() const
{
    // always connected as there is no database to control
    return true;
}

void medDatabaseNonPersistentControllerImpl::import(dtkAbstractData *data)
{
    medDatabaseNonPersistentImporter *importer = new medDatabaseNonPersistentImporter(data);

    connect(importer, SIGNAL(progressed(int)),    medMessageController::instance(), SLOT(setProgress(int)));
    connect(importer, SIGNAL(nonPersistentImported(const medDataIndex &)), this, SIGNAL(updated(const medDataIndex &)));
    connect(importer, SIGNAL(success(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(importer, SIGNAL(failure(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(importer, SIGNAL(success(QObject *)), importer, SLOT(deleteLater()));
    connect(importer, SIGNAL(failure(QObject *)), importer, SLOT(deleteLater()));

    medMessageController::instance()->showProgress(importer, "Importing data item");

    medJobManager::instance()->registerJobItem(importer);
    QThreadPool::globalInstance()->start(importer);
}

void medDatabaseNonPersistentControllerImpl::clear(void)
{
    // objects are reference counted.
    // We could check if the item is still in use... but we just remove our reference here.
    qDeleteAll(d->items);

    d->items.clear();
    d->pt_index = nonPersistentDataStartingIndex();
    d->st_index = nonPersistentDataStartingIndex();
    d->se_index = nonPersistentDataStartingIndex();
    d->im_index = nonPersistentDataStartingIndex();
}


void medDatabaseNonPersistentControllerImpl::remove(const medDataIndex &index)
{
    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType DataHashMapType;
    typedef QList<medDataIndex> medDataIndexList;
    medDataIndexList indexesToRemove;

    for (DataHashMapType::const_iterator it(d->items.begin()); it != d->items.end(); ++it ) {
        if (medDataIndex::isMatch( it.key(), index)) {
            indexesToRemove.push_back(it.key());
        }
    }

    for (medDataIndexList::const_iterator it(indexesToRemove.begin()); it != indexesToRemove.end(); ++it)
    {
        DataHashMapType::iterator itemIt(d->items.find(*it));
        delete itemIt.value();
        d->items.erase(itemIt);
    }
}

qint64 medDatabaseNonPersistentControllerImpl::getEstimatedSize( const medDataIndex& index ) const
{
    Q_UNUSED(index);
    return 0;
}

QList<medDataIndex> medDatabaseNonPersistentControllerImpl::availableItems() const
{
    return d->items.keys();
}

bool medDatabaseNonPersistentControllerImpl::contains( const medDataIndex& index ) const
{
    return index.patientId() >= this->nonPersistentDataStartingIndex();
}

QImage medDatabaseNonPersistentControllerImpl::thumbnail( const medDataIndex &index ) const
{
    medDatabaseNonPersistentItem * item = NULL;
    if ( d->items.contains(index) ) {
        item = d->items.find(index).value();
    } else {
        typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;
        // Cannot find an exact match for the given index. Find first data that may match
        // using ordered map, and scan while index matches.
        MapType::const_iterator it = d->items.lowerBound( index );
        if (it != d->items.end() && medDataIndex::isMatch(it.key() , index ) ) {
            item = it.value();
        }
        // Since this does not contain real images, but series, search on the series index too.
        if ( !item && index.isValidForImage() ) {
            medDataIndex seriesIndex = medDataIndex::makeSeriesIndex(index.dataSourceId(), index.patientId(), index.studyId(), index.seriesId() );
            MapType::const_iterator it = d->items.lowerBound( seriesIndex );
            if ( it != d->items.end() &&  medDataIndex::isMatch(it.key() , seriesIndex ) ) {
                item = it.value();
            }
        }
    }
    if ( item ) {
        return item->data()->thumbnail();
    }
    else
    {
        return QImage();
    }
}

int medDatabaseNonPersistentControllerImpl::dataSourceId() const
{
    return 2;
}

QList<medDataIndex> medDatabaseNonPersistentControllerImpl::patients() const
{
    QList<medDataIndex> ret;

    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;
    int prevId = -1;
    for (MapType::const_iterator it(d->items.begin()); it != d->items.end(); ++it)
    {
        int currId = it.key().patientId();
        if ( currId != prevId ) {
            ret.push_back(medDataIndex::makePatientIndex(dataSourceId(),currId));
            prevId = currId;
        }
    }
    return ret;
}

QList<medDataIndex> medDatabaseNonPersistentControllerImpl::studies( const medDataIndex& index ) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForPatient() )
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;
    // First which does not compare less then given index -> first study for this patient.
    MapType::const_iterator it(d->items.lowerBound(medDataIndex::makePatientIndex(this->dataSourceId(), index.patientId())));
    int prevId = -1;
    for ( ; it != d->items.end() ; ++it)
    {
        if ( it.key().patientId() != index.patientId() )
            break;
        int currId = it.key().studyId();
        if ( currId != prevId ) {
            ret.push_back(medDataIndex::makeStudyIndex(this->dataSourceId(), index.patientId(), currId));
            prevId = currId;
        }
    }
    return ret;
}

QList<medDataIndex> medDatabaseNonPersistentControllerImpl::series( const medDataIndex& index ) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForStudy() )
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;
    // First which does not compare less then given index -> first series for this patient.
    MapType::const_iterator it(d->items.lowerBound(medDataIndex::makeStudyIndex(this->dataSourceId(), index.patientId(), index.studyId())));
    int prevId = -1;
    for ( ; it != d->items.end() ; ++it)
    {
        if ( it.key().patientId() != index.patientId() || it.key().studyId() != index.studyId())
            break;

        int currId = it.key().seriesId();
        if ( currId != prevId ) {
            ret.push_back(medDataIndex::makeSeriesIndex(this->dataSourceId(), index.patientId(), index.studyId(), currId));
            prevId = currId;
        }
    }
    return ret;
}

QList<medDataIndex> medDatabaseNonPersistentControllerImpl::images( const medDataIndex& index ) const
{
    QList<medDataIndex> ret;

    if ( !index.isValidForSeries() )
    {
        qWarning() << "invalid index passed";
        return ret;
    }

    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;
    // First which does not compare less then given index -> first series for this patient.
    MapType::const_iterator it(d->items.lowerBound(medDataIndex::makeSeriesIndex(this->dataSourceId(), index.patientId(), index.studyId(), index.seriesId())));
    int prevId = -1;
    for ( ; it != d->items.end(); ++it)
    {
        if ( it.key().patientId() != index.patientId() || it.key().studyId() != index.studyId() || it.key().seriesId() != index.seriesId())
            break;

        int currId = it.key().seriesId();
        if ( currId != prevId ) {
            ret.push_back(medDataIndex(this->dataSourceId(), index.patientId(), index.studyId(), index.seriesId(), currId));
            prevId = currId;
        }
    }
    return ret;
}

QString medDatabaseNonPersistentControllerImpl::metaData( const medDataIndex& index, const QString& key ) const
{
    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;

    MapType::const_iterator it(d->items.find(index));
    if (it != d->items.end() ) {
        dtkAbstractData *data = it.value()->data();
        if (data &&  data->hasMetaData(key) )
            return data->metadata(key);
    } else {
        // Cannot find an exact match for the given index. Find first data that may match
        // using ordered map.
        it = d->items.lowerBound( index );
        if (it != d->items.end() && medDataIndex::isMatch( it.key(), index) ) {
            dtkAbstractData *data = it.value()->data();
            if (data &&  data->hasMetaData(key) )
                return data->metadata(key);
        }
    }
    return QString();
}

bool medDatabaseNonPersistentControllerImpl::setMetaData( const medDataIndex& index, const QString& key, const QString& value )
{
    typedef medDatabaseNonPersistentControllerImplPrivate::DataHashMapType MapType;

    MapType::const_iterator it(d->items.find(index));
    if (it != d->items.end() ) {
        dtkAbstractData *data = it.value()->data();
        if (data) {
            data->setMetaData(key, value);
            return true;
        }
    } else {
        // Cannot find an exact match for the given index. Find first data that may match
        // using ordered map, and scan while index matches.
        it = d->items.lowerBound( index );
        int numSet(0);
        for ( ; it != d->items.end() && medDataIndex::isMatch(it.key() , index ); ++it ) {
            dtkAbstractData *data = it.value()->data();
            if (data) {
                data->setMetaData(key, value);
                ++numSet;
            }
        }
        if (numSet)
            return true;
    }
    return false;
}

bool medDatabaseNonPersistentControllerImpl::isPersistent( ) const
{
    return false;
}


