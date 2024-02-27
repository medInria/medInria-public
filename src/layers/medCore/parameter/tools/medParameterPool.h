#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <QColor>

#include <medCoreExport.h>
class medAbstractParameter;
class medStringListParameter;

class medParameterPoolPrivate;
class MEDCORE_EXPORT medParameterPool : public QObject
{
    Q_OBJECT

public:
    medParameterPool(QObject* parent = nullptr);
    virtual ~medParameterPool();

    void append(medAbstractParameter *parameter);
    void append(QList<medAbstractParameter *> parameters);
    void clear();
    int count() const;
    void remove (medAbstractParameter* parameter);
    void removeAll (QString name);

    QList<medAbstractParameter*> parameters();
    QList<medAbstractParameter*> parameters(QString id);
    QStringList parametersNames();

    QString name() const;
    void setName(QString);

    QColor color();
    void setColor(QColor color);

private slots:
    void changeParamsValue();

    void removeInternParam();

private:
    void connectParam(medAbstractParameter *parameter);
    void disconnectParam(medAbstractParameter *parameter);

    medParameterPoolPrivate *d;
};
