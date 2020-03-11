#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "manualRegistrationLandmark.h"
#include "manualRegistrationPluginExport.h"

#include <itkProcessRegistration.h>

class manualRegistrationPrivate;

/**
 * @brief Registration process using diffeomorphic demons from itk.
 *
 * This plugin uses the registration programming interface (RPI).
 * It also implements a custom toolbox plugging itself onto the generic registration toolbox available in medInria/src/medGui.
 *
*/
class MANUALREGISTRATIONPLUGIN_EXPORT manualRegistration : public itkProcessRegistration
{
    Q_OBJECT

public:

    enum TransformName {RIGID, AFFINE};

    /**
     * @brief Constructor.
     *
     * @param void
     */
    manualRegistration();

    /**
     * @brief
     *
     * @param void
    */
    virtual ~manualRegistration();

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

    /**
     * @brief Runs the process.
     *
     * @param ImageType the fixed image image type.
     * @return int successful or not.
    */
    virtual int update(ImageType);

    virtual itk::Transform<double,3,3>::Pointer getTransform();

    /**
     * @brief Get parameters for tooltip in undo/redo area.
     *
     * @param void
     * @return QString of the algorithm title & parameters
    */
    virtual QString getTitleAndParameters();

    void SetFixedLandmarks(QList<manualRegistrationLandmark*>* fixedLandmarks);
    void SetMovingLandmarks(QList<manualRegistrationLandmark*>* movingLandmarks);

    /**
     * @brief Writes the transformation, in this case the displacement field,
     * in a file.
     *
     * @param file The path to the file is assumed to be existing. However the file may not exist beforehand.
     * @return bool successful or not.
    */
    virtual bool writeTransform(const QString& file);

    void setParameter(int data);

private:
    manualRegistrationPrivate* d;
    friend class manualRegistrationPrivate;
};

/**
 * @brief Function to instantiate the process from the factory.
 * @see registered()
 *
 * @param void
*/
dtkAbstractProcess *createManualRegistration();


