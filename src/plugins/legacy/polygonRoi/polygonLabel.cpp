/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <polygonLabel.h>

// medInria
#include <medAbstractDataFactory.h>
#include <medAbstractImageData.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medTagContours.h>
#include <medIntParameterL.h>
#include <medUtilities.h>
#include <medVtkViewBackend.h>
#include <viewevent/baseViewEvent.h>
#include <polygonRoiToolBox.h>
#include <viewinteractors/vtkContourOverlayRepresentation.h>

// vtk
#include <vtkAppendPolyData.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkParametricSpline.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyLine.h>
#include <vtkPointData.h>

class medLabelPolygonsPrivate
{
public:
    medLabelPolygonsPrivate(medAbstractImageView *view, baseViewEvent *eventCursor,
                            QColor &color, QString &name, int position, bool selected,
                            bool interpolate);
    ~medLabelPolygonsPrivate();
    void clearRois();
    QList<polygonRoi*> rois;
    int orientation;
    int sliceOrientation;
    medAbstractImageView *view;
    baseViewEvent *eventCursor;
    medLabelProperty property;
    bool enableInterpolation;
};

medLabelPolygonsPrivate::medLabelPolygonsPrivate(medAbstractImageView *view, baseViewEvent *eventCursor,
                                                 QColor &color, QString &name, int position, bool selected,
                                                 bool interpolate)
    : eventCursor(eventCursor), property(name, color, position, selected), enableInterpolation(interpolate)//, isActivated(true)
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend *>(view->backend())->view2D;
    this->view = view;

    this->orientation = view2d->GetViewOrientation();
    this->sliceOrientation = view2d->GetSliceOrientation();
}

void medLabelPolygonsPrivate::clearRois()
{
    for (polygonRoi *roi: rois)
    {
        delete roi;
    }
    rois.clear();
}

medLabelPolygonsPrivate::~medLabelPolygonsPrivate()
{
    clearRois();
}


polygonLabel::polygonLabel(medAbstractImageView *view, baseViewEvent *eventCursor, QColor &color, QString &name,
                           int position, bool isSelected, bool interpolate) :
    d(new medLabelPolygonsPrivate(view, eventCursor, color, name, position, isSelected, interpolate))
{
}

polygonLabel::~polygonLabel()
{
    delete d;
}

medLabelProperty &polygonLabel::getState()
{
    return d->property;
}

void polygonLabel::updateContourProperty(QString &name, QColor &color, bool isSelected, bool hasScore, bool checkState)
{
    d->property.mainName = name;
    if (d->property.mainColor != color)
    {
        d->property.mainColor = color;
        changeContoursColor(color);
    }
    d->property.selected = isSelected;
    d->property.scoreState = hasScore;
    d->property.checked = checkState;
    if (!hasScore)
    {
        d->property.secondName = "";
        d->property.secondColor = QColor::Invalid;
    }

}

polygonRoi* polygonLabel::appendRoi()
{
    QColor color = (d->rois.empty()) ? d->property.mainColor : d->rois[0]->getColor();

    vtkImageView2D *view2d = getView2D();
    if (!view2d)
        return nullptr;

    auto roi = new polygonRoi(view2d, color);
    d->rois.append(roi);
    connect(roi, SIGNAL(contourFinished(CURSORSTATE)), d->eventCursor, SLOT(onContourFinished(CURSORSTATE)), Qt::UniqueConnection);

    return roi;
}

vtkImageView2D * polygonLabel::getView2D() const
{
    if (d->view->orientation() == medImageView::VIEW_ORIENTATION_3D || d->view->orientation() == medImageView::VIEW_ALL_ORIENTATION)
        return nullptr;

    return static_cast<medVtkViewBackend *>(d->view->backend())->view2D;
}

QColor & polygonLabel::getColor()
{
    return d->property.mainColor;
}

int polygonLabel::getPosition()
{
    return d->property.position;
}

QColor & polygonLabel::getOptColor()
{
    return d->property.secondColor;
}

QString & polygonLabel::getName()
{
    return d->property.mainName;
}

QString & polygonLabel::getOptName()
{
    return d->property.secondName;
}

void polygonLabel::setName(QString &name)
{
    d->property.mainName = name;
}

void polygonLabel::setColor(QColor &color)
{
    d->property.mainColor = color;
}

void polygonLabel::setOptionalNameWithColor(const QString &name, const QColor &color)
{
    d->property.secondName = name;
    d->property.secondColor = color;
}

QList<polygonRoi *> polygonLabel::getRois()
{
    return d->rois;
}

void polygonLabel::loadContours(QVector<medWorldPosContours> &contours)
{
    d->clearRois();
    for (medWorldPosContours nodes : contours)
    {
        polygonRoi *roi = appendRoi();
        if (roi)
        {
            roi->setIdSlice(nodes.getSlice());
            roi->setOrientation(nodes.getOrientation());
            roi->loadNodes(nodes.getNodes());
        }
    }
}

