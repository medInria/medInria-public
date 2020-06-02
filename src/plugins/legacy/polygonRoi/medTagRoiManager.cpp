/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medTagRoiManager.h>

// medInria
#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medDataManager.h>
#include <medTagContours.h>
#include <medIntParameterL.h>
#include <medUtilities.h>
#include <medVtkViewBackend.h>
#include <polygonEventFilter.h>
#include <polygonRoiToolBox.h>
#include <vtkContourOverlayRepresentation.h>

// vtk
#include <vtkAppendPolyData.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkParametricSpline.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyLine.h>
#include <vtkPointData.h>

class medTagRoiManagerPrivate
{
public:
    medTagRoiManagerPrivate(){}
    medTagRoiManagerPrivate(medAbstractView *view, polygonEventFilter *eventCursor, QColor baseColor, QString baseName);
    ~medTagRoiManagerPrivate();
    void clearRois();
    QList<polygonRoi*> rois;
    int orientation;
    int sliceOrientation;
    medAbstractView *view;
    polygonEventFilter *eventCursor;
    QColor baseColor;
    QColor optColor;
    QString baseName;
    QString optName;
    bool enableInterpolation;
    bool isActivated;
    bool scoreState;
};

medTagRoiManagerPrivate::medTagRoiManagerPrivate(medAbstractView *view, polygonEventFilter *eventCursor, QColor color, QString name)
    :eventCursor(eventCursor), enableInterpolation(true), isActivated(true)
{

    this->view = view;
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(view->backend())->view2D;

    this->orientation = view2d->GetViewOrientation();
    this->sliceOrientation = view2d->GetSliceOrientation();
    this->baseColor = color;
    this->baseName = name;
    this->optName = QString();
    this->optColor = QColor::Invalid;
    this->scoreState = false;
    polygonRoi *roi = new polygonRoi(view2d, color);
    rois.append(roi);
}

void medTagRoiManagerPrivate::clearRois()
{
    for (polygonRoi *roi: rois)
    {
        delete roi;
    }
    rois.clear();
}

medTagRoiManagerPrivate::~medTagRoiManagerPrivate()
{
    clearRois();
}


medTagRoiManager::medTagRoiManager(medAbstractView *view, polygonEventFilter *eventCursor, QColor color, QString name):
    d(new medTagRoiManagerPrivate(view, eventCursor, color, name))
{
    connectRois();
}

medTagRoiManager::~medTagRoiManager()
{
    delete d;
}

polygonRoi* medTagRoiManager::appendRoi()
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    QColor color = (d->rois.size()==0)?d->baseColor:d->rois[0]->getColor();
    polygonRoi *roi = new polygonRoi(view2d, color);
    d->rois.append(roi);
    connectRois();
    return roi;
}

void medTagRoiManager::appendRoi(polygonRoi *roi)
{
    d->rois.append(roi);
}

QColor medTagRoiManager::getColor()
{
    return d->baseColor;
}

QColor medTagRoiManager::getOptColor()
{
    return d->optColor;
}

QString medTagRoiManager::getName()
{
    return d->baseName;
}

QString medTagRoiManager::getOptName()
{
    return d->optName;
}

void medTagRoiManager::setName(QString name)
{
    d->baseName = name;
}

void medTagRoiManager::setColor(QColor color)
{
    d->baseColor = color;
}

void medTagRoiManager::setOptionalNameWithColor(QString name, QColor color)
{
    d->optName = name;
    d->optColor = color;
}

QList<polygonRoi *> medTagRoiManager::getRois()
{
    return d->rois;
}

void medTagRoiManager::loadContours( QVector<medWorldPosContours> contours)
{
    d->clearRois();
    for (medWorldPosContours nodes : contours)
    {
        polygonRoi *roi = appendRoi();
        roi->setIdSlice(nodes.getSlice());
        roi->setOrientation(nodes.getOrientation());
        roi->loadNodes(nodes.getNodes());
    }
}

void medTagRoiManager::removeIntermediateContours()
{
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
}

void medTagRoiManager::setEnableInterpolation(bool state)
{
    d->enableInterpolation = state;
    if (state)
    {
        interpolateIfNeeded();
        d->eventCursor->manageTick();
        emit enableOtherViewsVisibility(true);
    }
    else
    {
        removeIntermediateContours();
    }
}

polygonRoi *medTagRoiManager::roiOpenInSlice()
{
    polygonRoi *roi = existingRoiInSlice();
    if (roi && roi->getContour()->GetContourRepresentation()->GetClosedLoop()==false)
    {
        return roi;
    }
    return nullptr;
}

