/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractWorkspaceLegacy.h>
#include <medToolBoxFactory.h>
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
                                            QString category,
                                            medWorkspaceCreator creator,
                                            medWorkspaceIsUsable isUsable,
                                            bool isActive)
{
    if(!d->creators.contains(identifier))
    {
        medWorkspaceFactory::Details* holder = new medWorkspaceFactory::Details(identifier, name, description, category, creator, isUsable, isActive);
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
 * @brief allocates the memory for a medAbstractWorkspaceLegacy.
 * @param type identifier for the Workspace type.
 * @param parent the parentWidget for all the Widget created in the workspace, even if the workspace is not a widget, its children can be destroyed by the qobject hierarchy.
 */
medAbstractWorkspaceLegacy *medWorkspaceFactory::createWorkspace(QString type,QWidget* parent)
{
    if(!d->creators.contains(type))
    {
        return nullptr;
    }
    medAbstractWorkspaceLegacy * workspace = d->creators[type]->creator(parent);

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

QList<medWorkspaceFactory::Details *> medWorkspaceFactory::workspaceDetailsSortedByName(bool activeOnly) const
{
    QHash<QString,medWorkspaceFactory::Details*> details = this->workspaceDetails();
    QHash<QString, medWorkspaceFactory::Details*> filteredDetails;

    if (activeOnly)
    {
        QHash<QString, medWorkspaceFactory::Details*>::iterator iter = details.begin();

        while (iter != details.end())
        {
            QString& identifier = (*iter)->identifier;
            medWorkspaceFactory::Details* details = (*iter);
            if ((*iter)->isActive)
                filteredDetails.insert(identifier, details);

            ++iter;
        }
    }
    else
        filteredDetails = details;

    QList<medWorkspaceFactory::Details*> detailsList = filteredDetails.values();
    std::sort(detailsList.begin(),detailsList.end(), wsDetailsSortByName);

    return detailsList;
}

medWorkspaceFactory::medWorkspaceFactory(void) : dtkAbstractFactory(), d(new medWorkspaceFactoryPrivate)
{

}

medWorkspaceFactory::~medWorkspaceFactory(void)
{
    qDeleteAll(d->creators);
    delete d;
    d = nullptr;
}

/**
 * @brief Gives the details of one workspace.
 *
 */
medWorkspaceFactory::Details * medWorkspaceFactory::workspaceDetailsFromId(QString identifier) const
{
    return d->creators.value(identifier);
}

medWorkspaceFactory *medWorkspaceFactory::s_instance = nullptr;



bool medWorkspaceFactory::isUsable(QString identifier) const
{
    if (d->creators.contains(identifier))
    {
        if (d->creators.value(identifier)->isUsable==nullptr)
            return true;
        else
            return d->creators.value(identifier)->isUsable();
    }
    
    return false;
}


/**
 * Get a list of the available workspaces from a specific category.
 *
 */
QList<QString> medWorkspaceFactory::workspacesFromCategory(
        const QString& category)const
{
    QList<QString> ids;
    typedef medWorkspaceFactoryPrivate::medWorkspaceCreatorHash::iterator creator_iterator;
    creator_iterator i = d->creators.begin();
    while (i != d->creators.end())
    {
        if (!i.value()->category.compare(category))
        {
            ids << i.key();
        }
        ++i;
    }
    return ids;
}
