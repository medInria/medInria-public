#ifndef _RPI_LCC_LOG_DEMONS_HXX_
#define _RPI_LCC_LOG_DEMONS_HXX_



#include <itkStationaryVelocityFieldTransform.h>
#include <rpiDisplacementFieldTransform.h>
#include "rpiRegistrationMethod.hxx"


// Namespace RPI : Registration Programming Interface
namespace rpi
{


/**
 * LCC-Demons registration method.
 *
 * There are three templates for this class:
 *
 *   TFixedImage           Type of the fixed image. Must be an itk::Image< TPixel, 3 > where
 *                         TPixel is a scalar type among "double" and "double".
 *
 *   TMovingImage          Type of the moving image. Must be an itk::Image< TPixel, 3 > where
 *                         TPixel is a scalar type among "double" and "double".
 *
 *   TTransformScalarType  Type of the transformation parameters. Must be "double" or "double".
 *
 * In the current implementation, TFixedImage and TMovingImage must be identical.
 *
 * @author Marco Lorenzi, Vincent Garcia, and Tom Vercauteren
 * @date   2011/04/01
 */
template < class TFixedImage, class TMovingImage, class TTransformScalarType=double >
class LCClogDemons : public RegistrationMethod< TFixedImage, TMovingImage, TTransformScalarType >{


public:


    /**
     * Update rule
     */
    enum UpdateRule {
        UPDATE_LOG_DOMAIN,                /** exp(v) <- exp(v) o exp(u) */
        UPDATE_SYMMETRIC_LOG_DOMAIN,      /** exp(v) <- symmetrized( exp(v) o exp(u) ) */
        UPDATE_SYMMETRIC_LOCAL_LOG_DOMAIN /** exp(v) <- symmetrized( exp(v) o exp(u) ) - Local Correlation Criteria */
    };


    /**
     * Gradient type
     */
    enum GradientType {
        GRADIENT_SYMMETRIZED,         /** symmetrized         */
        GRADIENT_FIXED_IMAGE,         /** fixed image         */
        GRADIENT_WARPED_MOVING_IMAGE, /** warped moving image */
        GRADIENT_MAPPED_MOVING_IMAGE  /** mapped moving image */
    };

    typedef TMovingImage MovingImageType;    

    typedef typename TMovingImage::Pointer MovingImagePointerType;

    typedef itk::StationaryVelocityFieldTransform< TTransformScalarType, TFixedImage::ImageDimension >
            TransformType;

    typedef typename TransformType::Pointer
            TransformPointerType;

    typedef rpi::DisplacementFieldTransform< TTransformScalarType, TFixedImage::ImageDimension >
            DisplacementFieldTransformType;

    typedef typename DisplacementFieldTransformType::Pointer
            DisplacementFieldTransformPointerType;


protected:

    /**
     * Number of iterations per level of resolution (from coarse to fine levels).
     */
    std::vector<unsigned int>              m_iterations;


    /**
     * Update rule.
     */
    UpdateRule                             m_updateRule;


    /**
     * Type of gradient used for computing the demons force.
     */
    GradientType                           m_gradientType;


    /**
     * Maximum length of an update vector (voxel units).
     */
    double                                  m_maximumUpdateStepLength;

    /**
     * Regularization type.
     */
    unsigned int                           m_RegularizationType;

    /**
     * Weight for the penalization of the harmonic energy.
     */
    double                           m_HarmonicWeight;

    /**
     * Weight for the penalization of the bending energy.
     */
    double                           m_BendingWeight;

    /**
     * Standard deviation (world unit) of the update field (Gaussian) smoothing.
     */
    double                                  m_updateFieldStandardDeviation;


    /**
     * Standard deviation (world unit) of the stationary velocity field (Gaussian) smoothing.
     */
    double                                  m_velocityFieldStandardDeviation;


    /**
     * Standard deviation (world unit) of the Similarity Criteria (Gaussian) smoothing.
     */
    double                                  m_SimilarityCriteriaStandardDeviation;

    /**
     *  Trade-off coefficient
     */
    double                                  m_SigmaI;

    /**
     * Histogram matching
     */
    bool                                   m_useHistogramMatching;


    /**
     *  Verbosity
     */
    bool                                   m_verbosity;


    /**
     * Number of terms in the BCH expansion.
     */
    unsigned int                           m_BCHExpansion;


    /**
     * Initial transformation.
     */
    DisplacementFieldTransformPointerType                   m_initialTransform;


    /**
     * Displacement field transformation
     */
    DisplacementFieldTransformPointerType  m_displacementFieldTransform;


    /**
      * TrueField
      */
    DisplacementFieldTransformPointerType  m_TrueField;

    /**
      * Mask Image flag
      */

    bool                                   m_UseMask;


    /**
      * Mask Image
      */    

    MovingImagePointerType                 m_MaskImage;


    /**
      * Bondary Checking
      */

    bool                                   m_BoundaryCheck;

public:

    /**
     * Class constructor.
     */
    LCClogDemons(void);


    /**
     * Class destructor.
     */
    virtual ~LCClogDemons(void);


    /**
     * Gets the number of iterations per level of resolution.
     * @return  number of iterations
     */
    std::vector<unsigned int>              GetNumberOfIterations(void) const;


    /**
     * Sets the number of iterations per level of resolution.
     * @param  iterations  number of iterations
     */
    void                                   SetNumberOfIterations(std::vector<unsigned int> iterations);


    /**
     * Gets the update rule.
     * @return  update rule
     */
    UpdateRule                             GetUpdateRule(void) const;


