/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractViewNavigator.h>

#include "medCoreExport.h"

class medAbstractView;

class medAbstractExtraNavigatorPrivate;
class MEDCORE_EXPORT medAbstractExtraNavigator : public dtkAbstractViewNavigator
{
    Q_OBJECT

public:
             medAbstractExtraNavigator(medAbstractView* parent);
    virtual ~medAbstractExtraNavigator();

public:
    virtual QString description() const = 0;

    virtual void setView(medAbstractView *view);
    virtual medAbstractView *view() const;

    /**
     * @brief implementationOf
     * @return Upper abstract class it derives from.
     * Do NOT reimplement it in non abstract class.
     * Used by the factory to kwnow what can be create.
     */
    static QString implementationOf()
    {
        return "medAbstractExtraNavigator";
    }

private:
    medAbstractExtraNavigatorPrivate *d;

/*=========================================================================
                         NOT IMPLEMENTED ON PURPOSE
*=========================================================================*/
private:
    virtual void setData(dtkAbstractData *data)
    {DTK_UNUSED(data);}
    virtual dtkAbstractData* data()
    {return 0;}
    virtual dtkAbstractData *output (void)
    {}
    virtual dtkAbstractData *output (int channel)
    {DTK_UNUSED(channel);}
    virtual dtkAbstractData *output (int channel, int frame)
    {DTK_UNUSED(channel);DTK_UNUSED(frame);}
    virtual void  predraw(void)
    {}
    virtual void     draw(void)
    {}
    virtual void postdraw(void)
    {}
    bool enabled(void) const
    {return true;}
    virtual void  enable(void)
    {}
    virtual void disable(void)
    {}
};
