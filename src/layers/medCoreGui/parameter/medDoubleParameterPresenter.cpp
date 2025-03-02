/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDoubleParameterPresenter.h>

#include <QWidget>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QProgressBar>

#include <medDoubleParameter.h>

class medDoubleSpinBox : public QDoubleSpinBox //TODO FLO extract to medWidget
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

    int convertToInt4Slider(double value);
    double convertFromInt4Slider(int value);
};

int medDoubleParameterPresenterPrivate::convertToInt4Slider(double value)
{
    return static_cast<int>((value - parameter->minimum()) / singleStep);
}

double medDoubleParameterPresenterPrivate::convertFromInt4Slider(int value)
{
    double currentDoubleValue = static_cast<double>(value);
    return currentDoubleValue * singleStep + parameter->minimum();
}

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
    QWidget *poWidgetRes = nullptr;
    switch (d->parameter->defaultRepresentation())
    {
    case 1:
        poWidgetRes = this->buildSlider(); break;
    case 2:
        poWidgetRes = this->buildProgressBar(); break;
    case 0:
    default:
        poWidgetRes = this->buildSpinBox(); break;
    }
    return poWidgetRes;
}

QDoubleSpinBox* medDoubleParameterPresenter::buildSpinBox()
{
    QDoubleSpinBox *spinBox = new medDoubleSpinBox;

    spinBox->setToolTip(d->parameter->description());
    spinBox->setRange(d->parameter->minimum(), d->parameter->maximum());
    spinBox->setDecimals(d->decimals);
    spinBox->setSingleStep(d->singleStep);
    spinBox->setValue(d->parameter->value());

    this->_connectWidget(spinBox);
    connect(spinBox, SIGNAL(valueChanged(double)),
           d->parameter, SLOT(setValue(double)));
    connect(d->parameter, &medDoubleParameter::valueChanged,
            spinBox, &QDoubleSpinBox::setValue);
    connect(d->parameter, &medDoubleParameter::rangeChanged,
            spinBox, &QDoubleSpinBox::setRange);
    connect(this, &medDoubleParameterPresenter::decimalsChanged,
            spinBox, &QDoubleSpinBox::setDecimals);
    connect(this, &medDoubleParameterPresenter::singleStepChanged,
            spinBox, &QDoubleSpinBox::setSingleStep);

    return spinBox;
}

QSlider * medDoubleParameterPresenter::buildSlider()
{
    QSlider *slider = new QSlider;

    slider->setRange(0, d->convertToInt4Slider(d->parameter->maximum()));
    slider->setSingleStep(d->convertToInt4Slider(d->singleStep));
    slider->setValue(d->convertToInt4Slider(d->parameter->value()));
    slider->setStyleSheet("QSlider::handle:horizontal {width: 15px;}");
    
    this->_connectWidget(slider);

    connect(slider, &QSlider::valueChanged,                        [=](int value) {d->parameter->setValue(d->convertFromInt4Slider(value)); });
    connect(d->parameter, &medDoubleParameter::valueChanged,       [=](double value) {slider->setValue(d->convertToInt4Slider(value)); });
    connect(d->parameter, &medDoubleParameter::rangeChanged,       [=](double min, double max) { slider->setRange(0, d->convertToInt4Slider(d->parameter->maximum())); });
    connect(this, &medDoubleParameterPresenter::singleStepChanged, [=](double singleStep) { slider->setSingleStep(d->convertToInt4Slider(singleStep)); });

    return slider;
}

QProgressBar* medDoubleParameterPresenter::buildProgressBar()
{
    QProgressBar *progressBar = new QProgressBar;
    progressBar->setValue(_percentFromValue(d->parameter->value()));
    connect(this, &medDoubleParameterPresenter::valueChanged, progressBar, &QProgressBar::setValue);

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
