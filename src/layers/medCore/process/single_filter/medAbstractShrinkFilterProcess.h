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

class medAbstractShrinkFilterProcessPrivate;
class MEDCORE_EXPORT medAbstractShrinkFilterProcess : public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT

public:
    medAbstractShrinkFilterProcess(QObject *parent = NULL);
    virtual ~medAbstractShrinkFilterProcess();

    virtual medIntParameter* shrinkFactor1();
    virtual medIntParameter* shrinkFactor2();
    virtual medIntParameter* shrinkFactor3();

protected:
    virtual QString outputNameAddon() const {return "shrink filter";}

private:
    const QScopedPointer<medAbstractShrinkFilterProcessPrivate> d;
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractShrinkFilterProcess, MEDCORE_EXPORT)
