#pragma once

#include <medAbstractDiffusionProcess.h>

#include "itkProcessTensorLogEuclideanPluginExport.h"

class itkProcessTensorDTIBrainMaskCalculatorPrivate;

class ITKPROCESSTENSORLOGEUCLIDEANPLUGIN_EXPORT itkProcessTensorDTIBrainMaskCalculator : public medAbstractDiffusionProcess
{
    Q_OBJECT

public:
     itkProcessTensorDTIBrainMaskCalculator();
    ~itkProcessTensorDTIBrainMaskCalculator();

    virtual QString identifier(void) const;
    virtual QString description(void) const;

    static bool registered(void);

public:
    virtual int update(void);

    virtual void *data(void);
    virtual medAbstractData *output (void);

    virtual void setInputImage(medAbstractData *data);

    virtual void setParameter(double value, int channel);

protected:

private:
    itkProcessTensorDTIBrainMaskCalculatorPrivate *d;

};

dtkAbstractProcess *createItkProcessTensorDTIBrainMaskCalculator(void);
