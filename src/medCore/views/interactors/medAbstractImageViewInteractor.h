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
class medDoubleParameter;
class medCompositeParameter;

class medAbstractImageViewInteractorPrivate;
class MEDCORE_EXPORT medAbstractImageViewInteractor : public medAbstractLayeredViewInteractor
{
    Q_OBJECT

public:
             medAbstractImageViewInteractor(medAbstractView* parent);
    virtual ~medAbstractImageViewInteractor();

    virtual medDoubleParameter* opacityParameter();

    virtual medCompositeParameter* windowLevelParameter();

public slots:

    virtual void setOpacity (double opacity) = 0;

    virtual void setWindowLevel(QList<QVariant>) = 0;

private:
    medAbstractImageViewInteractorPrivate *d;
};
