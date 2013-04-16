/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkImageClamp.h"

#include "vtkImageData.h"
#include "vtkImageProgressIterator.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkImageAccumulate.h"
#include "vtkIntArray.h"
#include "vtkPointData.h"


vtkCxxRevisionMacro(vtkImageClamp, "$Revision: 1 $");
vtkStandardNewMacro(vtkImageClamp);

//----------------------------------------------------------------------------
vtkImageClamp::vtkImageClamp()
{
  this->OutputScalarType = -1;
  this->ClampOverflow = 0;
  this->ClampRatio = 0.01;
  this->ClampMinimumValue = 0.0;
  this->ClampMaximumValue = 255.0;
  this->Window[0] = 0.0;
  this->Window[1] = 255.0;
}

//----------------------------------------------------------------------------
vtkImageClamp::~vtkImageClamp()
{
}

//----------------------------------------------------------------------------
void vtkImageClamp::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
int vtkImageClamp::RequestInformation(vtkInformation*,
				      vtkInformationVector** inputVector,
				      vtkInformationVector* outputVector)
{
  // Set the image scalar type for the output.
  if(this->OutputScalarType != -1)
    {
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkDataObject::SetPointDataActiveScalarInfo(
      outInfo, this->OutputScalarType, -1);
    }

  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkImageData *input = vtkImageData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!input)
    return 0;

  double* range = input->GetScalarRange();  
  
  vtkImageAccumulate* histogram = vtkImageAccumulate::New();
  histogram->SetInput ( input );
  
  int extent[6] = {0, 1000, 0, 0, 0, 0};
  histogram->SetComponentExtent (extent);
  
  double spacing[3] = { (range[1]-range[0])/1000.0, 0.0, 0.0};
  histogram->SetComponentSpacing (spacing);
  
  double origin[3] = {range[0], 0.0, 0.0};
  histogram->SetComponentOrigin (origin);
  histogram->Update();

  vtkImageData* histogramoutput = histogram->GetOutput();  
  vtkIntArray*  ptData = vtkIntArray::SafeDownCast (histogramoutput->GetPointData()->GetScalars());

  if( !ptData)
  {
    vtkErrorMacro( << "Error: Cannot cast point data to integers.");
    return 0;
  }

  double numVox = histogram->GetVoxelCount();
  double onePercent = numVox/100.0;

  int start=1;
  double currentPercent = 0.0;
  double minPercent = this->ClampRatio;
  // double maxPercent = 1 - this->ClampRatio;
  
  while( currentPercent<minPercent && start<999)
  {
    double tuple;
    ptData->GetTuple (start, &tuple);
    currentPercent += tuple/onePercent;    
    start++;
  }

  currentPercent = 0.0;
  int end = 999;
  while( currentPercent<minPercent && end>0 )
  {
    double tuple;
    ptData->GetTuple (end, &tuple);
    currentPercent += tuple/onePercent;    
    end--;
  }

  this->Window[0] = (start)*(range[1]-range[0])/1000.0;
  this->Window[1] = (end)*(range[1]-range[0])/1000.0;
  

  histogram->Delete();
  
  return 1;
}

//----------------------------------------------------------------------------
// This function template implements the filter for any type of data.
// The last two arguments help the vtkTemplateMacro calls below
// instantiate the proper input and output types.
template <class IT, class OT>
void vtkImageClampExecute(vtkImageClamp* self,
                               vtkImageData* inData,
                               vtkImageData* outData,
                               int outExt[6], int id,
                               IT*, OT*)
{
  // Create iterators for the input and output extents assigned to
  // this thread.
  vtkImageIterator<IT> inIt(inData, outExt);
  vtkImageProgressIterator<OT> outIt(outData, outExt, self, id);

  // Clamp pixel values within the range of the output type.
  double typeMin = outData->GetScalarTypeMin();
  double typeMax = outData->GetScalarTypeMax();
  int clamp = self->GetClampOverflow();

  double range = self->GetClampMaximumValue() - self->GetClampMinimumValue();
  
  // Loop through output pixels.
  while (!outIt.IsAtEnd())
  {
    IT* inSI = inIt.BeginSpan();
    OT* outSI = outIt.BeginSpan();
    OT* outSIEnd = outIt.EndSpan();
    if (clamp)
    {
      while (outSI != outSIEnd)
      {
	// Pixel operation
	double val = (double)(*inSI);
	
	if (val > self->GetWindow()[1])
	{
          val = self->GetClampMaximumValue();
	}
	else if (val < self->GetWindow()[0])
	{
          val = self->GetClampMinimumValue();
	}
	else
	{
          val = self->GetClampMinimumValue() + (val - self->GetWindow()[0]) * range / (self->GetWindow()[1] - self->GetWindow()[0]);
	}

	
	if (val > typeMax)
	{
          val = typeMax;
	}
        if (val < typeMin)
	{
          val = typeMin;
	}
        *outSI = (OT)(val);
        ++outSI;
        ++inSI;
      }
    }
    else
    {
      while (outSI != outSIEnd)
      {
	// Pixel operation
	double val = (double)(*inSI);

	if (val > self->GetWindow()[1])
	{
          val = self->GetClampMaximumValue();
	}
	else if (val < self->GetWindow()[0])
	{
          val = self->GetClampMinimumValue();
	}
	else
	{
          val = self->GetClampMinimumValue() + (val - self->GetWindow()[0]) * range / (self->GetWindow()[1] - self->GetWindow()[0]);
	}

        // Pixel operation
        *outSI = (OT)(val);
        ++outSI;
        ++inSI;
      }
    }
    inIt.NextSpan();
    outIt.NextSpan();
  }
}

//----------------------------------------------------------------------------
template <class T>
void vtkImageClampExecute1(vtkImageClamp* self,
                                vtkImageData* inData,
                                vtkImageData* outData,
                                int outExt[6], int id, T*)
{
  switch (outData->GetScalarType())
    {
    vtkTemplateMacro(
      vtkImageClampExecute(self, inData,
                                outData, outExt, id,
                                static_cast<T*>(0),
                                static_cast<VTK_TT*>(0)));
    default:
      vtkErrorWithObjectMacro(
        self, "ThreadedRequestData: Unknown output ScalarType");
      return;
    }
}

//----------------------------------------------------------------------------
// This method is passed a input and output data, and executes the filter
// algorithm to fill the output from the input.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImageClamp::ThreadedRequestData(vtkInformation*,
                                             vtkInformationVector**,
                                             vtkInformationVector*,
                                             vtkImageData*** inData,
                                             vtkImageData** outData,
                                             int outExt[6],
                                             int threadId)
{
  vtkImageData* input = inData[0][0];
  vtkImageData* output = outData[0];
  switch(input->GetScalarType())
    {
    vtkTemplateMacro(
      vtkImageClampExecute1(this, input, output, outExt, threadId,
                                 static_cast<VTK_TT*>(0)));
    default:
      vtkErrorMacro("ThreadedRequestData: Unknown input ScalarType");
      return;
    }
}
