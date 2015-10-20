/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDoubleParameter.h>

class medDoubleParameterPrivate
{
public:
    double value;
};

medDoubleParameter::medDoubleParameter(QString const& name,  QObject *parent)
    : medAbstractParameter(name, parent), d(new medDoubleParameterPrivate)
{

}

medDoubleParameter::~medDoubleParameter()
{

}

double medDoubleParameter::value() const
{
    return d->value;
}

void medDoubleParameter::setValue(double value)
{
    if(value != d->value)
    {
        d->value = value;
        emit valueChanged(d->value);
    }
}

void medDoubleParameter::trigger()
{
    emit valueChanged(d->value);
}
