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

#include "medCoreExport.h"

#include <medAbstractImageView.h>

class medAbstractImageViewInteractorPrivate;
class MEDCORE_EXPORT medAbstractImageViewInteractor : public medAbstractLayeredViewInteractor
{
    Q_OBJECT

public:
             medAbstractImageViewInteractor(medAbstractImageView* parent = 0);
    virtual ~medAbstractImageViewInteractor();

public:
    virtual void setView(medAbstractImageView *view);
    virtual medAbstractImageView *view() const;

    virtual void moveToSliceAtPosition    (const QVector3D &position) = 0;

    /**
    * Set the opacity of the data on the corresponding layer
    */
    virtual void setOpacity (double opacity, int layer) = 0;

    /**
    * Get the opacity of the data on the corresponding layer
    */
    virtual double opacity(int layer) const = 0;

    static QString implementationOf()
    {
     return "medAbstractImageViewInteractor";
    }

private:
    medAbstractImageViewInteractorPrivate *d;
};
