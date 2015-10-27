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

class medDoubleParameterPrivate;
class MEDCORE_EXPORT medDoubleParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medDoubleParameter(const QString & name, QObject *parent = NULL);
    virtual ~medDoubleParameter();

    double value() const;
public slots:
    void setValue(double value);

    virtual void trigger();

signals:
    void valueChanged(double value);

private:
    const QScopedPointer<medDoubleParameterPrivate> d;
};
