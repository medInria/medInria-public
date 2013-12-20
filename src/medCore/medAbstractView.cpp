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

    QStringList lut;
    lut << "Default";           // list of available lookup tables set
    // by subclass

    // properties to keep up to date synchronization
    this->addProperty ("Closable",              QStringList() << "true" << "false");
    // properties acting on image display
    this->addProperty ("Orientation",           QStringList() << "Axial" << "Sagittal" << "Coronal" << "3D");
    this->addProperty ("LookupTable",           lut);
    this->addProperty ("ShowScalarBar",         QStringList() << "true" << "false");
    this->addProperty ("ShowAxis",              QStringList() << "true" << "false");
    this->addProperty ("ShowRuler",             QStringList() << "true" << "false");
    this->addProperty ("ShowAnnotations",       QStringList() << "true" << "false");
    this->addProperty ("3DMode",                QStringList() << "VR" << "MPR" << "MIP - Maximum" << "MIP - Minimum" << "Off");
    this->addProperty ("Renderer",              QStringList() << "GPU" << "Ray Cast / Texture" << "Ray Cast" << "Texture" << "Default");
    this->addProperty ("UseLOD",                QStringList() << "On" << "Off" );
    this->addProperty ("Cropping",              QStringList() << "true" << "false");
    this->addProperty ("DepthPeeling",          QStringList() << "true" << "false");
    this->addProperty ("Preset",                QStringList() << "None" << "VR Muscles&Bones"
                       << "Vascular I" << "Vascular II" << "Vascular III" << "Vascular IV"
                       << "Standard" << "Soft" << "Soft on White" << "Soft on Blue"
                       << "Red on White" << "Glossy");
    // image interaction
    this->addProperty ("MouseInteraction",      QStringList() << "Zooming" << "Windowing" << "Slicing" << "Measuring");

    // do not set properties, leave it to subclass
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
