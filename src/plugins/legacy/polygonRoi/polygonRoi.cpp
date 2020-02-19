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
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCylinderSource.h>
#include <vtkCleanPolyData.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkWidgetEvent.h>
#include <vtkWidgetCallbackMapper.h>

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
    
protected:
    PolygonRoiObserver();
    ~PolygonRoiObserver();

private:
    polygonRoi * roi;
};

PolygonRoiObserver::PolygonRoiObserver()
{
}

PolygonRoiObserver::~PolygonRoiObserver(){}

void PolygonRoiObserver::Execute ( vtkObject *caller, unsigned long event, void *callData )
{
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
            emit roi->updateCursorState(CURSORSTATE::MOUSE_EVENT);
            break;
        }
        case vtkCommand::EndInteractionEvent:
        {
            roi->setMasterRoi(true);
            roi->saveState();
            emit roi->updateCursorState(CURSORSTATE::MOUSE_EVENT);
            emit roi->interpolate();
            break;
        }
        case vtkCommand::PlacePointEvent:
        {
            roi->saveState();
            break;
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
    vtkPolyData* polyData;
    vtkProperty* property;
    vtkImageView2D *view;
    PolygonRoiObserver *observer;
    polygonRoi *copyRoi;
    QList<QPair<medAbstractImageView*, medImageView::Orientation>> alternativeViews;
    QColor roiColor;
};

polygonRoi::polygonRoi(vtkImageView2D *view, QColor color, medAbstractRoi *parent )
    : medAbstractRoi(parent)
    , d(new polygonRoiPrivate)
{
    vtkSmartPointer<vtkContourOverlayRepresentation> contourRep = vtkSmartPointer<vtkContourOverlayRepresentation>::New();
    contourRep->GetLinesProperty()->SetLineWidth(4);
    contourRep->GetProperty()->SetPointSize(5);
    contourRep->GetProperty()->SetColor(1.0, 0.0, 0.0);
    contourRep->GetActiveProperty()->SetOpacity(0);
    contourRep->SetPixelTolerance(20);
    d->contour = vtkContourWidget::New();
    d->contour->SetRepresentation(contourRep);

    contourRep->SetRenderer(view->GetRenderer());
    d->contour->SetInteractor(view->GetInteractor());
    // deactivate the default RightButtonPressEvent -> AddFinalPointAction
    d->contour->GetEventTranslator()->SetTranslation(vtkCommand::RightButtonPressEvent,0);
    d->contour->On();
    d->view = view;

    setOrientation(view->GetViewOrientation());
    setIdSlice(view->GetSlice());
    d->observer = PolygonRoiObserver::New();
    d->observer->setRoi(this);
    d->view->AddObserver(vtkImageView2D::SliceChangedEvent,d->observer,0);
    d->contour->AddObserver(vtkCommand::EndInteractionEvent,d->observer,10);
    contourRep->AddObserver(vtkCommand::PlacePointEvent,d->observer,0);
    d->copyRoi = nullptr;

    d->roiColor = color;
    setMasterRoi(true);

}

polygonRoi::~polygonRoi()
{
    delete d;
    d = nullptr;
}

bool polygonRoi::isClosed()
{
    return d->contour->GetContourRepresentation()->GetClosedLoop();
}

void polygonRoi::setEnabled(bool state)
{
    d->contour->SetEnabled(state);
}

