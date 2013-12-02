/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkVectorVisuManager.h"

#include <vtkProperty.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkRenderWindow.h>

#include <vtkGlyph3D.h>

#include <sstream>
#include <assert.h>
#include <cmath>

#include <stdio.h>
#include <vtkCellData.h>
#include <vtkFloatArray.h>

vtkCxxRevisionMacro(vtkVectorVisuManager, "$Revision: 1112 $");
vtkStandardNewMacro(vtkVectorVisuManager);

void vtkVectorVisuManagerCallback::Execute ( vtkObject *caller, unsigned long, void* )
{
  // get the box widget
  vtkBoxWidget *widget = vtkBoxWidget::SafeDownCast (caller);

  // get the inputed vtkPolyData
  vtkUnstructuredGrid* input = (vtkUnstructuredGrid*)widget->GetInput();
  if( !input )
  {
    std::cout<<"ouch !..."<<std::endl;
    
    return;
  }
  
  // Get the poly data defined by the vtkBoxWidget
  vtkPolyData* myBox = vtkPolyData::New();
  widget->GetPolyData(myBox);
  
  // myBox contains 15 points and points 8 to 13
  // define the bounding box
  double xmin, xmax, ymin, ymax, zmin, zmax;
  double* pt = myBox->GetPoint(8);
  xmin = pt[0];
  pt = myBox->GetPoint(9);
  xmax = pt[0];
  pt = myBox->GetPoint(10);
  ymin = pt[1];
  pt = myBox->GetPoint(11);
  ymax = pt[1];
  pt = myBox->GetPoint(12);
  zmin = pt[2];
  pt = myBox->GetPoint(13);
  zmax = pt[2];

  this->Box->SetBounds(xmin, xmax, ymin, ymax, zmin, zmax);  
  this->VectorLimiter->Update();

  myBox->Delete();  
}



vtkVectorVisuManager::vtkVectorVisuManager()
{
  this->Input = 0;
  this->RWin = 0;
  this->BoxWidget    = vtkBoxWidget::New();
  this->Callback     = vtkVectorVisuManagerCallback::New();
  this->Mapper       = vtkDataSetMapper::New();
  this->Actor        = vtkActor::New();
  this->VectorFilter = vtkGlyph3D::New();
  this->VectorFilter->SetScaleModeToScaleByVector();
  this->VectorFilter->SetColorModeToColorByScalar();
  this->VectorFilter->SetScaleFactor (5.0);
  this->RenderingMode = RENDER_IS_POLYLINES;

  this->BoxWidget->RotationEnabledOff();
  this->BoxWidget->SetPlaceFactor (1.0);
  this->BoxWidget->AddObserver (vtkCommand::InteractionEvent, this->Callback);
  this->BoxWidgetVisibility = true;

  this->BoxWidget->GetOutlineProperty()->SetColor (0.3,0.3,1.0);
  this->BoxWidget->GetHandleProperty()->SetColor (0.8,0.8,1.0);

  this->Actor->SetMapper (this->Mapper);  
  this->Actor->GetProperty()->SetLineWidth (3.0);
  
}


vtkVectorVisuManager::~vtkVectorVisuManager()
{
  this->BoxWidget->RemoveObserver (this->Callback);
  this->BoxWidget->SetInteractor (NULL);

  if (this->RWin)
  {

    
//     vtkRenderer* first_renderer = this->RWin->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    vtkRenderer* first_renderer = this->RWin->FindPokedRenderer(
				this->RWin->GetLastEventPosition()[0],
				this->RWin->GetLastEventPosition()[1]);
    if (first_renderer)
    {
      first_renderer->RemoveActor(this->GetActor());
    }
  }
  this->BoxWidget->Delete();
  this->Callback->Delete();
  this->Mapper->Delete();
  this->Actor->Delete();
  this->VectorFilter->Delete();
  if (this->RWin)
  {
    this->RWin->Delete();
  }
  if (this->Input)
    this->Input->Delete();
  
  
}


