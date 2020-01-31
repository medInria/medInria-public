/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStringParameter.h>
#include <medStringParameterPresenter.h>

#include <QWidget>
#include <QLineEdit>
#include <QValidator>

class medStringParameterPresenterPrivate
{
public:
    medStringParameter* parameter;
};

medStringParameterPresenter::medStringParameterPresenter(medStringParameter* parameter)
    :medAbstractParameterPresenter(parameter), d(new medStringParameterPresenterPrivate)
{
    d->parameter = parameter;
}

medStringParameterPresenter::medStringParameterPresenter(QString const& newParameterId)
    : medStringParameterPresenter(new medStringParameter(newParameterId))
{

}

medStringParameterPresenter::~medStringParameterPresenter()
{

}

medStringParameter* medStringParameterPresenter::parameter() const
{
    return d->parameter;
}

QWidget* medStringParameterPresenter::buildWidget()
{
    return this->buildLineEdit();
}

QLineEdit* medStringParameterPresenter::buildLineEdit()
{
    QLineEdit *lineEdit = new QLineEdit;

    lineEdit->setToolTip(d->parameter->description());
    lineEdit->setText(d->parameter->value());
    lineEdit->setValidator(d->parameter->getValidator());

    this->_connectWidget(lineEdit);
    connect(d->parameter, &medStringParameter::validatorChanged, lineEdit, &QLineEdit::setValidator);
    connect(d->parameter, &medStringParameter::valueChanged, lineEdit, &QLineEdit::setText);
    connect(lineEdit, &QLineEdit::textEdited, d->parameter, &medStringParameter::setValue);

    return lineEdit;
}
