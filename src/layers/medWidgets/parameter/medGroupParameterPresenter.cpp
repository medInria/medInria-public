/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medGroupParameterPresenter.h>

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <medGroupParameter.h>






class medGroupParameterPresenterPrivate
{
public:
    medGroupParameter *parameter;
};

medGroupParameterPresenter::medGroupParameterPresenter(medGroupParameter *parent)
    :medAbstractParameterPresenter(parameter), d(new medGroupParameterPresenterPrivate)
{
    d->parameter = parent;
}

medGroupParameterPresenter::medGroupParameterPresenter(const QString & newParameterId)
    : medGroupParameterPresenter(new medGroupParameter(newParameterId))
{

}

medGroupParameterPresenter::~medGroupParameterPresenter()
{

}

medGroupParameter * medGroupParameterPresenter::parameter() const
{
    return d->parameter;
}

QWidget * medGroupParameterPresenter::buildWidget()
{
    QWidget *poWidgetRes = nullptr;
    switch (d->parameter->defaultRepresentation())
    {
    case 0:
    default:
        poWidgetRes = this->buildGroupButton(); break;
    }
    return poWidgetRes;
}

QGroupBox * medGroupParameterPresenter::buildGroupButton()
{
    QGroupBox * groupBoxRes = new QGroupBox();

    auto * pVLayout = new QVBoxLayout;
    for (auto * param : d->parameter->value())
    {
        addParam(param, pVLayout);
    }
    connect(d->parameter, &medGroupParameter::parameterAdded, [=](medAbstractParameter * paramAdded) {addParam(paramAdded, pVLayout); });
    groupBoxRes->setLayout(pVLayout);
    groupBoxRes->setTitle(d->parameter->caption);
    groupBoxRes->setToolTip(d->parameter->description());

    return groupBoxRes;
}

void medGroupParameterPresenter::addParam(medAbstractParameter * param, QVBoxLayout * pVLayout)
{
    auto * pHLayout = new QHBoxLayout;
    auto * pParamPresenter = medAbstractParameterPresenter::buildFromParameter(param);
    auto * pWidget = pParamPresenter->buildWidget();
    if (dynamic_cast<QPushButton*>(pWidget) == nullptr)
    {
        auto * pLabel = pParamPresenter->buildLabel();
        pHLayout->addWidget(pLabel);
        //connect(param, &medAbstractParameter::captionChanged, pLabel,)
    }
    pHLayout->addWidget(pWidget);
    pVLayout->addLayout(pHLayout);
    connect(d->parameter, &medGroupParameter::parameterRemoved, [=](medAbstractParameter * paramRemoved) {if (param == paramRemoved) { pVLayout->removeItem(pHLayout); delete pHLayout; }});
}
