#pragma once

#include <medAbstractTractographyProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include "ttkTensorTractographyProcess.h"
#include "ttkTensorTractographyProcessPluginExport.h"

class medIntParameterPresenter;
class TTKTENSORTRACTOGRAPHYPROCESSPLUGIN_EXPORT ttkTensorTractographyProcessPresenter: public medAbstractTractographyProcessPresenter
{
    Q_OBJECT

public:
    ttkTensorTractographyProcessPresenter(medAbstractTractographyProcess *parent);
    virtual medAbstractTractographyProcess* process() const;

    virtual QWidget *buildToolBoxWidget();

private:
    ttkTensorTractographyProcess *m_process;

    medIntParameterPresenter *m_faThreshold;
    medIntParameterPresenter *m_faThreshold2;
    medIntParameterPresenter *m_smoothness;
    medIntParameterPresenter *m_minLength;
    medIntParameterPresenter *m_sampling;

    medIntParameterPresenter *m_progressionPresenter;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractTractographyProcess, ttkTensorTractographyProcess)
