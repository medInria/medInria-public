#ifndef __itkLCCDeformableRegistrationFilter_txx
#define __itkLCCDeformableRegistrationFilter_txx

#include "itkLCCDeformableRegistrationFilter.h"

#include "itkMacro.h"
#include "itkImageRegionIterator.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkDataObject.h"

#include "itkGaussianOperator.h"
#include "itkVectorNeighborhoodOperatorImageFilter.h"

#include "vnl/vnl_math.h"


#include "itkImageFileWriter.h"
// REQUIRES FFTW, CURRENTLY NOT ACTIVE
//#include "itkVectorRegularizationFilter.h"

namespace itk {
// Default constructor
template <class TFixedImage, class TMovingImage, class TField>
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::LCCDeformableRegistrationFilter()
{
#if (ITK_VERSION_MAJOR < 4)
    this->SetNumberOfRequiredInputs(2);
#else
  //HACK:  This really should define the names of the required inputs.
  this->SetNumberOfIndexedInputs(2);
  // Primary input is optional in this filter
  this->RemoveRequiredInputName( "Primary" );
#endif

    this->SetNumberOfIterations(10);

    m_RegularizationType = 1;

    m_HarmonicWeight   = 1e-4;
    m_BendingWeight    = 1e-5;

    unsigned int j;
    for( j = 0; j < ImageDimension; j++ )
    {
        m_StandardDeviations[j] = 1.0;
        m_UpdateFieldStandardDeviations[j] = 1.0;
	m_SimilarityCriteriaStandardDeviations[j]=10.0;
    }
    m_StandardDeviationWorldUnit = true;

    m_TempField = VelocityFieldType::New();
    m_MaximumError = 0.1;
    m_MaximumKernelWidth = 30;
    m_StopRegistrationFlag = false;

    m_SmoothVelocityField = true;
    m_SmoothUpdateField = false;

    m_Exponentiator = FieldExponentiatorType::New();
    m_Exponentiator->ComputeInverseOff();

    m_InverseExponentiator = FieldExponentiatorType::New();
    m_InverseExponentiator->ComputeInverseOn();

    m_BoundaryCheck     = true;

}

template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage, TMovingImage, TField>
::SetInitialVelocityField( VelocityFieldType * ptr )
{
#if (ITK_VERSION_MAJOR < 4)
  this->SetNthInput( 0, ptr );
#else
  this->SetNthInput( 0, ptr );
#endif
}


// Set the fixed image.
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetFixedImage(
  const FixedImageType * ptr )
{
#if (ITK_VERSION_MAJOR < 4)
  this->ProcessObject::SetNthInput( 1, const_cast< FixedImageType * >( ptr ) );
#else
  this->ProcessObject::SetNthInput( 1, const_cast<FixedImageType *>( ptr ) );
#endif
}


// Get the fixed image.
template <class TFixedImage, class TMovingImage, class TField>
const typename LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::FixedImageType *
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetFixedImage() const
{
  return dynamic_cast< const FixedImageType * >
#if (ITK_VERSION_MAJOR < 4)
    ( this->ProcessObject::GetInput( 1 ) );
#else
         ( this->ProcessObject::GetInput( 1 ) );
#endif
}


// Set the moving image.
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetMovingImage(
  const MovingImageType * ptr )
{
#if (ITK_VERSION_MAJOR < 4)
  this->ProcessObject::SetNthInput( 2, const_cast< MovingImageType * >( ptr ) );
#else
  this->ProcessObject::SetNthInput( 2, const_cast<MovingImageType *>( ptr ) );
#endif
}


// Get the moving image.
template <class TFixedImage, class TMovingImage, class TField>
const typename LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::MovingImageType *
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetMovingImage() const
{
  return dynamic_cast< const MovingImageType * >
#if (ITK_VERSION_MAJOR < 4)
    ( this->ProcessObject::GetInput( 2 ) );
#else
         ( this->ProcessObject::GetInput( 2 ) );
#endif
}

// Set the flag for the mask image.
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::UseMask(bool flag )
{
   m_UseMask = flag;
}

// Get the flag for the mask image.
template <class TFixedImage, class TMovingImage, class TField>
bool
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetUseMask(void )
{
   return(m_UseMask);
}

// Set the mask image.
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetMaskImage(
  const MovingImageType * ptr )
{ m_MaskImage = MovingImageType::New(); 
  m_MaskImage =ptr;
}


// Get the mask image.
template <class TFixedImage, class TMovingImage, class TField>
const typename LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::MovingImageType *
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetMaskImage() const
{
  return m_MaskImage;
}


//Sets Boundary Checking
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetBoundaryCheck(bool flag)
{
   this->m_BoundaryCheck=flag;
}

//Get Boundary Checking
template <class TFixedImage, class TMovingImage, class TField>
bool
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetBoundaryCheck(void)
{
   return(m_BoundaryCheck);

}


template <class TFixedImage, class TMovingImage, class TField>
std::vector<SmartPointer<DataObject> >::size_type
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetNumberOfValidRequiredInputs() const
{
  typename std::vector<SmartPointer<DataObject> >::size_type num = 0;

  if (this->GetFixedImage())
    {
    num++;
    }

  if (this->GetMovingImage())
    {
    num++;
    }
  
  return num;
}


template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>::SetRegularizationType(unsigned int regularizationType)
{
    this->m_RegularizationType=regularizationType;
}

template <class TFixedImage, class TMovingImage, class TField>
unsigned int
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>::GetRegularizationType(void)
{
    return(this->m_RegularizationType);
}


template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>::SetHarmonicWeight(float harmonicWeight)
{
    this->m_HarmonicWeight=harmonicWeight;
}

template <class TFixedImage, class TMovingImage, class TField>
float
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>::GetHarmonicWeight(void)
{
    return(this->m_HarmonicWeight);
}


template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>::SetBendingWeight(float bendingWeight)
{
    this->m_BendingWeight=bendingWeight;
}

template <class TFixedImage, class TMovingImage, class TField>
float
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>::GetBendingWeight(void)
{
    return(this->m_BendingWeight);
}



// Set the standard deviations.
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetStandardDeviations(
  double value )
{

    std::cout<< "WARNING : Deprecated function" << std::endl;

    unsigned int j;
    for( j = 0; j < ImageDimension; j++ )
    {
        if( value != m_StandardDeviations[j] )
        {
            break;
        }
    }
    if( j < ImageDimension )
    {
        this->Modified();
        for( j = 0; j < ImageDimension; j++ )
        {
            m_StandardDeviations[j] = value;
        }
    }

}


template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetStandardDeviationsWorldUnit( double value )
{
    double tmp[ImageDimension];
    for( int i=0; i<static_cast<int>(ImageDimension); i++ )
    {
        tmp[i] = value;
    }
    SetStandardDeviationsWorldUnit( tmp );
}



template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
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



template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetStandardDeviationsVoxelUnit( double value )
{
    double tmp[ImageDimension];
    for( int i=0; i<static_cast<int>(ImageDimension); i++ )
    {
        tmp[i] = value;
    }
    SetStandardDeviationsVoxelUnit( tmp );
}



template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
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


// Set the standard deviations.
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetUpdateFieldStandardDeviations(
  double value )
{

    std::cout<< "WARNING : Deprecated function" << std::endl;

  unsigned int j;
  for( j = 0; j < ImageDimension; j++ )
    {
    if( value != m_UpdateFieldStandardDeviations[j] )
      {
      break;
      }
    }
  if( j < ImageDimension )
    {
    this->Modified();
    for( j = 0; j < ImageDimension; j++ )
      {
      m_UpdateFieldStandardDeviations[j] = value;
      }
    }

}



template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetUpdateFieldStandardDeviationsWorldUnit( double value )
{
    double tmp[ImageDimension];
    for( int i=0; i<static_cast<int>(ImageDimension); i++ )
    {
        tmp[i] = value;
    }
    SetUpdateFieldStandardDeviationsWorldUnit( tmp );
}



template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
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



template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetUpdateFieldStandardDeviationsVoxelUnit( double value )
{
    double tmp[ImageDimension];
    for( int i=0; i<static_cast<int>(ImageDimension); i++ )
    {
        tmp[i] = value;
    }
    SetUpdateFieldStandardDeviationsVoxelUnit( tmp );
}



template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
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



// Set the standard deviations.
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetSimilarityCriteriaStandardDeviations(
  double value )
{

    std::cout<< "WARNING : Deprecated function" << std::endl;

  unsigned int j;
  for( j = 0; j < ImageDimension; j++ )
    {
    if( value != m_SimilarityCriteriaStandardDeviations[j] )
      {
      break;
      }
    }
  if( j < ImageDimension )
    {
    this->Modified();
    for( j = 0; j < ImageDimension; j++ )
      {
      m_SimilarityCriteriaStandardDeviations[j] = value;
      }
    }

}



template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetSimilarityCriteriaStandardDeviationsWorldUnit( double value )
{
    double tmp[ImageDimension];
    for( int i=0; i<static_cast<int>(ImageDimension); i++ )
    {
        tmp[i] = value;
    }
    SetSimilarityCriteriaStandardDeviationsWorldUnit( tmp );
}



template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetSimilarityCriteriaStandardDeviationsWorldUnit( double value[] )
{
    // Check if the array has changed
    unsigned int i;
    for( i=0; i<ImageDimension; i++ )
    {
        if( value[i] != m_SimilarityCriteriaStandardDeviations[i] )
            break;
    }

    // Eventually set the values
    if( i<ImageDimension )
    {
        for( i=0; i<ImageDimension; i++ )
            m_SimilarityCriteriaStandardDeviations[i] = value[i];
        this->Modified();
    }

    // Eventually update the unit
    if ( !this->m_StandardDeviationWorldUnit )
    {
        this->m_StandardDeviationWorldUnit = true;
        this->Modified();
    }
}



template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetSimilarityCriteriaStandardDeviationsVoxelUnit( double value )
{
    double tmp[ImageDimension];
    for( int i=0; i<static_cast<int>(ImageDimension); i++ )
    {
        tmp[i] = value;
    }
    SetSimilarityCriteriaStandardDeviationsVoxelUnit( tmp );
}



template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetSimilarityCriteriaStandardDeviationsVoxelUnit( double value[] )
{
    // Check if the array has changed
    unsigned int i;
    for( i=0; i<ImageDimension; i++ )
    {
        if( value[i] != m_SimilarityCriteriaStandardDeviations[i] )
            break;
    }

    // Eventually set the values
    if( i<ImageDimension )
    {
        for( i=0; i<ImageDimension; i++ )
            m_SimilarityCriteriaStandardDeviations[i] = value[i];
        this->Modified();
    }

    // Eventually update the unit
    if ( this->m_StandardDeviationWorldUnit )
    {
        this->m_StandardDeviationWorldUnit = false;
        this->Modified();
    }
}


template <class TFixedImage, class TMovingImage, class TField>
const double *
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetSimilarityCriteriaStandardDeviations( ) const
{   
        static double stDev[ImageDimension] ;
   for( unsigned int j = 0; j < ImageDimension; j++ )
      {

          // Computation of the standard deviation (voxel unit) of the Gaussian kernel for the similarity criteria
        if ( m_StandardDeviationWorldUnit )
        {   
	    double s = this->GetFixedImage()->GetSpacing()[j];
            stDev[j] =  this->m_SimilarityCriteriaStandardDeviations[j] / (s);
           // stDev[j] = s*s*this->m_SimilarityCriteriaStandardDeviations[j];

        }
        else
            stDev[j] =  this->m_SimilarityCriteriaStandardDeviations[j] ;
     }

      //std::cout<<"Std Dev: "<<stDev[1]<<std::endl;

      return(static_cast<const double*> (stDev)); 
  }

template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SetSigmaI(
  double value )
{
  this->m_SigmaI=value;
}

template <class TFixedImage, class TMovingImage, class TField>
const double 
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetSigmaI( ) const
{   
 return m_SigmaI;
}


// Standard PrintSelf method.
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Smooth velocity field: "
     << (m_SmoothVelocityField ? "on" : "off") << std::endl;
  os << indent << "Standard deviations: [";
  unsigned int j;
  for( j = 0; j < ImageDimension - 1; j++ )
    {
    os << m_StandardDeviations[j] << ", ";
    }
  os << m_StandardDeviations[j] << "]" << std::endl;
  os << indent << "Smooth update field: "
     << (m_SmoothUpdateField ? "on" : "off") << std::endl;
  os << indent << "Update field standard deviations: [";
  for( j = 0; j < ImageDimension - 1; j++ )
    {
    os << m_UpdateFieldStandardDeviations[j] << ", ";
    }
  os << m_UpdateFieldStandardDeviations[j] << "]" << std::endl;
  os << indent << "Similarity Criteria standard deviations: [";
  for( j = 0; j < ImageDimension - 1; j++ )
    {
    os << m_SimilarityCriteriaStandardDeviations[j] << ", ";
    }
  os << m_SimilarityCriteriaStandardDeviations[j] << "]" << std::endl;

