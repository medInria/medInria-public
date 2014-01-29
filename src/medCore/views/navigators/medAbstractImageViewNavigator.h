/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractLayeredViewNavigator.h>

#include "medCoreExport.h"

class medVtkViewOrientation;
class medAbstractImageView;

class medAbstractImageViewNavigatorPrivate;
class MEDCORE_EXPORT medAbstractImageViewNavigator : public medAbstractLayeredViewNavigator
{
    Q_OBJECT

public:
             medAbstractImageViewNavigator(medAbstractImageView* parent = 0);
    virtual ~medAbstractImageViewNavigator();

public:
    virtual medVtkViewOrientation orientaion() const = 0;
    virtual void camera(QVector3D &position,
                        QVector3D &viewup,
                        QVector3D &focal,
                        double &parallelScale) const = 0;
    virtual QVector3D positionBeingViewed() const = 0;

    /**
    * @brief implementationOf
    * @return Upper abstract class it derives from.
    * Do NOT reimplement it in non abstract class.
    * Used by the factory to kwnow what can be create.
    */
    static QString implementationOf()
    {
        return "medAbstractImageViewNavigator";
    }

    virtual void setOrientation(medVtkViewOrientation orientation) = 0;
    virtual void setCamera(const QVector3D &position,
                           const QVector3D &viewup,
                           const QVector3D &focal,
                           double parallelScale) = 0;
private:
    medAbstractImageViewNavigatorPrivate *d;
};
