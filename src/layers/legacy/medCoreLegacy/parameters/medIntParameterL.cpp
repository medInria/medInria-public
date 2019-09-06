/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medIntParameterL.h>

#include <QSpinBox>
#include <QSlider>

class medIntParameterLPrivate
{
public:
    int min;
    int max;

    QSpinBox *spinBox;
    QSlider *slider;

    ~medIntParameterLPrivate()
    {
        if(spinBox)
            delete spinBox;

        if(slider)
            delete slider;
    }
};

medIntParameterL::medIntParameterL(QString name, QObject* parent):
    medAbstractIntParameterL(name, parent),
    d(new medIntParameterLPrivate)
{
    d->min = 0;
    d->max = 100;
    m_value = 0;
    d->spinBox = NULL;
    d->slider = NULL;
}

medIntParameterL::~medIntParameterL()
{
    delete d;
}

void medIntParameterL::setValue(int value)
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

void medIntParameterL::updateInternWigets()
{
    if(d->spinBox)
        d->spinBox->setValue(m_value);

    if(d->slider)
        d->slider->setValue(m_value);
}

void medIntParameterL::setRange(int min, int max)
{
    d->min = min;
    d->max = max;

    if(d->spinBox)
        d->spinBox->setRange(min, max);

    if(d->slider)
        d->slider->setRange(min, max);
}

QSlider* medIntParameterL::getSlider()
{
    if(!d->slider)
    {
        d->slider = new QSlider;
        d->slider->setRange(d->min, d->max);
        d->slider->setValue(m_value);
        d->slider->setStyleSheet("QSlider::handle:horizontal {width: 15px;}");

        this->addToInternWidgets(d->slider);
        connect(d->slider, SIGNAL(destroyed()), this, SLOT(removeInternSlider()));
        connect(d->slider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    }
    return d->slider;
}

QSpinBox* medIntParameterL::getSpinBox()
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


QWidget* medIntParameterL::getWidget()
{
    return this->getSpinBox();
}

void medIntParameterL::removeInternSlider()
{
    this->removeFromInternWidgets(d->slider);
    d->slider = NULL;
}

void medIntParameterL::removeInternSpinBox()
{
    this->removeFromInternWidgets(d->spinBox);
    d->spinBox = NULL;
}


