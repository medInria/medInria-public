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

class medAbstractImageData;
class medIntParameter;

class medAbstractMedianFilterProcessPrivate;
class MEDCORE_EXPORT medAbstractMedianFilterProcess: public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT

public:
    medAbstractMedianFilterProcess(QObject *parent = NULL);
    virtual ~medAbstractMedianFilterProcess();

    medIntParameter *radius();

protected:
    virtual QString outputNameAddon() const {return "median filter";}

private:
    const QScopedPointer<medAbstractMedianFilterProcessPrivate> d;
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractMedianFilterProcess, MEDCORE_EXPORT)
