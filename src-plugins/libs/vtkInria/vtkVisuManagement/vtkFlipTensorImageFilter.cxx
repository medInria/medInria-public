/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkFlipTensorImageFilter.h"

#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"

vtkCxxRevisionMacro (vtkFlipTensorImageFilter, "$Revision: 477 $");
vtkStandardNewMacro (vtkFlipTensorImageFilter);

vtkFlipTensorImageFilter::vtkFlipTensorImageFilter()
{
  FlipX=0;
  FlipY=0;
  FlipZ=0;
}


vtkFlipTensorImageFilter::~vtkFlipTensorImageFilter()
{}



int vtkFlipTensorImageFilter::RequestData(vtkInformation *vtkNotUsed(request), 
                                          vtkInformationVector **inputVector, 
                                          vtkInformationVector *outputVector)
{
  
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and ouptut
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkDataSet *output = vtkDataSet::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkDataArray *inTensors;
  double inT[9];
  vtkPointData *pd = input->GetPointData();
  vtkPointData *outPD = output->GetPointData();
  vtkIdType ptId, numPts;
  vtkDoubleArray *newTensors=NULL;
  
  // First, copy the input to the output as a starting point
  output->CopyStructure( input );

  inTensors = pd->GetTensors();
  numPts    = input->GetNumberOfPoints();

  newTensors = vtkDoubleArray::New();
  newTensors->SetNumberOfComponents (9);
  newTensors->SetNumberOfTuples (numPts);

  if ( !inTensors || numPts < 1 )
  {
    vtkErrorMacro(<<"No data to flip!");
    return 1;
  }
  
  outPD->CopyAllOn();
  outPD->PassData(pd);

  for (ptId=0; ptId < numPts; ptId++)
  {

    inTensors->GetTuple(ptId, inT);
    
    //processing
    if( FlipX )
    {
      inT[1] = -1.0*inT[1];
      inT[2] = -1.0*inT[2];
      inT[3] = -1.0*inT[3];
      inT[6] = -1.0*inT[6];
    }
    
    if( FlipY )
    {
      inT[1] = -1.0*inT[1];
      inT[3] = -1.0*inT[3];
      inT[5] = -1.0*inT[5];
      inT[7] = -1.0*inT[7];
    }
    
    if( FlipZ )
    {
      inT[2] = -1.0*inT[2];
      inT[5] = -1.0*inT[5];
      inT[6] = -1.0*inT[6];
      inT[7] = -1.0*inT[7];
    }

    newTensors->SetTuple (ptId, inT);
  }

  outPD->SetTensors (newTensors);
  newTensors->Delete();

  return 1;
  
}
