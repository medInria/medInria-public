/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSingleFilterOperationDoubleProcess.h>

#include <dtkCore>

#include <medCoreExport.h>

class MEDCORE_EXPORT medAbstractAddFilterProcess: public medAbstractSingleFilterOperationDoubleProcess
{
    Q_OBJECT
public:
    medAbstractAddFilterProcess(QObject *parent): medAbstractSingleFilterOperationDoubleProcess(parent) {}

protected:
    virtual QString outputNameAddon() const {return "add filter";}
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractAddFilterProcess, MEDCORE_EXPORT)