bool medTagRoiManager::roiClosedInSlice()
{
    polygonRoi *roi = existingRoiInSlice();
    if (roi)
    {
        return roi->getContour()->GetContourRepresentation()->GetClosedLoop();
    }
    return false;
}

bool medTagRoiManager::isSameSliceOrientation()
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    return (d->sliceOrientation==view2d->GetSliceOrientation());
}

void medTagRoiManager::removeAllTick()
{
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
    if (slicingParameter)
    {
        for (polygonRoi *roi : d->rois)
        {
            slicingParameter->getSlider()->removeTick(roi->getIdSlice());
            slicingParameter->getSlider()->update();
        }
    }
}

void medTagRoiManager::initializeMaskData( medAbstractData * imageData, medAbstractData * maskData )
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

vtkSmartPointer<vtkPolyData> medTagRoiManager::getContoursAsPolyData(int label)
{
    vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
    append->SetUserManagedInputs(true);
    append->SetNumberOfInputs(d->rois.size());
    int num = 0;
    for (polygonRoi *roi : d->rois)
    {
        vtkSmartPointer<vtkIdTypeArray> array = vtkSmartPointer<vtkIdTypeArray>::New();
        array->SetName("Labels");
        array->SetNumberOfComponents(1);
        vtkSmartPointer<vtkPolyData> pd = roi->getPolyData();
        array->SetNumberOfTuples(pd->GetNumberOfPoints());
        array->Fill(label);
        pd->GetPointData()->AddArray(array);
        append->SetInputDataByNumber(num, pd);
        num++;
    }
    append->Update();
    return append->GetOutput();
}

QVector<medWorldPosContours> medTagRoiManager::getContoursAsNodes()
{
    QVector<medWorldPosContours> contours;
    for (polygonRoi *roi : d->rois)
    {
        if ( roi->isMasterRoi())
        {
            contours.push_back(roi->getContourAsNodes());
        }
    }
    return contours;
}

void medTagRoiManager::createMaskWithLabel(int label)
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
                    m_itkMask->SetPixel(index,label+1);
                }
            }
        }
    }
    QString name = (d->optName==QString())?QString(d->baseName):QString("%1_%2").arg(d->baseName).arg(d->optName);
    QString desc = QString("mask ") + name;
    medUtilities::setDerivedMetaData(output, inputData, desc);
    medDataManager::instance()->importData(output, false);
    return;
}

void medTagRoiManager::SetMasterRoi(bool state)
{
    polygonRoi *roi = existingRoiInSlice();
    if (roi)
    {
        roi->setMasterRoi(true);
    }
}

void medTagRoiManager::manageTick(medSliderL *slider)
{
    for (polygonRoi *roi : d->rois)
    {
        roi->manageTick(slider);
    }
    d->view->render();
    return;
}

polygonRoi *medTagRoiManager::existingRoiInSlice()
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    int slice = view2d->GetSlice();

    if (!isSameOrientation(view2d->GetViewOrientation()))
    {
        emit sendErrorMessage(getName() + ": has not the same orientation as 2D view");
        return nullptr;
    }

    for (polygonRoi* roi : d->rois)
    {
        if (slice==roi->getIdSlice())
            return roi;
    }
    return nullptr;

}

bool medTagRoiManager::isSameOrientation(int orientation)
{
    return (d->orientation==orientation);
}

bool medTagRoiManager::sortRois(const polygonRoi *p1, const polygonRoi *p2)
{
   return p1->getIdSlice() < p2->getIdSlice();
}

void medTagRoiManager::connectRois()
{
    for (polygonRoi *roi: d->rois)
    {
        connect(roi, SIGNAL(updateCursorState(CURSORSTATE)), d->eventCursor, SLOT(setCursorState(CURSORSTATE)), Qt::UniqueConnection);
        connect(roi, SIGNAL(interpolate()), this, SLOT(interpolateIfNeeded()), Qt::UniqueConnection);
        connect(roi, SIGNAL(interpolate()), d->eventCursor, SLOT(manageTick()), Qt::UniqueConnection);
        connect(roi, SIGNAL(enableOtherViewsVisibility(bool)), this, SIGNAL(enableOtherViewsVisibility(bool)), Qt::UniqueConnection);
    }
}

void medTagRoiManager::manageVisibility()
{
    for (polygonRoi *roi : d->rois)
    {
        roi->manageVisibility();
    }
    d->view->render();
}

void medTagRoiManager::activateContours(bool state)
{
    d->isActivated = state;
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    int slice = view2d->GetSlice();
    for (polygonRoi *roi : d->rois)
    {
        if (roi->getIdSlice()==slice)
        {
            roi->setMasterRoi(true);
        }
        roi->activateContour(state);
    }
    d->view->render();
}

