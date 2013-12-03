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
    QList< dtkSmartPointer <dtkAbstractData> > dataList;
    //QMap <int, dtkSmartPointer<dtkAbstractData> > dataList;

    // dtkSmartPointer<dtkAbstractData> sharedData;

    QColor color; // The color used to represent this view in other views.

    QHash<QString, unsigned int> DataTypes;

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
    this->addProperty ("Daddy",                 QStringList() << "true" << "false");
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

void medAbstractView::setColorLookupTable(int min_range,
                                          int max_range,
                                          int size,
                                          const int & table)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::setColorLookupTable( QList<double> scalars,
                                           QList<QColor> colors )
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::setTransferFunctions( QList<double> scalars,
                                            QList<QColor> colors )
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::getTransferFunctions( QList<double> & scalars,
                                            QList<QColor> & colors )
{
    DTK_DEFAULT_IMPLEMENTATION;
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
    this->onSliceChanged (slice);
    emit sliceChanged (slice, d->linkPosition);
}

void medAbstractView::setPosition (const QVector3D &position)
{
    if ( d->position == position )
        return;

    d->position = position;
    this->onPositionChanged (position);
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
    this->onZoomChanged (zoom);
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
    this->onPanChanged (pan);
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
    this->onWindowingChanged (level, window);
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
    this->onCameraChanged (position, viewup, focal, parallelScale);
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
    this->onVisibilityChanged(visibility, layer);
    emit visibilityChanged(visibility, layer);
}

bool medAbstractView::visibility(int layer) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return true;
}

void medAbstractView::setOpacity(double opacity, int layer)
{
    this->onOpacityChanged(opacity, layer);
    emit opacityChanged(opacity, layer);
}

double medAbstractView::opacity(int layer) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return 1.0;
}
QString medAbstractView::getLUT(int layer) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return "";
}
QString medAbstractView::getPreset(int layer) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return "";
}

medViewBackend * medAbstractView::backend() const
{
    return 0;
}

void medAbstractView::setColor(const QColor & color)
{
    if ( d->color != color) {
        d->color = color;
        emitColorChangedEvent();
    }
}


QColor medAbstractView::color() const
{
    return d->color;
}

void medAbstractView::setCurrentLayer(int layer)
{
    d->currentLayer = layer;
}

int medAbstractView::currentLayer(void) const
{
    return d->currentLayer;
}

bool medAbstractView::hasImage(void) const
{
    return d->hasImage;
}

int medAbstractView::layerCount(void) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return 0;
}

void medAbstractView::setCurrentMeshLayer(int meshLayer)
{
    d->currentMeshLayer = meshLayer;
}

int medAbstractView::currentMeshLayer(void) const
{
    return d->currentMeshLayer;
}

void medAbstractView::setMeshLayerCount(int meshLayerCount)
{
    d->meshLayerCount = meshLayerCount;
}

int medAbstractView::meshLayerCount(void) const
{
    return d->meshLayerCount;
}


void medAbstractView::removeOverlay(int layer)
{
    //JGG qDebug()<<"ViewDataListSize"<<d->dataList.size();

    if (layer >= 0 && layer < d->dataList.size())
    {
        dtkAbstractData * oldData = d->dataList[layer];
        medAbstractView::removeDataType(oldData->identifier());
        emit (dataRemoved(oldData, layer));
        emit (dataRemoved(layer));
        d->dataList.removeAt(layer);

    }
}

void medAbstractView::onSliceChanged (int slice)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::addDataInList(dtkAbstractData * data, int layer)
{

    if(layer >=0 &&  layer < d->dataList.size() )
        d->dataList[layer] = data;
    else
        d->dataList.append(data);
    medAbstractView::addDataType(data->identifier());
}

void medAbstractView::addDataInList(dtkAbstractData * data)
{

    d->dataList.append(data);
    medAbstractView::addDataType(data->identifier());
}

dtkAbstractData * medAbstractView::dataInList(int layer) const
{
    if (layer >=0 &&  layer < d->dataList.size())
        return d->dataList[layer];

    return NULL;
}

bool medAbstractView::isInList(dtkAbstractData * data, int layer)
{

    if(layer == d->dataList.size())
        return false;
    if (d->dataList.contains(data) && d->dataList[layer]==data)

    {
        qDebug() << "data is in list,layer:" << layer;
        return true;
    }

    return false;
}

bool medAbstractView::isInList(dtkAbstractData * data)
{
    if (d->dataList.contains(data))

    {
        qDebug() << "data is already in list";
        return true;
    }
    return false;
}


void medAbstractView::setDataInList(dtkAbstractData * data, int layer)
{
    // start by removing the data type if layer already exists
    dtkAbstractData * oldData = NULL;

    if (layer >=0 &&  layer < d->dataList.size()) {
        oldData = d->dataList[layer];
        removeDataType(d->dataList[layer]->identifier());
    }

    if ( oldData && ( data != oldData ) ) {
        emit dataRemoved(oldData,layer);
    }
    d->dataList[layer] = data;

    addDataType(data->identifier());
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

void medAbstractView::onPositionChanged (const QVector3D &position)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::onZoomChanged (double zoom)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::onPanChanged (const QVector2D &pan)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::onWindowingChanged (double level, double window)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::onCameraChanged (const QVector3D &position,
                                       const QVector3D &viewup,
                                       const QVector3D &focal,
                                       double parallelScale)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::onVisibilityChanged(bool visibility, int layer)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::onOpacityChanged(double opacity, int layer)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::emitViewSliceChangedEvent(int slice)
{
    emit sliceChanged(slice, d->linkPosition);
}

void medAbstractView::onObliqueSettingsChanged(const medAbstractView * vsender)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::emitViewPositionChangedEvent(const QVector3D &position)
{
    d->position = position;
    emit positionChanged(position, d->linkPosition);
}

void medAbstractView::emitViewZoomChangedEvent(double zoom)
{
    d->zoom = zoom;
    emit zoomChanged(zoom, d->linkCamera);
}

void medAbstractView::emitViewPanChangedEvent(const QVector2D &pan)
{
    d->pan = pan;
    emit panChanged(pan, d->linkCamera);
}

void medAbstractView::emitViewWindowingChangedEvent(double level, double window)
{
    d->level = level;
    d->window = window;
    emit windowingChanged(level,window, d->linkWindowing);
}

void medAbstractView::emitViewCameraChangedEvent(const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale)
{
    d->camPosition = position;
    d->camViewup = viewup;
    d->camFocal = focal;
    d->camParallelScale = parallelScale;
    emit cameraChanged(position, viewup, focal, parallelScale, d->linkCamera);
}

void medAbstractView::setSharedDataPointer( dtkSmartPointer<dtkAbstractData> data )
{
    // set a reference to our view that gets destroyed when the view terminates
    //d->sharedData = data;

    //dtkAbstractData *dtkdata = d->sharedData.data();
    this->setData(data);
}

void medAbstractView::setSharedDataPointer( dtkSmartPointer<dtkAbstractData> data,
                                            int layer)
{
    this->setData(data,layer);
}

medAbstractView::~medAbstractView( void )
{
    delete d;
    d = NULL;
}

void medAbstractView::emitObliqueSettingsChangedEvent()
{
    emit obliqueSettingsChanged (this);
}

void medAbstractView::emitColorChangedEvent()
{
    emit colorChanged();
}

void medAbstractView::onRemoveViewFromPool( medAbstractView * viewLeaving )
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::onAppendViewToPool( medAbstractView * viewAppended )
{
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::setFullScreen( bool state )
{
    emit fullScreen( state );
}
