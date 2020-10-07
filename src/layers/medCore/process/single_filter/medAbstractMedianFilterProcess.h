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

class medAbstractImageData;
class medIntParameter;
class medAbstractMedianFilterProcessPrivate;

class MEDCORE_EXPORT medAbstractMedianFilterProcess: public medAbstractSingleFilterOperationProcess
{
    Q_OBJECT

public:
    medAbstractMedianFilterProcess(QObject *parent = nullptr);
    virtual ~medAbstractMedianFilterProcess();

    medIntParameter *radius();

protected:
    virtual QString outputNameAddon() const {return "median filter";}

private:
    const QScopedPointer<medAbstractMedianFilterProcessPrivate> d;
};

DTK_DECLARE_PLUGIN_FACTORY(medAbstractMedianFilterProcess, MEDCORE_EXPORT)
