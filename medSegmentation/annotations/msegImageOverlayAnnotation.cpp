#include "msegImageOverlayAnnotation.h"

#include "msegImageOverlayAnnotationData.h"
#include <QPainter>

namespace mseg {

ImageOverlayAnnotation::ImageOverlayAnnotation(QGraphicsItem * parent /* = 0 */) :
    medAnnotationGraphicsObject( parent),
    m_isInitialized(false)
{
}


ImageOverlayAnnotation::~ImageOverlayAnnotation(void)
{
}

void ImageOverlayAnnotation::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0 */ )
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if ( !m_isInitialized )
        initializeSceneCoordinates();


    painter->save();

    painter->setPen( ImageOverlayAnnotationData()->color() );
    painter->setRenderHints( 
        QPainter::Antialiasing          |
        QPainter::SmoothPixmapTransform |
        QPainter::TextAntialiasing      );

    qreal radiusScene =ImageOverlayAnnotationData()->radiusScene();
    painter->drawEllipse( QPointF( 0.0, 0.0 ), radiusScene, radiusScene );

    painter->drawLine(-radiusScene,0,radiusScene,0 );
    painter->drawLine(0, -radiusScene, 0,radiusScene);

    painter->restore();
}

void ImageOverlayAnnotation::initializeSceneCoordinates()
{
    this->prepareGeometryChange();
    const QVector3D centerPt = ImageOverlayAnnotationData()->centerWorld();
    this->setPos( this->worldToScene( centerPt ) );
    if ( this->isPointInCurrentSlice(centerPt)) {
        this->setVisible(true);
    } else {
        this->setVisible(false);
    }
    m_isInitialized = true;
}

QRectF ImageOverlayAnnotation::boundingRect( void ) const
{
    const qreal penWidth = 1;
    const qreal radiusScene = ImageOverlayAnnotationData()->radiusScene();

    return QRectF(-radiusScene - penWidth / 2, -radiusScene - penWidth / 2,
        radiusScene*2 + penWidth, radiusScene*2 + penWidth);
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


} // namespace mseg
