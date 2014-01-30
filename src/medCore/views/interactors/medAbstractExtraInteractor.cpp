/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <medAbstractExtraInteractor.h>

#include <medAbstractView.h>


class medAbstractExtraInteractorPrivate
{
public:
    medAbstractView* view;
    medAbstractData* data;
};

medAbstractExtraInteractor::medAbstractExtraInteractor(medAbstractView *parent):
    d(new medAbstractExtraInteractorPrivate)
{
    this->setView(parent);
}

medAbstractExtraInteractor::~medAbstractExtraInteractor()
{
    delete d;
}

void medAbstractExtraInteractor::setView(medAbstractView *view)
{
    this->setParent(view);
    d->view = view;
}

medAbstractView* medAbstractExtraInteractor::view() const
{
    return d->view;
}

void medAbstractExtraInteractor::setData(medAbstractData* data)
{
    d->data = data;
}

medAbstractData* medAbstractExtraInteractor::data() const
{
    return d->data;
}

