/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>

#include <medAbstractImageView.h>

#include "medCoreExport.h"

class QMouseEvent;

class v3dViewPrivate;
class v3dViewObserver;
class vtkImageView;
class vtkImageView2D;
class vtkImageView3D;
class vtkRenderer;
class vtkRenderWindowInteractor;


class MEDCORE_EXPORT medVtkViewPublicInterface : public medAbstractImageView
{
    Q_OBJECT

public:
    //TODO: find a better name than orientation - RDE.
    enum medVtkViewOrientation
    {
        VIEW_ORIENTATION_AXIAL,
        VIEW_ORIENTATION_SAGITTAL,
        VIEW_ORIENTATION_CORONAL,
        VIEW_ORIENTATION_3D
    };

    enum medVtkViewZoomMode
    {
        ZOOM_MODE_NORMAL,
        ZOOM_MODE_RUBBERBAND
    };

    virtual void setOrientation(medVtkViewOrientation orientation) = 0;

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
};

