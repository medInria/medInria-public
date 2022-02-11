#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <QMap>
#include <QVariant>
#include <typeinfo>

#include <medCoreExport.h>

enum medParameterType
{
    MED_PARAMETER_INT,
    MED_PARAMETER_DOUBLE,
    MED_PARAMETER_BOOL,
    MED_PARAMETER_STRING,
    MED_PARAMETER_STRING_LIST,
    MED_PARAMETER_TRIGGER,
    MED_PARAMETER_TIMELINE,
    MED_PARAMETER_VARIANT_LIST,
    MED_PARAMETER_DATA, //TODO need presenter
    MED_PARAMETER_VECTOR2D, //No presenter available, not needed
    MED_PARAMETER_VECTOR3D, //No presenter available, not needed
    MED_PARAMETER_DATA_LIST, //No presenter available, not needed
};

class medAbstractParameterPrivate;
class MEDCORE_EXPORT medAbstractParameter : public QObject
{
    Q_OBJECT

public:
    medAbstractParameter(QString const& id, QObject* parent = nullptr);
    virtual ~medAbstractParameter();

    QString id() const;

    QString description() const;
    void setDescription(QString const& description);

    QString caption() const;
    void setCaption(QString const& caption);

    int defaultRepresentation() const;
    void setDefaultRepresentation(int representationIndex);

    bool match(medAbstractParameter const* other) const;
    virtual bool copyValueTo(medAbstractParameter &dest) = 0;

    virtual QVariantMap toVariantMap() const = 0;
    virtual bool fromVariantMap(QVariantMap const& pi_variantMap) = 0;
    
signals:
    void captionChanged(QString caption);
    void descriptionChanged(QString description);
    void triggered();

public:
    virtual medParameterType type() const = 0;

public slots:
    virtual void trigger() = 0;

private:
    const QScopedPointer<medAbstractParameterPrivate> d;
};

