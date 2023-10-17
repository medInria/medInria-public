/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameterGroupL.h>
#include <medAbstractWorkspaceLegacy.h>
#include <medLayerParameterGroupL.h>
#include <medParameterGroupManagerL.h>
#include <medViewParameterGroupL.h>

#include <QApplication>

class medParameterGroupManagerLPrivate
{
public:
    QMultiHash<QString, medViewParameterGroupL* > viewGroups;
    QMultiHash<QString, medLayerParameterGroupL* > layerGroups;
    QString currentWorkspace;

};

std::unique_ptr<medParameterGroupManagerL> medParameterGroupManagerL::s_instance = nullptr;

medParameterGroupManagerL &medParameterGroupManagerL::instance()
{
    if(!s_instance)
    {
        s_instance = std::unique_ptr<medParameterGroupManagerL>(new medParameterGroupManagerL());
    }
    return *s_instance.get();
}

medParameterGroupManagerL::medParameterGroupManagerL(void) : d(new medParameterGroupManagerLPrivate)
{
    d->currentWorkspace = "";
}

medParameterGroupManagerL::~medParameterGroupManagerL()
{
    delete d;
    d = nullptr;
}

void medParameterGroupManagerL::registerNewGroup(medAbstractParameterGroupL* group)
{
    QString workspace;
    if(group->workspace() != "")
        workspace = group->workspace();
    else if(d->currentWorkspace!= "")
        workspace = d->currentWorkspace;
    else workspace = "All";

    medViewParameterGroupL* viewGroup = dynamic_cast<medViewParameterGroupL*>(group);
    medLayerParameterGroupL* layerGroup = dynamic_cast<medLayerParameterGroupL*>(group);
    if(viewGroup)
        d->viewGroups.insert(workspace, viewGroup);
    else if(layerGroup)
        d->layerGroups.insert(workspace, layerGroup);
}

void medParameterGroupManagerL::unregisterGroup(medAbstractParameterGroupL *group)
{
    QString workspace;
    if(group->workspace() != "")
        workspace = group->workspace();
    else workspace =d->currentWorkspace;

    medViewParameterGroupL* viewGroup = dynamic_cast<medViewParameterGroupL*>(group);
    medLayerParameterGroupL* layerGroup = dynamic_cast<medLayerParameterGroupL*>(group);
    if(viewGroup)
        d->viewGroups.remove(workspace, viewGroup);
    else if(layerGroup)
        d->layerGroups.remove(workspace, layerGroup);
}

QList<medViewParameterGroupL*> medParameterGroupManagerL::viewGroups(QString workspace)
{
    QList<medViewParameterGroupL*> res = d->viewGroups.values(workspace);
    res.append(d->viewGroups.values("All"));
    return res;
}

QList<medLayerParameterGroupL*> medParameterGroupManagerL::layerGroups(QString workspace)
{
    return d->layerGroups.values(workspace);
}

medViewParameterGroupL* medParameterGroupManagerL::viewGroup(QString groupName, QString workspace)
{
    medViewParameterGroupL* result = nullptr;

    if(workspace == "")
    {
        workspace = d->currentWorkspace;
    }

    for(medViewParameterGroupL* viewGroup : d->viewGroups.values(workspace))
    {
        if( viewGroup->name() == groupName )
        {
            result = viewGroup;
            break;
        }
    }

    return result;
}

medLayerParameterGroupL* medParameterGroupManagerL::layerGroup(QString groupName, QString workspace)
{
    medLayerParameterGroupL* result = nullptr;

    if(workspace == "")
    {
        workspace = d->currentWorkspace;
    }

    for(medLayerParameterGroupL* layerGroup : d->layerGroups.values(workspace))
    {
        if( layerGroup->name() == groupName )
        {
            result = layerGroup;
            break;
        }
    }

    return result;
}

QList<medViewParameterGroupL*> medParameterGroupManagerL::viewGroups(medAbstractView *view)
{
    QHashIterator<QString, medViewParameterGroupL*> iter(d->viewGroups);
    QList<medViewParameterGroupL*> results;

    while (iter.hasNext())
    {
        iter.next();
        medViewParameterGroupL *viewGroup = iter.value();
        if(viewGroup && viewGroup->impactedViews().contains(view))
            results.append(viewGroup);
    }

    return results;
}

QList<medLayerParameterGroupL*> medParameterGroupManagerL::layerGroups(medAbstractLayeredView *view, medAbstractData* layerData)
{
    QHashIterator<QString, medLayerParameterGroupL*> iter(d->layerGroups);
    QList<medLayerParameterGroupL*> results;

    while (iter.hasNext())
    {
        iter.next();
        medLayerParameterGroupL *layerGroup = iter.value();
        if(layerGroup && layerGroup->impactedLayers().contains(view, layerData))
            results.append(layerGroup);
    }

    return results;
}

void medParameterGroupManagerL::setCurrentWorkspace(QString workspace)
{
    d->currentWorkspace = workspace;
}
