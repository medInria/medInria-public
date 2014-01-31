/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractViewInteractor.h>

#include <medCoreExport.h>

class medAbstractLayeredView;

class medAbstractLayeredViewInteractorPrivate;
class MEDCORE_EXPORT medAbstractLayeredViewInteractor : public medAbstractViewInteractor
{
    Q_OBJECT

public:
             medAbstractLayeredViewInteractor(medAbstractLayeredView* parent);
    virtual ~medAbstractLayeredViewInteractor();

public:
    virtual bool visibility() const =0;
    virtual QWidget* layerWidget() =0;


public slots:
    virtual void setVisibility (bool visibility) = 0;

private:
    medAbstractLayeredViewInteractorPrivate *d;
};

