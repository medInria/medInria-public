/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medStringParameter.h>

class medPathParameterPrivate;

class MEDCORE_EXPORT medPathParameter : public medStringParameter
{
    Q_OBJECT

public:
    medPathParameter(QString name = "Unknown path parameter", QObject* parent = 0);
    virtual ~medPathParameter();

public:
    virtual QWidget* getWidget();

private slots:
    void removeInternWidget();
    void openFileDialog();

private:
    medPathParameterPrivate* d;
};

