/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medAbstractView.h"

#include <dtkCore/dtkAbstractData.h>
#include <medAbstractData.h>

#include <medAbstractInteractor.h>

class medAbstractViewPrivate
{
public:

    QVector2D  pan;
    double     zoom;
    QHash<QString, unsigned int> DataTypes;
    bool closable;
};

medAbstractView::medAbstractView(medAbstractView *parent) : dtkAbstractView(parent), d (new medAbstractViewPrivate)
{
    d->pan = QVector2D(0.0, 0.0);
    d->zoom = 1.0;
    d->closable = true;
}


medAbstractView::medAbstractView(const medAbstractView& view) : dtkAbstractView(view)
{
    // copy constructor not implemented!
    DTK_DEFAULT_IMPLEMENTATION;
}

medAbstractView::~medAbstractView( void )
{
    delete d;
    d = NULL;
}


QWidget *medAbstractView::receiverWidget(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}

bool medAbstractView::isClosable() const
{
    return d->closable;
}

void medAbstractView::setClosable(bool closable)
{
    d->closable = closable;
}

void medAbstractView::setZoom (double zoom)
{
    if  (d->zoom == zoom)
        return;

    d->zoom = zoom;

    this->setZoom_impl(zoom);

    //TODO GPR: to correct
    //emit zoomChanged (zoom, d->linkCamera);
}

double medAbstractView::zoom(void) const
{
    return d->zoom;
}

void medAbstractView::setPan (const QVector2D &pan)
{
    if ( d->pan == pan )
        return;

    d->pan = pan;

    this->setPan_impl(pan);
    //TODO GPR: to correct
    //emit panChanged (pan, d->linkCamera);
}

QVector2D medAbstractView::pan(void) const
{
    return d->pan;
}


medViewBackend * medAbstractView::backend() const
{
    return 0;
}


void medAbstractView::toggleMaximize(bool state)
{
    emit maximizeRequested(state);
}


void medAbstractView::addDataType(const QString & dataDescription)
{
    if (d->DataTypes.contains(dataDescription))
    {
        unsigned int numDataTypes = d->DataTypes.value(dataDescription);
        numDataTypes++;
        d->DataTypes.insert(dataDescription,numDataTypes);
    }
    else
        d->DataTypes.insert(dataDescription, 1);
}

void medAbstractView::removeDataType(const QString & dataDescription)
{
    if (d->DataTypes.contains(dataDescription))
    {
        unsigned int numDataTypes = d->DataTypes.value(dataDescription);
        numDataTypes--;
        d->DataTypes[dataDescription]=numDataTypes;
    }
    else
        qDebug() << "The view" << this << "doesn't have any data of the type" << dataDescription;
}

QHash<QString, unsigned int> medAbstractView::dataTypes()
{
    return d->DataTypes;
}

bool medAbstractView::isDataTypeHandled(const QString &dataType) const
{

    bool isDataTypeHandled = false;
    if(this->interactors().isEmpty())
        qWarning() << "There is is no interactor attached to this data";

    foreach ( dtkAbstractViewInteractor *interactor, this->interactors())
    {
//        medAbstractInteractor *medInteractor = dynamic_cast <medAbstractInteractor *>(interactor);
//        if(medInteractor->isDataTypeHandled(dataType))
        {
            isDataTypeHandled = true;
            break;
        }
    }

    return isDataTypeHandled;
}
