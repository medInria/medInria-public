/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSingleFilterOperationDoubleProcess.h>

#include <dtkCore>

#include <medCoreExport.h>

class MEDCORE_EXPORT medAbstractDivideFilterProcess: public medAbstractSingleFilterOperationDoubleProcess
{
    Q_OBJECT
public:
    medAbstractDivideFilterProcess(QObject *parent);

protected:
    virtual QString outputNameAddon() const {return "divide constant";}
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractDivideFilterProcess, MEDCORE_EXPORT)
