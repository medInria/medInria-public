/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "polygonRoi.h"

#include <medRoiManager.h>
#include <medVtkViewBackend.h>

#include <vtkContourOverlayRepresentation.h>
#include <vtkContourWidget.h>
#include <vtkImageView2D.h>
#include <vtkPolyData.h>
#include <vtkProperty2D.h>
#include <vtkSmartPointer.h>
#include <vtkWidgetEventTranslator.h>

static double ROI_COLOR[3] = {0, 0, 1.0};
static double MASTER_ROI_COLOR[3] = {0.0, 1.0, 0.0};
static double SELECTED_ROI_COLOR[3] = {1.0, 0.533, 0.2};

class PolygonRoiObserver : public vtkCommand
{
public:
    static PolygonRoiObserver* New()
    {
        return new PolygonRoiObserver;
    }

    void Execute ( vtkObject *caller, unsigned long event, void *callData );

    void setRoi ( polygonRoi *roi )
    {
        this->roi = roi;
    }
    
    inline void lock()
    {
        this->m_lock = 1;
    }
    inline void unlock()
    {
        this->m_lock = 0;
    }

protected:
    PolygonRoiObserver();
    ~PolygonRoiObserver();

private:
    int m_lock;
    polygonRoi * roi;
};

PolygonRoiObserver::PolygonRoiObserver()
{
    this->m_lock = 0;
}

PolygonRoiObserver::~PolygonRoiObserver(){}

void PolygonRoiObserver::Execute ( vtkObject *caller, unsigned long event, void *callData )
{
    if (this->m_lock )
    {
        return;
    }

    if (!this->roi->getView())
    {
        return;
    }

    switch ( event )
    {
        case vtkImageView2D::SliceChangedEvent:
        default:
        {
            roi->showOrHide();
            break;
        }
        case vtkCommand::EndInteractionEvent:
        {
            emit roi->selected(); // will trigger the computation of new statistics
            roi->saveState();
        }
        case vtkCommand::PlacePointEvent:
        {
            roi->saveState();
        }
        case vtkCommand::MouseMoveEvent:
        {
            roi->setMasterRoi(true);
        }
    }
}

class polygonRoiPrivate
{
public:
    ~polygonRoiPrivate()
    {
        contour->RemoveAllObservers();
        contour->GetContourRepresentation()->RemoveAllObservers();
        contour->Delete();
        view->RemoveObserver(observer);
        observer->Delete();
        view = nullptr;
        if (copyRoi)
        {
            delete copyRoi;
        }
    }

    vtkContourWidget *contour;
    vtkImageView2D *view;
    PolygonRoiObserver *observer;
    bool invisibilityForced;
    bool visibilityForced;
    polygonRoi *copyRoi;
};

polygonRoi::polygonRoi(vtkImageView2D *view, medAbstractRoi *parent )
    : medAbstractRoi(parent)
    , d(new polygonRoiPrivate)
{
    vtkSmartPointer<vtkContourOverlayRepresentation> contourRep = vtkSmartPointer<vtkContourOverlayRepresentation>::New();
    contourRep->GetLinesProperty()->SetLineWidth(3);
    contourRep->GetProperty()->SetPointSize(6);
    contourRep->GetProperty()->SetColor(1.0, 0.0, 0.0);
    contourRep->GetActiveProperty()->SetOpacity(0);

    d->contour = vtkContourWidget::New();
    d->contour->SetRepresentation(contourRep);
    contourRep->SetRenderer(view->GetRenderer());
    d->contour->SetInteractor(view->GetInteractor());

    d->contour->GetEventTranslator()->SetTranslation(vtkCommand::RightButtonPressEvent,0);
    
    d->view = view;
    setOrientation(view->GetViewOrientation());
    setIdSlice(view->GetSlice());
    d->observer = PolygonRoiObserver::New();
    d->observer->setRoi(this);
    d->view->AddObserver(vtkImageView2D::SliceChangedEvent,d->observer,0);
    d->contour->AddObserver(vtkCommand::EndInteractionEvent,d->observer,0);
    d->contour->AddObserver(vtkCommand::MouseMoveEvent,d->observer,0);
    contourRep->AddObserver(vtkCommand::PlacePointEvent,d->observer,0);

    d->invisibilityForced = false;
    d->visibilityForced = false;
    d->copyRoi = nullptr;
    setMasterRoi(true);
}

polygonRoi::~polygonRoi()
{
    delete d;
    d = nullptr;
}

vtkContourWidget * polygonRoi::getContour()
{
    return d->contour;
}

void polygonRoi::undo()
{
    vtkContourOverlayRepresentation *contourRep =
            dynamic_cast<vtkContourOverlayRepresentation*>(d->contour->GetContourRepresentation());
    contourRep->Undo();
}

