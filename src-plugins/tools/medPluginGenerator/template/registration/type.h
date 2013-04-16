/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/




#pragma once

#include <itkProcessRegistration/itkProcessRegistration.h>
#include "%1PluginExport.h"

class %1Private;

/**
 * @brief Registration process.
 *
 * This plugin uses the registration programming interface (RPI).
 * It also implements a custom toolbox plugging itself onto the generic registration toolbox available in medInria/src/medGui.
 *
 */
class %2PLUGIN_EXPORT %1 : public itk%3Registration
{
    Q_OBJECT
    
public:
    /**
     * @brief Constructor.
     *
     * @param void
     */
    %1();
    
    /**
     * @brief
     *
     * @param void
     */
    virtual ~%1();
    
    /**
     * @brief Description of the plugin.
     *
     * @param void
     * @return QString
     */
    virtual QString description() const;
    
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
    
    
protected :
    /**
     * @brief
     *
     * @param file The path to the file is assumed to be existing. However the file may not exist beforehand.
     * @return bool successful or not.
     */
    virtual bool writeTransform(const QString& file);
    
private:
    %1Private *d;
    
};

/**
 * @brief Function to instantiate the process from the factory.
 * @see registered()
 *
 * @param void
 */
dtkAbstract%3 *create%4();


