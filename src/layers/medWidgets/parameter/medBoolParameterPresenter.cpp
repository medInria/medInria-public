/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBoolParameterPresenter.h>

#include <QWidget>
#include <QCheckBox>

#include <medBoolParameter.h>

class medBoolParameterPresenterPrivate
{
public:
    medBoolParameter* parameter;
};

medBoolParameterPresenter::medBoolParameterPresenter(medBoolParameter* parameter)
    :medAbstractParameterPresenter(parameter), d(new medBoolParameterPresenterPrivate)
{
    d->parameter = parameter;
}

medBoolParameterPresenter::medBoolParameterPresenter(QString const& newParameterId)
    : medBoolParameterPresenter(new medBoolParameter(newParameterId))
{

}

medBoolParameterPresenter::~medBoolParameterPresenter()
{

}

medBoolParameter* medBoolParameterPresenter::parameter() const
{
    return d->parameter;
}

QWidget* medBoolParameterPresenter::buildWidget()
{
    return this->buildCheckBox();
}

QCheckBox* medBoolParameterPresenter::buildCheckBox()
{
    QCheckBox *checkBox = new QCheckBox;
    checkBox->setChecked(d->parameter->value());
    this->_connectButton(checkBox);

    checkBox->setToolTip(d->parameter->description());
    this->_connectWidget(checkBox);

    checkBox->setText(d->parameter->caption());
    connect(d->parameter, &medBoolParameter::captionChanged,
            checkBox, &QCheckBox::setText);

    return checkBox;
}

void medBoolParameterPresenter::_connectButton(QAbstractButton *button)
{
    connect(button, &QAbstractButton::toggled,
            d->parameter, &medBoolParameter::setValue);
    connect(d->parameter, &medBoolParameter::valueChanged,
            button, &QAbstractButton::toggle);
}
