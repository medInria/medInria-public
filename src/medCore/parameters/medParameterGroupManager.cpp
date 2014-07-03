/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medParameterGroupManager.h>

#include <medAbstractParameterGroup.h>
#include <medViewParameterGroup.h>
#include <medLayerParameterGroup.h>

#include <QApplication>
#include <medAbstractWorkspace.h>


class medParameterGroupManagerPrivate
{
public:
    QMultiHash<QString, medViewParameterGroup* > viewGroups;
    QMultiHash<QString, medLayerParameterGroup* > layerGroups;
    QString currentWorkspace;

};

medParameterGroupManager *medParameterGroupManager::instance()
{
    if(!s_instance)
        s_instance = new medParameterGroupManager();

    return s_instance;
}

medParameterGroupManager::medParameterGroupManager(void) : d(new medParameterGroupManagerPrivate)
{
    d->currentWorkspace = "";
}

medParameterGroupManager::~medParameterGroupManager(void)
{
    delete d;
    d = NULL;
}

void medParameterGroupManager::registerNewGroup(medAbstractParameterGroup* group)
{
    QString workspace;
    if(group->workspace() != "")
        workspace = group->workspace();
    else workspace =d->currentWorkspace;

    medViewParameterGroup* viewGroup = dynamic_cast<medViewParameterGroup*>(group);
    medLayerParameterGroup* layerGroup = dynamic_cast<medLayerParameterGroup*>(group);
    if(viewGroup)
        d->viewGroups.insert(workspace, viewGroup);
    else if(layerGroup)
        d->layerGroups.insert(workspace, layerGroup);
}

void medParameterGroupManager::unregisterGroup(medAbstractParameterGroup *group)
{
    medViewParameterGroup* viewGroup = dynamic_cast<medViewParameterGroup*>(group);
    medLayerParameterGroup* layerGroup = dynamic_cast<medLayerParameterGroup*>(group);
    if(viewGroup)
        d->viewGroups.remove(d->currentWorkspace, viewGroup);
    else if(layerGroup)
        d->layerGroups.remove(d->currentWorkspace, layerGroup);
}

QList<medViewParameterGroup*> medParameterGroupManager::viewGroups(QString workspace)
{
    return d->viewGroups.values(workspace);
}

QList<medLayerParameterGroup*> medParameterGroupManager::layerGroups(QString workspace)
{
    return d->layerGroups.values(workspace);
}

medViewParameterGroup* medParameterGroupManager::viewGroup(QString groupName, QString workspace)
{
    medViewParameterGroup* result = NULL;
    if(workspace == "")
        workspace = d->currentWorkspace;

    foreach(medViewParameterGroup* viewGroup, d->viewGroups.values(workspace))
    {
        if( viewGroup->name() == groupName )
        {
            result = viewGroup;
            break;
        }
    }

    return result;
}

medLayerParameterGroup* medParameterGroupManager::layerGroup(QString groupName, QString workspace)
{
    medLayerParameterGroup* result = NULL;
    if(workspace == "")
        workspace = d->currentWorkspace;

    foreach(medLayerParameterGroup* layerGroup, d->layerGroups.values(workspace))
    {
        if( layerGroup->name() == groupName )
        {
            result = layerGroup;
            break;
        }
    }

    return result;
}

QList<medViewParameterGroup*> medParameterGroupManager::groups(medAbstractView *view)
{
    QHashIterator<QString, medViewParameterGroup*> iter(d->viewGroups);
    QList<medViewParameterGroup*> results;

    while (iter.hasNext())
    {
        iter.next();
        medViewParameterGroup *viewGroup = iter.value();
        if(viewGroup && viewGroup->impactedViews().contains(view))
            results.append(viewGroup);
    }

    return results;
}

QList<medLayerParameterGroup*> medParameterGroupManager::groups(medAbstractLayeredView *view, unsigned int layer)
{
    QHashIterator<QString, medLayerParameterGroup*> iter(d->layerGroups);
    QList<medLayerParameterGroup*> results;

    while (iter.hasNext())
    {
        iter.next();
        medLayerParameterGroup *layerGroup = iter.value();
        if(layerGroup && layerGroup->impactedLayers().contains(view, layer))
            results.append(layerGroup);
    }

    return results;
}

void medParameterGroupManager::setCurrentWorkspace(QString workspace)
{
    d->currentWorkspace = workspace;
}

medParameterGroupManager *medParameterGroupManager::s_instance = NULL;
