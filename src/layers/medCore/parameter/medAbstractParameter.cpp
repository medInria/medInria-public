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
    QString id;
    QString description;
    QString caption;
};

medAbstractParameter::medAbstractParameter(const QString & id, QObject *parent)
    : QObject(parent), d(new medAbstractParameterPrivate)
{
    d->id = id;
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

QString medAbstractParameter::id() const
{
    return d->id;
}


void medAbstractParameter::setDescription(const QString & description)
{
    d->description = description;
    emit descriptionChanged(d->description);
}

QString medAbstractParameter::description() const
{    
    return d->description;
}

void medAbstractParameter::setCaption(const QString & caption)
{
    d->caption = caption;
    emit captionChanged(d->caption);
}

QString medAbstractParameter::caption() const
{
    return d->caption;
}

bool medAbstractParameter::match(const medAbstractParameter *other) const
{
    return (this->type() == other->type() && d->id == other->id());
}
