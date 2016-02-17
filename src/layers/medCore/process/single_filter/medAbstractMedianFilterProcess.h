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

class MEDCORE_EXPORT medAbstractMedianFilterProcess: public medAbstractSingleFilterOperationDoubleProcess
{
    Q_OBJECT
public:
    medAbstractMedianFilterProcess(QObject *parent): medAbstractSingleFilterOperationDoubleProcess(parent) {}

protected:
    virtual QString outputNameAddon() const {return "median filter";}
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractMedianFilterProcess, MEDCORE_EXPORT)
