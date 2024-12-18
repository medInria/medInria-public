/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDoubleParameterL.h>

#include <QDoubleSpinBox>
#include <QSlider>
#include <QLabel>

#include <dtkLog>

#include <cmath>
#include <iomanip>
#include <sstream>


class medDoubleSpinBox : public QDoubleSpinBox
{
public:
    virtual QString textFromValue ( double value ) const
    {
        return QString::number(value, 'g', 7);
    }
};

class medDoubleParameterLPrivate
{
public:
    double min;
    double max;
    double step;
    int decimals;
    medDoubleSpinBox *spinBox;
    QSlider *slider;
    QLabel *valueLabel;

    ~medDoubleParameterLPrivate()
    {
        delete spinBox;
    }
};

medDoubleParameterL::medDoubleParameterL(QString name, QObject *parent):
    medAbstractDoubleParameterL(name,parent),
    d(new medDoubleParameterLPrivate)
{
    d->min = 0;
    d->max = 0;
    m_value = 0;
    d->spinBox = nullptr;
    d->slider = nullptr;
    d->valueLabel = nullptr;
    d->step = 0.1;
    d->decimals = -1;
}

medDoubleParameterL::~medDoubleParameterL()
{
    delete d;
}

void medDoubleParameterL::setValue(double value)
{
    if(m_value != value)
    {
        if(d->min == d->max)
        {
            m_value = value;
        }
        else if(value < d->min)
        {
            m_value = d->min;
        }
        else if(value > d->max)
        {
            m_value = d->max;
        }
        else
        {
            m_value = value;
        }

        //  update intern widget
        this->updateInternWigets();

        emit valueChanged(m_value);
    }
}

void medDoubleParameterL::updateInternWigets()
{
    this->blockInternWidgetsSignals(true);

    if(d->spinBox)
    {
        d->spinBox->setValue(m_value);
    }
    if(d->slider)
    {
        d->slider->setValue(convertToInt(m_value));
    }
    if(d->valueLabel)
    {
        d->valueLabel->setText(QString::number(m_value, 'f', 2));
    }

    this->blockInternWidgetsSignals(false);
}

void medDoubleParameterL::setRange(double min, double max)
{
    d->min = min;
    d->max = max;

    if(d->spinBox)
    {
        d->spinBox->setRange(min, max);
    }
    if(d->slider)
    {
        d->slider->setRange(0, convertToInt(max));
    }

    updateInternWigets();
}

void medDoubleParameterL::setSingleStep(double step)
{
    if(step <= 0)
    {
        qWarning() << "Attempt to set a step <= 0 to medDoubleParameterL";
        return;
    }

    d->step = step;

    if(d->spinBox)
    {
        d->spinBox->setSingleStep(step);
    }
    if(d->slider)
    {
        d->slider->setSingleStep(convertToInt(step));
    }
}

void medDoubleParameterL::setDecimals(unsigned int decimals)
{
    d->decimals = decimals;

    if(d->spinBox)
    {
        d->spinBox->setDecimals(decimals);
    }
}

QDoubleSpinBox* medDoubleParameterL::getSpinBox()
{
    if(!d->spinBox)
    {
        d->spinBox = new medDoubleSpinBox;
        d->spinBox->setRange(d->min, d->max);
        d->spinBox->setSingleStep(d->step);
        d->spinBox->setKeyboardTracking(false);

        // if decimals has not been set
        if(d->decimals == -1)
        {
            d->decimals = 0;
            bool nullDecimal = true;
            double multipliedStep = d->step;
            while (nullDecimal)
            {
                d->decimals++;
                multipliedStep *= 10;
                int testNumber = floor(multipliedStep);
                if (testNumber > 0)
                {
                    nullDecimal = false;
                }
            }
        }

        d->spinBox->setDecimals(d->decimals);
        d->spinBox->setValue(m_value);

        this->addToInternWidgets(d->spinBox);
        connect(d->spinBox, SIGNAL(destroyed()), this, SLOT(removeInternSpinBox()));
        connect(d->spinBox, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
    }
    return d->spinBox;
}

QSlider* medDoubleParameterL::getSlider()
{
    if(!d->slider)
    {
        d->slider = new QSlider;
        d->slider->setRange(0, convertToInt(d->max));
        d->slider->setSingleStep(d->step);
        d->slider->setValue(convertToInt(m_value));
        d->slider->setStyleSheet("QSlider::handle:horizontal {width: 15px;}");

        this->addToInternWidgets(d->slider);
        connect(d->slider, SIGNAL(destroyed()),       this, SLOT(removeInternSlider()));
        connect(d->slider, SIGNAL(valueChanged(int)), this, SLOT(setSliderIntValue(int)));
    }
    return d->slider;
}

QLabel* medDoubleParameterL::getValueLabel()
{
    if(!d->valueLabel)
    {
        d->valueLabel = new QLabel;
        d->valueLabel->setText(QString::number(m_value, 'g', 2));

        this->addToInternWidgets(d->valueLabel);
        connect(d->valueLabel, SIGNAL(destroyed()), this, SLOT(removeInternLabel()));
    }

    return d->valueLabel;
}

QWidget* medDoubleParameterL::getWidget()
{
    return this->getSpinBox();
}

void medDoubleParameterL::removeInternSpinBox()
{
    this->removeFromInternWidgets(d->spinBox);
    d->spinBox = nullptr;
}

void medDoubleParameterL::removeInternSlider()
{
    this->removeFromInternWidgets(d->slider);
    d->slider = nullptr;
}

void medDoubleParameterL::removeInternLabel()
{
    this->removeFromInternWidgets(d->valueLabel);
    d->valueLabel = nullptr;
}

int medDoubleParameterL::convertToInt(double value)
{
    return static_cast<int>((value-d->min)/d->step);
}

void medDoubleParameterL::setSliderIntValue(int value)
{
    double currentDoubleValue = static_cast<double>(value);
    setValue(currentDoubleValue * d->step + d->min);
}
