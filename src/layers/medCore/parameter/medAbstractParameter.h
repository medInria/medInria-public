/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>

#include <medCoreExport.h>

class medAbstractParameterPrivate;
class MEDCORE_EXPORT medAbstractParameter : public QObject
{
    Q_OBJECT

public:
    medAbstractParameter(QString const& name, QObject* parent = NULL);
    virtual ~medAbstractParameter();

    QString name() const;

    QString description() const;
    void setDescription(QString const& description);

    bool match(medAbstractParameter const* other) const;

public slots:
    virtual void trigger() = 0;

private:
    const QScopedPointer<medAbstractParameterPrivate> d;
};

