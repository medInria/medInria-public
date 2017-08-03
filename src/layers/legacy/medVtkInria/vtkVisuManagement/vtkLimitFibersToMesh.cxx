/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkLimitFibersToMesh.h"

#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkObjectFactory.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkCellArray.h>
#include <vtkPolyLine.h>
#include <vtkCellData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkDelaunay3D.h>
#include <vtkUnstructuredGrid.h>


vtkStandardNewMacro(vtkLimitFibersToMesh);

vtkLimitFibersToMesh::vtkLimitFibersToMesh()
{
  this->Mesh = 0;
  this->BooleanOperation = 1;
  this->Mesher = vtkDelaunay3D::New();
  for (unsigned int i=0; i<6; i++)
    this->Bounds[i] = 0;
  
}

vtkLimitFibersToMesh::~vtkLimitFibersToMesh()
{
  this->Mesher->Delete();
}


void vtkLimitFibersToMesh::SetMesh (vtkDataSet* mesh)
{
  if (this->Mesh == mesh)
    return;

  if (this->Mesh)
  {
    this->Mesh->UnRegister (this);
  }

  this->Mesh = mesh;

  if (this->Mesh)
  {
    this->Mesh->Register(this);
  }

  if (this->Mesh)
  {
    this->Mesher->SetInputData (this->Mesh);
    this->Mesh->GetBounds (this->Bounds);
    this->Modified();
  }
  
}


int vtkLimitFibersToMesh::RequestData (vtkInformation *vtkNotUsed(request),
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
  output->GetPointData()->SetScalars ( input->GetPointData()->GetScalars() );

  for( int i=0; i<input->GetPointData()->GetNumberOfArrays(); i++)
  {
    output->GetPointData()->AddArray ( input->GetPointData()->GetArray (i) );
  }



  vtkUnsignedCharArray* allColors = vtkUnsignedCharArray::SafeDownCast ( input->GetCellData()->GetScalars() );
  vtkUnsignedCharArray* cellColors = vtkUnsignedCharArray::New();

  if( allColors )
    cellColors->SetNumberOfComponents ( allColors->GetNumberOfComponents() );
  //  cellColors->Initialize();
  
  vtkCellArray* lines = input->GetLines();
  if( lines==0 )
  {
    return 0;
  }
  
  lines->InitTraversal();


  vtkIdType npt  = 0;
  vtkIdType *pto = 0;

  vtkIdType test = lines->GetNextCell (npt, pto);
  vtkIdType cellId = 0;

  unsigned char *fiberColor = 0;
  if( allColors )
    fiberColor = new unsigned char[allColors->GetNumberOfComponents()];

  double pcoords[3], weights[3];
  int subId;
  
  this->Mesher->Update();
  
  while( test!=0 )
  {
    bool found = 0;

    for( int i=0; i<npt; i++)
    {
      double* pt = points->GetPoint (pto[i]);

      if ( (pt[0] < this->Bounds[0]) || (pt[0] > this->Bounds[1]) ||
	   (pt[1] < this->Bounds[2]) || (pt[1] > this->Bounds[3]) ||
	   (pt[2] < this->Bounds[4]) || (pt[2] > this->Bounds[5]) )
	continue;
	   
      vtkCell* cell = this->Mesher->GetOutput()->FindAndGetCell(pt,NULL,-1,0.0,subId,pcoords,weights);
      if (cell)
      {
        found = true;
        break;
      }
    }

    if ( ( found && this->GetBooleanOperation() ) ||
         (!found && !this->GetBooleanOperation() ) )
    {
      output->InsertNextCell (VTK_POLY_LINE, npt, pto);

      if( allColors )
      {
	/*
      allColors->GetTypedTuple (cellId, fiberColor);
	  cellColors->InsertNextValue (fiberColor[0]);
	  cellColors->InsertNextValue (fiberColor[1]);
	  cellColors->InsertNextValue (fiberColor[2]);
	*/
    allColors->GetTypedTuple (cellId, fiberColor);
    cellColors->InsertNextTypedTuple ( fiberColor );
      }
      
    }
    
    
    test = lines->GetNextCell (npt, pto);
    cellId++;
  }


  if( allColors )
  {
    output->GetCellData()->SetScalars (cellColors);
    delete [] fiberColor;
  }
  cellColors->Delete();
  
  return 1;
  
}
