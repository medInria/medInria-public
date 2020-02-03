#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractNavigator.h>
#include <medCoreLegacyExport.h>

class medDoubleParameterL;
class medAbstractVector2DParameterL;

class medAbstractViewNavigatorPrivate;

class MEDCORELEGACY_EXPORT medAbstractViewNavigator : public medAbstractNavigator
{
    Q_OBJECT

public:
    medAbstractViewNavigator(medAbstractView* parent);
    virtual ~medAbstractViewNavigator();

    medDoubleParameterL* zoomParameter();
    medAbstractVector2DParameterL* panParameter();

public slots:
    virtual void setZoom (double zoom) = 0;
    virtual void setPan (const QVector2D &pan) = 0;

private:
    medAbstractViewNavigatorPrivate *d;
};
