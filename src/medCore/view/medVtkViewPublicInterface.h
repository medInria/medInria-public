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
};

