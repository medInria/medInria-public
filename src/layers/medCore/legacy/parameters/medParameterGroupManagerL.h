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

#include <medCoreExport.h>

class medAbstractView;
class medAbstractLayeredView;
class medAbstractParameterGroupL;
class medViewParameterGroupL;
class medLayerParameterGroupL;
class medAbstractData;

class medParameterGroupManagerLPrivate;

class MEDCORE_EXPORT medParameterGroupManagerL : public QObject
{
    Q_OBJECT

public:
    static medParameterGroupManagerL *instance();

    void registerNewGroup(medAbstractParameterGroupL* group);
    void unregisterGroup(medAbstractParameterGroupL* group);

    QList<medViewParameterGroupL*> viewGroups(medAbstractView *);
    QList<medLayerParameterGroupL*> layerGroups(medAbstractLayeredView *, medAbstractData *);

    QList<medViewParameterGroupL*> viewGroups(QString workspace);
    QList<medLayerParameterGroupL*> layerGroups(QString workspace);

    medViewParameterGroupL* viewGroup(QString groupName, QString workspace = "");
    medLayerParameterGroupL* layerGroup(QString groupName, QString workspace = "");

    void setCurrentWorkspace(QString workspace);

protected:
    medParameterGroupManagerL();
    ~medParameterGroupManagerL();

    static medParameterGroupManagerL *s_instance;

private:
    medParameterGroupManagerLPrivate *d;
};

