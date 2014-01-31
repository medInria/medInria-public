/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <medAbstractNavigator.h>

#include <medAbstractView.h>


class medAbstractNavigatorPrivate
{
public:
    medAbstractView* view;
};

medAbstractNavigator::medAbstractNavigator(medAbstractView *parent):
    d(new medAbstractNavigatorPrivate)
{
    this->setView(parent);
}

medAbstractNavigator::~medAbstractNavigator()
{
    delete d;
}

void medAbstractNavigator::setView(medAbstractView *view)
{
    this->setParent(view);
    d->view = view;
}

medAbstractView* medAbstractNavigator::view() const
{
    return d->view;
}


void medAbstractNavigator::setData(dtkAbstractData *data)
{
    DTK_UNUSED(data);
}

dtkAbstractData* medAbstractNavigator::data()
{
    return 0;
}

dtkAbstractData* medAbstractNavigator::output (void)
{
    return 0;
}

dtkAbstractData* medAbstractNavigator::output (int channel)
{
    DTK_UNUSED(channel);
    return 0;
}

dtkAbstractData* medAbstractNavigator::output (int channel, int frame)
{
    DTK_UNUSED(channel);DTK_UNUSED(frame);
    return 0;
}

void medAbstractNavigator::predraw(void)
{
}

void medAbstractNavigator::draw(void)
{
}

void medAbstractNavigator::postdraw(void)
{
}

bool medAbstractNavigator::enabled(void) const
{
    return true;
}

void  medAbstractNavigator::enable(void)
{
}

void medAbstractNavigator::disable(void)
{
}
