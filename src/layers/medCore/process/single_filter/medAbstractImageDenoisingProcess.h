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

class MEDCORE_EXPORT medAbstractImageDenoisingProcess: public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT
public:
    medAbstractImageDenoisingProcess(QObject *parent): medAbstractSingleFilterOperationProcess(parent) {}

protected:
    virtual QString outputNameAddon() const {return "denoised";}
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractImageDenoisingProcess, MEDCORE_EXPORT)