void vtkVectorVisuManager::ProvideColorsToVectors()
{
  if(!this->Input)
    return;

  vtkFloatArray* vectors = vtkFloatArray::SafeDownCast (this->Input->GetPointData()->GetVectors());

  if (!vectors)
    return;

  vtkUnsignedCharArray* colors = vtkUnsignedCharArray::New();
  colors->SetNumberOfComponents (3);
  colors->SetName("vector_colors");
  
  int npts = 0;

  npts = this->Input->GetNumberOfPoints();

  
  for( int i=0; i<npts; i++)
  {
    float vector[3];
    vectors->GetTupleValue (i, vector);
    double norm = sqrt (vector[0]*vector[0]+
			vector[1]*vector[1]+
			vector[2]*vector[2]);
    vector[0] /= norm;
    vector[1] /= norm;
    vector[2] /= norm;
    
    unsigned char color[3];
    for( unsigned int j=0; j<3; j++)
      color[j] = (unsigned char)( fabs (vector[j])*255.0 );
    
    colors->InsertNextTupleValue( color );
  }
  
  this->Input->GetPointData()->AddArray (colors);
//   if (this->Input->GetPointData()->GetScalars() && this->Input->GetPointData()->GetScalars()->GetName())
//     this->ScalarName = this->Input->GetPointData()->GetScalars()->GetName();

  this->Input->GetPointData()->SetActiveScalars(colors->GetName());
//   this->Input->GetPointData()->SetScalars (colors);
//   std::cout<<"setting colors"<<std::endl;
  
  colors->Delete();
}


void vtkVectorVisuManager::NormalizeVectors()
{
  bool is_in_points = true;
  
  vtkFloatArray* vectors = vtkFloatArray::SafeDownCast (this->Input->GetPointData()->GetVectors());

  if (!vectors)
    return;

  int npts = 0;
  if (is_in_points)
    npts = this->Input->GetNumberOfPoints();
  else
    npts = this->Input->GetNumberOfCells();
    
  for( int i=0; i<npts; i++)
  {
    float vector[3];
    float v2[3];
    
    vectors->GetTupleValue (i, vector);
    double norm = sqrt (vector[0]*vector[0]+
			vector[1]*vector[1]+
			vector[2]*vector[2]);
    v2[0] = vector[0] / norm;
    v2[1] = vector[1] / norm;
    v2[2] = vector[2] / norm;
    
    vectors->SetTupleValue (i, v2);
    
  }



}




void vtkVectorVisuManager::SetInputInternal(vtkDataSet* input)
{

  
  if( !input )
    return;
  if( !input->GetNumberOfPoints() )
    return;
  vtkPointSet* pointset = vtkPointSet::SafeDownCast (input);
  if( !pointset )
    return;
  
  
  if (this->Input == input)
  {
    return;
  }
  
  if (this->Input)
  {
    this->Input->Delete();
    this->Input = NULL;
  }

  this->Input =  input;
  this->Input->Register(this);
  
//   if (vtkPolyData::SafeDownCast (input))
//     this->Input = vtkPolyData::New();
//   else if (vtkUnstructuredGrid::SafeDownCast (input))
//     this->Input = vtkUnstructuredGrid::New();
//   else
//   {
//     vtkWarningMacro (<<"wrong dataset format !"<<endl);
//     return;
//   }
//   this->Input->DeepCopy (input);
//   vtkPointSet* newpointset = vtkPointSet::SafeDownCast (this->Input);
//   newpointset->SetPoints(pointset->GetPoints());
  

  if (this->Input)
  {
    this->Initialize();
  }
  
}


