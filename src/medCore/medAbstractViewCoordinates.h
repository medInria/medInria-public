/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medCoreExport.h"

#include <QGraphicsScene>
#include <QVector2D>
#include <QVector3D>

class medAbstractView;
//class medViewEventHandler;

/**
 * Abstract base class for coorindate transformations in medAbstractViews.
 * \sa medAbstractView
 ***/
class MEDCORE_EXPORT medAbstractViewCoordinates
{

public:

    //! Convert from world coordinates to scene coordinates.
    virtual QPointF worldToDisplay( const QVector3D & worldVec ) const = 0;
    //! Convert from scene coordinates to world coordinates.
    virtual QVector3D displayToWorld( const QPointF & scenePoint ) const = 0;
    //! Get the view center vector in world space, the center of the slice for 2d views.
    virtual QVector3D viewCenter() const = 0;
    //! Get the view plane normal vector in world space.
    virtual QVector3D viewPlaneNormal() const = 0;
    //! Get the view plane up vector in world space.
    virtual QVector3D viewUp() const = 0;
    //! Is the scene 2D (true) or 3D (false)
    virtual bool is2D() const = 0;
    //! What is the thickness of the current slice (2D)
    virtual qreal sliceThickness() const = 0;
    //! The scale (number of pixels on screen per mm)
    virtual qreal scale() const = 0;

    // Utilities to convert between double arrays and Qt primitives.
    //! Convert double[3] to QVector3D
    static QVector3D doubleToQtVector3d( const double * v ) { return QVector3D( v[0], v[1], v[2] ); }
    //! Convert double[2] to QVector2D
    static QVector2D doubleToQtVector2d( const double * v ) { return QVector2D( v[0], v[1] ); }
    //! Convert double[2] to QPointF
    static QPointF   doubleToQtPointF( const double * v ) { return QPointF( v[0], v[1] ); }
    //! Convert QVector3D to double[3]
    static void qtVector3dToDouble( const QVector3D & v, double * out ) { out[0] = v.x();  out[1] = v.y(); out[2] = v.z();}
    //! Convert QVector2D to double[2]
    static void qtVector2dToDouble( const QVector2D & v, double * out ) { out[0] = v.x();  out[1] = v.y(); }
    //! Convert QPointF to double[2]
    static void qtPointFToDouble( const QPointF   & v, double * out ) { out[0] = v.x();  out[1] = v.y(); }
    //! Convert QColor to double[3]. Components are R,G,B. Alpha not included.
    static void qtColorToDouble ( const QColor & color, double * cv )    {  cv[0] = color.redF();   cv[1] = color.greenF();    cv[2] = color.blueF();    }
    //! Convert QColor to double[4]. Components are R,G,B,A.
    static void qtColorAlphaToDouble ( const QColor & color, double * cv )    {  cv[0] = color.redF();   cv[1] = color.greenF();    cv[2] = color.blueF();  cv[3] = color.alphaF();  }

protected :
    medAbstractViewCoordinates() {}
private:

};



