/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDoubleParameterPresenter.h>

#include <QWidget>
#include <QDoubleSpinBox>

#include <medDoubleParameter.h>

class medDoubleSpinBox : public QDoubleSpinBox
{
public:
    virtual QString textFromValue ( double value ) const
    {
        return QString::number(value, 'g', 7);
    }
};

class medDoubleParameterPresenterPrivate
{
public:
    medDoubleParameter* parameter;

    double singleStep;
    int decimals;
};

medDoubleParameterPresenter::medDoubleParameterPresenter(medDoubleParameter* parameter)
    :medAbstractParameterPresenter(parameter), d(new medDoubleParameterPresenterPrivate)
{
    d->parameter = parameter;


    d->singleStep = 0.1;
    d->decimals = 2;
}

medDoubleParameterPresenter::medDoubleParameterPresenter(QString const& newParameterId)
    : medDoubleParameterPresenter(new medDoubleParameter(newParameterId))
{

}

medDoubleParameterPresenter::~medDoubleParameterPresenter()
{

}

medDoubleParameter* medDoubleParameterPresenter::parameter() const
{
    return d->parameter;
}

void medDoubleParameterPresenter::setSingleStep(double step)
{
    d->singleStep = step;
    emit _singleStepChanged(d->singleStep);
}

double medDoubleParameterPresenter::singleStep() const
{
    return d->singleStep;
}

void medDoubleParameterPresenter::setDecimals(int precision)
{
    d->decimals = precision;
    emit _decimalsChanged(d->decimals);
}

int medDoubleParameterPresenter::decimals() const
{
    return d->decimals;
}

QWidget* medDoubleParameterPresenter::buildWidget()
{
    return this->buildSpinBox();
}

QDoubleSpinBox* medDoubleParameterPresenter::buildSpinBox()
{
    QDoubleSpinBox *spinBox = new medDoubleSpinBox;
    spinBox->setValue(d->parameter->value());
    connect(spinBox, SIGNAL(valueChanged(double)),
           d->parameter, SLOT(setValue(double)));
    connect(d->parameter, &medDoubleParameter::valueChanged,
            spinBox, &QDoubleSpinBox::setValue);

    spinBox->setToolTip(d->parameter->description());
    this->_connectWidget(spinBox);

    spinBox->setRange(d->parameter->minimum(), d->parameter->maximum());
    spinBox->setDecimals(d->decimals);
    spinBox->setSingleStep(d->singleStep);
    connect(d->parameter, &medDoubleParameter::rangeChanged,
            spinBox, &QDoubleSpinBox::setRange);
    connect(this, &medDoubleParameterPresenter::_decimalsChanged,
            spinBox, &QDoubleSpinBox::setDecimals);
    connect(this, &medDoubleParameterPresenter::_singleStepChanged,
            spinBox, &QDoubleSpinBox::setSingleStep);

    return spinBox;
}

