/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medSlicingParameter.h"

#include <math.h>       // floor

#include <QSpinBox>
#include <QSlider>
#include <QDebug>

#include <medIntParameter.h>

class medSlicingParameterPrivate
{
public:
    double posMin;
    double posMax;

    int numberOfSlice;
    double resolution;

    medIntParameter *sliceParameter;

    QSpinBox *spinBox;

    ~medSlicingParameterPrivate()
    {
        delete spinBox;
    }
};

medSlicingParameter::medSlicingParameter(QString name):
    medAbstractDoubleParameter(name),
    d(new medSlicingParameterPrivate)
{
    d->sliceParameter = new medIntParameter(name);

    d->numberOfSlice = 100;
    d->posMin = 0;
    d->posMax = 99;
    m_value = 49;
    d->spinBox = NULL;
}

medSlicingParameter::~medSlicingParameter()
{
    delete d;
}

void medSlicingParameter::updateInternWigets()
{
    if(d->spinBox)
        d->spinBox->setValue(m_value);
}

void medSlicingParameter::setPositionRange(double posMin, double posMax)
{
    d->posMin = posMin;
    d->posMax = posMax;

    if(d->spinBox)
        d->spinBox->setRange(posMin, posMax);
}

void medSlicingParameter::setNumberOfSLice(int numberOfSlice)
{
    if(numberOfSlice < 1)
    {
        qWarning() << "Attempt to set less than 1 slice to medSlicingParameter: " << this << "1 is set as default value";
        d->numberOfSlice = 1;
    }
    else
        d->numberOfSlice = numberOfSlice;

    d->resolution = (d->posMax - d->posMin) / d->numberOfSlice;
    d->sliceParameter->setRange(0, d->numberOfSlice - 1);
}

double medSlicingParameter::mapSLiceToPosition(int slice)
{
    return slice * d->resolution + d->posMin;
}

int medSlicingParameter::mapPositionToSLice(const double& position)
{
    return floor((position - d->posMin) / d->resolution);
}

void medSlicingParameter::setSlice(int slice)
{
    this->setSlice(this->mapSLiceToPosition(slice));
}

void medSlicingParameter::setSlice(double position)
{
    d->sliceParameter->setValue(this->mapPositionToSLice(position));
    this->setValue(position);
}

QSlider* medSlicingParameter::getSlider()
{
    return d->sliceParameter->getSlider();
}

QSpinBox* medSlicingParameter::getSpinBox()
{
    if(!d->spinBox)
    {
        d->spinBox = new QSpinBox;
        d->spinBox->setRange(d->posMin, d->posMax);
        d->spinBox->setValue(m_value);

        this->addToInternWidgets(d->spinBox);
        connect(d->spinBox, SIGNAL(destroyed()), this, SLOT(removeInternSlider()));
        connect(d->spinBox, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
    }
    return d->spinBox;
}


QWidget* medSlicingParameter::getWidget()
{
    return this->getSlider();
}


void medSlicingParameter::removeInternSpinBox()
{
    this->removeFromInternWidgets(d->spinBox);
    d->spinBox = NULL;
}


