#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSingleFilterOperationDoubleProcess.h>
#include <medCoreExport.h>

#include <dtkCore>

class MEDCORE_EXPORT medAbstractGaussianFilterProcess: public medAbstractSingleFilterOperationDoubleProcess
{
    Q_OBJECT
public:
    medAbstractGaussianFilterProcess(QObject *parent);

protected:
    virtual QString outputNameAddon() const {return "Gaussian filter";}
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractGaussianFilterProcess, MEDCORE_EXPORT)
