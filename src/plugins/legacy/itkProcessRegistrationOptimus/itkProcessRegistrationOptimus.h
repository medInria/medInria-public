#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkProcessRegistration.h>
#include "itkProcessRegistrationOptimusPluginExport.h"

class itkProcessRegistrationOptimusPrivate;

class ITKPROCESSREGISTRATIONOPTIMUSPLUGIN_EXPORT itkProcessRegistrationOptimus : public itkProcessRegistration
{
    Q_OBJECT

public:
             itkProcessRegistrationOptimus(void);
    virtual ~itkProcessRegistrationOptimus(void);

    virtual QString identifier(void) const;
    virtual QString description(void) const;

    static bool registered(void);

public:
    virtual int update(ImageType);

    void setNumberOfIterations(unsigned int maxIterations);
    void setNumberOfHistogramBins(unsigned int nbOfHistogramBins);
    void setNumberOfInterpolations(unsigned int nbOfInterpolations);
    void setNumberOfSpatialSamples(unsigned int nbOfSpatialSamples);
    void setRhoStart(float rhoStart);
    void setRhoEnd(float rhoEnd);
    void setScalingCoefficient(float scalingCoefficient);

    virtual itk::Transform<double,3,3>::Pointer getTransform();
    virtual QString getTitleAndParameters();

//public slots:
//    bool write(const QString& file);
protected :
    virtual bool writeTransform(const QString& file);

private:
    itkProcessRegistrationOptimusPrivate *d;
    friend class itkProcessRegistrationOptimusPrivate;

};

dtkAbstractProcess *createitkProcessRegistrationOptimus(void);
