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

#include <medAbstractViewCoordinates.h>

class medAbstractViewPrivate
{
public:

    QVector2D  pan;
    double     zoom;
    QHash<QString, unsigned int> DataTypes;
};

medAbstractView::medAbstractView(medAbstractView *parent) : dtkAbstractView(parent), d (new medAbstractViewPrivate)
{
    d->pan = QVector2D(0.0, 0.0);
    d->zoom = 1.0;


    // properties to keep up to date synchronization
    this->addProperty ("Closable",              QStringList() << "true" << "false");
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

void medAbstractView::setZoom (double zoom)
{
    if  (d->zoom == zoom)
        return;

    d->zoom = zoom;

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


void medAbstractView::setFullScreen( bool state )
{
    emit fullScreen( state );
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
        qDebug() << "Not DataType in the view! ";
}

QHash<QString, unsigned int> medAbstractView::dataTypes()
{
    return d->DataTypes;
}
