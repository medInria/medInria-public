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
#include <QDateEdit>
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
    QWidget *poWidgetRes = nullptr;
    switch (d->parameter->defaultRepresentation())
    {
    case 1:
        poWidgetRes = this->buildDateEdit(); break;
    case 2:
        poWidgetRes = this->buildLineEditOnFinish(); break;
    case 0:
    default:
        poWidgetRes = this->buildLineEdit(); break;
    }
    return poWidgetRes;
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
    connect(lineEdit, &QLineEdit::editingFinished, d->parameter, &medStringParameter::edit);

    return lineEdit;
}

QLineEdit* medStringParameterPresenter::buildLineEditOnFinish()
{
    QLineEdit *lineEdit = new QLineEdit;

    lineEdit->setToolTip(d->parameter->description());
    lineEdit->setText(d->parameter->value());
    lineEdit->setValidator(d->parameter->getValidator());

    this->_connectWidget(lineEdit);
    connect(d->parameter, &medStringParameter::validatorChanged, lineEdit, &QLineEdit::setValidator);
    connect(d->parameter, &medStringParameter::valueChanged, lineEdit, &QLineEdit::setText);
    auto *pParam = d->parameter;
    connect(lineEdit, &QLineEdit::editingFinished, [=]() {pParam->setValue(lineEdit->text()); });

    return lineEdit;
}

QDateEdit* medStringParameterPresenter::buildDateEdit()
{
    QDateEdit *dateEdit = new QDateEdit;

    dateEdit->setToolTip(d->parameter->description());
    dateEdit->setCalendarPopup(true);
    QDate date = QDate::fromString( d->parameter->value(), "yyyyMMdd");
    if (!date.isValid())
    {
        date = QDate::currentDate();
    }
    dateEdit->setDate(date);

    this->_connectWidget(dateEdit);
    connect(dateEdit, &QDateEdit::dateChanged, [&](QDate date){ 
        emit d->parameter->setValue(date.toString("yyyyMMdd"));
    });

    return dateEdit;
}
