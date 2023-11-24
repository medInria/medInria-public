#ifndef __itkMultiResolutionLogDomainDeformableRegistration_txx
#define __itkMultiResolutionLogDomainDeformableRegistration_txx
#include "itkMultiResolutionLogDomainDeformableRegistration.h"

#include "itkRecursiveGaussianImageFilter.h"
#include "itkRecursiveMultiResolutionPyramidImageFilter.h"
#include "itkImageRegionIterator.h"
#include "vnl/vnl_math.h"

namespace itk
{

// Default constructor
template <class TFixedImage, class TMovingImage, class TField, class TRealType>
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::MultiResolutionLogDomainDeformableRegistration()
{
#if (ITK_VERSION_MAJOR < 4)
  this->SetNumberOfRequiredInputs(2);
#else
  //HACK:  This really should define the names of the required inputs.
  this->SetNumberOfIndexedInputs(2);
  // Primary input is optional in this filter
  this->RemoveRequiredInputName( "Primary" );
#endif

  typename DefaultRegistrationType::Pointer registrator =
    DefaultRegistrationType::New();
  m_RegistrationFilter = static_cast<RegistrationType *>(
      registrator.GetPointer() );

  // /\todo Choose the right type of pyramid
#if ( ITK_VERSION_MAJOR == 3 && ITK_VERSION_MINOR == 12 && ITK_VERSION_PATCH == 0 )
  // Work-around for http://public.kitware.com/Bug/view.php?id=503
  itkWarningMacro(
    "This version of ITK has a bug in MultiResolutionPyramidImageFilter - using RecursiveMultiResolutionPyramidImageFilter instead");
  typedef RecursiveMultiResolutionPyramidImageFilter
  <FixedImageType, FloatImageType>  ActualFixedImagePyramidType;
  typedef RecursiveMultiResolutionPyramidImageFilter
  <MovingImageType, FloatImageType> ActualMovingImagePyramidType;
#else
  typedef MultiResolutionPyramidImageFilter
  <FixedImageType, FloatImageType>  ActualFixedImagePyramidType;
  typedef MultiResolutionPyramidImageFilter
  <MovingImageType, FloatImageType> ActualMovingImagePyramidType;
#endif

  m_MovingImagePyramid  = ActualMovingImagePyramidType::New();
  m_FixedImagePyramid     = ActualFixedImagePyramidType::New();
  m_FieldExpander     = FieldExpanderType::New();
  m_InitialVelocityField = NULL;

  m_NumberOfLevels = 3;
  m_NumberOfIterations.resize( m_NumberOfLevels );
  m_FixedImagePyramid->SetNumberOfLevels( m_NumberOfLevels );
  m_MovingImagePyramid->SetNumberOfLevels( m_NumberOfLevels );

  unsigned int ilevel;
  for( ilevel = 0; ilevel < m_NumberOfLevels; ilevel++ )
    {
    m_NumberOfIterations[ilevel] = 10;
    }
  m_CurrentLevel = 0;

  m_StopRegistrationFlag = false;

  m_Exponentiator = FieldExponentiatorType::New();

  m_RegularizationType = 1;

  m_HarmonicWeight   = 1e-4;
  m_BendingWeight    = 1e-5;

  // Initialize standard deviations for both Gaussian regularizations
  for( unsigned int i=0; i<ImageDimension; i++ )
  {
      this->m_StandardDeviations[i]            = 1.0;
      this->m_UpdateFieldStandardDeviations[i] = 1.0;
      this->m_SimilarityCriteriaStandardDeviations[i]=10.0;

  }
  this->m_StandardDeviationWorldUnit = true;

  m_SmoothVelocityField  = true;
  m_SmoothUpdateField    = false;
}

// Set the moving image image.
template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::SetMovingImage(
  const MovingImageType * ptr )
{
#if (ITK_VERSION_MAJOR < 4)
  this->ProcessObject::SetNthInput( MOVING_IMAGE_CODE, const_cast<MovingImageType *>( ptr ) );
#else
  this->ProcessObject::SetNthInput( MOVING_IMAGE_CODE, const_cast<MovingImageType *>( ptr ) );
#endif
}

// Get the moving image image.
template <class TFixedImage, class TMovingImage, class TField, class TRealType>
const typename MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::MovingImageType
* MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::GetMovingImage(void) const
  {
  return dynamic_cast<const MovingImageType *>
#if (ITK_VERSION_MAJOR < 4)
         ( this->ProcessObject::GetInput( MOVING_IMAGE_CODE ) );
#else
         ( this->ProcessObject::GetInput( MOVING_IMAGE_CODE ) );
#endif
  }

// Set the fixed image.
template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::SetFixedImage(
  const FixedImageType * ptr )
{
#if (ITK_VERSION_MAJOR < 4)
  this->ProcessObject::SetNthInput( FIXED_IMAGE_CODE, const_cast<FixedImageType *>( ptr ) );
#else
  this->ProcessObject::SetNthInput( FIXED_IMAGE_CODE, const_cast<FixedImageType *>( ptr ) );
#endif
}

// Get the fixed image.
template <class TFixedImage, class TMovingImage, class TField, class TRealType>
const typename MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::FixedImageType
* MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::GetFixedImage(void) const
  {
  return dynamic_cast<const FixedImageType *>
#if (ITK_VERSION_MAJOR < 4)
         ( this->ProcessObject::GetInput( FIXED_IMAGE_CODE ) );
#else
         ( this->ProcessObject::GetInput( FIXED_IMAGE_CODE ) );
#endif
  }

template <class TFixedImage, class TMovingImage, class TField, class TRealType>
std::vector<SmartPointer<DataObject> >::size_type
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::GetNumberOfValidRequiredInputs() const
{
  typename std::vector<SmartPointer<DataObject> >::size_type num = 0;

  if( this->GetFixedImage() )
    {
    num++;
    }

  if( this->GetMovingImage() )
    {
    num++;
    }

  return num;
}

// Set the number of multi-resolution levels
template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::SetNumberOfLevels(
  unsigned int num )
{
  if( m_NumberOfLevels != num )
    {
    this->Modified();
    m_NumberOfLevels = num;
    m_NumberOfIterations.resize( m_NumberOfLevels );
    }

  if( m_MovingImagePyramid && m_MovingImagePyramid->GetNumberOfLevels() != num )
    {
    m_MovingImagePyramid->SetNumberOfLevels( m_NumberOfLevels );
    }
  if( m_FixedImagePyramid && m_FixedImagePyramid->GetNumberOfLevels() != num )
    {
    m_FixedImagePyramid->SetNumberOfLevels( m_NumberOfLevels );
    }

}

template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>::SetRegularizationType(unsigned int regularizationType)
{
    this->m_RegularizationType=regularizationType;
}

template <class TFixedImage, class TMovingImage, class TField, class TRealType>
unsigned int
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>::GetRegularizationType(void)
{
    return(this->m_RegularizationType);
}


template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>::SetHarmonicWeight(float harmonicWeight)
{
    this->m_HarmonicWeight=harmonicWeight;
}

template <class TFixedImage, class TMovingImage, class TField, class TRealType>
float
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>::GetHarmonicWeight(void)
{
    return(this->m_HarmonicWeight);
}


template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>::SetBendingWeight(float bendingWeight)
{
    this->m_BendingWeight=bendingWeight;
}

template <class TFixedImage, class TMovingImage, class TField, class TRealType>
float
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>::GetBendingWeight(void)
{
    return(this->m_BendingWeight);
}


template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>
::SetStandardDeviationsWorldUnit( double value )
{
    double tmp[ImageDimension];
    for( int i=0; i<ImageDimension; i++ )
        tmp[i] = value;
    SetStandardDeviationsWorldUnit( tmp );
}



template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>
::SetStandardDeviationsWorldUnit( double value[] )
{
    // Check if the array has changed
    unsigned int i;
    for( i=0; i<ImageDimension; i++ )
    {
        if( value[i] != m_StandardDeviations[i] )
            break;
    }

    // Eventually set the values
    if( i<ImageDimension )
    {
        for( i=0; i<ImageDimension; i++ )
            m_StandardDeviations[i] = value[i];
        this->Modified();
    }

    // Eventually update the unit
    if ( !this->m_StandardDeviationWorldUnit )
    {
        this->m_StandardDeviationWorldUnit = true;
        this->Modified();
    }
}



template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>
::SetStandardDeviationsVoxelUnit( double value )
{
    double tmp[ImageDimension];
    for( int i=0; i<ImageDimension; i++ )
        tmp[i] = value;
    SetStandardDeviationsVoxelUnit( tmp );
}



template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>
::SetStandardDeviationsVoxelUnit( double value[] )
{
    // Check if the array has changed
    unsigned int i;
    for( i=0; i<ImageDimension; i++ )
    {
        if( value[i] != m_StandardDeviations[i] )
            break;
    }

    // Eventually set the values
    if( i<ImageDimension )
    {
        for( i=0; i<ImageDimension; i++ )
            m_StandardDeviations[i] = value[i];
        this->Modified();
    }

    // Eventually update the unit
    if ( this->m_StandardDeviationWorldUnit )
    {
        this->m_StandardDeviationWorldUnit = false;
        this->Modified();
    }
}



template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>
::SetUpdateFieldStandardDeviationsWorldUnit( double value )
{
    double tmp[ImageDimension];
    for( int i=0; i<ImageDimension; i++ )
        tmp[i] = value;
    SetUpdateFieldStandardDeviationsWorldUnit( tmp );
}



template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>
::SetUpdateFieldStandardDeviationsWorldUnit( double value[] )
{
    // Check if the array has changed
    unsigned int i;
    for( i=0; i<ImageDimension; i++ )
    {
        if( value[i] != m_UpdateFieldStandardDeviations[i] )
            break;
    }

    // Eventually set the values
    if( i<ImageDimension )
    {
        for( i=0; i<ImageDimension; i++ )
            m_UpdateFieldStandardDeviations[i] = value[i];
        this->Modified();
    }

    // Eventually update the unit
    if ( !this->m_StandardDeviationWorldUnit )
    {
        this->m_StandardDeviationWorldUnit = true;
        this->Modified();
    }
}



template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>
::SetUpdateFieldStandardDeviationsVoxelUnit( double value )
{
    double tmp[ImageDimension];
    for( int i=0; i<ImageDimension; i++ )
        tmp[i] = value;
    SetUpdateFieldStandardDeviationsVoxelUnit( tmp );
}



template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage,TMovingImage,TField,TRealType>
::SetUpdateFieldStandardDeviationsVoxelUnit( double value[] )
{
    // Check if the array has changed
    unsigned int i;
    for( i=0; i<ImageDimension; i++ )
    {
        if( value[i] != m_UpdateFieldStandardDeviations[i] )
            break;
    }

    // Eventually set the values
    if( i<ImageDimension )
    {
        for( i=0; i<ImageDimension; i++ )
            m_UpdateFieldStandardDeviations[i] = value[i];
        this->Modified();
    }

    // Eventually update the unit
    if ( this->m_StandardDeviationWorldUnit )
    {
        this->m_StandardDeviationWorldUnit = false;
        this->Modified();
    }
}





// Standard PrintSelf method.
template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "NumberOfLevels: " << m_NumberOfLevels << std::endl;
  os << indent << "CurrentLevel: " << m_CurrentLevel << std::endl;

