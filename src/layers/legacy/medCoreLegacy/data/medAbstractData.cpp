/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <medDataIndex.h>
#include <medAttachedData.h>
#include <medAbstractLayeredView.h>
#include <medViewFactory.h>
#include <medAbstractImageView.h>
#include <medGlobalDefs.h>

#include <dtkCoreSupport/dtkSmartPointer.h>
#include <QApplication>
#include <QMainWindow>

#ifdef Q_OS_X11
void qt_x11_wait_for_window_manager(QWidget*);
#endif

class medAbstractDataPrivate
{
public:
    medDataIndex index;
    QList< dtkSmartPointer<medAttachedData> > attachedData;

    QImage thumbnail;
};

medAbstractData::medAbstractData( medAbstractData *parent )
    : dtkAbstractData(parent)
    , d(new medAbstractDataPrivate)
{
    this->moveToThread(QApplication::instance()->thread());
}

medAbstractData::medAbstractData(const medAbstractData& other)
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
    d->index.setImageId(index.imageId());
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

    foreach (QString converterId, medAbstractDataFactory::instance()->converters())
    {
        QScopedPointer<dtkAbstractDataConverter> converter (medAbstractDataFactory::instance()->converter(converterId));

        if (converter->fromTypes().contains(this->identifier()) && converter->canConvert(toType))
        {
            converter->setData(this);
            conversion = dynamic_cast <medAbstractData *> (converter->convert());

            if(conversion)
            {
                foreach(QString metaDataKey, this->metaDataList())
                    conversion->setMetaData(metaDataKey, this->metaDataValues(metaDataKey));

                foreach(QString propertyKey, this->propertyList())
                    conversion->addProperty(propertyKey, this->propertyValues(propertyKey));
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
    foreach( medAttachedData * data, d->attachedData ) {
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
    foreach ( medAttachedData * data, d->attachedData )
        emit attachedDataRemoved( data );
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
        data->setParentData(NULL);
    }
}

/** Invoke emit datamodified(this);
 * \sa dataModfied(medAbstractData *);
*/
void medAbstractData::invokeModified()
{
    emit dataModified(this);
}

QImage& medAbstractData::thumbnail()
{
    if(d->thumbnail == QImage())
    {
        if (QThread::currentThread() != QApplication::instance()->thread())
            QMetaObject::invokeMethod(this, "generateThumbnail", Qt::BlockingQueuedConnection);
        else
            this->generateThumbnail();
    }

    return d->thumbnail;
}

void medAbstractData::generateThumbnail()
{
    // Hack: some drivers crash on offscreen rendering, so we detect which one
    // we're currently using, and if it is one of the crashy ones, render to a
    // proper window instead, that we try to hide behind the main medInria one.

    bool offscreenCapable = false;
    med::GPUInfo gpu = med::gpuModel();

#if defined(Q_OS_MAC)
    // all drivers work so far
    offscreenCapable = true;
#elif defined(Q_OS_WIN32)
    // doesn't work on Intel drivers
    if ( ! gpu.vendor.toLower().contains("intel"))
        offscreenCapable = true;
#elif defined(Q_OS_LINUX)
    // only works on NVidia
    if (gpu.vendor.toLower().contains("nvidia"))
        offscreenCapable = true;
#endif

    dtkSmartPointer<medAbstractImageView> view = medViewFactory::instance()->createView<medAbstractImageView>("medVtkView");

    if(offscreenCapable)
    {
        view->setOffscreenRendering(true);
    }
    else
    {
        // We need to get a handle to the main window, so we can A) find its position, and B) ensure it is drawn over the temporary window
        const QVariant property = QApplication::instance()->property("MainWindow");
        QObject* qObject = property.value<QObject*>();
        if (qObject)
        {
            QMainWindow* aMainWindow = dynamic_cast<QMainWindow*>(qObject);
            QWidget * viewWidget = view->viewWidget();

            // Show our view in a seperate, temporary window
            viewWidget->show();
            // position the temporary window behind the main application
            viewWidget->move(aMainWindow->geometry().x(), aMainWindow->geometry().y());
            // and raise the main window above the temporary
            aMainWindow->raise();
        }

        // We need to wait for the window manager to finish animating before we can continue.
    #ifdef Q_OS_X11
        qt_x11_wait_for_window_manager(viewWidget);
    #endif
    }

    view->addLayer(this);

    // We're rendering here, to the temporary window, and will then use the resulting image
    d->thumbnail = view->generateThumbnail(med::defaultThumbnailSize);
}


