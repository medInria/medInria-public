/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medIntParameter.h>

#include <dtkLog>

class medIntParameterPrivate
{
public:
    int value;
    int min, max;
};

medIntParameter::medIntParameter(QString const& name,  QObject *parent)
    : medAbstractParameter(name, parent), d(new medIntParameterPrivate)
{
    d->value = 0;
    d->min = 0;
    d->max = 100;
}

medIntParameter::~medIntParameter()
{

}

int medIntParameter::value() const
{
    return d->value;
}

void medIntParameter::setValue(int value)
{
    if(value != d->value)
    {
        // TODO RDE - Those tests sound bad.
        if(value < d->min)
            d->value = d->min;
        else if(value > d->max)
            d->value = d->max;
        else d->value = value;

        d->value = value;
        emit valueChanged(d->value);
    }
}

void medIntParameter::setRange(int min, int max)
{
    if(min >= max)
    {
        dtkDebug() << "attemp to set invalid range to "
                   << this->caption() << this->id() << this;
    }
    else
    {
        d->min = min;
        d->max = max;
        emit rangeChanged(min, max);
    }
}

int medIntParameter::minimum() const
{
    return d->min;
}

int medIntParameter::maximum() const
{
    return d->max;
}

void medIntParameter::trigger()
{
    emit valueChanged(d->value);
}
