#pragma once

#include <medAbstractDiffusionScalarMapsProcessPresenter.h>
#include <itkTensorScalarMapsProcessPluginExport.h>
#include <itkTensorScalarMapsProcess.h>
#include <medProcessPresenterFactory.h>

class medIntParameterPresenter;
class QSignalMapper;
class ITKTENSORSCALARMAPSPROCESSPLUGIN_EXPORT itkTensorScalarMapsProcessPresenter: public medAbstractDiffusionScalarMapsProcessPresenter
{
    Q_OBJECT

public:
    itkTensorScalarMapsProcessPresenter(medAbstractDiffusionScalarMapsProcess *parent);
    virtual medAbstractDiffusionScalarMapsProcess* process() const;

    virtual QWidget *buildToolBoxWidget();

public slots:
    void requestScalarMap(QString mapRequested);

private:
    itkTensorScalarMapsProcess *m_process;
    medIntParameterPresenter *m_progressionPresenter;
    QSignalMapper *m_mapper;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractDiffusionScalarMapsProcess, itkTensorScalarMapsProcess)
