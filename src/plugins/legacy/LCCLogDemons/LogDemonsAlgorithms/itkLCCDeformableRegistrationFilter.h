#ifndef __itkLCCDeformableRegistrationFilter_h
#define __itkLCCDeformableRegistrationFilter_h

#include "itkDenseFiniteDifferenceImageFilter.h"
#include "itkExponentialDeformationFieldImageFilter2.h"
#include "itkPDEDeformableRegistrationFunction.h"

namespace itk {

/**
 * \class LCCDeformableRegistrationFilter
 * \brief Deformably register two images using a PDE-like algorithm
 * where the transformation is represented as the exponential of a velocity field.
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
 * The PDE-like algorithm is run for a user defined number of iterations.
 * Typically the PDE-like algorithm requires period Gaussian smoothing of the
 * velocity field to enforce an elastic-like condition. The amount
 * of smoothing is governed by a set of user defined standard deviations
 * (one for each dimension).
 *
 * In terms of memory, this filter keeps two internal buffers: one for storing
 * the intermediate updates to the field and one for double-buffering when
 * smoothing the velocity field. Both buffers are the same type and size as the
 * output velocity field.
 *
 * This class make use of the finite difference solver hierarchy. Update
 * for each iteration is computed using a PDEDeformableRegistrationFunction.
 *
 * \warning This filter assumes that the fixed image type, moving image type
 * and velocity field type all have the same number of dimensions.
 *
 * \author Marco Lorenzi, Florence Dru, INRIA and Tom Vercauteren, MKT
 * 
 * \sa PDEDeformableRegistrationFunction.
 * \ingroup DeformableImageRegistration
 */
template<class TFixedImage, class TMovingImage, class TField>
class ITK_EXPORT LCCDeformableRegistrationFilter :
    public DenseFiniteDifferenceImageFilter<TField,TField>
{
public:
  /** Standard class typedefs. */
  typedef LCCDeformableRegistrationFilter    Self;
  typedef DenseFiniteDifferenceImageFilter<
    TField,TField>                                 Superclass;
  typedef SmartPointer<Self>                       Pointer;
  typedef SmartPointer<const Self>                 ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro( LCCDeformableRegistrationFilter,
                DenseFiniteDifferenceImageFilter );

  itkStaticConstMacro(FixedImageDimension, unsigned int, TFixedImage::ImageDimension);
  /** FixedImage image type. */
  typedef TFixedImage                            FixedImageType;
  typedef typename FixedImageType::Pointer       FixedImagePointer;
  typedef typename FixedImageType::ConstPointer  FixedImageConstPointer;

  /** MovingImage image type. */
  typedef TMovingImage                           MovingImageType;
  typedef typename MovingImageType::Pointer      MovingImagePointer;
  typedef typename MovingImageType::ConstPointer MovingImageConstPointer;
  
  /** Velocity field type. */
  typedef TField                                  VelocityFieldType;
  typedef typename VelocityFieldType::Pointer     VelocityFieldPointer;
  
  /** Deformation field type. */
  typedef TField                                  DeformationFieldType;
  typedef typename DeformationFieldType::Pointer  DeformationFieldPointer;

  /** Types inherithed from the superclass */
  typedef typename Superclass::OutputImageType    OutputImageType;

  /** FiniteDifferenceFunction type. */
  typedef typename Superclass::FiniteDifferenceFunctionType
     FiniteDifferenceFunctionType;

  /** PDEDeformableRegistrationFunction type. */
  typedef PDEDeformableRegistrationFunction<FixedImageType,MovingImageType,
      DeformationFieldType>                       PDEDeformableRegistrationFunctionType;

  /** Inherit some enums and typedefs from the superclass. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      Superclass::ImageDimension);

  /** Set the fixed image. */
  void SetFixedImage( const FixedImageType * ptr );

  /** Get the fixed image. */
  const FixedImageType * GetFixedImage(void) const;

  /** Set the moving image. */
  void SetMovingImage( const MovingImageType * ptr );

  /** Get the moving image. */
  const MovingImageType * GetMovingImage(void) const;

  /** Set the flag for the mask image. */
  void UseMask( bool);

  /** Set the flag for the mask image. */
  bool GetUseMask();

  /** Set the mask image. */
  void SetMaskImage( const MovingImageType * ptr );

  /** Get the mask image. */
  const MovingImageType * GetMaskImage(void) const;

  /** Set initial velocity field. */
  void SetInitialVelocityField( VelocityFieldType * ptr );

  /** Get output velocity field. */
  VelocityFieldType * GetVelocityField() { return this->GetOutput(); }

  /** Get output deformation field. */
  DeformationFieldPointer GetDeformationField();

  /** Get output inverse deformation field. */
  DeformationFieldPointer GetInverseDeformationField();

  /** Get the number of valid inputs.  For LCCDeformableRegistration,
   * this checks whether the fixed and moving images have been
   * set. While LCCDeformableRegistration can take a third input as an
   * initial velocity field, this input is not a required input. */
  virtual std::vector<SmartPointer<DataObject> >::size_type GetNumberOfValidRequiredInputs() const;

  /**
   * Sets the regularization type.
   * @param  value  maximum length
   */
  void                                   SetRegularizationType(unsigned int regularizationType) ;

  /**
   * Gets the regularization type.
   * @return  regularization type
   */
  unsigned int                           GetRegularizationType(void);

  /** Set/Get whether the velocity field is smoothed 
   * (regularized). Smoothing the velocity yields a solution
   * elastic in nature. If SmoothVelocityField is on, then the
   * velocity field is smoothed with a Gaussian whose standard
   * deviations are specified with SetStandardDeviations() */
  itkSetMacro( SmoothVelocityField, bool );
  itkGetConstMacro( SmoothVelocityField, bool );
  itkBooleanMacro( SmoothVelocityField );
  
  /** Set the Gaussian smoothing standard deviations for the
   * velocity field. The values are set with respect to pixel
   * coordinates. */
  itkSetVectorMacro( StandardDeviations, double, ImageDimension );
  virtual void SetStandardDeviations( double value );

  /**
   * Sets the standard deviation (world unit) used for the global field regularization.
   * @param  value  standard deviation
   */
  virtual void SetStandardDeviationsWorldUnit( double value );


  /**
   * Sets the standard deviation (world unit) used for the global field regularization.
   * @param  value  standard deviation
   */
  virtual void SetStandardDeviationsWorldUnit( double value[] );


  /**
   * Sets the standard deviation (voxel unit) used for the global field regularization.
   * @param  value  standard deviation
   */
  virtual void SetStandardDeviationsVoxelUnit( double value );


  /**
   * Sets the standard deviation (voxel unit) used for the global field regularization.
   * @param  value  standard deviation
   */
  virtual void SetStandardDeviationsVoxelUnit( double value[] );


  /** Get the Gaussian smoothing standard deviations use for smoothing
   * the velocity field. */
  const double * GetStandardDeviations(void) const
  {
      return static_cast<const double*>(this->m_StandardDeviations);
  }


  /** Set/Get whether the update field is smoothed 
   * (regularized). Smoothing the update field yields a solution
   * viscous in nature. If SmoothUpdateField is on, then the
   * update field is smoothed with a Gaussian whose standard
   * deviations are specified with SetUpdateFieldStandardDeviations() */
  itkSetMacro( SmoothUpdateField, bool );
  itkGetConstMacro( SmoothUpdateField, bool );
  itkBooleanMacro( SmoothUpdateField );
  


  /** Set the Gaussian smoothing standard deviations for the update
   * field. The values are set with respect to pixel coordinates. */
  itkSetVectorMacro( UpdateFieldStandardDeviations, double, ImageDimension );
  virtual void SetUpdateFieldStandardDeviations( double value );


  /**
   * Sets the standard deviation (world unit) used for the update field regularization.
   * @param  value  standard deviation
   */
  virtual void SetUpdateFieldStandardDeviationsWorldUnit( double value );


  /**
   * Sets the standard deviation (world unit) used for the update field regularization.
   * @param  value  standard deviation
   */
  virtual void SetUpdateFieldStandardDeviationsWorldUnit( double value[] );


  /**
   * Sets the standard deviation (voxel unit) used for the update field regularization.
   * @param  value  standard deviation
   */
  virtual void SetUpdateFieldStandardDeviationsVoxelUnit( double value );


  /**
   * Sets the standard deviation (voxel unit) used for the update field regularization.
   * @param  value  standard deviation
   */
  virtual void SetUpdateFieldStandardDeviationsVoxelUnit( double value[] );


  /** Get the Gaussian smoothing standard deviations used for
   * smoothing the update field. */
  const double * GetUpdateFieldStandardDeviations(void) const{
      return static_cast<const double*>(this->m_UpdateFieldStandardDeviations);
  }
  


  /**
   * Sets the weight for the penalization of the harmonic energy.
   * @param  harmonic weight
   */
  void                                   SetHarmonicWeight(float harmonicWeight) ;

  /**
   * Gets the weight for the penalization of the harmonic energy.
   * @return  harmonic weight
   */
  float                                  GetHarmonicWeight(void);

  /**
   * Sets the weight for the penalization of the bending energy.
   * @param  harmonic weight
   */
  void                                   SetBendingWeight(float bendingWeight) ;

  /**
   * Gets the weight for the penalization of the bending energy.
   * @return  bending weight
   */
  float                                  GetBendingWeight(void);

/** Set the Gaussian smoothing standard deviations for the similarity 
   * criteria. The values are set with respect to pixel coordinates. */
  itkSetVectorMacro( SimilarityCriteriaStandardDeviations, double, ImageDimension );
  virtual void SetSimilarityCriteriaStandardDeviations( double value );


  /**
   * Sets the standard deviation (world unit) used for the similarity criteria.
   * @param  value  standard deviation
   */
  virtual void SetSimilarityCriteriaStandardDeviationsWorldUnit( double value );


  /**
   * Sets the standard deviation (world unit) used for the similarity criteria.
   * @param  value  standard deviation
   */
  virtual void SetSimilarityCriteriaStandardDeviationsWorldUnit( double value[] );


  /**
   * Sets the standard deviation (voxel unit) used for the similarity criteria.
   * @param  value  standard deviation
   */
  virtual void SetSimilarityCriteriaStandardDeviationsVoxelUnit( double value );


  /**
   * Sets the standard deviation (voxel unit) used for the similarity criteria.
   * @param  value  standard deviation
   */
  virtual void SetSimilarityCriteriaStandardDeviationsVoxelUnit( double value[] );


  /** Get the Gaussian smoothing standard deviations used for
   * smoothing the update field. */
  const double * GetSimilarityCriteriaStandardDeviations(void) const;

  /**
   * Sets the trade-off coefficient
   * @param  value  SigmaI
   */
  virtual void SetSigmaI( double value );

  /** Gets the trde-off coeffcient
   */
  const double GetSigmaI(void) const;


  /** Stop the registration after the current iteration. */
  virtual void StopRegistration()
    { m_StopRegistrationFlag = true; }

  /** Set/Get the desired maximum error of the Gaussian kernel approximate. 
   * \sa GaussianOperator. */
  itkSetMacro( MaximumError, double );
  itkGetConstMacro( MaximumError, double );

  /** Set/Get the desired limits of the Gaussian kernel width.
   * \sa GaussianOperator. */
  itkSetMacro( MaximumKernelWidth, unsigned int );
  itkGetConstMacro( MaximumKernelWidth, unsigned int );

  /** Get the metric value. The metric value is the mean square difference 
   * in intensity between the fixed image and transforming moving image 
   * computed over the the overlapping region between the two images. 
   * This value is calculated for the current iteration */
  virtual double GetMetric() const {return 0;}

  
  /** Utility to smooth the velocity field (represented in the Output)
   * using a Gaussian operator. The amount of smoothing can be specified
   * by setting the StandardDeviations. */
  virtual void SmoothVelocityField();


  /**
  * Enable boundary checking
  * @param boundary check
  */
 void                                  SetBoundaryCheck(bool);

 /**
  * Gets boundary checking
  */
 bool                                  GetBoundaryCheck(void);

protected:
  LCCDeformableRegistrationFilter();
  ~LCCDeformableRegistrationFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Exponential type */
  typedef ExponentialDeformationFieldImageFilter< 
    VelocityFieldType, DeformationFieldType >      FieldExponentiatorType;
  
  typedef typename FieldExponentiatorType::Pointer FieldExponentiatorPointer;

  itkSetObjectMacro( Exponentiator, FieldExponentiatorType );
  itkGetObjectMacro( Exponentiator, FieldExponentiatorType );

  /** Supplies the halting criteria for this class of filters.  The
   * algorithm will stop after a user-specified number of iterations. */
  virtual bool Halt()
    {

    if ( m_StopRegistrationFlag )
      {
      return true;
      }

    return this->Superclass::Halt();
    }

  /** A simple method to copy the data from the input to the output.
   * If the input does not exist, a zero field is written to the output. */
  virtual void CopyInputToOutput();

  /** Initialize the state of filter and equation before each iteration.
   * Progress feeback is implemented as part of this method. */
  virtual void InitializeIteration();

  /** Utility to smooth the UpdateBuffer using a Gaussian operator.
   * The amount of smoothing can be specified by setting the
   * UpdateFieldStandardDeviations. */
  virtual void SmoothUpdateField();

  /** Utility to smooth a velocity field  using a Gaussian operator.
   * The amount of smoothing can be specified by setting the
   * StandardDeviations. */
  virtual void SmoothGivenField(VelocityFieldType * field, const double StandardDeviations[ImageDimension]);
  
  /** This method is called after the solution has been generated. In this case,
   * the filter release the memory of the internal buffers. */
  virtual void PostProcessOutput();

  /** This method is called before iterating the solution. */
  virtual void Initialize();

  /** By default the output velocity field has the same Spacing, Origin
   * and LargestPossibleRegion as the input/initial velocity field.  If
   * the initial velocity field is not set, the output information is
   * copied from the fixed image. */
  virtual void GenerateOutputInformation();

  /** It is difficult to compute in advance the input moving image region
   * required to compute the requested output region. Thus the safest
   * thing to do is to request for the whole moving image.
   * For the fixed image and velocity field, the input requested region
   * set to be the same as that of the output requested region. */
  virtual void GenerateInputRequestedRegion();

private:
  LCCDeformableRegistrationFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  /** Standard deviation for Gaussian smoothing */
  double                    m_StandardDeviations[ImageDimension];
  double                    m_UpdateFieldStandardDeviations[ImageDimension];
  double                    m_SimilarityCriteriaStandardDeviations[ImageDimension];

  /**Trade-off coefficient**/
  double                    m_SigmaI;


  /**
   * Unit of the standard deviation : true if it is world unit, false if it is voxel unit.
   */
  bool                      m_StandardDeviationWorldUnit;

  /** Modes to control smoothing of the update and velocity fields */
  bool                      m_SmoothVelocityField;
  bool                      m_SmoothUpdateField;
  
  /** Temporary field used for smoothing the velocity field. */
  VelocityFieldPointer      m_TempField;

  /** Maximum error for Gaussian operator approximation. */
  double                    m_MaximumError;

  /** Limits of Gaussian kernel width. */
  unsigned int              m_MaximumKernelWidth;

  /** Flag to indicate user stop registration request. */
  bool                      m_StopRegistrationFlag;

  FieldExponentiatorPointer m_Exponentiator;
  FieldExponentiatorPointer m_InverseExponentiator;

  bool                      m_UseMask;
  MovingImageConstPointer   m_MaskImage;

  /**
   * Regularization type.
   */
  unsigned int              m_RegularizationType;

  /**
   * Weight for the penalization of the harmonic energy.
   */
  float                     m_HarmonicWeight;

  /**
   * Weight for the penalization of the bending energy.
   */
  float                     m_BendingWeight;

  /**
   * Boundary checking
   */
  bool                      m_BoundaryCheck;

};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLCCDeformableRegistrationFilter.txx"
#endif

#endif
