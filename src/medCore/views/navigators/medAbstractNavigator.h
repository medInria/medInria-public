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

#include <medAbstractView.h>


class medAbstractNavigatorPrivate;
class MEDCORE_EXPORT medAbstractNavigator : public dtkAbstractViewNavigator
{
    Q_OBJECT

public:
             medAbstractNavigator(medAbstractView* parent = 0);
    virtual ~medAbstractNavigator();

public:
    virtual QString description() const = 0;
    virtual QStringList handled() const;


    virtual void setView(medAbstractView *view);
    virtual medAbstractView *view() const;

protected:
    void addHandledType(QStringList &handeledType);

private:
    medAbstractNavigatorPrivate *d;

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
};
