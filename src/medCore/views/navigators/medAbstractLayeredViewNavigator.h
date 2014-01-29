/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractViewNavigator.h>

#include "medCoreExport.h"

#include <medAbstractLayeredView.h>

class medAbstractLayeredViewNavigatorPrivate;
class MEDCORE_EXPORT medAbstractLayeredViewNavigator : public medAbstractViewNavigator
{
    Q_OBJECT

public:
             medAbstractLayeredViewNavigator(medAbstractLayeredView* parent = 0);
    virtual ~medAbstractLayeredViewNavigator();

    virtual void setView(medAbstractLayeredView *view);
    virtual medAbstractLayeredView *view() const;

    /**
    * @brief implementationOf
    * @return Upper abstract class it derives from.
    * Do NOT reimplement in in non abstract class.
    * Used by the factory to kwnow what can be create.
    */
    static QString implementationOf()
    {
        return "medAbstractLayerdViewNavigator";
    }

private:
    medAbstractLayeredViewNavigatorPrivate *d;
};
