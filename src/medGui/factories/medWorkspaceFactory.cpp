#include "medWorkspaceFactory.h"

#include <medToolBoxFactory.h>
#include "medViewerWorkspace.h"
#include "medWorkspaceFactory.h"

class medWorkspaceFactoryPrivate
{
public:
    typedef QHash<QString, medViewerWorkspaceDetails*> medViewerWorkspaceCreatorHash;
    medViewerWorkspaceCreatorHash creators;
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
                                                          medViewerWorkspaceCreator creator,
                                                          medViewerWorkspaceIsUsable isUsable)
{
    if(!d->creators.contains(identifier))
    {
        medViewerWorkspaceDetails* holder = new medViewerWorkspaceDetails
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

medViewerWorkspace *medWorkspaceFactory::createWorkspace(QString type,QWidget* parent)
{
    if(!d->creators.contains(type))
        return NULL;

    medViewerWorkspace * workspace = d->creators[type]->creator(parent);

    return workspace;
}

QHash<QString, medViewerWorkspaceDetails *> medWorkspaceFactory::workspaceDetails() const
{
    return d->creators;
}

medWorkspaceFactory::medWorkspaceFactory(void) : dtkAbstractFactory(), d(new medWorkspaceFactoryPrivate)
{

}

medWorkspaceFactory::~medWorkspaceFactory(void)
{
    foreach (medViewerWorkspaceDetails * detail, d->creators.values())
    {
        delete detail;
        detail = NULL;
    }

    delete d;

    d = NULL;
}


medViewerWorkspaceDetails * medWorkspaceFactory::workspaceDetailsFromId(QString identifier) const
{
    return d->creators.value(identifier);
}

medWorkspaceFactory *medWorkspaceFactory::s_instance = NULL;



bool medViewerWorkspaceFactory::isUsable(QString identifier) const
{
    if (d->creators.contains(identifier))
        if (d->creators.value(identifier)->isUsable==NULL)
            return true;
        else
            return d->creators.value(identifier)->isUsable();
        
    return false;
}