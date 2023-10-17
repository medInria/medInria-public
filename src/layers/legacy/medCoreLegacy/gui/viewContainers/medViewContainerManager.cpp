/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewContainerManager.h>

#include <QUuid>
#include <QHash>

#include <medViewContainer.h>


class medViewContainerManagerPrivate
{
public:
    QHash<QUuid, medViewContainer*> containers;
};

std::unique_ptr<medViewContainerManager> medViewContainerManager::s_instance = nullptr;

medViewContainerManager &medViewContainerManager::instance()
{
    if(!s_instance)
    {
        s_instance = std::unique_ptr<medViewContainerManager>(new medViewContainerManager());
    }
    return *s_instance.get();
}

medViewContainerManager::medViewContainerManager(void) : d(new medViewContainerManagerPrivate)
{
}

medViewContainerManager::~medViewContainerManager()
{
    delete d;
    d = nullptr;
}

void medViewContainerManager::registerNewContainer(medViewContainer* newContainer)
{
    d->containers.insert(newContainer->uuid(), newContainer);
}

void medViewContainerManager::unregisterContainer(medViewContainer *container)
{
    QUuid uuid = container->uuid();
    emit containerAboutToBeDestroyed(uuid);
    d->containers.remove(uuid);
}

medViewContainer* medViewContainerManager::container(QUuid uuid) const
{
    return d->containers.value(uuid);
}
