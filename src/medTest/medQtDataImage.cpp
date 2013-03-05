/*
 * medQtDataImage.cpp
 *
 *  Created on: 4 juil. 2011 10:29:14
 *      Author: John Stark
 */

#include "medQtDataImage.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkLog/dtkLog.h>

#include <QImage>

class medQtDataImagePrivate
{
public:
    QVector<QImage> images;
    bool thumbnailValid;

    int sizeX;
    int sizeY;
    QImage::Format imageFormat;
};

static dtkAbstractData* createMedQtDataImage()
{
    return new medQtDataImage;
}

medQtDataImage::medQtDataImage() :
    d(new medQtDataImagePrivate)
{
    d->thumbnailValid = false;
    d->imageFormat = QImage::Format_Invalid;
}

medQtDataImage::~medQtDataImage()
{
    delete d;
    d = NULL;
}

void medQtDataImage::setData( void* data )
{
    d->images.clear();
    d->thumbnailValid = false;

    if ( !data )
        return;

    QImage image = *( reinterpret_cast< QImage *>( data ) );
    d->images.push_back( image );

    d->sizeX = image.width();
    d->sizeY = image.height();
    d->imageFormat = image.format();
}

void medQtDataImage::setData( void* data, int channel )
{
    d->thumbnailValid = false;
    if ( !data || (channel < 0) )
        return;

    QImage image = *( reinterpret_cast< QImage *>( data ) );

    if ( d->images.isEmpty() ) {
        d->sizeX = image.width();
        d->sizeY = image.height();
        d->imageFormat = image.format();
    }

    if ( image.width() != d->sizeX ||
        image.height() != d->sizeY ||
        image.format() != d->imageFormat ) {

            dtkDebug() << "Image not correct size or type";
            return;
    }

    if ( channel >= d->images.size() ) {
        d->images.resize( channel + 1);
    }

    d->images[channel] = image;
}

void * medQtDataImage::data()
{
    if ( d->images.isEmpty() )
        return NULL;

    return &(d->images[0]);
}

void * medQtDataImage::data( int channel )
{
    if ( (channel < d->images.size() ) || (channel >= d->images.size() ))
        return NULL;

    return &(d->images[channel]);
}

QImage & medQtDataImage::thumbnail()
{
    if ( !d->thumbnailValid )
        this->generateThumbnails();

    return this->medAbstractDataImage::thumbnails()[0];
}

QList<QImage>& medQtDataImage::thumbnails()
{
    if ( !d->thumbnailValid )
        this->generateThumbnails();

    return this->medAbstractDataImage::thumbnails();
}

void medQtDataImage::generateThumbnails()
{
    QImage refImage;
    if ( this->medAbstractDataImage::thumbnails().isEmpty() ) {
        refImage = this->medAbstractDataImage::thumbnail();
    } else {
        refImage = this->medAbstractDataImage::thumbnails()[0];
    }
    const int sx = refImage.width();
    const int sy = refImage.height();

    int mid = d->images.size() / 2;
    this->medAbstractDataImage::thumbnails().clear();

    const QImage & unscaledImage = d->images[mid];
    QImage scaledImage;
    if ( unscaledImage.height() > unscaledImage.width() ) {
        scaledImage = unscaledImage.scaledToHeight(sy);
    } else {
        scaledImage = unscaledImage.scaledToWidth(sx);
    }
    this->medAbstractDataImage::thumbnails().push_back(scaledImage);
    d->thumbnailValid = true;
}

bool medQtDataImage::registered()
{
    return dtkAbstractDataFactory::instance()->registerDataType(s_identifier(), createMedQtDataImage);
}

void * medQtDataImage::output()
{
    return this->data();
}

int medQtDataImage::xDimension() const
{
    if ( d->images.isEmpty() ) {
        return 0;
    } else {
        return d->sizeX;
    }
}

int medQtDataImage::yDimension() const
{
    if ( d->images.isEmpty() ) {
        return 0;
    } else {
        return d->sizeY;
    }
}

int medQtDataImage::zDimension() const
{
    return d->images.size();
}

int medQtDataImage::tDimension() const
{
    if ( d->images.isEmpty() ) {
        return 0;
    } else {
        return 1;
    }
}

qreal medQtDataImage::xSpacing() const
{
    if ( d->images.isEmpty() ) {
        return 0.0;
    } else {
        return 1.0;
    }
}

qreal medQtDataImage::ySpacing() const
{
    if ( d->images.isEmpty() ) {
        return 0.0;
    } else {
        return 1.0;
    }
}

qreal medQtDataImage::zSpacing() const
{
    if ( d->images.isEmpty() ) {
        return 0.0;
    } else {
        return 1.0;
    }
}

qreal medQtDataImage::tSpacing() const
{
    if ( d->images.isEmpty() ) {
        return 0.0;
    } else {
        return 1.0;
    }
}

int medQtDataImage::Dimension() const
{
    if ( d->images.isEmpty() ) {
        return 0;
    } else {
        return 3;
    }
}

const medQtDataImage::PixId& medQtDataImage::PixelType() const
{
    if ( d->images.isEmpty() ) {
        return typeid(void); ;
    }

    switch ( d->imageFormat ) {
    case QImage::Format_Invalid:
        return typeid(void);
    case QImage::Format_Mono:
    case QImage::Format_MonoLSB:
        return typeid(char);
    case QImage::Format_Indexed8:
        return typeid(void);
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
        return typeid(char);
    case QImage::Format_RGB16:
    case QImage::Format_ARGB8565_Premultiplied:
    case QImage::Format_RGB666:
    case QImage::Format_ARGB6666_Premultiplied:
    case QImage::Format_RGB555:
    case QImage::Format_ARGB8555_Premultiplied:
    case QImage::Format_RGB888:
    case QImage::Format_RGB444:
    case QImage::Format_ARGB4444_Premultiplied:
        return typeid(void);
    default:
        return typeid(void);

    }
    return typeid(void);
}

int medQtDataImage::minRangeValue()
{
    return 0;
}

int medQtDataImage::maxRangeValue()
{
    return 255;
}

int medQtDataImage::scalarValueCount( int value )
{
    return 0;
}

int medQtDataImage::scalarValueMinCount()
{
    return 0;
}

int medQtDataImage::scalarValueMaxCount()
{
    return 0;
}

QString medQtDataImage::s_description()
{
    static const QString description = "medQtDataImage";
    return description;
}

QString medQtDataImage::description() const
{
    return s_description();
}


QString medQtDataImage::s_identifier()
{
    static const QString identifier = "medQtDataImage";
    return identifier;
}

QString medQtDataImage::identifier() const
{
    return s_identifier();
}






















