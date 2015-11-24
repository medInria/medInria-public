/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractProcess.h>
#include "medBinaryOperationPluginExport.h"
#include <medAbstractData.h>

class itkFiltersBinaryOpenProcessPrivate;

class MEDBINARYOPERATIONPLUGIN_EXPORT itkFiltersBinaryOpenProcess : public medAbstractProcess
{
    Q_OBJECT
    
public:
    itkFiltersBinaryOpenProcess();
    virtual ~itkFiltersBinaryOpenProcess();
    
    virtual QString description() const;
    
    static bool registered();

    template <class ImageType> int run();

public slots:
    
    //! Input data to the plugin is set through here
    void setInput(medAbstractData *data, int channel);
    
    //! Parameters are set through here, channel allows to handle multiple parameters
    void setParameter(double  data, int channel);
    
    //! Method to actually start the filter
    int update();
    
    //! The output will be available through here
    medAbstractData *output();
    
private:
    itkFiltersBinaryOpenProcessPrivate *d;
};

dtkAbstractProcess *createitkFiltersBinaryOpenProcess();


