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
}

void medVtkViewSignalsEmitter::emitSliceChanged(int slice)
{
    emit sliceChanged(slice);
}

void medVtkViewSignalsEmitter::emitWindowLevelChanged(double window, double level)
{
    emit windowLevelChanged(window, level);
}
