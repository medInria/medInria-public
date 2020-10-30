#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkEuler3DTransform.h>
#include "rpiRegistrationMethod.hxx"


// Namespace RPI : Registration Programming Interface
namespace rpi {


/**
 * Optimus regstration method. This method is based on the New UOA optimizer.
 *
 * There are three templates for this class:
 *
 *   TFixedImage           Type of the fixed image. Must be an itk::Image< TPixel, 3 > where
 *                         TPixel is a scalar type (e.g. unsigned char, short, float, etc.).
 *
 *   TMovingImage          Type of the moving image. Must be an itk::Image< TPixel, 3 > where
 *                         TPixel is a scalar type (e.g. unsigned char, short, float, etc.).
 *
 *   TTransformScalarType  Type of the transformation parameters. Must be "double".
 *
 * @author Vincent Garcia
 * @date   2010/10/14
 */
template < class TFixedImage, class TMovingImage, class TTransformScalarType=double >
class ITK_EXPORT Optimus : public RegistrationMethod< TFixedImage, TMovingImage, TTransformScalarType >{

public:

    typedef itk::Euler3DTransform<TTransformScalarType>
            TransformType;

    typedef typename TransformType::Pointer
            TransformPointerType;


protected:

    /**
     * Number of interpolations.
     */
    unsigned int         m_interpolations;

    /**
     * Maximal number of iterations.
     */
    unsigned int         m_iterations;

    /**
     * Number of histogram bins for the metric (mutual information).
     */
    unsigned int         m_histogramBins;

    /**
     * Number of spatial samples for the metric (mutual information).
     */
    unsigned int         m_spatialSamples;

    /**
     * Staring value of rho.
     */
    float                m_rhoStart;

    /**
     * Ending value of rho.
     */
    float                m_rhoEnd;

    /**
     * Scaling coefficient.
     */
    float                m_scalingCoefficient;

    /**
     * Initial transformation.
     */
    TransformPointerType  m_initialTransform;

public:


    /**
     * Class constructor.
     */
    Optimus(void);


    /**
     * Class destructor.
     */
    virtual ~Optimus(void);

    /**
     * Gets the initial transformation.
     * @return  initial transformation
     */
    TransformPointerType  GetInitialTransformation(void) const;

    /**
     * Sets the initial transformation.
     * @param  transform  initial transformation
     */
    void                 SetInitialTransformation(TransformType * transform);


    /**
     * Gets the number of interpolations.
     * @return  number of interpolations
     */
    unsigned int         GetNumberOfInterpolations(void) const;


    /**
     * Sets the number of interpolations.
     * @param  value  number of interpolations
     */
    void                 SetNumberOfInterpolations(unsigned int value);


    /**
     * Returns the maximal number of iterations allowed for the registration process.
     * @return  maximum number of iterations
     */
    unsigned int         GetNumberOfIterations(void) const;


    /**
     * Sets the maximal number of iterations allowed for the registration process.
     * This number must be greater than or equal to the number of interpolations.
     * @param  value  maximum number of iterations
     */
    void                 SetNumberOfIterations(unsigned int value);


    /**
     * Gets the number of histogram bins used for the metric (mutual information).
     * @return  number of histogram bins
     */
    unsigned int         GetNumberOfHistogramBins(void) const;


    /**
     * Sets the number of histogram bins used for the metric (mutual information).
     * @param  value  number of histogram bins
     */
    void                 SetNumberOfHistogramBins(unsigned int value);


    /**
     * Gets the number of spatial samples (points used to register images).
     * @return  number of spatial samples
     */
    unsigned int         GetNumberOfSpatialSamples(void) const;


    /**
     * Sets the number of spatial samples (points used to register images).
     * @param  value  number of spatial samples
     */
    void                 SetNumberOfSpatialSamples(unsigned int value);


    /**
     * Gets the starting value of rho.
     * @return  starting value of rho
     */
    float                GetRhoStart(void) const;


    /**
     * Sets the starting value of rho.
     * @param  value  starting value of rho
     */
    void                 SetRhoStart(float value);


    /**
     * Gets the ending value of rho.
     * @return  ending value of rho
     */
    float                GetRhoEnd(void) const;


    /**
     * Sets the ending value of rho.
     * @param  value  ending value of rho
     */
    void                 SetRhoEnd(float value);


    /**
     * Gets the scaling coefficient.
     * @return  scaling coefficient
     */
    float                GetScalingCoefficient(void) const;


    /**
     * Sets the scaling coefficient.
     * @param  value  scaling coefficient
     */
    void                 SetScalingCoefficient(float value);


    /**
     * Performs the image registration.
     */
    virtual void         StartRegistration(void);

};


} // End of namespace


/** Add the source code file (template) */
#include "rpiOptimus.tpp"

