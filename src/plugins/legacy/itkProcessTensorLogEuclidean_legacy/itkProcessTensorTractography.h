/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDiffusionProcess.h>

#include "itkProcessTensorLogEuclideanPluginExport.h"

class itkProcessTensorTractographyPrivate;
    
class ITKPROCESSTENSORLOGEUCLIDEANPLUGIN_EXPORT itkProcessTensorTractography : public medAbstractDiffusionProcess
{
    Q_OBJECT
    
public:
    itkProcessTensorTractography();
    virtual ~itkProcessTensorTractography();
    
    virtual QString identifier() const;
    virtual QString description() const;
    
    static bool registered();
    
    void setInputImage(medAbstractData *input);
    medAbstractData *output();
    
    void setFA1Value (double value);
    void setFA2Value (double value);
    void setSmoothnessValue (double value);
    void setSamplingValue (int value);
    void setMinLengthValue (int value);
    
public slots:
    int update();
    void onCanceled();
    
private:
    itkProcessTensorTractographyPrivate *d;
};

dtkAbstractProcess *createItkProcessTensorTractography();


