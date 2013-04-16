/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkImageMerge.h"
#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkImageMerge, "$Revision: 477 $");
vtkStandardNewMacro(vtkImageMerge);

vtkImageMerge::vtkImageMerge()
{
  //allocate
  Input1 = 0;
  Input2 = 0;
  Output = vtkImageData::New();
  
}

vtkImageMerge::~vtkImageMerge()
{
  Output->Delete();
}



//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
void vtkMergeScalarPointers (int NumPts, T* Pointer1, T* Pointer2, T* OutPointer)
{


  // Go through the scalars, assuming the dimensions match
  for( int i=0; i<NumPts; i++)
  {
    
    //int val2 = (int)(*Pointer2);

    // Priority to the second input scalar if not null
    if (*Pointer2)
      *OutPointer = *Pointer2;
    else 
      *OutPointer = *Pointer1;
    
    ++Pointer1;
    ++Pointer2;
    ++OutPointer;
  }
}


void vtkImageMerge::Update()
{
  // Check Data Validity
  if (!Input1)
  {
    vtkErrorMacro(<< "Input Not Set");
    return;
  }
  
  Output->DeepCopy (Input1);

  if (!Input2)
  {
    vtkErrorMacro(<< "Input Not Set");
    return;
  }
  
  int* dims1 = Input1->GetDimensions();
  int* dims2 = Input2->GetDimensions();
  
  if( (dims1[0]!=dims2[0]) || (dims1[1]!=dims2[1]) || (dims1[2]!=dims2[2]) )
  {
    vtkErrorMacro(<< "Images must be the same dimensions");
    return;
  }
  
  if (Input1->GetScalarType() != Input2->GetScalarType())
  {
    vtkErrorMacro(<< "Images must be the same type");
    return;
  }
  
  // compute data
  
  int numPoints = Input1->GetNumberOfPoints();
  void *InPtr1 = Input1->GetScalarPointer();
  void *InPtr2 = Input2->GetScalarPointer();
  void *OutPtr = Output->GetScalarPointer();

  // do the merging
  switch (Input1->GetScalarType())
  {
    vtkTemplateMacro(vtkMergeScalarPointers(numPoints,
                                            (VTK_TT *)InPtr1,
                                            (VTK_TT *)InPtr2,
                                            (VTK_TT *)OutPtr));
      default:
        vtkErrorMacro(<< "Execute: Unknown ScalarType");
        return;
  }
}


