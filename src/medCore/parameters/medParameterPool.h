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
#include <QColor>
#include <QVector2D>
#include <QLabel>

#include <medCoreExport.h>

class medAbstractParameter;
class medDataIndex;
class medStringListParameter;

class medParameterPoolPrivate;
class MEDCORE_EXPORT medParameterPool : public QObject
{
    Q_OBJECT

public:
    medParameterPool(QObject* parent = 0);
    virtual ~medParameterPool();

    void append(medAbstractParameter *parameter);
    void append(QList<medAbstractParameter *> parameters);
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
    void changeParamsValue(double value);
    void changeParamsValue(const QString& value);
    void changeParamsValue(const QVector2D& value);
    void changeParamsValue(const QVector3D& value);
    void changeParamsValue(const QVector4D& value);
    void changeParamsValue(const QHash<QString, QVariant> &values);
    void changeParamsValue(const QList<medDataIndex> &values);
    void changeParamsValue(const medDataIndex &value);

    void removeInternParam();

private:
    void connectParam(medAbstractParameter *parameter);
    void disconnectParam(medAbstractParameter *parameter);

private:
    medParameterPoolPrivate *d;
};



