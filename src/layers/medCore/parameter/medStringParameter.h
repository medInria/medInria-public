/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameter.h>
#include <QValidator>

class medStringParameterPrivate;
class MEDCORE_EXPORT medStringParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medStringParameter(const QString & name, QObject *parent = NULL);
    virtual ~medStringParameter();

    virtual medParameterType type() const {return medParameterType::MED_PARAMETER_STRING;}

    QString value() const;

    void setValidator(QValidator *pi_poValidator);
    QValidator *getValidator() const;

public slots:
    void setValue(QString const& value);

    virtual void trigger();

signals:
    void valueChanged(QString const& value);
    void validatorChanged(QValidator *const& poValidator);

private:
    const QScopedPointer<medStringParameterPrivate> d;
};
