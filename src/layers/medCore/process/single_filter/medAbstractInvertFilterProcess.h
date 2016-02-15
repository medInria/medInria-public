/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSingleFilterOperationProcess.h>

#include <dtkCore>

#include <medCoreExport.h>

class MEDCORE_EXPORT medAbstractInvertFilterProcess: public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT
public:
    medAbstractInvertFilterProcess(QObject *parent): medAbstractSingleFilterOperationProcess(parent) {}
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractInvertFilterProcess, MEDCORE_EXPORT)
