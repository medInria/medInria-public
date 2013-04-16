/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "msegSeedPointAnnotation.h"

#include "msegSeedPointAnnotationData.h"
#include <QPainter>

namespace mseg {

SeedPointAnnotation::SeedPointAnnotation(QObject * parent /* = 0 */) :
    medAnnotationGraphicsObject( parent),
    m_isInitialized(false)
{
}


SeedPointAnnotation::~SeedPointAnnotation()
{
}

void SeedPointAnnotation::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0 */ )
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if ( !m_isInitialized )
        initializeSceneCoordinates();


    painter->save();

    QColor color =   seedPointAnnotationData()->isSelected() ?
         seedPointAnnotationData()->selectedColor() :
         seedPointAnnotationData()->color();

    painter->setPen( color );
    painter->setRenderHints( 
        QPainter::Antialiasing          |
        QPainter::SmoothPixmapTransform |
        QPainter::TextAntialiasing      );

    qreal radiusScene =seedPointAnnotationData()->radiusScene();
    painter->drawEllipse( QPointF( 0.0, 0.0 ), radiusScene, radiusScene );

    painter->drawLine(-radiusScene,0,radiusScene,0 );
    painter->drawLine(0, -radiusScene, 0,radiusScene);

    painter->restore();
}

void SeedPointAnnotation::initializeSceneCoordinates()
{
    this->prepareGeometryChange();
    const QVector3D centerPt = seedPointAnnotationData()->centerWorld();
    this->setPos( this->worldToScene( centerPt ) );
    if ( this->isPointInCurrentSlice(centerPt)) {
        this->setVisible(true);
    } else {
        this->setVisible(false);
    }
    m_isInitialized = true;
}

QRectF SeedPointAnnotation::boundingRect() const
{
    const qreal penWidth = 1;
    const qreal radiusScene = seedPointAnnotationData()->radiusScene();

    return QRectF(-radiusScene - penWidth / 2, -radiusScene - penWidth / 2,
        radiusScene*2 + penWidth, radiusScene*2 + penWidth);
}

const SeedPointAnnotationData *SeedPointAnnotation::seedPointAnnotationData() const
{
    return qobject_cast<const mseg::SeedPointAnnotationData *>(this->annotationData());
}

QString SeedPointAnnotation::s_description()
{
    static const QString desc = "mseg::SeedPointAnnotation";
    return desc;
}

QString SeedPointAnnotation::s_identifier()
{
    static const QString id = "mseg::SeedPointAnnotation";
    return id;
}

QObject * SeedPointAnnotation::s_create()
{
    return new SeedPointAnnotation;
}

void SeedPointAnnotation::onSceneChanged( QGraphicsScene * scene )
{
    m_isInitialized = false;
    if ( scene )
        this->initializeSceneCoordinates();
}

void SeedPointAnnotation::onSceneCameraChanged()
{
    this->onSceneChanged( this->scene() );
}

void SeedPointAnnotation::onSceneOrientationChanged()
{
    this->onSceneChanged( this->scene() );
}

void SeedPointAnnotation::onDataModified()
{
    this->onSceneChanged( this->scene() );
    BaseClass::onDataModified();
}


} // namespace mseg
