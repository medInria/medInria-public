/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/
#include <voiCutterToolBox.h>

#include <medAbstractDataFactory.h>
#include <medAbstractImageView.h>
#include <medAbstractParameterL.h>
#include <medBoolGroupParameterL.h>
#include <medCompositeParameterL.h>
#include <medDataManager.h>
#include <medPluginManager.h>
#include <medStringListParameterL.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medUtilities.h>
#include <medUtilitiesITK.h>
#include <medVtkViewBackend.h>

#include <vtkCamera.h>
#include <vtkInriaInteractorStyleDrawPolygon.h>
#include <vtkPoints.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>

#include <itkImageDuplicator.h>

class voiCutterToolBoxPrivate
{
public:
    QPushButton *scissorButton, *saveImageButton;
    QLabel *explanation;
    vtkInteractorObserver *interactorStyle3D; // Pointer to keep the previous interactor
    vtkInriaInteractorStyleDrawPolygon *interactorStyleDrawPolygon;
    medAbstractImageView *currentView;
    bool scissorOn;
    vtkCutterObserver *observer;
    dtkSmartPointer<medAbstractData> resultData;
    medAbstractData *input;
    unsigned int layerInput;
    medStringListParameterL *mode3DParam;
    medAbstractBoolParameterL *orientation3DParam;
    
    // Rendering parameters
    QHash<QString, QVariant> windowingParams;
    QString lut;
};

class vtkCutterObserver : public vtkCommand
{
public:
    static vtkCutterObserver * New()
    {
        return new vtkCutterObserver;
    }

    vtkCutterObserver(){}

    virtual void Execute(vtkObject* caller, unsigned long event, void *calldata)
    {
        Q_UNUSED(caller)
        Q_UNUSED(event)
        Q_UNUSED(calldata)

        vtkRenderWindowInteractor *iren = static_cast<medVtkViewBackend*>
                (tb->d->currentView->backend())->renWin->GetInteractor();
        char keycode = iren->GetKeyCode();

        // enter => keep only VOI defined
        // tab => restore VOI defined
        // backspace => delete VOI defined
        switch(keycode)
        {
            case '\b': // backspace
                tb->launchTheCutting(Remove);
                break;
            case '\r': // return
                tb->launchTheCutting(Keep);
                break;
            case '\t': // tab
                tb->launchTheCutting(Restore);
                break;
        }
    }

    void setVoiCutterToolBox(voiCutterToolBox *tb)
    {
        this->tb = tb;
    }

private:
    voiCutterToolBox *tb;
};

voiCutterToolBox::voiCutterToolBox(QWidget *parent) :
    medAbstractSelectableToolBox(parent),
    d(new voiCutterToolBoxPrivate)
{
    QWidget *displayWidget = new QWidget(this);
    this->addWidget(displayWidget);

    QHBoxLayout *layout = new QHBoxLayout();

    d->scissorButton = new QPushButton ("", this);
    d->scissorButton->setFocusPolicy(Qt::NoFocus);
    d->scissorButton->setText("Cut Volume");
    d->scissorButton->setObjectName("Cut Volume");
    d->scissorButton->setCheckable(true);
    d->scissorButton->setMinimumWidth(20);
    d->scissorButton->setToolTip(tr("Cut volume tool"));
    connect(d->scissorButton, SIGNAL(toggled(bool)), this, SLOT(activateScissor(bool)));

    d->saveImageButton = new QPushButton("Save Image",this);
    d->saveImageButton->setObjectName("Save Image");
    d->saveImageButton->setToolTip(tr("Import the current image to the non persistent database"));
    connect(d->saveImageButton, SIGNAL(clicked()), this, SLOT(saveImage()));

    layout->addWidget(d->scissorButton);
    layout->addWidget(d->saveImageButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(displayWidget);

    d->explanation = new QLabel(tr(""));
    d->explanation->setWordWrap(true);
    d->explanation->setStyleSheet("font: italic");

    mainLayout->addWidget(d->explanation);
    mainLayout->addLayout(layout);
    displayWidget->setLayout(mainLayout);

    d->scissorOn = false;
    d->currentView = nullptr;
    d->resultData = nullptr;
    d->input = nullptr;
    d->layerInput = 0;

    d->observer = vtkCutterObserver::New();
    d->observer->setVoiCutterToolBox(this);
    d->interactorStyleDrawPolygon = vtkInriaInteractorStyleDrawPolygon::New();
    d->interactorStyleDrawPolygon->AddObserver(vtkCommand::KeyPressEvent, d->observer, 0);

    d->orientation3DParam = 0;

    activateButtons(false);
}

voiCutterToolBox::~voiCutterToolBox()
{
    d->observer->Delete();
    d->interactorStyleDrawPolygon->Delete();
    delete d;
    d = nullptr;
}

bool voiCutterToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<voiCutterToolBox>();
}

