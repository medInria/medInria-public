/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medWorkspaceFactory.h>

#include <medToolBoxFactory.h>
#include <medAbstractWorkspace.h>
#include <medWorkspaceFactory.h>

class medWorkspaceFactoryPrivate
{
public:
    typedef QHash<QString, medWorkspaceFactory::Details*> medWorkspaceCreatorHash;
    medWorkspaceCreatorHash creators;
};

medWorkspaceFactory* medWorkspaceFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medWorkspaceFactory;

    return s_instance;
}

bool medWorkspaceFactory::registerWorkspace(QString identifier,
                                            QString name,
                                            QString description,
                                            medWorkspaceCreator creator,
                                            medWorkspaceIsUsable isUsable)
{
    if(!d->creators.contains(identifier))
    {
        medWorkspaceFactory::Details* holder = new medWorkspaceFactory::Details(identifier, name, description, creator, isUsable);
        d->creators.insert(identifier, holder);
        return true;
    }
    return false;
}

QList<QString> medWorkspaceFactory::workspaces(void)
{
    return d->creators.keys();
}

/**
 * @brief allocates the memory for a medAbstractWorkspace.
 * @param type identifier for the Workspace type.
 * @param parent the parentWidget for all the Widget created in the workspace, even if the workspace is not a widget, its children can be destroyed by the qobject hierarchy.
 */
medAbstractWorkspace *medWorkspaceFactory::createWorkspace(QString type,QWidget* parent)
{
    if(!d->creators.contains(type))
        return NULL;

    medAbstractWorkspace * workspace = d->creators[type]->creator(parent);

    return workspace;
}

/**
 * @brief Gives the details of all workspaces.
 *
 */
QHash<QString, medWorkspaceFactory::Details *> medWorkspaceFactory::workspaceDetails() const
{
    return d->creators;
}


bool wsDetailsSortByName(const medWorkspaceFactory::Details* a, const medWorkspaceFactory::Details* b) {
    return a->name < b->name;
}

QList<medWorkspaceFactory::Details *> medWorkspaceFactory::workspaceDetailsSortedByName() const
{
    QHash<QString,medWorkspaceFactory::Details*> details = this->workspaceDetails();
    QList<medWorkspaceFactory::Details*> detailsList = details.values();
    qSort(detailsList.begin(),detailsList.end(), wsDetailsSortByName);
    return detailsList;
}

medWorkspaceFactory::medWorkspaceFactory(void) : dtkAbstractFactory(), d(new medWorkspaceFactoryPrivate)
{

}

medWorkspaceFactory::~medWorkspaceFactory(void)
{
    qDeleteAll(d->creators);
    delete d;
    d = NULL;
}

/**
 * @brief Gives the details of one workspace.
 *
 */
medWorkspaceFactory::Details * medWorkspaceFactory::workspaceDetailsFromId(QString identifier) const
{
    return d->creators.value(identifier);
}

medWorkspaceFactory *medWorkspaceFactory::s_instance = NULL;



bool medWorkspaceFactory::isUsable(QString identifier) const
{
    if (d->creators.contains(identifier))
    {
        if (d->creators.value(identifier)->isUsable==NULL)
            return true;
        else
            return d->creators.value(identifier)->isUsable();
    }
    
    return false;
}
