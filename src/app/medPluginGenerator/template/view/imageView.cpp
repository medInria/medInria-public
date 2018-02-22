/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <%1.h>

#include <medViewFactory.h>

// /////////////////////////////////////////////////////////////////
// %1Private
// /////////////////////////////////////////////////////////////////

class %1Private
{
public:
};

// /////////////////////////////////////////////////////////////////
// %1
// /////////////////////////////////////////////////////////////////

%1::%1(QObject* parent) : medAbstractImageView(parent), d(new %1Private)
{
    
}

%1::~%1()
{
    
}

QString %1::identifier() const
{
    return "%1";
}

QString %1::description() const
{
    return "%1";
}

bool %1::registered()
{
    return medViewFactory::instance()->registerView<%1>(QString("%1"), QStringList()<<"TODO: add the supported data identifier here");
}

QWidget* %1::viewWidget()
{
    //TODO: Return the widget containing the actual view object.
    return new QWidget;
}

medViewBackend* %1::backend() const
{
    //TODO: Return the backend of the view
    return new medViewBackend;
}

void %1::reset()
{
    //TODO: Implement your reset method
}

void %1::render()
{
     //TODO: Implement your render method
}

QImage %1::buildThumbnail(const QSize &size)
{
    //TODO: Build and return a thumbnail of the view of the specified size.
    return QImage();
}

QPointF %1::mapWorldToDisplayCoordinates( const QVector3D & worldVec )
{
    //TODO: return the conversion from world coordinates to scene coordinates.
    return QPointF();
}

QVector3D %1::mapDisplayToWorldCoordinates( const QPointF & scenePoint )
{
    //TODO: return the conversion from scene coordinates to world coordinates.
    return QVector3D();
}

QVector3D %1::viewCenter()
{
    //TODO: Get the view center vector in world space, the center of the slice for 2d views.
    return QVector3D();

}

QVector3D %1::viewPlaneNormal()
{
    //TODO: Get the view plane normal vector in world space.
    return QVector3D();
}

QVector3D %1::viewUp()
{
    //TODO: Get the view plane up vector in world space.
    return QVector3D();
}

bool %1::is2D()
{
    //TODO: Is the scene 2D (true) or 3D (false)
    return true;
}

qreal %1::sliceThickness()
{
    //TODO: What is the thickness of the current slice (2D)
    return 1.0;
}

qreal %1::scale()
{
    //TODO: The scale (number of pixels on screen per mm)
    return 1.0;

}

