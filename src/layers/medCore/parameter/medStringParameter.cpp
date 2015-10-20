/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStringParameter.h>

class medStringParameterPrivate
{
public:
    QString value;
};

medStringParameter::medStringParameter(QString const& name,  QObject *parent)
    : medAbstractParameter(name, parent), d(new medStringParameterPrivate)
{

}

medStringParameter::~medStringParameter()
{

}

QString medStringParameter::value() const
{
    return d->value;
}

void medStringParameter::setValue(const QString & value)
{
    if(value != d->value)
    {
        d->value = value;
        emit valueChanged(d->value);
    }
}

void medStringParameter::trigger()
{
    emit valueChanged(d->value);
}
