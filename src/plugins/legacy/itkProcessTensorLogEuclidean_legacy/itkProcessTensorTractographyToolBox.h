#pragma once

#include <medDiffusionAbstractToolBox.h>

class itkProcessTensorTractographyToolBoxPrivate;

class itkProcessTensorTractographyToolBox : public medDiffusionAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("DTI tractography",
                          "DTI tractography plugin interface",
                          << "diffusion-tractography")
public:
    itkProcessTensorTractographyToolBox(QWidget *parentToolBox = 0);
    ~itkProcessTensorTractographyToolBox(void);
    
public:
    static bool registered(void);
    
    dtkPlugin *plugin( void )const;
    
    QString processName();
    void setProcessParameters(medAbstractDiffusionProcess *process);
    
public slots:
    void setFA1Value (int value);
    void setFA2Value (int value);
    void setSmoothnessValue (int value);
    void setSamplingValue (int value);
    void setMinLengthValue (int value);

private:
    itkProcessTensorTractographyToolBoxPrivate * d;
};
