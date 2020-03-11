#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcess.h>
#include <medCoreExport.h>
#include <medDiffusionGradientReader.h>

#include <dtkCore>

class medAbstractImageData;
class medBoolParameter;
class medAbstractDiffusionModelImageData;

class medAbstractDiffusionModelEstimationProcessPrivate;
class MEDCORE_EXPORT medAbstractDiffusionModelEstimationProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractDiffusionModelEstimationProcess(QObject *parent = nullptr);
    virtual ~medAbstractDiffusionModelEstimationProcess();

    void setInput(medAbstractImageData* data);
    medAbstractImageData* input() const;

    void setGradients(QString fileName);
    void setBValues(QString fileName);

    medAbstractDiffusionModelImageData* output() const;

    typedef medDiffusionGradientReader::VectorType VectorType;
    typedef medDiffusionGradientReader::GradientsVectorType GradientsVectorType;

    VectorType bvalues() const;
    GradientsVectorType gradients() const;
    medBoolParameter *gradientsInImageCoordinates() const;

protected:
    //! Required as it needs the input to be set, mandatory in run method to access gradients
    void extractGradientsFromInformation();

    void setOutput(medAbstractDiffusionModelImageData* data);
    virtual QString outputNameAddon() const {return "models";}

private:
    const QScopedPointer<medAbstractDiffusionModelEstimationProcessPrivate> d;
};

DTK_DECLARE_PLUGIN        (medAbstractDiffusionModelEstimationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractDiffusionModelEstimationProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractDiffusionModelEstimationProcess, MEDCORE_EXPORT)
