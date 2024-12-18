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
#include <medDataIndex.h>

#include <QIcon>

class QComboBox;
class QWidget;
class QStringList;


class medDataListParameterPrivate;
class MEDCORE_EXPORT medDataListParameter: public medAbstractParameter
{
    Q_OBJECT

public:
    medDataListParameter(QString name = "Unknown data list parameter", QObject* parent = nullptr);
    virtual ~medDataListParameter();

    medParameterType type() const override { return medParameterType::MED_PARAMETER_DATA_LIST; }

    void setValues(QList<medDataIndex> values);
    void clear();

    QList <medDataIndex> &values() const;

    bool copyValueTo(medAbstractParameter &dest) override;
    
    virtual void trigger();

    QVariantMap toVariantMap() const override;
    bool fromVariantMap(QVariantMap const& pi_variantMap) override;

signals:
    void valuesChanged (QList <medDataIndex>);

private:
    medDataListParameterPrivate* d;
};
