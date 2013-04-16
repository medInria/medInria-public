/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkRegistrationMethod.h"

#include "itkTransformFileWriter.h"

#include "itkTransform.h"

namespace itk
{

/*
 * Constructor
 */
template <typename TImage>
RegistrationMethod<TImage>
::RegistrationMethod()
{
  this->SetNumberOfRequiredOutputs( 1 );  // for the Transform

  m_Transform = 0; // has to be provided by subclass.
  
  m_FixedImage   = 0; // has to be provided by the user.
  m_MovingImage  = 0; // has to be provided by the user.
  m_InitialTransform = 0; // can be provided by the user.
  
  m_InitialTransformParameters = ParametersType(1);
  m_LastTransformParameters = ParametersType(1);

  m_InitialTransformParameters.Fill( 0.0f );
  m_LastTransformParameters.Fill( 0.0f );

  m_Name = "MethodSuperClass";

  typename ImageType::Pointer image = ImageType::New();

  typename ImageType::Pointer Decorator = static_cast<ImageType*>(image);
  
  this->ProcessObject::SetNthOutput( 0, Decorator.GetPointer() );
}

/*
 * Set the initial transform parameters
 */
template <typename TImage>
void
RegistrationMethod<TImage>
::SetInitialTransform (TransformConstPointerType transform)
{
  m_InitialTransform = transform;
  this->Modified();
}
  
  

/*
 * Set the initial transform parameters
 */
template <typename TImage>
void
RegistrationMethod<TImage>
::SetInitialTransformParameters( const ParametersType & param )
{
  m_InitialTransformParameters = param;
  this->Modified();
}


/*
 * Initialize by setting the interconnects between components. 
 */
template <typename TImage>
void
RegistrationMethod<TImage>
::Initialize()
{
  
  if( !m_FixedImage )
  {
    itkExceptionMacro(<<"FixedImage is not present");
  }
  
  if( !m_MovingImage )
  {
    itkExceptionMacro(<<"MovingImage is not present");
  }

  this->SetTransform (NULL);


}



/*
 * PrintSelf
 */
template <typename TImage>
void
RegistrationMethod<TImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  if (m_Transform)
    os << indent << "Transform: " << m_Transform.GetPointer() << std::endl;
  if (m_FixedImage)
    os << indent << "Fixed Image: " << m_FixedImage.GetPointer() << std::endl;
  if (m_MovingImage)
    os << indent << "Moving Image: " << m_MovingImage.GetPointer() << std::endl;
  os << indent << "Initial Transform Parameters: " << m_InitialTransformParameters << std::endl;
  os << indent << "Last    Transform Parameters: " << m_LastTransformParameters << std::endl;
}


template <typename TImage>
void 
RegistrationMethod<TImage>
::SetFixedImage( ImageType * fixedImage )
{
  itkDebugMacro("setting Fixed Image to " << fixedImage ); 
  
  if (this->m_FixedImage.GetPointer() != fixedImage ) 
  { 
    this->m_FixedImage = fixedImage;
    
    // Process object is not const-correct so the const_cast is required here
    this->ProcessObject::SetNthInput(0, 
				     const_cast< ImageType*>( fixedImage ) );
    
    this->Modified(); 
  } 
}



template <typename TImage>
void 
RegistrationMethod<TImage>
::SetMovingImage( ImageType * movingImage )
{
  itkDebugMacro("setting Moving Image to " << movingImage ); 
  
  if (this->m_MovingImage.GetPointer() != movingImage ) 
  { 
    this->m_MovingImage = movingImage;
    
    // Process object is not const-correct so the const_cast is required here
    this->ProcessObject::SetNthInput(1, 
				     const_cast< ImageType*>( movingImage ) );
    
    this->Modified(); 
  } 
}


  



template <typename TImage>
void 
RegistrationMethod<TImage>
::WriteTransform (const char* filename)
{

  if (!this->GetTransform())
    return;
  
  itk::TransformFileWriter::Pointer writer = itk::TransformFileWriter::New();
  writer->SetFileName(filename);
  writer->SetInput (this->GetTransform());
  writer->Update();
  
}


  
  /**
   * Schedules
   */
