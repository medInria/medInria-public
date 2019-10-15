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
#include <medSliderSpinboxPair.h>
#include <medUtilities.h>
#include <medUtilitiesITK.h>
#include <medVtkViewBackend.h>
#include <mscTransform.h>

#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkImageReslice.h>
#include <vtkImageSlabReslice.h>
#include <vtkMatrix4x4.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
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
                reformatViewer->ensureOrthogonalPlanes();
                break;
            case vtkResliceCursorWidget::ResetCursorEvent:
                reformatViewer->resetViews();
                reformatViewer->applyRadiologicalConvention();
                break;
        }

        vtkImagePlaneWidget *ipw = dynamic_cast< vtkImagePlaneWidget* >( caller );
        if (ipw)
        {
            double *wl = static_cast<double*>( callData );

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
                ps->SetOrigin(reformatViewer->getResliceImageViewer(i)->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetPlaneSource()->GetOrigin());
                ps->SetPoint1(reformatViewer->getResliceImageViewer(i)->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetPlaneSource()->GetPoint1());
                ps->SetPoint2(reformatViewer->getResliceImageViewer(i)->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetPlaneSource()->GetPoint2());

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

    medResliceCursorCallback() {}

    medResliceViewer *reformatViewer;
};

medResliceViewer::medResliceViewer(medAbstractView *view, QWidget *parent): medAbstractView(parent)
{
    if (!view)
    {
        return;
    }

    inputData = static_cast<medAbstractLayeredView*>(view)->layerData(0);
    view3d = static_cast<medVtkViewBackend*>(view->backend())->view3D;

    int *imageDims = view3d->GetMedVtkImageInfo()->dimensions;
    outputSpacing  = view3d->GetMedVtkImageInfo()->spacing;

    viewBody = new QWidget(parent);
    for (int i = 0; i < 3; i++)
    {
        riw[i] = vtkSmartPointer<vtkResliceImageViewer>::New();
        frames[i] = new QVTKFrame(viewBody);
        views[i] = frames[i]->getView();
        views[i]->setSizePolicy ( QSizePolicy::Minimum, QSizePolicy::Minimum );
        if (i==0)
        {
            frames[i]->setStyleSheet("* {border : 1px solid #FF0000;}");
        }
        else if (i==1)
        {
            frames[i]->setStyleSheet("* {border : 1px solid #00FF00;}");
        }
        else if (i==2)
        {
            frames[i]->setStyleSheet("* {border : 1px solid #0000FF;}");
        }
        views[i]->installEventFilter(this);
    }
    frames[3] = new QVTKFrame(viewBody);
    views[3] = frames[3]->getView();
    views[3]->setSizePolicy ( QSizePolicy::Minimum, QSizePolicy::Minimum );

    QGridLayout *gridLayout = new QGridLayout(parent);
    gridLayout->addWidget(frames[2],0,0);
    gridLayout->addWidget(frames[3],0,1);
    gridLayout->addWidget(frames[1],1,0);
    gridLayout->addWidget(frames[0],1,1);

    gridLayout->setColumnStretch ( 0, 0 );
    gridLayout->setColumnStretch ( 1, 0 );
    gridLayout->setRowStretch ( 0, 0 );
    gridLayout->setRowStretch ( 1, 0 );

    viewBody->setLayout(gridLayout);
    
    views[0]->SetRenderWindow(riw[0]->GetRenderWindow());
    riw[0]->SetupInteractor(views[0]->GetRenderWindow()->GetInteractor());

    views[1]->SetRenderWindow(riw[1]->GetRenderWindow());
    riw[1]->SetupInteractor(views[1]->GetRenderWindow()->GetInteractor());

    views[2]->SetRenderWindow(riw[2]->GetRenderWindow());
    riw[2]->SetupInteractor(views[2]->GetRenderWindow()->GetInteractor());

    for (int i = 0; i < 3; i++)
    {
        // make them all share the same reslice cursor object.

        vtkResliceCursorLineRepresentation *rep = vtkResliceCursorLineRepresentation::SafeDownCast(
                    riw[i]->GetResliceCursorWidget()->GetRepresentation());
        riw[i]->SetResliceCursor(riw[2]->GetResliceCursor());

        rep->GetResliceCursorActor()->GetCursorAlgorithm()->SetReslicePlaneNormal(i);

        riw[i]->SetInputConnection(view3d->GetInputAlgorithm(view3d->GetCurrentLayer())->GetOutputPort());
        riw[i]->SetSliceOrientation(i);
        riw[i]->SetResliceModeToOblique();
    }

    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.005);

    vtkSmartPointer<vtkProperty> ipwProp = vtkSmartPointer<vtkProperty>::New();

    vtkSmartPointer< vtkRenderer > ren = vtkSmartPointer< vtkRenderer >::New();

    views[3]->GetRenderWindow()->AddRenderer(ren);
    vtkRenderWindowInteractor *iren = views[3]->GetInteractor();

    for (int i = 0; i < 3; i++)
    {
        planeWidget[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();
        planeWidget[i]->SetInteractor( iren );
        planeWidget[i]->SetPicker(picker);
        planeWidget[i]->RestrictPlaneToVolumeOn();
        double color[3] = {0, 0, 0};
        color[i] = 1;
        planeWidget[i]->GetPlaneProperty()->SetColor(color);

        color[0] /= 4.0;
        color[1] /= 4.0;
        color[2] /= 4.0;
        riw[i]->GetRenderer()->SetBackground( 0,0,0 );

        planeWidget[i]->SetTexturePlaneProperty(ipwProp);
        planeWidget[i]->TextureInterpolateOff();
        planeWidget[i]->SetResliceInterpolateToLinear();
        planeWidget[i]->SetInputConnection(view3d->GetInputAlgorithm(view3d->GetCurrentLayer())->GetOutputPort());
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
        riw[i]->SetLookupTable(riw[2]->GetLookupTable());
        riw[i]->SetColorLevel(view3d->GetColorLevel());
        riw[i]->SetColorWindow(view3d->GetColorWindow());

        planeWidget[i]->GetColorMap()->SetLookupTable(riw[2]->GetLookupTable());
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

    selectedView = 2;

    this->initialiseNavigators();
}

medResliceViewer::~medResliceViewer()
{
    for (int i = 0; i < 3; i++)
    {
        riw[i] = nullptr;
        planeWidget[i] = nullptr;
    }
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
        //TODO: set the axes to the reslicecursorwidget vtkimageReslice .... this will propably solve the problem of disappearance.
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
    reslicerTop->SetInputConnection(view3d->GetInputAlgorithm(view3d->GetCurrentLayer())->GetOutputPort());
    reslicerTop->AutoCropOutputOn();
    reslicerTop->SetResliceAxes(resliceMatrix);
    reslicerTop->SetBackgroundLevel(riw[0]->GetInput()->GetScalarRange()[0]); // todo: set the background value in an automatic way.

    // Apply resampling in mm

    if (reformaTlbx->findChild<QComboBox*>("bySpacingOrDimension")->currentText() == "Spacing")
    {
        reslicerTop->SetOutputSpacing(outputSpacing);
    }
    reslicerTop->SetInterpolationModeToLinear();

    // Apply orientation changes
    switch (view3d->GetMedVtkImageInfo()->scalarType)
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

void medResliceViewer::thickSlabChanged(double val)
{
    QDoubleSpinBox *spinBoxSender = qobject_cast<QDoubleSpinBox*>(QObject::sender());

    if (spinBoxSender)
    {
        double x, y, z;
        riw[2]->GetResliceCursor()->GetThickness(x,y,z);

        if (spinBoxSender->accessibleName()=="SpacingX")
        {
            if (reformaTlbx->findChild<QComboBox*>("bySpacingOrDimension")->currentText() == "Spacing")
            {
                riw[0]->GetResliceCursor()->SetThickness(val,y,z); //the three windows share the same reslice cursor
            }
            outputSpacing[0] = val;
        }

        if (spinBoxSender->accessibleName()=="SpacingY")
        {
            if (reformaTlbx->findChild<QComboBox*>("bySpacingOrDimension")->currentText() == "Spacing")
            {
                riw[0]->GetResliceCursor()->SetThickness(x,val,z); //the three windows share the same reslice cursor
            }
            outputSpacing[1] = val;
        }

        if (spinBoxSender->accessibleName()=="SpacingZ")
        {
            if (reformaTlbx->findChild<QComboBox*>("bySpacingOrDimension")->currentText() == "Spacing")
            {
                riw[0]->GetResliceCursor()->SetThickness(x,y,val); //the three windows share the same reslice cursor
            }
            outputSpacing[2] = val;
        }
        this->render();
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
            if (views[i]==object)
            {
                if (i==0)
                {
                    frames[i]->setStyleSheet("QFrame {border : 5px solid #FF0000;}");
                }
                else if (i==1)
                {
                    frames[i]->setStyleSheet("QFrame {border : 5px solid #00FF00;}");
                }
                else if (i==2)
                {
                    frames[i]->setStyleSheet("QFrame {border : 5px solid #0000FF;}");
                }

                if (selectedView==0)
                {
                    frames[0]->setStyleSheet("* {border : 1px solid #FF0000;}");
                }
                else if (selectedView==1)
                {
                    frames[1]->setStyleSheet("* {border : 1px solid #00FF00;}");
                }
                else if (selectedView==2)
                {
                    frames[2]->setStyleSheet("* {border : 1px solid #0000FF;}");
                }
                selectedView = i;
            }
        }
        return false;
    }
    if (event->type() == QEvent::FocusOut)
    {
        return false;
    }
    return false;
}

vtkResliceImageViewer* medResliceViewer::getResliceImageViewer(int i)
{
    assert(0 <= i <= 3);

    return riw[i];
}

vtkImagePlaneWidget* medResliceViewer::getImagePlaneWidget(int i)
{
    assert(0 <= i <= 3);

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
    filter->GetOutput()->Update();

    outputData = medAbstractDataFactory::instance()->createSmartPointer(destType);
    outputData->setData(filter->GetOutput());

    // Apply resampling in pix
    if (reformaTlbx->findChild<QComboBox*>("bySpacingOrDimension")->currentText() == "Dimension")
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
    resamplePr->setParameter(outputSpacing[0],0);
    resamplePr->setParameter(outputSpacing[1],1);
    resamplePr->setParameter(outputSpacing[2],2);
    resamplePr->update();

    outputData = resamplePr->output();
    delete resamplePr;
}

template <typename DATA_TYPE>
void medResliceViewer::compensateForRadiologicalView(itk::Image<DATA_TYPE, 3>* outputImage)
{
    vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    mscTransform::getTransform(*outputImage, *transformMatrix);
    for (int i = 0; i < 3; i++)
    {
        transformMatrix->SetElement(i, 1, -transformMatrix->GetElement(i, 1));
        transformMatrix->SetElement(i, 2, -transformMatrix->GetElement(i, 2));
    }
    mscTransform::setTransform(*outputImage, *transformMatrix);
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
    mscTransform::getTransform<DATA_TYPE>(*inputImage, *inputTransformMatrix);
    mscTransform::getTransform<DATA_TYPE>(*outputImage, *outputInverseTransformMatrix);
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

    mscTransform::applyTransform<DATA_TYPE>(*outputImage, *transform->GetMatrix());
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
