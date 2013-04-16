/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "msegImageOverlayAnnotation.h"

#include "msegImageOverlayAnnotationData.h"
#include <QPainter>

namespace mseg {

ImageOverlayAnnotation::ImageOverlayAnnotation(QGraphicsItem * parent /* = 0 */) :
    medAnnotationGraphicsObject( parent),
    m_isInitialized(false)
{
}


ImageOverlayAnnotation::~ImageOverlayAnnotation()
{
}

void ImageOverlayAnnotation::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0 */ )
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if ( !m_isInitialized )
        initializeSceneCoordinates();


    painter->save();

    painter->restore();
}

void ImageOverlayAnnotation::initializeSceneCoordinates()
{
    m_isInitialized = true;
}

QRectF ImageOverlayAnnotation::boundingRect() const
{
    return QRectF();
}

const ImageOverlayAnnotationData *ImageOverlayAnnotation::ImageOverlayAnnotationData() const
{
    return qobject_cast<const mseg::ImageOverlayAnnotationData *>(this->annotationData());
}

QString ImageOverlayAnnotation::s_description()
{
    static const QString desc = "mseg::ImageOverlayAnnotation";
    return desc;
}

medAnnotationGraphicsObject * ImageOverlayAnnotation::s_create()
{
    return new ImageOverlayAnnotation;
}

void ImageOverlayAnnotation::onSceneChanged( QGraphicsScene * scene )
{
    m_isInitialized = false;
    if ( scene )
        this->initializeSceneCoordinates();
}

void ImageOverlayAnnotation::onSceneCameraChanged()
{
    this->onSceneChanged( this->scene() );
}

void ImageOverlayAnnotation::onSceneOrientationChanged()
{
    this->onSceneChanged( this->scene() );
}

QString ImageOverlayAnnotation::s_identifier()
{
    static const QString desc = "mseg::ImageOverlayAnnotation";
    return desc;
}

QString ImageOverlayAnnotation::identifier() const
{
    return ImageOverlayAnnotation::s_identifier();
}


} // namespace mseg
