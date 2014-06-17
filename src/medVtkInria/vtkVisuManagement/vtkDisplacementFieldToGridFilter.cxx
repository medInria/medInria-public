/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkDisplacementFieldToGridFilter.h>


#include "vtkAlgorithmOutput.h"
#include "vtkPolyData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include <vtkImageData.h>
#include <vtkPoints.h>
#include <vtkIdList.h>
#include <vtkDataArray.h>
#include <vtkCellArray.h>


vtkCxxRevisionMacro(vtkDisplacementFieldToGridFilter, "$Revision: 1 $");
vtkStandardNewMacro(vtkDisplacementFieldToGridFilter);

vtkDisplacementFieldToGridFilter::vtkDisplacementFieldToGridFilter()
{
  this->Resolution[0] = 16;
  this->Resolution[1] = 16;
  this->Resolution[2] = 16;
}

vtkDisplacementFieldToGridFilter::~vtkDisplacementFieldToGridFilter()
{
}

//----------------------------------------------------------------------------
// The default vtkImageAlgorithm semantics are that SetInput() puts
// each input on a different port, we want all the image inputs to
// go on the first port.
void vtkDisplacementFieldToGridFilter::SetInput(int idx, vtkDataObject *input)
{
  // Ask the superclass to connect the input.
  this->SetNthInputConnection(0, idx, (input ? input->GetProducerPort() : 0));
}

//----------------------------------------------------------------------------
vtkDataObject *vtkDisplacementFieldToGridFilter::GetInput(int idx)
{
  if (this->GetNumberOfInputConnections(0) <= idx)
    {
    return 0;
    }
  return vtkImageData::SafeDownCast(
    this->GetExecutive()->GetInputData(0, idx));
}


//----------------------------------------------------------------------------
int vtkDisplacementFieldToGridFilter::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *vtkNotUsed( outputVector ))
{

  
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkImageData *displacementgrid = static_cast<vtkImageData *>(inInfo->Get(vtkDataObject::DATA_OBJECT()));
    
  if (!displacementgrid)
    return 0;

  int N = displacementgrid->GetNumberOfScalarComponents();
  if (N != 3)
    return 0;
  
  return 1;
}


void vtkDisplacementFieldToGridFilter::FillRegularGridFromImage (vtkImageData* image, vtkPolyData* polydata, unsigned int resolution[3])
{

  double* spacing = image->GetSpacing();
  double* bounds = image->GetBounds();
  
  vtkPoints* points = vtkPoints::New();
  vtkCellArray* lines = vtkCellArray::New();

  vtkIdType ids[2];
  double point1[3];
  double point2[3];
  

  double xmin = bounds[0], xmax = bounds[1],
    ymin = bounds[2], ymax = bounds[3],
    zmin = bounds[4], zmax = bounds[5] ;

//   vtkIdList* idlist = vtkIdList::New();
//   idlist->SetNumberOfIds (2);
  
  
  for (unsigned int i = 0; i<this->Resolution[1]; i++)    
    for (unsigned int j = 0; j<this->Resolution[2]; j++)
    {
      double y = ymin + i * (ymax - ymin) / (this->Resolution[1] - 1);
      double z = zmin + j * (zmax - zmin) / (this->Resolution[2] - 1);
      
      for (double x = xmin; x <= xmax - spacing[0]; x += spacing[0])
      {
	point1[0]=x;
	point1[1]=y;
	point1[2]=z;	
	point2[0] =  point1[0] + spacing[0];
	point2[1] =  point1[1];
	point2[2] =  point1[2];
	
	ids[0] = points->InsertNextPoint (point1);
	ids[1] = points->InsertNextPoint (point2);
	
	lines->InsertNextCell ((vtkIdType)(2), ids);
      }
    }

  for (unsigned int i = 0; i<this->Resolution[0]; i++)    
    for (unsigned int j = 0; j<this->Resolution[2]; j++)
    {
      double x = xmin + i * (xmax - xmin) / (this->Resolution[0] - 1);
      double z = zmin + j * (zmax - zmin) / (this->Resolution[2] - 1);
      
      for (double y = ymin; y <= ymax - spacing[1]; y += spacing[1])
      {
	point1[0]=x;
	point1[1]=y;
	point1[2]=z;	
	point2[0] =  point1[0];
	point2[1] =  point1[1] + spacing[1];
	point2[2] =  point1[2];
	
	ids[0] = points->InsertNextPoint (point1);
	ids[1] = points->InsertNextPoint (point2);
	
	lines->InsertNextCell ((vtkIdType)(2), ids);
      }
    }

  for (unsigned int i = 0; i<this->Resolution[0]; i++)    
    for (unsigned int j = 0; j<this->Resolution[1]; j++)
    {
      double x = xmin + i * (xmax - xmin) / (this->Resolution[0] - 1);
      double y = ymin + j * (ymax - ymin) / (this->Resolution[1] - 1);
      
      for (double z = zmin; z <= zmax - spacing[2]; z += spacing[2])
      {
	point1[0]=x;
	point1[1]=y;
	point1[2]=z;	
	point2[0] =  point1[0];
	point2[1] =  point1[1];
	point2[2] =  point1[2] + spacing[2];
	
	ids[0] = points->InsertNextPoint (point1);
	ids[1] = points->InsertNextPoint (point2);
	
	lines->InsertNextCell ((vtkIdType)(2), ids);
      }
    }
  


  polydata->SetPoints (points);
  polydata->SetLines (lines);
//   idlist->Delete();
  
  lines->Delete();
  points->Delete();

}


//----------------------------------------------------------------------------
int vtkDisplacementFieldToGridFilter::RequestData(
  vtkInformation* request,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  vtkInformation* info = outputVector->GetInformationObject(0);
  vtkPolyData *outpolydata = static_cast<vtkPolyData *>(info->Get(vtkDataObject::DATA_OBJECT()));

  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkImageData *displacementgrid = static_cast<vtkImageData *>(inInfo->Get(vtkDataObject::DATA_OBJECT()));
    
  if (!displacementgrid || !displacementgrid->GetPointData()->GetScalars())
    return 0;

  this->FillRegularGridFromImage (displacementgrid, outpolydata, this->Resolution);

  vtkPoints* points = outpolydata->GetPoints();
  double point[3];
  double* displacement;
  vtkIdType pointid;
  
  for(int i=0; i<points->GetNumberOfPoints(); i++)
  {
    points->GetPoint (i, point);
    pointid = displacementgrid->FindPoint (point);
    if (pointid < 0)
      continue;
    
    displacement = displacementgrid->GetPointData()->GetScalars()->GetTuple3 (pointid);
    points->SetPoint (i,
		      point[0]+displacement[0],
		      point[1]+displacement[1],
		      point[2]+displacement[2]
		      );
  }
	 

  return 1;
}


//----------------------------------------------------------------------------
int vtkDisplacementFieldToGridFilter::FillInputPortInformation(int port, vtkInformation* info)
{
  if (port == 0)
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
  }
  return 1;
}

