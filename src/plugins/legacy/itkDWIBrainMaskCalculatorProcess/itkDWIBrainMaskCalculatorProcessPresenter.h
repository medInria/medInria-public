#pragma once

#include <medAbstractDWIMaskingProcessPresenter.h>
#include <itkDWIBrainMaskCalculatorProcess.h>
#include <medProcessPresenterFactory.h>

#include <itkDWIBrainMaskCalculatorProcessPluginExport.h>

class medIntParameterPresenter;
class ITKDWIBRAINMASKCALCULATORPROCESSPLUGIN_EXPORT itkDWIBrainMaskCalculatorProcessPresenter: public medAbstractDWIMaskingProcessPresenter
{
    Q_OBJECT

public:
    itkDWIBrainMaskCalculatorProcessPresenter(medAbstractDWIMaskingProcess *parent);
    virtual medAbstractDWIMaskingProcess* process() const;

    virtual QWidget *buildToolBoxWidget();

private:
    itkDWIBrainMaskCalculatorProcess *m_process;
    medIntParameterPresenter *m_offsetPresenter;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractDWIMaskingProcess, itkDWIBrainMaskCalculatorProcess)
