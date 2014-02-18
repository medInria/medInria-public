/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDoubleParameter.h>

#include <QDoubleSpinBox>
#include <QSlider>

class medDoubleParameterPrivate
{
public:
    double min;
    double max;
    double step;

    QDoubleSpinBox *spinBox;
    QSlider *slider;

    ~medDoubleParameterPrivate()
    {
        delete spinBox;
    }
};

medDoubleParameter::medDoubleParameter(QString name, QObject *parent):
    medAbstractDoubleParameter(name,parent),
    d(new medDoubleParameterPrivate)
{
    d->min = 0;
    d->max = 100;
    m_value = 0;
    d->spinBox = NULL;
    d->slider = NULL;
    d->step = 0.1;
}

medDoubleParameter::~medDoubleParameter()
{
    delete d;
}

void medDoubleParameter::updateInternWigets()
{
    if(d->spinBox)
        d->spinBox->setValue(m_value);
    if(d->slider)
        d->slider->setValue(convertToInt(m_value));
}

void medDoubleParameter::setRange(double min, double max)
{
    d->min = min;
    d->max = max;

    if(d->spinBox)
        d->spinBox->setRange(min, max);
    if(d->slider)
        d->slider->setRange(0,convertToInt(max));
}

void medDoubleParameter::setSingleStep(double step)
{
    d->step = step;

    if(d->spinBox)
        d->spinBox->setSingleStep(step);
    if(d->slider)
        d->slider->setSingleStep(convertToInt(step));
}


QDoubleSpinBox* medDoubleParameter::getSpinBox()
{
    if(!d->spinBox)
    {
        d->spinBox = new QDoubleSpinBox;
        d->spinBox->setRange(d->min, d->max);
        d->spinBox->setValue(m_value);
        d->spinBox->setSingleStep(d->step);

        this->addToInternWidgets(d->spinBox);
        connect(d->spinBox, SIGNAL(destroyed()), this, SLOT(removeInternSpinBox()));
        connect(d->spinBox, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
    }
    return d->spinBox;
}

QSlider* medDoubleParameter::getSlider()
{
    if(!d->slider)
    {
        d->slider = new QSlider;
        d->slider->setRange(0, convertToInt(d->max));
        d->slider->setSingleStep(d->step);
        d->slider->setValue(convertToInt(m_value));

        this->addToInternWidgets(d->slider);
        connect(d->slider, SIGNAL(destroyed()), this, SLOT(removeInternSlider()));
        connect(d->slider, SIGNAL(valueChanged(int)), this, SLOT(setIntValue(int)));
    }
    return d->slider;
}

QWidget* medDoubleParameter::getWidget()
{
    return this->getSpinBox();
}

void medDoubleParameter::removeInternSpinBox()
{
    this->removeFromInternWidgets(d->spinBox);
    d->spinBox = NULL;
}

void medDoubleParameter::removeInternSlider()
{
    this->removeFromInternWidgets(d->slider);
    d->slider = NULL;
}

int medDoubleParameter::convertToInt(double value)
{
    return (value-d->min)/d->step;
}

void medDoubleParameter::setIntValue(int value)
{
    double dValue = (double)value*d->step + d->min;
    setValue(dValue);

}