  os << indent << "StopRegistrationFlag: ";
  os << m_StopRegistrationFlag << std::endl;
  os << indent << "MaximumError: ";
  os << m_MaximumError << std::endl;
  os << indent << "MaximumKernelWidth: ";
  os << m_MaximumKernelWidth << std::endl;
  os << indent << "Exponentiator: ";
  os << m_Exponentiator << std::endl;
  os << indent << "InverseExponentiator: ";
  os << m_InverseExponentiator << std::endl;

}


// Set the function state values before each iteration
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::InitializeIteration()
{
  //std::cout<<"LCCDeformableRegistrationFilter::InitializeIteration"<<std::endl;
  MovingImageConstPointer movingPtr = this->GetMovingImage();
  FixedImageConstPointer fixedPtr = this->GetFixedImage();

  if( !movingPtr || !fixedPtr )
    {
    itkExceptionMacro( << "Fixed and/or moving image not set" );
    }

  // update variables in the equation object
  PDEDeformableRegistrationFunctionType *f = 
    dynamic_cast<PDEDeformableRegistrationFunctionType *>
    (this->GetDifferenceFunction().GetPointer());

  if ( !f )
    {
    itkExceptionMacro(<<"FiniteDifferenceFunction not of type LCCDeformableRegistrationFilterFunction");
    }

  f->SetFixedImage( fixedPtr );
  f->SetMovingImage( movingPtr );

  this->Superclass::InitializeIteration();

}


