/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkImageFuse.h>


#include "vtkAlgorithmOutput.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPointData.h"
#include "vtkImageMapToColors.h"
#include "vtkLookupTable.h"
#include "vtkImageBlend.h"
#include "vtkImageCheckerboard.h"
#include "vtkImageClamp.h"
#include "vtkImageMapToColors.h"
#include "vtkLookupTableManager.h"


vtkCxxRevisionMacro(vtkImageFuse, "$Revision: 1 $");
vtkStandardNewMacro(vtkImageFuse);

vtkImageFuse::vtkImageFuse()
{
  this->CheckerBoardFilter = vtkImageCheckerboard::New();
  this->BlendFilter = vtkImageBlend::New();
  this->ClampFilter1 = vtkImageClamp::New();
  this->ClampFilter2 = vtkImageClamp::New();
  this->ColorMapper1 = vtkImageMapToColors::New();
  this->ColorMapper2 = vtkImageMapToColors::New();
  
  this->FuseMode = FUSEMODE_BLEND;
  this->BlendAlpha = 0.5;
  this->CheckerAlpha = 0.5;
  this->Divisions[0] = 2;
  this->Divisions[1] = 2;
  this->Divisions[2] = 2;

  this->BlendFilter->SetBlendModeToNormal();
  
  this->ClampFilter1->SetOutputScalarTypeToShort();
  this->ClampFilter1->SetClampMinimumValue (0);
  this->ClampFilter1->SetClampMaximumValue (255);
  this->ClampFilter1->SetClampRatio (0.01);
  this->ClampFilter2->SetOutputScalarTypeToShort();
  this->ClampFilter2->SetClampMinimumValue (0);
  this->ClampFilter2->SetClampMaximumValue (255);
  this->ClampFilter2->SetClampRatio (0.01);
  
  this->ColorMap1 = vtkLookupTableManager::GetBWLookupTable();
  this->ColorMap2 = vtkLookupTableManager::GetSpectrumLookupTable();

  this->ColorMapper1->SetOutputFormatToRGB();
  this->ColorMapper1->SetLookupTable (this->ColorMap1);
  this->ColorMapper2->SetOutputFormatToRGB();
  this->ColorMapper2->SetLookupTable (this->ColorMap2);

  this->UseClamp = 1;
  this->UseColorMaps = 0;

  
//   this->RedLUT = vtkLookupTable::New();
//   this->RedLUT->SetNumberOfTableValues(256);
//   this->RedLUT->Build();
  
//   for( int i=0; i<256; i++)
//   {
//     this->RedLUT->SetTableValue(i, (double)i/255.0, 0, 0, 1.0);
//   }

//   this->GreenLUT = vtkLookupTable::New();
//   this->GreenLUT->SetNumberOfTableValues(256);
//   this->GreenLUT->Build();
  
//   for( int i=0; i<256; i++)
//   {
//     this->GreenLUT->SetTableValue(i, 0, (double)i/255.0, 0, 1.0);
//   }
  
}

vtkImageFuse::~vtkImageFuse()
{
  this->CheckerBoardFilter->Delete();
  this->BlendFilter->Delete();
  this->ClampFilter1->Delete();
  this->ClampFilter2->Delete();
  this->ColorMapper1->Delete();
  this->ColorMapper2->Delete();
  
  this->ColorMap1->Delete();
  this->ColorMap2->Delete();
  
//   this->RedLUT->Delete();
//   this->GreenLUT->Delete();
}





//----------------------------------------------------------------------------
void vtkImageFuse::SetUseClamp(int val)
{
  this->UseClamp = val;
  this->SetupPipeline();
}

//----------------------------------------------------------------------------
void vtkImageFuse::SetUseColorMaps(int val)
{
  this->UseColorMaps = val;
  this->SetupPipeline();
}


//----------------------------------------------------------------------------
void vtkImageFuse::SetLookupTable1(vtkLookupTable* lut)
{
  if (this->ColorMap1 == lut)
    return;

  if (this->ColorMap1)
  {
    this->ColorMap1->UnRegister (this);
  }

  this->ColorMap1 = lut;

  if (this->ColorMap1)
  {
    this->ColorMap1->Register(this);
  }

  if (this->ColorMap1)
  {
    this->ColorMapper1->SetLookupTable (this->ColorMap1);
    this->ColorMapper1->Modified();
    this->Modified();
  }
  
}



