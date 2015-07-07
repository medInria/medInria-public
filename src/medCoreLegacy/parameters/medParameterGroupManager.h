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

#include <medCoreExport.h>

class medAbstractView;
class medAbstractLayeredView;
class medAbstractParameterGroup;
class medViewParameterGroup;
class medLayerParameterGroup;
class medAbstractData;

class medParameterGroupManagerPrivate;

class MEDCORE_EXPORT medParameterGroupManager : public QObject
{
    Q_OBJECT

public:
    static medParameterGroupManager *instance();

    void registerNewGroup(medAbstractParameterGroup* group);
    void unregisterGroup(medAbstractParameterGroup* group);

    QList<medViewParameterGroup*> viewGroups(medAbstractView *);
    QList<medLayerParameterGroup*> layerGroups(medAbstractLayeredView *, medAbstractData *);

    QList<medViewParameterGroup*> viewGroups(QString workspace);
    QList<medLayerParameterGroup*> layerGroups(QString workspace);

    medViewParameterGroup* viewGroup(QString groupName, QString workspace = "");
    medLayerParameterGroup* layerGroup(QString groupName, QString workspace = "");

    void setCurrentWorkspace(QString workspace);

protected:
    medParameterGroupManager();
    ~medParameterGroupManager();

protected:
    static medParameterGroupManager *s_instance;

private:
    medParameterGroupManagerPrivate *d;
};