  os << indent << "NumberOfIterations: [";
  unsigned int ilevel;
  for( ilevel = 0; ilevel < m_NumberOfLevels - 1; ilevel++ )
    {
    os << m_NumberOfIterations[ilevel] << ", ";
    }
  os << m_NumberOfIterations[ilevel] << "]" << std::endl;

  os << indent << "RegistrationFilter: ";
  os << m_RegistrationFilter.GetPointer() << std::endl;
  os << indent << "MovingImagePyramid: ";
  os << m_MovingImagePyramid.GetPointer() << std::endl;
  os << indent << "FixedImagePyramid: ";
  os << m_FixedImagePyramid.GetPointer() << std::endl;

  os << indent << "FieldExpander: ";
  os << m_FieldExpander.GetPointer() << std::endl;

  os << indent << "StopRegistrationFlag: ";
  os << m_StopRegistrationFlag << std::endl;

  os << indent << "Exponentiator: ";
  os << m_Exponentiator << std::endl;

}

/*
 * Perform a the deformable registration using a multiresolution scheme
 * using an internal mini-pipeline
 *
 *  ref_pyramid ->  registrator  ->  field_expander --|| tempField
 * test_pyramid ->           |                              |
 *                           |                              |
 *                           --------------------------------
 *
 * A tempField image is used to break the cycle between the
 * registrator and field_expander.*/