void voiCutterToolBox::updateView()
{
    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();

    if (view)
    {
        d->currentView = qobject_cast<medAbstractImageView*>(view);

        // Toolbox does not work with meshes or vector images
        for (unsigned int i=0; i<d->currentView->layersCount(); ++i)
        {
            medAbstractData *data = d->currentView->layerData(i);
            if(!data || data->identifier().contains("vtkDataMesh")
                    || !data->identifier().contains("itkDataImage") //avoid medVtkFibersData also
                    || data->identifier().contains("itkDataImageVector"))
            {
                handleDisplayError(medAbstractProcessLegacy::DIMENSION_3D);
                d->currentView = nullptr;
                return;
            }
        }

        d->mode3DParam = 0;
        d->orientation3DParam = 0;
        QList<medAbstractParameterL*> primaryParams = view->linkableParameters();

        for(int i=0; i<primaryParams.length(); i++)
        {
            if (primaryParams[i]->name() == "Orientation")
            {
                QList<medAbstractParameterL*> orientationCandidates = qobject_cast<medBoolGroupParameterL*>(primaryParams[i])->parametersCandidateToPool();
                for(int j =0; j<orientationCandidates.length(); j++)
                {
                    if (orientationCandidates[j]->name() == "3d")
                    {
                        d->orientation3DParam = qobject_cast<medAbstractBoolParameterL*>(orientationCandidates[j]);
                        break;
                    }
                }
                if (d->orientation3DParam)
                {
                    break;
                }
            }
        }

        QList<medAbstractNavigator*> extraNavigators = view->navigators();

        for(int i= 0; i<extraNavigators.length(); i++)
        {
            QList<medAbstractParameterL*> extraParams = extraNavigators[i]->linkableParameters();
            for(int j=0; j<extraParams.length(); j++)
            {
                if (extraParams[j]->name() == "3D Mode")
                {
                    d->mode3DParam = qobject_cast<medStringListParameterL*>(extraParams[j]);
                    break;
                }
            }
            if (d->mode3DParam)
            {
                break;
            }
        }

        // Automatic display of mask in 3D
        if (!d->orientation3DParam->value()) // data on 2D mode
        {
            activateButtons(true);
            switchToVR(true); // adjust the data view.
        }

        connect(view, SIGNAL(closed()), this, SLOT(onViewClosed()), Qt::UniqueConnection);
        connect(d->currentView, SIGNAL(orientationChanged()), this, SLOT(adaptWidgetsToOrientationChange()), Qt::UniqueConnection);

        d->interactorStyleDrawPolygon->SetView(view);
    }
    else
    {
        d->currentView = nullptr;
    }
}

void voiCutterToolBox::onViewClosed()
{
    medAbstractView *viewClosed = qobject_cast<medAbstractView*>(QObject::sender());

    if (viewClosed == d->currentView)
    {
        d->currentView = nullptr;
    }

    if (d->scissorOn)
    {
        d->scissorButton->click(); // Deactivate cut volume tool
    }

    d->resultData = nullptr;
    activateButtons(false);
}

dtkPlugin* voiCutterToolBox::plugin()
{
    return medPluginManager::instance().plugin("VOI Cutter");
}

medAbstractData *voiCutterToolBox::processOutput()
{
    if (!d->resultData)
    {
        return d->currentView->layerData(d->currentView->currentLayer());
    }

    fillOutputMetaData();
    return d->resultData;
}

void voiCutterToolBox::activateButtons(bool param)
{
    if (!param)
    {
        // No valid data is in the container, or not in 3D
        d->explanation->setText(tr("Drop a volume in the view"));
        d->scissorButton->setEnabled(false);
    }
    else
    {
        d->explanation->setText(tr("To define a VOI, choose 'Cut Volume', click on the volume and then <br>"
                                   " * press Enter to keep it.<br>"
                                   " * press Backspace to remove it."));
        d->scissorButton->setEnabled(true);
    }

    if (!d->resultData)
    {
        d->saveImageButton->setEnabled(false);
    }
    else
    {
        d->saveImageButton->setEnabled(true);
    }
}

