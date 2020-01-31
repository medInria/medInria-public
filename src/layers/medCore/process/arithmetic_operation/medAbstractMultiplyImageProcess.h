#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractArithmeticOperationProcess.h>
#include <medCoreExport.h>

#include <dtkCore>

class MEDCORE_EXPORT medAbstractMultiplyImageProcess: public medAbstractArithmeticOperationProcess
{
    Q_OBJECT
public:
    medAbstractMultiplyImageProcess(QObject *parent): medAbstractArithmeticOperationProcess(parent) {}

protected:
    virtual QString outputNameAddon() const {return "multiply image";}
};

DTK_DECLARE_OBJECT        (medAbstractMultiplyImageProcess*)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractMultiplyImageProcess, MEDCORE_EXPORT)
