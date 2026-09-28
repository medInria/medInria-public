/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkLimitFibersToVOI.h"

#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkObjectFactory.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkCellArray.h>
#include <vtkPolyLine.h>
#include <vtkCellData.h>
#include <vtkUnsignedCharArray.h>


vtkStandardNewMacro(vtkLimitFibersToVOI);

vtkLimitFibersToVOI::vtkLimitFibersToVOI()
{
  m_XMin = 0.0;
  m_XMax = -1.0;
  m_YMin = 0.0;
  m_YMax = -1.0;
  m_ZMin = 0.0;
  m_ZMax = -1.0;
  BooleanOperation = 1;
}


void vtkLimitFibersToVOI::SetVOI (const double &xmin, const double &xmax, const double &ymin,
                                  const double &ymax, const double &zmin, const double &zmax)
{
  m_XMin = xmin;
  m_XMax = xmax;
  m_YMin = ymin;
  m_YMax = ymax;
  m_ZMin = zmin;
  m_ZMax = zmax;
  this->Modified();
}


int vtkLimitFibersToVOI::RequestData (vtkInformation *vtkNotUsed(request),
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
  vtkIdType const *pto = nullptr;

  vtkIdType test = lines->GetNextCell (npt, pto);
  vtkIdType cellId = 0;

  unsigned char *fiberColor = 0;
  if( allColors )
    fiberColor = new unsigned char[allColors->GetNumberOfComponents()];
  
  
  while( test!=0 )
  {
    bool found = 0;


    for( int i=0; i<npt; i++)
    {
      double* pt = points->GetPoint (pto[i]);
      if( pt[0]>=m_XMin && pt[0]<=m_XMax &&
          pt[1]>=m_YMin && pt[1]<=m_YMax &&
          pt[2]>=m_ZMin && pt[2]<=m_ZMax )
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
