#pragma once

#include <medDiffusionAbstractToolBox.h>

class itkProcessTensorScalarMapsToolBoxPrivate;

class itkProcessTensorScalarMapsToolBox : public medDiffusionScalarMapsAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("DTI Scalar Maps",
                          "DTI scalar maps plugin interface",
                          << "diffusion-scalar-maps")

public:
    itkProcessTensorScalarMapsToolBox(QWidget *parentToolBox = 0);
    ~itkProcessTensorScalarMapsToolBox(void);
    
public:
    static bool registered(void);
    
    dtkPlugin *plugin( void )const;
    
    QString processName();
    void setProcessParameters(medAbstractDiffusionProcess *process);
    
public slots:
    void requestProcess(QString mapRequested);
    
private:
    itkProcessTensorScalarMapsToolBoxPrivate * d;
};