QPair<vtkPolyData*, vtkProperty*> polygonRoi::getPoly()
{
    vtkContourRepresentation* contourRep = d->contour->GetContourRepresentation();
    d->polyData = contourRep->GetContourRepresentationAsPolyData();
    vtkPolygon *polygon = vtkPolygon::New();
    vtkPoints *points = vtkPoints::New();
    std::vector<vtkIdType> vec_ids;
    double pos[3];
    for(int j=0; j<contourRep->GetNumberOfNodes(); j++)
    {
        contourRep->GetNthNodeWorldPosition(j, pos);

        vec_ids.push_back(points->InsertNextPoint(pos));
        for ( int k = 0; k<contourRep->GetNumberOfIntermediatePoints(j); k++ )
        {
            contourRep->GetIntermediatePointWorldPosition(j, k, pos);
            vec_ids.push_back(points->InsertNextPoint(pos));
        }

    }
    polygon->Initialize(points->GetNumberOfPoints(), &vec_ids[0], points);
    vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
    cells->InsertNextCell(polygon);
    d->polyData->SetPolys(cells);

    d->property = vtkProperty::New();
    double color[3];
    color[0] = d->roiColor.redF();
    color[1] = d->roiColor.greenF();
    color[2] = d->roiColor.blueF();
    d->property->SetColor(color);
    if(isMasterRoi())
    {
        d->property->SetOpacity(1.);
    }
    else
    {
        d->property->SetOpacity(0.2);
    }
    d->property->SetLighting(false);
    d->property->SetLineWidth(1.);
    d->property->SetPointSize(1.);
    d->property->SetInterpolationToFlat();
    d->property->SetVertexColor(0., 0., 1.);
    return QPair<vtkPolyData*, vtkProperty*>(d->polyData, d->property);
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
    d->contour->Off();
}

void polygonRoi::On()
{
    d->contour->On();
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
    //medRoiManager::instance()->addToUndoRedoStack(command);
    emit stateSaved();
}

bool polygonRoi::copyROI(medAbstractView *view)
{
    if (getContour()->GetWidgetState()==2)
    {
//        vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(view->backend())->view2D;
//        d->copyRoi = new polygonRoi(view2d);
//        vtkSmartPointer<vtkPolyData> Nodes = vtkSmartPointer<vtkPolyData>::New();
//        d->contour->GetContourRepresentation()->GetNodePolyData(Nodes);
//        dynamic_cast<vtkContourOverlayRepresentation*>(d->copyRoi->getContour()->GetContourRepresentation())->Initialize(Nodes);
//        d->copyRoi->getContour()->SetWidgetState(getContour()->GetWidgetState());
//        d->copyRoi->forceInvisibilityOn();
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
    double color[3];
    color[0] = d->roiColor.redF();
    color[1] = d->roiColor.greenF();
    color[2] = d->roiColor.blueF();
    vtkContourOverlayRepresentation *contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(d->contour->GetContourRepresentation());
    contourRep->GetLinesProperty()->SetColor(color);
    if(isMasterRoi())
    {
        contourRep->GetLinesProperty()->SetOpacity(1.);
    }
    else
    {
        contourRep->GetLinesProperty()->SetOpacity(0.2);
    }
}

void polygonRoi::addViewToList(medAbstractImageView *viewToAdd, medImageView::Orientation orientation)
{
    d->alternativeViews.append(QPair<medAbstractImageView*, medImageView::Orientation>(viewToAdd, orientation));
}

void polygonRoi::addDataSet()
{
    qDebug()<<"number of alternative views "<<d->alternativeViews.size();
    for (QPair<medAbstractImageView*, medImageView::Orientation> view :  d->alternativeViews)
    {
        qDebug()<<"pointer on viewx is "<<view.first;
        if ( view.second == medImageView::VIEW_ALL_ORIENTATION )
            continue;

        view.first->setOrientation(view.second);
        QPair<vtkPolyData*, vtkProperty*> poly = getPoly();

        if (view.second == medImageView::VIEW_ORIENTATION_3D)
        {
            vtkImageView3D* view3D = static_cast<medVtkViewBackend*>(view.first->backend())->view3D;
            view3D->AddDataSet(poly.first, poly.second);

        }
        else
        {
            vtkImageView2D* view2D = static_cast<medVtkViewBackend*>(view.first->backend())->view2D;

            view2D->RemoveDataSet(poly.first);
            view2D->AddDataSet(poly.first, poly.second);
        }
    }
}
