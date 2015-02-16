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
#include <medDiffeomorphicDemonsRegistrationExport.h>

class medDiffeomorphicDemonsRegistrationPrivate;

/**
 * @brief Registration process using diffeomorphic demons from itk.
 *
 * This plugin uses the registration programming interface (RPI).
 * It also implements a custom toolbox plugging itself onto the generic registration toolbox available in medInria/src/medCore/gui.
 *
*/
class MEDDIFFEROMORPHICDEMONSREGISTRATIONPLUGIN_EXPORT medDiffeomorphicDemonsRegistrationProcess : public itkProcessRegistration
{
    Q_OBJECT

public:

    medDiffeomorphicDemonsRegistrationProcess();
    virtual ~medDiffeomorphicDemonsRegistrationProcess();

    virtual QString description() const;
    virtual QString identifier() const;

    static bool registered();

public:
    virtual int update(ImageType);

    virtual itk::Transform<double,3,3>::Pointer getTransform();
    virtual QString getTitleAndParameters();

public:
    QList<medAbstractParameter*> parameters();


protected :
    virtual bool writeTransform(const QString& file);

private:
    medDiffeomorphicDemonsRegistrationPrivate *d;
    friend class medDiffeomorphicDemonsRegistrationPrivate;
};


dtkAbstractProcess *createmedDiffeomorphicDemonsRegistration();


