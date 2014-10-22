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
#include <itkProcessRegistrationDiffeomorphicDemonsPluginExport.h>

class itkProcessRegistrationDiffeomorphicDemonsPrivate;

/**
 * @brief Registration process using diffeomorphic demons from itk.
 *
 * This plugin uses the registration programming interface (RPI).
 * It also implements a custom toolbox plugging itself onto the generic registration toolbox available in medInria/src/medCore/gui.
 *
*/
class ITKPROCESSREGISTRATIONDIFFEOMORPHICDEMONSPLUGIN_EXPORT itkProcessRegistrationDiffeomorphicDemons : public itkProcessRegistration
{
    Q_OBJECT

public:

    itkProcessRegistrationDiffeomorphicDemons();
    virtual ~itkProcessRegistrationDiffeomorphicDemons();

    virtual QString description() const;
    virtual QString identifier() const;

    static bool registered();

public:
    virtual int update(ImageType);

    void setNumberOfIterations(std::vector <unsigned int> iterations);

    virtual itk::Transform<double,3,3>::Pointer getTransform();
    virtual QString getTitleAndParameters();


protected :
    virtual bool writeTransform(const QString& file);

private:
    itkProcessRegistrationDiffeomorphicDemonsPrivate *d;
    friend class itkProcessRegistrationDiffeomorphicDemonsPrivate;
};


dtkAbstractProcess *createitkProcessRegistrationDiffeomorphicDemons();


