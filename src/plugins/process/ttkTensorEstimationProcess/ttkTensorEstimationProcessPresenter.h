#pragma once

#include <medAbstractDiffusionModelEstimationProcessPresenter.h>
#include <medProcessPresenterFactory.h>

#include "ttkTensorEstimationProcess.h"
#include "ttkTensorEstimationProcessPluginExport.h"

class medBoolParameterPresenter;
class TTKTENSORESTIMATIONPROCESSPLUGIN_EXPORT ttkTensorEstimationProcessPresenter: public medAbstractDiffusionModelEstimationProcessPresenter
{
    Q_OBJECT

public:
    ttkTensorEstimationProcessPresenter(medAbstractDiffusionModelEstimationProcess *parent);
    virtual medAbstractDiffusionModelEstimationProcess* process() const;

    virtual QWidget *buildToolBoxWidget();

private:
    ttkTensorEstimationProcess *m_process;

    medBoolParameterPresenter *m_noSmoothingPresenter;
    medBoolParameterPresenter *m_lowSmoothingPresenter;
    medBoolParameterPresenter *m_mediumSmoothingPresenter;
    medBoolParameterPresenter *m_highSmoothingPresenter;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractDiffusionModelEstimationProcess, ttkTensorEstimationProcess)
