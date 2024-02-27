/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QWidget>
#include <QString>

#include <medCoreExport.h>

class MEDCORE_EXPORT medAbstractArea : public QWidget
{
    Q_OBJECT

public:
    medAbstractArea(QWidget* parent = nullptr);
    virtual ~medAbstractArea();

    virtual QString title() const = 0;
    virtual QString description() const = 0;
};
