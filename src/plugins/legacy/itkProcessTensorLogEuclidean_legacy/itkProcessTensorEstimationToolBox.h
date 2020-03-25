#pragma once

#include <medDiffusionAbstractToolBox.h>

class itkProcessTensorEstimationToolBoxPrivate;

class itkProcessTensorEstimationToolBox : public medDiffusionAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("DTI estimation",
                          "DTI estimation plugin interface",
                          << "diffusion-estimation")
public:
    itkProcessTensorEstimationToolBox(QWidget *parentToolBox = 0);
    ~itkProcessTensorEstimationToolBox(void);
    
public:
    static bool registered(void);
    
    dtkPlugin *plugin( void )const;
    
    QString processName();
    void setProcessParameters(medAbstractDiffusionProcess *process);
    
public slots:
    void toggleAutomaticBrainExtraction (bool);
    void changeBSTSliderValue (int);
    void toggleTensorSmoothing(bool);
    void toggleLowSmoothing(bool);
    void toggleMedSmoothing(bool);
    void toggleHighSmoothing(bool);
    
private:
    itkProcessTensorEstimationToolBoxPrivate * d;
};