template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::GenerateData()
{
  // Check for NULL images and pointers
  MovingImageConstPointer movingImage = this->GetMovingImage();
  FixedImageConstPointer  fixedImage = this->GetFixedImage();

  if( !movingImage || !fixedImage )
    {
    itkExceptionMacro( << "Fixed and/or moving image not set" );
    }

  if( !m_MovingImagePyramid || !m_FixedImagePyramid )
    {
    itkExceptionMacro( << "Fixed and/or moving pyramid not set" );
    }

  if( !m_RegistrationFilter )
    {
    itkExceptionMacro( << "Registration filter not set" );
    }

#if (ITK_VERSION_MAJOR < 4)
  if( this->m_InitialVelocityField && this->GetInput(VELOCITYFIELD_IMAGE_CODE) )
#else
  if( this->m_InitialVelocityField && this->GetInput(VELOCITYFIELD_IMAGE_CODE) )
#endif
    {
    itkExceptionMacro( << "Only one initial velocity can be given. "
                       << "SetInitialVelocityField should not be used in "
                       << "cunjunction with SetArbitraryInitialVelocityField "
                       << "or SetInput.");
    }

  // Create the image pyramids.
  m_MovingImagePyramid->SetInput( movingImage );
  m_MovingImagePyramid->UpdateLargestPossibleRegion();

  m_FixedImagePyramid->SetInput( fixedImage );
  m_FixedImagePyramid->UpdateLargestPossibleRegion();

  // Initializations
  m_CurrentLevel = 0;
  m_StopRegistrationFlag = false;

  unsigned int movingLevel = std::min( (int) m_CurrentLevel,
                                           (int) m_MovingImagePyramid->GetNumberOfLevels() );

  unsigned int fixedLevel = std::min( (int) m_CurrentLevel,
                                          (int) m_FixedImagePyramid->GetNumberOfLevels() );

  VelocityFieldPointer tempField = nullptr;

  VelocityFieldPointer inputPtr =
#if (ITK_VERSION_MAJOR < 4)
    const_cast<VelocityFieldType *>( this->GetInput(VELOCITYFIELD_IMAGE_CODE) );
#else
    const_cast<VelocityFieldType *>( this->GetInput(VELOCITYFIELD_IMAGE_CODE) );
#endif
  if( this->m_InitialVelocityField )
    {
    tempField = this->m_InitialVelocityField;
    }
  else if( inputPtr )
    {
    // Arbitrary initial velocity field is set.
    // smooth it and resample

    // First smooth it
    tempField = inputPtr;

    typedef RecursiveGaussianImageFilter<VelocityFieldType,
                                         VelocityFieldType> GaussianFilterType;
    typename GaussianFilterType::Pointer smoother
      = GaussianFilterType::New();
    for( unsigned int dim = 0; dim < VelocityFieldType::ImageDimension; ++dim )
      {
      // sigma accounts for the subsampling of the pyramid
      double sigma = 0.5 * static_cast<float>(
          m_FixedImagePyramid->GetSchedule()[fixedLevel][dim] );

      // but also for a possible discrepancy in the spacing
      sigma *= fixedImage->GetSpacing()[dim]
        / inputPtr->GetSpacing()[dim];

      smoother->SetInput( tempField );
      smoother->SetSigma( sigma );
      smoother->SetDirection( dim );

      smoother->Update();

      tempField = smoother->GetOutput();
      tempField->DisconnectPipeline();
      }

    // Now resample
    m_FieldExpander->SetInput( tempField );

    typename FloatImageType::Pointer fi =
      m_FixedImagePyramid->GetOutput( fixedLevel );
    m_FieldExpander->SetSize(
      fi->GetLargestPossibleRegion().GetSize() );
    m_FieldExpander->SetOutputStartIndex(
      fi->GetLargestPossibleRegion().GetIndex() );
    m_FieldExpander->SetOutputOrigin( fi->GetOrigin() );
    m_FieldExpander->SetOutputSpacing( fi->GetSpacing() );
    m_FieldExpander->SetOutputDirection( fi->GetDirection() );

    m_FieldExpander->UpdateLargestPossibleRegion();
    m_FieldExpander->SetInput( NULL );
    tempField = m_FieldExpander->GetOutput();
    tempField->DisconnectPipeline();
    }

  bool lastShrinkFactorsAllOnes = false;

  m_RegistrationFilter->SetRegularizationType(this->m_RegularizationType);

  if (m_RegularizationType==0)
    {
     // Mode for smoothing or not the velocity field
     if ( this->m_SmoothVelocityField )
       {
        m_RegistrationFilter->SmoothVelocityFieldOn();
       }
     else
        m_RegistrationFilter->SmoothVelocityFieldOff();

     // Mode for smoothing or not the velocity field
     if ( this->m_SmoothUpdateField )
       m_RegistrationFilter->SmoothUpdateFieldOn();
     else
       m_RegistrationFilter->SmoothUpdateFieldOff();

     // Sets the filter standard deviation for global field regularization (voxel unit only)
     if ( !m_StandardDeviationWorldUnit )
         m_RegistrationFilter->SetStandardDeviationsVoxelUnit( m_StandardDeviations );

     // Sets the filter standard deviation for update field regularization (voxel unit only)
     if ( !m_StandardDeviationWorldUnit )
        m_RegistrationFilter->SetUpdateFieldStandardDeviationsVoxelUnit( m_UpdateFieldStandardDeviations );
  }

else if (m_RegularizationType==1)
  {
   // Set penalization for the harmonic energy term
   m_RegistrationFilter->SetHarmonicWeight(this->m_HarmonicWeight);

   // Set penalization for the bending energy term
   m_RegistrationFilter->SetBendingWeight(this->m_BendingWeight);
  }

  while( !this->Halt() )
    {

    if( tempField.IsNull() )
      {
      m_RegistrationFilter->SetInitialVelocityField( NULL );
      }
    else
      {
      // Resample the field to be the same size as the fixed image
      // at the current level
      m_FieldExpander->SetInput( tempField );

      typename FloatImageType::Pointer fi =
        m_FixedImagePyramid->GetOutput( fixedLevel );
      m_FieldExpander->SetSize(
        fi->GetLargestPossibleRegion().GetSize() );
      m_FieldExpander->SetOutputStartIndex(
        fi->GetLargestPossibleRegion().GetIndex() );
      m_FieldExpander->SetOutputOrigin( fi->GetOrigin() );
      m_FieldExpander->SetOutputSpacing( fi->GetSpacing() );
      m_FieldExpander->SetOutputDirection( fi->GetDirection() );

      m_FieldExpander->UpdateLargestPossibleRegion();
      m_FieldExpander->SetInput( NULL );
      tempField = m_FieldExpander->GetOutput();
      tempField->DisconnectPipeline();

      m_RegistrationFilter->SetInitialVelocityField( tempField );

      }

    // setup registration filter and pyramids
    m_RegistrationFilter->SetMovingImage( m_MovingImagePyramid->GetOutput(movingLevel) );
    m_RegistrationFilter->SetFixedImage( m_FixedImagePyramid->GetOutput(fixedLevel) );

    m_RegistrationFilter->SetNumberOfIterations(
      m_NumberOfIterations[m_CurrentLevel] );

    // Resolution
    double resolution = 1;
    for (int i=0; i<ImageDimension; i++)
    {
        double s1  = fixedImage->GetLargestPossibleRegion().GetSize()[i];
        double s2  = m_FixedImagePyramid->GetOutput(movingLevel)->GetLargestPossibleRegion().GetSize()[i];
        resolution = std::max( resolution, s1/s2 );
    }


    // Sets the filter standard deviation for global field regularization (world unit only)
    if ( m_StandardDeviationWorldUnit )
    {
        double sd[ImageDimension];
        for (int i=0; i<ImageDimension; i++)
            sd[i] = m_StandardDeviations[i] * resolution;
        m_RegistrationFilter->SetStandardDeviationsWorldUnit( sd );

    // Sets the filter standard deviation for update field regularization (world unit only)

        for (int i=0; i<ImageDimension; i++)
            sd[i] = m_UpdateFieldStandardDeviations[i] * resolution;
        m_RegistrationFilter->SetUpdateFieldStandardDeviationsWorldUnit( sd );
    }




    // cache shrink factors for computing the next expand factors.
    lastShrinkFactorsAllOnes = true;
    for( unsigned int idim = 0; idim < ImageDimension; idim++ )
      {
      if( m_FixedImagePyramid->GetSchedule()[fixedLevel][idim] > 1 )
        {
        lastShrinkFactorsAllOnes = false;
        break;
        }
      }

    // compute new velocity field
    m_RegistrationFilter->UpdateLargestPossibleRegion();
    tempField = m_RegistrationFilter->GetOutput();
    tempField->DisconnectPipeline();

    // Increment level counter.
    m_CurrentLevel++;
    movingLevel = std::min( (int) m_CurrentLevel,
                                (int) m_MovingImagePyramid->GetNumberOfLevels() );
    fixedLevel = std::min( (int) m_CurrentLevel,
                               (int) m_FixedImagePyramid->GetNumberOfLevels() );

    // Invoke an iteration event.
    this->InvokeEvent( IterationEvent() );

    // We can release data from pyramid which are no longer required.
    if( movingLevel > 0 )
      {
      m_MovingImagePyramid->GetOutput( movingLevel - 1 )->ReleaseData();
      }
    if( fixedLevel > 0 )
      {
      m_FixedImagePyramid->GetOutput( fixedLevel - 1 )->ReleaseData();
      }

    } // while not Halt()

  if( !lastShrinkFactorsAllOnes )
    {
    // Some of the last shrink factors are not one
    // graft the output of the expander filter to
    // to output of this filter

    // resample the field to the same size as the fixed image
    m_FieldExpander->SetInput( tempField );
    m_FieldExpander->SetSize(
      fixedImage->GetLargestPossibleRegion().GetSize() );
    m_FieldExpander->SetOutputStartIndex(
      fixedImage->GetLargestPossibleRegion().GetIndex() );
    m_FieldExpander->SetOutputOrigin( fixedImage->GetOrigin() );
    m_FieldExpander->SetOutputSpacing( fixedImage->GetSpacing() );
    m_FieldExpander->SetOutputDirection( fixedImage->GetDirection() );

    m_FieldExpander->UpdateLargestPossibleRegion();
    this->GraftOutput( m_FieldExpander->GetOutput() );
    }
  else
    {
    // all the last shrink factors are all ones
    // graft the output of registration filter to
    // to output of this filter
    this->GraftOutput( tempField );
    }

  // Release memory
  m_FieldExpander->SetInput( NULL );
  m_FieldExpander->GetOutput()->ReleaseData();
  m_RegistrationFilter->SetInput( NULL );
  m_RegistrationFilter->GetOutput()->ReleaseData();

}

