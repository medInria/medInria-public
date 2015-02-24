/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <QObject>

#include <QMetaType>

class medAbstractData;

class MEDCORE_EXPORT medAbstractTransformation: public QObject
{
    Q_OBJECT

public:
    medAbstractTransformation(QObject  *parent = NULL) : QObject(parent) { }
    ~medAbstractTransformation() {}

public:
    virtual medAbstractData* parameter() const { return 0; }
    virtual void setParameter(medAbstractData *parameter) { }
};

Q_DECLARE_METATYPE(medAbstractTransformation*)

