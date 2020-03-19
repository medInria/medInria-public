/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <polygonRoi.h>

// medInria
#include <medTagRoiManager.h>
#include <medVtkViewBackend.h>

// vtk
#include <vtkCellLocator.h>
#include <vtkContourOverlayRepresentation.h>
#include <vtkContourWidget.h>
#include <vtkImageView2D.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkWidgetEvent.h>

// Qt
#include <QVector3D>

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
        {
            roi->manageVisibility();
            emit roi->updateCursorState(CURSORSTATE::CS_SLICE_CHANGED);
            emit roi->toggleRepulsorButton(false);
            break;
        }
        case vtkCommand::EndInteractionEvent:
        {
            qDebug()<<"end interaction event";
            roi->setMasterRoi(true);
            emit roi->updateCursorState(CURSORSTATE::CS_MOUSE_EVENT);
            emit roi->interpolate();
            emit roi->updateRoiInAlternativeViews();
            break;
        }
        case vtkCommand::MouseMoveEvent:
        {
            qDebug()<<"mouse move event";
            roi->setMasterRoi(true);
            emit roi->updateCursorState(CURSORSTATE::CS_MOUSE_EVENT);
            emit roi->interpolate();
            emit roi->updateRoiInAlternativeViews();
            break;
        }
        default:
        {
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
    vtkSmartPointer<vtkPolyData> polyData;
    vtkSmartPointer<vtkProperty> property;
    vtkImageView2D *view;
    PolygonRoiObserver *observer;
    polygonRoi *copyRoi;
    QList<medAbstractImageView*> alternativeViews;
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
    d->contour->AddObserver(vtkCommand::MouseMoveEvent,d->observer,10);
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

vtkPolyData *polygonRoi::createPolyDataFromContour()
{
    vtkContourRepresentation* contourRep = d->contour->GetContourRepresentation();
    vtkSmartPointer<vtkPolygon> polygon = vtkSmartPointer<vtkPolygon>::New();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
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
    vtkPolyData *pd = contourRep->GetContourRepresentationAsPolyData();
    pd->SetPolys(cells);

    return pd;
}

vtkProperty *polygonRoi::createPropertyForPolyData()
{
    vtkProperty* property = vtkProperty::New();
    double color[3];
    color[0] = d->roiColor.redF();
    color[1] = d->roiColor.greenF();
    color[2] = d->roiColor.blueF();
    property->SetColor(color);
    if(isMasterRoi())
    {
        property->SetOpacity(1.);
    }
    else
    {
        property->SetOpacity(0.2);
    }
    property->SetLighting(false);
    property->SetLineWidth(1.);
    property->SetPointSize(1.);
    property->SetInterpolationToFlat();
    property->SetVertexColor(0., 0., 1.);
    //property->SetEdgeVisibility(true);
    return property;
}

vtkPolyData* polygonRoi::getPolyData()
{
    if (!d->polyData)
        d->polyData = createPolyDataFromContour();
    return d->polyData;
}

void polygonRoi::loadNodes(QVector<QVector3D> coordinates)
{

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    int numPts = coordinates.size();
    for (int i = 0; i < numPts; i++)
    {
        points->InsertPoint(static_cast<vtkIdType>(i), coordinates[i].x(), coordinates[i].y(), coordinates[i].z());
    }

    // Create a cell array to connect the points into meaningful geometry
    vtkIdType* vertexIndices = new vtkIdType[numPts];
    for (int i = 0; i < numPts; i++) { vertexIndices[i] = static_cast<vtkIdType>(i); }
    // Set the last vertex to 0; this means the last line segment will join the 19th point (vertices[19])
    // with the first one (vertices[0]), thus closing the circle.
//    vertexIndices[numPts] = 0;
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    lines->InsertNextCell(numPts, vertexIndices);

    // Create polydata to hold the geometry just created, and populate it
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);
    polydata->SetLines(lines);
    d->contour->Initialize(polydata);
    d->contour->GetContourRepresentation()->SetClosedLoop(1);

    emit updateCursorState(CURSORSTATE::CS_MOUSE_EVENT);
    emit interpolate();
    emit updateRoiInAlternativeViews();

}

void polygonRoi::createPolydataToAddInViews()
{
    d->polyData = createPolyDataFromContour();
    d->property = createPropertyForPolyData();
    return;
}

