/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medQtDataImage.h>

#include <medAbstractDataFactory.h>
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

void * medQtDataImage::data( void )
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

QImage & medQtDataImage::thumbnail( void )
{
    if ( !d->thumbnailValid )
        this->generateThumbnails();

    return this->medAbstractImageData::thumbnails()[0];
}

QList<QImage>& medQtDataImage::thumbnails( void )
{
    if ( !d->thumbnailValid )
        this->generateThumbnails();

    return this->medAbstractImageData::thumbnails();
}

void medQtDataImage::generateThumbnails()
{
    QImage refImage;
    if ( this->medAbstractImageData::thumbnails().isEmpty() ) {
        refImage = this->medAbstractImageData::thumbnail();
    } else {
        refImage = this->medAbstractImageData::thumbnails()[0];
    }
    const int sx = refImage.width();
    const int sy = refImage.height();

    int mid = d->images.size() / 2;
    this->medAbstractImageData::thumbnails().clear();

    const QImage & unscaledImage = d->images[mid];
    QImage scaledImage;
    if ( unscaledImage.height() > unscaledImage.width() ) {
        scaledImage = unscaledImage.scaledToHeight(sy);
    } else {
        scaledImage = unscaledImage.scaledToWidth(sx);
    }
    this->medAbstractImageData::thumbnails().push_back(scaledImage);
    d->thumbnailValid = true;
}

bool medQtDataImage::registered()
{
    return medAbstractDataFactory::instance()->registerDataType<medQtDataImage>();
}

void * medQtDataImage::output( void )
{
    return this->data();
}

int medQtDataImage::xDimension( void )
{
    if ( d->images.isEmpty() ) {
        return 0;
    } else {
        return d->sizeX;
    }
}

int medQtDataImage::yDimension( void )
{
    if ( d->images.isEmpty() ) {
        return 0;
    } else {
        return d->sizeY;
    }
}

int medQtDataImage::zDimension( void )
{
    return d->images.size();
}

int medQtDataImage::tDimension( void )
{
    if ( d->images.isEmpty() ) {
        return 0;
    } else {
        return 1;
    }
}

qreal medQtDataImage::xSpacing( void ) const
{
    if ( d->images.isEmpty() ) {
        return 0.0;
    } else {
        return 1.0;
    }
}

qreal medQtDataImage::ySpacing( void ) const
{
    if ( d->images.isEmpty() ) {
        return 0.0;
    } else {
        return 1.0;
    }
}

qreal medQtDataImage::zSpacing( void ) const
{
    if ( d->images.isEmpty() ) {
        return 0.0;
    } else {
        return 1.0;
    }
}

qreal medQtDataImage::tSpacing( void ) const
{
    if ( d->images.isEmpty() ) {
        return 0.0;
    } else {
        return 1.0;
    }
}

int medQtDataImage::Dimension( void ) const
{
    if ( d->images.isEmpty() ) {
        return 0;
    } else {
        return 3;
    }
}

const medQtDataImage::PixId& medQtDataImage::PixelType( void ) const
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

int medQtDataImage::minRangeValue( void )
{
    return 0;
}

int medQtDataImage::maxRangeValue( void )
{
    return 255;
}

int medQtDataImage::scalarValueCount( int value )
{
    return 0;
}

int medQtDataImage::scalarValueMinCount( void )
{
    return 0;
}

int medQtDataImage::scalarValueMaxCount( void )
{
    return 0;
}
