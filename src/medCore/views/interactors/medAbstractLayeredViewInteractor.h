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

#include "medCoreExport.h"

#include <medAbstractLayeredView.h>

class medAbstractLayeredViewInteractorPrivate;
class MEDCORE_EXPORT medAbstractLayeredViewInteractor : public medAbstractViewInteractor
{
    Q_OBJECT

public:
             medAbstractLayeredViewInteractor(medAbstractLayeredView* parent = 0);
    virtual ~medAbstractLayeredViewInteractor();

public:
    virtual void setView(medAbstractLayeredView *view);
    virtual medAbstractLayeredView *view() const;

    void setLayer(int layer);
    int layer() const;

    virtual bool visibility(int layer) const =0;
    virtual QWidget* layerWidget() =0;

    static QString derivedFrom()
    {
        return "medAbstractLayeredViewInteractor";
    }

public slots:
    virtual void setVisibility (bool visibility, int layer) = 0;

private:
    medAbstractLayeredViewInteractorPrivate *d;
};
