/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medBinaryOperatorBase.h"
#include "medBinaryOperationPluginExport.h"

class itkNotOperatorPrivate;

class MEDBINARYOPERATIONPLUGIN_EXPORT itkNotOperator : public medBinaryOperatorBase
{
    Q_OBJECT
    
public:
    
    virtual QString description() const;
    
    static bool registered();

    int update();
    template <class ImageType> int run(medAbstractData* inputData);

public slots:
    
    //! Input data to the plugin is set through here
    void setInput(medAbstractData *data, int channel=0);
};

dtkAbstractProcess *createitkNotOperator();


