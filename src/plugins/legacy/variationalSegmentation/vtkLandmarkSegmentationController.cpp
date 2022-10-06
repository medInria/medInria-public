/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <itkBinaryThresholdImageFilter.h>
#include <itkResampleImageFilter.h>

#include <vtkActor.h>
#include <vtkCellPicker.h>
#include <vtkCollection.h>
#include <vtkContourFilter.h>
#include <vtkFloatArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkLandmarkSegmentationController.h>
#include <vtkLandmarkWidget.h>
#include <vtkMatrix4x4.h>
#include <vtkMatrixToLinearTransform.h>
#include <vtkPointData.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindowInteractor.h>

vtkStandardNewMacro(vtkLandmarkSegmentationController)

//----------------------------------------------------------------------------
vtkLandmarkSegmentationControllerCommand::vtkLandmarkSegmentationControllerCommand()
{
    this->Controller = nullptr;
}
//----------------------------------------------------------------------------
vtkLandmarkSegmentationControllerCommand::~vtkLandmarkSegmentationControllerCommand()
{
    if (this->Controller)
    {
        this->Controller->UnRegister(this);
    }
}

//----------------------------------------------------------------------------
void vtkLandmarkSegmentationControllerCommand::SetController (vtkLandmarkSegmentationController *arg)
{
    if (arg == this->Controller)
    {
        return;
    }
    if (this->Controller)
    {
        this->Controller->UnRegister(this);
    }
    this->Controller = arg;
    if (this->Controller)
    {
        this->Controller->Register(this);
    }
}

//----------------------------------------------------------------------------
void vtkLandmarkSegmentationControllerCommand::Execute ( vtkObject *caller, unsigned long event, void *vtkNotUsed(callData))
{
    // if we are in 3D mode we block all interactions, IMO it is too difficult to add or manipulate sphere widgets in 3d.
    if (!this->Controller)
    {
        return;
    }

    if (!this->Controller->GetInteractorCollection())
    {
        return;
    }

    if ( ( (event == vtkCommand::RightButtonPressEvent)  ||
           (event == vtkCommand::MiddleButtonPressEvent) ||
           (event == vtkCommand::LeftButtonPressEvent) ) )
    {
        /**************  Landmark addition  **************/

        /*************************************************/
        /**  First we find the 3D position of the click  */
        /*************************************************/
        vtkRenderWindowInteractor* rwi = vtkRenderWindowInteractor::SafeDownCast(caller);
        if (! rwi->GetShiftKey() )
        {
            return;
        }
        int X, Y;
        X = rwi->GetEventPosition()[0];
        Y = rwi->GetEventPosition()[1];
        vtkRenderer *renderer = rwi->FindPokedRenderer(X,Y);
        if (!renderer)
        {
            return;
        }
        vtkCellPicker *pointpicker = vtkCellPicker::New();
        pointpicker->Pick(X,Y,0.0, renderer);
        vtkDataSet *dataset = pointpicker->GetDataSet();
        if (!dataset)
        {
            pointpicker->Delete();
            return;
        }
        vtkPoints *points = pointpicker->GetPickedPositions();
        if (!points || !points->GetNumberOfPoints())
        {
            pointpicker->Delete();
            return;
        }
        double *position = points->GetPoint ((vtkIdType)0);

        /*************************************************/
        /**  Second we check the type of landmark to add */
        /*************************************************/
        int type = 0;
        switch(event)
        {
            case vtkCommand::RightButtonPressEvent:
            {
                if (rwi->GetControlKey())
                {
                    type = 1;
                }
                else
                {
                    type = -1;
                }
                break;
            }
            case vtkCommand::LeftButtonPressEvent:
                type = 0;
                break;
            case vtkCommand::MiddleButtonPressEvent:
            default:
                type = 1;
                break;
        }

        /*************************************************/
        /**  Third we add the landmark and invoke event  */
        /*************************************************/
        if (!this->Controller->getMode3D() && !(this->Controller->getModeMPR() && this->Controller->GetInteractorCollection()->GetItemAsObject(3)==rwi ))
        {
            vtkLandmarkWidget *initial_landmark = this->Controller->AddConstraint(position, type);
            initial_landmark->InvokeEvent (vtkCommand::PlacePointEvent);
            return;
        }
    }

    vtkLandmarkWidget *landmark = vtkLandmarkWidget::SafeDownCast (caller);
    if (landmark && (event == vtkCommand::PlacePointEvent || event == vtkCommand::DeleteEvent))
    {
        /**************  Landmark deletion  **************/
        if (event == vtkCommand::DeleteEvent)
        {
            landmark->SetToDelete(true);
            landmark->Off();
            landmark->GetWidget2D()->Off();
            this->Controller->GetLandmarkCollection()->RemoveItem(landmark);
        }

        this->Controller->RefreshConstraints();
        this->Controller->Update();

        this->Controller->GetInteractorCollection()->InitTraversal();
        vtkRenderWindowInteractor *interactor = vtkRenderWindowInteractor::SafeDownCast (this->Controller->GetInteractorCollection()->GetNextItemAsObject());
        while(interactor)
        {
            interactor->Render();
            interactor = vtkRenderWindowInteractor::SafeDownCast (this->Controller->GetInteractorCollection()->GetNextItemAsObject());
        }
    }
}

