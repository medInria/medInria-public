/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "polygonRoi.h"

// medInria
#include <polygonLabel.h>
#include <medVtkViewBackend.h>

// vtk
#include <vtkCellLocator.h>
#include <viewinteractors/vtkContourOverlayRepresentation.h>
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

PolygonRoiObserver::PolygonRoiObserver(){}

PolygonRoiObserver::~PolygonRoiObserver(){}

void PolygonRoiObserver::Execute ( vtkObject *caller, unsigned long event, void *callData )
{
    if (!this->roi->getView())
    {
        return;
    }

    switch ( event )
    {
        case vtkCommand::EndInteractionEvent:
        {
            roi->setMasterRoi(true);
            emit roi->contourFinished(CURSORSTATE::CS_DEFAULT);
            break;
        }
        case vtkImageView2D::SliceChangedEvent:
        {
            roi->manageVisibility();
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
        for (medAbstractImageView *view : alternativeViews)
        {
            vtkImageView3D *view3D = static_cast<medVtkViewBackend *>(view->backend())->view3D;
            view3D->RemoveDataSet(polyData);
            vtkImageView2D *view2D = static_cast<medVtkViewBackend *>(view->backend())->view2D;
            view2D->RemoveDataSet(polyData);
        }
        alternativeViews.clear();
        contour->RemoveAllObservers();
        contour->GetContourRepresentation()->RemoveAllObservers();
        view->RemoveObserver(observer);
        view = nullptr;
    }

    vtkSmartPointer<vtkContourWidget> contour;
    vtkSmartPointer<vtkPolyData> polyData;
    vtkSmartPointer<vtkProperty> property;
    vtkImageView2D *view;
    vtkSmartPointer<PolygonRoiObserver> observer;
    QList<medAbstractImageView*> alternativeViews;
    QColor roiColor;
};

polygonRoi::polygonRoi(vtkImageView2D *view, QColor color, medAbstractRoi *parent )
    : medAbstractRoi(parent)
    , d(new polygonRoiPrivate)
{
    vtkSmartPointer<vtkContourOverlayRepresentation> contourRep = vtkSmartPointer<vtkContourOverlayRepresentation>::New();
    contourRep->GetLinesProperty()->SetLineWidth(4);
    contourRep->GetProperty()->SetPointSize(8);
    contourRep->GetActiveProperty()->SetOpacity(0);
    contourRep->SetPixelTolerance(20);
    d->contour = vtkSmartPointer<vtkContourWidget>::New();
    d->contour->SetRepresentation(contourRep);

    contourRep->SetRenderer(view->GetRenderer());
    d->contour->SetInteractor(view->GetInteractor());
    // deactivate the default RightButtonPressEvent -> AddFinalPointAction
    d->contour->GetEventTranslator()->SetTranslation(vtkCommand::RightButtonPressEvent,vtkWidgetEvent::NoEvent);
    d->contour->GetEventTranslator()->SetTranslation(vtkCommand::KeyPressEvent, vtkWidgetEvent::NoEvent);
    d->contour->GetEventTranslator()->SetTranslation(vtkCommand::KeyReleaseEvent, vtkWidgetEvent::NoEvent);
    d->contour->On();
    d->view = view;

    setOrientation(view->GetViewOrientation());
    setIdSlice(view->GetSlice());
    d->observer = vtkSmartPointer<PolygonRoiObserver>::New();
    d->observer->setRoi(this);

    d->contour->AddObserver(vtkCommand::EndInteractionEvent,d->observer,10);

    d->view->AddObserver(vtkImageView2D::SliceChangedEvent, d->observer, 0);

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
        property->SetOpacity(0.3);
    }
    property->SetLighting(false);
    property->SetLineWidth(1.);
    property->SetPointSize(1.);
    property->SetInterpolationToFlat();
    property->SetVertexColor(0., 0., 1.);
    return property;
}

vtkPolyData* polygonRoi::getPolyData()
{
    if (!d->polyData)
    {
        d->polyData = createPolyDataFromContour();
    }
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
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    lines->InsertNextCell(numPts, vertexIndices);

    // Create polydata to hold the geometry just created, and populate it
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);
    polydata->SetLines(lines);
    d->contour->Initialize(polydata);
    d->contour->GetContourRepresentation()->SetClosedLoop(1);

    emit contourFinished(CURSORSTATE::CS_DEFAULT);
}

vtkContourWidget * polygonRoi::getContour()
{
    return d->contour;
}

