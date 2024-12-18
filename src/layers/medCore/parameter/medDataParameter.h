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
#include <medAbstractData.h>

class medDataParameterPrivate;
class MEDCORE_EXPORT medDataParameter : public medAbstractParameter
{
    Q_OBJECT

public:
    medDataParameter(const QString & name, QObject *parent = nullptr);
    virtual ~medDataParameter();

    virtual medParameterType type() const {return medParameterType::MED_PARAMETER_DATA;}

    medAbstractData*  value() const;

    bool copyValueTo(medAbstractParameter &dest) override;

    QVariantMap toVariantMap() const override;
    bool fromVariantMap(QVariantMap const& pi_variantMap) override;

    void setRowPosition(int iRow);
    int  rowPosition();
    void setColPosition(int iCol);
    int  colPosition();
    void setTabPosition(int iTab);
    int  tabPosition();

    void setDirection(bool bIn, bool bOut);
    bool isInput();
    bool isOuput();

public slots:
    void setValue(medAbstractData *value);

    virtual void trigger();

signals:
    void valueChanged(medAbstractData*);

private:
    const QScopedPointer<medDataParameterPrivate> d;
};
