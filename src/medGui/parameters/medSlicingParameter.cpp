/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSlicingParameter.h>

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

medSlicingParameter::medSlicingParameter(QString name, QObject* parent):
    medAbstractDoubleParameter(name, parent),
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

void medSlicingParameter::setNumberOfSlice(int numberOfSlice)
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

double medSlicingParameter::mapSliceToPosition(int slice)
{
    return slice * d->resolution + d->posMin;
}

int medSlicingParameter::mapPositionToSlice(const double& position)
{
    return floor((position - d->posMin) / d->resolution);
}

void medSlicingParameter::setSlice(int slice)
{
    this->setSlice(this->mapSliceToPosition(slice));
}

void medSlicingParameter::setSlice(double position)
{
    d->sliceParameter->setValue(this->mapPositionToSlice(position));
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
        connect(d->spinBox, SIGNAL(destroyed()), this, SLOT(_prvt_removeInternSlider()));
        connect(d->spinBox, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
    }
    return d->spinBox;
}


QWidget* medSlicingParameter::getWidget()
{
    return this->getSlider();
}


void medSlicingParameter::_prvt_removeInternSpinBox()
{
    this->removeFromInternWidgets(d->spinBox);
    d->spinBox = NULL;
}


