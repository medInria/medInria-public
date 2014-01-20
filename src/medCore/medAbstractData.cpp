/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractData.h"

#include "medDataIndex.h"
#include "medAttachedData.h"
#include <medAbstractView.h>

#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>

#include <medDatabaseThumbnailHelper.h>


class medAbstractDataPrivate
{
public:
    bool isTrueVolumetric;
    medDataIndex index;
    QList< dtkSmartPointer<medAttachedData> > attachedData;
    QImage thumbnail;
};

medAbstractData::medAbstractData( dtkAbstractData *parent )
    : dtkAbstractData(parent)
    , d(new medAbstractDataPrivate)
{
    this->moveToThread(QApplication::instance()->thread());
    d->isTrueVolumetric = false;
    qDebug() << "constructing medAbstractData: ";
}


medAbstractData::~medAbstractData( void )
{
    qDebug() << "deleting data with index " << d->index.asString();
    delete d;
    d = NULL;
}

void medAbstractData::setTrueVolumetric( bool flag )
{
    d->isTrueVolumetric = flag;
}

bool medAbstractData::trueVolumetric() const
{
    return d->isTrueVolumetric;
}

void medAbstractData::setDataIndex( const medDataIndex& index )
{
    // copy ids
    d->index.setDataSourceId(index.dataSourceId());
    d->index.setPatientId(index.patientId());
    d->index.setStudyId(index.studyId());
    d->index.setSeriesId(index.seriesId());
    d->index.setImageId(index.imageId());
}

medDataIndex medAbstractData::dataIndex() const
{
    return d->index;
}

QList< medAttachedData * > medAbstractData::attachedData() const
{
    QList< medAttachedData * > ret;
#if QT_VERSION > 0x0406FF
    ret.reserve(d->attachedData.size());
#endif
    foreach( medAttachedData * data, d->attachedData ) {
        ret.push_back(data);
    }
    return ret;
}

void medAbstractData::clearAttachedData()
{
    foreach ( medAttachedData * data, d->attachedData )
        emit attachedDataRemoved( data );
    d->attachedData.clear();
}

void medAbstractData::addAttachedData( medAttachedData * data )
{
    if ( !d->attachedData.contains( data ) ) {
        d->attachedData.append( data );
        data->setParentData(this);
        emit attachedDataAdded( data );
    }
}

void medAbstractData::removeAttachedData( medAttachedData * data )
{
    int n = d->attachedData.count();
    d->attachedData.removeAll( data );
    if ( n > d->attachedData.count() ) {
        emit attachedDataRemoved( data );
        data->setParentData(NULL);
    }
}

void medAbstractData::invokeModified()
{
    emit dataModified(this);
}


QImage& medAbstractData::thumbnail()
{
    if (QThread::currentThread() != QApplication::instance()->thread())
        QMetaObject::invokeMethod(this, "generateThumbnail", Qt::BlockingQueuedConnection);
    else
        this->generateThumbnail();

    return d->thumbnail;
}

void medAbstractData::generateThumbnail()
{

    dtkSmartPointer<medAbstractView> view = qobject_cast<medAbstractView*>(dtkAbstractViewFactory::instance()->createSmartPointer("medVtkView"));
    view->setSharedDataPointer(this);

    d->thumbnail = view->generateThumbnail(QSize(medDatabaseThumbnailHelper::width, medDatabaseThumbnailHelper::height));
}
