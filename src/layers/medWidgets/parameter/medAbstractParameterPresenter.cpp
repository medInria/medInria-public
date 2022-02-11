/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameterPresenter.h>

#include <QWidget>
#include <QLabel>

#include <dtkLog>

#include <medAbstractParameter.h>
#include <medBoolParameterPresenter.h>
#include <medIntParameterPresenter.h>
#include <medDoubleParameterPresenter.h>
#include <medStringParameterPresenter.h>
#include <medStringListParameterPresenter.h>
#include <medTimeLineParameterPresenter.h>
#include <medTriggerParameterPresenter.h>
#include <medVariantListParameterPresenter.h>

class medAbstractParameterPresenterPrivate
{
public:
    bool visibility;
    bool isEnabled;
    medAbstractParameter *parameter;
};

medAbstractParameterPresenter::medAbstractParameterPresenter(medAbstractParameter *parent)
    : QObject(parent), d(new medAbstractParameterPresenterPrivate)
{
    if(parent == nullptr)
    {
        qWarning() << "Constructing presenter parented to a null parameter"
                  << this;
    }

    d->parameter = parent;
    d->visibility = true;
    d->isEnabled = true;
}

medAbstractParameterPresenter::~medAbstractParameterPresenter()
{

}

QWidget * medAbstractParameterPresenter::buildLabel()
{
    QLabel *poLabelRes = new QLabel(d->parameter->caption());
    _connectWidget(poLabelRes);
    return poLabelRes;
}

void medAbstractParameterPresenter::setVisible(bool visibility)
{
    d->visibility = visibility;
    emit _visibilityChanged(d->visibility);
}

bool medAbstractParameterPresenter::isVisible() const
{
    return d->visibility;
}

void medAbstractParameterPresenter::setEnabled(bool enabled)
{
    d->isEnabled = enabled;
    emit _isEnabledChanged(d->isEnabled);
}

bool medAbstractParameterPresenter::isEnabled() const
{
    return d->isEnabled;
}

void medAbstractParameterPresenter::_connectWidget(QWidget *widget)
{
    connect(d->parameter, &medAbstractParameter::descriptionChanged,
            widget, &QWidget::setToolTip);
    connect(this, &medAbstractParameterPresenter::_visibilityChanged,
            widget, &QWidget::setVisible);
    connect(this, &medAbstractParameterPresenter::_isEnabledChanged,
            widget, &QWidget::setEnabled);
}

medAbstractParameterPresenter* medAbstractParameterPresenter::buildFromParameter(medAbstractParameter *parameter)
{
    medAbstractParameterPresenter *presenter = nullptr;
    switch(parameter->type())
    {
    case medParameterType::MED_PARAMETER_TRIGGER:
        presenter = new medTriggerParameterPresenter(qobject_cast<medTriggerParameter*>(parameter)); break;
    case medParameterType::MED_PARAMETER_BOOL :
        presenter = new medBoolParameterPresenter(qobject_cast<medBoolParameter*>(parameter)); break;
    case medParameterType::MED_PARAMETER_INT :
        presenter = new medIntParameterPresenter(qobject_cast<medIntParameter*>(parameter)); break;
    case medParameterType::MED_PARAMETER_DOUBLE :
        presenter = new medDoubleParameterPresenter(qobject_cast<medDoubleParameter*>(parameter)); break;
    case medParameterType::MED_PARAMETER_STRING :
        presenter = new medStringParameterPresenter(qobject_cast<medStringParameter*>(parameter)); break;
    case medParameterType::MED_PARAMETER_STRING_LIST:
        presenter = new medStringListParameterPresenter(qobject_cast<medStringListParameter*>(parameter)); break;
    case medParameterType::MED_PARAMETER_TIMELINE:
        presenter = new medTimeLineParameterPresenter(qobject_cast<medTimeLineParameter*>(parameter)); break;
    case medParameterType::MED_PARAMETER_VARIANT_LIST:
        presenter = new medVariantListParameterPresenter(qobject_cast<medVariantListParameter*>(parameter)); break;
    default:
        dtkDebug() << "Unable to build presenter for parameter of type" << parameter->type();
    }
    return presenter;
}


