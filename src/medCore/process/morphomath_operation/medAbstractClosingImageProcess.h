/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMorphomathOperationProcess.h>

#include <dtkCore>

#include <medCoreExport.h>

class medAbstractClosingImageProcess: public medAbstractMorphomathOperationProcess
{
    Q_OBJECT
public:
    medAbstractClosingImageProcess(QObject *parent): medAbstractMorphomathOperationProcess(parent) {}
};

DTK_DECLARE_OBJECT        (medAbstractClosingImageProcess*)
DTK_DECLARE_PLUGIN        (medAbstractClosingImageProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractClosingImageProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractClosingImageProcess, MEDCORE_EXPORT)
