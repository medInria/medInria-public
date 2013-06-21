/*=========================================================================
 
 medInria
 
 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.
 
 =========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "medCoreExport.h"

class MEDCORE_EXPORT medAbstractViewInteractor: public dtkAbstractViewInteractor
{
    Q_OBJECT
public:
    medAbstractViewInteractor(){}
    virtual ~medAbstractViewInteractor(){}

    virtual bool isDataTypeHandled(QString dataType) const = 0;
};