void medTagRoiManager::changeContoursColor(QColor color)
{
    for (polygonRoi *roi : d->rois)
    {
        roi->updateColor(color, d->isActivated);
    }
    d->view->render();
}

QColor medTagRoiManager::switchColor()
{
    QColor color;
    if (d->optColor==QColor::Invalid)
    {
        color = QColor::Invalid;
    }
    else
    {
        QColor roiColor = d->rois[0]->getColor();
        color= (roiColor==d->baseColor)?d->optColor:d->baseColor;
        for (polygonRoi *roi : d->rois)
        {
            roi->updateColor(color, d->isActivated);
        }
        d->view->render();
    }
    return color;
}

bool medTagRoiManager::hasScore()
{
    return d->scoreState;
}

void medTagRoiManager::setScoreState(bool state)
{
    d->scoreState = state;
    if (!d->scoreState)
    {
        d->optName = QString();
        d->optColor = QColor::Invalid;
        for (polygonRoi *roi : d->rois)
        {
            roi->updateColor(d->baseColor, d->isActivated);
        }
        d->view->render();
    }
}

void medTagRoiManager::enableOtherViewVisibility(medAbstractImageView *v, bool state)
{
    for (polygonRoi *roi : d->rois)
    {
        roi->updateContourOtherView(v, state);
    }
    v->render();
}

QVector<QVector2D> medTagRoiManager::copyContour()
{
    polygonRoi *roi = existingRoiInSlice();
    if ( roi )
    {
        return roi->copyContour();
    }
    // impossible case : test of existing roi is already done in caller
    return QVector<QVector2D>();
}

bool medTagRoiManager::pasteContour(QVector<QVector2D> nodes)
{
    polygonRoi *roi = existingRoiInSlice();
    if ( roi )
    {
        emit sendErrorMessage(getName() + " already exists in current slice.");
        return false;
    }
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    int slice = view2d->GetSlice();

    if (!isSameOrientation(view2d->GetViewOrientation()))
    {
        emit sendErrorMessage(getName() + ": has not the same orientation as 2D view");
        return false;
    }

    roi = appendRoi();
    roi->setIdSlice(slice);
    roi->setOrientation(d->orientation);
    roi->pasteContour(nodes);
    view2d->Render();
    return true;
}

bool medTagRoiManager::mouseIsCloseFromNodes(double mousePos[2])
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

double medTagRoiManager::getMinimumDistanceFromNodesToMouse(double eventPos[2], bool allNodes)
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
            {
                minDist = dist;
            }

            if (allNodes)
            {
                for ( int k = 0; k<contourRep->GetNumberOfIntermediatePoints(j); k++ )
                {
                    contourRep->GetIntermediatePointDisplayPosition(j, k, contourPos);
                    dist = getDistance(eventPos, contourPos);
                    if ( dist < minDist )
                    {
                        minDist = dist;
                    }
                }
            }
        }

    }
    return minDist;
}

void medTagRoiManager::deleteNode(double X, double Y)
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
            d->rois.removeOne(roi);
            delete roi;
        }
        else if (contourRep->GetNumberOfNodes() == 2)
        {
            qDebug()<<"widget State "<<roi->getContour()->GetWidgetState();
            roi->getContour()->SetWidgetState(vtkContourWidget::Start);
            roi->getContour()->GetContourRepresentation()->SetClosedLoop(false);
        }
    }
    interpolateIfNeeded();
    d->view->render();
}

