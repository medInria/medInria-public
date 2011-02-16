#include "medAbstractView.h"

class medAbstractViewPrivate
{
public:
  bool linkPosition;
  bool linkCamera;
  bool linkWindowing;
  
  int currentLayer;
};

medAbstractView::medAbstractView(medAbstractView *parent) : dtkAbstractView(parent), d (new medAbstractViewPrivate)
{
    d->linkPosition  = false;
    d->linkCamera    = false;
    d->linkWindowing = false;
    d->currentLayer = 0;
  
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
    emit positionChanged (position);
}

void medAbstractView::setZoom (double zoom)
{
    this->onZoomChanged (zoom);
    emit zoomChanged (zoom);
}

void medAbstractView::setPan (const QVector2D &pan)
{
    this->onPanChanged (pan);
    emit panChanged (pan);
}

void medAbstractView::setWindowLevel (double level, double window)
{
    this->onWindowingChanged (level, window);
    emit windowingChanged (level, window);
}

void medAbstractView::setCamera (const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale)
{
    this->onCameraChanged (position, viewup, focal, parallelScale);
    emit cameraChanged (position, viewup, focal, parallelScale);
}

void medAbstractView::setVisibility(int visibility, int layer)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

int medAbstractView::visibility(int layer) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return 1;
}

void medAbstractView::setOpacity(double opacity, int layer)
{
    DTK_DEFAULT_IMPLEMENTATION;
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

void medAbstractView::emitViewPositionChangedEvent(const QVector3D &position)
{
    emit positionChanged(position);
}

void medAbstractView::emitViewZoomChangedEvent(double zoom)
{
    emit zoomChanged(zoom);
}

void medAbstractView::emitViewPanChangedEvent(const QVector2D &pan)
{
    emit panChanged(pan);
}

void medAbstractView::emitViewWindowingChangedEvent(double level, double window)
{
    emit windowingChanged(level,window);
}

void medAbstractView::emitViewCameraChangedEvent(const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale)
{
    emit cameraChanged(position, viewup, focal, parallelScale);
}
