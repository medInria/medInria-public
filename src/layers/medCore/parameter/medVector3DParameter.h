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

class QVector3D;
class QDomDocument;
class QDomElement;
class medVector3DParameterPrivate;

/*---------------------------------------------------------------------------*/
/*--- WARNING NO presenter for medVector2DParameter and not needed        ---*/
/*---------------------------------------------------------------------------*/
class MEDCORE_EXPORT medVector3DParameter : public medAbstractParameter
{
    Q_OBJECT
public:
    medVector3DParameter(QString name = "Unknown Vector 3D parameter", QObject* parent = nullptr);
    virtual ~medVector3DParameter();

    medParameterType type() const override { return medParameterType::MED_PARAMETER_VECTOR3D; }

    QVector3D value() const;

    bool copyValueTo(medAbstractParameter &dest) override;

    QVariantMap toVariantMap() const override;
    bool fromVariantMap(QVariantMap const& pi_variantMap) override;

    virtual void toXMLNode(QDomDocument *doc, QDomElement *currentNode);

    static QVector3D fromString(QString value);

public slots:
    virtual void setValue(const QVector3D &);
    void trigger() override;

signals:
    void valueChanged(const QVector3D &);

protected:
    const QScopedPointer<medVector3DParameterPrivate> d;
};