template <typename TImage>
typename RegistrationMethod<TImage>::ScheduleType
RegistrationMethod<TImage>
::GetOptimumSchedule(unsigned int MinimumDimensionSize, unsigned int DesiredNumberOfLevels)
{
  ScheduleType nullschedule (0,0);

  // First we check if there is an image present in the method
  // we base our schedule on the Fixed (target) image because it
  // is the one we will take for resampling
  if (!m_FixedImage)
  {
    itkDebugMacro (<<"no input image : cannot generate schedule !");
    return nullschedule;
  }

  // We need those typedefs to define the schedule
  typedef typename ImageType::SizeType    SizeType;
  typedef typename ImageType::SpacingType SpacingType;
  typedef Array<double>                   RatioArrayType;
  typedef Array<unsigned int>             MaxLevelArrayType;
  
  // Get information from the image
  // we instantiate arrays at the right dimensions
  SizeType size = m_FixedImage->GetLargestPossibleRegion().GetSize();
  SpacingType spacing = m_FixedImage->GetSpacing();
  RatioArrayType ratioarray (ImageType::ImageDimension);
  MaxLevelArrayType maxlevelarray (ImageType::ImageDimension);
  unsigned int maxmaxlevels = 0;

  // Get more information about the maximum spacing value in the
  // image and the minimum size value
  double spacingmax = 0;
  unsigned long sizemin = NumericTraits<unsigned long>::max();

  for (unsigned int i=0; i<ImageType::ImageDimension; i++)
  {
    if (spacingmax < spacing[i])
      spacingmax = spacing[i];
    if (sizemin > size[i])
      sizemin = size[i];
  }
  
  itkDebugMacro (<<"sizemin : "<<sizemin<<"\n");
  itkDebugMacro (<<"MinimumDimensionSize : "<<MinimumDimensionSize<<"\n");
  itkDebugMacro (<<"ratioarray - maxlevelarray;   "<<"\n");

  // Build the level array from this information
  
  for (unsigned int i=0; i<ImageType::ImageDimension; i++)
  {
    ratioarray[i] = spacingmax / spacing[i];
    maxlevelarray[i] = (unsigned int) RegistrationMethod<TImage>::roundint ( (double)(std::log ((double)(ratioarray[i])) / std::log (2.0)) );
    itkDebugMacro (<<ratioarray[i]<<" - "<<maxlevelarray[i]<<";   ");
    if (maxmaxlevels < maxlevelarray[i])
      maxmaxlevels = maxlevelarray[i];
  }

  typedef std::vector<unsigned int> TableLine;
  typedef std::vector< TableLine > TableType;
  TableType reversetable;
  TableLine firstline;
  for (unsigned int d = 0; d< ImageType::ImageDimension; d++)
  { firstline.push_back (1); }
  reversetable.push_back (firstline);
  unsigned int currentminsize = sizemin;
  unsigned int MaximumAllowedNumberOfLevels = 0;

  itkDebugMacro (<<"Reversed : "<<"\n");
  if (currentminsize < MinimumDimensionSize)
  {
    MinimumDimensionSize = currentminsize;
  }
  
  while (currentminsize >= MinimumDimensionSize)
  {
    MaximumAllowedNumberOfLevels++;
    itkDebugMacro (<<"level : "<<MaximumAllowedNumberOfLevels<<"\n");
    TableLine line;
    
    for (unsigned int d = 0; d< ImageType::ImageDimension; d++)
    {
      if (MaximumAllowedNumberOfLevels <= maxmaxlevels - maxlevelarray[d])
	line.push_back (reversetable[MaximumAllowedNumberOfLevels-1][d]);
      else
	line.push_back (2*reversetable[MaximumAllowedNumberOfLevels-1][d]);
      unsigned int tmp = (unsigned int)RegistrationMethod<TImage>::roundint((double)(size[d]) / (double)(line[d]));
      if (tmp < currentminsize)
	currentminsize = tmp;
      itkDebugMacro (<<line[d]<<" \n");
    }
    
    reversetable.push_back (line);
  }
  
  itkDebugMacro (<<"MaximumAllowedNumberOfLevels : "<<MaximumAllowedNumberOfLevels<<"\n");

  unsigned int numberoflevels = std::min (MaximumAllowedNumberOfLevels, DesiredNumberOfLevels);
  ScheduleType Schedule (numberoflevels, ImageType::ImageDimension);

  // We have to shift it so that the finest resolution will appear
  // Note that we are sure that this value is greater that or egual to zero 
  unsigned int shift = MaximumAllowedNumberOfLevels - numberoflevels;
  
  
  itkDebugMacro (<<"Schedule :"<<"\n");
  
  for (unsigned int level=0; level<numberoflevels; level++)
  {
    for (unsigned int dimension=0; dimension<ImageType::ImageDimension; dimension++)
    {
      Schedule[level][dimension] = reversetable[MaximumAllowedNumberOfLevels - level - shift - 1][dimension];
      itkDebugMacro (<<Schedule[level][dimension]<<" ");
    }
  }  
  
  return Schedule;
}
  




} // end namespace itk



