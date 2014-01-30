/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>

#include <medGuiExport.h>

#include <medGuiExport.h>

class medAbstractParameter;

class medParameterPoolPrivate;
class MEDGUI_EXPORT medParameterPool : public QObject
{
    Q_OBJECT

public:
    medParameterPool(QObject* parent = 0);
    virtual ~medParameterPool();

    void append(medAbstractParameter *parameter);
    void clear();
    int count() const;
    void remove (medAbstractParameter* parameter);

private slots:
    void triggerParams();
    void changeParamsValue(bool value);
    void changeParamsValue(int value);
    void changeParamsValue(double& value);
    void changeParamsValue(QString& value);

private:
    void connectParam(medAbstractParameter *parameter);
    void disconnectParam(medAbstractParameter *parameter);

private:
    medParameterPoolPrivate *d;
};
