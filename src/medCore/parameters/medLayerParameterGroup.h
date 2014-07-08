/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameterGroup.h>

class medLayerParameterGroupPrivate;

class medLayerParameterGroup : public medAbstractParameterGroup
{
    Q_OBJECT

public:
    medLayerParameterGroup(QString name = "", QObject *parent = 0, QString workspace = "");
    virtual ~medLayerParameterGroup();

    void addImpactedlayer(medAbstractLayeredView *view, unsigned int layer);
    void removeImpactedlayer(medAbstractLayeredView *view, unsigned int layer);
    QMultiHash<medAbstractLayeredView*, unsigned int> impactedLayers();

    void setLinkAllParameters(bool linkAll);

protected:
    virtual void updatePool();
    void updateParameterToLinkList(medAbstractLayeredView *view, unsigned int layer);

private slots:
    void removeImpactedlayer(uint);

private:
    medLayerParameterGroupPrivate *d;
};


