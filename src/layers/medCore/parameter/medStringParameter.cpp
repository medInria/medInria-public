/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
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
    QValidator *poValidator;
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

void medStringParameter::setValue( QString const& value)
{
    if(value != d->value)
    {
       int i = -1;
       QString tmpVal = value;
       if (d->poValidator == NULL || d->poValidator->validate(tmpVal, i) == QValidator::Acceptable)
       {
          d->value = value;
          emit valueChanged(d->value);
       }
    }
}

void medStringParameter::setValidator(QValidator *pi_poValidator)
{
   if (d->poValidator != pi_poValidator)
   {
      d->poValidator = pi_poValidator;
      if (pi_poValidator)
      {
         int i = -1;
         if (d->poValidator->validate(d->value, i) != QValidator::Acceptable)
         {
            d->value = "";
         }
      }
      emit validatorChanged(d->poValidator);
   }
}


QValidator *medStringParameter::getValidator() const
{
   return d->poValidator;
}

void medStringParameter::trigger()
{
    emit valueChanged(d->value);
}
