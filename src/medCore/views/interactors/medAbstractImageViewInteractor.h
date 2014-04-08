/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractLayeredViewInteractor.h>
#include <medImageViewEnum.h>

#include <medCoreExport.h>

class medAbstractImageView;

class medAbstractImageViewInteractorPrivate;
class MEDCORE_EXPORT medAbstractImageViewInteractor : public medAbstractLayeredViewInteractor
{
    Q_OBJECT

public:
             medAbstractImageViewInteractor(medAbstractView* parent);
    virtual ~medAbstractImageViewInteractor();

    virtual void moveToSliceAtPosition    (const QVector3D &position) = 0;
    virtual void moveToSlice  (int slice) = 0;

    /**
    * Set the opacity of the data on the corresponding layer
    */
    virtual void setOpacity (double opacity) = 0;

    /**
    * Get the opacity of the data on the corresponding layer
    */
    virtual double opacity() const = 0;

    virtual void setWindowLevel (double &window, double &level) = 0;
    virtual void windowLevel(double &window, double &level) = 0;


private:
    medAbstractImageViewInteractorPrivate *d;
};
