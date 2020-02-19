/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "vtkContourOverlayRepresentation.h"

#include <medRoiManager.h>
#include <polygonRoiToolBox.h>
#include <vtkContourRepresentation.h>

#include <QTreeWidget>
#include <medVtkViewBackend.h>
#include <medToolBox.h>
#include <vtkPolyData.h>
#include <vtkPolyLine.h>
#include <vtkParametricSpline.h>
#include <polygonEventFilter.h>
#include <medIntParameterL.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

class medLabelManagerPrivate
{
public:
    medLabelManagerPrivate(){}
    medLabelManagerPrivate(medAbstractView *view, polygonEventFilter *eventCursor, QColor color);

    QList<polygonRoi*> rois;
    int orientation;
    medAbstractView *view;
    polygonEventFilter *eventCursor;
    QColor color;
    bool enableInterpolation;
};

medLabelManagerPrivate::medLabelManagerPrivate(medAbstractView *view, polygonEventFilter *eventCursor, QColor color)
    :eventCursor(eventCursor), enableInterpolation(true)
{

    this->view = view;
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(view->backend())->view2D;

    this->orientation = view2d->GetViewOrientation();
    this->color = color;

    polygonRoi *roi = new polygonRoi(view2d, color);
    rois.append(roi);
}


medLabelManager::medLabelManager(medAbstractView *view, polygonEventFilter *eventCursor, QColor color):
    d(new medLabelManagerPrivate(view, eventCursor, color))
{
    connectRois();
}

void medLabelManager::appendRoi()
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    polygonRoi *roi = new polygonRoi(view2d, d->color);
    d->rois.append(roi);
    connectRois();
}

void medLabelManager::appendRoi(polygonRoi *roi)
{
    d->rois.append(roi);
}

QColor medLabelManager::getColor()
{
    return d->color;
}

QList<polygonRoi *> medLabelManager::getRois()
{
    return d->rois;
}

void medLabelManager::setContourEnabled(bool state)
{
    for (polygonRoi *roi : d->rois)
    {
        roi->setEnabled(state);
        if (state)
            roi->On();
        else
            roi->Off();
    }
}

void medLabelManager::setEnableInterpolation(bool state)
{
    d->enableInterpolation = state;
    if (state)
        interpolateIfNeeded();
}

polygonRoi *medLabelManager::roiOpenInSlice()
{
    polygonRoi *roi = existingRoiInSlice();
    if (roi && roi->getContour()->GetContourRepresentation()->GetClosedLoop()==false)
    {
        return roi;
    }
    return nullptr;
}

bool medLabelManager::roiClosedInSlice()
{
    polygonRoi *roi = existingRoiInSlice();
    if (roi)
    {
        return roi->getContour()->GetContourRepresentation()->GetClosedLoop();
    }
    return false;
}

void medLabelManager::manageTick()
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;

    medIntParameterL *slicingParameter = nullptr;
    foreach (medAbstractInteractor *interactor, qobject_cast<medAbstractLayeredView*>(d->view)->layerInteractors(0))
    {
        if ((interactor->identifier() == "medVtkViewItkDataImageInteractor") ||
                (interactor->identifier() == "medVtkViewItkDataImage4DInteractor"))
        {
            foreach (medAbstractParameterL *parameter, interactor->linkableParameters())
            {
                if (parameter->name() == "Slicing")
                {
                    slicingParameter = qobject_cast<medIntParameterL*>(parameter);
                    break;
                }
            }
            break;
        }
    }
    for (polygonRoi *roi : d->rois)
    {
        if (view2d->GetViewOrientation() != roi->getOrientation())
        {
            slicingParameter->getSlider()->removeTick(roi->getIdSlice());
        }
        else
        {
            slicingParameter->getSlider()->addTick(roi->getIdSlice());
        }
        slicingParameter->getSlider()->update();
    }
    d->view->render();
    return;
}

polygonRoi *medLabelManager::existingRoiInSlice()
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    int slice = view2d->GetSlice();

    if (!isSameOrientation(view2d->GetViewOrientation()))
        return nullptr;

    for (polygonRoi* roi : d->rois)
    {
        if (slice==roi->getIdSlice())
            return roi;
    }
    return nullptr;

}

bool medLabelManager::isSameOrientation(int orientation)
{
    return (d->orientation==orientation);
}

bool medLabelManager::sortRois(const polygonRoi *p1, const polygonRoi *p2)
{
   return p1->getIdSlice() < p2->getIdSlice();
}

