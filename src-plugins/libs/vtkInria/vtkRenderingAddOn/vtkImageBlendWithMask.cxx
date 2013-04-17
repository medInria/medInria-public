/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRenderingAddOn/vtkImageBlendWithMask.h>

#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"

vtkCxxRevisionMacro (vtkImageBlendWithMask, "$Revision: 1080 $");
vtkStandardNewMacro (vtkImageBlendWithMask);

vtkImageBlendWithMask::vtkImageBlendWithMask()
{
  LookupTable=0;
  this->SetNumberOfInputPorts (2);
}

vtkImageBlendWithMask::~vtkImageBlendWithMask()
{
  if( LookupTable )
    LookupTable->Delete();
}

//----------------------------------------------------------------------------
void vtkImageBlendWithMask::SetImageInput(vtkImageData *in)
{
  this->SetInput1(in);
}

//----------------------------------------------------------------------------
void vtkImageBlendWithMask::SetMaskInput(vtkImageData *in) 
{
  this->SetInput2(in);
}

//----------------------------------------------------------------------------
// The output extent is the intersection.
int vtkImageBlendWithMask::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *inInfo2 = inputVector[1]->GetInformationObject(0);

  int ext[6], ext2[6], idx;

  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),ext);
  inInfo2->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),ext2);
  for (idx = 0; idx < 3; ++idx)
    {
    if (ext2[idx*2] > ext[idx*2])
      {
      ext[idx*2] = ext2[idx*2];
      }
    if (ext2[idx*2+1] < ext[idx*2+1])
      {
      ext[idx*2+1] = ext2[idx*2+1];
      }
    }
  
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),ext,6);

  return 1;
}


void vtkImageBlendWithMask::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  if( LookupTable )
  {
    os << indent << "LookupTable: \n";
    os << indent << *LookupTable << endl;
  }  
}




//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
void vtkImageBlendWithMaskExecute(vtkImageBlendWithMask *self, int ext[6],
                           vtkImageData *in1Data, T *in1Ptr,
                           vtkImageData *in2Data, T *in2Ptr,
                           vtkImageData *outData, T *outPtr, int id)
{
  int num0, num1, num2, numC, numM, pixSize;
  int idx0, idx1, idx2, idxC;
  vtkIdType in1Inc0, in1Inc1, in1Inc2;
  vtkIdType in2Inc0, in2Inc1, in2Inc2;
  vtkIdType outInc0, outInc1, outInc2;
  double maskAlpha, oneMinusMaskAlpha;
  unsigned long count = 0;
  unsigned long target;
  
  numC = outData->GetNumberOfScalarComponents();
  pixSize = numC * sizeof(T);
  maskAlpha = 0.5;
  oneMinusMaskAlpha = 0.5;

  numM = in2Data->GetNumberOfScalarComponents();
  
  // Get information to march through data 
  in1Data->GetContinuousIncrements(ext, in1Inc0, in1Inc1, in1Inc2);
  in2Data->GetContinuousIncrements(ext, in2Inc0, in2Inc1, in2Inc2);
  outData->GetContinuousIncrements(ext, outInc0, outInc1, outInc2);
  num0 = ext[1] - ext[0] + 1;
  num1 = ext[3] - ext[2] + 1;
  num2 = ext[5] - ext[4] + 1;

  
  target = (unsigned long)(num2*num1/50.0);
  target++;

  // Loop through ouput pixels
  for (idx2 = 0; idx2 < num2; ++idx2)
  {
    for (idx1 = 0; !self->AbortExecute && idx1 < num1; ++idx1)
    {
      if (!id) 
      {
        if (!(count%target))
          self->UpdateProgress(count/(50.0*target));
        count++;
      }
      
      for (idx0 = 0; idx0 < num0; ++idx0)
      {
        
        if( int(*in2Ptr)==0 )
        {
          memcpy (outPtr, in1Ptr, pixSize);
          in1Ptr += numC;
          outPtr += numC;
        }
        else
        {
          double color[4];
          self->GetLookupTable()->GetTableValue ((int)(*in2Ptr), color);
          maskAlpha = color[3];
          oneMinusMaskAlpha = 1.0-maskAlpha;
          
          for(idxC = 0; idxC<numC; idxC++)
          {
            *outPtr = (T)((int)(*in1Ptr)*oneMinusMaskAlpha+(int)(color[idxC]*255.0)*maskAlpha);
            ++outPtr;
            ++in1Ptr;
          }
        }

        in2Ptr += numM;
                
      }
      in1Ptr += in1Inc1;
      in2Ptr += in2Inc1;
      outPtr += outInc1;
    }
    in1Ptr += in1Inc2;
    in2Ptr += in2Inc2;
    outPtr += outInc2;
    }
  
}







