/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkProcessRegistration.h>
#include <diffeomorphicDemonsPluginExport.h>

class diffeomorphicDemonsProcessPrivate;

/**
 * @brief Registration process using diffeomorphic demons from itk.
 *
 * This plugin uses the registration programming interface (RPI).
 * It also implements a custom toolbox plugging itself onto the generic registration toolbox available in medInria/src/medCore/gui.
 *
*/
class DIFFEOMORPHICDEMONSPLUGIN_EXPORT diffeomorphicDemonsProcess : public itkProcessRegistration
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     *
     * @param void
     */
    diffeomorphicDemonsProcess();

    /**
     * @brief
     *
     * @param void
    */
    virtual ~diffeomorphicDemonsProcess();

    /**
     * @brief Description of the plugin.
     *
     * @param void
     * @return QString
     */
    virtual QString description() const;

    /**
     * @brief Identifier of the plugin.
     *
     * @param void
     * @return QString
     */
    virtual QString identifier() const;

    /**
     * @brief tries to register the process with the factory.
     *
     * @param void
     * @return bool true if it succeeded, false otherwise.
    */
    static bool registered();

public:
    /**
     * @brief Runs the process.
     *
     * @param ImageType the fixed image image type.
     * @return int successful or not.
    */
    virtual int update(ImageType);

    /**
     * @brief
     *
     * @see rpidiffeomorphicDemonsProcess
     *
     * @param iterations
    */
    void setNumberOfIterations(std::vector <unsigned int> iterations);

    /**
     * @brief
     * @see rpidiffeomorphicDemonsProcess
     * @param updateRule
    */
    void setUpdateRule( unsigned char updateRule);

    /**
     * @brief
     * @see rpidiffeomorphicDemonsProcess
     * @param gradientType
    */
    void setGradientType (unsigned char gradientType);

    /**
     * @brief
     * @see rpidiffeomorphicDemonsProcess
     * @param maximumUpdateStepLength
    */
    void setMaximumUpdateLength (float maximumUpdateStepLength);

    /**
     * @brief
     * @see rpidiffeomorphicDemonsProcess
     * @param updateFieldStandardDeviation
    */
    void setUpdateFieldStandardDeviation(float updateFieldStandardDeviation);

    /**
     * @brief
     * @see rpidiffeomorphicDemonsProcess
     * @param displacementFieldStandardDeviation
    */
    void setDisplacementFieldStandardDeviation(float displacementFieldStandardDeviation);

    /**
     * @brief
     * @see rpidiffeomorphicDemonsProcess
     * @param useHistogramMatching
    */
    void setUseHistogramMatching(bool useHistogramMatching);

    virtual itk::Transform<double,3,3>::Pointer getTransform();
    virtual QString getTitleAndParameters();

protected :
    /**
     * @brief Writes the transformation, in this case the displacement field,
     * in a file.
     *
     * @param file The path to the file is assumed to be existing. However the file may not exist beforehand.
     * @return bool successful or not.
    */
    virtual bool writeTransform(const QString& file);

private:
    diffeomorphicDemonsProcessPrivate *d;
    friend class diffeomorphicDemonsProcessPrivate;
};

/**
 * @brief Function to instantiate the process from the factory.
 * @see registered()
 *
 * @param void
*/
dtkAbstractProcess *creatediffeomorphicDemonsProcess();


