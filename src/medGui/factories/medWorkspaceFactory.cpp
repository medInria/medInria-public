/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medWorkspaceFactory.h>

#include <medToolBoxFactory.h>
#include <medWorkspace.h>
#include <medWorkspaceFactory.h>

class medWorkspaceFactoryPrivate
{
public:
    typedef QHash<QString, medWorkspaceDetails*> medWorkspaceCreatorHash;
    medWorkspaceCreatorHash creators;
};

medWorkspaceFactory *medWorkspaceFactory::instance(void)
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
        medWorkspaceDetails* holder = new medWorkspaceDetails
                (name,
                 description,
                 creator,
                 isUsable);

        d->creators.insert( identifier,
                            holder);
        return true;
    }
    return false;
}

QList<QString> medWorkspaceFactory::workspaces(void)
{
    return d->creators.keys();
}

medWorkspace *medWorkspaceFactory::createWorkspace(QString type,QWidget* parent)
{
    if(!d->creators.contains(type))
        return NULL;

    medWorkspace * workspace = d->creators[type]->creator(parent);

    return workspace;
}

QHash<QString, medWorkspaceDetails *> medWorkspaceFactory::workspaceDetails() const
{
    return d->creators;
}

medWorkspaceFactory::medWorkspaceFactory(void) : dtkAbstractFactory(), d(new medWorkspaceFactoryPrivate)
{

}

medWorkspaceFactory::~medWorkspaceFactory(void)
{
    foreach (medWorkspaceDetails * detail, d->creators.values())
    {
        delete detail;
        detail = NULL;
    }

    delete d;

    d = NULL;
}


medWorkspaceDetails * medWorkspaceFactory::workspaceDetailsFromId(QString identifier) const
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