template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::StopRegistration()
{
  m_RegistrationFilter->StopRegistration();
  m_StopRegistrationFlag = true;
}

template <class TFixedImage, class TMovingImage, class TField, class TRealType>
bool
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::Halt()
{
  // Halt the registration after the user-specified number of levels
  if( m_NumberOfLevels != 0 )
    {
    this->UpdateProgress( static_cast<float>( m_CurrentLevel )
                          / static_cast<float>( m_NumberOfLevels ) );
    }

  if( m_CurrentLevel >= m_NumberOfLevels )
    {
    return true;
    }
  if( m_StopRegistrationFlag )
    {
    return true;
    }
  else
    {
    return false;
    }

}

template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::GenerateOutputInformation()
{

  typename DataObject::Pointer output;

#if (ITK_VERSION_MAJOR < 4)
  if( this->GetInput(VELOCITYFIELD_IMAGE_CODE) )
#else
  if( this->GetInput(VELOCITYFIELD_IMAGE_CODE) )
#endif
    {
    // Initial velocity field is set.
    // Copy information from initial field.
    this->Superclass::GenerateOutputInformation();

    }
  else if( this->GetFixedImage() )
    {
    // Initial deforamtion field is not set.
    // Copy information from the fixed image.
    for( unsigned int idx = 0; idx <
         this->GetNumberOfOutputs(); ++idx )
      {
      output = this->GetOutput(idx);
      if( output )
        {
        output->CopyInformation(this->GetFixedImage() );
        }
      }

    }

}

