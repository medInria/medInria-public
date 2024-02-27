#ifndef _RPI_LCC_LOG_DEMONS_CXX_
#define _RPI_LCC_LOG_DEMONS_CXX_

//sasdsadsad

#include <itkHistogramMatchingImageFilter.h>
#include "itkLogDomainDemonsRegistrationFilter.h"
#include "itkLCCDeformableRegistrationFilter.h"
#include "itkSymmetricLogDomainDemonsRegistrationFilter.h"
#include "itkSymmetricLCClogDemonsRegistrationFilter.h"
#include "itkMultiResolutionLogDomainDeformableRegistration.h"
#include "itkMultiResolutionLCCDeformableRegistration.h"
#include "rpiLCClogDemons.hxx"
#include "itkDemonsCommandIterationUpdate.h"
#include "itkStatisticsImageFilter.h"
#include "itkDivideByConstantImageFilter.h"
#include "itkImageFileWriter.h"

// Namespace RPI : Registration Programming Interface
namespace rpi
{



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::LCClogDemons(void)
{

    // Initialize parameters (except iterations)
    this->m_updateRule                          = UPDATE_SYMMETRIC_LOG_DOMAIN;
    this->m_gradientType                        = GRADIENT_SYMMETRIZED;
    this->m_maximumUpdateStepLength             = 2.0;
    this->m_updateFieldStandardDeviation        = 0.0;
    this->m_velocityFieldStandardDeviation      = 1.5;
    this->m_SimilarityCriteriaStandardDeviation =3;
    this->m_SigmaI 				                =0.2;
    this->m_BCHExpansion                        = 2;

    this->m_RegularizationType                  =0;
    this->m_HarmonicWeight                      =1e-3;
    this->m_BendingWeight                       =1e-6;

    // Initialize iterations
    this->m_iterations.resize(3);
    this->m_iterations[0] = 15;
    this->m_iterations[1] = 10;
    this->m_iterations[2] =  5;

    // Initialize the transformation
    this->m_transform                  = TransformType::New();
    this->m_displacementFieldTransform = DisplacementFieldTransformType::New();

    this->m_BoundaryCheck    = true;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::~LCClogDemons(void)
{
    // Do nothing
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
std::vector<unsigned int>
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetNumberOfIterations(void) const
{
    return this->m_iterations;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetNumberOfIterations(std::vector<unsigned int> iterations)
{
    this->m_iterations = iterations;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
typename LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::UpdateRule
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetUpdateRule(void) const
{
    return this->m_updateRule;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetUpdateRule(UpdateRule value)
{
    this->m_updateRule = value;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
typename LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GradientType
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetGradientType(void) const
{
    return this->m_gradientType;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetGradientType(GradientType value)
{
    this->m_gradientType = value;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
double
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetMaximumUpdateStepLength(void) const
{
    return this->m_maximumUpdateStepLength;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetMaximumUpdateStepLength(double value)
{
    if ( value>=0 )
        this->m_maximumUpdateStepLength = value;
    else
        throw std::runtime_error( "Maximum step length must be greater than or equal to 0." );
}


template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetRegularizationType(unsigned int regularizationType)
{
    this->m_RegularizationType=regularizationType;
}

template < class TFixedImage, class TMovingImage, class TTransformScalarType >
unsigned int
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetRegularizationType(void)
{
    return(this->m_RegularizationType);
}


template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetHarmonicWeight(double harmonicWeight)
{
    this->m_HarmonicWeight=harmonicWeight*1e-3;
}

template < class TFixedImage, class TMovingImage, class TTransformScalarType >
double
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetHarmonicWeight(void)
{
    return(this->m_HarmonicWeight);
}


template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetBendingWeight(double bendingWeight)
{
    this->m_BendingWeight=bendingWeight*1e-6;
}

template < class TFixedImage, class TMovingImage, class TTransformScalarType >
double
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetBendingWeight(void)
{
    return(this->m_BendingWeight);
}

template < class TFixedImage, class TMovingImage, class TTransformScalarType >
double
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetUpdateFieldStandardDeviation(void) const
{
    return this->m_updateFieldStandardDeviation;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetUpdateFieldStandardDeviation(double value)
{
    if ( value>=0 )
        this->m_updateFieldStandardDeviation = value;
    else
        throw std::runtime_error( "Standard deviation must be greater than or equal to 0." );
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
double
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetStationaryVelocityFieldStandardDeviation(void) const
{
    return this->m_velocityFieldStandardDeviation;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetStationaryVelocityFieldStandardDeviation(double value)
{
    if ( value>=0 )
        this->m_velocityFieldStandardDeviation = value;
    else
        throw std::runtime_error( "Standard deviation must be greater than or equal to 0." );
}


template < class TFixedImage, class TMovingImage, class TTransformScalarType >
double
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetSimilarityCriteriaStandardDeviation(void) const
{
    return this->m_SimilarityCriteriaStandardDeviation;
}

template < class TFixedImage, class TMovingImage, class TTransformScalarType >
double
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetSigmaI(void) const
{
    return this->m_SigmaI;
}


template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetSimilarityCriteriaStandardDeviation(double value)
{
    if ( value>=0 )
        this->m_SimilarityCriteriaStandardDeviation = value;
    else
        throw std::runtime_error( "Standard deviation must be greater than or equal to 0." );
}

template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetSigmaI(double value)
{
    if ( value>0 )
        this->m_SigmaI = value;
    else
        throw std::runtime_error( "Sigma_i must be greater than 0." );
}


template < class TFixedImage, class TMovingImage, class TTransformScalarType >
bool
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetUseHistogramMatching(void) const
{
    return this->m_useHistogramMatching;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetUseHistogramMatching(bool value)
{
    this->m_useHistogramMatching = value;
}


template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetVerbosity(bool value)
{
    this->m_verbosity = value;
}


template < class TFixedImage, class TMovingImage, class TTransformScalarType >
unsigned int
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetNumberOfTermsBCHExpansion(void) const
{
    return this->m_BCHExpansion;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetNumberOfTermsBCHExpansion(unsigned int value)
{
    this->m_BCHExpansion = value;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
typename LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::TransformPointerType
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetInitialTransformation(void) const
{
    return this->m_initialTransform;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetInitialTransformation(DisplacementFieldTransformType * transform)
{
    this->m_initialTransform = transform;
}


template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetTrueField(DisplacementFieldTransformType * transform)
{
    this->m_TrueField = transform;
}

template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::UseMask(bool flag)
{
   m_UseMask=flag;

}

template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetMaskImage(MovingImageType * MaskImage)
{
   m_MaskImage= MovingImageType::New(); 
   this->m_MaskImage = MaskImage;
}

template < class TFixedImage, class TMovingImage, class TTransformScalarType >
typename LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::MovingImagePointerType
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetMaskImage(void) const
{
    return this->m_MaskImage;
}

template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::SetBoundaryCheck(bool flag)
{
   this->m_BoundaryCheck=flag;
}

template < class TFixedImage, class TMovingImage, class TTransformScalarType >
bool
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetBoundaryCheck(void)
{
   return(m_BoundaryCheck);

}


template < class TFixedImage, class TMovingImage, class TTransformScalarType >
typename LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::DisplacementFieldTransformPointerType
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::GetDisplacementFieldTransformation(void) const
{
    return this->m_displacementFieldTransform;
}



template < class TFixedImage, class TMovingImage, class TTransformScalarType >
void
LCClogDemons< TFixedImage, TMovingImage, TTransformScalarType >::StartRegistration(void)
{


    // Check if fixed image has been set
    if (this->m_fixedImage.IsNull())
        throw std::runtime_error( "Fixed image has not been set." );


    // Check if moving image has been set
    if (this->m_movingImage.IsNull())
        throw std::runtime_error( "Moving image has not been set." );


    // Type definition
    typedef  typename  TFixedImage::PixelType                                 PixelType;
    typedef  typename  TransformType::VectorFieldType                       FieldContainerType;


    // Local images
    typename  TFixedImage::ConstPointer   fixedImage  = this->m_fixedImage;
    typename  TMovingImage::ConstPointer  movingImage = this->m_movingImage;
//  typename  TFixedImage::ConstPointer   fixedImage  = DividerFixed->GetOutput();
//  typename  TMovingImage::ConstPointer  movingImage = DividerMoving->GetOutput();
   
    // Match the histogram between the fixed and moving images
    if ( this->m_useHistogramMatching )
    {
        // Create and set the historgam matcher
        typedef itk::HistogramMatchingImageFilter< TMovingImage, TMovingImage> MatchingFilterType;
        typename MatchingFilterType::Pointer matcher = MatchingFilterType::New();
        matcher->SetInput(                   this->m_movingImage );
        matcher->SetReferenceImage(          this->m_fixedImage );
        matcher->SetNumberOfHistogramLevels( 1024 );
        matcher->SetNumberOfMatchPoints(     7 );
        matcher->ThresholdAtMeanIntensityOn();

        // Update the matcher
        try
        {
            matcher->Update();
        }
        catch( itk::ExceptionObject& err )
        {
            throw std::runtime_error( "Could not match the histogram of input images." );
        }

        // Get the moving image
        movingImage = matcher->GetOutput();
    }



  if ( this->m_updateRule == UPDATE_SYMMETRIC_LOCAL_LOG_DOMAIN)
   	{
		
        typedef  typename  itk::MultiResolutionLCCDeformableRegistration< TFixedImage, TMovingImage, FieldContainerType, PixelType >
            MultiResLocalRegistrationFilterType;
        typedef  typename  itk::LCCDeformableRegistrationFilter< TFixedImage, TMovingImage, FieldContainerType>
            BaseRegistrationFilterType;

		typename MultiResLocalRegistrationFilterType::Pointer multires = MultiResLocalRegistrationFilterType::New();
				

                // Create the "actual" registration filter, and set it to the existing filter
                typedef  typename  itk::SymmetricLCClogDemonsRegistrationFilter< TFixedImage, TMovingImage, FieldContainerType>
                        ActualRegistrationFilterType;
		typename ActualRegistrationFilterType::Pointer actualfilter = ActualRegistrationFilterType::New();

        multires->SetSimilarityCriteriaStandardDeviationsWorldUnit( this->m_SimilarityCriteriaStandardDeviation);
		multires->SetSigmaI( this->m_SigmaI);
        multires->SetBoundaryCheck(this->m_BoundaryCheck);

        if (m_verbosity)
		{	
            typename DemonsCommandIterationUpdate<BaseRegistrationFilterType,MultiResLocalRegistrationFilterType,TTransformScalarType, TFixedImage::ImageDimension>::Pointer observer =
                DemonsCommandIterationUpdate<BaseRegistrationFilterType,MultiResLocalRegistrationFilterType,TTransformScalarType, TFixedImage::ImageDimension>::New();

			    if ( m_TrueField )
			      {
				      if (m_iterations.size() > 1)
				        {		
				        std::cout << "You cannot compare the results with a true field in a multiresolution setting yet." << std::endl;
				        exit( EXIT_FAILURE );
        				}

                       observer->SetTrueField((m_TrueField->GetParametersAsVectorField()));
      
                              }


			actualfilter->AddObserver( itk::IterationEvent(), observer );
			
            typename DemonsCommandIterationUpdate<BaseRegistrationFilterType,MultiResLocalRegistrationFilterType,TTransformScalarType, TFixedImage::ImageDimension>::Pointer multiresobserver =
                DemonsCommandIterationUpdate<BaseRegistrationFilterType,MultiResLocalRegistrationFilterType,TTransformScalarType, TFixedImage::ImageDimension>::New();
		        multires->AddObserver( itk::IterationEvent(), multiresobserver );
		}

		multires->SetFixedImage(         fixedImage );
		multires->SetMovingImage(        movingImage );
		multires->SetRegistrationFilter( actualfilter );
		multires->SetNumberOfLevels(     this->m_iterations.size() );
		multires->SetNumberOfIterations( &m_iterations[0] );

        if (m_UseMask)
          {
           multires->UseMask(m_UseMask);
           multires->SetMaskImage(this->m_MaskImage);
          }
        else  multires->UseMask(m_UseMask);



		// Set the field interpolator
        typedef  itk::NearestNeighborInterpolateImageFunction< FieldContainerType, double >  FieldInterpolatorType;
		typename FieldInterpolatorType::Pointer interpolator = FieldInterpolatorType::New();
		multires->GetFieldExpander()->SetInterpolator( interpolator );

        multires->SetRegularizationType(this->GetRegularizationType());
        if (this->GetRegularizationType()==0)
          {
           // Set the standard deviation of the displacement field smoothing
           if ( this->m_velocityFieldStandardDeviation >= 0.1 )
             {
		        multires->SetStandardDeviationsWorldUnit( this->m_velocityFieldStandardDeviation );
		        multires->SmoothVelocityFieldOn();
             }
           else
		        multires->SmoothVelocityFieldOff();
        // Set the standard deviation of the update field smoothing
           if ( this->m_updateFieldStandardDeviation >= 0.1 )
            {
               multires->SetUpdateFieldStandardDeviationsWorldUnit( this->m_updateFieldStandardDeviation );
               multires->SmoothUpdateFieldOn();
            }
           else
               multires->SmoothUpdateFieldOff();
          }
        if (this->GetRegularizationType()==1)
          {
            multires->SetHarmonicWeight(this->GetHarmonicWeight());
            multires->SetBendingWeight(this->GetBendingWeight());
          }

    
	        // Set the initial displacement field only if it exists
 		 if (this->m_initialTransform.IsNotNull())
    			{
                    typename DisplacementFieldTransformType::Pointer transform = this->m_initialTransform;
                    typename FieldContainerType::ConstPointer field            = transform->GetParametersAsVectorField();
			        multires->SetArbitraryInitialVelocityField( const_cast<FieldContainerType *>(field.GetPointer()) );
    			}	


	      // Start the registration process
		try
		       {
			        multires->UpdateLargestPossibleRegion();
			 }
		catch( itk::ExceptionObject& err )
    			{
       				 std::cout << err << std::endl;
			        throw std::runtime_error( "Unexpected error." );
    			}
		
        std::cout<<"Creating images"<<std::endl;

		// Create the velocity field transform object
		typename TransformType::Pointer stationaryVelocityFieldTransform = TransformType::New();
		stationaryVelocityFieldTransform->SetParametersAsVectorField( static_cast<typename FieldContainerType::ConstPointer>( multires->GetVelocityField() ) );
		this->m_transform = stationaryVelocityFieldTransform;


		// Create the velocity field transform object
		typename DisplacementFieldTransformType::Pointer displacementFieldTransform = DisplacementFieldTransformType::New();
        displacementFieldTransform->SetParametersAsVectorField( static_cast<typename FieldContainerType::Pointer>( multires->GetDeformationField() ) );
		this->m_displacementFieldTransform = displacementFieldTransform;

		}

  else
	
  {

	typedef  typename  itk::MultiResolutionLogDomainDeformableRegistration< TFixedImage, TMovingImage, FieldContainerType, PixelType >    MultiResRegistrationFilterType;
        typedef  typename  itk::LogDomainDeformableRegistrationFilter< TFixedImage, TMovingImage, FieldContainerType>			      BaseRegistrationFilterType;
		    typename BaseRegistrationFilterType::Pointer filter;
		    typename MultiResRegistrationFilterType::Pointer multires = MultiResRegistrationFilterType::New();

    // Initialize the filter
    switch ( this->m_updateRule )
    {

        case UPDATE_LOG_DOMAIN:
            { 

                // Type definition
                typedef  typename  itk::LogDomainDemonsRegistrationFilter< TFixedImage, TMovingImage, FieldContainerType>
                        ActualRegistrationFilterType;
                typedef  typename  ActualRegistrationFilterType::GradientType  Gradient;
                // Create the "actual" registration filter, and set it to the existing filter
                typename ActualRegistrationFilterType::Pointer actualfilter = ActualRegistrationFilterType::New();
                actualfilter->SetMaximumUpdateStepLength( this->m_maximumUpdateStepLength );
                actualfilter->SetUseGradientType(         static_cast<Gradient>( this->m_gradientType ) );
                filter = actualfilter;

		/*Add observers*/
        typename DemonsCommandIterationUpdate<BaseRegistrationFilterType,MultiResRegistrationFilterType,TTransformScalarType, TFixedImage::ImageDimension>::Pointer observer =
        DemonsCommandIterationUpdate<BaseRegistrationFilterType,MultiResRegistrationFilterType,TTransformScalarType, TFixedImage::ImageDimension>::New();
		filter->AddObserver( itk::IterationEvent(), observer );

        typename DemonsCommandIterationUpdate<BaseRegistrationFilterType,MultiResRegistrationFilterType,TTransformScalarType, TFixedImage::ImageDimension>::Pointer multiresobserver =
            DemonsCommandIterationUpdate<BaseRegistrationFilterType,MultiResRegistrationFilterType,TTransformScalarType, TFixedImage::ImageDimension>::New();
		multires->AddObserver( itk::IterationEvent(), multiresobserver );


            }
            break;

        case UPDATE_SYMMETRIC_LOG_DOMAIN:
            {  


                // Type definition
                typedef  typename  itk::SymmetricLogDomainDemonsRegistrationFilter< TFixedImage, TMovingImage, FieldContainerType>
                        ActualRegistrationFilterType;
                typedef  typename  ActualRegistrationFilterType::GradientType  Gradient;
                // Create the "actual" registration filter, and set it to the existing filter
                typename ActualRegistrationFilterType::Pointer actualfilter = ActualRegistrationFilterType::New();
                actualfilter->SetMaximumUpdateStepLength( this->m_maximumUpdateStepLength );
                actualfilter->SetUseGradientType(         static_cast<Gradient>( this->m_gradientType ) );
                filter = actualfilter;
	
		/*Add observers*/
        typename DemonsCommandIterationUpdate<BaseRegistrationFilterType,MultiResRegistrationFilterType,TTransformScalarType, TFixedImage::ImageDimension>::Pointer observer =
        DemonsCommandIterationUpdate<BaseRegistrationFilterType,MultiResRegistrationFilterType,TTransformScalarType, TFixedImage::ImageDimension>::New();
		filter->AddObserver( itk::IterationEvent(), observer );
	     
            typename DemonsCommandIterationUpdate<BaseRegistrationFilterType,MultiResRegistrationFilterType,TTransformScalarType, TFixedImage::ImageDimension>::Pointer multiresobserver =
            DemonsCommandIterationUpdate<BaseRegistrationFilterType,MultiResRegistrationFilterType,TTransformScalarType, TFixedImage::ImageDimension>::New();
	        multires->AddObserver( itk::IterationEvent(), multiresobserver );

            }

	    break;


        default:
            throw std::runtime_error( "Unsupported update rule." );
    }


    // This line was commented in the code of Tom
    //filter->SetIntensityDifferenceThreshold( 0.001 );

    multires->SetFixedImage(         fixedImage );
    multires->SetMovingImage(        movingImage );
    multires->SetRegistrationFilter( filter );
    multires->SetNumberOfLevels(     this->m_iterations.size() );
    multires->SetNumberOfIterations( &m_iterations[0] );


    // Set the field interpolator
    typedef  itk::NearestNeighborInterpolateImageFunction< FieldContainerType, double >  FieldInterpolatorType;
    typename FieldInterpolatorType::Pointer interpolator = FieldInterpolatorType::New();
    multires->GetFieldExpander()->SetInterpolator( interpolator );


    multires->SetRegularizationType(this->GetRegularizationType());
    if (this->GetRegularizationType()==0)
    {
      // Set the standard deviation of the displacement field smoothing
      if ( this->m_velocityFieldStandardDeviation >= 0.1 )
      {
          multires->SetStandardDeviationsWorldUnit( this->m_velocityFieldStandardDeviation );
          multires->SmoothVelocityFieldOn();
      }
      else
          multires->SmoothVelocityFieldOff();


      // Set the standard deviation of the update field smoothing
      if ( this->m_updateFieldStandardDeviation >= 0.1 )
      {
          multires->SetUpdateFieldStandardDeviationsWorldUnit( this->m_updateFieldStandardDeviation );
          multires->SmoothUpdateFieldOn();
      }
      else
          multires->SmoothUpdateFieldOff();
    }
    if (this->GetRegularizationType()==1)
      {
        multires->SetHarmonicWeight(this->GetHarmonicWeight());
        multires->SetBendingWeight(this->GetBendingWeight());
      }
    
    // Set the initial displacement field only if it exists
    if (this->m_initialTransform.IsNotNull())
    {
        typename DisplacementFieldTransformType::Pointer           transform = this->m_initialTransform;
        typename FieldContainerType::ConstPointer field     = transform->GetParametersAsVectorField();
        multires->SetArbitraryInitialVelocityField( const_cast<FieldContainerType *>(field.GetPointer()) );
    }


    // Start the registration process
    try
    {
        multires->UpdateLargestPossibleRegion();
    }
    catch( itk::ExceptionObject& err )
    {
        std::string message = "Unexpected error: ";
        message += err.GetDescription();
        throw std::runtime_error( message  );
    }


    // Create the velocity field transform object
    typename TransformType::Pointer stationaryVelocityFieldTransform = TransformType::New();
    stationaryVelocityFieldTransform->SetParametersAsVectorField( static_cast<typename FieldContainerType::ConstPointer>( multires->GetVelocityField() ) );
    this->m_transform = stationaryVelocityFieldTransform;


    // Create the velocity field transform object
    typename DisplacementFieldTransformType::Pointer displacementFieldTransform = DisplacementFieldTransformType::New();
    displacementFieldTransform->SetParametersAsVectorField( static_cast<typename FieldContainerType::Pointer>( multires->GetDeformationField() ) );
    this->m_displacementFieldTransform = displacementFieldTransform;
  }
}


} // End of namespace


#endif // _RPI_LCC_LOG_DEMONS_CXX_
