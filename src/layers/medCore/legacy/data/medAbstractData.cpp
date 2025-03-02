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

    QList<medAbstractData * > parentDataList;
    QList<medAbstractData * > derivedDataList;
    QString expectedName;

    QImage thumbnail;
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

    d->parentDataList = other.d->parentDataList;
    d->derivedDataList = other.d->derivedDataList;
}

medAbstractData::~medAbstractData( void )
{
    for (auto dData : d->derivedDataList)
    {
        dData->removeParentData(this);
    }
    for (auto pData : d->parentDataList)
    {
        pData->removeDerivedData(this);
    }

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
    d->index = index;
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

QString medAbstractData::fetchMetaData(QString word) const
{
    QString metaDataRes;

    if (hasMetaData(word))
    {
        metaDataRes = metadata(word);
    }
    else
    {
        Key2 key = medMetaDataKeys::key(word);
        if (key.isValid())
        {
            metaDataRes = medMetaDataKeys::getValue(key, getMetaDataMap());
        }
    }

    return metaDataRes;
}

QMap<QString, QString> medAbstractData::getMetaDataMap() const
{
    QMap<QString, QString> metaDataListRes;

    for (QString k : this->metaDataList())
    {
        metaDataListRes[k] = metaDataValues(k).join(';');
    }

    return metaDataListRes;
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

    return thumbnail;
}

QImage medAbstractData::generateThumbnailInGuiThread(QSize size)
{
    // Hack: some drivers crash on offscreen rendering, so we detect which one
    // we're currently using, and if it is one of the crashy ones, render to a
    // proper window instead, that we try to hide behind the main one.

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
    if (gpu.vendor.toLower().contains("nvidia")
            || gpu.vendor.toLower().contains("intel")
            || gpu.vendor.toLower().contains("mesa/x.org"))
    {
        offscreenCapable = true;
    }
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

            // Show our view in a separate, temporary window
            viewWidget->show();
            // position the temporary window behind the main application
            viewWidget->move(aMainWindow->geometry().x(), aMainWindow->geometry().y());
            // and raise the main window above the temporary
            aMainWindow->raise();

            // We need to wait for the window manager to finish animating before we can continue.
#ifdef Q_OS_LINUX
            Q_UNUSED(QTest::qWaitForWindowExposed(viewWidget));
#endif
        }
    }

    view->addLayer(this);

    // We're rendering here, to the temporary window, and will then use the resulting image
    return view->generateThumbnail(size);
}

bool medAbstractData::addParentData(medAbstractData *pi_parentData)
{
    bool bRes = pi_parentData != nullptr;
    if (bRes)
    {
        if (!d->parentDataList.contains(pi_parentData))
        {
            d->parentDataList.append(pi_parentData);
            pi_parentData->d->derivedDataList.append(pi_parentData);
        }
    }
    return bRes;
}

bool medAbstractData::addDerivedData(medAbstractData *pi_derivedData)
{
    bool bRes = pi_derivedData != nullptr;
    if (bRes)
    {
        if (!d->derivedDataList.contains(pi_derivedData))
        {
            d->derivedDataList.append(pi_derivedData);
            pi_derivedData->d->parentDataList.append(this);
        }
    }
    return bRes;
}

bool medAbstractData::removeParentData(medAbstractData *pi_parentData)
{
    pi_parentData->d->derivedDataList.removeOne(this);
    return d->parentDataList.removeOne(pi_parentData);
}

bool medAbstractData::removeDerivedData(medAbstractData *pi_derivedData)
{
    pi_derivedData->d->parentDataList.removeOne(this);
    return d->derivedDataList.removeOne(pi_derivedData);
}

QList<medAbstractData *> medAbstractData::parentData()
{
    return d->parentDataList;
}

QList<medAbstractData *> medAbstractData::derivedData()
{
    return d->derivedDataList;
}

QString medAbstractData::getExpectedName()
{
    return d->expectedName;
}

void medAbstractData::setExpectedName(QString name)
{
    d->expectedName = name;
}
