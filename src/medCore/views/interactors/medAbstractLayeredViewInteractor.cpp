/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "medAbstractLayeredViewInteractor.h"

#include <medAbstractLayeredView.h>


class medAbstractLayeredViewInteractorPrivate
{
public:
    medAbstractLayeredView* view;
    int layer;
};

medAbstractLayeredViewInteractor::medAbstractLayeredViewInteractor(medAbstractLayeredView *parent):
    d(new medAbstractLayeredViewInteractorPrivate)
{
    this->setView(parent);
    QString viewType = "medAbstractLayerdView";
    QString dataType = this->dataTypeHandled();
    this->setTypeHandled(viewType, dataType);
}

medAbstractLayeredViewInteractor::~medAbstractLayeredViewInteractor()
{
    delete d;
}

void medAbstractLayeredViewInteractor::setView(medAbstractLayeredView *view)
{
    this->setParent(view);
    d->view = view;
}

medAbstractLayeredView* medAbstractLayeredViewInteractor::view() const
{
    return d->view;
}

void medAbstractLayeredViewInteractor::setLayer(int layer)
{
    d->layer = layer;
}

int medAbstractLayeredViewInteractor::layer() const
{
    return d->layer;
}

