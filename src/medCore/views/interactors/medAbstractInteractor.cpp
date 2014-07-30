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

    QPointer<QWidget> toolBoxWidget;
    QPointer<QWidget> toolBarWidget;
    QPointer<QWidget> layerWidget;
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

void medAbstractInteractor::setInputData(medAbstractData* data)
{
    d->data = data;
}

medAbstractData* medAbstractInteractor::inputData() const
{
    return d->data;
}

QWidget* medAbstractInteractor::toolBoxWidget()
{
    if(d->toolBoxWidget.isNull())
    {
        d->toolBoxWidget = this->buildToolBoxWidget();
    }

    return d->toolBoxWidget;
}

QWidget* medAbstractInteractor::toolBarWidget()
{
    if(d->toolBarWidget.isNull())
    {
        d->toolBarWidget = this->buildToolBarWidget();
    }

    return d->toolBarWidget;
}

QWidget* medAbstractInteractor::layerWidget()
{
    if(d->layerWidget.isNull())
    {
        d->layerWidget = this->buildLayerWidget();
    }

    return d->layerWidget;
}
