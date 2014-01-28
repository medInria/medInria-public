/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "medAbstractLayeredViewNavigator.h"

#include <medAbstractLayeredView.h>


class medAbstractLayeredViewNavigatorPrivate
{
public:
    medAbstractLayeredView* view;
};

medAbstractLayeredViewNavigator::medAbstractLayeredViewNavigator(medAbstractLayeredView *parent):
    d(new medAbstractLayeredViewNavigatorPrivate)
{

    this->setView(parent);
    QString handledType = "medAbstractLayerdView";
    this->setTypeHandled(handledType);
}

medAbstractLayeredViewNavigator::~medAbstractLayeredViewNavigator()
{
    delete d;
}

void medAbstractLayeredViewNavigator::setView(medAbstractLayeredView *view)
{
    this->setParent(view);
    d->view = view;
}

medAbstractLayeredView* medAbstractLayeredViewNavigator::view() const
{
    return d->view;
}

