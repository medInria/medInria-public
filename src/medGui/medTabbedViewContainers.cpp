/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTabbedViewContainers.h>

#include <QtCore>
#include <QShortcut>

#include <medViewContainer.h>
#include <medViewContainerSplitter.h>
#include <medViewContainerManager.h>
#include <medAbstractView.h>
#include <medAbstractParameter.h>
#include <medParameterPoolManager.h>
#include <medParameterPool.h>


class medTabbedViewContainersPrivate
{
public:
    QShortcut *closeShortcut;    
    QPushButton *addTabButton;
    QHash <int, QList<QUuid> > containerSelectedForTabIndex;
    medParameterPool *pool;
};

medTabbedViewContainers::medTabbedViewContainers(QWidget *parent) : QTabWidget(parent), d(new medTabbedViewContainersPrivate)
{
    this->setTabsClosable(true);
    this->setMovable(true);

    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(resetTab(int)));

    d->addTabButton = new QPushButton(this);
    d->addTabButton->setObjectName("addTabButton");
    d->addTabButton->setShortcut(Qt::ControlModifier + Qt::Key_T);
    this->setCornerWidget(d->addTabButton);
 
    connect(d->addTabButton,SIGNAL(clicked()), this, SLOT(addContainerInTab()));
    
    d->closeShortcut = new QShortcut(this);
    d->closeShortcut->setKey(Qt::ControlModifier + Qt::Key_W);
    connect(d->closeShortcut,SIGNAL(activated()),this,SLOT(resetCurrentTab()));

    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(disconnectTabFromSplitter(int)));

    connect(medViewContainerManager::instance(), SIGNAL(containerAboutToBeDestroyed(QUuid)), this, SLOT(removeContainerFromSelection(QUuid)));

    connect(this, SIGNAL(containersSelectedChanged()), this, SLOT(buildTemporaryPool()));

    d->pool = new medParameterPool(this);
}

medTabbedViewContainers::~medTabbedViewContainers(void)
{
    delete d;
    d = NULL;
}

QList <QUuid> medTabbedViewContainers::containersSelected()
{
    return d->containerSelectedForTabIndex.value(this->currentIndex());
}

void medTabbedViewContainers::lockTabs()
{
    this->setTabsClosable(false);
    this->setMovable(false);
    d->addTabButton->hide();
}

void medTabbedViewContainers::unlockTabs()
{
    this->setTabsClosable(true);
    this->setMovable(true);
    d->addTabButton->show();
}

void medTabbedViewContainers::resetCurrentTab()
{
    int index = this->currentIndex();
    this->resetTab(index);
}

void medTabbedViewContainers::resetTab(int index)
{
    this->removeTab(index);
    if(this->count() < 1)
        this->addContainerInTab();
}

medViewContainer* medTabbedViewContainers::addContainerInTab()
{
    return this->addContainerInTab("TODO find fancy name - RDE");
}

medViewContainer* medTabbedViewContainers::addContainerInTab(const QString &name)
{
    return this->insertContainerInTab(this->count(), name);
}

medViewContainer *medTabbedViewContainers::insertContainerInTab(int index, const QString &name)
{
    medViewContainerSplitter *splitter  = new medViewContainerSplitter;
    int idx = this->insertTab(index, splitter, name);
    this->setCurrentIndex(idx);
    d->containerSelectedForTabIndex.insert(idx, QList<QUuid>());
    connect(splitter, SIGNAL(destroyed()), this, SLOT(repopulateCurrentTab()));
    connect(splitter, SIGNAL(newContainer(QUuid)), this, SLOT(connectContainer(QUuid)), Qt::UniqueConnection);
    medViewContainer* container = new medViewContainer;
    splitter->addViewContainer(container);

    return container;
}

void medTabbedViewContainers::hideTabBar()
{
    QTabBar *tabBar = this->tabBar();
    tabBar->hide();
}

