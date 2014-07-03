/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>

class medAbstractView;
class medAbstractLayeredView;
class medParameterPool;
class medAbstractParameterGroupPrivate;

class medAbstractParameterGroup : public QObject
{
    Q_OBJECT

public:
    medAbstractParameterGroup(QString name = "", QObject *parent = 0, QString workspace = "");
    virtual ~medAbstractParameterGroup();

    void setName(QString name);
    QString name() const;

    void addParameterToLink(QString parameter);
    void removeParameter(QString parameter);

    void setParametersToLink(QStringList parameters);
    QStringList parameters() const;

    void setColor(QColor color);
    QColor color() const;

    virtual void setLinkAllParameters(bool linkAll);
    bool linkAll() const;

    void setWorkspace(QString workspace);
    QString workspace() const;

    void saveAsPreset();

    virtual void update() = 0;

signals:
    void groupColorChanged(QColor oldColor, QColor newColor);

private:
    medAbstractParameterGroupPrivate *d;
};

