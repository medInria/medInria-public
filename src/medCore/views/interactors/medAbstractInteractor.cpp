/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractInteractor.h>

#include <medAbstractView.h>


class medAbstractInteractorPrivate
{
public:
    medAbstractData* data;

    QWidget *toolBoxWidget;
    QWidget *toolBarWidget;
    QWidget *layerWidget;
};

medAbstractInteractor::medAbstractInteractor(medAbstractView *parent):
    d(new medAbstractInteractorPrivate)
{
    setParent(parent);
    d->toolBarWidget = NULL;
    d->toolBoxWidget = NULL;
    d->layerWidget = NULL;
}

medAbstractInteractor::~medAbstractInteractor()
{
    delete d;
}

void medAbstractInteractor::setData(medAbstractData* data)
{
    d->data = data;
}

medAbstractData* medAbstractInteractor::data() const
{
    return d->data;
}

QWidget* medAbstractInteractor::toolBoxWidget()
{
    if(!d->toolBoxWidget)
    {
        d->toolBoxWidget = this->buildToolBoxWidget();
        if(d->toolBoxWidget)
            connect(d->toolBoxWidget, SIGNAL(destroyed()), this, SLOT(removeInternToolBoxWidget()));
    }

    return d->toolBoxWidget;
}

QWidget* medAbstractInteractor::toolBarWidget()
{
    if(!d->toolBarWidget)
    {
        d->toolBarWidget = this->buildToolBarWidget();
        if(d->toolBarWidget)
            connect(d->toolBarWidget, SIGNAL(destroyed()), this, SLOT(removeInternToolBarWidget()));
    }

    return d->toolBarWidget;
}

QWidget* medAbstractInteractor::layerWidget()
{
    if(!d->layerWidget)
    {
        d->layerWidget = this->buildLayerWidget();
        if(d->layerWidget)
            connect(d->layerWidget, SIGNAL(destroyed()), this, SLOT(removeInternLayerWidget()));
    }

    return d->layerWidget;
}

void medAbstractInteractor::removeInternToolBoxWidget()
{
    d->toolBoxWidget = NULL;
}

void medAbstractInteractor::removeInternToolBarWidget()
{
    d->toolBarWidget = NULL;
}

void medAbstractInteractor::removeInternLayerWidget()
{
    d->layerWidget = NULL;
}