void polygonLabel::removeIntermediateContours()
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

void polygonLabel::setEnableInterpolation(bool state)
{
    d->enableInterpolation = state;
    if (state)
    {
        interpolateIfNeeded();
    }
    else
    {
        removeIntermediateContours();
    }
}

polygonRoi *polygonLabel::roiOpenInSlice()
{
    polygonRoi *roi = getRoiInSlice();
    if (roi && roi->getContour()->GetContourRepresentation()->GetClosedLoop()==false)
    {
        return roi;
    }
    return nullptr;
}

[[maybe_unused]] bool polygonLabel::roiClosedInSlice()
{
    polygonRoi *roi = getRoiInSlice();
    if (roi)
    {
        return roi->getContour()->GetContourRepresentation()->GetClosedLoop();
    }
    return false;
}

bool polygonLabel::isSameSliceOrientation()
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    return (d->sliceOrientation==view2d->GetSliceOrientation());
}

void polygonLabel::removeAllTick()
{
    medIntParameterL *slicingParameter = nullptr;
    foreach (medAbstractInteractor *pInteractor, qobject_cast<medAbstractLayeredView*>(d->view)->layerInteractors(0))
    {
        if ((pInteractor->identifier() == "medVtkViewItkDataImageInteractor") ||
            (pInteractor->identifier() == "medVtkViewItkDataImage4DInteractor"))
        {
            foreach (medAbstractParameterL *parameter, pInteractor->linkableParameters())
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
            slicingParameter->getSlider()->removeTick((int)roi->getIdSlice());
            slicingParameter->getSlider()->update();
        }
    }
}

