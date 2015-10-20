/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameter.h>

class medAbstractParameterPrivate
{
public:
    QString name;
    QString description;
};

medAbstractParameter::medAbstractParameter(const QString & name, QObject *parent)
    : QObject(parent), d(new medAbstractParameterPrivate)
{
    d->name = name;
}

medAbstractParameter::~medAbstractParameter()
{
     // no needed to explicitly delete the d pointer because we used a QScopedPointer
     // to hold it.

     // However we still have to explictly write the destructor, even if empty, otherwise,
     // the compiler auto create an inlined one which make
     // 'const QScopedPointer<medAbstractParameterPrivate> d;' impossible with a forward
     // declaration of medAbstractParameterPrivate.
}

QString medAbstractParameter::name() const
{
    return d->name;
}


void medAbstractParameter::setDescription(const QString & description)
{
    d->description = description;
}

QString medAbstractParameter::description() const
{    
    return d->name;
}

bool medAbstractParameter::match(const medAbstractParameter *other) const
{
    return d->name == other->name();
}