void medLabelManager::connectRois()
{
    for (polygonRoi *roi: d->rois)
    {
        connect(roi, SIGNAL(updateCursorState(CURSORSTATE)), d->eventCursor, SLOT(setCursorState(CURSORSTATE)), Qt::UniqueConnection);
        connect(roi, SIGNAL(interpolate()), this, SLOT(interpolateIfNeeded()), Qt::UniqueConnection);
    }
}

void medLabelManager::manageVisibility()
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;

    for (polygonRoi *roi : d->rois)
    {
        vtkContourOverlayRepresentation *contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(roi->getContour()->GetContourRepresentation());
        contourRep->GetProperty()->SetPointSize(6);
        if (view2d->GetViewOrientation() != d->orientation)
        {
            contourRep->GetProperty()->SetPointSize(1);
            roi->On();
        }
        else
        {
            if (view2d->GetSlice() == roi->getIdSlice())
                roi->On();
            else
                roi->Off();
        }
    }
    d->view->render();
}

void medLabelManager::updateAlternativeViews(medAbstractImageView* v, medTableWidgetItem *item)
{
    for (polygonRoi *roi: d->rois)
    {
        roi->addViewToList(v, item->orientation());
        roi->addDataSet();
    }
    v->render();
}

bool medLabelManager::mouseIsCloseFromContour(double mousePos[2])
{
    polygonRoi *roi = existingRoiInSlice();
    if (roi)
    {
        for (int i = 0; i< roi->getContour()->GetContourRepresentation()->GetNumberOfNodes(); i++)
        {
            double contourPos[2];
            roi->getContour()->GetContourRepresentation()->GetNthNodeDisplayPosition(i, contourPos);
            if ( getDistance(mousePos, contourPos) <= 10. )
            {
                return true;
            }
        }
    }
    return false;
}

double medLabelManager::getMinimumDistanceFromNodesToEventPosition(double eventPos[2])
{
    polygonRoi *roi = existingRoiInSlice();
    double minDist = DBL_MAX;
    double dist = 0.;
    double contourPos[2];
    if (roi)
    {
        for (int i = 0; i< roi->getContour()->GetContourRepresentation()->GetNumberOfNodes(); i++)
        {
            roi->getContour()->GetContourRepresentation()->GetNthNodeDisplayPosition(i, contourPos);
            dist = getDistance(eventPos, contourPos);
            if ( dist < minDist )
                minDist = dist;
        }
    }
    return minDist;

}

double medLabelManager::getMinimumDistanceFromIntermediateNodesToEventPosition(double eventPos[2])
{
    polygonRoi *roi = existingRoiInSlice();
    double minDist = DBL_MAX;
    double contourPos[2];
    double dist = 0.;
    if (roi)
    {
        vtkContourOverlayRepresentation *contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(roi->getContour()->GetContourRepresentation());
        for(int j=0; j<contourRep->GetNumberOfNodes(); j++)
        {
            contourRep->GetNthNodeDisplayPosition(j, contourPos);
            dist = getDistance(eventPos, contourPos);
            if ( dist < minDist )
                minDist = dist;
            for ( int k = 0; k<contourRep->GetNumberOfIntermediatePoints(j); k++ )
            {
                contourRep->GetIntermediatePointDisplayPosition(j, k, contourPos);
                dist = getDistance(eventPos, contourPos);
                if ( dist < minDist )
                    minDist = dist;
            }

        }

    }
    return minDist;
}

double medLabelManager::getDistance(double mousePos[2], double contourPos[2])
{
//    qDebug()<<"ratio : "<<QGuiApplication::primaryScreen()->devicePixelRatio();
//    qDebug()<<"mouse = x : "<<mouseX<<" y : "<<mouseY;
//    qDebug()<<"node  = x : "<<contourPos[0]<<" y : "<<contourPos[1];
//    qDebug()<<"distance  : "<<sqrt(pow(mouseX - contourPos[0], 2) + pow(mouseY - contourPos[1], 2));
    return sqrt(pow(mousePos[0] - contourPos[0], 2) + pow(mousePos[1] - contourPos[1], 2));
}

