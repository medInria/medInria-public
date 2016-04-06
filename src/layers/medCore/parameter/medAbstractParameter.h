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

enum medParameterType
{
    MED_PARAMETER_INT,
    MED_PARAMETER_DOUBLE,
    MED_PARAMETER_BOOL,
    MED_PARAMETER_STRING
};

class medAbstractParameterPrivate;
class MEDCORE_EXPORT medAbstractParameter : public QObject
{
    Q_OBJECT

public:
    medAbstractParameter(QString const& id, QObject* parent = NULL);
    virtual ~medAbstractParameter();

    QString id() const;

    QString description() const;
    void setDescription(QString const& description);

    QString caption() const;
    void setCaption(QString const& caption);

    bool match(medAbstractParameter const* other) const;

signals:
    void captionChanged(QString caption);
    void descriptionChanged(QString description);

public:
    virtual medParameterType type() const = 0;

public slots:
    virtual void trigger() = 0;

private:
    const QScopedPointer<medAbstractParameterPrivate> d;
};