void voiCutterToolBox::clear()
{
    activateButtons(false);
    switchToVR(false);
}

void voiCutterToolBox::switchToVR(bool checked)
{
    this->setToolBoxOnWaitStatusForNonRunnableProcess();

    if(checked)
    {
        d->mode3DParam->setValue("VR");
    }
    else
    {
        d->scissorButton->setChecked(checked);
    }

    if(d->currentView)
    {
        if (d->orientation3DParam)
        {
            d->orientation3DParam->setValue(checked);
        }
        if (checked) // 3d
        {
            d->currentView->setOrientation(medImageView::VIEW_ORIENTATION_3D);
        }
        else
        {
            d->currentView->setOrientation(medImageView::VIEW_ORIENTATION_SAGITTAL);
        }
    }

    this->setToolBoxOnReadyToUse();
}

void voiCutterToolBox::adaptWidgetsToOrientationChange()
{
    if (d->currentView->orientation() != medImageView::VIEW_ORIENTATION_3D)
    {
        if (d->scissorOn)
        {
            d->scissorButton->click(); // Deactivate cut volume tool
        }
        activateButtons(false);
    }
    else
    {
        activateButtons(true);
    }
}

void voiCutterToolBox::activateScissor(bool checked)
{
    if (d->currentView)
    {
        vtkImageView3D *view3D =  static_cast<medVtkViewBackend*>(d->currentView->backend())->view3D;

        if (checked)
        {
            d->interactorStyle3D = view3D->GetInteractor()->GetInteractorStyle();
            view3D->GetInteractor()->SetInteractorStyle(d->interactorStyleDrawPolygon);
            d->scissorOn = true;
        }
        else
        {
            view3D->GetInteractor()->SetInteractorStyle(d->interactorStyle3D);
            d->interactorStyleDrawPolygon->ResetPolygon();
            d->scissorOn = false;
        }
    }
    else
    {
        d->interactorStyleDrawPolygon->ResetPolygon();
        d->scissorOn = false;
    }
}

void voiCutterToolBox::saveImage()
{
    if (d->resultData)
    {
        fillOutputMetaData();
        medDataManager::instance().importData(d->resultData, false);
    }
}

void voiCutterToolBox::launchTheCutting(MODE m)
{
    if (d->currentView && d->scissorOn)
    {
        vtkImageView3D *view3D =  static_cast<medVtkViewBackend*>(d->currentView->backend())->view3D;
        std::vector<vtkVector2i> polygonPoints = static_cast<vtkInriaInteractorStyleDrawPolygon*>(view3D->GetInteractor()->GetInteractorStyle())->GetPolygonPoints();
        definePolygonsImage(polygonPoints, m);
        d->interactorStyleDrawPolygon->ResetPolygon();
        view3D->GetInteractor()->SetInteractorStyle(d->interactorStyleDrawPolygon);
    }
}

void voiCutterToolBox::saveRenderingParameters()
{
    // Synchronize Lut
    QList<medAbstractInteractor*> interactors = d->currentView->layerInteractors(0);
    QList<medAbstractParameterL*> parameters;
    for(int i=0; i<interactors.size(); i++)
    {
        if (interactors[i]->identifier() == "medVtkViewItkDataImageInteractor")
        {
            parameters = interactors[i]->linkableParameters();
            for(int j = 0; j<parameters.size(); j++)
            {
                if (parameters[j]->name() == "Lut")
                {
                    d->lut = qobject_cast<medStringListParameterL*>(parameters[j])->value();
                }
            }
        }
    }

    // Synchronize Window/Level
    medCompositeParameterL *windowLevelParameter1 = d->currentView->windowLevelParameter(0);
    QList<QVariant> windowLevel = windowLevelParameter1->values();
    if (windowLevel.size() == 2)
    {
        d->windowingParams.insert("Level",  windowLevel[0]);
        d->windowingParams.insert("Window", windowLevel[1]);
    }
}

