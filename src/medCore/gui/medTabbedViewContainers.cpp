/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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
#include <medAbstractWorkspace.h>
#include <medAbstractView.h>
#include <medAbstractNavigator.h>
#include <medAbstractViewNavigator.h>
#include <medAbstractParameter.h>
#include <medParameterPool.h>
#include <medStringListParameter.h>


class medTabbedViewContainersPrivate
{
public:
    QShortcut *closeShortcut;
    QPushButton *addTabButton;
    QHash <int, QList<QUuid> > containerSelectedForTabIndex;
    medParameterPool *pool;
    medAbstractWorkspace * owningWorkspace;

    int currentIdx;
};

medTabbedViewContainers::medTabbedViewContainers(medAbstractWorkspace* owningWorkspace, QWidget *parent)
    : QTabWidget(parent)
    , d(new medTabbedViewContainersPrivate)
{
    d->owningWorkspace = owningWorkspace;
    this->setTabsClosable(true);
    this->setMovable(true);



    d->addTabButton = new QPushButton(this);
    d->addTabButton->setObjectName("addTabButton");
    d->addTabButton->setShortcut(Qt::ControlModifier + Qt::Key_T);
    this->setCornerWidget(d->addTabButton);

    d->closeShortcut = new QShortcut(this);
    d->closeShortcut->setKey(Qt::ControlModifier + Qt::Key_W);

    connect(d->addTabButton,SIGNAL(clicked()), this, SLOT(addContainerInTab()));
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(disconnectTabFromSplitter(int)));
    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
    connect(d->closeShortcut,SIGNAL(activated()),this,SLOT(closeCurrentTab()));
    connect(medViewContainerManager::instance(), SIGNAL(containerAboutToBeDestroyed(QUuid)), this, SLOT(removeContainerFromSelection(QUuid)));
    connect(this, SIGNAL(containersSelectedChanged()), this, SLOT(buildTemporaryPool()));
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(connectContainerSelectedForCurrentTab()));

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
    d->closeShortcut->blockSignals(true);
    d->addTabButton->blockSignals(true);
}

void medTabbedViewContainers::unlockTabs()
{
    this->setTabsClosable(true);
    this->setMovable(true);
    d->addTabButton->show();
    d->closeShortcut->blockSignals(false);
    d->addTabButton->blockSignals(false);
}

void medTabbedViewContainers::closeCurrentTab()
{
    this->closeTab(this->currentIndex());
}

void medTabbedViewContainers::closeTab(int index)
{
    // Repopulate would be called because of the destruction of the root
    // splitter after closing all the container in it.
    // We don't want to repopulate the tab, we want to close it.
    this->widget(index)->disconnect(this, SLOT(repopulateCurrentTab()));

    // However because of the deleteLater in medViewContainer::closeEvent
    // we don't really know when the splitter will be destroyed and when (this->count() < 1) will be true
    // so we can't call directly resetTabState, we need another SIGNAL/SLOT connection for that
    connect(this->widget(index), SIGNAL(destroyed()), this, SLOT(resetTabState()));

    foreach(medViewContainer* container, containersInTab(index))
        container->close();
}

void medTabbedViewContainers::resetTabState()
{
    for(int i = 0; i < this->count(); ++i)
        if(this->tabText(i).isEmpty() || this->tabText(i).startsWith("Tab", Qt::CaseSensitive))
            this->setTabText(i, "Tab" +  QString::number(i+1));

    if(this->count() < 1)
        this->addContainerInTab();
}

medViewContainer* medTabbedViewContainers::addContainerInTab()
{
    if (this->count())
        return this->addContainerInTab(QString("%0 %1").arg(d->owningWorkspace->name()).arg(count()));
    else
        return this->addContainerInTab(QString("%0").arg(d->owningWorkspace->name()));
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
    connect(splitter, SIGNAL(aboutTobedestroyed()), this, SLOT(repopulateCurrentTab()));
    connect(splitter, SIGNAL(newContainer(QUuid)), this, SLOT(connectContainer(QUuid)), Qt::UniqueConnection);
    medViewContainer* container = new medViewContainer;
    splitter->addViewContainer(container);

    return container;
}

void medTabbedViewContainers::setSplitter(int index, medViewContainerSplitter *splitter)
{
    int idx = this->insertTab(index, splitter, QString("%0 %1").arg(d->owningWorkspace->name()).arg(count()));
    this->setCurrentIndex(idx);
    d->containerSelectedForTabIndex.insert(idx, QList<QUuid>());
    connect(splitter, SIGNAL(aboutTobedestroyed()), this, SLOT(repopulateCurrentTab()));
    connect(splitter, SIGNAL(newContainer(QUuid)), this, SLOT(connectContainer(QUuid)), Qt::UniqueConnection);
    foreach(medViewContainer* container, splitter->containers())
    {
        this->connectContainer(container->uuid());
    }
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

    connect(cont, SIGNAL(maximized(QUuid,bool)), this, SLOT(minimizeOtherContainers(QUuid,bool)));

    cont->setSelected(true);
}

