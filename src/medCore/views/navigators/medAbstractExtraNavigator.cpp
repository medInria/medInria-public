/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "medAbstractExtraNavigator.h"

#include <medAbstractView.h>


class medAbstractExtraNavigatorPrivate
{
public:
    medAbstractView* view;
};

medAbstractExtraNavigator::medAbstractExtraNavigator(medAbstractView *parent):
    d(new medAbstractExtraNavigatorPrivate)
{
    this->setView(parent);
}

medAbstractExtraNavigator::~medAbstractExtraNavigator()
{
    delete d;
}

void medAbstractExtraNavigator::setView(medAbstractView *view)
{
    this->setParent(view);
    d->view = view;
}

medAbstractView* medAbstractExtraNavigator::view() const
{
    return d->view;
}

