#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameterGroupL.h>
#include <medAbstractData.h>

class medLayerParameterGroupLL;

class MEDCORE_EXPORT medLayerParameterGroupL : public medAbstractParameterGroupL
{
    Q_OBJECT

public:
    medLayerParameterGroupL(QString name = "", QObject *parent = nullptr, QString workspace = "");
    virtual ~medLayerParameterGroupL();

    void addImpactedlayer(medAbstractLayeredView *view, medAbstractData* layerData);
    void removeImpactedlayer(medAbstractLayeredView *view, medAbstractData* layerData);
    QMultiHash<medAbstractLayeredView*, medAbstractData*> impactedLayers();

    void setLinkAllParameters(bool linkAll);

protected:
    virtual void updatePool();
    void updateParameterToLinkList(medAbstractLayeredView *view, medAbstractData* layerData);

private slots:
    void removeImpactedlayer(medAbstractData*);

private:
    medLayerParameterGroupLL *d;
};
