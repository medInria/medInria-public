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

class medIntParameterPrivate;
class MEDCORE_EXPORT medIntParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medIntParameter(const QString & name, QObject *parent = NULL);
    virtual ~medIntParameter();

    int value() const;
public slots:
    void setValue(int value);

    virtual void trigger();

signals:
    void valueChanged(int value);

private:
    const QScopedPointer<medIntParameterPrivate> d;
};
