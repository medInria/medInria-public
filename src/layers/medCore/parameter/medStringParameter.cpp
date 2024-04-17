/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medStringParameter.h>

#include <QLineEdit>

class medStringParameterPrivate
{
public:
    QString value;
    QValidator *poValidator;
};

medStringParameter::medStringParameter(QString const& name,  QObject *parent)
    : medAbstractParameter(name, parent), d(new medStringParameterPrivate)
{
    d->poValidator = nullptr;
    connect(this, &medStringParameter::valueChanged, this, &medStringParameter::triggered);
}

medStringParameter::~medStringParameter()
{

}

QString medStringParameter::value() const
{
    return d->value;
}

bool medStringParameter::copyValueTo(medAbstractParameter & dest)
{
    bool bRes = typeid(dest) == typeid(*this);

    if (bRes)
    {
        setValue(dynamic_cast<medStringParameter*>(&dest)->value());
    }

    return bRes;
}

bool medStringParameter::setValue( QString const& value)
{
    QLineEdit *line = qobject_cast<QLineEdit*>(QObject::sender());

    bool bRes = true;
    
    int cursorPos;
    if(line)
        cursorPos = line->cursorPosition();

    if(value != d->value)
    {
       int i = -1;
       QString tmpVal = value;
       bRes = (d->poValidator == nullptr) || (d->poValidator->validate(tmpVal, i) == QValidator::Acceptable);
       if (bRes)
       {
          d->value = value;
          emit valueChanged(d->value);
       }
    }

    if(line)
        line->setCursorPosition(cursorPos);

    return bRes;
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

void medStringParameter::edit()
{
    emit valueEdited(d->value);
}

QVariantMap medStringParameter::toVariantMap() const
{
    QVariantMap varMapRes;

    varMapRes.insert("id", id());
    varMapRes.insert("caption", caption());
    varMapRes.insert("description", description());

    varMapRes.insert("value", d->value);

    return varMapRes;
}

bool medStringParameter::fromVariantMap(QVariantMap const& pi_variantMap)
{
    bool bRes = true;

    bRes = bRes && pi_variantMap.contains("id");
    bRes = bRes && pi_variantMap.contains("caption");
    bRes = bRes && pi_variantMap.contains("description");
    bRes = bRes && pi_variantMap.contains("value");

    if (bRes)
    {
        bRes = pi_variantMap["value"].canConvert(QMetaType::QString);
        if (bRes)
        {
            setCaption(pi_variantMap["caption"].toString());
            setDescription(pi_variantMap["description"].toString());

            d->value = pi_variantMap["value"].toString();
        }
    }

    return bRes;
}