void medLabelManager::interpolateIfNeeded()
{
    if (!d->enableInterpolation)
        return;

    if (d->rois.size() < 2)
        return;

    // TODO : Remove. Find a way to ensure all contour are closed (add pixel tolerance or force close on sliceChangedEvent)
    for (polygonRoi *roi : d->rois)
    {
        if ( roi->getContour()->GetContourRepresentation()->GetClosedLoop() == false )
        {
            //displayMessageError("Non-closed polygon at slice: " + QString::number(roi->getIdSlice()+1) + ". Operation aborted");
            return;
        }
    }

    QList<polygonRoi *> masterRois;
    QList<polygonRoi *>::Iterator it = d->rois.begin();
    while( it != d->rois.end())
    {
        polygonRoi *roi = *it;
        if ( roi->isMasterRoi() )
        {
            masterRois.append(roi);
            ++it;
        }
        else
        {
            it = d->rois.erase(it);
            delete roi;
        }
    }

    std::sort(d->rois.begin(), d->rois.end(), medLabelManager::sortRois);

    QList<polygonRoi *> listOfRois;
    for (int i=0; i<d->rois.size()-1; i++)
    {
        polygonRoi *firstRoi = d->rois.at(i);
        polygonRoi *secondRoi = d->rois.at(i+1);
        listOfRois.append(interpolateBetween2Slices(firstRoi, secondRoi));
    }
    d->rois.append(listOfRois);
    connectRois();
    manageVisibility();
    manageTick();
}

QList<polygonRoi *> medLabelManager::interpolateBetween2Slices(polygonRoi *firstRoi, polygonRoi *secondRoi)
{
    QList<polygonRoi *> outputRois;
    vtkContourRepresentation* contour;
    // Contour first ROI
    contour = firstRoi->getContour()->GetContourRepresentation();
    vtkSmartPointer<vtkPolyData> curveMin = contour->GetContourRepresentationAsPolyData();
    int curveMinNbNode = contour->GetNumberOfNodes();
    int minSlice = firstRoi->getIdSlice();

    // Contour second ROI
    contour = secondRoi->getContour()->GetContourRepresentation();
    vtkSmartPointer<vtkPolyData> curveMax = contour->GetContourRepresentationAsPolyData();

    int curveMaxNbNode = contour->GetNumberOfNodes();
    int maxSlice = secondRoi->getIdSlice();

    // Compute intermediate ROIs between two successive ROIs
    QList<vtkPolyData *> listPolyData = generateIntermediateCurves(curveMax,curveMin,maxSlice-minSlice-1);
    double number = ceil(1.8 * (double)(curveMinNbNode));
    if (curveMaxNbNode > curveMinNbNode)
    {
        number = ceil(1.8 * (double)(curveMaxNbNode));
    }

    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;

    for (int i = minSlice+1;i<maxSlice;i++)
    {
        polygonRoi *polyRoi = new polygonRoi(view2d, d->color);
        vtkContourWidget *contour = polyRoi->getContour();
        contour->SetEnabled(true);

        int nbPointsPoly = listPolyData[i-(minSlice+1)]->GetNumberOfPoints();
        int pointToRemove = 0;
        contour->Initialize(listPolyData[i-(minSlice+1)]);
        for (int k = 0; k<nbPointsPoly; k++)
        {
            if (k%5==0)
            {
                pointToRemove++;
            }
            else
            {
                contour->GetContourRepresentation()->DeleteNthNode(pointToRemove);
            }
        }

        vtkContourRepresentation *contourRep = contour->GetContourRepresentation();

        contourRep->SetClosedLoop(1);

        polyRoi->setIdSlice(i);
        polyRoi->setMasterRoi(false);
        //addViewsToRoi(polyRoi);

        outputRois.append(polyRoi);
    }
    return outputRois;
}

