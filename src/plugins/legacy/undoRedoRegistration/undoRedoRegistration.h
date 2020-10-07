#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <itkProcessRegistration.h>
#include <undoRedoRegistrationPluginExport.h>
#include <itkImage.h>

class undoRedoRegistrationPrivate;

/**
 * @brief Registration process.
 *
 * This plugin uses the registration programming interface (RPI).
 * It also implements a custom toolbox plugging itself onto the generic registration toolbox available in medInria/src/medCore/gui.
 *
 */
class UNDOREDOREGISTRATIONPLUGIN_EXPORT undoRedoRegistration : public itkProcessRegistration
{
    Q_OBJECT
    
public:

    typedef itk::Image< float, 3 > RegImageType;
    /**
     * @brief Constructor.
     *
     * @param void
     */
    undoRedoRegistration(void);
    
    /**
     * @brief
     *
     * @param void
     */
    virtual ~undoRedoRegistration(void);
    
    /**
     * @brief Description of the plugin.
     *
     * @param void
     * @return QString
     */
    virtual QString description(void) const;
    
    /**
     * @brief tries to register the process with the factory.
     *
     * @param void
     * @return bool true if it succeeded, false otherwise.
     */
    static bool registered(void);

    void undo();
    
    void redo();

    void generateOutput(bool algorithm = false,dtkAbstractProcess * process = nullptr);

    virtual itk::Transform<double,3,3>::Pointer getTransform();
    virtual QString getTitleAndParameters();

protected :
    /**
     * @brief
     *
     * @param file The path to the file is assumed to be existing. However the file may not exist beforehand.
     * @return bool successful or not.
     */
    virtual bool writeTransform(const QString& file);

    virtual bool setInputData(medAbstractData *data, int channel);

private:
    undoRedoRegistrationPrivate *d;
    
};

/**
 * @brief Function to instantiate the process from the factory.
 * @see registered()
 *
 * @param void
 */
dtkAbstractProcess *createUndoRedoRegistration(void);