/* Override the default implementation for the case when the 
 * initial velocity is not set.
 * If the initial velocity is not set, the output is
 * fill with zero vectors.*/
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::CopyInputToOutput()
{
#if (ITK_VERSION_MAJOR < 4)
  typename Superclass::InputImageType::ConstPointer  inputPtr  = this->GetInput(0);
#else
  typename Superclass::InputImageType::ConstPointer  inputPtr  = this->GetInput(0);
#endif

  if( inputPtr )
    {
    this->Superclass::CopyInputToOutput();
    }
  else
   {
    typename Superclass::PixelType zeros;
    for( unsigned int j = 0; j < ImageDimension; j++ )
      {
      zeros[j] = 0;
      }

    typename OutputImageType::Pointer output = this->GetOutput();
  
    ImageRegionIterator<OutputImageType> out(output, output->GetRequestedRegion());

    while( ! out.IsAtEnd() )
      {
      out.Value() =  zeros;
      ++out;
      }
    }
}


template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::GenerateOutputInformation()
{
  //std::cout<<"LCCDeformableRegistrationFilter::GenerateOutputInformation"<<std::endl;
  typename DataObject::Pointer output;

#if (ITK_VERSION_MAJOR < 4)
  if( this->GetInput(0) )
#else
  if( this->GetInput(0) )
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
    for (unsigned int idx = 0; idx < 
           this->GetNumberOfOutputs(); ++idx )
      {
      output = this->GetOutput(idx);
      if (output)
        {
        output->CopyInformation(this->GetFixedImage());
        }  
      }

    }

}


