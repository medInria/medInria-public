/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "medAbstractNavigator.h"

#include <medAbstractView.h>


class medAbstractNavigatorPrivate
{
public:
    QStringList hanndeledType;
    medAbstractView* view;
};

medAbstractNavigator::medAbstractNavigator(medAbstractView *parent):
    d(new medAbstractNavigatorPrivate)
{
    this->setView(parent);
    d->hanndeledType << "medAbstractView";
}

medAbstractNavigator::~medAbstractNavigator()
{
    delete d;
}

QStringList medAbstractNavigator::handled() const
{
    return d->hanndeledType;
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

void medAbstractNavigator::addHandledType(QStringList &handeledType)
{
    d->hanndeledType << handeledType;
}