//----------------------------------------------------------------------------
vtkLandmarkSegmentationController::vtkLandmarkSegmentationController()
{
    this->SetNumberOfInputPorts(0);

    m_Input                    = nullptr;
    implicitFunction           = nullptr;
    this->InteractorCollection = nullptr;
    this->Enabled              = 0;

    m_Filter                 = FilterType::New();
    m_Converter              = ConverterType::New();
    this->SurfaceExtractor   = vtkContourFilter::New();
    this->Transformer        = vtkMatrixToLinearTransform::New();
    this->LandmarkCollection = vtkCollection::New();
    this->Command            = vtkLandmarkSegmentationControllerCommand::New();
    this->Mapper             = vtkPolyDataMapper::New();
    this->Actor              = vtkActor::New();
    this->TotalLandmarkCollection = vtkCollection::New();
    this->Command->SetController (this);
    m_Converter->SetInput (m_Filter->GetOutput());

    vtkImageData *tmp = vtkImageData::New();
    this->SurfaceExtractor->SetInputData(tmp);
    this->SurfaceExtractor->SetValue (0, 0.0);
    this->SurfaceExtractor->Update();

    this->SetOutput (this->SurfaceExtractor->GetOutput());
    this->Mapper->SetInputData(this->SurfaceExtractor->GetOutput());
    this->Actor->SetMapper (this->Mapper);

    this->Views2D = nullptr;
    this->Views3D = nullptr;
    this->mode3D = false; // useful in single view mode
    this->modeMPR = false; // if this is true we do not care about mode3D.
    this->LandmarkRadius = 1.5;

    tmp->Delete();
}

//----------------------------------------------------------------------------
vtkLandmarkSegmentationController::~vtkLandmarkSegmentationController()
{
    this->Transformer->Delete();
    this->SurfaceExtractor->Delete();
    this->LandmarkCollection->Delete();
    this->Command->Delete();
    if (this->InteractorCollection)
    {
        this->InteractorCollection->UnRegister(this);
    }
    this->Mapper->Delete();
    this->Actor->Delete();
    this->TotalLandmarkCollection->Delete();
}

//----------------------------------------------------------------------------
void vtkLandmarkSegmentationController::PrintSelf(ostream &os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkLandmarkSegmentationController::SetInteractorCollection (vtkCollection *arg)
{
    if (arg == this->InteractorCollection)
    {
        return;
    }
    if (this->InteractorCollection)
    {
        this->InteractorCollection->UnRegister(this);
    }
    this->InteractorCollection = arg;
    if (this->InteractorCollection)
    {
        this->InteractorCollection->Register(this);
    }
    this->SetEnabled (this->GetEnabled());
}

//----------------------------------------------------------------------------
void vtkLandmarkSegmentationController::SetInput(ImageType::Pointer input)
{
    m_Input = input;
    m_Filter->SetInput (m_Input);

    ImageType::DirectionType directions = input->GetDirection();
    ImageType::PointType origin = input->GetOrigin();
    vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
    matrix->Identity();

    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            matrix->SetElement (i, j, directions (i,j));
        }
    }
    double v_origin[4], v_origin2[4];
    for (int i=0; i<3; i++)
    {
        v_origin[i] = origin[i];
    }
    v_origin[3] = 1.0;
    matrix->MultiplyPoint (v_origin, v_origin2);
    for (int i=0; i<3; i++)
    {
        matrix->SetElement (i, 3, v_origin[i]-v_origin2[i]);
    }
    this->Transformer->SetInput (matrix);
    matrix->Delete();
    this->SurfaceExtractor->SetInputData(m_Converter->GetOutput());
    this->Modified();
    double minspacing = std::min(input->GetSpacing()[0], std::min(input->GetSpacing()[1], input->GetSpacing()[2]));
    this->LandmarkRadius = minspacing / 2.0;
}

