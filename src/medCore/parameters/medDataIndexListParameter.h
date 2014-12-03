/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameter.h>
#include <QIcon>

#include <medCoreExport.h>

class QComboBox;
class QWidget;
class QStringList;

class medDataIndex;

class medDataListIndexParameterPrivate;
class MEDCORE_EXPORT medDataIndexListParameter: public medAbstractParameter
{
    Q_OBJECT

public:
    medDataIndexListParameter(QString name = "Unknown data list parameter", QObject* parent = 0);
    virtual ~medDataIndexListParameter();

    void setValues(QList<medDataIndex> values);
    void clear();

    QList <medDataIndex> &values() const;

    virtual QWidget* getWidget();

    virtual void trigger();

signals:
    void valuesChanged (QList <medDataIndex>);
    
private:
    medDataListIndexParameterPrivate* d;
};