template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::GenerateInputRequestedRegion()
{
  //std::cout<<"LCCDeformableRegistrationFilter::GenerateInputRequestedRegion"<<std::endl;
  // call the superclass's implementation
  Superclass::GenerateInputRequestedRegion();

  // request the largest possible region for the moving image
  MovingImagePointer movingPtr = 
    const_cast< MovingImageType * >( this->GetMovingImage() );
  if( movingPtr )
    {
    movingPtr->SetRequestedRegionToLargestPossibleRegion();
    }
  
  // just propagate up the output requested region for
  // the fixed image and initial velocity field.
#if (ITK_VERSION_MAJOR < 4)
  VelocityFieldPointer inputPtr = 
    const_cast< VelocityFieldType * >( this->GetInput(0) );
#else
  VelocityFieldPointer inputPtr =
    const_cast<VelocityFieldType *>( this->GetInput(0) );
#endif

  VelocityFieldPointer outputPtr = this->GetOutput();
  FixedImagePointer fixedPtr = 
    const_cast< FixedImageType *>( this->GetFixedImage() );

  if( inputPtr )
    {
    inputPtr->SetRequestedRegion( outputPtr->GetRequestedRegion() );
    }

  if( fixedPtr )
    {
    fixedPtr->SetRequestedRegion( outputPtr->GetRequestedRegion() );
    }
}