void voiCutterToolBox::applyRenderingParameters()
{
    QList<medAbstractInteractor*> interactors = d->currentView->layerInteractors(0);

    for(int i=0; i<interactors.size(); i++)
    {
        if (interactors[i]->identifier() == "medVtkViewItkDataImageInteractor")
        {
            QList<medAbstractParameterL*> parameters = interactors[i]->linkableParameters();
            for(int j = 0; j<parameters.size(); j++)
            {
                if (parameters[j]->name() == "Lut")
                {
                    qobject_cast<medStringListParameterL*>(parameters[j])->setValue(d->lut);
                }
            }
        }
    }

    medCompositeParameterL *windowLevelParameter = d->currentView->windowLevelParameter(0);
    windowLevelParameter->setValues(d->windowingParams);
}

// intersect3D_SegmentPlane(): intersect a segment and a plane
//    Input:  S = a segment, and Pn = a plane = {Point V0; Vector n;}
//    Output: *I0 = the intersect point (when it exists)
//    Return: 0 = disjoint (no intersection)
//            1 = intersection in the unique point *I0
//            2 = the segment lies in the plane

#define SMALL_NUM  0.00000001 // anything that avoids division overflow
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

int voiCutterToolBox::intersect3D_SegmentPlane(float *P0, float *P1, float *Pnormal, float *Ppoint, double* resultPt)
{
    float u[ 3];
    float w[ 3];

    u[ 0]  = P1[ 0] - P0[ 0];
    u[ 1]  = P1[ 1] - P0[ 1];
    u[ 2]  = P1[ 2] - P0[ 2];

    w[ 0] =  P0[ 0] - Ppoint[ 0];
    w[ 1] =  P0[ 1] - Ppoint[ 1];
    w[ 2] =  P0[ 2] - Ppoint[ 2];

    float D = DOT(Pnormal, u);
    float N = -DOT(Pnormal, w);

    if (fabs(D) < SMALL_NUM)            // segment is parallel to plane
    {
        return 0;
    }

    // they are not parallel
    // compute intersect param

    float sI = N / D;
    if (sI < 0 || sI > 1)
    {
        return 0;	// no intersection
    }

    resultPt[ 0] = P0[ 0] + sI * u[ 0];		// compute segment intersect point
    resultPt[ 1] = P0[ 1] + sI * u[ 1];
    resultPt[ 2] = P0[ 2] + sI * u[ 2];

    return 1;
}


