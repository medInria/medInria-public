#pragma once

#include <medAbstractTractographyProcessPresenter.h>
#include <itkTensorTractographyProcessPluginExport.h>
#include <itkTensorTractographyProcess.h>
#include <medProcessPresenterFactory.h>

class medIntParameterPresenter;
class ITKTENSORTRACTOGRAPHYPROCESSPLUGIN_EXPORT itkTensorTractographyProcessPresenter: public medAbstractTractographyProcessPresenter
{
    Q_OBJECT

public:
    itkTensorTractographyProcessPresenter(medAbstractTractographyProcess *parent);
    virtual medAbstractTractographyProcess* process() const;

    virtual QWidget *buildToolBoxWidget();

private:
    itkTensorTractographyProcess *m_process;

    medIntParameterPresenter *m_faThreshold;
    medIntParameterPresenter *m_faThreshold2;
    medIntParameterPresenter *m_smoothness;
    medIntParameterPresenter *m_minLength;
    medIntParameterPresenter *m_sampling;

    medIntParameterPresenter *m_progressionPresenter;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractTractographyProcess, itkTensorTractographyProcess)
