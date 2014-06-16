/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medIntParameter.h>

#include <QSpinBox>
#include <QSlider>

class medIntParameterPrivate
{
public:
    int min;
    int max;

    QSpinBox *spinBox;
    QSlider *slider;

    ~medIntParameterPrivate()
    {
        if(spinBox)
            delete spinBox;

        if(slider)
            delete slider;
    }
};

medIntParameter::medIntParameter(QString name, QObject* parent):
    medAbstractIntParameter(name, parent),
    d(new medIntParameterPrivate)
{
    d->min = 0;
    d->max = 100;
    m_value = 0;
    d->spinBox = NULL;
    d->slider = NULL;
}

medIntParameter::~medIntParameter()
{
    delete d;
}

void medIntParameter::setValue(int value)
{
    if(m_value == value)
        return;

    if(value < d->min)
        m_value = d->min;
    else if(value > d->max)
        m_value = d->max;
    else m_value = value;

    //  update intern widget
    this->blockInternWidgetsSignals(true);
    this->updateInternWigets();
    this->blockInternWidgetsSignals(false);

    emit valueChanged(m_value);
}

void medIntParameter::updateInternWigets()
{
    if(d->spinBox)
        d->spinBox->setValue(m_value);

    if(d->slider)
        d->slider->setValue(m_value);
}

void medIntParameter::setRange(int min, int max)
{
    d->min = min;
    d->max = max;

    if(d->spinBox)
        d->spinBox->setRange(min, max);

    if(d->slider)
        d->slider->setRange(min, max);
}

QSlider* medIntParameter::getSlider()
{
    if(!d->slider)
    {
        d->slider = new QSlider;
        d->slider->setRange(d->min, d->max);
        d->slider->setValue(m_value);

        this->addToInternWidgets(d->slider);
        connect(d->slider, SIGNAL(destroyed()), this, SLOT(removeInternSlider()));
        connect(d->slider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    }
    return d->slider;
}

QSpinBox* medIntParameter::getSpinBox()
{
    if(!d->spinBox)
    {
        d->spinBox = new QSpinBox;
        d->spinBox->setRange(d->min, d->max);
        d->spinBox->setValue(m_value);

        this->addToInternWidgets(d->spinBox);
        connect(d->spinBox, SIGNAL(destroyed()), this, SLOT(removeInternSpinBox()));
        connect(d->spinBox, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    }
    return d->spinBox;
}


QWidget* medIntParameter::getWidget()
{
    return this->getSpinBox();
}

void medIntParameter::removeInternSlider()
{
    this->removeFromInternWidgets(d->slider);
    d->slider = NULL;
}

void medIntParameter::removeInternSpinBox()
{
    this->removeFromInternWidgets(d->spinBox);
    d->spinBox = NULL;
}


