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

#include <medCoreExport.h>

class medGroupParameterPrivate;
class MEDCORE_EXPORT medGroupParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medGroupParameter(QString const& id, QObject* parent = nullptr);
    virtual ~medGroupParameter();

    medParameterType type() const override { return medParameterType::MED_PARAMETER_GROUP; }

    QList<medAbstractParameter*> value() const;

    bool copyValueTo(medAbstractParameter &dest) override;

    QVariantMap toVariantMap() const override;
    bool fromVariantMap(QVariantMap const& pi_variantMap) override;

public slots:
    void addParameter(medAbstractParameter* param);
    void removeParameter(medAbstractParameter* param);

    void trigger() override;

signals:
    void triggered();
    void parameterAdded(medAbstractParameter* param);
    void parameterRemoved(medAbstractParameter* param);

private:
    QStringList idList();

private:
    const QScopedPointer<medGroupParameterPrivate> d;
};