//----------------------------------------------------------------------------
void vtkImageFuse::SetLookupTable2(vtkLookupTable* lut)
{
  if (this->ColorMap2 == lut)
    return;

  if (this->ColorMap2)
  {
    this->ColorMap2->UnRegister (this);
  }

  this->ColorMap2 = lut;

  if (this->ColorMap2)
  {
    this->ColorMap2->Register(this);
  }

  if (this->ColorMap2)
  {
    this->ColorMapper2->SetLookupTable (this->ColorMap2);
    this->ColorMapper2->Modified();
    this->Modified();
  }
  
}


//----------------------------------------------------------------------------
void vtkImageFuse::ReplaceNthInputConnection(int idx,
                                              vtkAlgorithmOutput *input)
{
  if (idx < 0 || idx >= this->GetNumberOfInputConnections(0))
    {
    vtkErrorMacro("Attempt to replace connection idx " << idx
                  << " of input port " << 0 << ", which has only "
                  << this->GetNumberOfInputConnections(0)
                  << " connections.");
    return;
    }

  if (!input || !input->GetProducer())
    {
    vtkErrorMacro("Attempt to replace connection index " << idx
                  << " for input port " << 0 << " with " <<
                  (!input ? "a null input." : "an input with no producer."));
    return;
    }

  this->SetNthInputConnection(0, idx, input);
}

//----------------------------------------------------------------------------
// The default vtkImageAlgorithm semantics are that SetInput() puts
// each input on a different port, we want all the image inputs to
// go on the first port.
void vtkImageFuse::SetInput(int idx, vtkDataObject *input)
{
  // Ask the superclass to connect the input.
  this->SetNthInputConnection(0, idx, (input ? input->GetProducerPort() : 0));
    
//   // Ask the superclass to connect the input.
//   if (idx == 0)
//   {
//     this->PlugInput1(input);
//   }
//   if (idx == 1)
//   {
//     this->PlugInput2(input);
//   }

  this->SetupPipeline();
  
}

//----------------------------------------------------------------------------
vtkDataObject *vtkImageFuse::GetInput(int idx)
{
  if (this->GetNumberOfInputConnections(0) <= idx)
    {
    return 0;
    }
  return vtkImageData::SafeDownCast(
    this->GetExecutive()->GetInputData(0, idx));
}


//----------------------------------------------------------------------------
int vtkImageFuse::RequestInformation (
  vtkInformation * vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *vtkNotUsed( outputVector ))
{

  
  if (this->FuseMode != FUSEMODE_GRID)
    return 1;
  

  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkImageData *image = static_cast<vtkImageData *>(inInfo->Get(vtkDataObject::DATA_OBJECT()));


    
  if (!image)
    return 0;
  
  unsigned int i = 0;
  unsigned int iter = 0;
  int minsize = 9999999;
  int* size = image->GetDimensions();
  double* bounds = image->GetBounds();

  for (i=0; i<3; i++)
  {
    if (size[i] < minsize)
    {
      iter = i;
      minsize = size[i];
    }
  }
  
  for (i=0; i<3; i++)
  {
    this->Divisions[i] = (unsigned int)(this->CheckerAlpha * minsize * (bounds[2*i+1] - bounds[2*i])/(bounds[2*iter+1] - bounds[2*iter]));
  }
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkImageFuse::RequestData(
  vtkInformation* request,
  vtkInformationVector** inputVector,
  vtkInformationVector* outputVector)
{
  vtkInformation* info = outputVector->GetInformationObject(0);
  vtkImageData *outimage = static_cast<vtkImageData *>(info->Get(vtkDataObject::DATA_OBJECT()));
  vtkImageData *inimage = 0;

  if (this->UseColorMaps)
  {
    this->ColorMapper1->Update ();
    this->ColorMapper2->Update ();
  }
  
  switch(this->GetFuseMode())
  {
      case FUSEMODE_GRID :

	this->CheckerBoardFilter->SetNumberOfDivisions(this->Divisions);
	this->CheckerBoardFilter->Update();
	inimage = this->CheckerBoardFilter->GetOutput();
	break;
      case FUSEMODE_BLEND :
      default:
	this->BlendFilter->SetOpacity (1, this->BlendAlpha);
	this->BlendFilter->Update();
	inimage = this->BlendFilter->GetOutput();
	break;
  }
  
  outimage->CopyStructure(inimage);
  outimage->SetExtent(inimage->GetExtent());
  outimage->GetPointData()->PassData(inimage->GetPointData());
  outimage->GetPointData()->Modified();
  outimage->Modified();
  
  return 1;
}


//----------------------------------------------------------------------------
int vtkImageFuse::FillInputPortInformation(int port, vtkInformation* info)
{
  if (port == 0)
    {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
    info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 1);
    }
  if (port == 1)
    {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
    }
  return 1;
}




