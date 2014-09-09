/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractNavigator.h>

#include <medCoreExport.h>

class medDoubleParameter;
class medAbstractVector2DParameter;

class medAbstractViewNavigatorPrivate;

class MEDCORE_EXPORT medAbstractViewNavigator : public medAbstractNavigator
{
    Q_OBJECT

public:
    medAbstractViewNavigator(medAbstractView* parent);
    virtual ~medAbstractViewNavigator();

    medDoubleParameter* zoomParameter();
    medAbstractVector2DParameter* panParameter();

public slots:
    virtual void setZoom (double zoom) = 0;
    virtual void setPan (const QVector2D &pan) = 0;

private:
    medAbstractViewNavigatorPrivate *d;
};
