/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkLimitFibersToROI.h"

#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkPolyData.h>
#include <vtkObjectFactory.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkUnsignedCharArray.h>

#include <vtkMath.h>

#include <algorithm>

#ifdef WIN32
#define snprintf sprintf_s
#endif


vtkStandardNewMacro(vtkLimitFibersToROI);

vtkLimitFibersToROI::vtkLimitFibersToROI()
{
  MaskImage = 0;
  for( unsigned int i=0; i<256; i++)
  {
    this->BooleanOperationVector[i] = 2;
  }
  this->DirectionMatrix = 0;
}

vtkLimitFibersToROI::~vtkLimitFibersToROI()
{
  if( this->MaskImage )
    this->MaskImage->Delete();

  if (this->DirectionMatrix)
    this->DirectionMatrix->Delete();
}


int vtkLimitFibersToROI::RequestData (vtkInformation *vtkNotUsed(request),
                                       vtkInformationVector **inputVector,
                                       vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  output->Initialize();
  output->Allocate();
  
  vtkPoints* points = input->GetPoints();
  output->SetPoints ( points );
  vtkDataArray* scalars = input->GetPointData()->GetScalars();
  output->GetPointData()->SetScalars ( scalars );

  for( int i=0; i<input->GetPointData()->GetNumberOfArrays(); i++)
  {
    output->GetPointData()->AddArray ( input->GetPointData()->GetArray (i) );
  }

  vtkUnsignedCharArray* allColors = vtkUnsignedCharArray::SafeDownCast ( input->GetCellData()->GetScalars() );

  vtkCellArray* lines = input->GetLines();
  if( lines==0 )
  {
    return 0;
  }
  
  if (MaskImage == 0)
  {
    output->SetLines (lines);
    if( allColors )
    {
      output->GetCellData()->SetScalars (allColors);
    }
    return 1;
  }


  vtkUnsignedCharArray* newColors = vtkUnsignedCharArray::New();
  newColors->SetNumberOfComponents (3);

  
  int* dim        = MaskImage->GetDimensions();
  double* origin  = MaskImage->GetOrigin();
  double* spacing = MaskImage->GetSpacing();

  /**
     Automatically extract labels (i.e. any scalar value except 0) contained
     in MaskImage.
  */
  std::vector<int> Labels;
  unsigned char*   MaskValues = (unsigned char*)MaskImage->GetScalarPointer();
  
  for (int i=0;i<MaskImage->GetNumberOfPoints();i++)
  {
    int val = (int)(*MaskValues);
    if (val>0)
    {
      bool IsInserted = false;
      for (unsigned int j=0; j<Labels.size(); j++)
      {
        if ( Labels[j]==val )
        {
          IsInserted = true;
          break;
        }
      }
      
      if ( !IsInserted )
      {
        Labels.push_back (val);
      }
    }

    ++MaskValues;
  }

  std::sort ( Labels.begin(), Labels.end() );
  unsigned int numLabels = static_cast<unsigned int>(Labels.size());
  
  vtkDebugMacro ( << "Number Of Valid ROIs: " << numLabels );



  // Test if the BooleanOperationVector size matches the max label
  if( Labels[numLabels-1]>255 )
  {
    vtkErrorMacro (<<"Error: The BoolearOperationVector variable has an incompatible length with the mask image.");
    newColors->Delete();
    return 0;
  }



  //reset the Scalar Pointer
  MaskValues= (unsigned char*)MaskImage->GetScalarPointer();

  
  /**
     Algorithm: For each fiber, we test the number of regions (i.e. labels)
     it passes through. Along with the BooleanOperationVector, we determine
     if the fiber should be retained or not.
  */
  
  lines->InitTraversal();
  
  vtkIdType  npts  = 0;
  vtkIdType* ptids = 0;
  vtkIdType   test = lines->GetNextCell (npts, ptids);
  int cellId = 0;

  char tmp[256];
  snprintf (tmp, 256, "%d ROIs to process.", (int)Labels.size());
  this->SetProgressText (tmp);

  if( !Labels.size() )
  {
    vtkWarningMacro( << "There is no label to process." );
    return 1;
  }
  
    
  this->UpdateProgress (0.0);

  int iterProgress = 0;
  int maxProgress  = lines->GetNumberOfCells();
  int step         = maxProgress/100;


  vtkMatrix4x4 *t_direction = vtkMatrix4x4::New();
  t_direction->Identity();
  if (this->DirectionMatrix)
  {
    vtkMatrix4x4::Invert (this->DirectionMatrix, t_direction);
  }
 
  
  // for All the Fibers 
  while ( test )
  {

    //bool found = false;

    std::vector<int> FiberRegions (numLabels, 0);
    
    // for All the points of the fiber
    for (int k=0; k<npts; k++)
    {
      double pt[4];
      points->GetPoint (ptids[k], pt);
      pt[3] = 1.0;
      
      int c[3];

      for (int i=0; i<3; i++)
      {
	pt[i] -= origin[i];
      }

      t_direction->MultiplyPoint (pt, pt);

      /*
	c[0] = (int)( vtkMath::Round( (pt[0]-origin[0])/spacing[0]) );
	c[1] = (int)( vtkMath::Round( (pt[1]-origin[1])/spacing[1]) );
	c[2] = (int)( vtkMath::Round( (pt[2]-origin[2])/spacing[2]) );
      */
      c[0] = (int)( vtkMath::Round( pt[0]/spacing[0] ));
      c[1] = (int)( vtkMath::Round( pt[1]/spacing[1] ));
      c[2] = (int)( vtkMath::Round( pt[2]/spacing[2] ));
      
      if( c[0]>=0 && c[0]<dim[0] &&
          c[1]>=0 && c[1]<dim[1] &&
          c[2]>=0 && c[2]<dim[2] )
      {
        
        int index = c[0] + c[1]*dim[0] + c[2]*dim[0]*dim[1];
        MaskValues += index;
        int maskVal = (int)(*MaskValues);
        MaskValues -= index;
        

        for( unsigned int i=0; i<numLabels; i++)
        {
          if( maskVal==Labels[i] )
          {
            FiberRegions[i]=1;
            break;
          }            
        }

        
      }
    }
    
    bool insert = true;

    // 0: nullptr
    // 1: NOT
    // 2: AND
    
    for( unsigned int i=0; i<numLabels; i++)
    {
      if( this->BooleanOperationVector[ Labels[i] ] > 0 )
      {
	if( this->BooleanOperationVector[ Labels[i] ] == 1 )
	{
	  insert = insert && !FiberRegions[i];
	}
	else
	{
	  insert = insert && FiberRegions[i];
	}
      }
    }
    
    
    if (insert)
    {
      output->InsertNextCell(VTK_POLY_LINE, npts, ptids);
      if( allColors )
      {
        unsigned char fiberColor[3];
        allColors->GetTypedTuple ( cellId, fiberColor );
        newColors->InsertNextValue (fiberColor[0]);
        newColors->InsertNextValue (fiberColor[1]);
        newColors->InsertNextValue (fiberColor[2]);
      }
      
    }
    
    test = lines->GetNextCell (npts, ptids);
    cellId++;
    
    iterProgress++;
    if( step>0 )
    {
      if ( !(iterProgress%step) )
      {
        this->UpdateProgress (double(iterProgress)/double(maxProgress));
      }
    }
    
  }

  if( allColors )
  {
    output->GetCellData()->SetScalars (newColors);
  }
  newColors->Delete();

  t_direction->Delete();
  
  this->UpdateProgress (1.0);
  

  return 1;
  
}

void vtkLimitFibersToROI::SetBooleanOperation(int id, int value)
{
    this->BooleanOperationVector[id] = value;
    this->Modified();
}