//----------------------------------------------------------------------------
void vtkImageFuse::PlugInput1(vtkDataObject *input)
{
//   vtkImageData* image = vtkImageData::SafeDownCast (input);
//   double* range = image->GetScalarRange();
//   double min = 0;
//   double max = 255;
  
//   min = range[0] + min/255.0*(range[1]-range[0]);
//   max = range[0] + max/255.0*(range[1]-range[0]);

//   double* lutRange = this->ColorMap1->GetTableRange();
//   lutRange[0] = min;
//   lutRange[1] = max;

//   if( lutRange[0] > lutRange[1] )
//   {
//     lutRange[0] = lutRange[1];
//   }
    
//   this->ColorMap1->SetTableRange (lutRange);
  
//   this->ClampFilter1->SetInput (input);
//   this->CheckerBoardFilter->SetInput1 (this->ClampFilter1->GetOutput());
//   this->ColorMapper1->SetInput (this->ClampFilter1->GetOutput());
//   this->BlendFilter->SetInput (0, this->ColorMapper1->GetOutput());


//   this->ColorMapper1->SetInput (input);
//   this->BlendFilter->SetInput (0, this->ColorMapper1->GetOutput());
//   this->CheckerBoardFilter->SetInput1 (this->ColorMapper1->GetOutput());
  
}


//----------------------------------------------------------------------------
void vtkImageFuse::PlugInput2(vtkDataObject *input)
{
//   vtkImageData* image = vtkImageData::SafeDownCast (input);
//   double* range = image->GetScalarRange();
//   double min = 0;
//   double max = 255;
  
//   min = range[0] + min/255.0*(range[1]-range[0]);
//   max = range[0] + max/255.0*(range[1]-range[0]);

//   double* lutRange = this->ColorMap2->GetTableRange();
//   lutRange[0] = min;
//   lutRange[1] = max;
  

//   if( lutRange[0] > lutRange[1] )
//   {
//     lutRange[0] = lutRange[1];
//   }
    
//   this->ColorMap2->SetTableRange (lutRange);
//   this->ClampFilter2->SetInput (input);
//   this->CheckerBoardFilter->SetInput2 (this->ClampFilter2->GetOutput());
//   this->ColorMapper2->SetInput (this->ClampFilter2->GetOutput());
//   this->BlendFilter->SetInput (1, this->ColorMapper2->GetOutput());
//   this->ColorMapper2->SetInput (input);
//   this->BlendFilter->SetInput (1, this->ColorMapper2->GetOutput());
//   this->CheckerBoardFilter->SetInput2 (this->ColorMapper2->GetOutput());
  
}



//----------------------------------------------------------------------------
void vtkImageFuse::SetupPipeline()
{
  vtkImageData* input1 = vtkImageData::SafeDownCast (this->GetInput (0));
  vtkImageData* input2 = vtkImageData::SafeDownCast (this->GetInput (1));

  if (input1 && input2)
  {
    if (this->UseColorMaps)
    {
      this->ColorMapper1->SetInput (input1);
      this->ColorMapper2->SetInput (input2);
      this->BlendFilter->SetInput(0, this->ColorMapper1->GetOutput());
      this->BlendFilter->SetInput(1, this->ColorMapper2->GetOutput());
      this->CheckerBoardFilter->SetInputConnection (0, this->ColorMapper1->GetOutputPort());
      this->CheckerBoardFilter->SetInputConnection (1, this->ColorMapper2->GetOutputPort());
    }
    else if (this->UseClamp)
    {
      this->ClampFilter1->SetInputConnection (input1->GetProducerPort());
      this->ClampFilter2->SetInputConnection (input2->GetProducerPort());
      this->BlendFilter->SetInput (0, this->ClampFilter1->GetOutput());
      this->BlendFilter->SetInput (1, this->ClampFilter2->GetOutput());
      this->CheckerBoardFilter->SetInputConnection (0, this->ClampFilter1->GetOutputPort());
      this->CheckerBoardFilter->SetInputConnection (1, this->ClampFilter2->GetOutputPort());
    }
    else
    {
      this->BlendFilter->SetInput (0, input1);
      this->BlendFilter->SetInput (1, input2);
      this->CheckerBoardFilter->SetInputConnection (0, input1->GetProducerPort());
      this->CheckerBoardFilter->SetInputConnection (1, input2->GetProducerPort());
    }

    this->Modified();
  }
  
    
      
      
      
}



void vtkImageFuse::SetBlendAlpha (double val)
{
  this->BlendAlpha = val;
  this->Modified();
}

void vtkImageFuse::SetCheckerAlpha (double val)
{
  this->CheckerAlpha = val;
  this->Modified();
}
