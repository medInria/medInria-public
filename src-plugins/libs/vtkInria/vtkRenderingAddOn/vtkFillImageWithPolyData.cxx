/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <vtkRenderingAddOn/vtkFillImageWithPolyData.h>

#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPoints.h"
#include "vtkMath.h"

#define PI    3.14159265358979
#define TWOPI 6.28318530717959


vtkCxxRevisionMacro (vtkFillImageWithPolyData, "$Revision: 1209 $");
vtkStandardNewMacro (vtkFillImageWithPolyData);


vtkFillImageWithPolyData::vtkFillImageWithPolyData()
{
  PolyData = 0;
  InsidePixelValue = 1.0;
  ExtractionDirection = 0;
}

vtkFillImageWithPolyData::~vtkFillImageWithPolyData()
{
  if( PolyData )
    PolyData->Delete();
}


//----------------------------------------------------------------------------
// The output extent is the intersection.
int vtkFillImageWithPolyData::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // copy the polydata bounding box into bbox
  if( PolyData )
    PolyData->GetBounds (BBox);  
  
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_UNSIGNED_CHAR , 1);
  
  return 1;
}


void vtkFillImageWithPolyData::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  if( PolyData )
  {
    os << indent << "PolyData: \n";
    os << indent << *PolyData << endl;
  }  
}




//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
void vtkFillImageWithPolyDataExecuteZ(vtkFillImageWithPolyData *self, int ext[6],
                                      vtkImageData *inData, T *in1Ptr,
                                      vtkImageData *outData, unsigned char *outPtr,
                                      int id, int slice, double bbox[6])
{
  int num0, num1, num2, numP;
  int idx0, idx1, idx2;
  vtkIdType in1Inc0, in1Inc1, in1Inc2;
  vtkIdType outInc0, outInc1, outInc2;
  unsigned long count = 0;
  unsigned long target;

  // Get information to march through data 
  inData->GetContinuousIncrements(ext, in1Inc0, in1Inc1, in1Inc2);
  outData->GetContinuousIncrements(ext, outInc0, outInc1, outInc2);
  num0 = ext[1] - ext[0] + 1;
  num1 = ext[3] - ext[2] + 1;
  num2 = ext[5] - ext[4] + 1;

  double outVal = self->GetInsidePixelValue();
  
  vtkPoints* points = self->GetPolyData()->GetPoints();
  numP = self->GetPolyData()->GetNumberOfPoints();

  double* spacing = inData->GetSpacing();
  double* origin  = inData->GetOrigin();
  
  target = (unsigned long)(num2*num1/50.0);
  target++;
  
  // Loop through ouput pixels
  for (idx2 = ext[4]; idx2 < ext[4]+num2; ++idx2)
  {
    
    for (idx1 = ext[2]; !self->AbortExecute && idx1 < ext[2]+num1; ++idx1)
    {
      if (!id) 
      {
        if (!(count%target))
          self->UpdateProgress(count/(50.0*target));
        count++;
      }
      
      for (idx0 = ext[0]; idx0 < ext[0]+num0; ++idx0)
      {

        double val=0.0;
        if( idx2==slice)
        {
          double x=idx0*spacing[0]+origin[0];
          double y=idx1*spacing[1]+origin[1];

          if( x>=bbox[0] && x<=bbox[1] && y>=bbox[2] && y<=bbox[3])
          {            
            double angle=0;
            
            for( int i=0;i<numP; i++)
            {
              
              double p1[3], p2[3], dp1[2], dp2[2];
              points->GetPoint (i,p1);
              points->GetPoint ((i+1)%numP,p2);
              
              dp1[0] = p1[0]-x;
              dp1[1] = p1[1]-y;
              
              dp2[0] = p2[0]-x;
              dp2[1] = p2[1]-y;                           
              
              angle += self->Angle2D (dp1,dp2);

            }            
            if( fabs (angle) >= PI )
              val = outVal;
          }
        }
        
        *outPtr=(unsigned char)(val);
        ++outPtr;
        in1Ptr+=in1Inc0;                
      }
      in1Ptr += in1Inc1;
      outPtr += outInc1;
    }
    in1Ptr += in1Inc2;
    outPtr += outInc2;
  }
  
}




