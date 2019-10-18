/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <iterativeClosestPointPluginExport.h>

#include <medAbstractData.h>
#include <medAbstractProcessLegacy.h>

class iterativeClosestPointProcessPrivate;

class ITERATIVECLOSESTPOINTPLUGIN_EXPORT iterativeClosestPointProcess : public medAbstractProcessLegacy
{
    Q_OBJECT
    
public:
    iterativeClosestPointProcess();
    virtual ~iterativeClosestPointProcess();
    
    virtual QString description() const;
    
    static bool registered();
    
public slots:
    
    //! Input data to the plugin is set through here
    void setInput(medAbstractData *data, int channel);

    //! Parameters are set through here, channel allows to handle multiple parameters
    void setParameter(double data, int channel);
    void setParameter(int data, int channel);

    //! Method to actually start the filter
    int update();
    
    //! The output will be available through here
    medAbstractData *output();
    
    
private:
    iterativeClosestPointProcessPrivate *d;
};

dtkAbstractProcess *createiterativeClosestPointProcess();