//----------------------------------------------------------------------------
void vtkLandmarkSegmentationController::SetConstraints (ConstraintListType arg)
{
    this->Constraints = arg;
    m_Filter->SetConstraints (this->Constraints);
    if (this->GetInteractorCollection())
    {
        for (unsigned int i=0; i<this->Constraints.size(); i++)
        {
            ConstraintType c = this->Constraints[i];
            this->AddConstraint (c.first.GetDataPointer(), c.second);
        }
        this->RefreshConstraints();
    }
    this->Modified();
}

void vtkLandmarkSegmentationController::SetConstraints (vtkPointSet *arg)
{
    if (!arg->GetPoints()->GetNumberOfPoints() || !arg->GetPointData()->GetScalars())
    {
        vtkErrorMacro (<<"vtkLandmarkSegmentationController::SetConstraints(): argument empty");
        return;
    }
    ConstraintListType constraints;
    for (unsigned int i=0; i<arg->GetPoints()->GetNumberOfPoints(); i++)
    {
        ConstraintType c;
        c.first = arg->GetPoint (i);
        c.second = arg->GetPointData()->GetScalars()->GetComponent (i,0);
        constraints.push_back (c);
    }
    this->SetConstraints (constraints);
}

//----------------------------------------------------------------------------
vtkLandmarkWidget* vtkLandmarkSegmentationController::AddConstraint (double *pos, int type)
{
    if (!this->InteractorCollection)
    {
        return nullptr;
    }
    this->GetInteractorCollection()->InitTraversal();
    vtkRenderWindowInteractor *item = vtkRenderWindowInteractor::SafeDownCast (this->GetInteractorCollection()->GetNextItemAsObject());
    int cpt = 0;

    vtkLandmarkWidget *initial_landmark = nullptr;
    while(item)
    {
        vtkLandmarkWidget *l = vtkLandmarkWidget::New();
        vtkPointHandleRepresentation2D *pointRep = vtkPointHandleRepresentation2D::New();
        pointRep->SetWorldPosition(pos);
        pointRep->BuildRepresentation();

        l->ScaleOff();
        l->SetCenter (pos);
        l->SetRadius (this->LandmarkRadius);
        l->SetValue (type);
        l->SetView2D(Views2D->at(cpt));
        l->SetView3D(Views3D->at(cpt));

        if (type == 1)
        {
            l->GetSphereProperty()->SetColor (1,0,0);
            pointRep->GetProperty()->SetColor(1,0,0);
        }
        if (type == 0)
        {
            l->GetSphereProperty()->SetColor (1,1,0);
            pointRep->GetProperty()->SetColor(1,1,0);
        }
        if (type ==-1)
        {
            l->GetSphereProperty()->SetColor (0,1,0);
            pointRep->GetProperty()->SetColor(0,1,0);
        }
        l->GetWidget2D()->SetRepresentation(pointRep);
        if (!mode3D)
        {
            l->GetWidget2D()->SetInteractor(item);
        }
        int indices[3];
        Views2D->at(cpt)->GetImageCoordinatesFromWorldCoordinates(pos,indices);
        l->SetIndices(indices);
        
        l->SetRepresentationToSurface();
        l->SetInteractor (item);
        
        if (item == vtkRenderWindowInteractor::SafeDownCast (this->GetInteractorCollection()->GetItemAsObject(3)))
        {
            l->On();
            l->GetWidget2D()->Off();
        }
        else
        {
            l->GetWidget2D()->On();
        }

        this->GetTotalLandmarkCollection()->AddItem (l);

        if (!initial_landmark)
        {
            initial_landmark = l;
        }
        else
        {
            initial_landmark->AddBrothers(l);
            l->SetBigBrother(initial_landmark);
        }
        l->Delete();
        item = vtkRenderWindowInteractor::SafeDownCast (this->GetInteractorCollection()->GetNextItemAsObject());
        cpt++;
    }
    this->LandmarkCollection->AddItem(initial_landmark);
    this->LinkInteractions();
    return initial_landmark;
}

