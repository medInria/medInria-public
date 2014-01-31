/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractNavigator.h>

#include <medCoreExport.h>

class MEDCORE_EXPORT medAbstractViewNavigator : public medAbstractNavigator
{
    Q_OBJECT

public:
    medAbstractViewNavigator(medAbstractView* parent);
virtual ~medAbstractViewNavigator();

    virtual double zoom() const = 0;
    virtual QVector2D pan() const = 0;


public slots:
     virtual void setZoom (double zoom) = 0;
     virtual void setPan (const QVector2D &pan) = 0;
};