//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
void vtkFillImageWithPolyDataExecuteY(vtkFillImageWithPolyData *self, int ext[6],
                                      vtkImageData *inData, T *in1Ptr,
                                      vtkImageData *outData, unsigned char *outPtr,
                                      int id, int slice, double bbox[6])
{
  int num0, num1, num2, numP;
  int idx0, idx1, idx2;
  vtkIdType in1Inc0, in1Inc1, in1Inc2;
  vtkIdType outInc0, outInc1, outInc2;
  unsigned long count = 0;
  unsigned long target;
  
  // Get information to march through data 
  inData->GetContinuousIncrements(ext, in1Inc0, in1Inc1, in1Inc2);
  outData->GetContinuousIncrements(ext, outInc0, outInc1, outInc2);
  num0 = ext[1] - ext[0] + 1;
  num1 = ext[3] - ext[2] + 1;
  num2 = ext[5] - ext[4] + 1;

  vtkPoints* points = self->GetPolyData()->GetPoints();
  numP = self->GetPolyData()->GetNumberOfPoints();

  double* spacing = inData->GetSpacing();
  double* origin  = inData->GetOrigin();
  
  target = (unsigned long)(num2*num1/50.0);
  target++;

  // Loop through ouput pixels
  for (idx2 = ext[4]; idx2 <ext[4]+ num2; ++idx2)
  {
    for (idx1 = ext[2]; !self->AbortExecute && idx1 < ext[2]+num1; ++idx1)
    {
      if (!id) 
      {
        if (!(count%target))
          self->UpdateProgress(count/(50.0*target));
        count++;
      }
      
      for (idx0 = ext[0]; idx0 < ext[0]+num0; ++idx0)
      {

        double val=0.0;
        
        if( idx1==slice)
        {
          double angle=0;
          
          double x=idx0*spacing[0]+origin[0];
          double y=idx2*spacing[2]+origin[2];

          if( x>=bbox[0] && x<=bbox[1] && y>=bbox[4] && y<=bbox[5])
          {
            
            for( int i=0;i<numP; i++)
            {
              
              double p1[3], p2[3], dp1[2], dp2[2];
              points->GetPoint (i,p1);
              points->GetPoint ((i+1)%numP,p2);
              
              dp1[0] = p1[0]-x;
              dp1[1] = p1[2]-y;
              
              dp2[0] = p2[0]-x;
              dp2[1] = p2[2]-y;                           
              
              angle += self->Angle2D (dp1,dp2);
            }
            
            if( fabs (angle) >= PI )
              val = self->GetInsidePixelValue();
          }
        }
        
        *outPtr=(unsigned char)(val);
        ++outPtr;
        in1Ptr+=in1Inc0;                
      }
      in1Ptr += in1Inc1;
      outPtr += outInc1;
    }
    in1Ptr += in1Inc2;
    outPtr += outInc2;
    }
  
}





//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data.
template <class T>
void vtkFillImageWithPolyDataExecuteX(vtkFillImageWithPolyData *self, int ext[6],
                                      vtkImageData *inData, T *in1Ptr,
                                      vtkImageData *outData, unsigned char *outPtr,
                                      int id, int slice, double bbox[6])
{
  int num0, num1, num2, numP;
  int idx0, idx1, idx2;
  vtkIdType in1Inc0, in1Inc1, in1Inc2;
  vtkIdType outInc0, outInc1, outInc2;
  unsigned long count = 0;
  unsigned long target;
    
  // Get information to march through data 
  inData->GetContinuousIncrements(ext, in1Inc0, in1Inc1, in1Inc2);
  outData->GetContinuousIncrements(ext, outInc0, outInc1, outInc2);
  num0 = ext[1] - ext[0] + 1;
  num1 = ext[3] - ext[2] + 1;
  num2 = ext[5] - ext[4] + 1;

  //get the polydata bounding box
  //double bbox[6];
  //self->GetPolyData()->GetBounds (bbox);

  vtkPoints* points = self->GetPolyData()->GetPoints();
  numP = self->GetPolyData()->GetNumberOfPoints();

  double* spacing = inData->GetSpacing();
  double* origin  = inData->GetOrigin();
  
  target = (unsigned long)(num2*num1/50.0);
  target++;

  // Loop through ouput pixels
  for (idx2 = ext[4]; idx2 < ext[4]+num2; ++idx2)
  {
    for (idx1 = ext[2]; !self->AbortExecute && idx1 < ext[2]+num1; ++idx1)
    {
      if (!id) 
      {
        if (!(count%target))
          self->UpdateProgress(count/(50.0*target));
        count++;
      }
      
      for (idx0 = ext[0]; idx0 < ext[0]+num0; ++idx0)
      {

        double val=0.0;

        if( idx0==slice)
        {
          double angle=0;
          
          double x=idx1*spacing[1]+origin[1];
          double y=idx2*spacing[2]+origin[2];

          if( x>=bbox[2] && x<=bbox[3] && y>=bbox[4] && y<=bbox[5])
          {
            
            for( int i=0;i<numP; i++)
            {
              
              double p1[3], p2[3], dp1[2], dp2[2];
              points->GetPoint (i,p1);
              points->GetPoint ((i+1)%numP,p2);
              
              dp1[0] = p1[1]-x;
              dp1[1] = p1[2]-y;
              
              dp2[0] = p2[1]-x;
              dp2[1] = p2[2]-y;                           
              
              angle += self->Angle2D (dp1,dp2);
            }
            
            if( fabs (angle) >= PI )
              val = self->GetInsidePixelValue();
          }
        }

        *outPtr=(unsigned char)(val);
        ++outPtr;
        in1Ptr+=in1Inc0;                
      }
      in1Ptr += in1Inc1;
      outPtr += outInc1;
    }
    in1Ptr += in1Inc2;
    outPtr += outInc2;
    }
  
}


