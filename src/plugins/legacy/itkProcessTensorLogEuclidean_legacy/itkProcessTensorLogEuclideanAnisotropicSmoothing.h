#pragma once

#include <dtkCore/dtkGlobal.h>
#include <medAbstractData.h>
#include <medAbstractDiffusionProcess.h>

#include "itkProcessTensorLogEuclideanPluginExport.h"

class itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate;

class ITKPROCESSTENSORLOGEUCLIDEANPLUGIN_EXPORT itkProcessTensorLogEuclideanAnisotropicSmoothing : public medAbstractDiffusionProcess
{
    Q_OBJECT

public:
             itkProcessTensorLogEuclideanAnisotropicSmoothing(void);
    virtual ~itkProcessTensorLogEuclideanAnisotropicSmoothing(void);

    virtual QString identifier(void) const;
    virtual QString description(void) const;

    static bool registered(void);
    
public slots:
    virtual void setInputImage (medAbstractData* data);
    virtual int  update (void);

    virtual medAbstractData* output (void);
    virtual void onCanceled (void);

public slots:
    void onPropertySet (const QString &key, const QString &value);

    void onSmoothingLevelPropertySet (const QString &value);

private:
    itkProcessTensorLogEuclideanAnisotropicSmoothingPrivate *d;
    
};

dtkAbstractProcess *createItkProcessTensorLogEuclideanAnisotropicSmoothing(void);