void polygonRoi::redo()
{
    vtkContourOverlayRepresentation *contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(d->contour->GetContourRepresentation());
    contourRep->Redo();
}

void polygonRoi::Off()
{
    if (d->visibilityForced)
    {
        return;
    }
    d->contour->Off();
}

void polygonRoi::On()
{
    if (d->invisibilityForced)
    {
        return;
    }
    d->contour->On();
}

void polygonRoi::forceInvisibilityOff()
{
    d->invisibilityForced = false;
    showOrHide();
}

void polygonRoi::forceInvisibilityOn()
{
    d->visibilityForced = false;
    d->invisibilityForced = true;
    Off();
}

void polygonRoi::forceVisibilityOn()
{
    d->invisibilityForced = false;
    d->visibilityForced = true;
    On();
}

void polygonRoi::forceVisibilityOff()
{
    d->visibilityForced = false;
    showOrHide();
}

void polygonRoi::showOrHide()
{
    if (!d->view->GetRenderWindow())
    {
        return;
    }

    if (getIdSlice() == static_cast<unsigned int>(d->view->GetSlice() )
            && getOrientation() == static_cast<unsigned char>(d->view->GetViewOrientation()))
    {
        On();
    }
    else
    {
        d->view->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_CROSSHAIR);
        Off();
    }
}

vtkImageView2D * polygonRoi::getView()
{
    return d->view;
}

void polygonRoi::lockObserver(bool val)
{
    if (val)
    {
        d->observer->lock();
    }
    else
    {
        d->observer->unlock();
    }
}

QString polygonRoi::type()
{
    return "Polygon";
}

QString polygonRoi::info()
{
    QString info = "Orientation: ";
    switch (getOrientation())
    {
        case 0:
            info = info + "Sagittal - ";
            break;
        case 1:
            info = info + "Coronal - ";
            break;
        case 2:
            info = info + "Axial - ";
            break;
    }

    return info + " Slice " + QString::number(getIdSlice()+1);
}

void polygonRoi::select()
{
    setColor(SELECTED_ROI_COLOR);
    medAbstractRoi::select();
}

void polygonRoi::unselect()
{
    setRightColor();
    medAbstractRoi::unselect();
}

void polygonRoi::computeRoiStatistics(){}

void polygonRoi::saveState()
{
    if (!dynamic_cast<vtkContourOverlayRepresentation*>(d->contour->GetContourRepresentation())->SaveState())
    {
        return;
    }
    RoiUndoRedo *command = new RoiUndoRedo(this,type());
    medRoiManager::instance()->addToUndoRedoStack(command);
    emit stateSaved();
}

bool polygonRoi::copyROI(medAbstractView *view)
{
    if (getContour()->GetWidgetState()==2)
    {
        vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(view->backend())->view2D;
        d->copyRoi = new polygonRoi(view2d);
        vtkSmartPointer<vtkPolyData> Nodes = vtkSmartPointer<vtkPolyData>::New();
        d->contour->GetContourRepresentation()->GetNodePolyData(Nodes);
        dynamic_cast<vtkContourOverlayRepresentation*>(d->copyRoi->getContour()->GetContourRepresentation())->Initialize(Nodes);
        d->copyRoi->getContour()->SetWidgetState(getContour()->GetWidgetState());
        d->copyRoi->forceInvisibilityOn();
        return true;
    }
    else
    {
        return false;
    }
}

medAbstractRoi * polygonRoi::getCopy(medAbstractView *view)
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(view->backend())->view2D;
    d->copyRoi->setOrientation(view2d->GetViewOrientation());
    d->copyRoi->setIdSlice(view2d->GetSlice());
    d->copyRoi->forceInvisibilityOff();
    polygonRoi *copyRoi = d->copyRoi;
    copyROI(view);
    dynamic_cast<vtkContourOverlayRepresentation*>(copyRoi->getContour()->GetContourRepresentation())->SaveState();
    return copyRoi;
}

bool polygonRoi::isVisible()
{
    return (d->contour->GetEnabled());
}

QList<medAbstractRoi*> * polygonRoi::interpolate(medAbstractRoi *roi)
{
    return nullptr;
}

void polygonRoi::setColor(double newColor[])
{
    vtkContourOverlayRepresentation *contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(d->contour->GetContourRepresentation());
    contourRep->GetLinesProperty()->SetColor(newColor);
}

void polygonRoi::setRightColor()
{
    if(isMasterRoi())
    {
        setColor(MASTER_ROI_COLOR);
    }
    else
    {
        setColor(ROI_COLOR);
    }
}
