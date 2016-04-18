/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medBinaryOperatorBase.h"
#include "medBinaryOperationPluginExport.h"


class itkXorOperatorPrivate;

class MEDBINARYOPERATIONPLUGIN_EXPORT itkXorOperator : public medBinaryOperatorBase
{
    Q_OBJECT
    
public:

    virtual QString description() const;    
    static bool registered();
};

dtkAbstractProcess *createitkXorOperator();


