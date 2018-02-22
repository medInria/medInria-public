/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameterPresenter.h>

#include <QWidget>

#include <dtkLog>

#include <medAbstractParameter.h>
#include <medBoolParameterPresenter.h>
#include <medIntParameterPresenter.h>
#include <medDoubleParameterPresenter.h>
#include <medStringParameterPresenter.h>

class medAbstractParameterPresenterPrivate
{
public:
    bool visibility;
    bool isEnable;
    medAbstractParameter *parameter;
};

medAbstractParameterPresenter::medAbstractParameterPresenter(medAbstractParameter *parent)
    : QObject(parent), d(new medAbstractParameterPresenterPrivate)
{
    if(parent == NULL)
    {
        dtkWarn() << "Constructing presenter parented to a null parameter"
                  << this;
    }

    d->parameter = parent;
    d->visibility = true;
    d->isEnable = true;
}

medAbstractParameterPresenter::~medAbstractParameterPresenter()
{

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

void medAbstractParameterPresenter::setEnable(bool enabled)
{
    d->isEnable = enabled;
    emit _isEnabledChanged(d->isEnable);
}

bool medAbstractParameterPresenter::isEnable() const
{
    return d->isEnable;
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
    medAbstractParameterPresenter *presenter = NULL;
    switch(parameter->type())
    {
    case medParameterType::MED_PARAMETER_BOOL :
        presenter = new medBoolParameterPresenter(qobject_cast<medBoolParameter*>(parameter));
    case medParameterType::MED_PARAMETER_INT :
        presenter = new medIntParameterPresenter(qobject_cast<medIntParameter*>(parameter));
    case medParameterType::MED_PARAMETER_DOUBLE :
        presenter = new medDoubleParameterPresenter(qobject_cast<medDoubleParameter*>(parameter));
    case medParameterType::MED_PARAMETER_STRING :
        presenter = new medStringParameterPresenter(qobject_cast<medStringParameter*>(parameter));
    default:
        dtkDebug() << "Unable to build presenter for parameter of type" << parameter->type();
    }
    return presenter;
}


