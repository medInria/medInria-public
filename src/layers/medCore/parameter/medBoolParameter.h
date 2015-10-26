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


class medBoolParameterPrivate;
class MEDCORE_EXPORT medBoolParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medBoolParameter(const QString & name, QObject *parent = NULL);
    virtual ~medBoolParameter();

    virtual medParameterType type() const {return medParameterType::MED_PARAMETER_BOOL;}

    bool value() const;
public slots:
    void setValue(bool value);

    virtual void trigger();

signals:
    void valueChanged(bool value);

private:
    const QScopedPointer<medBoolParameterPrivate> d;
};
