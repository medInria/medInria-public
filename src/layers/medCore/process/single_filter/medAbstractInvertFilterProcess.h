#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSingleFilterOperationProcess.h>
#include <medCoreExport.h>

#include <dtkCore>

class MEDCORE_EXPORT medAbstractInvertFilterProcess: public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT
public:
    medAbstractInvertFilterProcess(QObject *parent): medAbstractSingleFilterOperationProcess(parent) {}

protected:
    virtual QString outputNameAddon() const {return "invert filter";}
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractInvertFilterProcess, MEDCORE_EXPORT)
