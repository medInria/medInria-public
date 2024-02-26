/*=========================================================================

 medInria

 Copyright (c) INRIA 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "medResliceViewer.h"
#include "resampleProcess.h"

#include <itkVTKImageToImageFilter.h>

#include <medAbstractDataFactory.h>
#include <medAbstractLayeredView.h>
#include <medDoubleParameterL.h>
#include <medSliderSpinboxPair.h>
#include <medTransform.h>
#include <medUtilities.h>
#include <medUtilitiesITK.h>
#include <medVtkViewBackend.h>

#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkImageReslice.h>
#include <vtkImageSlabReslice.h>
#include <vtkMatrix4x4.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkResliceCursor.h>
#include <vtkResliceCursorActor.h>
#include <vtkResliceCursorPolyDataAlgorithm.h>
#include <vtkResliceCursorThickLineRepresentation.h>
#include <vtkResliceCursorWidget.h>
#include <vtkTransform.h>

class medResliceCursorCallback : public vtkCommand
{
public:
    static medResliceCursorCallback *New()
    {
        return new medResliceCursorCallback;
    }

    void Execute(vtkObject *caller, unsigned long ev, void *callData )
    {
        switch (ev)
        {
            case vtkResliceCursorWidget::ResliceAxesChangedEvent:
            {
                reformatViewer->ensureOrthogonalPlanes();
                break;
            }
            case vtkResliceCursorWidget::ResetCursorEvent:
            {
                reformatViewer->resetViews();
                reformatViewer->applyRadiologicalConvention();
                break;
            }
        }

        vtkImagePlaneWidget *ipw = dynamic_cast< vtkImagePlaneWidget* >(caller);
        if (ipw)
        {
            double *wl = static_cast<double*>(callData);

            if (ipw == reformatViewer->getImagePlaneWidget(0))
            {
                reformatViewer->getImagePlaneWidget(1)->SetWindowLevel(wl[0],wl[1],1);
                reformatViewer->getImagePlaneWidget(2)->SetWindowLevel(wl[0],wl[1],1);
            }
            else if(ipw == reformatViewer->getImagePlaneWidget(1))
            {
                reformatViewer->getImagePlaneWidget(0)->SetWindowLevel(wl[0],wl[1],1);
                reformatViewer->getImagePlaneWidget(2)->SetWindowLevel(wl[0],wl[1],1);
            }
            else if (ipw == reformatViewer->getImagePlaneWidget(2))
            {
                reformatViewer->getImagePlaneWidget(0)->SetWindowLevel(wl[0],wl[1],1);
                reformatViewer->getImagePlaneWidget(1)->SetWindowLevel(wl[0],wl[1],1);
            }
        }

        vtkResliceCursorWidget *rcw = dynamic_cast<vtkResliceCursorWidget*>(caller);
        if (rcw)
        {
            vtkResliceCursorLineRepresentation *rep = dynamic_cast<vtkResliceCursorLineRepresentation*>(rcw->GetRepresentation());
            // Although the return value is not used, we keep the get calls
            // in case they had side-effects
            rep->GetResliceCursorActor()->GetCursorAlgorithm()->GetResliceCursor();

            for (int i = 0; i < 3; i++)
            {
                vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(reformatViewer->getImagePlaneWidget(i)->GetPolyDataAlgorithm());
                ps->SetOrigin(reformatViewer->getResliceImageViewer(i)->GetResliceCursorWidget()
                              ->GetResliceCursorRepresentation()->GetPlaneSource()->GetOrigin());
                ps->SetPoint1(reformatViewer->getResliceImageViewer(i)->GetResliceCursorWidget()
                              ->GetResliceCursorRepresentation()->GetPlaneSource()->GetPoint1());
                ps->SetPoint2(reformatViewer->getResliceImageViewer(i)->GetResliceCursorWidget()
                              ->GetResliceCursorRepresentation()->GetPlaneSource()->GetPoint2());

                // If the reslice plane has modified, update it on the 3D widget
                reformatViewer->getImagePlaneWidget(i)->UpdatePlacement();
            }
        }

        // Render everything
        for (int i = 0; i < 3; i++)
        {
            reformatViewer->getResliceImageViewer(i)->GetResliceCursorWidget()->Render();
        }
        reformatViewer->getImagePlaneWidget(0)->GetInteractor()->GetRenderWindow()->Render();
    }

    ~medResliceCursorCallback()
    {
        reformatViewer = nullptr;
    }

    medResliceViewer *reformatViewer;
};

medResliceViewer::medResliceViewer(medAbstractView *view, QWidget *parent): medAbstractView(parent)
{
    if (!view)
    {
        return;
    }

    view3d    = static_cast<medVtkViewBackend*>(view->backend())->view3D;
    inputData = static_cast<medAbstractLayeredView*>(view)->layerData(view3d->GetCurrentLayer());

    selectedView = 2;

    int *imageDims = view3d->GetMedVtkImageInfo()->dimensions;

    // Copy view information before it's destroyed, allowing to use outputSpacingOrSize later.
    double *spacing = view3d->GetMedVtkImageInfo()->spacing;
    memcpy(outputSpacingOrSize.data(), spacing, 3*sizeof(double));

    viewBody = new QWidget(parent);

    // Copy of the current data. We're going to work on it
    vtkViewData = vtkSmartPointer<vtkImageData>::New();
    vtkViewData->DeepCopy(view3d->GetInputAlgorithm(view3d->GetCurrentLayer())->GetOutput());

    // Build reslice viewers
    for (int i = 0; i < 3; i++)
    {
        riw[i] = vtkSmartPointer<vtkResliceImageViewer>::New();
        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        riw[i]->SetRenderWindow(renderWindow);
        riw[i]->GetRenderer()->SetBackground(0,0,0); // black background
    }
    riw[selectedView]->GetRenderer()->SetBackground(0.3,0,0);

    // Build views
    for (int i = 0; i < 4; i++)
    {
        views[i] = new QVTKOpenGLWidget();
        views[i]->setEnableHiDPI(true);
        views[i]->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        views[i]->installEventFilter(this);
    }

    // Position of the new views in tab
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(views[2], 0, 0);
    gridLayout->addWidget(views[3], 0, 1);
    gridLayout->addWidget(views[1], 1, 0);
    gridLayout->addWidget(views[0], 1, 1);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnMinimumWidth(0, imageDims[0]);
    gridLayout->setColumnMinimumWidth(1, imageDims[0]);
    gridLayout->setRowMinimumHeight(0, imageDims[1]);
    gridLayout->setRowMinimumHeight(1, imageDims[1]);
    gridLayout->setRowStretch(0, 0);
    gridLayout->setRowStretch(1, 0);
    viewBody->setLayout(gridLayout);

    // Share render windows and interactors
    views[0]->SetRenderWindow(riw[0]->GetRenderWindow());
    riw[0]->SetupInteractor(views[0]->GetRenderWindow()->GetInteractor());

    views[1]->SetRenderWindow(riw[1]->GetRenderWindow());
    riw[1]->SetupInteractor(views[1]->GetRenderWindow()->GetInteractor());

    views[2]->SetRenderWindow(riw[2]->GetRenderWindow());
    riw[2]->SetupInteractor(views[2]->GetRenderWindow()->GetInteractor());

    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    views[3]->SetRenderWindow(renderWindow);
    views[3]->GetRenderWindow()->AddRenderer(ren);

    // Make them all share the same reslice cursor object.
    for (int i = 0; i < 3; i++)
    {
        vtkResliceCursorLineRepresentation *rep = vtkResliceCursorLineRepresentation::SafeDownCast(
                    riw[i]->GetResliceCursorWidget()->GetRepresentation());
        riw[i]->SetResliceCursor(riw[selectedView]->GetResliceCursor());

        rep->GetResliceCursorActor()->GetCursorAlgorithm()->SetReslicePlaneNormal(i);

        riw[i]->SetInputData(vtkViewData);
        riw[i]->SetSliceOrientation(i);
        riw[i]->SetResliceModeToOblique();
    }

    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.005);

    vtkSmartPointer<vtkProperty> ipwProp = vtkSmartPointer<vtkProperty>::New();

    vtkRenderWindowInteractor *iren = views[3]->GetInteractor();

    // Build planes on views
    for (int i = 0; i < 3; i++)
    {
        planeWidget[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();
        planeWidget[i]->SetInteractor(iren);
        planeWidget[i]->SetPicker(picker);
        planeWidget[i]->RestrictPlaneToVolumeOn();

        // Plane colors
        double color[3] = {0, 0, 0};
        color[i] = 1;
        planeWidget[i]->GetPlaneProperty()->SetColor(color);

        planeWidget[i]->SetTexturePlaneProperty(ipwProp);
        planeWidget[i]->TextureInterpolateOff();
        planeWidget[i]->SetResliceInterpolateToLinear();
        planeWidget[i]->SetInputData(vtkViewData);
        planeWidget[i]->SetPlaneOrientation(i);
        planeWidget[i]->SetSliceIndex(imageDims[i]/2);
        planeWidget[i]->DisplayTextOn();
        planeWidget[i]->SetDefaultRenderer(ren);
        planeWidget[i]->On();
        planeWidget[i]->InteractionOn();
    }

    vtkSmartPointer<medResliceCursorCallback> cbk = vtkSmartPointer<medResliceCursorCallback>::New();
    cbk->reformatViewer = this;

    for (int i = 0; i < 3; i++)
    {
        riw[i]->GetResliceCursorWidget()->AddObserver(vtkResliceCursorWidget::ResliceAxesChangedEvent, cbk);
        riw[i]->GetResliceCursorWidget()->AddObserver(vtkResliceCursorWidget::WindowLevelEvent, cbk);
        riw[i]->GetResliceCursorWidget()->AddObserver(vtkResliceCursorWidget::ResliceThicknessChangedEvent, cbk);
        riw[i]->GetResliceCursorWidget()->AddObserver(vtkResliceCursorWidget::ResetCursorEvent, cbk);
        riw[i]->GetInteractorStyle()->AddObserver(vtkCommand::WindowLevelEvent, cbk);
        riw[i]->GetInteractorStyle()->AddObserver(vtkCommand::MouseMoveEvent, cbk);

        // Make them all share the same color map.
        riw[i]->SetLookupTable(riw[selectedView]->GetLookupTable());
        riw[i]->SetColorLevel(view3d->GetColorLevel());
        riw[i]->SetColorWindow(view3d->GetColorWindow());
        planeWidget[i]->GetColorMap()->SetLookupTable(riw[selectedView]->GetLookupTable());
        planeWidget[i]->SetColorMap(riw[i]->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetColorMap());
    }

    resetViews();
    applyRadiologicalConvention();
    updatePlaneNormals();

    planeWidget[0]->GetCurrentRenderer()->ResetCamera();
    planeWidget[0]->GetCurrentRenderer()->GetActiveCamera()->Azimuth(180);
    planeWidget[0]->GetCurrentRenderer()->GetActiveCamera()->Roll(180);

    views[0]->show();
    views[1]->show();
    views[2]->show();

    this->initialiseNavigators();
}

medResliceViewer::~medResliceViewer()
{
    for (int i = 0; i < 3; i++)
    {
        riw[i] = nullptr;
        planeWidget[i] = nullptr;
    }
    vtkViewData = nullptr;
    inputData = nullptr;
}

QString medResliceViewer::identifier() const
{
    return "medResliceViewer";
}

void medResliceViewer::setToolBox(resliceToolBox *tlbx)
{
    reformaTlbx = tlbx;
}

void medResliceViewer::thickMode(int val)
{
    for (int i = 0; i < 3; i++)
    {
        riw[i]->SetThickMode(val);
        riw[i]->GetRenderer()->ResetCamera();
        riw[i]->Render();
    }
}

void medResliceViewer::blendMode(int val)
{
    if (val)
    {
        SetBlendModeToMinIP();
    }
}

void medResliceViewer::SetBlendMode(int m)
{
    for (int i = 0; i < 3; i++)
    {
        vtkImageSlabReslice *thickSlabReslice = vtkImageSlabReslice::SafeDownCast(
                    vtkResliceCursorThickLineRepresentation::SafeDownCast(
                        riw[i]->GetResliceCursorWidget()->GetRepresentation())->GetReslice());
        thickSlabReslice->SetBlendMode(m);
        riw[i]->Render();
    }
}

void medResliceViewer::SetBlendModeToMaxIP()
{
    this->SetBlendMode(VTK_IMAGE_SLAB_MAX);
}

void medResliceViewer::SetBlendModeToMinIP()
{
    this->SetBlendMode(VTK_IMAGE_SLAB_MIN);
}

void medResliceViewer::SetBlendModeToMeanIP()
{
    this->SetBlendMode(VTK_IMAGE_SLAB_MEAN);
}

void medResliceViewer::reset()
{
    resetViews();
}

void medResliceViewer::resetViews()
{
    for (int i = 0; i < 3; i++)
    {
        riw[i]->GetRenderer()->ResetCamera();
    }

    riw[0]->GetRenderer()->GetActiveCamera()->SetViewUp(0, 0, 1);
    riw[1]->GetRenderer()->GetActiveCamera()->SetViewUp(0, 0, 1);
    riw[2]->GetRenderer()->GetActiveCamera()->SetViewUp(0, -1, 0);
}

void medResliceViewer::render()
{
    for (int i = 0; i < 3; i++)
    {
        riw[i]->Render();
        views[i]->GetRenderWindow()->Render();
    }
}

void medResliceViewer::saveImage()
{
    vtkSmartPointer<vtkMatrix4x4> resliceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    calculateResliceMatrix(resliceMatrix);

    vtkImageReslice *reslicerTop = vtkImageReslice::New();
    reslicerTop->SetInputData(vtkViewData);
    reslicerTop->AutoCropOutputOn();
    reslicerTop->SetResliceAxes(resliceMatrix);
    reslicerTop->SetBackgroundLevel(riw[0]->GetInput()->GetScalarRange()[0]);
    reslicerTop->SetInterpolationModeToLinear();

    // Apply resampling in mm
    if (reformaTlbx->findChild<QComboBox*>("bySpacingOrSize")->currentText() == "Spacing")
    {
        reslicerTop->SetOutputSpacing(outputSpacingOrSize.data());
    }
    reslicerTop->Update();

    // Apply orientation changes
    switch (vtkViewData->GetScalarType())
    {
        case VTK_CHAR:
            generateOutput<char>(reslicerTop, "itkDataImageChar3");
            break;
        case VTK_UNSIGNED_CHAR:
            generateOutput<unsigned char>(reslicerTop, "itkDataImageUChar3");
            break;
        case VTK_SHORT:
            generateOutput<short>(reslicerTop, "itkDataImageShort3");
            break;
        case VTK_UNSIGNED_SHORT:
            generateOutput<unsigned short>(reslicerTop, "itkDataImageUShort3");
            break;
        case VTK_INT:
            generateOutput<int>(reslicerTop, "itkDataImageInt3");
            break;
        case VTK_UNSIGNED_INT:
            generateOutput<unsigned int>(reslicerTop, "itkDataImageUInt3");
            break;
        case VTK_LONG:
            generateOutput<long>(reslicerTop, "itkDataImageLong3");
            break;
        case VTK_UNSIGNED_LONG:
            generateOutput<unsigned long>(reslicerTop, "itkDataImageULong3");
            break;
        case VTK_FLOAT:
            generateOutput<float>(reslicerTop, "itkDataImageFloat3");
            break;
        case VTK_DOUBLE:
            generateOutput<double>(reslicerTop, "itkDataImageDouble3");
            break;
    }

    emit imageReformatedGenerated();
}

void medResliceViewer::askedSpacingOrSizeChange(double val)
{
    medDoubleParameterL * doubleParam = qobject_cast<medDoubleParameterL*>(QObject::sender());
    auto spinBoxSender = doubleParam? doubleParam->getSpinBox() : nullptr;
    if (spinBoxSender)
    {
        auto changeFormat = reformaTlbx->findChild<QComboBox*>("bySpacingOrSize")->currentText();
        auto accessibleName = spinBoxSender->accessibleName();

        if (changeFormat == "Spacing")
        {
            double x, y, z;
            riw[2]->GetResliceCursor()->GetThickness(x,y,z);

            // The three windows share the same reslice cursor
            if (accessibleName == "SpacingOrSizeX")
            {
                riw[0]->GetResliceCursor()->SetThickness(val,y,z);
            }
            else if (accessibleName == "SpacingOrSizeY")
            {
                riw[0]->GetResliceCursor()->SetThickness(x,val,z);
            }
            else if (accessibleName == "SpacingOrSizeZ")
            {
                riw[0]->GetResliceCursor()->SetThickness(x,y,val);
            }
        }

        // For spacing or size changes
        if (accessibleName == "SpacingOrSizeX")
        {
            outputSpacingOrSize[0] = val;
        }
        else if (accessibleName == "SpacingOrSizeY")
        {
            outputSpacingOrSize[1] = val;
        }
        else if (accessibleName == "SpacingOrSizeZ")
        {
            outputSpacingOrSize[2] = val;
        }
    }
}

void medResliceViewer::extentChanged(int val)
{
    Q_UNUSED(val)
    medSliderSpinboxPair *pairSender = qobject_cast<medSliderSpinboxPair*>(QObject::sender());

    if (pairSender)
    {
        if (pairSender->accessibleName() == "fromSlice")
        {
            fromSlice = pairSender->value();
        }
        if (pairSender->accessibleName() == "toSlice")
        {
            toSlice = pairSender->value();
        }
    }
}

bool medResliceViewer::eventFilter(QObject *object, QEvent *event)
{
    if (!qobject_cast<QVTKOpenGLWidget*>(object))
    {
        return true;
    }

    if (event->type() == QEvent::FocusIn)
    {
        for(int i=0; i<3; i++)
        {
            riw[i]->GetRenderer()->SetBackground(0,0,0);
            if (views[i]==object)
            {
                selectedView = i;
            }
        }
        riw[selectedView]->GetRenderer()->SetBackground(0.3,0,0);
    }
    
    return false;
}

vtkResliceImageViewer* medResliceViewer::getResliceImageViewer(int i)
{
    return riw[i];
}

vtkImagePlaneWidget* medResliceViewer::getImagePlaneWidget(int i)
{
    return planeWidget[i];
}

dtkSmartPointer<medAbstractData> medResliceViewer::getOutput()
{
    if (!outputData)
    {
        blockSignals(true);
        saveImage();
        blockSignals(false);
    }
    return outputData;
}

void medResliceViewer::applyRadiologicalConvention()
{
    double normal[3];

    getResliceImageViewer(0)->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetResliceCursor()->GetPlane(2)->GetNormal(normal);
    for (int i = 0; i < 3; i++)
    {
        normal[i] = -normal[i];
    }
    getResliceImageViewer(0)->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetResliceCursor()->GetPlane(2)->SetNormal(normal);
    getResliceImageViewer(0)->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetResliceCursor()->Update();
    for (int i = 0; i < 3; i++)
    {
        getResliceImageViewer(i)->GetResliceCursorWidget()->Render();
    }
    getImagePlaneWidget(0)->GetInteractor()->GetRenderWindow()->Render();
}

void medResliceViewer::calculateResliceMatrix(vtkMatrix4x4* result)
{
    double resliceX[3], resliceY[3], resliceZ[3], resliceOrigin[3], outputOrigin[3];
    double *outputX, *outputY, *outputZ;

    riw[0]->GetResliceCursor()->GetXAxis(resliceX);
    riw[0]->GetResliceCursor()->GetYAxis(resliceY);
    riw[0]->GetResliceCursor()->GetZAxis(resliceZ);
    riw[0]->GetResliceCursor()->GetCenter(resliceOrigin);

    switch (selectedView)
    {
        case 0:
        {
            outputX = resliceY;
            outputY = resliceZ;
            outputZ = resliceX;
            outputOrigin[0] = resliceOrigin[1];
            outputOrigin[1] = resliceOrigin[2];
            outputOrigin[2] = resliceOrigin[0];
            break;
        }
        case 1:
        {
            outputX = resliceZ;
            outputY = resliceX;
            outputZ = resliceY;
            outputOrigin[0] = resliceOrigin[2];
            outputOrigin[1] = resliceOrigin[0];
            outputOrigin[2] = resliceOrigin[1];
            break;
        }
        default:
        {
            outputX = resliceX;
            outputY = resliceY;
            outputZ = resliceZ;
            outputOrigin[0] = resliceOrigin[0];
            outputOrigin[1] = resliceOrigin[1];
            outputOrigin[2] = resliceOrigin[2];
            break;
        }
    }

    result->Identity();
    for (int i = 0; i < 3; i++)
    {
        result->SetElement(i, 0, outputX[i]);
        result->SetElement(i, 1, outputY[i]);
        result->SetElement(i, 2, outputZ[i]);
        result->SetElement(i, 3, outputOrigin[i]);
    }

    adjustResliceMatrixToViewUp(result);
}

void medResliceViewer::adjustResliceMatrixToViewUp(vtkMatrix4x4* resliceMatrix)
{
    double dotX, dotY, X[3], Y[3], viewUp[3];

    for (int i = 0; i < 3; i++)
    {
        X[i] = resliceMatrix->GetElement(i, 0);
        Y[i] = resliceMatrix->GetElement(i, 1);
    }

    riw[selectedView]->GetRenderer()->GetActiveCamera()->GetViewUp(viewUp);

    dotX = vtkMath::Dot(viewUp, X);
    dotY = vtkMath::Dot(viewUp, Y);

    if (std::abs(dotY) > std::abs(dotX))
    {
        if (dotY < 0)
        {
            for (int i = 0; i < 3; i++)
            {
                resliceMatrix->SetElement(i, 0, -X[i]);
                resliceMatrix->SetElement(i, 1, -Y[i]);
            }
        }
    }
    else
    {
        if (dotX < 0)
        {
            for (int i = 0; i < 3; i++)
            {
                resliceMatrix->SetElement(i, 0, Y[i]);
                resliceMatrix->SetElement(i, 1, -X[i]);
            }
        }
        else
        {
            for (int i = 0; i < 3; i++)
            {
                resliceMatrix->SetElement(i, 0, -Y[i]);
                resliceMatrix->SetElement(i, 1, X[i]);
            }
        }
    }
}

void medResliceViewer::updatePlaneNormals()
{
    for (int i = 0; i < 3; i++)
    {
        double *normal = getResliceImageViewer(0)->GetResliceCursor()->GetPlane(i)->GetNormal();
        std::copy(normal, normal + 3, planeNormal[i]);
    }
}

// The two fixed planes must be moved so that they stay orthogonal to the moving plane
void medResliceViewer::ensureOrthogonalPlanes()
{
    int movingPlaneIndex = findMovingPlaneIndex();
    vtkPlane *movingPlane = getResliceImageViewer(0)->GetResliceCursor()->GetPlane(movingPlaneIndex);
    vtkPlane *fixedPlane1 = getResliceImageViewer(0)->GetResliceCursor()->GetPlane((movingPlaneIndex + 1) % 3);
    vtkPlane *fixedPlane2 = getResliceImageViewer(0)->GetResliceCursor()->GetPlane((movingPlaneIndex + 2) % 3);

    makePlaneOrthogonalToOtherPlanes(fixedPlane1, movingPlane, fixedPlane2);
    makePlaneOrthogonalToOtherPlanes(fixedPlane2, movingPlane, fixedPlane1);
    updatePlaneNormals();
}

// The plane who's normal has changed the most is the currently moving plane.
int medResliceViewer::findMovingPlaneIndex()
{
    int movingPlaneIndex = 0;
    double maxNormalDifference = -1;

    for (int i = 0; i < 3; i++)
    {
        vtkPlane *plane = getResliceImageViewer(0)->GetResliceCursor()->GetPlane(i);
        double *currentNormal = plane->GetNormal();
        double normalDifference = 0;
        for (int j = 0; j < 3; j++)
        {
            normalDifference += pow(planeNormal[i][j] - currentNormal[j], 2);
        }
        if (normalDifference > maxNormalDifference)
        {
            maxNormalDifference = normalDifference;
            movingPlaneIndex = i;
        }
    }

    return movingPlaneIndex;
}

void medResliceViewer::makePlaneOrthogonalToOtherPlanes(vtkPlane* targetPlane, vtkPlane* plane1, vtkPlane* plane2)
{
    double idealNormal[3];

    vtkMath::Cross(plane1->GetNormal(), plane2->GetNormal(), idealNormal);
    vtkMath::Normalize(idealNormal);

    // Make sure the ideal normal is on the same side as the current normal
    if (vtkMath::Dot(targetPlane->GetNormal(), idealNormal) < 0)
    {
        for (int i = 0; i < 3; i++)
        {
            idealNormal[i] = -idealNormal[i];
        }
    }
    targetPlane->SetNormal(idealNormal);
}

template <typename DATA_TYPE>
void medResliceViewer::generateOutput(vtkImageReslice* reslicer, QString destType)
{
    typedef itk::Image<DATA_TYPE, 3> ImageType;

    typedef itk::VTKImageToImageFilter<ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput(reslicer->GetOutput());
    filter->Update();

    outputData = medAbstractDataFactory::instance()->createSmartPointer(destType);
    outputData->setData(filter->GetOutput());

    // Apply resampling in pix
    if (reformaTlbx->findChild<QComboBox*>("bySpacingOrSize")->currentText() == "Size")
    {
        applyResamplingPix();
    }

    typename ImageType::Pointer outputImage = static_cast<itk::Image<DATA_TYPE, 3>*>(outputData->data());

    compensateForRadiologicalView<DATA_TYPE>(outputImage);
    correctOutputTransform<DATA_TYPE>(outputImage, reslicer->GetResliceAxes());

    // Final output image
    outputData->setData(outputImage);

    medUtilities::setDerivedMetaData(outputData, inputData, "reformatted");
    medUtilitiesITK::updateMetadata<ImageType>(outputData);
}

void medResliceViewer::applyResamplingPix()
{
    resampleProcess *resamplePr = new resampleProcess();
    resamplePr->setInput(outputData);
    resamplePr->setParameter(outputSpacingOrSize[0], 0);
    resamplePr->setParameter(outputSpacingOrSize[1], 1);
    resamplePr->setParameter(outputSpacingOrSize[2], 2);
    resamplePr->update();

    outputData = resamplePr->output();
    delete resamplePr;
}

template <typename DATA_TYPE>
void medResliceViewer::compensateForRadiologicalView(itk::Image<DATA_TYPE, 3>* outputImage)
{
    vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    medTransform::getTransform(*outputImage, *transformMatrix);
    for (int i = 0; i < 3; i++)
    {
        transformMatrix->SetElement(i, 1, -transformMatrix->GetElement(i, 1));
        transformMatrix->SetElement(i, 2, -transformMatrix->GetElement(i, 2));
    }
    medTransform::setTransform(*outputImage, *transformMatrix);
}

/*
 * The new image "contains" a rotated version of the initial image.
 * The corrective transformation is composed of the following:
 * 1. Inverse of the output image transformation. This places the image with it's origin at (0, 0, 0)
 * and its axes aligned with the world axes.
 * 2. Translate the image so that its center is at (0, 0, 0). This places the center of rotation at
 * the center of the image.
 * 3. Rotate the image by the reslice rotation. This aligns the initial image with the world axes.
 * 4. Translate the image so the initial origin is at (0, 0, 0).
 * 5. Apply the initial transformation (the one it had before the reslice)
*/
template <typename DATA_TYPE>
void medResliceViewer::correctOutputTransform(itk::Image<DATA_TYPE, 3>* outputImage, vtkMatrix4x4* resliceMatrix)
{
    typedef itk::Image<DATA_TYPE, 3> ImageType;
    typename ImageType::Pointer inputImage = static_cast<ImageType*>(inputData->data());

    vtkSmartPointer<vtkMatrix4x4> inputTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkSmartPointer<vtkMatrix4x4> outputInverseTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    medTransform::getTransform<DATA_TYPE>(*inputImage, *inputTransformMatrix);
    medTransform::getTransform<DATA_TYPE>(*outputImage, *outputInverseTransformMatrix);
    outputInverseTransformMatrix->Invert();

    double inputCenter[3], outputCenter[3];
    getImageCenterInLocalSpace<DATA_TYPE>(inputImage, inputCenter);
    getImageCenterInLocalSpace<DATA_TYPE>(outputImage, outputCenter);

    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->PostMultiply();
    transform->Concatenate(outputInverseTransformMatrix);
    transform->Translate(-outputCenter[0], -outputCenter[1], -outputCenter[2]);
    transform->Concatenate(getResliceRotationMatrix(resliceMatrix));
    transform->Translate(inputCenter[0], inputCenter[1], inputCenter[2]);
    transform->Concatenate(inputTransformMatrix);

    medTransform::applyTransform<DATA_TYPE>(*outputImage, *transform->GetMatrix());
}

