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
#include <QProgressBar>

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
    connect(d->parameter, &medDoubleParameter::valueChanged,
            this, &medDoubleParameterPresenter::_emitValueChangedInPercent);

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
    emit singleStepChanged(d->singleStep);
}

double medDoubleParameterPresenter::singleStep() const
{
    return d->singleStep;
}

void medDoubleParameterPresenter::setDecimals(int precision)
{
    d->decimals = precision;
    emit decimalsChanged(d->decimals);
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
    connect(this, &medDoubleParameterPresenter::decimalsChanged,
            spinBox, &QDoubleSpinBox::setDecimals);
    connect(this, &medDoubleParameterPresenter::singleStepChanged,
            spinBox, &QDoubleSpinBox::setSingleStep);

    return spinBox;
}

QProgressBar* medDoubleParameterPresenter::buildProgressBar()
{
    QProgressBar *progressBar = new QProgressBar;
    progressBar->setValue(_percentFromValue(d->parameter->value()));
    connect(this, &medDoubleParameterPresenter::valueChanged,
            progressBar, &QProgressBar::setValue);

    progressBar->setToolTip(d->parameter->description());
    this->_connectWidget(progressBar);

    progressBar->setMinimum(0);
    progressBar->setMaximum(100);

    return progressBar;
}

int medDoubleParameterPresenter::_percentFromValue(double value)
{
    int max = static_cast<int>(d->parameter->maximum());
    int min = static_cast<int>(d->parameter->minimum());
    int percent = static_cast<int>(value) * (max - min) / 100 - min;
    return percent;
}


double medDoubleParameterPresenter::_valueFromPercent(int percent)
{
   double max = d->parameter->maximum();
   double min = d->parameter->minimum();
   double value = static_cast<double>(percent) * 100.0 / (max - min) - min;
   return value;
}

void medDoubleParameterPresenter::_setValueFromPercent(int percent)
{
   d->parameter->setValue(_valueFromPercent(percent));
}


void medDoubleParameterPresenter::_emitValueChangedInPercent(double value)
{
    emit valueChanged(_percentFromValue(value));
}

