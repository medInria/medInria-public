#ifndef __itkLocalCriteriaOptimizer_h
#define __itkLocalCriteriaOptimizer_h

#include "itkPDEDeformableRegistrationFunction.h"
#include "itkWarpImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkImageIterator.h"
#include "itkImage.h"
#include <mutex>

namespace itk
{
/**
 * \class LocalCriteriaOptimizer
 * \brief Evaluates the current update for the optimization of the symmetric LCC
 * similarity criterion.
 *
 * See M. Lorenzi, N. Ayache, G.B. Frisoni and X. Pennec,
 * "LCC-Demons: A robust and accurate symmetric diffeomorphic registration algorithm",
 * Neuroimage. 2013 Nov 1;81:470-83.
 *
 * Velocity and deformation fields are represented as images whose pixel type are
 * some vector type with at least N elements, where N is the dimension of
 * the fixed image. The vector type must support element access via operator
 * []. It is assumed that the vector elements behave like floating point
 * scalars.
 *
 * This class is templated over the fixed image type, moving image type
 * and the velocity/deformation field type.
 *
 * The input fixed and moving images are set via methods SetFixedImage
 * and SetMovingImage respectively.
 *
 * \warning This filter assumes that the fixed image type, moving image type
 * and velocity field type all have the same number of dimensions.
 *
 * \author Marco Lorenzi, INRIA
 *
 * \sa LocalCriteriaOptimizer.
 * \ingroup PDEDeformableRegistrationFunction
 */


template <class TFixedImage, class TMovingImage, class TDeformationField>
class ITK_EXPORT LocalCriteriaOptimizer : 
	public PDEDeformableRegistrationFunction<TFixedImage, TMovingImage, TDeformationField>
	{
	public: 
		typedef LocalCriteriaOptimizer					Self;
		typedef PDEDeformableRegistrationFunction<TFixedImage,TMovingImage, TDeformationField>	
										SuperClass;
		typedef itk::SmartPointer<Self>					Pointer;
		typedef itk::SmartPointer<const Self>				ConstPointer;

		itkNewMacro(Self);

		itkTypeMacro(LocalCriteriaOptimizer,PDEDeformableregistrationFunction);


		typedef TFixedImage				FixedImageType;
		typedef typename TFixedImage::PixelType		FixedImagePixelType; 
	  	typedef typename TFixedImage::ConstPointer	FixedImageConstPointer;
		typedef typename TFixedImage::Pointer		FixedImagePointer;
		typedef typename FixedImageType::SpacingType    SpacingType;
		typedef typename FixedImageType::IndexType      IndexType;
		typedef typename FixedImageType::DirectionType  DirectionType;
			
		typedef TMovingImage				MovingImageType;
		typedef typename TMovingImage::PixelType	MovingImagePixelType; 
	  	typedef typename TMovingImage::ConstPointer	MovingImageConstPointer;
		typedef typename TMovingImage::Pointer		MovingImagePointer;


		itkStaticConstMacro(FixedImageDimension, unsigned int, TFixedImage::ImageDimension);
		typedef typename TDeformationField::PixelType		VectorType;
		typedef typename TDeformationField::Pointer  		VectorImagePointer;
		typedef 	 TDeformationField                      VectorImageType;

		typedef  typename itk::CovariantVector<FixedImagePixelType,FixedImageDimension > CovVectorType;
		typedef  typename itk::Image<CovVectorType,FixedImageDimension> CovVectorImageType;
		typedef  typename CovVectorImageType::PixelType    CovPixelType;
		typedef typename  CovPixelType::ValueType      ScalarType;
		typedef typename  CovVectorImageType::PixelContainerPointer CovPixelContainerPointer;

		itkStaticConstMacro(MatrixDimension,unsigned int,FixedImageDimension*FixedImageDimension);
		typedef itk::Vector<FixedImagePixelType,MatrixDimension> MatrixType;
		typedef itk::Image<MatrixType,FixedImageDimension> MatrixImageType;
		typedef typename MatrixImageType::Pointer          MatrixImagePointer;
		
		typedef double   				                                 CoordRepType;
		typedef InterpolateImageFunction< MovingImageType,CoordRepType>                   InterpolatorType;
	 	typedef typename InterpolatorType::Pointer       				  InterpolatorPointer;
		typedef typename InterpolatorType::PointType      				  PointType;
                typedef LinearInterpolateImageFunction<MovingImageType,CoordRepType>              DefaultInterpolatorType;

		typedef typename SuperClass::FloatOffsetType      FloatOffsetType;
		typedef typename SuperClass::NeighborhoodType     NeighborhoodType;	
		typedef double         TimeStepType;


        typedef WarpImageFilter<MovingImageType,MovingImageType,VectorImageType> WarperType;


		void SetSigma(const double Sigma[FixedImageDimension] ){for (int i=0;i<FixedImageDimension;++i) m_Sigma[i]=Sigma[i];};


		void SetMaskImage(FixedImageConstPointer Mask )
			{
			m_MaskImage=FixedImageType::New();
			m_MaskImage=Mask; };

        void UseMask (bool flag)
            {
             m_UseMask=flag;
             }

		FixedImagePointer GetMaskImage( )
			{
			return(m_MaskImage);
			};


		virtual VectorType ComputeUpdate(  const NeighborhoodType &neighborhood,
				    			    void *globalData,
		                     		            const FloatOffsetType &offset = FloatOffsetType(0.0));

		void SetMovingImageInterpolator( InterpolatorType * ptr )
	    		{ m_MovingImageInterpolator = ptr; m_MovingImageWarper->SetInterpolator( ptr ); }	

		InterpolatorType * GetMovingImageInterpolator(void)
	    				{ return m_MovingImageInterpolator; }

		void InitializeIteration(void);

		virtual TimeStepType ComputeGlobalTimeStep(void * itkNotUsed(GlobalData)) const
	    		{ return m_TimeStep; }

		virtual void *GetGlobalDataPointer() const
			    {
			        GlobalDataStruct *global = new GlobalDataStruct();
                    global->m_LCC  = 0.0;
			        global->m_NumberOfPixelsProcessed = 0L;
			        global->m_SumOfSquaredChange      = 0;
			        return global;
			    }

		virtual void ReleaseGlobalDataPointer( void *GlobalData ) const;

		virtual double GetMetric() const
			    { return m_Metric; }
		
		virtual const double &GetRMSChange() const
			    { return m_RMSChange; }
		

		void SetSimilarityImage(FixedImagePointer Sim)
            {
              m_SimilarityImage=FixedImageType::New();
              m_SimilarityImage = Sim;
			};		
		
		FixedImagePointer GetSimilarityImage()
            {
              return	 m_SimilarityImage;
			};		


		void SetSigmaI(float value)
            {
              m_SigmaI=value;
			};


        void                                  SetBoundaryCheck(bool);


        void SetInverseDeformationField(VectorImagePointer InvField )
            {
              m_InverseField=VectorImageType::New();
              m_InverseField=InvField;
            };

        virtual VectorImagePointer GetInverseDeformationField( )
            {
              return(m_InverseField);
            };

	protected:
		  LocalCriteriaOptimizer();
        virtual ~LocalCriteriaOptimizer(){}
		  void PrintSelf(std::ostream& os, Indent indent) const;

		  

		  typedef itk::SubtractImageFilter<CovVectorImageType,CovVectorImageType,CovVectorImageType> SubtracterType;


		  typedef itk::AddImageFilter<FixedImageType,FixedImageType,FixedImageType> ScalarAdderType;
		  typedef itk::SubtractImageFilter<FixedImageType,FixedImageType,FixedImageType> ScalarSubtracterType;

		 struct GlobalDataStruct
		     {
               double          m_LCC;
		       unsigned long   m_NumberOfPixelsProcessed;      
		       double          m_SumOfSquaredChange;
		     };

		void EvaluateHighOrderTerms(void);	
		
        void SetSimGrad(VectorImagePointer Grad){m_SmoothedSimGrad=VectorImageType::New();m_SmoothedSimGrad = Grad;};


	private:
	
		  LocalCriteriaOptimizer(const Self&);
		  void operator=(const Self&);

        typename WarperType::Pointer 		m_MovingImageWarper;
		typename WarperType::Pointer 		m_FixedImageWarper;
		typename WarperType::Pointer 		m_MaskImageWarper;


		double 					m_Sigma[3];
		float 					m_SigmaI;


        typename FixedImageType::Pointer		m_SimilarityImage;
        FixedImageConstPointer			        m_MaskImage;

        typename VectorImageType::Pointer	    m_SmoothedSimGrad;


		PointType                       m_FixedImageOrigin;
		SpacingType                     m_FixedImageSpacing;
		DirectionType                   m_FixedImageDirection;

		PointType                       m_MovingImageOrigin;
		SpacingType                     m_MovingImageSpacing;
		DirectionType                   m_MovingImageDirection;

        InterpolatorPointer		        m_MovingImageInterpolator;

		mutable double                  m_Metric;
        mutable double                  m_LCC;
		mutable unsigned long           m_NumberOfPixelsProcessed;
		mutable double                  m_RMSChange;
		mutable double                  m_SumOfSquaredChange;
		
        VectorImagePointer 		m_InverseField;

		TimeStepType                    m_TimeStep;	
        mutable std::mutex m_MetricCalculationLock;

        bool                            m_UseMask;

        bool                            m_BoundaryCheck;
       	};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLocalCriteriaOptimizer.txx"
#endif

#endif

