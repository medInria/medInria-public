#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractMorphomathOperationProcess.h>
#include <medCoreExport.h>

#include <dtkCore>

class MEDCORE_EXPORT medAbstractClosingImageProcess: public medAbstractMorphomathOperationProcess
{
    Q_OBJECT
public:
    medAbstractClosingImageProcess(QObject *parent): medAbstractMorphomathOperationProcess(parent) {}

protected:
    virtual QString outputNameAddon() const {return "closing";}
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractClosingImageProcess, MEDCORE_EXPORT)
