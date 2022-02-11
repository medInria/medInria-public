/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBoolParameterPresenter.h>

#include <QWidget>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>

#include <medBoolParameter.h>

class medBoolParameterPresenterPrivate
{
public:
    medBoolParameter* parameter;
    QIcon icon;
    QSize iconSize;
};

medBoolParameterPresenter::medBoolParameterPresenter(medBoolParameter* parameter)
    :medAbstractParameterPresenter(parameter), d(new medBoolParameterPresenterPrivate)
{
    d->parameter = parameter;
    d->iconSize = QSize(15, 15);
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

void medBoolParameterPresenter::setIcon(QIcon icon)
{
    d->icon = icon;
}

void medBoolParameterPresenter::setIconSize(QSize const & size)
{
    d->iconSize = size;
}

QWidget* medBoolParameterPresenter::buildWidget()
{
    QWidget *poWidgetRes = nullptr;
    switch (d->parameter->defaultRepresentation())
    {
    case 0:
    default:
        poWidgetRes = this->buildCheckBox(); break;
    }
    return poWidgetRes; 
}

QCheckBox* medBoolParameterPresenter::buildCheckBox()
{
    QCheckBox *checkBox = new QCheckBox;
    checkBox->setChecked(d->parameter->value());
    this->_connectButton(checkBox);

    checkBox->setToolTip(d->parameter->description());
    this->_connectWidget(checkBox);

    checkBox->setText(d->parameter->caption());
    connect(d->parameter, &medBoolParameter::captionChanged, checkBox, &QCheckBox::setText);

    return checkBox;
}

QPushButton* medBoolParameterPresenter::buildPushButton()
{
    QPushButton *pushButton = new QPushButton;

    pushButton->setCheckable(true);
    pushButton->setChecked(d->parameter->value());
    pushButton->setText(d->parameter->caption());
    if (!d->icon.isNull())
    {
        pushButton->setIcon(d->icon);
        pushButton->setIconSize(d->iconSize);
        pushButton->setMinimumSize(d->iconSize);
    }

    this->_connectButton(pushButton);
    this->_connectWidget(pushButton);

    return pushButton;
}

QRadioButton* medBoolParameterPresenter::buildRadioButton()
{
    QRadioButton *radioButton = new QRadioButton;

    radioButton->setChecked(d->parameter->value());
    radioButton->setText(d->parameter->caption());
    radioButton->setAutoExclusive(false);

    this->_connectButton(radioButton);
    this->_connectWidget(radioButton);

    return radioButton;
}


void medBoolParameterPresenter::_connectButton(QAbstractButton *button)
{
    connect(button, &QAbstractButton::toggled,
            d->parameter, &medBoolParameter::setValue);
    connect(d->parameter, &medBoolParameter::valueChanged,
            button, &QAbstractButton::setChecked);
}