void polygonLabel::initializeMaskData(medAbstractData * imageData, medAbstractData * maskData )
{
    UChar3ImageType::Pointer mask = UChar3ImageType::New();
    Q_ASSERT(mask->GetImageDimension() == 3);

    auto *mImage = qobject_cast<medAbstractImageData*>(imageData);
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
            auto * imageDataOb = dynamic_cast<itk::ImageBase <2> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

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
            auto * imageDataOb = dynamic_cast<itk::ImageBase <4> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

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
            auto *imageDataOb = dynamic_cast<itk::ImageBase <3> *>( reinterpret_cast<itk::Object*>(imageData->data()) );

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

vtkSmartPointer<vtkPolyData> polygonLabel::getContoursAsPolyData(int label)
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

QVector<medWorldPosContours> polygonLabel::getContoursAsNodes()
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

medAbstractData *polygonLabel::createMask(int label, QString &desc)
{
    vtkImageView2D *view2d = getView2D();
    if (!view2d)
        return nullptr;

    QList<QPair<vtkPolygon *, int>> polygons;
    for (polygonRoi *roi : d->rois)
    {
        vtkContourWidget * contour =  roi->getContour();
        vtkContourRepresentation * contourRep = contour->GetContourRepresentation();
        vtkPolyData * polyData = contourRep->GetContourRepresentationAsPolyData();

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

        for(int j=0; j < polyData->GetNumberOfPoints(); j++)
        {
            double * point = polyData->GetPoint(j);

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
    medAbstractData * data = d->view->layerData(0);
    auto *inputData = qobject_cast<medAbstractImageData*>(data);
    initializeMaskData(inputData, output);
    UChar3ImageType::Pointer m_itkMask = dynamic_cast<UChar3ImageType*>( reinterpret_cast<itk::Object*>(output->data()) );
    for(QPair<vtkPolygon *, int> polygon : polygons)
    {
        double bounds[6];
        polygon.first->GetBounds(bounds);
        unsigned int x=0, y=0, z=0;
        unsigned int bx=0, by=0;
        int dimX=0, dimY=0;
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

        for(int i=(int)bounds[bx]; i<=(int)bounds[bx+1]; i++)
        {
            for(int j=(int)bounds[by]; j<=(int)bounds[by+1]; j++)
            {
                if (i>=0 && i<dimX && j>=0 && j<dimY)
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
    }
    medUtilities::setDerivedMetaData(output, inputData, desc, false, false);

    output->setMetaData(medMetaDataKeys::Toolbox.key(), "PolygonROI");
    output->setMetaData(medMetaDataKeys::OriginalDataUID.key(), inputData->metadata(medMetaDataKeys::SeriesInstanceUID.key()));
    output->setMetaData(medMetaDataKeys::OriginalDataDesc.key(), inputData->metadata(medMetaDataKeys::SeriesDescription.key()));
    return output;
}

void polygonLabel::SetMasterRoi()
{
    polygonRoi *roi = getRoiInSlice();
    if (roi)
    {
        roi->setMasterRoi(true);
    }
}

void polygonLabel::manageTick(medSliderL *slider)
{
    for (polygonRoi *roi : d->rois)
    {
        roi->manageTick(slider);
    }
    d->view->render();
}

bool polygonLabel::isRoiInSlice()
{
    for (polygonRoi* roi : d->rois)
    {
        if (roi->isInCurrentSlice())
            return true;
    }
    return false;
}

polygonRoi *polygonLabel::getRoiInSlice()
{
    vtkImageView2D *view2d = getView2D();
    if (!view2d)
        return nullptr;

    if (!isSameOrientation(view2d->GetViewOrientation()))
    {
        return nullptr;
    }

    for (polygonRoi* roi : d->rois)
    {
        if (roi->isInCurrentSlice())
            return roi;
    }
    return nullptr;
}

bool polygonLabel::isSameOrientation(int orientation)
{
    return (d->orientation==orientation);
}

bool polygonLabel::sortRois(const polygonRoi *p1, const polygonRoi *p2)
{
   return p1->getIdSlice() < p2->getIdSlice();
}

void polygonLabel::connectRois()
{
    for (polygonRoi *roi: d->rois)
    {
        connect(roi, SIGNAL(contourFinished(CURSORSTATE)), d->eventCursor, SLOT(onContourFinished(CURSORSTATE)), Qt::UniqueConnection);
    }
}

void polygonLabel::manageVisibility()
{
    for (polygonRoi *roi : d->rois)
    {
        roi->manageVisibility();
    }
}

void polygonLabel::setRoisSelectedState()
{
    bool selected = d->property.selected;
    for (polygonRoi *roi : d->rois)
    {
        if (selected && roi->isInCurrentSlice())
        {
            roi->setMasterRoi(selected);
        }
        roi->activateContour(selected);
    }
}

void polygonLabel::changeContoursColor(QColor color)
{
    for (polygonRoi *roi : d->rois)
    {
        roi->updateColor(color, d->property.selected);
    }
}

QColor polygonLabel::switchColor()
{
    QColor color;
    if (d->property.secondColor == QColor::Invalid)
    {
        color = QColor::Invalid;
    }
    else
    {
        QColor roiColor = d->rois[0]->getColor();
        color= (roiColor==d->property.mainColor) ? d->property.secondColor : d->property.mainColor;
        for (polygonRoi *roi : d->rois)
        {
            roi->updateColor(color, d->property.selected);
        }
        d->view->render();
    }
    return color;
}

bool polygonLabel::hasScore()
{
    return d->property.scoreState;
}

void polygonLabel::setScoreState(bool state)
{
    d->property.scoreState = state;
}

QVector<QVector2D> polygonLabel::copyContour()
{
    if (isRoiInSlice())
    {
        polygonRoi *roi = getRoiInSlice();
        if (roi)
        {
            return roi->copyContour();
        }
    }
    // impossible case : test of existing roi is already done in caller
    return QVector<QVector2D>();
}

bool polygonLabel::pasteContour(QVector<QVector2D> nodes)
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    polygonRoi *roi = getRoiInSlice();
    if ( roi )
    {
        emit sendErrorMessage(getName() + " already exists in current slice.");
        return false;
    }

    if (!isSameOrientation(view2d->GetViewOrientation()))
    {
        emit sendErrorMessage(getName() + ": has not the same orientation as 2D view");
        return false;
    }

    roi = appendRoi();
    if (roi)
    {
        roi->setCurrentSlice();
        roi->setOrientation(d->orientation);
        roi->pasteContour(nodes);
    }
//    d->view->render();
    return true;
}

double polygonLabel::getMinimumDistanceFromNodesToMouse(double eventPos[2], bool allNodes)
{
    polygonRoi *roi = getRoiInSlice();
    double minDist = DBL_MAX;
    double contourPos[2];
    double dist = 0.;
    if (roi)
    {
        auto *contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(roi->getContour()->GetContourRepresentation());
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

void polygonLabel::deleteNode(double X, double Y)
{
    polygonRoi *roi = getRoiInSlice();
    double minDist = DBL_MAX;
    double contourPos[2];
    double dist = 0.;
    double eventPos[2];
    eventPos[0] = X;
    eventPos[1] = Y;

    int node = -1;
    if (roi)
    {
        auto *contourRep = dynamic_cast<vtkContourOverlayRepresentation*>(roi->getContour()->GetContourRepresentation());
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
//    d->view->render();
}

void polygonLabel::deleteContour()
{
    polygonRoi *roiToDelete = getRoiInSlice();
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
}

double polygonLabel::getDistance(double mousePos[2], double contourPos[2])
{
    return sqrt(pow(mousePos[0] - contourPos[0], 2) + pow(mousePos[1] - contourPos[1], 2));
}

void polygonLabel::interpolateIfNeeded()
{
    if (!d->enableInterpolation)
        return;

    if (d->rois.size() < 2)
        return;

    removeAllTick();
    // TODO : Remove. Find a way to ensure all contour are closed (add pixel tolerance or force close on sliceChangedKeyEvent)
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
        std::sort(d->rois.begin(), d->rois.end(), polygonLabel::sortRois);

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
}

QList<polygonRoi *> polygonLabel::interpolateBetween2Slices(polygonRoi *firstRoi, polygonRoi *secondRoi)
{
    QList<polygonRoi *> outputRois;
    vtkContourRepresentation* minContour;
    vtkContourRepresentation* maxContour;
    // Contour first ROI
    minContour = firstRoi->getContour()->GetContourRepresentation();
    unsigned int minSlice = firstRoi->getIdSlice();

    maxContour = secondRoi->getContour()->GetContourRepresentation();
    unsigned int maxSlice = secondRoi->getIdSlice();
    // Compute intermediate ROIs between two successive ROIs
    QList<QVector<QVector3D>> listOfNodes = generateIntermediateCurves(maxContour,minContour,maxSlice-minSlice-1);
    if ( listOfNodes.size() != static_cast<int>(maxSlice-minSlice-1) )
    {
        emit sendErrorMessage(getName() + ": Unable to interpolate between slice: " + QString::number(minSlice+1) + " and " + QString::number(maxSlice-1) + ". Operation aborted");
    }

    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;
    unsigned int idSlice = minSlice+1;
    for (QVector<QVector3D>& nodes : listOfNodes)
    {
        QColor color = d->property.mainColor;
        if (d->property.scoreState && d->property.checked)
        {
            color = d->property.secondColor;
        }
        auto *polyRoi = new polygonRoi(view2d, color);
        polyRoi->loadNodes(nodes);
        polyRoi->setIdSlice(idSlice);
        idSlice++;
        polyRoi->setMasterRoi(false);

        outputRois.append(polyRoi);
    }
    listOfNodes.clear();
    return outputRois;
}

QList<QVector<QVector3D>> polygonLabel::generateIntermediateCurves(vtkContourRepresentation* firstContour, vtkContourRepresentation* secondContour, int nb)
{
    vtkSmartPointer<vtkPolyData> firstCurve = vtkSmartPointer<vtkPolyData>::New();
    firstContour->GetNodePolyData(firstCurve);

    vtkSmartPointer<vtkPolyData> secondCurve = vtkSmartPointer<vtkPolyData>::New();
    secondContour->GetNodePolyData(secondCurve);

    int nbNodesFirst = firstContour->GetNumberOfNodes();
    int nbNodesSecond = secondContour->GetNumberOfNodes();

    bool curve2ToCurve1 = false;
    vtkSmartPointer<vtkPolyData> startCurve = firstCurve, endCurve = secondCurve;
    vtkSmartPointer<vtkPolyData> poly = firstContour->GetContourRepresentationAsPolyData();

    int max = nbNodesSecond-1;
    if (nbNodesFirst >= nbNodesSecond)
    {
        startCurve = secondCurve;
        endCurve = firstCurve;
        poly = secondContour->GetContourRepresentationAsPolyData();
        max = nbNodesFirst-1;
        curve2ToCurve1 = true;       
    }

    //Homogenise the points distribution on the curve
    if (nbNodesFirst != nbNodesSecond)
    {
        resampleCurve(startCurve, max, poly);
    }
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

void polygonLabel::reorderPolygon(vtkPolyData *poly)
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->view->backend())->view2D;

    double displayPoint[3];
    double *worldPoint;
    auto xmin = VTK_DOUBLE_MAX;
    int xminIndex = 0;
    auto ymin = VTK_DOUBLE_MAX;
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
                xminIndex = (int)poly->GetNumberOfPoints()-1;
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

void polygonLabel::resampleCurve(vtkPolyData *nodes, int nbPoints, vtkPolyData *poly)
{
    vtkSmartPointer<vtkParametricSpline> spline = vtkSmartPointer<vtkParametricSpline>::New();
    nodes->GetPoints()->InsertNextPoint(nodes->GetPoints()->GetPoint(0));
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

    nodes->SetPoints(points);
    points->Delete();
}

void polygonLabel::updateRoiOnOrientedView(medAbstractImageView *pView, bool state)
{
    for (polygonRoi *roi : getRois())
    {
        roi->updateContourOtherView(pView, state);
    }

}

void polygonLabel::removeViewObservers()
{
    for (auto roi : d->rois)
    {
        roi->removeObservers();
    }
}

void polygonLabel::addViewObservers()
{
    for (auto roi : d->rois)
    {
        roi->addObservers();
    }
}




