/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

class medAbstractPlugin : public QObject
{
public:
    medAbstractPlugin(QObject *parent = NULL): QObject(parent) {}

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QString version() const = 0;
};
