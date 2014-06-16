/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkSqueezeFibers.h"

#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkObjectFactory.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkCellArray.h>

vtkCxxRevisionMacro(vtkSqueezeFibers, "$Revision: 555 $");
vtkStandardNewMacro(vtkSqueezeFibers);

vtkSqueezeFibers::vtkSqueezeFibers()
{
  MaximumNumberOfFibers = 20000;
}

vtkSqueezeFibers::~vtkSqueezeFibers()
{	
}

void vtkSqueezeFibers::PrintSelf(ostream &os, vtkIndent indent)
{
  os << indent << "Maximum Number of Fibers Allowed: " << this->GetMaximumNumberOfFibers() << std::endl;
}

int vtkSqueezeFibers::RequestData (vtkInformation *vtkNotUsed(request),
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
   
  vtkCellArray* lines = input->GetLines();
  if( lines==0 )
    return 0;
  lines->InitTraversal();

  int totalLines = input->GetNumberOfLines();
  
  vtkIdType npt=0;
  vtkIdType *pto=0;
  vtkIdType test = lines->GetNextCell (npt, pto);

  if( (double)totalLines > this->GetMaximumNumberOfFibers() )
  {
	// calculate a sampling rate
    int sampling = (int)ceil( (double)(totalLines) / this->GetMaximumNumberOfFibers() ); 
	
	while( test!=0 )
	{
		output->InsertNextCell( VTK_POLY_LINE, npt, pto);		
		for( int i=0; i<sampling; i++)
		{
			test = lines->GetNextCell (npt, pto);
			if( test==0 ) break;
		}

	}
  }
  else
  {
	while( test!=0 )
	{
		output->InsertNextCell( VTK_POLY_LINE, npt, pto);
		test = lines->GetNextCell (npt, pto);
	}
  }

  return 1;

}