vtkContourWidget * polygonRoi::getContour()
{
    return d->contour;
}

medContourNodes polygonRoi::getContourAsNodes()
{
    vtkContourRepresentation * contourRep = d->contour->GetContourRepresentation();
    QVector<QVector3D> coordinates;
    for (int node = 0; node < contourRep->GetNumberOfNodes(); node++)
    {
        double pos[3];
        contourRep->GetNthNodeWorldPosition(node, pos);
        coordinates.push_back(QVector3D(pos[0],pos[1],pos[2]));
    }
    medContourNodes contourNodes = medContourNodes(getIdSlice(), getOrientation(), coordinates);
    return contourNodes;
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

void polygonRoi::manageTick(medSliderL *slider)
{
    if (d->view->GetViewOrientation() != getOrientation())
    {
        slider->removeTick(getIdSlice());
    }
    else
    {
        slider->addTick(getIdSlice());
    }
    slider->update();
    return;
}

void polygonRoi::manageVisibility()
{
    vtkContourOverlayRepresentation *contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(d->contour->GetContourRepresentation());
    contourRep->GetProperty()->SetPointSize(6);
    if (d->view->GetViewOrientation() != getOrientation())
    {
        contourRep->GetProperty()->SetPointSize(1);
        On();
    }
    else
    {
        if (d->view->GetSlice() == getIdSlice())
            On();
        else
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
    double selectedColor[3] = {1.0, 0.533, 0.2};
    vtkContourOverlayRepresentation *contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(d->contour->GetContourRepresentation());
    contourRep->GetLinesProperty()->SetColor(selectedColor);
    medAbstractRoi::select();
}

void polygonRoi::unselect()
{
    setRightColor();
    medAbstractRoi::unselect();
}

bool polygonRoi::isVisible()
{
    return (d->contour->GetEnabled());
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

void polygonRoi::addViewToList(medAbstractImageView *viewToAdd)
{
    d->alternativeViews.append(viewToAdd);
}

void polygonRoi::addContourInAlternativeView(medAbstractImageView *view)
{
    if ( view->orientation() == medImageView::VIEW_ALL_ORIENTATION )
        return;

    createPolydataToAddInViews();
    if (view->orientation() == medImageView::VIEW_ORIENTATION_3D)
    {
        vtkImageView3D* view3D = static_cast<medVtkViewBackend*>(view->backend())->view3D;
        view3D->RemoveDataSet(d->polyData);
        view3D->AddDataSet(d->polyData, d->property);
    }
    else
    {
        vtkImageView2D* view2D = static_cast<medVtkViewBackend*>(view->backend())->view2D;
        view2D->RemoveDataSet(d->polyData);
        view2D->AddDataSet(d->polyData, d->property);
    }

    return;
}

void polygonRoi::removeContourInAlternativeView(medAbstractImageView *view)
{
    if ( view->orientation() == medImageView::VIEW_ALL_ORIENTATION )
        return;

    createPolydataToAddInViews();
    if (view->orientation() == medImageView::VIEW_ORIENTATION_3D)
    {
        vtkImageView3D* view3D = static_cast<medVtkViewBackend*>(view->backend())->view3D;
        view3D->RemoveDataSet(d->polyData);
    }
    else
    {
        vtkImageView2D* view2D = static_cast<medVtkViewBackend*>(view->backend())->view2D;
        view2D->RemoveDataSet(d->polyData);
    }

    return;
}

double polygonRoi::findClosestContourFromPoint(QVector3D worldMouseCoord)
{
    vtkContourRepresentation* contourRep = d->contour->GetContourRepresentation();
    double coords[3] = {worldMouseCoord.x(), worldMouseCoord.y(), worldMouseCoord.z()};
    vtkSmartPointer<vtkCellLocator> cellLocator =
        vtkSmartPointer<vtkCellLocator>::New();
    cellLocator->SetDataSet(d->polyData);
    cellLocator->BuildLocator();

    //Find the closest points to TestPoint
    double closestPoint[3];//the coordinates of the closest point will be returned here
    double closestPointDist2; //the squared distance to the closest point will be returned here
    vtkIdType cellId; //the cell id of the cell containing the closest point will be returned here
    int subId; //this is rarely used (in triangle strips only, I believe)
    cellLocator->FindClosestPoint(coords, closestPoint, cellId, subId, closestPointDist2);
    return closestPointDist2;
}

