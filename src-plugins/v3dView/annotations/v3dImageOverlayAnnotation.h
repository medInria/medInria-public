/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "msegPluginExport.h"

#include "medAnnotationGraphicsObject.h"

#include <dtkCore/dtkSmartPointer.h>

#include <QColor>
#include <QVector3D>

namespace mseg {
class ImageOverlayAnnotationData;

class MEDVIEWSEGMENTATIONPLUGIN_EXPORT ImageOverlayAnnotation : public medAnnotationGraphicsObject
{
    Q_OBJECT;
public:
    ImageOverlayAnnotation( QGraphicsItem * parent = 0 );
    virtual ~ImageOverlayAnnotation();

    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 ) MED_OVERRIDE;
    QRectF boundingRect() const MED_OVERRIDE;

    static medAnnotationGraphicsObject * s_create( );
    /** Get name to use for this when registering with a factory.*/
    static QString s_identifier();

    static QString s_description();

    QString description() const;
    QString identifier() const;


protected:
    void onSceneChanged(QGraphicsScene * scene) MED_OVERRIDE;
    void onSceneCameraChanged() MED_OVERRIDE;
    void onSceneOrientationChanged() MED_OVERRIDE;
    void initializeSceneCoordinates();
private:
    const ImageOverlayAnnotationData *ImageOverlayAnnotationData() const;

    bool m_isInitialized;

    QImage m_image;
};

} // namespace mseg



