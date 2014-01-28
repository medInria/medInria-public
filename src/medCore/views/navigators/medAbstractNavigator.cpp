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
    QString viewType;
    medAbstractView* view;
};

medAbstractNavigator::medAbstractNavigator(medAbstractView *parent):
    d(new medAbstractNavigatorPrivate)
{
    this->setView(parent);
    QString handledType = "medAbstractView";
    this->setTypeHandled(handledType);
}

medAbstractNavigator::~medAbstractNavigator()
{
    delete d;
}

QStringList medAbstractNavigator::handled() const
{
    return QStringList() << d->viewType;
}

void medAbstractNavigator::setTypeHandled(QString &viewType)
{
    d->viewType = viewType;
}

QString medAbstractNavigator::viewTypeHandled() const
{
    return d->viewType;
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

