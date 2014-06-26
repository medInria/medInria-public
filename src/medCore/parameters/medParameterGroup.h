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
class medParameterGroupPrivate;

class medParameterGroup : public QObject
{
public:
    medParameterGroup(QString name = "", QObject *parent = 0);
    virtual ~medParameterGroup();

    void setName(QString name);
    QString name() const;

    void addParameter(QString parameter);
    void removeParameter(QString parameter);

    void setParameters(QStringList parameters);
    QStringList parameters() const;

    void addImpactedView(medAbstractView *view);
    void removeImpactedView(medAbstractView *view);
    QList<medAbstractView*> impactedViews();

    void addImpactedlayer(medAbstractLayeredView *view, unsigned int layer);
    void removeImpactedlayer(medAbstractLayeredView *view, unsigned int layer);
    QMultiHash<medAbstractLayeredView*, unsigned int> impactedLayers();

    void saveAsPreset();

    void update();

private:
    medParameterGroupPrivate *d;
};

