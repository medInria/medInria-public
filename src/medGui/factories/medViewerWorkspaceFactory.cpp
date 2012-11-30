#include "medViewerWorkspaceFactory.h"

#include <medToolBoxFactory.h>
#include "medViewerWorkspace.h"
#include "medViewerWorkspaceFactory.h"

class medViewerWorkspaceFactoryPrivate
{
public:
    typedef QHash<QString, medViewerWorkspaceDetails*> medViewerWorkspaceCreatorHash;
    medViewerWorkspaceCreatorHash creators;
};

medViewerWorkspaceFactory *medViewerWorkspaceFactory::instance(void)
{
    if(!s_instance)
        s_instance = new medViewerWorkspaceFactory;

    return s_instance;
}

bool medViewerWorkspaceFactory::registerWorkspace(QString identifier,
                                                          QString name,
                                                          QString description,
                                                          medViewerWorkspaceCreator creator,
                                                          medViewerWorkspaceIsUsable isUsable)
{
    if(!d->creators.contains(identifier))
    {
        medViewerWorkspaceDetails* holder = new medViewerWorkspaceDetails
                (identifier,
                 name,
                 description,
                 creator,
                 isUsable);

        d->creators.insert( identifier,
                            holder);
        return true;
    }
    return false;
}

QList<QString> medViewerWorkspaceFactory::workspaces(void)
{
    return d->creators.keys();
}

medViewerWorkspace *medViewerWorkspaceFactory::createWorkspace(QString type,QWidget* parent)
{
    if(!d->creators.contains(type))
        return NULL;

    medViewerWorkspace * workspace = d->creators[type]->creator(parent);

    return workspace;
}

QHash<QString, medViewerWorkspaceDetails *> medViewerWorkspaceFactory::workspaceDetails() const
{
    return d->creators;
}

medViewerWorkspaceFactory::medViewerWorkspaceFactory(void) : dtkAbstractFactory(), d(new medViewerWorkspaceFactoryPrivate)
{

}

medViewerWorkspaceFactory::~medViewerWorkspaceFactory(void)
{
    foreach (medViewerWorkspaceDetails * detail, d->creators.values())
    {
        delete detail;
        detail = NULL;
    }

    delete d;

    d = NULL;
}


medViewerWorkspaceDetails * medViewerWorkspaceFactory::workspaceDetailsFromId(QString identifier) const
{
    return d->creators.value(identifier);
}

medViewerWorkspaceFactory *medViewerWorkspaceFactory::s_instance = NULL;



bool medViewerWorkspaceFactory::isUsable(QString identifier) const
{
    if (d->creators.value(identifier))
        return d->creators.value(identifier)->isUsable();
    
    return false;
}