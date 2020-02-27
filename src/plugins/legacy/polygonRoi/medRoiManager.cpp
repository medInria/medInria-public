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
#include <vtkAppendPolyData.h>
#include <medUtilitiesITK.h>
#include <itkImageBase.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <itkVTKImageToImageFilter.h>
#include <vtkSphereSource.h>
#include <itkImageFileWriter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkMatrix4x4.h>
#include <medAbstractDataFactory.h>
#include <medUtilities.h>
#include <medDataManager.h>

class medLabelManagerPrivate
{
public:
    medLabelManagerPrivate(){}
    medLabelManagerPrivate(medAbstractView *view, polygonEventFilter *eventCursor, QColor color);
    ~medLabelManagerPrivate();

    QList<polygonRoi*> rois;
    int orientation;
    int sliceOrientation;
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
    this->sliceOrientation = view2d->GetSliceOrientation();
    this->color = color;

    polygonRoi *roi = new polygonRoi(view2d, color);
    rois.append(roi);
}

medLabelManagerPrivate::~medLabelManagerPrivate()
{
    for (polygonRoi *roi: rois)
    {
        delete roi;
    }
    rois.clear();
}


medLabelManager::medLabelManager(medAbstractView *view, polygonEventFilter *eventCursor, QColor color):
    d(new medLabelManagerPrivate(view, eventCursor, color))
{
    connectRois();
    manageTick();
}

medLabelManager::~medLabelManager()
{
    delete d;
}

void medLabelManager::appendRoi()
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    polygonRoi *roi = new polygonRoi(view2d, d->color);
    d->rois.append(roi);
    manageTick();
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
        manageVisibility();
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

bool medLabelManager::isSameSliceOrientation()
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    return (d->sliceOrientation==view2d->GetSliceOrientation());
}

void medLabelManager::removeAllTick()
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
        slicingParameter->getSlider()->removeTick(roi->getIdSlice());
        slicingParameter->getSlider()->update();
    }

}

void medLabelManager::initializeMaskData( medAbstractData * imageData, medAbstractData * maskData )
{
    UChar3ImageType::Pointer mask = UChar3ImageType::New();
    Q_ASSERT(mask->GetImageDimension() == 3);

    medAbstractImageData *mImage = qobject_cast<medAbstractImageData*>(imageData);
    Q_ASSERT(mImage);

    UChar3ImageType::RegionType region;
    region.SetSize(0, ( mImage->Dimension() > 0 ? mImage->xDimension() : 1 ) );
    region.SetSize(1, ( mImage->Dimension() > 1 ? mImage->yDimension() : 1 ) );
    region.SetSize(2, ( mImage->Dimension() > 2 ? mImage->zDimension() : 1 ) );

    UChar3ImageType::DirectionType direction;
    UChar3ImageType::SpacingType spacing;
    UChar3ImageType::PointType origin;

    direction.Fill(0);
    spacing.Fill(0);
    origin.Fill(0);
    for (unsigned int i = 0; i < mask->GetImageDimension(); ++i)
    {
        direction(i,i) = 1;
    }

    unsigned int maxIndex = std::min<unsigned int>(mask->GetImageDimension(),mImage->Dimension());

    switch (mImage->Dimension())
    {
        case 2:
        {
            itk::ImageBase <2> * imageDataOb = dynamic_cast<itk::ImageBase <2> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

            for (unsigned int i = 0; i < maxIndex; ++i)
            {
                for (unsigned int j = 0; j < maxIndex; ++j)
                {
                    direction(i,j) = imageDataOb->GetDirection()(i,j);
                }

                spacing[i] = imageDataOb->GetSpacing()[i];
                origin[i] = imageDataOb->GetOrigin()[i];
            }

            break;
        }

        case 4:
        {
            itk::ImageBase <4> * imageDataOb = dynamic_cast<itk::ImageBase <4> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

            for (unsigned int i = 0; i < maxIndex; ++i)
            {
                for (unsigned int j = 0; j < maxIndex; ++j)
                {
                    direction(i,j) = imageDataOb->GetDirection()(i,j);
                }

                spacing[i] = imageDataOb->GetSpacing()[i];
                origin[i] = imageDataOb->GetOrigin()[i];
            }

            break;
        }

        case 3:
        default:
        {
            itk::ImageBase <3> *imageDataOb = dynamic_cast<itk::ImageBase <3> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

            for (unsigned int i = 0; i < maxIndex; ++i)
            {
                for (unsigned int j = 0; j < maxIndex; ++j)
                {
                    direction(i,j) = imageDataOb->GetDirection()(i,j);
                }

                spacing[i] = imageDataOb->GetSpacing()[i];
                origin[i] = imageDataOb->GetOrigin()[i];
            }

            break;
        }
    }

    mask->SetOrigin(origin);
    mask->SetDirection(direction);
    mask->SetSpacing(spacing);
    mask->SetLargestPossibleRegion(region);
    mask->SetBufferedRegion(region);
    mask->Allocate();
    mask->FillBuffer(0);

    maskData->setData((QObject*)(mask.GetPointer()));
}

