#pragma once

#include <medAbstractDiffusionProcess.h>

#include "itkProcessTensorLogEuclideanPluginExport.h"

class itkProcessTensorLogEuclideanRemoveNonPositivePrivate;

class ITKPROCESSTENSORLOGEUCLIDEANPLUGIN_EXPORT itkProcessTensorLogEuclideanRemoveNonPositive : public medAbstractDiffusionProcess
{
    Q_OBJECT

public:
    itkProcessTensorLogEuclideanRemoveNonPositive(void);
    virtual ~itkProcessTensorLogEuclideanRemoveNonPositive(void);

    virtual QString identifier(void) const;
    virtual QString description(void) const;

    static bool registered(void);
    
public slots:
    virtual void setInputImage (medAbstractData* data);
    virtual int  update (void);

    virtual medAbstractData* output (void);

public slots:
    void onPropertySet (const QString &key, const QString &value);
    void onRemovalTypePropertySet (const QString &value);

private:
    itkProcessTensorLogEuclideanRemoveNonPositivePrivate *d;
    
};

dtkAbstractProcess *createItkProcessTensorLogEuclideanRemoveNonPositive(void);