// Release memory of internal buffers
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::PostProcessOutput()
{
  this->Superclass::PostProcessOutput();
  m_TempField->Initialize();
}


// Initialize flags
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::Initialize()
{
  //std::cout<<"LCCDeformableRegistrationFilter::Initialize"<<std::endl;
  this->Superclass::Initialize();
  m_StopRegistrationFlag = false;
}


// Smooth velocity using a separable Gaussian kernel
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SmoothVelocityField()
{ 

  if (m_RegularizationType == 1)
    {

      /*
      float param[2];
       param[0]=m_HarmonicWeight;
       param[1]=m_BendingWeight;

       typedef typename itk::VectorRegularizationFilter<VelocityFieldType> RegularizerType;
       //std::cout<<param[0]<<" "<<param[1]<<std::endl;

       typename RegularizerType::Pointer Regularizer=RegularizerType::New();
       Regularizer->SetInput(this->GetOutput());
       Regularizer->SetFactor(param);

       Regularizer->Update();

       this->GraftOutput(Regularizer->GetOutput());
       */
      }
  else if (m_RegularizationType == 0)
    {
     if (this->GetSmoothVelocityField())
        {
         // The output buffer will be overwritten with new data.
         this->SmoothGivenField(this->GetOutput(), this->m_StandardDeviations);
        }
    }
 
}

// Smooth update field using a separable Gaussian kernel
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SmoothUpdateField()
{

 if (m_RegularizationType == 0)
  {
    /*double variance[ImageDimension];
    for( unsigned int j = 0; j < ImageDimension; j++ )
      {
       double s = this->GetUpdateBuffer()->GetSpacing()[j];
       variance[j] = this->m_UpdateFieldStandardDeviations[j]/(s) ;
      }*/
    this->SmoothGivenField(this->GetUpdateBuffer(), this->m_UpdateFieldStandardDeviations);//this->m_UpdateFieldStandardDeviations);
   }
 else {}

}


