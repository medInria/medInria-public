#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medUtilitiesExport.h"

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractData.h>

class MEDUTILITIES_EXPORT statsROI
{
public:
    dtkSmartPointer <medAbstractData> input0; //data
    dtkSmartPointer <medAbstractData> input1; //mask
    std::vector<double> computedOutput;
    enum statsParameter {MEANVARIANCE, VOLUMEML, MINMAX};
    statsParameter chooseFct;
    double outsideValue;

    statsROI();
    virtual ~statsROI();
    
    //! Input data to the plugin is set through here
    void setInput(medAbstractData *data, int channel);
       
    //! Parameters are set through here, channel allows to handle multiple parameters
    void setParameter(statsParameter fct);

    //! Parameter used only with VOLUMEML statsParameter
    void setParameter(double outsideValue);

    //! Method to actually start the filter
    int update();
    
    //! The output will be available through here
    std::vector<double> output();
};