void medLabelManager::createMaskWithLabel(int label)
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    medAbstractImageView *v = qobject_cast<medAbstractImageView*>(d->view);

    QList<QPair<vtkPolygon *, int>> polygons;
    for (polygonRoi *roi : d->rois)
    {
        vtkContourWidget * contour =  roi->getContour();
        vtkContourRepresentation * contourRep = contour->GetContourRepresentation();
        vtkPolyData * polydata = contourRep->GetContourRepresentationAsPolyData();

        vtkPolygon *polygon = vtkPolygon::New();
        vtkPoints * points = vtkPoints::New();

        std::vector<vtkIdType> vec_ids;
        double imagePreviousPoint[3] ={0,0,0};

        unsigned int x1=0,y1=0,z1=0;
        switch (d->sliceOrientation)
        {
        case 0 :
        {
            x1=1;
            y1=2;
            z1=0;
            break;
        }
        case 1 :
        {
            x1=0;
            y1=2;
            z1=1;
            break;
        }
        case 2 :
        {
            x1=0;
            y1=1;
            z1=2;
            break;
        }
        }

        for(int j=0;j<polydata->GetNumberOfPoints();j++)
        {
            double * point = polydata->GetPoint(j);

            int imagePoint[3];
            double imagePointDouble[3];

            view2d->GetImageCoordinatesFromWorldCoordinates(point,imagePoint);

            imagePointDouble[x1]= (double)imagePoint[x1];
            imagePointDouble[y1]= (double)imagePoint[y1];

            imagePointDouble[z1]= (double)roi->getIdSlice();

            if (imagePointDouble[x1]==imagePreviousPoint[x1] && imagePointDouble[y1]==imagePreviousPoint[y1] && imagePointDouble[z1]==imagePreviousPoint[z1])
            {
                continue;
            }

            vec_ids.push_back(points->InsertNextPoint(imagePointDouble));

            imagePreviousPoint[x1] = imagePointDouble[x1];
            imagePreviousPoint[y1] = imagePointDouble[y1];
            imagePreviousPoint[z1] = imagePointDouble[z1];
        }

        polygon->Initialize(points->GetNumberOfPoints(),&vec_ids[0],points);

        polygons.append(QPair<vtkPolygon*,int>(polygon,roi->getIdSlice()));
    }

    output = medAbstractDataFactory::instance()->create( "itkDataImageUChar3" );
    medAbstractData * data = v->layerData(0);
    medAbstractImageData *inputData = qobject_cast<medAbstractImageData*>(data);
    initializeMaskData(inputData, output);
    UChar3ImageType::Pointer m_itkMask = dynamic_cast<UChar3ImageType*>( reinterpret_cast<itk::Object*>(output->data()) );
    for(QPair<vtkPolygon *, int> polygon : polygons)
    {

        double bounds[6];
        polygon.first->GetBounds(bounds);
        unsigned int x=0, y=0, z=0;
        unsigned int bx=0, by=0;
        unsigned int dimX=0, dimY=0;
        switch (d->sliceOrientation)
        {
            case 0 :
            {
                dimX = inputData->yDimension();
                dimY = inputData->zDimension();
                x = 1;
                y = 2;
                z = 0;
                bx = 2;
                by = 4;
                break;
            }
            case 1 :
            {
                dimX = inputData->xDimension();
                dimY = inputData->zDimension();
                x = 0;
                y = 2;
                z = 1;
                bx = 0;
                by = 4;
                break;
            }
            case 2 :
            {
                dimX = inputData->xDimension();
                dimY = inputData->yDimension();
                x = 0;
                y = 1;
                z = 2;
                bx = 0;
                by = 2;
                break;
            }
        }

        // Qt rasterization
        int nbPoints = polygon.first->GetPoints()->GetNumberOfPoints();
        vtkPoints *pointsArray = polygon.first->GetPoints();
        QPolygonF polygonF;
        for(int i=0; i<nbPoints; i++)
        {
            polygonF << QPointF(pointsArray->GetPoint(i)[x], pointsArray->GetPoint(i)[y]);
        }

        QPainterPath path;
        path.addPolygon(polygonF);

        QBrush brush;
        brush.setColor(Qt::white);
        brush.setStyle(Qt::SolidPattern);

        QImage imagePolygon(dimX,dimY,QImage::Format_RGB16);
        imagePolygon.fill(Qt::black);
        QPainter painter;
        painter.begin(&imagePolygon);
        painter.fillPath(path,brush);

        for(int i=bounds[bx]; i<=bounds[bx+1]; i++)
        {
            for(int j=bounds[by]; j<=bounds[by+1]; j++)
            {
                QRgb val = imagePolygon.pixel(i,j);

                if (val==qRgb(255,255,255))
                {
                    UChar3ImageType::IndexType index;
                    index[x]=i;
                    index[y]=j;
                    index[z]=polygon.second;
                    m_itkMask->SetPixel(index,label);
                }
            }
        }
    }
    QString desc = QString("contour with label ") + QString::number(label);
    medUtilities::setDerivedMetaData(output, inputData, desc);
    medDataManager::instance()->importData(output, false);
    return;
}

