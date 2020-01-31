#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameter.h>

class medDoubleParameterPrivate;
class MEDCORE_EXPORT medDoubleParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medDoubleParameter(const QString & name, QObject *parent = nullptr);
    virtual ~medDoubleParameter();

    virtual medParameterType type() const {return medParameterType::MED_PARAMETER_DOUBLE;}

    double value() const;

    void setRange(double min, double max);
    double minimum() const;
    double maximum() const;

public slots:
    void setValue(double value);

    virtual void trigger();

signals:
    void valueChanged(double value);
    void rangeChanged(double min, double max);

private:
    const QScopedPointer<medDoubleParameterPrivate> d;
};