void medTabbedViewContainers::connectContainer(QUuid container)
{
    medViewContainer* cont = medViewContainerManager::instance()->container(container);
    connect(cont, SIGNAL(containerSelected(QUuid)), this, SLOT(addContainerToSelection(QUuid)), Qt::UniqueConnection);
    connect(cont, SIGNAL(containerUnSelected(QUuid)), this, SLOT(removeContainerFromSelection(QUuid)), Qt::UniqueConnection);

    connect(cont, SIGNAL(linkRequested(QUuid, QString)), this, SLOT(link(QUuid, QString)));
    connect(cont, SIGNAL(unlinkRequested(QUuid)), this, SLOT(unlink(QUuid)));


    cont->setSelected(true);
}

void medTabbedViewContainers::repopulateCurrentTab()
{
    int idx = this->currentIndex();
    QString tabText = this->tabText(idx);
    this->removeTab(idx);
    this->insertContainerInTab(idx, tabText);
}

void medTabbedViewContainers::disconnectTabFromSplitter(int index)
{
    medViewContainerSplitter* splitter = dynamic_cast<medViewContainerSplitter*>(this->sender());
    if(!splitter)
        return;

    splitter->disconnect(this);
}

void medTabbedViewContainers::addContainerToSelection(QUuid container)
{
    QList<QUuid> containersSelected = d->containerSelectedForTabIndex.value(this->currentIndex());
    if(QApplication::keyboardModifiers() != Qt::ControlModifier)
    {
        foreach (QUuid uuid, containersSelected)
            medViewContainerManager::instance()->container(uuid)->setSelected(false);
    }
    // containersSelected may have been mofified in removeContainerFromSelection so
    // we have to get it back again
    containersSelected = d->containerSelectedForTabIndex.value(this->currentIndex());
    containersSelected.append(container);
    medViewContainer *newSelectedContainer =  medViewContainerManager::instance()->container(container);
    d->containerSelectedForTabIndex.insert(this->currentIndex(), containersSelected);

    emit containersSelectedChanged();
    connect(newSelectedContainer, SIGNAL(currentLayerChanged()), this, SIGNAL(currentLayerChanged()), Qt::UniqueConnection);
    connect(newSelectedContainer, SIGNAL(viewRemoved()), this, SIGNAL(containersSelectedChanged()), Qt::UniqueConnection);
    connect(newSelectedContainer, SIGNAL(viewContentChanged()), this, SIGNAL(containersSelectedChanged()), Qt::UniqueConnection);
}

void medTabbedViewContainers::removeContainerFromSelection(QUuid container)
{
    foreach(QList<QUuid> containersSelected, d->containerSelectedForTabIndex.values())
    {
        if(containersSelected.removeOne(container))
        {
            d->containerSelectedForTabIndex.insert(d->containerSelectedForTabIndex.key(containersSelected), containersSelected);
            medViewContainer *unSelectedContainer =  medViewContainerManager::instance()->container(container);
            this->disconnect(unSelectedContainer, SIGNAL(currentLayerChanged()), this, 0);
            this->disconnect(unSelectedContainer, SIGNAL(viewRemoved()), this, 0);
            this->disconnect(unSelectedContainer, SIGNAL(viewContentChanged()), this, 0);
            emit containersSelectedChanged();
            break;
        }
    }
}

void medTabbedViewContainers::link(QUuid uuid, QString pool)
{
    if(this->containersSelected().contains(uuid))
    {
        foreach(QUuid uuidSelected, this->containersSelected())
        {
            medViewContainer *container = medViewContainerManager::instance()->container(uuidSelected);
            container->link(pool);
        }
    }
    else
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        container->link(pool);
    }
}

void medTabbedViewContainers::unlink(QUuid uuid)
{
    if(this->containersSelected().contains(uuid))
    {
        foreach(QUuid uuidSelected, this->containersSelected())
        {
            medViewContainer *container = medViewContainerManager::instance()->container(uuidSelected);
            container->unlink();
        }
    }
    else
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);
        container->unlink();
    }
}

void medTabbedViewContainers::buildTemporaryPool()
{
    d->pool->clear();
    foreach(QUuid uuid, this->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);

        if(!container->view())
            continue;

        foreach(medAbstractParameter *param, container->view()->navigatorsParameters())
        {
            d->pool->append(param);
        }
    }
}
