/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkCompareImageManager.h"
#include <vtkObjectFactory.h>


vtkStandardNewMacro(vtkCompareImageManager);

vtkCompareImageManager::vtkCompareImageManager()
{
  Input1 = 0;
  Input2 = 0;
  Output = 0;
  
  m_IsWindowLevelSet=false;
  

  m_NGridDivisions[0] = -1;
  m_NGridDivisions[1] = -1;
  m_NGridDivisions[2] = -1;
  m_FuseOpacity       = 0.5;
  
  LUT                            = vtkLookupTable::New();
  m_MapToWindowLevelImageFilter1 = vtkImageMapToWindowLevelColors::New();
  m_MapToWindowLevelImageFilter2 = vtkImageMapToWindowLevelColors::New();
  m_CheckerImageFilter           = vtkImageCheckerboard::New();

  m_FuseImageFilter              = vtkImageBlend::New();
  
  //m_MapToWindowLevelImageFilter1->SetInput (Input1);
  //m_MapToWindowLevelImageFilter2->SetInput (Input2);
  m_CheckerImageFilter->SetInput1Data(m_MapToWindowLevelImageFilter1->GetOutput());
  m_CheckerImageFilter->SetInput2Data (m_MapToWindowLevelImageFilter2->GetOutput());

  m_FuseImageFilter->SetInputData (0, m_MapToWindowLevelImageFilter1->GetOutput());
  m_FuseImageFilter->SetInputData (1, m_MapToWindowLevelImageFilter2->GetOutput());
  

  m_ComparisonMode = vtkCompareImageManager::COMPARE_FUSE;
  
  this->SetProperties();
  
}


vtkCompareImageManager::~vtkCompareImageManager()
{
  LUT->Delete();
  m_MapToWindowLevelImageFilter1->Delete();
  m_MapToWindowLevelImageFilter2->Delete();
  m_CheckerImageFilter->Delete();
  m_FuseImageFilter->Delete();
  
}

void vtkCompareImageManager::SetProperties()
{
  this->SetLUTValues();
  m_MapToWindowLevelImageFilter1->SetWindow (0);
  m_MapToWindowLevelImageFilter1->SetLevel (0);
  m_MapToWindowLevelImageFilter2->SetWindow (0);
  m_MapToWindowLevelImageFilter2->SetLevel (0);
  m_CheckerImageFilter->SetNumberOfDivisions (0,0,0);

  m_FuseImageFilter->SetOpacity (1, 0.5);
  
}


void vtkCompareImageManager::SetWindowLevel (float window1, float level1,
					     float window2, float level2)
{
  m_MapToWindowLevelImageFilter1->SetWindow (window1);
  m_MapToWindowLevelImageFilter2->SetWindow (window2);
  m_MapToWindowLevelImageFilter1->SetLevel (level1);
  m_MapToWindowLevelImageFilter2->SetLevel (level2);
  m_IsWindowLevelSet = true;
  
}

 

void vtkCompareImageManager::GenerateData()
{
  if (!Input1 || !Input2)
  {
    vtkErrorMacro(<<"Inputs not set!");
    return;
  }
  
  if (!m_IsWindowLevelSet)
  {
    double *input1range = Input1->GetScalarRange();
    double *input2range = Input2->GetScalarRange();
    
    m_MapToWindowLevelImageFilter1->SetWindow (input1range[1] - input1range[0]);
    m_MapToWindowLevelImageFilter2->SetWindow (input2range[1] - input2range[0]);
    m_MapToWindowLevelImageFilter1->SetLevel (0.5*(input1range[1] - input1range[0]));
    m_MapToWindowLevelImageFilter2->SetLevel (0.5*(input2range[1] - input2range[0]));
  }

  switch(m_ComparisonMode)
  {
      case COMPARE_GRID:

	for (int i=0; i<3; i++)
	{
          /* // It is not our role say if the images can or cannot be compared.
	  if (Input1->GetDimensions()[i] != Input2->GetDimensions()[i])
	  {
	      vtkErrorMacro(<<"Cannot compare images: they must have the same dimensions.");
	      return;
	  }
          */
	  if ( m_NGridDivisions[i] > Input1->GetDimensions()[i] )
          {
            m_NGridDivisions[i] = Input1->GetDimensions()[i];
          }
	  else if (m_NGridDivisions[i] == -1)
          {
            m_NGridDivisions[i] = (int)((double)(Input1->GetDimensions()[i])/2.0);
          }
	  
	}

	m_CheckerImageFilter->SetNumberOfDivisions (m_NGridDivisions[0],m_NGridDivisions[1],m_NGridDivisions[2]);
	m_CheckerImageFilter->UpdateWholeExtent();
	
	break;
	
      case COMPARE_FUSE:
	m_FuseImageFilter->SetOpacity (1, m_FuseOpacity);	
	m_FuseImageFilter->UpdateWholeExtent();
	
	break;

	
      default:
	vtkErrorMacro(<<"Unknown Comparison Type !");
	return;
  }
  
}

void vtkCompareImageManager::ResetData()
{
  Input1 = 0;
  Input2 = 0;
  Output = 0;

  m_IsWindowLevelSet=false;

  m_NGridDivisions[0] = -1;
  m_NGridDivisions[1] = -1;
  m_NGridDivisions[2] = -1;
  m_FuseOpacity       = 0.5;

  m_ComparisonMode = vtkCompareImageManager::COMPARE_FUSE;
  
  this->SetLUTValues();
}


void vtkCompareImageManager::SetLUTValues()
{

  LUT->SetNumberOfTableValues (256);
  LUT->SetTableValue (0, 0.0, 0.0, 0.0, 0.0); // erase color

  LUT->SetTableValue (1, 1.0, 0.0, 0.0, 0.5);
  LUT->SetTableValue (2, 0.0, 1.0, 0.0, 0.5);
  LUT->SetTableValue (3, 0.0, 0.0, 1.0, 0.5);
  LUT->SetTableValue (4, 1.0, 1.0, 0.0, 0.5);
  LUT->SetTableValue (5, 0.0, 1.0, 1.0, 0.5);
  LUT->SetTableValue (6, 1.0, 0.0, 1.0, 0.5);
  
  // Fill the rest of the labels with color ramps, code taken from SNAP
  for (int i = 7; i < 256; i++)
  {
    if (i < 85)
      LUT->SetTableValue (i, ((84.0-i)/85.0 * 200.0 + 50.0)/255.0, (i/85.0 * 200.0 + 50.0)/255.0, 0, 0.5);
    else if (i < 170)
      LUT->SetTableValue (i, 0, ((169.0-i)/85.0 * 200.0 + 50)/255.0, ((i-85)/85.0 * 200.0 + 50)/255.0, 0.5);
    else
      LUT->SetTableValue (i, ((i-170)/85.0 * 200.0 + 50)/255.0, 0.0, ((255.0-i)/85.0 * 200.0 + 50)/255.0, 0.5);
  }

}

vtkImageData* vtkCompareImageManager::GetOutput()
{
  switch(m_ComparisonMode)
  {
      case COMPARE_GRID:
	return m_CheckerImageFilter->GetOutput();
      case COMPARE_FUSE:
	return m_FuseImageFilter->GetOutput();
      default:
	return NULL;
  }
}

  
void vtkCompareImageManager::SetInput1 (vtkImageData* im)
{
  Input1 = im;
  m_MapToWindowLevelImageFilter1->SetInputData (im);
}

void vtkCompareImageManager::SetInput2 (vtkImageData* im)
{
  Input2 = im;
  m_MapToWindowLevelImageFilter2->SetInputData (im);
}
