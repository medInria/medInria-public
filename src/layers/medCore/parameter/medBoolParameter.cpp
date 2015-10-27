/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medBoolParameter.h>

class medBoolParameterPrivate
{
public:
    bool value;
};

medBoolParameter::medBoolParameter(QString const& name,  QObject *parent)
    : medAbstractParameter(name, parent), d(new medBoolParameterPrivate)
{

}

medBoolParameter::~medBoolParameter()
{

}

bool medBoolParameter::value() const
{
    return d->value;
}

void medBoolParameter::setValue(bool value)
{
    if(value != d->value)
    {
        d->value = value;
        emit valueChanged(d->value);
    }
}

void medBoolParameter::trigger()
{
    emit valueChanged(d->value);
}