void medTagRoiManager::deleteContour()
{
    polygonRoi *roiToDelete = existingRoiInSlice();
    if (roiToDelete)
    {
       QList<polygonRoi *>::Iterator it = d->rois.begin();
        while( it != d->rois.end())
        {
            polygonRoi *roi = *it;
            if ( roiToDelete == roi )
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

void medTagRoiManager::removeContourOtherView(medAbstractImageView *v)
{
    polygonRoi *roiToDelete = existingRoiInSlice();
    if (roiToDelete)
    {
        roiToDelete->updateContourOtherView(v, false);
    }

}

void medTagRoiManager::removeIntermediateContoursOtherView(medAbstractImageView *v )
{
    for (polygonRoi *roi: d->rois)
    {
        if (!roi->isMasterRoi())
            roi->updateContourOtherView(v, false);
    }
    v->render();
}

double medTagRoiManager::getDistance(double mousePos[2], double contourPos[2])
{
    return sqrt(pow(mousePos[0] - contourPos[0], 2) + pow(mousePos[1] - contourPos[1], 2));
}

void medTagRoiManager::interpolateIfNeeded()
{
    if (!d->enableInterpolation)
        return;

    if (d->rois.size() < 2)
        return;

    emit enableOtherViewsVisibility(false);
    removeAllTick();
    // TODO : Remove. Find a way to ensure all contour are closed (add pixel tolerance or force close on sliceChangedEvent)
    for (polygonRoi *roi : d->rois)
    {
        if ( roi->getContour()->GetContourRepresentation()->GetClosedLoop() == false )
        {
            emit sendErrorMessage(getName() + ": Non-closed polygon at slice: " + QString::number(roi->getIdSlice()+1) + ". Operation aborted");
            return;
        }
    }

    removeIntermediateContours();
    if (d->rois.size() >= 2)
    {
        std::sort(d->rois.begin(), d->rois.end(), medTagRoiManager::sortRois);

        QList<polygonRoi *> listOfRois;
        for (int i=0; i<d->rois.size()-1; i++)
        {
            polygonRoi *firstRoi = d->rois.at(i);
            polygonRoi *secondRoi = d->rois.at(i+1);
            listOfRois.append(interpolateBetween2Slices(firstRoi, secondRoi));
        }
        d->rois.append(listOfRois);
    }
    emit enableOtherViewsVisibility(true);
    connectRois();
    manageVisibility();
}

QList<polygonRoi *> medTagRoiManager::interpolateBetween2Slices(polygonRoi *firstRoi, polygonRoi *secondRoi)
{
    QList<polygonRoi *> outputRois;
    vtkContourRepresentation* contour;
    // Contour first ROI
    contour = firstRoi->getContour()->GetContourRepresentation();

    vtkSmartPointer<vtkPolyData> curveMin = contour->GetContourRepresentationAsPolyData();
    int minSlice = firstRoi->getIdSlice();

    // Contour second ROI
    contour = secondRoi->getContour()->GetContourRepresentation();
    vtkSmartPointer<vtkPolyData> curveMax = contour->GetContourRepresentationAsPolyData();

    int maxSlice = secondRoi->getIdSlice();

    // Compute intermediate ROIs between two successive ROIs
    QList<QVector<QVector3D>> listOfNodes = generateIntermediateCurves(curveMax,curveMin,maxSlice-minSlice-1);
    if ( listOfNodes.size() != (maxSlice-minSlice-1) )
    {
        emit sendErrorMessage(getName() + ": Unable to interpolate between slice: " + QString::number(minSlice+1) + " and " + QString::number(maxSlice-1) + ". Operation aborted");
    }

    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    int idSlice = minSlice+1;
    for (QVector<QVector3D> nodes : listOfNodes)
    {
        polygonRoi *polyRoi = new polygonRoi(view2d, d->baseColor);
        polyRoi->loadNodes(nodes);
        vtkContourWidget *contour = polyRoi->getContour();
        vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
        contour->GetContourRepresentation()->GetNodePolyData(poly);
        int nbPointsPoly = contour->GetContourRepresentation()->GetNumberOfNodes();
        int pointToRemove = 0;
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
        contour->SetEnabled(true);
        polyRoi->setIdSlice(idSlice);
        idSlice++;
        polyRoi->setMasterRoi(false);

        outputRois.append(polyRoi);
    }
    listOfNodes.clear();
    return outputRois;
}

QList<QVector<QVector3D>> medTagRoiManager::generateIntermediateCurves(vtkSmartPointer<vtkPolyData> curve1, vtkSmartPointer<vtkPolyData> curve2, int nb)
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

    QList<QVector<QVector3D>> listOfNodes;
    for(int i=1; i<=nb; i++)
    {
        QVector<QVector3D> nodes;
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
            nodes.push_back(QVector3D(p3[0], p3[1], p3[2]));
        }
        listOfNodes.append(nodes);
    }
    return listOfNodes;
}

void medTagRoiManager::reorderPolygon(vtkPolyData *poly)
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

void medTagRoiManager::resampleCurve(vtkPolyData *poly,int nbPoints)
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

int medTagRoiManager::getClosestSliceFromCurrent2DView()
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    int currentSlice = view2d->GetSlice();
    int newSlice = -1;
    int minDist = INT_MAX;
    for (polygonRoi *roi : d->rois)
    {
        int dist = roi->getIdSlice() - currentSlice;
        if (std::abs(dist)<minDist)
        {
            minDist = std::abs(dist);
            newSlice = roi->getIdSlice();
        }
    }
    return newSlice;
}
