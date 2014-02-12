#include <medVtkViewSignalsEmitter.h>
#include <medVtkView.h>

medVtkViewSignalsEmitter::medVtkViewSignalsEmitter()
{
    m_view = NULL;
}


void medVtkViewSignalsEmitter::setView (medVtkView *view)
{
    if(m_view != NULL)
        this->disconnect(m_view);

    m_view = view;

    connect(this, SIGNAL(sliceChanged(int)), m_view, SIGNAL(sliceChanged(int)));
    connect(this, SIGNAL(windowLevelChanged(double, double)), m_view, SIGNAL(windowLevelChanged(double, double)));
    connect(this, SIGNAL(zoomChanged(double)), m_view, SIGNAL(zoomChanged(double)));
    connect(this, SIGNAL(panChanged(QVector2D)), m_view, SIGNAL(panChanged(QVector2D)));
    connect(this, SIGNAL(positionViewedChanged(QVector3D)), m_view, SIGNAL(positionViewedChanged(QVector3D)));
    connect(this, SIGNAL(cameraChanged(QVector3D,QVector3D,QVector3D,double)),
            m_view, SIGNAL(cameraChanged(QVector3D,QVector3D,QVector3D,double)));
}

void medVtkViewSignalsEmitter::emitSliceChanged(int slice)
{
    emit sliceChanged(slice);
}

void medVtkViewSignalsEmitter::emitWindowLevelChanged(double window, double level)
{
    emit windowLevelChanged(window, level);
}

void medVtkViewSignalsEmitter::emitZoomChanged(double zoom)
{
    emit zoomChanged(zoom);
}

void medVtkViewSignalsEmitter::emitPanChanged(const QVector2D& pan)
{
    emit panChanged(pan);
}

void medVtkViewSignalsEmitter::emitPositionViewedChanged(const QVector3D& position)
{
    emit positionViewedChanged(position);
}

void medVtkViewSignalsEmitter::emitCameraChanged(const QVector3D &position,
                   const QVector3D &viewup,
                   const QVector3D &focal,
                   double parallelScale)
{
    emit cameraChanged(position, viewup, focal, parallelScale);
}
