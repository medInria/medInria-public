/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include "medAbstractInteractor.h"

#include <medAbstractView.h>


class medAbstractInteractorPrivate
{
public:
    medAbstractView* view;
    medAbstractData* data;
    QString viewType;
    QString dataType;
};

medAbstractInteractor::medAbstractInteractor(medAbstractView *parent):
    d(new medAbstractInteractorPrivate)
{
    this->setView(parent);

    QString viewType = "medAbstractView";
    QString dataType = "medAbstractData";
    this->setTypeHandled(viewType, dataType);
}

medAbstractInteractor::~medAbstractInteractor()
{
    delete d;
}

QStringList medAbstractInteractor::handled() const
{
    return QStringList() << d->viewType << d->dataType;
}


QString medAbstractInteractor::viewTypeHandled() const
{
    return d->viewType;
}


QString medAbstractInteractor::dataTypeHandled() const
{
    return d->dataType;
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


void medAbstractInteractor::setTypeHandled(QString &viewType, QString &dataType)
{
    d->viewType = viewType;
    d->dataType = dataType;
}


