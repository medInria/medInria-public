#ifndef __itkMultiResolutionLCCDeformableRegistration_h
#define __itkMultiResolutionLCCDeformableRegistration_h

#include "itkExponentialDeformationFieldImageFilter2.h"
#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkLCCDeformableRegistrationFilter.h"
#include "itkSymmetricLCClogDemonsRegistrationFilter.h"
#include "itkMultiResolutionPyramidImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkShrinkImageFilter.h"


#include <vector>

namespace itk
{
/**
 * \class MultiResolutionLCCDeformableRegistration
 * \brief Framework for performing multi-resolution log-domain
 * deformable registration.
 *
 * MultiResolutionLCCDeformableRegistration provides a generic framework
 * to peform multi-resolution deformable registration.
 *
 * At each resolution level a LCCDeformableRegistrationFilter is used
 * to register two images by computing the velocity field whose exponential will 
 * map a moving image onto a fixed image.
 *
 * Velocity and deformation fields are represented as images whose pixel type is some
 * vector type with at least N elements, where N is the dimension of
 * the fixed image. The vector type must support element access via operator
 * []. It is assumed that the vector elements behave like floating point
 * scalars.
 *
 * The internal LCCDeformableRegistrationFilter can be set using
 * SetRegistrationFilter. By default a LCClogDemonsRegistrationFilter is used.
 *
 * The input fixed and moving images are set via methods SetFixedImage
 * and SetMovingImage respectively. An initial velocity field maybe set via
 * SetInitialVelocityField if is matches the characteristics of the coarsest
 * pyramid level. If no such assumption can be made (e.g. the velocity field
 * has the same characteristics as the input images), an initial velocity
 * field can still be set via SetArbitraryInitialVelocityField or
 * SetInput. The filter will then take care of mathching the coarsest level
 * characteristics. If no initial field is set a zero field is used as the
 * initial condition.
 *
 * MultiResolutionPyramidImageFilters are used to downsample the fixed
 * and moving images. A VectorExpandImageFilter is used to upsample
 * the velocity field as we move from a coarse to fine solution.
 *
 * This class is templated over the fixed image type, the moving image type,
 * and the velocity/deformation Field type.
 *
 * \warning This class assumes that the fixed, moving and
 * field image types all have the same number of dimensions.
 *
 * \author Marco Lorenzi, Florence Dru, INRIA and Tom Vercauteren, MKT
 *
 * \sa LCCDeformableRegistrationFilter
 * \sa LCClogDemonsRegistrationFilter
 * \sa MultiResolutionPyramidImageFilter
 * \sa VectorExpandImageFilter
 *
 * The current implementation of this class does not support streaming.
 *
 * \ingroup DeformableImageRegistration
 */
template <class TFixedImage, class TMovingImage, class TField, class  TRealType = float>
class ITK_EXPORT MultiResolutionLCCDeformableRegistration :
    public ImageToImageFilter <TField, TField>
{
public:
  /** Standard class typedefs */
  typedef MultiResolutionLCCDeformableRegistration Self;
  typedef ImageToImageFilter<TField, TField>
                                                   Superclass;
  typedef SmartPointer<Self>                       Pointer;
  typedef SmartPointer<const Self>                 ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( MultiResolutionLCCDeformableRegistration,
                ImageToImageFilter );

  /** Fixed image type. */
  typedef TFixedImage                            FixedImageType;
  typedef typename FixedImageType::Pointer       FixedImagePointer;
  typedef typename FixedImageType::ConstPointer  FixedImageConstPointer;

  /** Moving image type. */
  typedef TMovingImage                           MovingImageType;
  typedef typename MovingImageType::Pointer      MovingImagePointer;
  typedef typename MovingImageType::ConstPointer MovingImageConstPointer;

  /** Velocity field type. */
  typedef TField                                 VelocityFieldType;
  typedef typename VelocityFieldType::Pointer    VelocityFieldPointer;
  
  /** Deformation field image type. */
  typedef TField                                 DeformationFieldType;
  typedef typename DeformationFieldType::Pointer DeformationFieldPointer;

  /** ImageDimension. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      FixedImageType::ImageDimension);

  /** Internal float image type. */
  typedef Image<TRealType,itkGetStaticConstMacro(ImageDimension)> FloatImageType;

  /** The internal registration type. */
  typedef LCCDeformableRegistrationFilter<FloatImageType, FloatImageType, VelocityFieldType >
                                             RegistrationType;
  typedef typename RegistrationType::Pointer RegistrationPointer;

  /** The default registration type. */
  typedef SymmetricLCClogDemonsRegistrationFilter<
    FloatImageType, FloatImageType, VelocityFieldType > DefaultRegistrationType;

  /** The fixed multi-resolution image pyramid type. */
  typedef MultiResolutionPyramidImageFilter<FixedImageType, FloatImageType >
                                                  FixedImagePyramidType;
  typedef typename FixedImagePyramidType::Pointer FixedImagePyramidPointer;

  /** The moving multi-resolution image pyramid type. */
  typedef MultiResolutionPyramidImageFilter<MovingImageType, FloatImageType >
                                                   MovingImagePyramidType;
  typedef typename MovingImagePyramidType::Pointer MovingImagePyramidPointer;
   
  /** The velocity field expander type. */
  typedef ResampleImageFilter<VelocityFieldType, VelocityFieldType >
                                               FieldExpanderType;
  typedef typename FieldExpanderType::Pointer  FieldExpanderPointer;

   /** The image expander type. */
  typedef ShrinkImageFilter< MovingImageType,MovingImageType >
                                               ImageExpanderType;
  typedef typename ImageExpanderType::Pointer  ImageExpanderPointer;



  /** Set the fixed image. */
  virtual void SetFixedImage( const FixedImageType * ptr );

  /** Get the fixed image. */
  const FixedImageType * GetFixedImage(void) const;

  /** Set the moving image. */
  virtual void SetMovingImage( const MovingImageType * ptr );

  /** Get the moving image. */
  const MovingImageType * GetMovingImage(void) const;

  /** Set the flag for the mask image. */
  void UseMask(bool);

  /** Set the Mask image. */
  virtual void SetMaskImage( const MovingImageType * ptr );

  /** Get the mask image. */
  const MovingImageType * GetMaskImage(void) const;


  /** Set initial velocity field to be used as is (no smoothing, no
   *  subsampling at the coarsest level of the pyramid. */
  virtual void SetInitialVelocityField( VelocityFieldType * ptr )
    {

    this->m_InitialVelocityField=ptr;

    }

  /** Set initial velocity field. No assumption is made on the
   *  input. It will therefore be smoothed and resampled to match the
   *  images characteristics at the coarsest level of the pyramid. */
  virtual void SetArbitraryInitialVelocityField( VelocityFieldType * ptr )
    {
#if (ITK_VERSION_MAJOR < 4)
    this->SetNthInput(0, ptr );
#else
    this->SetNthInput(0, ptr);
#endif
    }

  /** Get output velocity field. */
  VelocityFieldType * GetVelocityField() { return this->GetOutput(); }

  /** Get output deformation field. */
  DeformationFieldPointer GetDeformationField();

  /** Get output inverse deformation field. */
  DeformationFieldPointer GetInverseDeformationField();

  /** Get the number of valid inputs.  For
   * MultiResolutionLCCDeformableRegistration, this checks whether the
   * fixed and moving images have been set. While
   * MultiResolutionLCCDeformableRegistration can take a third input
   * as an initial velocity field, this input is not a required input. */
  virtual std::vector<SmartPointer<DataObject> >::size_type GetNumberOfValidRequiredInputs() const;

  /** Set the internal registrator. */
  itkSetObjectMacro( RegistrationFilter, RegistrationType );

  /** Get the internal registrator. */
  itkGetObjectMacro( RegistrationFilter, RegistrationType );
  
  /** Set the fixed image pyramid. */
  itkSetObjectMacro( FixedImagePyramid, FixedImagePyramidType );

  /** Get the fixed image pyramid. */
  itkGetObjectMacro( FixedImagePyramid, FixedImagePyramidType );

  /** Set the moving image pyramid. */
  itkSetObjectMacro( MovingImagePyramid, MovingImagePyramidType );

  /** Get the moving image pyramid. */
  itkGetObjectMacro( MovingImagePyramid, MovingImagePyramidType );

  /** Set number of multi-resolution levels. */
  virtual void SetNumberOfLevels( unsigned int num );

  /** Get number of multi-resolution levels. */
  itkGetConstReferenceMacro( NumberOfLevels, unsigned int );

  /** Get the current resolution level being processed. */
  itkGetConstReferenceMacro( CurrentLevel, unsigned int );

  /** Set number of iterations per multi-resolution levels. */
  itkSetVectorMacro( NumberOfIterations, unsigned int, m_NumberOfLevels );

  /** Set the moving image pyramid. */
  itkSetObjectMacro( FieldExpander, FieldExpanderType );

  /** Get the moving image pyramid. */
  itkGetObjectMacro( FieldExpander, FieldExpanderType );

  /** Get number of iterations per multi-resolution levels. */
  virtual const unsigned int * GetNumberOfIterations() const
  { return &(m_NumberOfIterations[0]); }

  /** Stop the registration after the current iteration. */
  virtual void StopRegistration();

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


  /**
   * Gets the standard deviation used for the global field regularization.
   */
  const double * GetStandardDeviations(void) const
  {
      return static_cast<const double*>( this->m_StandardDeviations );
  }


  /**
   * Sets if the velocity field is smoothed or not
   */
  itkSetMacro( SmoothVelocityField, bool );
  itkGetConstMacro( SmoothVelocityField, bool );
  itkBooleanMacro( SmoothVelocityField );


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


  /**
   * Gets the standard deviation used for the update field regularization.
   */
  const double * GetUpdateFieldStandardDeviations(void) const
  {
      return static_cast<const double*>( this->m_UpdateFieldStandardDeviations );
  }

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


  /**
   * Gets the standard deviation used for the similairty criteria.
   */
  const double * GetSimilarityCriteriaStandardDeviations(void) const
  {
      return static_cast<const double*>( this->m_SimilarityCriteriaStandardDeviations );
  }

  /**
   * Sets the trade-off coefficient
   * @param  value  SigmaI
   */
  virtual void SetSigmaI( double value );


  /**
   * Gets the trade-off coefficient. 
   */
  double GetSigmaI(void) const
  {
      return(this->m_SigmaI );
  }


  /**
  * Enable boundary checking
  * @param boundary check
  */
 void                                  SetBoundaryCheck(bool);

  /**
   * Sets if the velocity field is smoothed or not
   */
  itkSetMacro( SmoothUpdateField, bool );
  itkGetConstMacro( SmoothUpdateField, bool );
  itkBooleanMacro( SmoothUpdateField );



protected:
  MultiResolutionLCCDeformableRegistration();
  ~MultiResolutionLCCDeformableRegistration() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Exponential type */
  typedef ExponentialDeformationFieldImageFilter< 
    VelocityFieldType, DeformationFieldType >      FieldExponentiatorType;
  
  typedef typename FieldExponentiatorType::Pointer FieldExponentiatorPointer;

  itkSetObjectMacro( Exponentiator, FieldExponentiatorType );
  itkGetObjectMacro( Exponentiator, FieldExponentiatorType );

  /** Generate output data by performing the registration
   * at each resolution level. */
  virtual void GenerateData();

  /** The current implementation of this class does not support
   * streaming. As such it requires the largest possible region
   * for the moving, fixed and input velocity field. */
  virtual void GenerateInputRequestedRegion();

  /** By default, the output velocity field has the same
   * spacing, origin and LargestPossibleRegion as the input/initial
   * velocity field. If the initial velocity field is not set, the output
   * information is copied from the fixed image. */
  virtual void GenerateOutputInformation();

  /** The current implementation of this class does not supprot
   * streaming. As such it produces the output for the largest
   * possible region. */
  virtual void EnlargeOutputRequestedRegion( DataObject *ptr );

  /** This method returns true to indicate that the registration should
   * terminate at the current resolution level. */
  virtual bool Halt();

private:
  MultiResolutionLCCDeformableRegistration(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  RegistrationPointer        m_RegistrationFilter;
  FixedImagePyramidPointer   m_FixedImagePyramid;
  MovingImagePyramidPointer  m_MovingImagePyramid;
  FieldExpanderPointer       m_FieldExpander;
  VelocityFieldPointer       m_InitialVelocityField;
  ImageExpanderPointer       m_ImageExpander;

  unsigned int               m_NumberOfLevels;
  unsigned int               m_CurrentLevel;
  std::vector<unsigned int>  m_NumberOfIterations;

  /** Flag to indicate user stop registration request. */
  bool                      m_StopRegistrationFlag;

  FieldExponentiatorPointer m_Exponentiator;

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
   * Standard deviation for global field regularization
   */
  double                    m_StandardDeviations[ImageDimension];

  /**
   * Standard deviation for update field regularization
   */
  double                    m_UpdateFieldStandardDeviations[ImageDimension];

/**
   * Standard deviation for the similarity criteria
   */
  double                    m_SimilarityCriteriaStandardDeviations[ImageDimension];

/**
   * Trade-off coefficient
   */
  double                    m_SigmaI;


  /**
   * Unit of the standard deviation : true if it is world unit, false if it is voxel unit.
   */
  bool                      m_StandardDeviationWorldUnit;

  /**
   * Modes to control smoothing of the velocity field
   */
  bool                      m_SmoothVelocityField;

  /**
   * Modes to control smoothing of the velocity field
   */
  bool                      m_SmoothUpdateField;

  /**
   * Mask Image flag
   */
  bool                      m_UseMask;

  /**
   * Mask Image
   */
  MovingImageConstPointer   m_MaskImage;

  /**
   * Boundary checking
   */
  bool                      m_BoundaryCheck;


};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMultiResolutionLCCDeformableRegistration.txx"
#endif


#endif
