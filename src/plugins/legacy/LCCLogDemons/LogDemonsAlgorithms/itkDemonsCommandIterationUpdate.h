#include <itkCommand.h>
#include <itkDisplacementFieldJacobianDeterminantFilter.h>
#include <itkGridForwardWarpImageFilter.h>
#include <itkHistogramMatchingImageFilter.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkTransformFileReader.h>
#include <itkTransformToVelocityFieldSource.h>
#include <itkCentralDifferenceImageFunction.h>
#include <itkVectorLinearInterpolateNearestNeighborExtrapolateImageFunction.h>
#include <itkWarpHarmonicEnergyCalculator.h>
#include <itkWarpImageFilter.h>

#include <metaCommand.h>

#include <errno.h>
#include <iostream>
#include <limits.h>


template <class DeformableRegistrationType, class MultiResolutionRegistrationFilterType,class TPixel=float, unsigned int VImageDimension=3>
class DemonsCommandIterationUpdate : public itk::Command
{
public:
  typedef  DemonsCommandIterationUpdate                         Self;
  typedef  itk::Command                                   Superclass;
  typedef  itk::SmartPointer<Self>                        Pointer;

  typedef itk::Image< TPixel, VImageDimension >           InternalImageType;
  typedef itk::Vector< TPixel, VImageDimension >          VectorPixelType;
  typedef itk::Image<  VectorPixelType, VImageDimension > VelocityFieldType;
  typedef itk::Image<  VectorPixelType, VImageDimension > DeformationFieldType;

/*  typedef itk::LogDomainDeformableRegistrationFilter<
   InternalImageType,
   InternalImageType,
   VelocityFieldType>                                     LogDomainDeformableRegistrationFilterType;

  typedef itk::MultiResolutionLogDomainDeformableRegistration<
   InternalImageType, InternalImageType,
   VelocityFieldType, TPixel >                            MultiResRegistrationFilterType;*/


  typedef DeformableRegistrationType LogDomainDeformableRegistrationFilterType;
  
  typedef  MultiResolutionRegistrationFilterType MultiResRegistrationFilterType; 

  typedef itk::DisplacementFieldJacobianDeterminantFilter<
   DeformationFieldType, TPixel> JacobianFilterType;
   
  typedef itk::MinimumMaximumImageCalculator<
     InternalImageType>                                   MinMaxFilterType;

  typedef itk::WarpHarmonicEnergyCalculator<
     DeformationFieldType>                                HarmonicEnergyCalculatorType;

  typedef itk::CentralDifferenceImageFunction<
     DeformationFieldType,float,itk::Matrix<double, DeformationFieldType::PixelType::Dimension, DeformationFieldType::ImageDimension> > WarpGradientCalculatorType;

  typedef typename WarpGradientCalculatorType::OutputType WarpGradientType;
   
  itkNewMacro( Self );


