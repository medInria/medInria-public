/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkProcessRegistration.h>
#include "LCCLogDemonsPluginExport.h"

class LCCLogDemonsPrivate;

/**
 * @brief Registration process.
 *
 * This plugin uses the registration programming interface (RPI).
 * It also implements a custom toolbox plugging itself onto the generic registration toolbox available in medInria/src/medGui.
 *
 */
class LCCLogDemonsPLUGIN_EXPORT LCCLogDemons : public itkProcessRegistration
{
    Q_OBJECT
    
public:

    LCCLogDemons();
    virtual ~LCCLogDemons();

    virtual QString description() const;
    static bool registered();
    
    virtual void setUpdateRule(int rule);

    virtual void setVerbosity(bool verbose);
    virtual void setMaximumUpdateStepLength(double value);
    virtual void setGradientType(int value);
    virtual void setUseHistogramMatching(bool flag);
    virtual void setNumberOfIterations(std::vector<unsigned int> iterations);
    virtual void setSimilarityCriteriaSigma(double std);
    virtual void setSigmaI(double sigmaI);
    virtual void setBoundaryCheck(bool flag);
    virtual void setUpdateFieldSigma(double sigma);
    virtual void setVelocityFieldSigma(double sigma);
    virtual void setNumberOfTermsBCHExpansion(unsigned int number);
    virtual void useMask(bool flag);
    virtual void setInterpolatorType(int value);

    /**
     * @brief Runs the process.
     *
     * @param ImageType the fixed image image type.
     * @return int successful or not.
     */
    virtual int update(ImageType);
    
    /**
     * @brief Get parameters for tooltip in undo/redo area.
     *
     * @param void
     * @return QString of the algorithm title & parameters
    */
    virtual QString getTitleAndParameters();

protected :
    /**
     * @brief
     *
     * @param file The path to the file is assumed to be existing. However the file may not exist beforehand.
     * @return bool successful or not.
     */
    virtual bool writeTransform(const QString& file);
    
    virtual itk::Transform<double,3,3>::Pointer getTransform();

private:
    LCCLogDemonsPrivate *d;
};

/**
 * @brief Function to instantiate the process from the factory.
 * @see registered()
 *
 * @param void
 */
dtkAbstractProcess *createLCCLogDemons();


