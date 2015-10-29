/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStringParameterPresenter.h>

#include <QWidget>
#include <QLineEdit>

#include <medStringParameter.h>

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

    lineEdit->setText(d->parameter->value());
    connect(d->parameter, &medStringParameter::valueChanged,
            lineEdit, &QLineEdit::setText);
    connect(lineEdit, &QLineEdit::textEdited,
            d->parameter, &medStringParameter::setValue);

    lineEdit->setToolTip(d->parameter->description());
    lineEdit->setText(d->parameter->caption());
    this->_connectWidget(lineEdit);

    return lineEdit;
}