QList<vtkPolyData* > medLabelManager::generateIntermediateCurves(vtkSmartPointer<vtkPolyData> curve1, vtkSmartPointer<vtkPolyData> curve2, int nb)
{
    int max = curve2->GetNumberOfPoints();

    vtkSmartPointer<vtkPolyData> startCurve = curve1, endCurve = curve2;
    bool curve2ToCurve1 = false;

    if (curve1->GetNumberOfPoints()>=curve2->GetNumberOfPoints())
    {
        max = curve1->GetNumberOfPoints();
        startCurve = curve2;
        endCurve = curve1;
        curve2ToCurve1 = true;
    }

    //Homogenise the points distribution on the curve
    resampleCurve(startCurve, max);
    resampleCurve(endCurve, max);

    reorderPolygon(startCurve);
    reorderPolygon(endCurve);

    vtkSmartPointer<vtkPoints> bufferPoints = vtkPoints::New();
    QList<vtkPolyData*> list;
    for(int i=1; i<=nb; i++)
    {
        vtkPolyData *poly = vtkPolyData::New();
        vtkCellArray *cells = vtkCellArray::New();
        vtkPolyLine *polyLine = vtkPolyLine::New();

        polyLine->GetPointIds()->SetNumberOfIds(startCurve->GetNumberOfPoints());

        bufferPoints->Reset();
        for(int k=0; k<startCurve->GetNumberOfPoints(); k++)
        {
            double p1[3],p2[3],p3[3];
            startCurve->GetPoint(k, p1);
            endCurve->GetPoint(k, p2);
            if (curve2ToCurve1)
            {
                p3[0]= p1[0] +(p2[0]-p1[0])*((i)/(double)(nb+1));
                p3[1]= p1[1] +(p2[1]-p1[1])*((i)/(double)(nb+1));
                p3[2]= p1[2] +(p2[2]-p1[2])*((i)/(double)(nb+1));
            }
            else
            {
                p3[0]= p2[0] +(p1[0]-p2[0])*((i)/(double)(nb+1));
                p3[1]= p2[1] +(p1[1]-p2[1])*((i)/(double)(nb+1));
                p3[2]= p2[2] +(p1[2]-p2[2])*((i)/(double)(nb+1));
            }
            bufferPoints->InsertNextPoint(p3);
            polyLine->GetPointIds()->SetId(k, k);
        }
        cells->InsertNextCell(polyLine);

        vtkPoints * polyPoints = vtkPoints::New();
        polyPoints->DeepCopy(bufferPoints);
        poly->SetPoints(polyPoints);
        poly->SetLines(cells);
        list.append(poly);
    }
    return list;
}

void medLabelManager::reorderPolygon(vtkPolyData *poly)
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;

    double displayPoint[3];
    double *worldPoint;
    double xmin = VTK_DOUBLE_MAX;
    int xminIndex = 0;
    double ymin = VTK_DOUBLE_MAX;
    int yminIndex = 0;

    for(int i=0; i<poly->GetNumberOfPoints(); i++)
    {
        worldPoint = poly->GetPoint(i);
        vtkInteractorObserver::ComputeWorldToDisplay(view2d->GetRenderer(), worldPoint[0], worldPoint[1], worldPoint[2], displayPoint);

        if (displayPoint[0] < xmin)
        {
            xmin =  displayPoint[0];
            xminIndex= i;
        }

        if (displayPoint[1] < ymin)
        {
            ymin =  displayPoint[1];
            yminIndex= i;
        }
    }

    int dist = xminIndex-yminIndex;
    bool reverse =((abs(dist)>poly->GetNumberOfPoints()/2) != (dist<0));
    vtkSmartPointer<vtkPoints> reorderedPoints = vtkSmartPointer<vtkPoints>::New();
    vtkPoints *points = poly->GetPoints();
    if (reverse)
    {
        for(int i=0; i<poly->GetNumberOfPoints(); i++)
        {
            double p[3];
            points->GetPoint(xminIndex,p);
            xminIndex--;
            if (xminIndex < 0)
            {
                xminIndex = poly->GetNumberOfPoints()-1;
            }
            reorderedPoints->InsertNextPoint(p);
        }
    }
    else
    {
        for(int i=0; i<poly->GetNumberOfPoints(); i++)
        {
            double p[3];
            points->GetPoint(xminIndex, p);
            xminIndex = (xminIndex+1) % poly->GetNumberOfPoints();
            reorderedPoints->InsertNextPoint(p);
        }
    }
    poly->SetPoints(reorderedPoints);
}

void medLabelManager::resampleCurve(vtkPolyData *poly,int nbPoints)
{
    vtkSmartPointer<vtkParametricSpline> spline =vtkSmartPointer<vtkParametricSpline>::New();
    poly->GetPoints()->InsertNextPoint(poly->GetPoints()->GetPoint(0));
    spline->SetPoints(poly->GetPoints());

    vtkPoints *points = vtkPoints::New();
    double p[3];
    double u[3];
    for(int j=0; j<nbPoints+1; j++)
    {
        u[0] = u[1] = u[2] = j/(double)(nbPoints+1);
        spline->Evaluate(u, p, nullptr);
        points->InsertNextPoint(p);
    }

    poly->SetPoints(points);
}
