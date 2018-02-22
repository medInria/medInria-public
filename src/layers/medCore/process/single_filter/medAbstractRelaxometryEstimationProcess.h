/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSingleFilterOperationProcess.h>

#include <dtkCore>

#include <medCoreExport.h>

class MEDCORE_EXPORT medAbstractRelaxometryEstimationProcess: public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT
public:
    medAbstractRelaxometryEstimationProcess(QObject *parent): medAbstractSingleFilterOperationProcess(parent) {}

protected:
    virtual QString outputNameAddon() const {return "relaxometry map";}
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractRelaxometryEstimationProcess, MEDCORE_EXPORT)
