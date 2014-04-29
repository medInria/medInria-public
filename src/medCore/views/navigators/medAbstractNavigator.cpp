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
    QWidget *toolBoxWidget;
    QWidget *toolBarWidget;
};

medAbstractNavigator::medAbstractNavigator(medAbstractView *parent):
    d(new medAbstractNavigatorPrivate)
{
    d->toolBarWidget = NULL;
    d->toolBoxWidget = NULL;
}

medAbstractNavigator::~medAbstractNavigator()
{
    delete d;
}

QWidget* medAbstractNavigator::toolBoxWidget()
{
    if(!d->toolBoxWidget)
    {
        d->toolBoxWidget = this->buildToolBoxWidget();
        connect(d->toolBoxWidget, SIGNAL(destroyed()), this, SLOT(removeInternToolBoxWidget()));
    }

    return d->toolBoxWidget;
}

QWidget* medAbstractNavigator::toolBarWidget()
{
    if(!d->toolBarWidget)
    {
        d->toolBarWidget = this->buildToolBarWidget();
        connect(d->toolBarWidget, SIGNAL(destroyed()), this, SLOT(removeInternToolBarWidget()));
    }

    return d->toolBarWidget;
}

void medAbstractNavigator::removeInternToolBoxWidget()
{
    d->toolBoxWidget = NULL;
}

void medAbstractNavigator::removeInternToolBarWidget()
{
    d->toolBarWidget = NULL;
}