void medTabbedViewContainers::repopulateCurrentTab()
{
    if(this->count() < 1)
        this->addContainerInTab();

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

    connect(newSelectedContainer, SIGNAL(viewRemoved()), this, SIGNAL(containersSelectedChanged()), Qt::UniqueConnection);
    connect(newSelectedContainer, SIGNAL(viewContentChanged()), this, SIGNAL(containersSelectedChanged()), Qt::UniqueConnection);

    emit containersSelectedChanged();
}

void medTabbedViewContainers::removeContainerFromSelection(QUuid container)
{
    foreach(QList<QUuid> containersSelected, d->containerSelectedForTabIndex.values())
    {
        int tabIndex = d->containerSelectedForTabIndex.key(containersSelected);
        if(containersSelected.removeOne(container))
        {
            d->containerSelectedForTabIndex.insert(tabIndex, containersSelected);
            medViewContainer *unSelectedContainer =  medViewContainerManager::instance()->container(container);
            this->disconnect(unSelectedContainer, SIGNAL(viewRemoved()), this, 0);
            this->disconnect(unSelectedContainer, SIGNAL(viewContentChanged()), this, 0);
            emit containersSelectedChanged();
            break;
        }
    }
}

void medTabbedViewContainers::connectContainerSelectedForCurrentTab()
{
    foreach(QList<QUuid> containersSelected, d->containerSelectedForTabIndex.values())
    {
        int tabIndex = d->containerSelectedForTabIndex.key(containersSelected);
        if(tabIndex == this->currentIndex())
        {
            foreach (QUuid uuid, containersSelected)
            {
                medViewContainer *container =  medViewContainerManager::instance()->container(uuid);
                connect(container, SIGNAL(viewRemoved()), this, SIGNAL(containersSelectedChanged()), Qt::UniqueConnection);
                connect(container, SIGNAL(viewContentChanged()), this, SIGNAL(containersSelectedChanged()), Qt::UniqueConnection);
            }
        }
        else
        {
            foreach (QUuid uuid, containersSelected)
            {
                medViewContainer *container =  medViewContainerManager::instance()->container(uuid);
                this->disconnect(container, SIGNAL(viewRemoved()), this, 0);
                this->disconnect(container, SIGNAL(viewContentChanged()), this, 0);
            }
        }
    }
    emit containersSelectedChanged();
}


void medTabbedViewContainers::buildTemporaryPool()
{
    d->pool->clear();
    foreach(QUuid uuid, this->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance()->container(uuid);

        if(!container->view())
            continue;

        foreach(medAbstractParameter *param, container->view()->linkableParameters())
        {
            d->pool->append(param);
        }
    }
}

void medTabbedViewContainers::minimizeOtherContainers(QUuid containerMaximized, bool maximized)
{
    medViewContainerSplitter *splitter = dynamic_cast<medViewContainerSplitter*>(this->widget(this->currentIndex()));

    if(!splitter)
        return;

    // hide/show all other containers and their splitter
    minimizeSplitterContainers(containerMaximized, maximized, splitter);

    if(maximized)
    {
        medViewContainer *container = medViewContainerManager::instance()->container(containerMaximized);
        //container->show();

        QWidget *parent = container->parentWidget();

        //iterates through parents to restablish splitter visibility (hidden by minimizeSplitterContainers)
        while(parent)
        {
            parent->show();
            parent = parent->parentWidget();
        }
    }
}

void medTabbedViewContainers::minimizeSplitterContainers(QUuid containerMaximized, bool maximized,
                                                         medViewContainerSplitter *splitter)
{
    for(int i=0; i<splitter->count(); i++)
    {
        if(medViewContainerSplitter *nestedSplitter = dynamic_cast<medViewContainerSplitter*>(splitter->widget(i)))
        {
            minimizeSplitterContainers(containerMaximized, maximized, nestedSplitter);
        }
        else if(medViewContainer *container = dynamic_cast<medViewContainer*>(splitter->widget(i)))
        {
            if(container->uuid() != containerMaximized)
            {
                if(maximized)
                    container->hide();
                else container->show();
            }
        }
    }

    if(maximized)
        splitter->hide();
    else splitter->show();
}

void medTabbedViewContainers::adjustContainersSize()
{
    medViewContainerSplitter *splitter = dynamic_cast<medViewContainerSplitter*>(this->widget(this->currentIndex()));
    splitter->adjustContainersSize();
}

QList<medAbstractView*> medTabbedViewContainers::viewsInTab(int index)
{
    QList<medAbstractView*> views;
    medViewContainerSplitter *splitter = dynamic_cast<medViewContainerSplitter*>(this->widget(index));

    if(!splitter)
        return views;

    QList<medViewContainer*> containers = splitter->containers();
    foreach(medViewContainer *container, containers)
    {
        views << container->view();
    }

    return views;
}

QList<medViewContainer*> medTabbedViewContainers::containersInTab(int index)
{
    medViewContainerSplitter *splitter = dynamic_cast<medViewContainerSplitter*>(this->widget(index));

    if(!splitter)
        return QList<medViewContainer*>();

    return splitter->containers();
}
