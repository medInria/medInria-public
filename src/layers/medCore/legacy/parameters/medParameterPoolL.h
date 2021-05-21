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
#include <QVector2D>
#include <QLabel>

#include <medCoreLegacyExport.h>
class medAbstractParameterL;
class medDataIndex;
class medStringListParameterL;

class medParameterPoolLPrivate;
class MEDCORELEGACY_EXPORT medParameterPoolL : public QObject
{
    Q_OBJECT

public:
    medParameterPoolL(QObject* parent = nullptr);
    virtual ~medParameterPoolL();

    void append(medAbstractParameterL *parameter);
    void append(QList<medAbstractParameterL *> parameters);
    void clear();
    int count() const;
    void remove (medAbstractParameterL* parameter);
    void removeAll (QString name);

    QList<medAbstractParameterL*> parameters();
    QList<medAbstractParameterL*> parameters(QString name);
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

    void removeInternParam();

private:
    void connectParam(medAbstractParameterL *parameter);
    void disconnectParam(medAbstractParameterL *parameter);

    medParameterPoolLPrivate *d;
};