  void SetTrueField( const DeformationFieldType * truefield)
    {
    m_TrueField = truefield;

    m_TrueWarpGradientCalculator = WarpGradientCalculatorType::New();
    m_TrueWarpGradientCalculator->SetInputImage( m_TrueField );
    
    m_CompWarpGradientCalculator =  WarpGradientCalculatorType::New();
    }
   
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    { 
      if( !(itk::IterationEvent().CheckEvent( &event )) )
      {
      return;
      }

    typename DeformationFieldType::ConstPointer deffield = nullptr;
    unsigned int iter = -1;
    double metricbefore = -1.0;
    
    if ( const LogDomainDeformableRegistrationFilterType * filter = 
         dynamic_cast< const LogDomainDeformableRegistrationFilterType * >( object ) )
      {
      iter = filter->GetElapsedIterations() - 1;
      metricbefore = filter->GetMetric();
      deffield = const_cast<LogDomainDeformableRegistrationFilterType *>
        (filter)->GetDeformationField();
      }
    else if ( const MultiResRegistrationFilterType * multiresfilter = 
              dynamic_cast< const MultiResRegistrationFilterType * >( object ) )
      {
      std::cout<<"Finished Multi-resolution iteration :"<<multiresfilter->GetCurrentLevel()-1<<std::endl;
      std::cout<<"=============================="<<std::endl<<std::endl;
      }
    else
      {
      return;
      }

    if (deffield)
      {
      std::cout<<iter<<": Metric "<<metricbefore<<" - ";
  
      double fieldDist = -1.0;
      double fieldGradDist = -1.0;
      double tmp;
      if (m_TrueField)
        {
        typedef itk::ImageRegionConstIteratorWithIndex<DeformationFieldType>
           FieldIteratorType;
        FieldIteratorType currIter(
           deffield, deffield->GetLargestPossibleRegion() );
        FieldIteratorType trueIter(
           m_TrueField, deffield->GetLargestPossibleRegion() );
        
        m_CompWarpGradientCalculator->SetInputImage( deffield );
        
        fieldDist = 0.0;
        fieldGradDist = 0.0;
        for ( currIter.GoToBegin(), trueIter.GoToBegin();
              ! currIter.IsAtEnd(); ++currIter, ++trueIter )
          {
          fieldDist += (currIter.Value() - trueIter.Value()).GetSquaredNorm();
  
          // No need to add Id matrix here as we do a substraction
          tmp = (
             ( m_CompWarpGradientCalculator->EvaluateAtIndex(currIter.GetIndex())
               -m_TrueWarpGradientCalculator->EvaluateAtIndex(trueIter.GetIndex())
                ).GetVnlMatrix() ).frobenius_norm();
          fieldGradDist += tmp*tmp;
          }
        fieldDist = sqrt( fieldDist/ (double)(
                             deffield->GetLargestPossibleRegion().GetNumberOfPixels()) );
        fieldGradDist = sqrt( fieldGradDist/ (double)(
                                 deffield->GetLargestPossibleRegion().GetNumberOfPixels()) );
        
        std::cout<<"d(.,true) "<<fieldDist<<" - ";
        std::cout<<"d(.,Jac(true)) "<<fieldGradDist<<" - ";
        }
          
      m_HarmonicEnergyCalculator->SetImage( deffield );
      m_HarmonicEnergyCalculator->Compute();
      const double harmonicEnergy
         = m_HarmonicEnergyCalculator->GetHarmonicEnergy();
      std::cout<<"harmo. "<<harmonicEnergy<<" - ";
      
      
      m_JacobianFilter->SetInput( deffield );
      m_JacobianFilter->UpdateLargestPossibleRegion();
      
      
      const unsigned int numPix = m_JacobianFilter->
         GetOutput()->GetLargestPossibleRegion().GetNumberOfPixels();
      
      TPixel* pix_start = m_JacobianFilter->GetOutput()->GetBufferPointer();
      TPixel* pix_end = pix_start + numPix;
      
      TPixel* jac_ptr;
      
      // Get percentage of det(Jac) below 0
      unsigned int jacBelowZero(0u);
      for (jac_ptr=pix_start; jac_ptr!=pix_end; ++jac_ptr)
        {
        if ( *jac_ptr<=0.0 ) ++jacBelowZero;
        }
      const double jacBelowZeroPrc = static_cast<double>(jacBelowZero)
         / static_cast<double>(numPix);
      
      
      // Get min an max jac
      const double minJac = *(std::min_element (pix_start, pix_end));
      const double maxJac = *(std::max_element (pix_start, pix_end));
      
      // Get some quantiles
      // We don't need the jacobian image
      // we can modify/sort it in place
      jac_ptr = pix_start + static_cast<unsigned int>(0.002*numPix);
      std::nth_element(pix_start, jac_ptr, pix_end);
      const double Q002 = *jac_ptr;
  
      jac_ptr = pix_start + static_cast<unsigned int>(0.01*numPix);
      std::nth_element(pix_start, jac_ptr, pix_end);
      const double Q01 = *jac_ptr;
      
      jac_ptr = pix_start + static_cast<unsigned int>(0.99*numPix);
      std::nth_element(pix_start, jac_ptr, pix_end);
      const double Q99 = *jac_ptr;
      
      jac_ptr = pix_start + static_cast<unsigned int>(0.998*numPix);
      std::nth_element(pix_start, jac_ptr, pix_end);
      const double Q998 = *jac_ptr;
      
      
      std::cout<<"max|Jac| "<<maxJac<<" - "
               <<"min|Jac| "<<minJac<<" - "
               <<"ratio(|Jac|<=0) "<<jacBelowZeroPrc<<std::endl;
      
      
      if (this->m_Fid.is_open())
        {
        if (! m_headerwritten)
          {
          this->m_Fid<<"Iteration"
                     <<", MSE before"
                     <<", Harmonic energy"
                     <<", min|Jac|"
                     <<", 0.2% |Jac|"
                     <<", 01% |Jac|"
                     <<", 99% |Jac|"
                     <<", 99.8% |Jac|"
                     <<", max|Jac|"
                     <<", ratio(|Jac|<=0)";
          
          if (m_TrueField)
            {
            this->m_Fid<<", dist(warp,true warp)"
                       <<", dist(Jac,true Jac)";
            }
                
          this->m_Fid<<std::endl;
          
          m_headerwritten = true;
          }
             
        this->m_Fid<<iter
                   <<", "<<metricbefore
                   <<", "<<harmonicEnergy
                   <<", "<<minJac
                   <<", "<<Q002
                   <<", "<<Q01
                   <<", "<<Q99
                   <<", "<<Q998
                   <<", "<<maxJac
                   <<", "<<jacBelowZeroPrc;
        
        if (m_TrueField)
          {
          this->m_Fid<<", "<<fieldDist
                     <<", "<<fieldGradDist;
          }
        
        this->m_Fid<<std::endl;
        }
      }
    }
   
protected:   
  DemonsCommandIterationUpdate() :
    m_Fid( "metricvalues.csv" ),
    m_headerwritten(false)
    {
    m_JacobianFilter = JacobianFilterType::New();
    m_JacobianFilter->SetUseImageSpacing( true );
    m_JacobianFilter->ReleaseDataFlagOn();
    
    m_Minmaxfilter = MinMaxFilterType::New();
    
    m_HarmonicEnergyCalculator = HarmonicEnergyCalculatorType::New();
    
    m_TrueField = 0;
    m_TrueWarpGradientCalculator = 0;
    m_CompWarpGradientCalculator = 0;
    };

  ~DemonsCommandIterationUpdate()
    {
    this->m_Fid.close();
    }

private:
  std::ofstream m_Fid;
  bool m_headerwritten;
  typename JacobianFilterType::Pointer m_JacobianFilter;
  typename MinMaxFilterType::Pointer m_Minmaxfilter;
  typename HarmonicEnergyCalculatorType::Pointer m_HarmonicEnergyCalculator;
  typename DeformationFieldType::ConstPointer m_TrueField;
  typename WarpGradientCalculatorType::Pointer m_TrueWarpGradientCalculator;
  typename WarpGradientCalculatorType::Pointer m_CompWarpGradientCalculator;
};

