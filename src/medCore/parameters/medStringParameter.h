/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameter.h>

#include <medCoreExport.h>

class QLineEdit;

class medStringParameterPrivate;

class MEDCORE_EXPORT medStringParameter : public medAbstractStringParameter
{
    Q_OBJECT

public:
    medStringParameter(QString name = "Unkown string parameter", QObject* parent = 0);
    virtual ~medStringParameter();

public:
    void clear();

public:
    QLineEdit* getLineEdit();
    virtual QWidget* getWidget();

protected:
    virtual void updateInternWigets();

private slots:
    void removeInternLineEdit();

private:
    medStringParameterPrivate* d;
};


