/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameterL.h>

#include <QIcon>

#include <medCoreLegacyExport.h>

class QComboBox;
class QWidget;
class QStringList;

class medDataIndex;

class medDataListParameterLPrivate;
class MEDCORELEGACY_EXPORT medDataListParameterL: public medAbstractParameterL
{
    Q_OBJECT

public:
    medDataListParameterL(QString name = "Unknown data list parameter", QObject* parent = 0);
    virtual ~medDataListParameterL();

    void setValues(QList<medDataIndex> values);
    void clear();

    QList <medDataIndex> &values() const;

    virtual QWidget* getWidget();

    virtual void trigger();

signals:
    void valuesChanged (QList <medDataIndex>);

private:
    medDataListParameterLPrivate* d;
};
