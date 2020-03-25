#pragma once

#include <medAbstractDiffusionModelEstimationProcessPresenter.h>
#include <itkTensorEstimationProcessPluginExport.h>
#include <itkTensorEstimationProcess.h>
#include <medProcessPresenterFactory.h>

class medBoolParameterPresenter;
class ITKTENSORESTIMATIONPROCESSPLUGIN_EXPORT itkTensorEstimationProcessPresenter: public medAbstractDiffusionModelEstimationProcessPresenter
{
    Q_OBJECT

public:
    itkTensorEstimationProcessPresenter(medAbstractDiffusionModelEstimationProcess *parent);
    virtual medAbstractDiffusionModelEstimationProcess* process() const;

    virtual QWidget *buildToolBoxWidget();

private:
    itkTensorEstimationProcess *m_process;

    medBoolParameterPresenter *m_noSmoothingPresenter;
    medBoolParameterPresenter *m_lowSmoothingPresenter;
    medBoolParameterPresenter *m_mediumSmoothingPresenter;
    medBoolParameterPresenter *m_highSmoothingPresenter;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractDiffusionModelEstimationProcess, itkTensorEstimationProcess)