    /**
     * Sets the update rule:
     *   LOG_DOMMAIN          : exp(v) <- exp(v) o exp(u)
     *   SYMMETRIC_LOG_DOMAIN : exp(v) <- symmetrized( exp(v) o exp(u) )
     * @param  value  update rule
     */
    void                                   SetUpdateRule(UpdateRule value);


    /**
     * Gets the gradient type used for computing the demons force.
     * @return  gradient type
     */
    GradientType                           GetGradientType(void) const;


    /**
     * Sets the gradient type used for computing the demons force.
     * @param  value  gradient type
     */
    void                                   SetGradientType(GradientType value);


    /**
     * Gets the maximum length of an update vector (voxel units).
     * @return  maximum length
     */
    double                                  GetMaximumUpdateStepLength(void) const;


    /**
     * Sets the maximum length of an update vector (voxel units).
     * Setting it to 0 implies no restrictions will be made on the step length.
     * @param  value  maximum length
     */
    void                                   SetMaximumUpdateStepLength(double value);

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
    void                                   SetHarmonicWeight(double harmonicWeight) ;

    /**
     * Gets the weight for the penalization of the harmonic energy.
     * @return  harmonic weight
     */
    double                                  GetHarmonicWeight(void);

    /**
     * Sets the weight for the penalization of the bending energy.
     * @param  harmonic weight
     */
    void                                   SetBendingWeight(double bendingWeight) ;

    /**
     * Gets the weight for the penalization of the bending energy.
     * @return  bending weight
     */
    double                                  GetBendingWeight(void);

    /**
     * Gets the standard deviation (world unit) of the update field (Gaussian) smoothing.
     * Setting it below 0.5 means no smoothing will be performed.
     * @return  standard deviation
     */
    double                                  GetUpdateFieldStandardDeviation(void) const;


    /**
     * Sets the standard deviation (world unit) of the update field (Gaussian) smoothing.
     * @param  value  standard deviation
     */
    void                                   SetUpdateFieldStandardDeviation(double value);


    /**
     * Gets the standard deviation (world unit) of the stationary velocity field (Gaussian) smoothing.
     * Setting it below 0.5 means no smoothing will be performed.
     * @return  standard deviation
     */
    double                                  GetStationaryVelocityFieldStandardDeviation(void) const;


    /**
     * Sets the standard deviation (world unit) of the stationary velocity field (Gaussian) smoothing.
     * @param  value  standard deviation
     */
    void                                   SetStationaryVelocityFieldStandardDeviation(double value);

    /**
     * Gets the standard deviation (world unit) of the Similarity Criteria (Gaussian) smoothing.
     * @return  standard deviation
     */
    double                                  GetSimilarityCriteriaStandardDeviation(void) const;
    double                                  GetSigmaI(void) const;


    /**
     * Sets the standard deviation (world unit) of the Similarity Criteria (Gaussian) smoothing.
     * @param  value  standard deviation
     */
    void                                   SetSimilarityCriteriaStandardDeviation(double value);

    /**
     * Sets the trade-off coefficient.
     * @param  SigmaI
     */

    void                                   SetSigmaI(double value);

    /**
     * Does the algorithm uses histogram matching before processing?
     * @return  true if histogram matching is used, and false otherwise.
     */
    bool                                   GetUseHistogramMatching(void) const;


    /**
     * Sets if the algorithm uses histogram matching before processing or not?
     * @param  value  true if histogram matching sould be used, and false otherwise.
     */
    void                                   SetUseHistogramMatching(bool value);


    /**
     * Sets verbosity
     * @param  value  true for verbose mode
     */
    void                                   SetVerbosity(bool value);


    /**
     * Gets the number of terms in the BCH expansion.
     * @return  number of terms
     */
    unsigned int                           GetNumberOfTermsBCHExpansion(void) const;


    /**
     * Sets the number of terms in the BCH expansion.
     * @param  value  number of terms
     */
    void                                   SetNumberOfTermsBCHExpansion(unsigned int value);


    /**
     * Gets the initial transformation.
     * @return  initial transformation
     */
    TransformPointerType                   GetInitialTransformation(void) const;


    /**
     * Sets the initial transformation.
     * @param  transform  initial transformation
     */
    void                                   SetInitialTransformation(DisplacementFieldTransformType * transform);


    /**
     * Sets the true displcement field.
     * @param  transform  true field
     */
    void                                   SetTrueField(DisplacementFieldTransformType * transform);


    /**
     * Gets the output displacement field transformation.
     * @return  displacement field transformation
     */
    DisplacementFieldTransformPointerType  GetDisplacementFieldTransformation(void) const;


    /**
     * Performs the image registration. Must be called before GetTransformation().
     */
    virtual void                           StartRegistration(void);

    /**
     * Enable the use of the mask
     * @param use mask
     */
    void                                  UseMask(bool);

   /**
     * Sets the Mask Image.
     * @param  mask image
     */
    void                                   SetMaskImage(MovingImageType * MaskImage);


    /**
     * Gets the MaskImage.
     * @return  mask image
     */
    MovingImagePointerType  GetMaskImage(void) const;


    /**
     * Enable boundary checking
     * @param boundary check
     */
    void                                  SetBoundaryCheck(bool);

    /**
     * Gets boundary checking
     */
    bool                                  GetBoundaryCheck(void);

};


} // End of namespace


/** Add the source code file (template) */
#include "rpiLCClogDemons.txx"


#endif // _RPI_LCC_LOG_DEMONS_HXX_
