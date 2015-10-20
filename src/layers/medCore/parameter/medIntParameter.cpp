/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medIntParameter.h>

class medIntParameterPrivate
{
public:
    int value;
};

medIntParameter::medIntParameter(QString const& name,  QObject *parent)
    : medAbstractParameter(name, parent), d(new medIntParameterPrivate)
{

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
        d->value = value;
        emit valueChanged(d->value);
    }
}

void medIntParameter::trigger()
{
    emit valueChanged(d->value);
}