void medLabelManager::SetMasterRoi(bool state)
{
    polygonRoi *roi = existingRoiInSlice();
    roi->setMasterRoi(true);
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
    if ( slicingParameter )
    {
        slicingParameter->getSlider()->removeAllTicks();
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
    }
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
    for (polygonRoi *roi : d->rois)
    {
        roi->manageVisibility();
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

void medLabelManager::deleteNode(double X, double Y)
{
    polygonRoi *roi = existingRoiInSlice();
    double minDist = DBL_MAX;
    double contourPos[2];
    double dist = 0.;
    double eventPos[2];
    eventPos[0] = X;
    eventPos[1] = Y;

    int node = -1;
    if (roi)
    {
        vtkContourOverlayRepresentation *contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(roi->getContour()->GetContourRepresentation());
        for(int j=0; j<contourRep->GetNumberOfNodes(); j++)
        {
            contourRep->GetNthNodeDisplayPosition(j, contourPos);
            dist = getDistance(eventPos, contourPos);
            if ( dist < minDist )
            {
                minDist = dist;
                node = j;
            }
        }
        contourRep->DeleteNthNode(node);
        if (contourRep->GetNumberOfNodes() == 0)
        {
            qDebug()<<"rois size 1 "<<d->rois.size();
            d->rois.removeOne(roi);
            delete roi;
            qDebug()<<"rois size 2 "<<d->rois.size();
            //d->view->render();
            //return;
        }
    }
    interpolateIfNeeded();
    d->view->render();
}

void medLabelManager::deleteContour()
{
    polygonRoi *roiToCheck = existingRoiInSlice();
    if (roiToCheck)
    {
        QList<polygonRoi *>::Iterator it = d->rois.begin();
        while( it != d->rois.end())
        {
            polygonRoi *roi = *it;
            if ( roiToCheck == roi )
            {
                it = d->rois.erase(it);
                delete roi;
            }
            else
            {
                ++it;
            }
        }
    }
    interpolateIfNeeded();
    d->view->render();
}

double medLabelManager::getDistance(double mousePos[2], double contourPos[2])
{
    return sqrt(pow(mousePos[0] - contourPos[0], 2) + pow(mousePos[1] - contourPos[1], 2));
}

void medLabelManager::interpolateIfNeeded()
{
    if (!d->enableInterpolation)
        return;

    if (d->rois.size() < 2)
        return;

    removeAllTick();

    // TODO : Remove. Find a way to ensure all contour are closed (add pixel tolerance or force close on sliceChangedEvent)
    for (polygonRoi *roi : d->rois)
    {
        if ( roi->getContour()->GetContourRepresentation()->GetClosedLoop() == false )
        {
            //displayMessageError("Non-closed polygon at slice: " + QString::number(roi->getIdSlice()+1) + ". Operation aborted");
            return;
        }
    }

    QList<polygonRoi *>::Iterator it = d->rois.begin();
    while( it != d->rois.end())
    {
        polygonRoi *roi = *it;
        if ( roi->isMasterRoi() )
        {
            ++it;
        }
        else
        {
            it = d->rois.erase(it);
            delete roi;
        }
    }
    qDebug()<<"size of rois "<<d->rois.size();
    if (d->rois.size() >= 2)
    {
        std::sort(d->rois.begin(), d->rois.end(), medLabelManager::sortRois);

        QList<polygonRoi *> listOfRois;
        for (int i=0; i<d->rois.size()-1; i++)
        {
            polygonRoi *firstRoi = d->rois.at(i);
            polygonRoi *secondRoi = d->rois.at(i+1);
            listOfRois.append(interpolateBetween2Slices(firstRoi, secondRoi));
        }
        d->rois.append(listOfRois);
    }
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

    for (vtkPolyData *pd : listPolyData)
    {
        pd->Delete();
    }
    listPolyData.clear();

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

    vtkSmartPointer<vtkPoints> bufferPoints = vtkSmartPointer<vtkPoints>::New();
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
        cells->Delete();
        polyLine->Delete();
        polyPoints->Delete();
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
    points->Delete();
}
