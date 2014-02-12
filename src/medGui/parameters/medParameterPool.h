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
#include <QColor>

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
    void removeAll (QString name);

    QList<medAbstractParameter*> parameters();
    QList<medAbstractParameter*> parameters(QString name);
    QStringList parametersNames();

    QString name() const;
    void setName(QString);

    QColor color();
    void setColor(QColor color);

private slots:
    void triggerParams();
    void changeParamsValue(bool value);
    void changeParamsValue(int value);
    void changeParamsValue(double& value);
    void changeParamsValue(QString& value);

    void _prvt_removeInternParam();

private:
    void _prvt_connectParam(medAbstractParameter *parameter);
    void _prvt_disconnectParam(medAbstractParameter *parameter);

private:
    medParameterPoolPrivate *d;
};
