#include <medVtkViewObserver.h>

#include <QVector>

#include <vtkImageView.h>
#include <vtkImageView2DCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInriaInteractorStyleRubberBandZoom.h>
#include <vtkImageView2D.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <medVtkViewBackend.h>

#include <medVtkView.h>


// Construct a QVector3d from pointer-to-double
inline QVector3D doubleToQtVector3D ( const double * v )
{
    return QVector3D ( v[0], v[1], v[2] );
}
// Convert a QVector3D to array of double.
inline void qtVector3dToDouble ( const QVector3D & inV , double * outV )
{
    outV[0] = inV.x();
  outV[1] = inV.y();
    outV[2] = inV.z();
}
// Convert QColor to vtk's double[3]. (no alpha)
inline void qtColorToDouble ( const QColor & color, double * cv )
{
    cv[0] = color.redF();
    cv[1] = color.greenF();
    cv[2] = color.blueF();
}

medVtkViewObserver::medVtkViewObserver()
{
    this->m_locked = false;
}

medVtkViewObserver::~medVtkViewObserver()
{

}

void medVtkViewObserver::setView (medVtkView *view)
{
    this->m_view = view;

    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(view->backend());
    this->view2d = backend->view2D;
    this->view3d = backend->view3D;
}

void medVtkViewObserver::Execute(vtkObject *caller, unsigned long event, void *callData)
{
    if(this->m_locked)
        return;

    if(!this->m_view)
        return;

    switch(event)
    {
    case vtkImageView::CurrentPointChangedEvent:
    {
        const double *pos = this->view2d->GetCurrentPoint();
        QVector3D qpos(doubleToQtVector3D(pos));

        m_view->positionViewedChanged(qpos);
        break;
    }
    case vtkImageView2DCommand::CameraZoomEvent:
    {
        double zoom = this->view2d->GetZoom();
        m_view->zoomChanged(zoom);
        break;
    }
    case vtkImageView2DCommand::CameraPanEvent:
    {
        const double *pan = this->view2d->GetPan();
        QVector2D qpan (pan[0], pan[1]);
        m_view->panChanged(qpan);
        break;
    }
    case vtkImageView::WindowLevelChangedEvent:
    {
        unsigned int layer = this->m_view->currentLayer();
        double level = this->view2d->GetColorLevel(layer);
        double window = this->view2d->GetColorWindow(layer);
        m_view->windowLevelChanged(window, level, layer);

        break;
    }
    case vtkCommand::InteractionEvent:
    {
        double *pos = this->view3d->GetRenderer()->GetActiveCamera()->GetPosition();
        double *vup = this->view3d->GetRenderer()->GetActiveCamera()->GetViewUp();
        double *foc = this->view3d->GetRenderer()->GetActiveCamera()->GetFocalPoint();
        double   ps = this->view3d->GetRenderer()->GetActiveCamera()->GetParallelScale();
        QVector3D position(doubleToQtVector3D(pos));
        QVector3D viewup(doubleToQtVector3D(vup));
        QVector3D focal(doubleToQtVector3D(foc));
        m_view->cameraChanged(position, viewup, focal, ps);
        break;
    }
    case vtkCommand::KeyPressEvent:
    {
       /* vtkRenderWindowInteractor *windowInteractor = static_cast<vtkRenderWindowInteractor*>(caller);
        if (windowInteractor->GetControlKey())
        {
            m_view->setProperty( "ZoomMode" , "RubberBand" );
            dynamic_cast<vtkInriaInteractorStyleRubberBandZoom*>(m_view->view2d()->GetInteractor()->GetInteractorStyle())->RightButtonModeOn();
            if (m_view->property("MouseInteraction")!="Zooming")
                dynamic_cast<vtkInriaInteractorStyleRubberBandZoom*>(m_view->view2d()->GetInteractor()->GetInteractorStyle())->LeftButtonModeOff();
        }*/
        break;
    }
    case vtkCommand::KeyReleaseEvent:
    {
       /* vtkRenderWindowInteractor *windowInteractor = static_cast<vtkRenderWindowInteractor*>(caller);
        if (!windowInteractor->GetControlKey())
        {
            if (this->m_view->property("ZoomMode")=="RubberBand")
              this->m_view->setProperty( "ZoomMode" , "Normal" );
        }*/
        break;
    }
    }// switch end
}
