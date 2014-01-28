#include "medAbstractImageView.h"


class medAbstractImageViewPrivate
{
public:
    bool linkPosition;
    bool linkCamera;
    bool linkWindowing;

    QVector3D  position;
    double     level;
    double     window;
    QVector3D  camPosition;
    QVector3D  camViewup;
    QVector3D  camFocal;
    double camParallelScale;
};


medAbstractImageView::medAbstractImageView(QObject * parent) : medAbstractLayeredView(parent), d (new medAbstractImageViewPrivate)
{
    d->linkPosition  = false;
    d->linkCamera    = false;
    d->linkWindowing = false;
    d->position = QVector3D(0.0, 0.0, 0.0);
    d->level = 0.0;
    d->window = 0.0;
    d->camPosition = QVector3D(0.0, 0.0, 0.0);
    d->camViewup = QVector3D(0.0, 0.0, 0.0);
    d->camFocal = QVector3D(0.0, 0.0, 0.0);
    d->camParallelScale = 1.0;

}

medAbstractImageView::~medAbstractImageView()
{

}

void medAbstractImageView::toggleLinkPosition (bool value)
{
    d->linkPosition = value;
}

bool medAbstractImageView::isPositionLinked() const
{
    return d->linkPosition;
}

void medAbstractImageView::toggleLinkWindowing (bool value)
{
    d->linkWindowing = value;
}

bool medAbstractImageView::isWindowingLinked() const
{
    return d->linkWindowing;
}

void medAbstractImageView::toggleLinkCamera (bool value)
{
    d->linkCamera = value;
}

bool medAbstractImageView::isCameraLinked() const
{
    return d->linkCamera;
}

void medAbstractImageView::setSlice (int slice)
{
    emit sliceChanged (slice, d->linkPosition);
}

void medAbstractImageView::moveToSliceAtPosition (const QVector3D &position)
{
    if ( d->position == position )
        return;

    d->position = position;

    emit positionChanged (position, d->linkPosition);
}

QVector3D medAbstractImageView::positionBeingViewed(void) const
{
    return d->position;
}

void medAbstractImageView::setWindowLevel (double level, double window)
{
    if ( ( d->level == level ) &&
         ( d->window == window) ) {
        return;
    }

    d->level = level;
    d->window = window;

    emit windowingChanged (level, window, d->linkWindowing);
}

void medAbstractImageView::windowLevel(double &level, double &window) const
{
    level = d->level;
    window = d->window;
}

void medAbstractImageView::setCamera (const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale)
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

void medAbstractImageView::camera (QVector3D &position, QVector3D &viewup, QVector3D &focal, double &parallelScale) const
{
    position = d->camPosition;
    viewup = d->camViewup;
    focal = d->camFocal;
    parallelScale = d->camParallelScale;
}


void medAbstractImageView::setOpacity(double opacity, int layer)
{
    emit opacityChanged(opacity, layer);
}

double medAbstractImageView::opacity(int layer) const
{
    DTK_DEFAULT_IMPLEMENTATION;
    return 1.0;
}
