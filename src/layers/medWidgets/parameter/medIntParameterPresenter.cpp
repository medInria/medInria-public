/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medIntParameterPresenter.h>

#include <QWidget>
#include <QSpinBox>
#include <QProgressBar>

#include <medIntParameter.h>

class medIntParameterPresenterPrivate
{
public:
    medIntParameter* parameter;

    int singleStep;
};

medIntParameterPresenter::medIntParameterPresenter(medIntParameter* parameter)
    :medAbstractParameterPresenter(parameter), d(new medIntParameterPresenterPrivate)
{
    d->parameter = parameter;

    d->singleStep = 1;
}

medIntParameterPresenter::medIntParameterPresenter(QString const& newParameterId)
    : medIntParameterPresenter(new medIntParameter(newParameterId))
{

}

medIntParameterPresenter::~medIntParameterPresenter()
{

}

medIntParameter* medIntParameterPresenter::parameter() const
{
    return d->parameter;
}

void medIntParameterPresenter::setSingleStep(int step)
{
    d->singleStep = step;
    emit singleStepChanged(d->singleStep);
}

int medIntParameterPresenter::singleStep() const
{
    return d->singleStep;
}

QWidget* medIntParameterPresenter::buildWidget()
{
    return this->buildSpinBox();
}

QSpinBox* medIntParameterPresenter::buildSpinBox()
{
    QSpinBox *spinBox = new QSpinBox;

    spinBox->setToolTip(d->parameter->description());
    spinBox->setRange(d->parameter->minimum(), d->parameter->maximum());
    spinBox->setValue(d->parameter->value());
    spinBox->setSingleStep(d->singleStep);

    this->_connectWidget(spinBox);
    connect(spinBox, SIGNAL(valueChanged(int)),
            d->parameter, SLOT(setValue(int)));
    connect(d->parameter, &medIntParameter::valueChanged,
            spinBox, &QSpinBox::setValue);
    connect(d->parameter, &medIntParameter::rangeChanged,
            spinBox, &QSpinBox::setRange);
    connect(this, &medIntParameterPresenter::singleStepChanged,
            spinBox, &QSpinBox::setSingleStep);

    return spinBox;
}

QProgressBar* medIntParameterPresenter::buildProgressBar()
{
    QProgressBar *progressBar = new QProgressBar;

    progressBar->setToolTip(d->parameter->description());
    progressBar->setRange(d->parameter->minimum(), d->parameter->maximum());
    progressBar->setValue(d->parameter->value());

    this->_connectWidget(progressBar);
    connect(d->parameter, &medIntParameter::valueChanged,
            progressBar, &QProgressBar::setValue);
    connect(d->parameter, &medIntParameter::rangeChanged,
            progressBar, &QProgressBar::setRange);

    return progressBar;
}
