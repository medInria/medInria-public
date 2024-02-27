#ifndef __itkSymmetricLCClogDemonsRegistrationFilter_txx
#define __itkSymmetricLCClogDemonsRegistrationFilter_txx

#include "itkSymmetricLCClogDemonsRegistrationFilter.h"

#include "itkOppositeImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkVelocityFieldBCHCompositionFilter.h"
#include "itkImageFileWriter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkDivideImageFilter.h"


namespace itk {

// Default constructor
template <class TFixedImage, class TMovingImage, class TField>
SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::SymmetricLCClogDemonsRegistrationFilter()
{
  DemonsRegistrationFunctionPointer drfpf = DemonsRegistrationFunctionType::New();
  this->SetDifferenceFunction( drfpf );
  DemonsRegistrationFunctionPointer drfpb = DemonsRegistrationFunctionType::New();
  this->SetBackwardDifferenceFunction( drfpb );
  
  m_Multiplier = MultiplyByConstantType::New();
  m_Multiplier->InPlaceOn();
 
  m_Adder = AdderType::New();
  m_Adder->InPlaceOn();

  // Set number of terms in the BCH approximation to default value
  m_NumberOfBCHApproximationTerms = 2;

  m_BackwardUpdateBuffer = 0;
}


// Checks whether the DifferenceFunction is of type DemonsRegistrationFunction.
template <class TFixedImage, class TMovingImage, class TField>
typename SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::DemonsRegistrationFunctionType*
SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetForwardRegistrationFunctionType()
{
  DemonsRegistrationFunctionType *drfp = 
    dynamic_cast<DemonsRegistrationFunctionType *>(this->GetDifferenceFunction().GetPointer());
 
  if( !drfp )
    {
    itkExceptionMacro( << "Could not cast difference function to SymmetricDemonsRegistrationFunction" );
    }

  return drfp;
}


// Checks whether the DifferenceFunction is of type DemonsRegistrationFunction.
template <class TFixedImage, class TMovingImage, class TField>
const typename SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::DemonsRegistrationFunctionType*
SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetForwardRegistrationFunctionType() const
{
  const DemonsRegistrationFunctionType *drfp = 
    dynamic_cast<const DemonsRegistrationFunctionType *>(this->GetDifferenceFunction().GetPointer());
 
  if( !drfp )
    {
    itkExceptionMacro( << "Could not cast difference function to SymmetricDemonsRegistrationFunction" );
    }

  return drfp;
}


// Checks whether the DifferenceFunction is of type DemonsRegistrationFunction.
template <class TFixedImage, class TMovingImage, class TField>
typename SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::DemonsRegistrationFunctionType*
SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetBackwardRegistrationFunctionType()
{
  DemonsRegistrationFunctionType *drfp = 
    dynamic_cast<DemonsRegistrationFunctionType *>(this->GetBackwardDifferenceFunction().GetPointer());
 
  if( !drfp )
    {
    itkExceptionMacro( << "Could not cast difference function to SymmetricDemonsRegistrationFunction" );
    }

  return drfp;
}


// Checks whether the DifferenceFunction is of type DemonsRegistrationFunction.
template <class TFixedImage, class TMovingImage, class TField>
const typename SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::DemonsRegistrationFunctionType*
SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetBackwardRegistrationFunctionType() const
{
  const DemonsRegistrationFunctionType *drfp = 
    dynamic_cast<const DemonsRegistrationFunctionType *>(this->GetBackwardDifferenceFunction().GetPointer());
 
  if( !drfp )
    {
    itkExceptionMacro( << "Could not cast difference function to SymmetricDemonsRegistrationFunction" );
    }

  return drfp;
}


// Set the function state values before each iteration
template <class TFixedImage, class TMovingImage, class TField>
void
SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::InitializeIteration()
{
  // update variables in the equation object
  DemonsRegistrationFunctionType *f = this->GetForwardRegistrationFunctionType();

#if (ITK_VERSION_MAJOR < 4)
  f->SetDisplacementField( this->GetDeformationField() );
#else
  f->SetDisplacementField( this->GetDeformationField() );
#endif

  f->SetSigma(this->GetSimilarityCriteriaStandardDeviations());
  f->SetInverseDeformationField( this->GetInverseDeformationField() );
  f->SetSigmaI(this->GetSigmaI());
  f->SetBoundaryCheck(this->GetBoundaryCheck());

  if (this->GetUseMask())
   {
    f->UseMask(true);
    f->SetMaskImage(this->GetMaskImage());
   }
  else f->UseMask(false);


  // call the superclass  implementation ( initializes f )
  Superclass::InitializeIteration();

}


// Initialize flags
template <class TFixedImage, class TMovingImage, class TField>
void
SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::Initialize()
{
  this->Superclass::Initialize();

  const FixedImageType * fixim = this->GetFixedImage();
  const MovingImageType * movim = this->GetMovingImage();

  if ( fixim==0 || movim==0 )
    {
    itkExceptionMacro( << "A fixed and a moving image are required" );
    }

  if ( fixim->GetLargestPossibleRegion() != movim->GetLargestPossibleRegion() )
    {
    itkExceptionMacro( << "Registering images that have diffent sizes is not supported yet." );
    }

  if ( (fixim->GetSpacing() - movim->GetSpacing()).GetNorm() > 1e-10 )
    {
    itkExceptionMacro( << "Registering images that have diffent spacing is not supported yet." );
    }

  if ( (fixim->GetOrigin() - movim->GetOrigin()).GetNorm() > 1e-10 )
    {
    itkExceptionMacro( << "Registering images that have diffent origins is not supported yet." );
    }
}


// Get the metric value from the difference function
template <class TFixedImage, class TMovingImage, class TField>
double
SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::GetMetric() const
{
  const DemonsRegistrationFunctionType *drfpf = this->GetForwardRegistrationFunctionType();
     return (drfpf->GetMetric());

}



// Allocate storage in m_UpdateBuffer
template <class TFixedImage, class TMovingImage, class TField>
void
SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::AllocateUpdateBuffer()
{
  Superclass::AllocateUpdateBuffer();
}


template <class TFixedImage, class TMovingImage, class TField>
typename
SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>::TimeStepType
SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::ThreadedCalculateChange(const ThreadRegionType &regionToProcess, ThreadIdType)
{
  typedef typename VelocityFieldType::RegionType      RegionType;
  typedef typename VelocityFieldType::SizeType        SizeType;
  typedef typename VelocityFieldType::SizeValueType   SizeValueType;
  typedef typename VelocityFieldType::IndexType       IndexType;
  typedef typename VelocityFieldType::IndexValueType  IndexValueType;
  typedef typename 
    FiniteDifferenceFunctionType::NeighborhoodType    NeighborhoodIteratorType;
  typedef ImageRegionIterator<VelocityFieldType>      UpdateIteratorType;

  VelocityFieldPointer output = this->GetOutput();

  // Get the FiniteDifferenceFunction to use in calculations.
  const typename FiniteDifferenceFunctionType::Pointer dff
    = this->GetDifferenceFunction();

  const SizeType  radius = dff->GetRadius();
  
  // Break the input into a series of regions.  The first region is free
  // of boundary conditions, the rest with boundary conditions.  We operate
  // on the output region because input has been copied to output.
  typedef NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<VelocityFieldType>
    FaceCalculatorType;

  typedef typename FaceCalculatorType::FaceListType FaceListType;

  FaceCalculatorType faceCalculator;

  FaceListType faceList = faceCalculator(output, regionToProcess, radius);
  typename FaceListType::iterator fIt = faceList.begin();

  // Ask the function object for a pointer to a data structure it
  // will use to manage any global values it needs.  We'll pass this
  // back to the function object at each calculation and then
  // again so that the function object can use it to determine a
  // time step for this iteration.
  void *globalDataf = dff->GetGlobalDataPointer();

  // Process the non-boundary region.
  NeighborhoodIteratorType nD(radius, output, *fIt);
  if ( m_NumberOfBCHApproximationTerms == 2 )
    {
    UpdateIteratorType       nU(this->GetUpdateBuffer(),  *fIt);
    while( !nD.IsAtEnd() )
      {
       nU.Value() = (dff->ComputeUpdate(nD, globalDataf));

      ++nD;
      ++nU;
      }
    
    // Process each of the boundary faces.
    NeighborhoodIteratorType bD;
    UpdateIteratorType   bU;
    for (++fIt; fIt != faceList.end(); ++fIt)
      {
      bD = NeighborhoodIteratorType(radius, output, *fIt);
      bU = UpdateIteratorType  (this->GetUpdateBuffer(), *fIt);
      while ( !bD.IsAtEnd() )
        {
        bU.Value() = (dff->ComputeUpdate(bD, globalDataf));

        ++bD;
        ++bU;
        }
      }
    }
  else
    {
    UpdateIteratorType       nUF(this->GetUpdateBuffer(),  *fIt);
        while( !nD.IsAtEnd() )
      {
      nUF.Value() = dff->ComputeUpdate(nD, globalDataf);
      ++nD;
      ++nUF;
      }
    
    // Process each of the boundary faces.
    NeighborhoodIteratorType bD;
    UpdateIteratorType   bUF;
    UpdateIteratorType   bUB;
    for (++fIt; fIt != faceList.end(); ++fIt)
      {
      bD = NeighborhoodIteratorType(radius, output, *fIt);
      bUF = UpdateIteratorType  (this->GetUpdateBuffer(), *fIt);
      while ( !bD.IsAtEnd() )
        {
        bUF.Value() = dff->ComputeUpdate(bD, globalDataf);
        ++bD;
        ++bUF;
        ++bUB;
        }
      }
    }

  // Ask the finite difference function to compute the time step for
  // this iteration.  We give it the global data pointer to use, then
  // ask it to free the global data memory.
     TimeStepType timeStep = ( dff->ComputeGlobalTimeStep(globalDataf) );

  dff->ReleaseGlobalDataPointer(globalDataf);
  
  return timeStep;
}


// Get the metric value from the difference function
template <class TFixedImage, class TMovingImage, class TField>
void
SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
#if (ITK_VERSION_MAJOR < 4)
::ApplyUpdate(TimeStepType dt)
#else
::ApplyUpdate(const TimeStepType& dt)
#endif
{
  const DemonsRegistrationFunctionType *drfpf = this->GetForwardRegistrationFunctionType();

  this->SetRMSChange( drfpf->GetRMSChange() );


  if ( this->m_NumberOfBCHApproximationTerms < 3 )
    {
    // If we smooth the update buffer before applying it, then the are
    // approximating a viscuous problem as opposed to an elastic problem
    if ( this->GetSmoothUpdateField() )
      {
      this->SmoothUpdateField();
      }

    // Use time step if necessary. In many cases
    // the time step is one so this will be skipped
    if ( fabs(dt - 1.0)>1.0e-4 )
      {
      itkDebugMacro( "Using timestep: " << dt );
      m_Multiplier->SetConstant( dt );
      m_Multiplier->SetInput( this->GetUpdateBuffer() );
      m_Multiplier->GraftOutput( this->GetUpdateBuffer() );
      // in place update
      m_Multiplier->Update();
      // graft output back to this->GetUpdateBuffer()
      this->GetUpdateBuffer()->Graft( m_Multiplier->GetOutput() );
      }
	typedef typename itk::ImageFileWriter<VelocityFieldType> WT;
	typename WT::Pointer W=WT::New();


    // Apply update
    m_Adder->SetInput( 0, this->GetOutput() );
    m_Adder->SetInput( 1, this->GetUpdateBuffer() );
    m_Adder->GraftOutput( this->GetOutput() );
    m_Adder->GetOutput()->SetRequestedRegion( this->GetOutput()->GetRequestedRegion() );
   
 
    // Triggers in place update
    m_Adder->Update();
    
    // Region passing stuff
    this->GraftOutput( m_Adder->GetOutput() );
    }
  else
    {
    // If we smooth the update buffer before applying it, then the are
    // approximating a viscuous problem as opposed to an elastic problem
    if ( this->GetSmoothUpdateField() )
      {
      this->SmoothUpdateField();

      }

    // Use time step if necessary. In many cases
    // the time step is one so this will be skipped
    if ( fabs(dt - 1.0)>1.0e-4 )
      {
      itkDebugMacro( "Using timestep: " << dt );
      m_Multiplier->SetConstant( dt );
      
      m_Multiplier->SetInput( this->GetUpdateBuffer() );
      m_Multiplier->GraftOutput( this->GetUpdateBuffer() );
      // in place update
      m_Multiplier->Update();
      this->GetUpdateBuffer()->Graft( m_Multiplier->GetOutput() );

      m_Multiplier->SetInput( this->GetBackwardUpdateBuffer() );
      m_Multiplier->GraftOutput( this->GetBackwardUpdateBuffer() );
      // in place update
      m_Multiplier->Update();
      this->GetBackwardUpdateBuffer()->Graft( m_Multiplier->GetOutput() );
      }

    // Apply update (declare the filters here as efficiency is not critical
    // with "high" order BCH approximations)
    typedef VelocityFieldBCHCompositionFilter<
       VelocityFieldType, VelocityFieldType>   BCHFilterType;
    
    typename BCHFilterType::Pointer bchfilter = BCHFilterType::New();
    bchfilter->SetNumberOfApproximationTerms( this->m_NumberOfBCHApproximationTerms );

    // First get Z( v, K_fluid * u_forward )
    bchfilter->SetInput( 0, this->GetOutput() );
    bchfilter->SetInput( 1, this->GetUpdateBuffer() );

    bchfilter->GetOutput()->SetRequestedRegion( this->GetOutput()->GetRequestedRegion() );
    bchfilter->Update();
    VelocityFieldPointer Zf = bchfilter->GetOutput();
    Zf->DisconnectPipeline();

    // Now get Z( -v, K_fluid * u_backward )
    typedef OppositeImageFilter<
       VelocityFieldType, VelocityFieldType>  OppositeFilterType;

    typename OppositeFilterType::Pointer oppositefilter = OppositeFilterType::New();
    oppositefilter->SetInput( this->GetOutput() );
    oppositefilter->InPlaceOn();

    bchfilter->SetInput( 0, oppositefilter->GetOutput() );
    bchfilter->SetInput( 1, this->GetBackwardUpdateBuffer() );

    bchfilter->GetOutput()->SetRequestedRegion( this->GetOutput()->GetRequestedRegion() );
    bchfilter->Update();
    VelocityFieldPointer Zb = bchfilter->GetOutput();
    Zb->DisconnectPipeline();

    // Finally get 0.5*( Z( v, K_fluid * u_forward ) - Z( -v, K_fluid * u_backward ) )
    typedef SubtractImageFilter<
       VelocityFieldType, VelocityFieldType, VelocityFieldType>  SubtracterType;

    typename SubtracterType::Pointer subtracter = SubtracterType::New();
    subtracter->SetInput( 0, Zf );
    subtracter->SetInput( 1, Zb );

    subtracter->GraftOutput( this->GetOutput() );
    
    m_Multiplier->SetConstant( 0.5 );
    m_Multiplier->SetInput( subtracter->GetOutput() );
    m_Multiplier->GraftOutput( this->GetOutput() );
    
    // Triggers in place update
    m_Multiplier->GetOutput()->SetRequestedRegion( this->GetOutput()->GetRequestedRegion() );
    m_Multiplier->Update();
    
    // Region passing stuff
    this->GraftOutput( m_Multiplier->GetOutput() );
    }


  // Smooth the velocity field
  if (this->GetRegularizationType()==0)
    {
     if( this->GetSmoothVelocityField() )
      {
       this->SmoothVelocityField();
      }
    }
  else if (this->GetRegularizationType()==1)
   {
    this->SmoothVelocityField();
   }

}
template <class TFixedImage, class TMovingImage, class TField>
void
SymmetricLCClogDemonsRegistrationFilter<TFixedImage,TMovingImage,TField>
::PrintSelf(std::ostream& os, Indent indent) const
{ 
  Superclass::PrintSelf( os, indent );

  os << indent << "Multiplier: " << m_Multiplier << std::endl;
  os << indent << "Adder: " << m_Adder << std::endl;
  os << indent << "NumberOfBCHApproximationTerms: " << m_NumberOfBCHApproximationTerms << std::endl;
}


} // end namespace itk

#endif