template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::GenerateInputRequestedRegion()
{

  // call the superclass's implementation
  Superclass::GenerateInputRequestedRegion();

  // request the largest possible region for the moving image
  MovingImagePointer movingPtr =
    const_cast<MovingImageType *>( this->GetMovingImage() );
  if( movingPtr )
    {
    movingPtr->SetRequestedRegionToLargestPossibleRegion();
    }

  // just propagate up the output requested region for
  // the fixed image and initial velocity field.
  VelocityFieldPointer inputPtr =
#if (ITK_VERSION_MAJOR < 4)
    const_cast<VelocityFieldType *>( this->GetInput(VELOCITYFIELD_IMAGE_CODE) );
#else
    const_cast<VelocityFieldType *>( this->GetInput(VELOCITYFIELD_IMAGE_CODE) );
#endif
  VelocityFieldPointer outputPtr = this->GetOutput();
  FixedImagePointer    fixedPtr =
    const_cast<FixedImageType *>( this->GetFixedImage() );

  if( inputPtr )
    {
    inputPtr->SetRequestedRegion( outputPtr->GetRequestedRegion() );
    }

  if( fixedPtr )
    {
    fixedPtr->SetRequestedRegion( outputPtr->GetRequestedRegion() );
    }

}

template <class TFixedImage, class TMovingImage, class TField, class TRealType>
void
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::EnlargeOutputRequestedRegion(
  DataObject * ptr )
{
  // call the superclass's implementation
  Superclass::EnlargeOutputRequestedRegion( ptr );

  // set the output requested region to largest possible.
  VelocityFieldType * outputPtr;
  outputPtr = dynamic_cast<VelocityFieldType *>( ptr );

  if( outputPtr )
    {
    outputPtr->SetRequestedRegionToLargestPossibleRegion();
    }

}