void vtkVectorVisuManager::SetInput(vtkDataSet* input)
{

  this->SetInputInternal (input);

  if(!this->GetInput())
    return;  
  
  this->ParseVectorFromCellsToPoints();
  this->ProvideColorsToVectors();
//   this->NormalizeVectors();

  this->Callback->GetVectorLimiter()->SetInput ( this->GetInput() );
  
  this->BoxWidget->SetInput ( this->GetInput() );
  this->BoxWidget->PlaceWidget();

  this->Callback->Execute (this->BoxWidget, 0, NULL);  

  vtkFloatArray* vectors = vtkFloatArray::SafeDownCast (this->Input->GetPointData()->GetVectors());
  if (vectors)
  {
    this->VectorFilter->SetInput(this->Callback->GetOutput());
    this->VectorFilter->Update();
    this->Mapper->SetInput (this->VectorFilter->GetOutput());
  }
  else
  {
    this->Mapper->SetInput (this->Callback->GetOutput());
  }

//   if (this->ScalarName.size() > 2)
//     this->Input->GetPointData()->SetActiveScalars(this->ScalarName.c_str());
//   else
//     this->Mapper->SetScalarVisibility(false);
  
    
    
  if (this->RWin)
  {  
//     vtkRenderer* first_renderer = this->RWin->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    vtkRenderer* first_renderer = this->RWin->FindPokedRenderer(
				this->RWin->GetLastEventPosition()[0],
				this->RWin->GetLastEventPosition()[1]);
    

    if (first_renderer)
    {
      first_renderer->AddActor(this->GetActor());
    }
  }
  
}


void vtkVectorVisuManager::SwapInputOutput()
{
}


void vtkVectorVisuManager::Reset()
{
  if( !this->GetInput() ) return;

  this->BoxWidget->SetInput ( this->GetInput() );
  this->Callback->GetVectorLimiter()->SetInput ( this->Callback->GetVectorLimiter()->GetOutput() );
  this->Callback->Execute (this->BoxWidget, 0, NULL);
}


void vtkVectorVisuManager::SetVisibility (bool isVisible)
{
  this->Actor->SetVisibility (isVisible);
  if (isVisible)
    this->SetBoxWidget (this->BoxWidgetVisibility);
}

void vtkVectorVisuManager::Initialize()
{
  if (this->Callback->GetVectorLimiter()->GetOutput())
    this->Callback->GetVectorLimiter()->GetOutput()->Initialize();
  this->Callback->GetVectorLimiter()->RemoveAllInputs();

  this->VectorFilter->GetOutput()->Initialize();
  
}


void vtkVectorVisuManager::ParseVectorFromCellsToPoints()
{
  vtkFloatArray* vectors = vtkFloatArray::SafeDownCast (this->Input->GetPointData()->GetVectors());
  if (vectors)
    return;
  
  vectors = vtkFloatArray::SafeDownCast (this->Input->GetCellData()->GetVectors());
  
  if (!vectors)
    return;

  vtkFloatArray* newvectors = vtkFloatArray::New();
  
  newvectors->SetNumberOfComponents (3);
  std::ostringstream os;
  if (vectors->GetName())
    os << vectors->GetName();
  os << "(copy)";
  
  newvectors->SetName(os.str().c_str());
  
  int npts = this->Input->GetNumberOfPoints();
    
  for( int pointid=0; pointid<npts; pointid++)
  {
    vtkIdList* list = vtkIdList::New();
    this->Input->GetPointCells (pointid, list);
    float vector[3] = {0.0, 0.0, 0.0};
    for (int j=0; j<list->GetNumberOfIds(); j++)
    {
      int cellid = list->GetId (j);
      float t_vector[3];
      vectors->GetTupleValue (cellid, t_vector);
      vector[0]+=t_vector[0];
      vector[1]+=t_vector[1];
      vector[2]+=t_vector[2];
    }
    newvectors->InsertNextTupleValue( vector );
    list->Delete();
  }

  this->Input->GetPointData()->SetVectors (newvectors);
  newvectors->Delete();
  
}