void voiCutterToolBox::definePolygonsImage(std::vector<vtkVector2i> polygonPoints, MODE m)
{
    d->layerInput = d->currentView->currentLayer();
    d->input = d->currentView->layerData(d->layerInput);

    if (d->input->identifier().contains("vtkDataMesh")) // meshes are not allowed
    {
        return;
    }
    vtkImageView3D *view3D =  static_cast<medVtkViewBackend*>(d->currentView->backend())->view3D;
    vtkSmartPointer<vtkPoints> points = vtkPoints::New();
    vtkIdType ids[100];
    QList<vtkPolygon*> *RoiList = new QList<vtkPolygon*>();
    QList<vtkPoints*> *RoiPointList = new QList<vtkPoints*>();

    vtkMatrix4x4 *ActorMatrix;
    vtkVolume *volume = view3D->GetVolumeActor();
    ActorMatrix = volume->GetUserMatrix();
    vtkTransform *Transform = vtkTransform::New();
    Transform->SetMatrix(ActorMatrix);
    Transform->Push();

    vtkCamera *aCamera = view3D->GetRenderer()->GetActiveCamera();
    double cameraProj[3], xyz[3];
    aCamera->GetViewPlaneNormal(cameraProj);
    aCamera->GetPosition(xyz);

    int *dim = view3D->GetMedVtkImageInfo()->dimensions;
    vtkMatrix4x4 *orientationMatrix = view3D->GetOrientationMatrix();

    double *vPos = view3D->GetMedVtkImageInfo()->origin;

    for( int i = 0; i<static_cast<int>(polygonPoints.size()); i++)
    {
        points->InsertNextPoint(polygonPoints[i].GetX(),
                                polygonPoints[i].GetY(),
                                0);
        ids[i] = i;
    }

    double vector[9];

    for (int i = 0; i<3; i++)
    {
        for (int j = 0; j<3; j++)
        {
            vector[j*3+i] = orientationMatrix->GetElement(i,j);
        }
    }

    unsigned int stackOrientation;
    double cameraProjObj[3];
    cameraProjObj[ 0] = cameraProj[ 0] * vector[ 0] + cameraProj[ 1] * vector[ 1] + cameraProj[ 2] * vector[ 2];
    cameraProjObj[ 1] = cameraProj[ 0] * vector[ 3] + cameraProj[ 1] * vector[ 4] + cameraProj[ 2] * vector[ 5];
    cameraProjObj[ 2] = cameraProj[ 0] * vector[ 6] + cameraProj[ 1] * vector[ 7] + cameraProj[ 2] * vector[ 8];

    if( fabs( cameraProjObj[ 0]) > fabs(cameraProjObj[ 1]) && fabs(cameraProjObj[ 0]) > fabs(cameraProjObj[ 2]))
    {
        stackOrientation = 0;
    }
    else if( fabs(cameraProjObj[ 1]) > fabs(cameraProjObj[ 0]) && fabs(cameraProjObj[ 1]) > fabs(cameraProjObj[ 2]))
    {
        stackOrientation = 1;
    }
    else
    {
        stackOrientation = 2;
    }

    long stackMax;
    switch( stackOrientation)
    {
        case 0: stackMax = dim[0]; break;
        case 1: stackMax = dim[1]; break;
        case 2: stackMax = dim[2]; break;
    }

    for( int i = 0 ; i < stackMax ; i++)
    {
        RoiList->append(vtkPolygon::New());
        RoiPointList->append(vtkPoints::New());
    }
    double *ImageSpacing  = view3D->GetMedVtkImageInfo()->spacing;
    for(long tt = 0; tt < points->GetNumberOfPoints(); tt++)
    {
        float point1[ 3], point2[ 3];
        long x;
        double point2D[ 3], *pp;

        points->GetPoint(tt, point2D);
        view3D->GetRenderer()->SetDisplayPoint(point2D[ 0], point2D[ 1], 0);
        view3D->GetRenderer()->DisplayToWorld();
        pp = view3D->GetRenderer()->GetWorldPoint();

        if(aCamera->GetParallelProjection())
        {
            aCamera->GetPosition(xyz);

            xyz[ 0] = pp[0] + cameraProj[ 0];
            xyz[ 1] = pp[1] + cameraProj[ 1];
            xyz[ 2] = pp[2] + cameraProj[ 2];

            // Go beyond the object...
            pp[0] = xyz[ 0] + (pp[0] - xyz[ 0]) * 5000.;
            pp[1] = xyz[ 1] + (pp[1] - xyz[ 1]) * 5000.;
            pp[2] = xyz[ 2] + (pp[2] - xyz[ 2]) * 5000.;
            //
            point1[ 0] = xyz[ 0];
            point1[ 1] = xyz[ 1];
            point1[ 2] = xyz[ 2];
            //
            point2[ 0] = pp[ 0];
            point2[ 1] = pp[ 1];
            point2[ 2] = pp[ 2];
        }
        else
        {
            // Go beyond the object...
            point1[ 0] = xyz[ 0];
            point1[ 1] = xyz[ 1];
            point1[ 2] = xyz[ 2];
            //
            point2[0] = xyz[ 0] + (pp[0] - xyz[ 0])*5000.;
            point2[1] = xyz[ 1] + (pp[1] - xyz[ 1])*5000.;
            point2[2] = xyz[ 2] + (pp[2] - xyz[ 2])*5000.;
        }

        // Intersection between this line and planes in Z direction
        for(x = 0; x < stackMax; x++)
        {
            float planeVector[ 3];
            float point[ 3];
            double resultPt[ 3];

            switch(stackOrientation)
            {
                case 0:
                {
                    point[ 0] = x * ImageSpacing[0];
                    point[ 1] = 0;
                    point[ 2] = 0;

                    planeVector[ 0] =  vector[ 0];
                    planeVector[ 1] =  vector[ 1];
                    planeVector[ 2] =  vector[ 2];
                    break;
                }
                case 1:
                {
                    point[ 0] = 0;
                    point[ 1] = x * ImageSpacing[1];
                    point[ 2] = 0;

                    planeVector[ 0] =  vector[ 3];
                    planeVector[ 1] =  vector[ 4];
                    planeVector[ 2] =  vector[ 5];
                    break;
                }
                case 2:
                {
                    point[ 0] = 0;
                    point[ 1] = 0;
                    point[ 2] = x * ImageSpacing[2];

                    planeVector[ 0] =  vector[ 6];
                    planeVector[ 1] =  vector[ 7];
                    planeVector[ 2] =  vector[ 8];
                    break;
                }
            }

            point[ 0] += vPos[ 0];
            point[ 1] += vPos[ 1];
            point[ 2] += vPos[ 2];

            Transform->TransformPoint(point, point);

            if( intersect3D_SegmentPlane(point2, point1, planeVector, point, resultPt))
            {
                int ptInt[ 3];
                long roiID;

                view3D->GetImageCoordinatesFromWorldCoordinates(resultPt, ptInt);

                switch( stackOrientation)
                {
                    case 0: roiID = ptInt[0]; break;
                    case 1: roiID = ptInt[1]; break;
                    case 2: roiID = ptInt[2]; break;
                }

                if( roiID >= 0 && roiID < stackMax)
                {
                    RoiPointList->at(roiID)->InsertNextPoint(ptInt[0], ptInt[1], ptInt[2]);
                }
            }
        }
    }
    Transform->Delete();

    for(int i= 0; i<RoiList->size(); i++)
    {
        RoiList->at(i)->Initialize(RoiPointList->at(i)->GetNumberOfPoints(),
                                   ids,
                                   RoiPointList->at(i));
    }

    if (d->input->identifier() == "itkDataImageChar3")
    {
        cutThroughImage<itk::Image<char,3> >(RoiList,RoiPointList,stackMax,stackOrientation,m);
    }
    else if (d->input->identifier() == "itkDataImageUChar3")
    {
        cutThroughImage<itk::Image<unsigned char,3> >(RoiList,RoiPointList,stackMax,stackOrientation,m);
    }
    else if (d->input->identifier() == "itkDataImageShort3")
    {
        cutThroughImage<itk::Image<short,3> >(RoiList,RoiPointList,stackMax,stackOrientation,m);
    }
    else if (d->input->identifier() == "itkDataImageUShort3")
    {
        cutThroughImage<itk::Image<unsigned short,3> >(RoiList,RoiPointList,stackMax,stackOrientation,m);
    }
    else if (d->input->identifier() == "itkDataImageInt3")
    {
        cutThroughImage<itk::Image<int,3> >(RoiList,RoiPointList,stackMax,stackOrientation,m);
    }
    else if (d->input->identifier() == "itkDataImageUInt3")
    {
        cutThroughImage<itk::Image<unsigned int,3> >(RoiList,RoiPointList,stackMax,stackOrientation,m);
    }
    else if (d->input->identifier() == "itkDataImageLong3")
    {
        cutThroughImage<itk::Image<long,3> >(RoiList,RoiPointList,stackMax,stackOrientation,m);
    }
    else if (d->input->identifier() == "itkDataImageULong3")
    {
        cutThroughImage<itk::Image<unsigned long,3> >(RoiList,RoiPointList,stackMax,stackOrientation,m);
    }
    else if (d->input->identifier() == "itkDataImageFloat3")
    {
        cutThroughImage<itk::Image<float,3> >(RoiList,RoiPointList,stackMax,stackOrientation,m);
    }
    else if (d->input->identifier() == "itkDataImageDouble3")
    {
        cutThroughImage<itk::Image<double,3> >(RoiList,RoiPointList,stackMax,stackOrientation,m);
    }

    for(auto poly : *RoiList)
        poly->Delete();
    RoiList->clear();
    delete RoiList;

    for(auto pts : *RoiPointList)
        pts->Delete();
    RoiPointList->clear();
    delete RoiPointList;

}