template <typename DATA_TYPE>
void medResliceViewer::getImageCenterInLocalSpace(itk::Image<DATA_TYPE, 3>* image, double center[3])
{
    typedef itk::Image<DATA_TYPE, 3> ImageType;
    typename ImageType::SizeType size;
    typename ImageType::SpacingType spacing;

    size = image->GetLargestPossibleRegion().GetSize();
    spacing = image->GetSpacing();

    for (int i = 0; i < 3; i++)
    {
        center[i] = 0.5 * (size[i] - 1) * spacing[i];
    }
}

vtkSmartPointer<vtkMatrix4x4> medResliceViewer::getResliceRotationMatrix(vtkMatrix4x4* resliceMatrix)
{
    vtkSmartPointer<vtkMatrix4x4> resliceRotationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

    resliceRotationMatrix->DeepCopy(resliceMatrix);
    for (int i = 0; i < 3; i++)
    {
        resliceRotationMatrix->SetElement(i, 3, 0);
    }

    return resliceRotationMatrix;
}

QString medResliceViewer::description() const
{
    return QString("");
}

medViewBackend * medResliceViewer::backend() const
{
    return nullptr;
}

QWidget* medResliceViewer::navigatorWidget()
{
    return nullptr;
}

QWidget *medResliceViewer::viewWidget()
{
    return viewBody;
}

QWidget *medResliceViewer::mouseInteractionWidget()
{
    return nullptr;
}

QImage medResliceViewer::buildThumbnail(const QSize &size)
{
    return QImage();
}
