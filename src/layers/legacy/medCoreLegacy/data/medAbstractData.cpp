/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medAbstractImageView.h>
#include <medAttachedData.h>
#include <medDataIndex.h>
#include <medGlobalDefs.h>
#include <medSettingsManager.h>
#include <medViewFactory.h>

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <QApplication>
#include <QMainWindow>

#include <QTest>

class medAbstractDataPrivate
{
public:
    medDataIndex index;
    QList< dtkSmartPointer<medAttachedData> > attachedData;
};

medAbstractData::medAbstractData( medAbstractData *parent )
    : dtkAbstractData(parent)
    , d(new medAbstractDataPrivate)
{
    this->moveToThread(QApplication::instance()->thread());
}

medAbstractData::medAbstractData(const medAbstractData &other)

    : dtkAbstractData(other),
      d(new medAbstractDataPrivate())
{
    *d = *other.d;
    d->index = medDataIndex();
    for (int i = 0; i < d->attachedData.count(); ++i)
    {
        d->attachedData[i] = dynamic_cast<medAttachedData*>(other.d->attachedData[i]->clone());
    }
}

medAbstractData::~medAbstractData( void )
{
    delete d;
    d = nullptr;
}

medAbstractData* medAbstractData::clone(void)
{
    return new medAbstractData(*this);
}

/**
* Attach a meddataindex to the data to carry it arround
* @param const medDataIndex & index
*/
void medAbstractData::setDataIndex( const medDataIndex& index )
{
    // copy ids
    d->index.setDataSourceId(index.dataSourceId());
    d->index.setPatientId(index.patientId());
    d->index.setStudyId(index.studyId());
    d->index.setSeriesId(index.seriesId());
}

/**
* Get the dataindex attached to the data or an invalid one
* @return medDataIndex
*/
medDataIndex medAbstractData::dataIndex() const
{
    return d->index;
}

/**
* Re-implements conversion to another data type from dtkAbstractData
* @return medAbstractData *
*/
medAbstractData * medAbstractData::convert(const QString &toType)
{
    medAbstractData *conversion = nullptr;

    for(QString converterId : medAbstractDataFactory::instance()->converters())
    {
        QScopedPointer<dtkAbstractDataConverter> converter (medAbstractDataFactory::instance()->converter(converterId));

        if (converter->fromTypes().contains(this->identifier()) && converter->canConvert(toType))
        {
            converter->setData(this);
            conversion = dynamic_cast <medAbstractData *> (converter->convert());

            if(conversion)
            {
                for(QString metaDataKey : this->metaDataList())
                {
                    conversion->setMetaData(metaDataKey, this->metaDataValues(metaDataKey));
                }
                for(QString propertyKey : this->propertyList())
                {
                    conversion->addProperty(propertyKey, this->propertyValues(propertyKey));
                }
            }
        }
    }

    return conversion;
}

/**
 * @brief Get attached data (like histogram, annotations etc.)
 *
 * @return QList< medAttachedData * >
 */
QList< medAttachedData * > medAbstractData::attachedData() const
{
    QList< medAttachedData * > ret;
#if QT_VERSION > 0x0406FF
    ret.reserve(d->attachedData.size());
#endif
    for( medAttachedData * data : d->attachedData )
    {
        ret.push_back(data);
    }
    return ret;
}

/**
 * @brief Clear the list of attached data
 *
 * @return void
 */
void medAbstractData::clearAttachedData()
{
    for( medAttachedData * data : d->attachedData )
    {
        emit attachedDataRemoved( data );
    }
    d->attachedData.clear();
}

/**
 * @brief add attached data
 *
 * @return void
 */
void medAbstractData::addAttachedData( medAttachedData * data )
{
    if ( !d->attachedData.contains( data ) ) {
        d->attachedData.append( data );
        data->setParentData(this);
        emit attachedDataAdded( data );
    }
}

/**
 * @brief remove attached data
 *
 * @return void
 */
void medAbstractData::removeAttachedData( medAttachedData * data )
{
    int n = d->attachedData.count();
    d->attachedData.removeAll( data );
    if ( n > d->attachedData.count() ) {
        emit attachedDataRemoved( data );
        data->setParentData(nullptr);
    }
}

/** Invoke emit datamodified(this);
 * \sa dataModfied(medAbstractData *);
*/
void medAbstractData::invokeModified()
{
    emit dataModified(this);
}

QImage medAbstractData::generateThumbnail(QSize size)
{
    QImage thumbnail;

    if(medSettingsManager::instance().value("Browser", "thumbnail_creation_setting").toBool())
    {
        if (QThread::currentThread() != QApplication::instance()->thread())
        {
            QMetaObject::invokeMethod(this,
                                    "generateThumbnailInGuiThread",
                                    Qt::BlockingQueuedConnection,
                                    Q_RETURN_ARG(QImage, thumbnail),
                                    Q_ARG(QSize, size));
        }
        else
        {
            thumbnail = this->generateThumbnailInGuiThread(size);
        }
    }

    return thumbnail;
}

QImage medAbstractData::generateThumbnailInGuiThread(QSize size)
{
    auto view = medViewFactory::instance()->createView<medAbstractLayeredView>("medVtkView");

    QImage thumbnail;
    if(view)
    {
        view->addLayer(this);
        thumbnail = view->generateThumbnail(size);
    }
    else
    {
        qWarning() << "medViewContainer: unable to create a medVtkView";
    }

    delete view;

    return thumbnail;
}