template <class TFixedImage, class TMovingImage, class TField, class TRealType>
typename MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::DeformationFieldPointer
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::GetDeformationField()
{
  // std::cout<<"MultiResolutionLogDomainDeformableRegistration::GetDeformationField"<<std::endl;
  m_Exponentiator->SetInput( this->GetVelocityField() );
  m_Exponentiator->ComputeInverseOff();
  m_Exponentiator->Update();
  DeformationFieldPointer field = m_Exponentiator->GetOutput();
  field->DisconnectPipeline();
  return field;
}

template <class TFixedImage, class TMovingImage, class TField, class TRealType>
typename MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::DeformationFieldPointer
MultiResolutionLogDomainDeformableRegistration<TFixedImage, TMovingImage, TField, TRealType>
::GetInverseDisplacementField()
{
  // std::cout<<"MultiResolutionLogDomainDeformableRegistration::GetInverseDisplacementField"<<std::endl;
  m_Exponentiator->SetInput( this->GetVelocityField() );
  m_Exponentiator->ComputeInverseOn();
  m_Exponentiator->Update();
  DeformationFieldPointer field = m_Exponentiator->GetOutput();
  field->DisconnectPipeline();
  // Reset compute inverse back to off to avoid some broder effects
  m_Exponentiator->ComputeInverseOff();
  return field;
}

} // end namespace itk

#endif
