/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractArithmeticOperationProcess.h>

#include <dtkCore>

#include <medCoreExport.h>

class MEDCORE_EXPORT medAbstractSubtractImageProcess: public medAbstractArithmeticOperationProcess
{
    Q_OBJECT
public:
     medAbstractSubtractImageProcess(QObject *parent): medAbstractArithmeticOperationProcess(parent) {}

protected:
    virtual QString outputNameAddon() const {return "subtract image";}
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractSubtractImageProcess, MEDCORE_EXPORT)
