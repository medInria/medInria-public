/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>
#include <medCoreExport.h>

class medBoolParameter;
class medBoolParameterGroupPrivate;

class MEDCORE_EXPORT medBoolParameterGroup : public QObject
{
    Q_OBJECT

public:
    medBoolParameterGroup(QObject *parent = NULL);
    virtual ~medBoolParameterGroup();

    void addBoolParameter(medBoolParameter *param);
    QList <medBoolParameter *> &parameters();

public slots:
    void changeActiveBoolParameter(bool value);

private:
    const QScopedPointer<medBoolParameterGroupPrivate> d;
};
