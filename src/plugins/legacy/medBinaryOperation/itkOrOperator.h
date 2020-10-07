#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medBinaryOperatorBase.h"
#include "medBinaryOperationPluginExport.h"

class itkOrOperatorPrivate;

class MEDBINARYOPERATIONPLUGIN_EXPORT itkOrOperator : public medBinaryOperatorBase
{
    Q_OBJECT
    
public:

    virtual QString description() const;    
    static bool registered();
};

dtkAbstractProcess *createitkOrOperator();
