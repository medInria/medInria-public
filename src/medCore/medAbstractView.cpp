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
    bool linkPosition;
    bool linkCamera;
    bool linkWindowing;

    QVector3D  position;
    QVector2D  pan;
    double     zoom;
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

    QHash<QString, unsigned int> DataTypes;

    QList < dtkSmartPointer <medAbstractData> > layersDataList;

};

medAbstractView::medAbstractView(medAbstractView *parent) : dtkAbstractView(parent), d (new medAbstractViewPrivate)
{
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
    d->pan = QVector2D(0.0, 0.0);
    d->zoom = 1.0;
    d->level = 0.0;
    d->window = 0.0;
    d->camPosition = QVector3D(0.0, 0.0, 0.0);
    d->camViewup = QVector3D(0.0, 0.0, 0.0);
    d->camFocal = QVector3D(0.0, 0.0, 0.0);
    d->camParallelScale = 1.0;

    QStringList lut;
    lut << "Default";           // list of available lookup tables set
    // by subclass

    // properties to keep up to date synchronization
    this->addProperty ("Closable",              QStringList() << "true" << "false");
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
}


medAbstractView::medAbstractView(const medAbstractView& view) : dtkAbstractView(view)
{
    // copy constructor not implemented!
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::addLayer(medAbstractData *data)
{
    return d->layersDataList.append(data);
}

bool medAbstractView::removeLayer(medAbstractData *data)
{
    int res = d->layersDataList.removeAll(data);
    return res > 0;
}

void medAbstractView::removeLayerAt(unsigned int layer)
{
    d->layersDataList.removeAt(layer);
}

void medAbstractView::insertLayer(unsigned int layer, medAbstractData *data)
{
    d->layersDataList.insert(layer, data);
}

void medAbstractView::moveLayer(unsigned int fromLayer, unsigned int toLayer)
{
    d->layersDataList.move(fromLayer, toLayer);
}

medAbstractData * medAbstractView::layerData(unsigned int layer)
{
    if ((layer < d->layersDataList.size()) && (layer >= 0))
        return d->layersDataList[layer];

    return NULL;
}

bool medAbstractView::contains(medAbstractData * data)
{
    return d->layersDataList.contains(data);
}

unsigned int medAbstractView::layersCount() const
{
    return d->layersDataList.count();
}


QWidget *medAbstractView::receiverWidget(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}

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

void medAbstractView::setZoom (double zoom)
{
    if  (d->zoom == zoom)
        return;

    d->zoom = zoom;

    emit zoomChanged (zoom, d->linkCamera);
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

    emit panChanged (pan, d->linkCamera);
}

QVector2D medAbstractView::pan(void) const
{
    return d->pan;
}

void medAbstractView::setWindowLevel (double level, double window)
{
    if ( ( d->level == level ) &&
         ( d->window == window) ) {
        return;
    }

    d->level = level;
    d->window = window;

    emit windowingChanged (level, window, d->linkWindowing);
}

void medAbstractView::windowLevel(double &level, double &window) const
{
    level = d->level;
    window = d->window;
}

void medAbstractView::setCamera (const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale)
{
    if (    (d->camPosition == position) &&
            (d->camViewup   == viewup)   &&
            (d->camFocal    == focal) &&
            (d->camParallelScale == parallelScale) ) {

        return;
    }

    d->camPosition = position;
    d->camViewup = viewup;
    d->camFocal = focal;
    d->camParallelScale = parallelScale;

    emit cameraChanged (position, viewup, focal, parallelScale, d->linkCamera);
}

void medAbstractView::camera (QVector3D &position, QVector3D &viewup, QVector3D &focal, double &parallelScale) const
{
    position = d->camPosition;
    viewup = d->camViewup;
    focal = d->camFocal;
    parallelScale = d->camParallelScale;
}

void medAbstractView::setVisibility(bool visibility, int layer)
{
    emit visibilityChanged(visibility, layer);
}

bool medAbstractView::visibility(int layer) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return true;
}

void medAbstractView::setOpacity(double opacity, int layer)
{
    emit opacityChanged(opacity, layer);
}

double medAbstractView::opacity(int layer) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return 1.0;
}

medViewBackend * medAbstractView::backend() const
{
    return 0;
}

void medAbstractView::setCurrentLayer(int layer)
{
    d->currentLayer = layer;
}

bool medAbstractView::hasImage(void) const
{
    return d->hasImage;
}

int medAbstractView::currentLayer(void) const
{
    return d->currentLayer;
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

    QHash<QString, unsigned int>::const_iterator i = d->DataTypes.constBegin();
    while (i != d->DataTypes.constEnd()) {
        d->hasImage=false;
        if (i.value()>0 && i.key().contains("Image"))
        {
            d->hasImage=true;
            break;
        }
        ++i;
    }
    qDebug() << d->hasImage<<endl;//JGG
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

    QHash<QString, unsigned int>::const_iterator i = d->DataTypes.constBegin();
    while (i != d->DataTypes.constEnd()) {
        d->hasImage=false;
        qDebug() << i.key() << ": " << i.value() << endl;
        if (i.value()>0 && i.key().contains("Image"))
        {
            d->hasImage=true;
            break;
        }
        ++i;
    }
}

QHash<QString, unsigned int> medAbstractView::dataTypes()
{
    return d->DataTypes;
}


medAbstractView::~medAbstractView( void )
{
    delete d;
    d = NULL;
}


void medAbstractView::setFullScreen( bool state )
{
    emit fullScreen( state );
}