//----------------------------------------------------------------------------
// This method is passed a input and output Datas, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the Datas data types.
void vtkImageBlendWithMask::ThreadedRequestData(
  vtkInformation * vtkNotUsed( request ), 
  vtkInformationVector ** vtkNotUsed( inputVector ), 
  vtkInformationVector * vtkNotUsed( outputVector ),
  vtkImageData ***inData, 
  vtkImageData **outData,
  int outExt[6], int id)
{
  void *inPtr1;
  void *inPtr2;
  void *outPtr;
  int *tExt;

  if(!LookupTable)
  {
    vtkErrorMacro("LookupTable not set");
    return;
  }

  vtkImageData* mask = vtkImageData::SafeDownCast (inData[1][0]);

  if( !mask )
  {
    vtkErrorMacro("Mask is not set");
    return;
  }
  
  
  inPtr1 = inData[0][0]->GetScalarPointerForExtent(outExt);
  inPtr2 = inData[1][0]->GetScalarPointerForExtent(outExt);
  outPtr = outData[0]->GetScalarPointerForExtent(outExt);

  tExt = inData[1][0]->GetExtent();
  if (tExt[0] > outExt[0] || tExt[1] < outExt[1] || 
      tExt[2] > outExt[2] || tExt[3] < outExt[3] ||
      tExt[4] > outExt[4] || tExt[5] < outExt[5])
    {
    vtkErrorMacro("Mask extent not large enough");
    return;
    }
  /*
  if (inData[1][0]->GetNumberOfScalarComponents() != 1)
    {
    vtkErrorMacro("Mask can have one component");
    }*/
    
  if (inData[0][0]->GetScalarType() != outData[0]->GetScalarType() ||
      inData[0][0]->GetScalarType() != VTK_UNSIGNED_CHAR ||
      (inData[0][0]->GetNumberOfScalarComponents() != 3 && inData[0][0]->GetNumberOfScalarComponents() != 4)
      /*inData[1][0]->GetScalarType() != VTK_UNSIGNED_CHAR*/)
    {
      vtkErrorMacro(<< "Execute: image ScalarType (" 
                    << inData[0][0]->GetScalarType() << ") must match out ScalarType (" 
                    << outData[0]->GetScalarType() << "), and mask scalar type (" 
                    << inData[1][0]->GetScalarType() << ") must be unsigned char."
                    << "Number of input components: " << inData[0][0]->GetNumberOfScalarComponents());
    return;
    }
  
  switch (inData[0][0]->GetScalarType())
    {
    vtkTemplateMacro(
                     vtkImageBlendWithMaskExecute(this, outExt,
                                                  inData[0][0],(VTK_TT *)(inPtr1),
                                                  //inData[1][0],(unsigned char *)(inPtr2),
                                                  inData[1][0],(VTK_TT *)(inPtr2),
                                                  outData[0], (VTK_TT *)(outPtr),id));
        default:
          vtkErrorMacro(<< "Execute: Unknown ScalarType");
          return;
    }
}



