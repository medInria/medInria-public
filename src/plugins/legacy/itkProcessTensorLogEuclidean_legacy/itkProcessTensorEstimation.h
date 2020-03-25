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
#include <medAbstractData.h>

#include "itkProcessTensorLogEuclideanPluginExport.h"

class itkProcessTensorEstimationPrivate;
    
class ITKPROCESSTENSORLOGEUCLIDEANPLUGIN_EXPORT itkProcessTensorEstimation : public medAbstractDiffusionProcess
{
    Q_OBJECT
    
public:
    itkProcessTensorEstimation();
    virtual ~itkProcessTensorEstimation();
    
    virtual QString identifier() const;
    virtual QString description() const;
    
    static bool registered();
    
    void setInputImage(medAbstractData *input);
    medAbstractData *output();
    
    void setAutomaticBrainMasking(bool value);
    void setTensorSmoothing(bool value);
    void setSmoothingLevel(QString value);
    void setB0Threshold(double value);
    
public slots:
    int update();
    void onCanceled();
    
private:
    itkProcessTensorEstimationPrivate *d;
};

dtkAbstractProcess *createItkProcessTensorEstimation();