medWorldPosContours polygonRoi::getContourAsNodes()
{
    vtkContourRepresentation * contourRep = d->contour->GetContourRepresentation();
    QVector<QVector3D> coordinates;
    for (int node = 0; node < contourRep->GetNumberOfNodes(); node++)
    {
        double pos[3];
        contourRep->GetNthNodeWorldPosition(node, pos);
        coordinates.push_back(QVector3D(pos[0],pos[1],pos[2]));
    }
    medWorldPosContours contourNodes = medWorldPosContours(getIdSlice(), getOrientation(), coordinates);
    return contourNodes;
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
        Off();
    }
    else
    {
        if (isInCurrentSlice())
        {
            On();
        }
        else
        {
            Off();
        }
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

int polygonRoi::getNumberOfNodes()
{
    vtkContourRepresentation * contourRep = d->contour->GetContourRepresentation();
    return contourRep->GetNumberOfNodes();
}

void polygonRoi::select()
{
    auto contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(d->contour->GetContourRepresentation());
    contourRep->GetLinesProperty()->SetLineWidth(8.);
    medAbstractRoi::select();
}

void polygonRoi::unselect()
{
    auto contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(d->contour->GetContourRepresentation());
    contourRep->GetLinesProperty()->SetLineWidth(4.);
    medAbstractRoi::unselect();
}

bool polygonRoi::isVisible()
{
    return (d->contour->GetEnabled());
}

void polygonRoi::activateContour(bool state)
{
    auto contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(d->contour->GetContourRepresentation());
    if (state)
    {
        d->contour->GetEventTranslator()->SetTranslation(vtkCommand::LeftButtonPressEvent, vtkWidgetEvent::Select);
        d->contour->GetEventTranslator()->SetTranslation(vtkCommand::MouseMoveEvent, vtkWidgetEvent::Move);
        setRightColor();
    }
    else
    {
        d->contour->GetEventTranslator()->SetTranslation(vtkCommand::LeftButtonPressEvent, vtkWidgetEvent::NoEvent);
        d->contour->GetEventTranslator()->SetTranslation(vtkCommand::MouseMoveEvent, vtkWidgetEvent::NoEvent);
        double color[3];
        color[0] = d->roiColor.redF();
        color[1] = d->roiColor.greenF();
        color[2] = d->roiColor.blueF();
        contourRep->GetLinesProperty()->SetColor(color);
        contourRep->GetProperty()->SetColor(color);
        contourRep->GetLinesProperty()->SetOpacity(0.4);
        contourRep->GetProperty()->SetOpacity(0.4);
    }
}

void polygonRoi::setRightColor()
{
    double color[3];
    color[0] = d->roiColor.redF();
    color[1] = d->roiColor.greenF();
    color[2] = d->roiColor.blueF();
    auto contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(d->contour->GetContourRepresentation());
    contourRep->GetLinesProperty()->SetColor(color);
    contourRep->GetProperty()->SetColor(1., 0., 0.);

    if(isMasterRoi())
    {
        contourRep->GetLinesProperty()->SetOpacity(1.);
        contourRep->GetProperty()->SetOpacity(1.);
    }
    else
    {
        contourRep->GetLinesProperty()->SetOpacity(0.2);
        contourRep->GetProperty()->SetOpacity(0.2);
    }
}

void polygonRoi::updateColor(QColor &color, bool activate)
{
    d->roiColor = color;
    activateContour(activate);
}

QColor polygonRoi::getColor()
{
    return d->roiColor;
}

void polygonRoi::updateContourOtherView(medAbstractImageView *view, bool state)
{
    if ( view->orientation() == medImageView::VIEW_ALL_ORIENTATION )
    {
        return;
    }

    if (state)
    {
        if (!d->alternativeViews.contains(view))
        {
            d->alternativeViews.append(view);
            if (!d->polyData)
            {
                d->polyData = createPolyDataFromContour();
            }
            if (!d->property)
            {
                d->property = createPropertyForPolyData();
            }
            double color[3];
            color[0] = d->roiColor.redF();
            color[1] = d->roiColor.greenF();
            color[2] = d->roiColor.blueF();
            d->property->SetColor(color);

            if (view->orientation() == medImageView::VIEW_ORIENTATION_3D)
            {
                vtkImageView3D *view3D = static_cast<medVtkViewBackend *>(view->backend())->view3D;
                view3D->RemoveDataSet(d->polyData);
                view3D->AddDataSet(d->polyData, d->property);
            }
            else
            {
                vtkImageView2D *view2D = static_cast<medVtkViewBackend *>(view->backend())->view2D;
                view2D->RemoveDataSet(d->polyData);
                view2D->AddDataSet(d->polyData, d->property);
            }
        }
    }
    else
    {
        if (d->alternativeViews.contains(view))
        {
            d->alternativeViews.removeOne(view);
            if (!d->polyData)
            {
                return;
            }
            vtkImageView3D *view3D = static_cast<medVtkViewBackend *>(view->backend())->view3D;
            view3D->RemoveDataSet(d->polyData);
            vtkImageView2D *view2D = static_cast<medVtkViewBackend *>(view->backend())->view2D;
            view2D->RemoveDataSet(d->polyData);
        }
    }
}

QVector<QVector2D> polygonRoi::copyContour()
{
    QVector<QVector2D> coordinates;
    if ( d->contour->GetWidgetState() != 2 )
    {
        return coordinates;
    }

    vtkContourRepresentation * contourRep = d->contour->GetContourRepresentation();
    for (int node = 0; node < contourRep->GetNumberOfNodes(); node++)
    {
        double pos[2];
        contourRep->GetNthNodeDisplayPosition(node, pos);
        coordinates.push_back(QVector2D(pos[0],pos[1]));
    }
    return coordinates;
}

bool polygonRoi::pasteContour(QVector<QVector2D> &nodes)
{
    auto contourRep = static_cast<vtkContourOverlayRepresentation*>(d->contour->GetContourRepresentation());
    for (QVector2D node : nodes)
    {
        double pos[2];
        pos[0] = node.x();
        pos[1] = node.y();
        d->contour->GetContourRepresentation()->AddNodeAtDisplayPosition(pos);
    }
    contourRep->SetClosedLoop(1);
    d->contour->On();
    d->contour->SetWidgetState(2);

    emit contourFinished(CURSORSTATE::CS_DEFAULT);
    return true;
}

bool polygonRoi::isInCurrentSlice()
{
    return (static_cast<unsigned int>(d->view->GetSlice()) == getIdSlice());
}

void polygonRoi::setCurrentSlice()
{
    setIdSlice(d->view->GetSlice());
}

void polygonRoi::removeObservers()
{
    d->contour->RemoveAllObservers();
    d->view->RemoveObserver(d->observer);
}

void polygonRoi::addObservers()
{
    d->contour->AddObserver(vtkCommand::EndInteractionEvent,d->observer,10);
    d->view->AddObserver(vtkImageView2D::SliceChangedEvent, d->observer, 0);
}


