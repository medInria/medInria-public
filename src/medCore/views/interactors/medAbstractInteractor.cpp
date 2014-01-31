/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
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
    medAbstractView* view;
    medAbstractData* data;
};

medAbstractInteractor::medAbstractInteractor(medAbstractView *parent):
    d(new medAbstractInteractorPrivate)
{
    this->setView(parent);
}

medAbstractInteractor::~medAbstractInteractor()
{
    delete d;
}

void medAbstractInteractor::setView(medAbstractView *view)
{
    this->setParent(view);
    d->view = view;
}

medAbstractView* medAbstractInteractor::view() const
{
    return d->view;
}

void medAbstractInteractor::setData(medAbstractData* data)
{
    d->data = data;
}

medAbstractData* medAbstractInteractor::data() const
{
    return d->data;
}

