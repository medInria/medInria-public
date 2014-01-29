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

class medAbstractLayeredView;

class medAbstractLayeredViewInteractorPrivate;
class MEDCORE_EXPORT medAbstractLayeredViewInteractor : public medAbstractViewInteractor
{
    Q_OBJECT

public:
             medAbstractLayeredViewInteractor(medAbstractLayeredView* parent = 0);
    virtual ~medAbstractLayeredViewInteractor();

public:
    virtual bool visibility() const =0;
    virtual QWidget* layerWidget() =0;

    /**
     * @brief implementationOf
     * @return Upper abstract class it derives from.
     * Do NOT reimplement it in non abstract class.
     * Used by the factory to kwnow what can be create.
     */
    static QString implementationOf()
    {
        return "medAbstractLayeredViewInteractor";
    }

public slots:
    virtual void setVisibility (bool visibility) = 0;

private:
    medAbstractLayeredViewInteractorPrivate *d;
};

