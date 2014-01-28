/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "medAbstractImageViewInteractor.h"

#include <medAbstractImageView.h>


class medAbstractImageViewInteractorPrivate
{
public:
    medAbstractImageView* view;
};

medAbstractImageViewInteractor::medAbstractImageViewInteractor(medAbstractImageView *parent):
    d(new medAbstractImageViewInteractorPrivate)
{
    this->setView(parent);
    QString viewType = "medAbstractLayerdView";
    QString dataType = "medAbstractData";
    this->setTypeHandled(viewType, dataType);
}

medAbstractImageViewInteractor::~medAbstractImageViewInteractor()
{
    delete d;
}

void medAbstractImageViewInteractor::setView(medAbstractImageView *view)
{
    this->setParent(view);
    d->view = view;
}

medAbstractImageView* medAbstractImageViewInteractor::view() const
{
    return d->view;
}

