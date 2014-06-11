/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractData.h>

#include <medDataIndex.h>
#include <medAttachedData.h>
#include <medAbstractLayeredView.h>
#include <medViewFactory.h>
#include <medAbstractImageView.h>
#include <medDatabaseThumbnailHelper.h>

#include <dtkCore/dtkSmartPointer.h>

class medAbstractDataPrivate
{
public:
    medDataIndex index;
    QList< dtkSmartPointer<medAttachedData> > attachedData;
    medData::Category type;

    QImage thumbnail;
};

medAbstractData::medAbstractData( medAbstractData *parent )
    : dtkAbstractData(parent)
    , d(new medAbstractDataPrivate)
{
    qDebug() << "constructing medAbstractData: ";
    this->moveToThread(QApplication::instance()->thread());
}


medAbstractData::~medAbstractData( void )
{
    qDebug() << "deleting data with index " << d->index.asString();
    delete d;
    d = NULL;
}

void medAbstractData::setDataCategory(medData::Category type)
{
    d->type = type;
}

medData::Category medAbstractData::dataCategory() const
{
    return d->type;
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
    this->retain();
    if(d->thumbnail == QImage())
    {
        if (QThread::currentThread() != QApplication::instance()->thread())
            QMetaObject::invokeMethod(this, "generateThumbnail", Qt::BlockingQueuedConnection);
        else
            this->generateThumbnail();
    }
    this->release();

    return d->thumbnail;
}

void medAbstractData::generateThumbnail()
{
    //TODO find which view is handled by this type of data - RDE

    dtkSmartPointer<medAbstractImageView> view = medViewFactory::instance()->createView<medAbstractImageView>("medVtkView");
    view->addLayer(this);

    d->thumbnail = view->generateThumbnail(QSize(medDatabaseThumbnailHelper::width, medDatabaseThumbnailHelper::height));
}


