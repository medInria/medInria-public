/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDatasetToImageGenerator.h"
#include "vtkObjectFactory.h"

#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"


vtkCxxRevisionMacro(vtkDatasetToImageGenerator, "$Revision: 724 $");
vtkStandardNewMacro(vtkDatasetToImageGenerator);

vtkDatasetToImageGenerator::vtkDatasetToImageGenerator()
{
  this->OutputImageSize[0] = 128;
  this->OutputImageSize[1] = 128;
  this->OutputImageSize[2] = 128;

  this->OutputImageOrigin[0] = 0.0;
  this->OutputImageOrigin[1] = 0.0;
  this->OutputImageOrigin[2] = 0.0;

  this->OutputImageSpacing[0] = 1.0;
  this->OutputImageSpacing[1] = 1.0;
  this->OutputImageSpacing[2] = 1.0;

  this->SpacingSet = 0;
  this->OriginSet = 0;

}

vtkDatasetToImageGenerator::~vtkDatasetToImageGenerator()
{
  //this->RemoveAllInputs();
}


//----------------------------------------------------------------------------
int vtkDatasetToImageGenerator::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector )
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkDataSet *dataset = static_cast<vtkDataSet *>(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  if (!dataset)
    return 0;
  
  double bounds[6];
  dataset->GetBounds(bounds);
  
  // Use the bounds to set the image origin
  if (!this->OriginSet)
  {
    if ( bounds[0] <= bounds[1] &&
	 bounds[2] <= bounds[3] &&
	 bounds[4] <= bounds[5] )
    {
      this->OutputImageOrigin[0] = bounds[0];
      this->OutputImageOrigin[1] = bounds[2];
      this->OutputImageOrigin[2] = bounds[4];
    }
  }

  // If spacing has been set by the user, estimate size to match spacing.
  if (this->SpacingSet)
  {
    for (unsigned int i=0; i<3; i++)
      this->OutputImageSize[i] = (unsigned int)((double)(bounds[2*i+1] - bounds[2*i])/this->OutputImageSpacing[i]);
    
  }
  // Else even if size has not been set by the user, estimate spacing to match size.
  else
  {
    for (unsigned int i=0; i<3; i++)
      this->OutputImageSpacing[i] = (bounds[2*i+1] - bounds[2*i])/(double)this->OutputImageSize[i];
  }
    
  outInfo->Set(vtkDataObject::ORIGIN(), this->OutputImageOrigin, 3);
  outInfo->Set(vtkDataObject::SPACING(),this->OutputImageSpacing,3);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
               0, this->OutputImageSize[0] - 1, 
               0, this->OutputImageSize[1] - 1, 
               0, this->OutputImageSize[2] - 1);
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_UNSIGNED_CHAR, 1);
  return 1;
}


int vtkDatasetToImageGenerator::RequestData(
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector )
{


  // get the data object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkImageData *output = vtkImageData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // just allocate data ! 
  output->SetExtent(
    outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
  output->AllocateScalars();

  return 1;
}

//----------------------------------------------------------------------------
int vtkDatasetToImageGenerator::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}



//----------------------------------------------------------------------------
void vtkDatasetToImageGenerator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Output Dimensions: (" 
               << this->OutputImageSize[0] << ", "
               << this->OutputImageSize[1] << ", "
               << this->OutputImageSize[2] << ")\n";

  os << indent << "Output Spacing: (" 
               << this->OutputImageSpacing[0] << ", "
               << this->OutputImageSpacing[1] << ", "
               << this->OutputImageSpacing[2] << ")\n";
  
  os << indent << "Output Origin: (" 
               << this->OutputImageOrigin[0] << ", "
               << this->OutputImageOrigin[1] << ", "
               << this->OutputImageOrigin[2] << ")\n";
}
