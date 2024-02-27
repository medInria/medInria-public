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

#include <QVariant>

class medCompositeParameterPrivate;

class MEDCORE_EXPORT medVariantListParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medVariantListParameter(QString name = "Unknown composite parameter", QObject* parent = nullptr);
    ~medVariantListParameter();

    medParameterType type() const override { return medParameterType::MED_PARAMETER_VARIANT_LIST; }

    void addVariant(QString name, QVariant variant, QVariant min = QVariant(0), QVariant max = QVariant(100), QVariant step = QVariant(1));
    void delVariant(QString name);

    virtual QList<QVariant> values() const;
    virtual QVariant value(const QString) const;
    QList<QPair <QVariant, QVariant> > ranges() const;
    QList<QVariant> steps() const;

    virtual void trigger();
    bool copyValueTo(medAbstractParameter &dest) override;

    QVariantMap toVariantMap() const override;
    bool fromVariantMap(QVariantMap const& pi_variantMap) override;

public slots:
    void setValues(QHash<QString, QVariant> const values);
    bool setValue (QString const &name, QVariant const &value);


signals:
    void valuesChanged(const QHash<QString, QVariant>&);
    void valueChanged  (QString const &, QVariant const &);
    void variantAdded  (QString const &, QVariant const &);
    void variantRemoved(QString const &);

private:
    medCompositeParameterPrivate* d;
};
