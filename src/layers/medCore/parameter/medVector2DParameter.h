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

class QVector2D;
class QDomDocument;
class QDomElement;
class medVector2DParameterPrivate;

/*---------------------------------------------------------------------------*/
/*--- WARNING NO presenter for medVector2DParameter and not needed        ---*/
/*---------------------------------------------------------------------------*/
class MEDCORE_EXPORT medVector2DParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medVector2DParameter(QString name = "Unknown Vector 2D parameter", QObject* parent = nullptr);
    virtual ~medVector2DParameter();

    medParameterType type() const override { return medParameterType::MED_PARAMETER_VECTOR2D; }

    QVector2D value() const;

    bool copyValueTo(medAbstractParameter &dest) override;

    QVariantMap toVariantMap() const override;
    bool fromVariantMap(QVariantMap const& pi_variantMap) override;

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);

    static QVector2D fromString(QString value);

public slots:
    virtual void setValue(const QVector2D &);
    virtual void trigger();

signals:
    void valueChanged(const QVector2D &);

protected:
    const QScopedPointer<medVector2DParameterPrivate> d;
};
