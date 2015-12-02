/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractProcess.h>

#include <dtkCore>

#include <medCoreExport.h>

class medAbstractFibersData;
class medAbstractDiffusionModelImageData;

class medAbstractTractographyProcessPrivate;
class MEDCORE_EXPORT medAbstractTractographyProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractTractographyProcess(QObject *parent = NULL);
    virtual ~medAbstractTractographyProcess();

    void setInput(medAbstractDiffusionModelImageData* data);
    medAbstractDiffusionModelImageData* input() const;

    medAbstractFibersData* output() const;

protected:
    void setOutput(medAbstractFibersData* data);

private:
    const QScopedPointer<medAbstractTractographyProcessPrivate> d;
};

DTK_DECLARE_PLUGIN        (medAbstractTractographyProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractTractographyProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractTractographyProcess, MEDCORE_EXPORT)