//----------------------------------------------------------------------------
bool vtkLandmarkSegmentationController::RemoveConstraint (vtkLandmarkWidget *arg)
{
    if (arg->GetToDelete())
    {
        arg->RemoveAllObservers();
        this->TotalLandmarkCollection->RemoveItem(arg);
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------
void vtkLandmarkSegmentationController::RefreshConstraints()
{
    this->Constraints.clear();
    for (int i=0; i<this->LandmarkCollection->GetNumberOfItems(); i++)
    {
        vtkLandmarkWidget *landmark = vtkLandmarkWidget::SafeDownCast (this->LandmarkCollection->GetItemAsObject(i));
        FilterType::ConstraintType c;
        FilterType::PointType p (landmark->GetCenter());
        c.first = p;
        c.second = landmark->GetValue();
        this->Constraints.push_back (c);
    }
    m_Filter->SetConstraints (this->Constraints);
    this->Modified();
}

//----------------------------------------------------------------------------
void vtkLandmarkSegmentationController::GetLandmarkSet(vtkPolyData *arg)
{
    vtkPoints *pts = vtkPoints::New();
    vtkFloatArray *values = vtkFloatArray::New();
    this->LandmarkCollection->InitTraversal();
    vtkLandmarkWidget *landmark = vtkLandmarkWidget::SafeDownCast (this->LandmarkCollection->GetNextItemAsObject());
    while(landmark)
    {
        pts->InsertNextPoint (landmark->GetCenter());
        values->InsertNextValue (landmark->GetValue());
        landmark = vtkLandmarkWidget::SafeDownCast (this->LandmarkCollection->GetNextItemAsObject());
    }
    arg->SetPoints (pts);
    arg->GetPointData()->SetScalars (values);
    pts->Delete();
    values->Delete();
}

//----------------------------------------------------------------------------
void vtkLandmarkSegmentationController::SetEnabled (unsigned int arg)
{
    this->Enabled = arg;

    if (this->InteractorCollection)
    {
        this->InteractorCollection->InitTraversal();

        vtkRenderWindowInteractor *interactor = vtkRenderWindowInteractor::SafeDownCast (this->InteractorCollection->GetNextItemAsObject());
        while(interactor)
        {
            if (arg)
            {
                if (!interactor->HasObserver (vtkCommand::CharEvent, this->Command) )
                {
                    interactor->AddObserver(vtkCommand::CharEvent, this->Command, 0);
                }
                if (!interactor->HasObserver (vtkCommand::LeftButtonPressEvent, this->Command) )
                {
                    interactor->AddObserver(vtkCommand::LeftButtonPressEvent, this->Command, 0);
                }
                if (!interactor->HasObserver (vtkCommand::MiddleButtonPressEvent, this->Command) )
                {
                    interactor->AddObserver(vtkCommand::MiddleButtonPressEvent, this->Command, 0);
                }
                if (!interactor->HasObserver (vtkCommand::RightButtonPressEvent, this->Command) )
                {
                    interactor->AddObserver(vtkCommand::RightButtonPressEvent, this->Command, 0);
                }
            }
            else
            {
                interactor->RemoveObserver(this->Command);
            }
            interactor = vtkRenderWindowInteractor::SafeDownCast (this->InteractorCollection->GetNextItemAsObject());
        }
    }
    this->LandmarkCollection->InitTraversal();
    vtkLandmarkWidget *landmark = vtkLandmarkWidget::SafeDownCast (this->LandmarkCollection->GetNextItemAsObject());

    while(landmark)
    {
        landmark->SetEnabled(arg);
        landmark = vtkLandmarkWidget::SafeDownCast (this->LandmarkCollection->GetNextItemAsObject());
    }
}

//----------------------------------------------------------------------------
void vtkLandmarkSegmentationController::LinkInteractions ()
{ 
    vtkLandmarkWidget *l1 = nullptr;

    unsigned int numberofinteractors = this->GetInteractorCollection()->GetNumberOfItems();
    vtkCollection *collection        = this->GetTotalLandmarkCollection();
    unsigned int numberoflandmarks   = collection->GetNumberOfItems() / numberofinteractors;

    for (unsigned int n = 0; n < numberoflandmarks; n++)
    {
        for (unsigned int i=0; i<numberofinteractors; i++)
        {
            int id1 = numberofinteractors * n + i;
            l1 = vtkLandmarkWidget::SafeDownCast (collection->GetItemAsObject(id1));

            if (!l1->HasObserver(vtkCommand::PlacePointEvent, this->Command))
            {
                l1->AddObserver(vtkCommand::PlacePointEvent, this->Command, -1);
            }
            if (!l1->HasObserver(vtkCommand::DeleteEvent, this->Command))
            {
                l1->AddObserver(vtkCommand::DeleteEvent, this->Command, -1);
            }
        }
    }
}

void vtkLandmarkSegmentationController::setViews2D(QList<vtkImageView2D*> * views)
{
    this->Views2D = views;
    if (views->size()>1)
    {
        this->modeMPR = true;
    }
}

void vtkLandmarkSegmentationController::setViews3D(QList<vtkImageView3D*> *views)
{
    this->Views3D = views;
    if (views->size()>1)
    {
        this->modeMPR = true;
    }
}

//----------------------------------------------------------------------------
int vtkLandmarkSegmentationController::RequestData(
        vtkInformation *vtkNotUsed(request),
        vtkInformationVector **inputVector,
        vtkInformationVector *outputVector)
{
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    m_Filter->Update();
    implicitFunction = m_Filter->GetOutput();
    m_Converter->Update();

    this->SurfaceExtractor->Update();

    vtkPolyData *data = this->SurfaceExtractor->GetOutput();
    vtkPoints *newpoints = vtkPoints::New();
    if (data->GetPoints())
    {
        this->Transformer->TransformPoints (data->GetPoints(), newpoints);
    }
    data->SetPoints (newpoints);
    newpoints->Delete();
    output->SetPoints (data->GetPoints());
    output->SetPolys (data->GetPolys());

    return 1;
}

vtkLandmarkSegmentationController::binaryType::Pointer vtkLandmarkSegmentationController::GetBinaryImage()
{
    if (!implicitFunction)
    {
        return nullptr;
    }

    itk::ResampleImageFilter<ImageType , ImageType>::Pointer filter = itk::ResampleImageFilter<ImageType , ImageType>::New();
    ImageType::SizeType size;
    size[0] = outputSize[0];
    size[1] = outputSize[1];
    size[2] = outputSize[2];
    ImageType::SpacingType outputSpacing;

    ImageType::SizeType filterInputSize = implicitFunction->GetLargestPossibleRegion().GetSize();
    outputSpacing[0] = implicitFunction->GetSpacing()[0] * (static_cast<double>(filterInputSize[0]-1) / static_cast<double>(outputSize[0]));
    outputSpacing[1] = implicitFunction->GetSpacing()[1] * (static_cast<double>(filterInputSize[1]-1) / static_cast<double>(outputSize[1]));
    outputSpacing[2] = implicitFunction->GetSpacing()[2] * (static_cast<double>(filterInputSize[2]-1) / static_cast<double>(outputSize[2]));

    filter->SetInput(implicitFunction);
    filter->SetSize(size);
    filter->SetOutputOrigin(m_Input->GetOrigin());
    filter->SetOutputDirection(m_Input->GetDirection());
    filter->SetOutputSpacing(outputSpacing);
    filter->UpdateLargestPossibleRegion();

    itk::BinaryThresholdImageFilter<ImageType,binaryType>::Pointer thresholdFilter = itk::BinaryThresholdImageFilter<ImageType,binaryType>::New();
    thresholdFilter->SetInput(filter->GetOutput());
    thresholdFilter->SetUpperThreshold(0);
    thresholdFilter->SetInsideValue(1);
    thresholdFilter->Update();
    binaryType::Pointer img = thresholdFilter->GetOutput();

    return img;
}

void vtkLandmarkSegmentationController::DeleteAllLandmarks()
{
    this->GetLandmarkCollection()->InitTraversal();
    vtkLandmarkWidget *toRemove = vtkLandmarkWidget::SafeDownCast(this->GetLandmarkCollection()->GetNextItemAsObject());
    while(toRemove)
    {
        toRemove->RemoveAllObservers();
        toRemove->cleanUpLittleBrothersReferences();
        toRemove = vtkLandmarkWidget::SafeDownCast (this->GetLandmarkCollection()->GetNextItemAsObject());
    }
    
    this->GetLandmarkCollection()->RemoveAllItems();

    this->GetTotalLandmarkCollection()->InitTraversal();
    toRemove = vtkLandmarkWidget::SafeDownCast(this->GetTotalLandmarkCollection()->GetNextItemAsObject());
    while(toRemove)
    {
        toRemove->RemoveAllObservers();
        toRemove = vtkLandmarkWidget::SafeDownCast (this->GetTotalLandmarkCollection()->GetNextItemAsObject());
    }

    this->GetTotalLandmarkCollection()->RemoveAllItems();
}
