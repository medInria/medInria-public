#ifndef __itkSymmetricLCClogDemonsRegistrationFilter_h
#define __itkSymmetricLCClogDemonsRegistrationFilter_h

#include "itkLCCDeformableRegistrationFilter.h"
#include "itkLocalCriteriaOptimizer.h"
#include "itkMultiplyByConstantImageFilter.h"


namespace itk {

/**
 * \class SymmetricLCClogDemonsRegistrationFilter
 * \brief Deformably register two images using a diffeomorphic demons algorithm
 * and a symmetrized optimization scheme.  
 *
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
 * and SetMovingImage respectively. An initial velocity field maybe set via
 * SetInitialVelocityField or SetInput. If no initial field is set,
 * a zero field is used as the initial condition.
 *
 * The output velocity field can be obtained via methods GetOutput
 * or GetVelocityField.
 *
 * The output deformation field can be obtained via method GetDeformationField.
 *
 * This class make use of the finite difference solver hierarchy. Update
 * for each iteration is computed using a PDEDeformableRegistrationFunction.
 *
 * \warning This filter assumes that the fixed image type, moving image type
 * and velocity field type all have the same number of dimensions.
 *
 * \sa DemonsRegistrationFilter
 * \sa DemonsRegistrationFunction
 * \ingroup DeformableImageRegistration MultiThreaded
 * \author Marco Lorenzi, Florence Dru, INRIA and Tom Vercauteren, MKT
 */
template<class TFixedImage, class TMovingImage, class TField>
class ITK_EXPORT SymmetricLCClogDemonsRegistrationFilter :
public LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>
{
public:
  /** Standard class typedefs. */
  typedef SymmetricLCClogDemonsRegistrationFilter    Self;
  typedef LCCDeformableRegistrationFilter<TFixedImage,TMovingImage,TField>   Superclass;
  typedef SmartPointer<Self>                            Pointer;
  typedef SmartPointer<const Self>                      ConstPointer;
 
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro( SymmetricLCClogDemonsRegistrationFilter,LCCDeformableRegistrationFilter );

  itkStaticConstMacro(FixedImageDimension, unsigned int, TFixedImage::ImageDimension);

  
  /** FixedImage image type. */
  typedef typename Superclass::FixedImageType           FixedImageType;
  typedef typename Superclass::FixedImagePointer        FixedImagePointer;

  /** MovingImage image type. */
  typedef typename Superclass::MovingImageType          MovingImageType;
  typedef typename Superclass::MovingImagePointer       MovingImagePointer;
  
  /** Velocity field type. */
  typedef TField                                        VelocityFieldType;
  typedef typename VelocityFieldType::Pointer           VelocityFieldPointer;
  
  /** Deformation field type. */
  typedef typename Superclass::DeformationFieldType     DeformationFieldType;
  typedef typename Superclass::DeformationFieldPointer  DeformationFieldPointer;

  /** Types inherithed from the superclass */
  typedef typename Superclass::OutputImageType          OutputImageType;

  /** FiniteDifferenceFunction type. */
  typedef typename Superclass::FiniteDifferenceFunctionType  FiniteDifferenceFunctionType;

  /** Take timestep type from the FiniteDifferenceFunction. */
  typedef typename 
    FiniteDifferenceFunctionType::TimeStepType          TimeStepType;
 
  /** DemonsRegistrationFilterFunction type. */
  typedef LocalCriteriaOptimizer <FixedImageType,  MovingImageType, DeformationFieldType> DemonsRegistrationFunctionType;
  typedef typename DemonsRegistrationFunctionType::Pointer          DemonsRegistrationFunctionPointer;

  /** Get the metric value. The metric value is the mean square difference 
   * in intensity between the fixed image and transforming moving image 
   * computed over the the overlapping region between the two images. 
   * This value is calculated for the current iteration */
  virtual double GetMetric() const;


  /** Set/Get the number of terms used in the Baker-Campbell-Hausdorff approximation. */
  itkSetMacro( NumberOfBCHApproximationTerms, unsigned int );
  itkGetConstMacro( NumberOfBCHApproximationTerms, unsigned int );




protected:
  SymmetricLCClogDemonsRegistrationFilter();
  ~SymmetricLCClogDemonsRegistrationFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Initialize the state of filter and equation before each iteration. */
  virtual void InitializeIteration();

  /** This method is called before iterating the solution. */
  virtual void Initialize();

  /** This method allocates storage in m_UpdateBuffer.  It is called from
   * FiniteDifferenceFilter::GenerateData(). */
  virtual void AllocateUpdateBuffer();

  /** Method to allow subclasses to get direct access to the backward update
   * buffer */
  virtual VelocityFieldType* GetBackwardUpdateBuffer()
    { return m_BackwardUpdateBuffer; }


  typedef typename VelocityFieldType::RegionType ThreadRegionType;
  /** Does the actual work of calculating change over a region supplied by
   * the multithreading mechanism. */
  virtual TimeStepType ThreadedCalculateChange(
     const ThreadRegionType &regionToProcess, ThreadIdType threadId);

  /** Apply update. */
#if (ITK_VERSION_MAJOR < 4)
  virtual void ApplyUpdate(TimeStepType dt);
#else
  virtual void ApplyUpdate(const TimeStepType& dt);
#endif

  /** This method returns a pointer to a FiniteDifferenceFunction object that
   * will be used by the filter to calculate updates at image pixels.
   * \returns A FiniteDifferenceObject pointer. */
  itkGetConstReferenceObjectMacro(BackwardDifferenceFunction,
                                  FiniteDifferenceFunctionType );

  /** This method sets the pointer to a FiniteDifferenceFunction object that
   * will be used by the filter to calculate updates at image pixels.
   * \returns A FiniteDifferenceObject pointer. */
  itkSetObjectMacro(BackwardDifferenceFunction, FiniteDifferenceFunctionType );

private:
  SymmetricLCClogDemonsRegistrationFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Downcast the DifferenceFunction using a dynamic_cast to ensure that it is of the correct type.
   * this method will throw an exception if the function is not of the expected type. */
  DemonsRegistrationFunctionType *  GetForwardRegistrationFunctionType();
  const DemonsRegistrationFunctionType *  GetForwardRegistrationFunctionType() const;
  DemonsRegistrationFunctionType *  GetBackwardRegistrationFunctionType();
  const DemonsRegistrationFunctionType *  GetBackwardRegistrationFunctionType() const;

  /** Exp and composition typedefs */
  typedef MultiplyByConstantImageFilter<
    VelocityFieldType, 
    TimeStepType, VelocityFieldType >                   MultiplyByConstantType;

  typedef AddImageFilter<
   VelocityFieldType, VelocityFieldType>                AdderType;

  typedef typename MultiplyByConstantType::Pointer      MultiplyByConstantPointer;
  typedef typename AdderType::Pointer                   AdderPointer;

  typename FiniteDifferenceFunctionType::Pointer        m_BackwardDifferenceFunction;

  MultiplyByConstantPointer                             m_Multiplier;
  AdderPointer                                          m_Adder;
  unsigned int                                          m_NumberOfBCHApproximationTerms;
  

  /** The buffer that holds the updates for an iteration of the algorithm. */
  VelocityFieldPointer                                  m_BackwardUpdateBuffer;


};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSymmetricLCClogDemonsRegistrationFilter.txx"
#endif

#endif
