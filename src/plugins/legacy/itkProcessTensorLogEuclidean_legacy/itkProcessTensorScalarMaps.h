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

class itkProcessTensorScalarMapsPrivate;
    
class ITKPROCESSTENSORLOGEUCLIDEANPLUGIN_EXPORT itkProcessTensorScalarMaps : public medAbstractDiffusionProcess
{
    Q_OBJECT
    
public:
    itkProcessTensorScalarMaps();
    virtual ~itkProcessTensorScalarMaps();
    
    virtual QString identifier() const;
    virtual QString description() const;
    
    static bool registered();
    
    void setInputImage(medAbstractData *input);
    medAbstractData *output();
    
    void setTensorScalarMapRequested(QString map);
        
public slots:
    int update();
    
private:
    itkProcessTensorScalarMapsPrivate *d;
};

dtkAbstractProcess *createItkProcessTensorScalarMaps();


