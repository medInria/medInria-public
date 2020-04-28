#pragma once

#include <medAbstractDiffusionScalarMapsProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include "ttkTensorScalarMapsProcess.h"
#include "ttkTensorScalarMapsProcessPluginExport.h"

class medIntParameterPresenter;
class QSignalMapper;
class TTKTENSORSCALARMAPSPROCESSPLUGIN_EXPORT ttkTensorScalarMapsProcessPresenter: public medAbstractDiffusionScalarMapsProcessPresenter
{
    Q_OBJECT

public:
    ttkTensorScalarMapsProcessPresenter(medAbstractDiffusionScalarMapsProcess *parent);
    virtual medAbstractDiffusionScalarMapsProcess* process() const;

    virtual QWidget *buildToolBoxWidget();

public slots:
    void requestScalarMap(QString mapRequested);

private:
    ttkTensorScalarMapsProcess *m_process;
    medIntParameterPresenter *m_progressionPresenter;
    QSignalMapper *m_mapper;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractDiffusionScalarMapsProcess, ttkTensorScalarMapsProcess)
