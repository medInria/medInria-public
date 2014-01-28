/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "medAbstractImageViewNavigator.h"

#include <medAbstractImageView.h>


class medAbstractImageViewNavigatorPrivate
{
public:
    medAbstractImageView* view;
};

medAbstractImageViewNavigator::medAbstractImageViewNavigator(medAbstractImageView *parent):
    d(new medAbstractImageViewNavigatorPrivate)
{
    this->setView(parent);
    QString handledType = "medAbstractImageView";
    this->addHandledType(handledType);
}

medAbstractImageViewNavigator::~medAbstractImageViewNavigator()
{
    delete d;
}

void medAbstractImageViewNavigator::setView(medAbstractImageView *view)
{
    this->setParent(view);
    d->view = view;
}

medAbstractImageView* medAbstractImageViewNavigator::view() const
{
    return d->view;
}

