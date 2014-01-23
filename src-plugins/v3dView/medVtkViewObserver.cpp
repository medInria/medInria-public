#include "medVtkViewObserver.h"

#include <QVector>

#include <vtkImageView.h>
#include <vtkImageView2DCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInriaInteractorStyleRubberBandZoom.h>
#include <vtkImageView2D.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

#include <dtkCore/dtkSignalBlocker.h>

#include <medVtkView.h>

//=============================================================================
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
    this->m_slider = 0;
    this->m_lock = 0;
}

medVtkViewObserver::~medVtkViewObserver()
{

}

void medVtkViewObserver::Execute(vtkObject *caller, unsigned long event, void *callData)
{
    if(this->m_lock)
        return;

    if(!this->m_slider || !this->m_view)
        return;

    switch(event)
    {
    case vtkImageView::CurrentPointChangedEvent:
    {
        // a dummy scope just to 'controll' the life time of the dtkSignalBlocker.
        {
            dtkSignalBlocker blocker ( this->m_slider );
            unsigned int zslice = this->m_view->view2d()->GetSlice();
            this->m_slider->setValue(zslice);
            this->m_slider->update();
        }
        const double *pos = this->m_view->currentView()->GetCurrentPoint();
        QVector3D qpos(doubleToQtVector3D(pos));
        this->m_view->setToSliceAtPosition(qpos);
        break;
    }
    case vtkImageView2DCommand::CameraZoomEvent:
    {
        double zoom = this->m_view->currentView()->GetZoom();
        this->m_view->setZoom(zoom);
        break;
    }
    case vtkImageView2DCommand::CameraPanEvent:
    {
        const double *pan = this->m_view->view2d()->GetPan();
        QVector2D qpan (pan[0], pan[1]);
        this->m_view->setPan(qpan);
        break;
    }
    case vtkImageView::WindowLevelChangedEvent:
    {
        double level = this->m_view->currentView()->GetColorLevel();
        double window = this->m_view->currentView()->GetColorWindow();
        this->m_view->setWindowLevel(level, window);
        break;
    }
    case vtkCommand::InteractionEvent:
    {
        double *pos = this->m_view->renderer3d()->GetActiveCamera()->GetPosition();
        double *vup = this->m_view->renderer3d()->GetActiveCamera()->GetViewUp();
        double *foc = this->m_view->renderer3d()->GetActiveCamera()->GetFocalPoint();
        double   ps = this->m_view->renderer3d()->GetActiveCamera()->GetParallelScale();
        QVector3D position(doubleToQtVector3D(pos));
        QVector3D viewup(doubleToQtVector3D(vup));
        QVector3D focal(doubleToQtVector3D(foc));
        this->m_view->setCamera(position, viewup, focal, ps);
        break;
    }
    case vtkCommand::KeyPressEvent:
    {
        vtkRenderWindowInteractor *windowInteractor = static_cast<vtkRenderWindowInteractor*>(caller);
        if (windowInteractor->GetControlKey())
        {
            m_view->setProperty( "ZoomMode" , "RubberBand" );
            dynamic_cast<vtkInriaInteractorStyleRubberBandZoom*>(m_view->view2d()->GetInteractor()->GetInteractorStyle())->RightButtonModeOn();
            if (m_view->property("MouseInteraction")!="Zooming")
                dynamic_cast<vtkInriaInteractorStyleRubberBandZoom*>(m_view->view2d()->GetInteractor()->GetInteractorStyle())->LeftButtonModeOff();
        }
        break;
    }
    case vtkCommand::KeyReleaseEvent:
    {
        vtkRenderWindowInteractor *windowInteractor = static_cast<vtkRenderWindowInteractor*>(caller);
        if (!windowInteractor->GetControlKey())
        {
            if (this->m_view->property("ZoomMode")=="RubberBand")
                this->m_view->setProperty( "ZoomMode" , "Normal" );
        }
        break;
    }
    }// switch end
}