//----------------------------------------------------------------------------
// This method is passed a input and output Datas, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the Datas data types.
void vtkFillImageWithPolyData::ThreadedRequestData(
  vtkInformation * vtkNotUsed( request ), 
  vtkInformationVector ** vtkNotUsed( inputVector ), 
  vtkInformationVector * vtkNotUsed( outputVector ),
  vtkImageData ***inData, 
  vtkImageData **outData,
  int outExt[6], int id)
{
  void *inPtr1;
  void *outPtr;

  if(!PolyData)
  {
    vtkErrorMacro("PolyData not set");
    return;
  }

  if( PolyData->GetNumberOfPoints() < 3 )
  {
    vtkErrorMacro("PolyData must have more than 2 points.");
    return;
  }

  // check for the orientation and slice to draw the ROI
  int slice=0;
  double* spacing = inData[0][0]->GetSpacing();
  double* origin  = inData[0][0]->GetOrigin();

  vtkPoints* points = PolyData->GetPoints();
  double pt[3];
  points->GetPoint (0,pt);

  switch(ExtractionDirection)
  {
      case 0:
        slice = int( vtkMath::Round ( (pt[0]-origin[0])/spacing[0] ));
        break;
        
      case 1:
        slice = int( vtkMath::Round( (pt[1]-origin[1])/spacing[1] ) );
        break;

      case 2:
        slice = int( vtkMath::Round( (pt[2]-origin[2])/spacing[2] ) );
        break;

      default:
        vtkErrorMacro("Dont know extraction direction.");
        
  }
  
  
  inPtr1 = inData[0][0]->GetScalarPointerForExtent(outExt);
  outPtr = outData[0]->GetScalarPointerForExtent(outExt);
  
  if( ExtractionDirection==2 )
  {
    switch (inData[0][0]->GetScalarType())
    {
      vtkTemplateMacro(vtkFillImageWithPolyDataExecuteZ(this, outExt,
                                                        inData[0][0],(VTK_TT *)(inPtr1),
                                                        outData[0], (unsigned char *)(outPtr), id, slice, BBox));
        default:
          vtkErrorMacro(<< "Execute: Unknown ScalarType");
          return;
    }
  }  
  else
  {
    if ( ExtractionDirection==1 )
    {
      switch (inData[0][0]->GetScalarType())
      {
        
        vtkTemplateMacro(vtkFillImageWithPolyDataExecuteY(this, outExt,
                                                          inData[0][0],(VTK_TT *)(inPtr1),
                                                          outData[0], (unsigned char *)(outPtr),id,slice,BBox));
          default:
            vtkErrorMacro(<< "Execute: Unknown ScalarType");
            return;
      }
      
      
    }
    else
    {
      switch (inData[0][0]->GetScalarType())
      {
        vtkTemplateMacro(vtkFillImageWithPolyDataExecuteX(this, outExt,
                                                          inData[0][0],(VTK_TT *)(inPtr1),
                                                          outData[0], (unsigned char *)(outPtr),id,slice,BBox));
          default:
            vtkErrorMacro(<< "Execute: Unknown ScalarType");
            return;
      }
      
    }
    
  }    
}




double vtkFillImageWithPolyData::Angle2D (const double dp1[2], const double dp2[2])
{
  double dt, t1, t2;
  t1 = atan2 (dp1[0],dp1[1]);
  t2 = atan2 (dp2[0],dp2[1]);

  dt = t2-t1;

  while( dt > PI )
    dt -= TWOPI;
  while( dt < -PI )
    dt += TWOPI;

  return dt;
}
