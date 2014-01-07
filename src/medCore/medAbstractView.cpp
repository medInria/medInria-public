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
<<<<<<< HEAD
    double     level;
    double     window;
    QVector3D  camPosition;
    QVector3D  camViewup;
    QVector3D  camFocal;
    double camParallelScale;

    int currentLayer;
    int currentMeshLayer;
    int meshLayerCount;
    bool hasImage;
    QList< dtkSmartPointer <medAbstractData> > dataList;
    //QMap <int, dtkSmartPointer<medAbstractData> > dataList;

    int currentLayer;

=======
>>>>>>> medAbstractView refactoring
    QHash<QString, unsigned int> DataTypes;
};

medAbstractView::medAbstractView(medAbstractView *parent) : dtkAbstractView(parent), d (new medAbstractViewPrivate)
{
<<<<<<< HEAD
    d->linkPosition  = false;
    d->linkCamera    = false;
    d->linkWindowing = false;
    d->currentLayer = 0;

    d->position = QVector3D(0.0, 0.0, 0.0);
    d->pan = QVector2D(0.0, 0.0);
    d->zoom = 1.0;
    d->level = 0.0;
    d->window = 0.0;
    d->camPosition = QVector3D(0.0, 0.0, 0.0);
    d->camViewup = QVector3D(0.0, 0.0, 0.0);
    d->camFocal = QVector3D(0.0, 0.0, 0.0);
    d->camParallelScale = 1.0;

    d->currentMeshLayer = 0;
    d->meshLayerCount = 0;
    d->hasImage= false;

    d->position = QVector3D(0.0, 0.0, 0.0);
=======
>>>>>>> medAbstractView refactoring
    d->pan = QVector2D(0.0, 0.0);
    d->zoom = 1.0;


    // properties to keep up to date synchronization
    this->addProperty ("Closable",              QStringList() << "true" << "false");
<<<<<<< HEAD
    this->addProperty ("PositionLinked",        QStringList() << "true" << "false");
    this->addProperty ("CameraLinked",          QStringList() << "true" << "false");
    this->addProperty ("WindowingLinked",       QStringList() << "true" << "false");

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
=======
>>>>>>> removing a good part of the v3dView properties
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

<<<<<<< HEAD
void medAbstractView::setLinkPosition (bool value)
{
    d->linkPosition = value;
    if (value)
        setProperty("PositionLinked","true");
    else
        setProperty("PositionLinked","false");
}

bool medAbstractView::positionLinked() const
{
    return d->linkPosition;
}

void medAbstractView::setLinkWindowing (bool value)
{
    d->linkWindowing = value;
}

bool medAbstractView::windowingLinked() const
{
    return d->linkWindowing;
}

void medAbstractView::setLinkCamera (bool value)
{
    d->linkCamera = value;
}

bool medAbstractView::cameraLinked() const
{
    return d->linkCamera;
}

void medAbstractView::setSlice (int slice)
{
    emit sliceChanged (slice, d->linkPosition);
}

void medAbstractView::setPosition (const QVector3D &position)
{
    if ( d->position == position )
        return;

    d->position = position;

    emit positionChanged (position, d->linkPosition);
}

QVector3D medAbstractView::position(void) const
{
    return d->position;
}

=======
>>>>>>> medAbstractView refactoring
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



/*
void medAbstractView::removeOverlay(int layer)
{
    if (layer >= 0 && layer < d->dataList.size())
    {
        dtkAbstractData * oldData = d->dataList[layer];
        medAbstractView::removeDataType(oldData->identifier());
        emit (dataRemoved(oldData, layer));
        emit (dataRemoved(layer));
        d->dataList.removeAt(layer);

    }
}*/


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
