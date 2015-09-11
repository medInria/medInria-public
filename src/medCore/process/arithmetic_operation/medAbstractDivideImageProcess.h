/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractArithmeticOperationProcess.h>

#include <dtkCore>

#include <medInriaExport.h>

class MEDINRIA_EXPORT medAbstractDivideImageProcess: public medAbstractArithmeticOperationProcess
{
    Q_OBJECT
public:
    medAbstractDivideImageProcess(QObject *parent): medAbstractArithmeticOperationProcess(parent) {}
};

DTK_DECLARE_OBJECT        (medAbstractDivideImageProcess*)
DTK_DECLARE_PLUGIN        (medAbstractDivideImageProcess, MEDINRIA_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractDivideImageProcess, MEDINRIA_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractDivideImageProcess, MEDINRIA_EXPORT)
