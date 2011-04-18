#include "medAbstractView.h"


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
  QList<dtkAbstractData *> dataList;
  
  

};

medAbstractView::medAbstractView(medAbstractView *parent) : dtkAbstractView(parent), d (new medAbstractViewPrivate)
{
    d->linkPosition  = false;
    d->linkCamera    = false;
    d->linkWindowing = false;
    d->currentLayer = 0;
    d->currentMeshLayer = 0;
    d->meshLayerCount = 0;
    
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
    lut << "Default";		// list of available lookup tables set
				// by subclass
    
    // properties to keep up to date synchronization
    this->addProperty ("Daddy",    QStringList() << "true" << "false");
    this->addProperty ("PositionLinked",  QStringList() << "true" << "false");
    this->addProperty ("CameraLinked",    QStringList() << "true" << "false");
    this->addProperty ("WindowingLinked", QStringList() << "true" << "false");
    
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
    delete d;
    d = NULL;
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
}

bool medAbstractView::positionLinked (void) const
{
    return d->linkPosition;
}

void medAbstractView::setLinkWindowing (bool value)
{
    d->linkWindowing = value;
}

bool medAbstractView::windowingLinked (void) const
{
    return d->linkWindowing;
}

void medAbstractView::setLinkCamera (bool value)
{
    d->linkCamera = value;
}

bool medAbstractView::cameraLinked (void) const
{
    return d->linkCamera;
}

void medAbstractView::setPosition (const QVector3D &position)
{
    this->onPositionChanged (position);
	d->position = position;
    emit positionChanged (position);
}

QVector3D medAbstractView::position(void) const
{
	return d->position;
}

void medAbstractView::setZoom (double zoom)
{
    this->onZoomChanged (zoom);
	d->zoom = zoom;
    emit zoomChanged (zoom);
}

double medAbstractView::zoom(void) const
{
	return d->zoom;
}

void medAbstractView::setPan (const QVector2D &pan)
{
    this->onPanChanged (pan);
	d->pan = pan;
    emit panChanged (pan);
}

QVector2D medAbstractView::pan(void) const
{
	return d->pan;
}

void medAbstractView::setWindowLevel (double level, double window)
{
    this->onWindowingChanged (level, window);
	d->level = level;
	d->window = window;
    emit windowingChanged (level, window);
}

void medAbstractView::windowLevel(double &level, double &window) const
{
	level = d->level;
	window = d->window;
}

void medAbstractView::setCamera (const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale)
{
    this->onCameraChanged (position, viewup, focal, parallelScale);
	d->camPosition = position;
	d->camViewup = viewup;
	d->camFocal = focal;
	d->camParallelScale = parallelScale;
    emit cameraChanged (position, viewup, focal, parallelScale);
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

void medAbstractView::setCurrentLayer(int layer)
{
    d->currentLayer = layer;
}

int medAbstractView::currentLayer(void) const
{
    return d->currentLayer;
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
    DTK_DEFAULT_IMPLEMENTATION;
}

void medAbstractView::addDataInList(dtkAbstractData * data)
{
    d->dataList.append(data);
}

dtkAbstractData * medAbstractView::dataInList(int layer)
{
    if (layer < d->dataList.size())
        return d->dataList.at(layer);

    return NULL;
}

void medAbstractView::setDataInList(int layer, dtkAbstractData * data)
{
    d->dataList[layer] = data;
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

void medAbstractView::emitViewPositionChangedEvent(const QVector3D &position)
{
	d->position = position;
    emit positionChanged(position);
}

void medAbstractView::emitViewZoomChangedEvent(double zoom)
{
	d->zoom = zoom;
    emit zoomChanged(zoom);
}

void medAbstractView::emitViewPanChangedEvent(const QVector2D &pan)
{
	d->pan = pan;
    emit panChanged(pan);
}

void medAbstractView::emitViewWindowingChangedEvent(double level, double window)
{
	d->level = level;
	d->window = window;
    emit windowingChanged(level,window);
}

void medAbstractView::emitViewCameraChangedEvent(const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale)
{
	d->camPosition = position;
	d->camViewup = viewup;
	d->camFocal = focal;
	d->camParallelScale = parallelScale;
    emit cameraChanged(position, viewup, focal, parallelScale);
}
