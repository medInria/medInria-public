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

#include <dtkCore>

#include <medCoreExport.h>

class MEDCORE_EXPORT medAbstractDivideImageProcess: public medAbstractArithmeticOperationProcess
{
    Q_OBJECT
public:
    medAbstractDivideImageProcess(QObject *parent): medAbstractArithmeticOperationProcess(parent) {}

protected:
    virtual QString outputNameAddon() const {return "divide image";}
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractDivideImageProcess, MEDCORE_EXPORT)
