/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medBinaryOperationPluginExport.h"

#include <dtkCoreSupport/dtkAbstractProcess.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractData.h>

class medBinaryOperatorBasePrivate;

class MEDBINARYOPERATIONPLUGIN_EXPORT medBinaryOperatorBase : public dtkAbstractProcess
{
    Q_OBJECT
    
public:
    medBinaryOperatorBase();
    virtual ~medBinaryOperatorBase();

    template <class ImageType> int run();
    template <class ImageType, class ImageType2> int runProcess();

public slots:
    
    //! Input data to the plugin is set through here
    void setInput(medAbstractData *data, int channel);

    //! Method to actually start the filter
    virtual int update();
    
    //! The output will be available through here
    medAbstractData *output();

protected:
    dtkSmartPointer <medAbstractData> m_inputA;
    dtkSmartPointer <medAbstractData> m_inputB;
    dtkSmartPointer <medAbstractData> m_output;
};


