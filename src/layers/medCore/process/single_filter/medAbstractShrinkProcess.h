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

class medAbstractImageData;
class medIntParameter;

class medAbstractShrinkProcessPrivate;
class MEDCORE_EXPORT  medAbstractShrinkProcess : public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT

public:
    medAbstractShrinkProcess(QObject *parent = NULL);
    virtual ~medAbstractShrinkProcess();

    virtual medIntParameter* shrinkFactor1();
    virtual medIntParameter* shrinkFactor2();
    virtual medIntParameter* shrinkFactor3();

private:
    const QScopedPointer<medAbstractShrinkProcessPrivate> d;
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractShrinkProcess, MEDCORE_EXPORT)