// Smooth velocity using a separable Gaussian kernel
template <class TFixedImage, class TMovingImage, class TField>
void
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::SmoothGivenField(VelocityFieldType * field, const double StandardDeviations[ImageDimension])
{

    // copy field to TempField
    m_TempField->SetOrigin( field->GetOrigin() );
    m_TempField->SetSpacing( field->GetSpacing() );
    m_TempField->SetDirection( field->GetDirection() );
    m_TempField->SetLargestPossibleRegion(
            field->GetLargestPossibleRegion() );
    m_TempField->SetRequestedRegion(
            field->GetRequestedRegion() );
    m_TempField->SetBufferedRegion( field->GetBufferedRegion() );
    m_TempField->Allocate();

    typedef typename VelocityFieldType::PixelType       VectorType;
    typedef typename VectorType::ValueType              ScalarType;
    typedef GaussianOperator<ScalarType,ImageDimension> OperatorType;
    typedef VectorNeighborhoodOperatorImageFilter<
            VelocityFieldType,
            VelocityFieldType>                                SmootherType;

    OperatorType * oper = new OperatorType;
    typename SmootherType::Pointer smoother = SmootherType::New();

    typedef typename VelocityFieldType::PixelContainerPointer
            PixelContainerPointer;
    PixelContainerPointer swapPtr;

    // graft the output field onto the mini-pipeline
    smoother->GraftOutput( m_TempField );

    for( unsigned int j = 0; j < ImageDimension; j++ )
    {
        // smooth along this dimension
        oper->SetDirection( j );

        // Computation of the variance (voxel unit) of the Gaussian kernel
        double variance;
        if ( this->m_StandardDeviationWorldUnit )
        {   double s = field->GetSpacing()[j];
            variance = ( StandardDeviations[j] * StandardDeviations[j] ) / (s*s);
        }
        else
            variance = StandardDeviations[j] * StandardDeviations[j];

        //std::cout << "Standard deviation : " << StandardDeviations[j] << std::endl;

        // Set other parameters
        oper->SetVariance( variance );
        oper->SetMaximumError( m_MaximumError );
        oper->SetMaximumKernelWidth( m_MaximumKernelWidth );
        oper->CreateDirectional();

        // todo: make sure we only smooth within the buffered region
        smoother->SetOperator( *oper );
        smoother->SetInput( field );
        smoother->Update();

        if ( j < ImageDimension - 1 )
        {
            // swap the containers
            swapPtr = smoother->GetOutput()->GetPixelContainer();
            smoother->GraftOutput( field );
            field->SetPixelContainer( swapPtr );
            smoother->Modified();
        }

    }

    // graft the output back to field
    m_TempField->SetPixelContainer( field->GetPixelContainer() );

    // field to contain the final smoothed data, do the equivalent of a graft
    field->SetPixelContainer( smoother->GetOutput()->GetPixelContainer() );
    field->SetRequestedRegion( smoother->GetOutput()->GetRequestedRegion() );
    field->SetBufferedRegion( smoother->GetOutput()->GetBufferedRegion() );
    field->SetLargestPossibleRegion( smoother->GetOutput()->GetLargestPossibleRegion() );
    field->CopyInformation( smoother->GetOutput() );

    delete oper;

}


template <class TFixedImage, class TMovingImage, class TField>
typename LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::DeformationFieldPointer
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetDeformationField()
{ 

  //std::cout<<"LCCDeformableRegistration::GetDeformationField"<<std::endl;
  m_Exponentiator->SetInput( this->GetVelocityField() );
  m_Exponentiator->GetOutput()->SetRequestedRegion( this->GetVelocityField()->GetRequestedRegion() );
  m_Exponentiator->Update();
  return m_Exponentiator->GetOutput();
}


template <class TFixedImage, class TMovingImage, class TField>
typename LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::DeformationFieldPointer
LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetInverseDeformationField()
{
  //std::cout<<"LCCDeformableRegistration::GetInverseDeformationField"<<std::endl;
  m_InverseExponentiator->SetInput( this->GetVelocityField() );
  m_InverseExponentiator->GetOutput()->SetRequestedRegion( this->GetVelocityField()->GetRequestedRegion() );
  m_InverseExponentiator->Update();
  return m_InverseExponentiator->GetOutput();
}


} // end namespace itk

#endif
