/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "medCoreExport.h"

#include <QWidget>

#include <medAbstractView.h>
#include <medAbstractData.h>

class medAbstractExtraInteractorPrivate;
class MEDCORE_EXPORT medAbstractExtraInteractor : public dtkAbstractViewInteractor
{
    Q_OBJECT

public:
             medAbstractExtraInteractor(medAbstractView* parent);
    virtual ~medAbstractExtraInteractor();

public:
    virtual QString description() const = 0;

    virtual void setView(medAbstractView*view);
    virtual medAbstractView *view() const;

    virtual void setData(medAbstractData *data);
    virtual medAbstractData *data() const;

    virtual QWidget* toolBoxWidget() = 0;
    virtual QWidget* toolBarWidget() = 0;
    virtual QWidget* layerWidget() = 0;

    /**
    * @brief implementationOf
    * @return Upper abstract class it derives from.
    * Do NOT reimplement it in non abstract class.
    * Used by the factory to kwnow what can be create.
    */
    static QString implementationOf()
    {
        return "medAbstractExtraInteractor";
    }

private:
    medAbstractExtraInteractorPrivate *d;

/*=========================================================================
                         NOT IMPLEMENTED ON PURPOSE
*=========================================================================*/
private:
    virtual dtkAbstractData *output (void)
    {}
    virtual dtkAbstractData *output (int channel)
    {DTK_UNUSED(channel);}
    virtual dtkAbstractData *output (int channel, int frame)
    {DTK_UNUSED(channel);DTK_UNUSED(frame);}
    virtual void predraw(void)
    {}
    virtual void draw(void)
    {}
    virtual void postdraw(void)
    {}
    bool enabled(void) const
    {return true;}
    virtual void enable(void)
    {}
    virtual void disable(void)
    {}
    virtual void setData(dtkAbstractData* data, int channel)
    {DTK_UNUSED(data);DTK_UNUSED(channel);}
    virtual void setData(dtkAbstractData* data, int channel, int frame)
    {DTK_UNUSED(data);DTK_UNUSED(channel);DTK_UNUSED(frame);}
};