template <typename IMAGE>
void voiCutterToolBox::cutThroughImage(QList<vtkPolygon*>*RoiList, QList<vtkPoints*>*RoiPointList,
                                       long stackMax, unsigned int stackOrientation, MODE m)
{
    int valOfOutside = medUtilitiesITK::minimumValue(d->input);

    vtkImageView3D *view3D =  static_cast<medVtkViewBackend*>(d->currentView->backend())->view3D;
    int *dim = view3D->GetMedVtkImageInfo()->dimensions;

    IMAGE *inputImage = dynamic_cast<IMAGE*>((itk::Object*)(d->input->data()));

    typedef itk::ImageDuplicator< IMAGE > DuplicatorType;
    typename DuplicatorType::Pointer duplicator = DuplicatorType::New();
    duplicator->SetInputImage(inputImage);
    duplicator->Update();
    typename IMAGE::Pointer clonedImage = duplicator->GetOutput();

    typename IMAGE::IndexType index;
    unsigned int x=0, y=0, z=0;
    unsigned int bx=0, by=0;

    switch (stackOrientation)
    {
        case 0 :
        {
            x = 1;
            y = 2;
            z = 0;
            bx = 2;
            by = 4;
            break;
        }
        case 1 :
        {
            x = 0;
            y = 2;
            z = 1;
            bx = 0;
            by = 4;
            break;
        }
        case 2 :
        {
            x = 0;
            y = 1;
            z = 2;
            bx = 0;
            by = 2;
            break;
        }
    }

    for(int stack = 0; stack<stackMax; stack++)
    {
        double *boundsPoints;
        int nbPoints = RoiList->at(stack)->GetNumberOfPoints();
        boundsPoints = RoiPointList->at(stack)->GetBounds(); //(xmin,xmax, ymin,ymax, zmin,zmax)

        int i, end_i, j, end_j, start_i, start_j;

        if (dim[x]<=boundsPoints[bx+1])
        {
            end_i = dim[x]-1;
        }
        else
        {
            end_i = boundsPoints[bx+1];
        }

        if (dim[y]<=boundsPoints[by+1])
        {
            end_j = dim[y]-1;
        }
        else
        {
            end_j = boundsPoints[by+1];
        }

        if (boundsPoints[bx]>=0)
        {
            start_i = boundsPoints[bx];
        }
        else
        {
            start_i = 0;
        }

        if (boundsPoints[by]>=0)
        {
            start_j = boundsPoints[by];
        }
        else
        {
            start_j = 0;
        }

        // Qt rasterization
        vtkPoints *pointsArray = RoiList->at(stack)->GetPoints();
        
        QPolygonF polygon;
        for(int i=0; i<nbPoints; i++)
        {
            polygon << QPointF(pointsArray->GetPoint(i)[x],
                               pointsArray->GetPoint(i)[y]);
        }

        QPainterPath path;
        path.addPolygon(polygon);

        QBrush brush;
        brush.setColor(Qt::white);
        brush.setStyle(Qt::SolidPattern);

        QImage imagePolygon(dim[x], dim[y], QImage::Format_RGB16);
        imagePolygon.fill(Qt::black);
        QPainter painter;
        painter.begin(&imagePolygon);
        painter.fillPath(path, brush);
        
        if (m == Keep)
        {
            for(i = 0; i<dim[x]; i++)
            {
                for(j = 0; j<dim[y]; j++)
                {
                    index[x] = i;
                    index[y] = j;
                    index[z] = stack;
                    if (i >= start_i && i <= end_i)
                    {
                        if (j >= start_j && j <= end_j)
                        {
                            QRgb val = imagePolygon.pixel(i, j);
                            if (val != qRgb(255, 255, 255))
                            {
                                clonedImage->SetPixel(index, valOfOutside);
                            }
                        }
                        else
                        {
                            clonedImage->SetPixel(index, valOfOutside);
                        }
                    }
                    else
                    {
                        clonedImage->SetPixel(index, valOfOutside);
                    }
                }
            }
        }
        if (m == Remove)
        {
            for(i = start_i; i<=end_i; i++)
            {
                for(j = start_j; j<=end_j; j++)
                {
                    index[x] = i;
                    index[y] = j;
                    index[z] = stack;
                    QRgb val = imagePolygon.pixel(i, j);
                    if (val == qRgb(255, 255, 255))
                    {
                        clonedImage->SetPixel(index, valOfOutside);
                    }
                }
            }
        }
    }

    d->resultData = medAbstractDataFactory::instance()->createSmartPointer(d->input->identifier());
    d->resultData->setData(clonedImage);

    medUtilities::setDerivedMetaData(d->resultData, d->input, "");

    disconnect(d->currentView, SIGNAL(orientationChanged()), this, SLOT(adaptWidgetsToOrientationChange()));
    this->saveRenderingParameters();
    d->currentView->insertLayer(d->layerInput, d->resultData);
    d->currentView->removeLayer(d->layerInput + 1);
    this->applyRenderingParameters();
    d->mode3DParam->setValue("VR");
    d->orientation3DParam->setValue(true);
    view3D->Render();
    connect(d->currentView, SIGNAL(orientationChanged()), this, SLOT(adaptWidgetsToOrientationChange()), Qt::UniqueConnection);

    d->saveImageButton->setEnabled(true);
}

void voiCutterToolBox::fillOutputMetaData()
{
    medUtilities::setDerivedMetaData(d->resultData, d->currentView->layerData(d->currentView->currentLayer()), "voiCutting");
}
