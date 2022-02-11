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
#include <QValidator>

class medStringParameterPrivate;
class MEDCORE_EXPORT medStringParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medStringParameter(const QString & name, QObject *parent = nullptr);
    ~medStringParameter() override;

    medParameterType type() const override{return medParameterType::MED_PARAMETER_STRING;}

    virtual QString value() const;

    bool copyValueTo(medAbstractParameter &dest) override;

    virtual void setValidator(QValidator *pi_poValidator);
    virtual QValidator *getValidator() const;

    QVariantMap toVariantMap() const override;
    bool fromVariantMap(QVariantMap const& pi_variantMap) override;

public slots:
    virtual bool setValue(QString const& value);

    virtual void trigger();

signals:
    void valueChanged(QString const& value);
    void validatorChanged(QValidator *const& poValidator);

private:
    const QScopedPointer<medStringParameterPrivate> d;
};
