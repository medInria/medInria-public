/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTabbedViewContainers.h>

#include <QtCore>
#include <QShortcut>

#include <medRootContainer.h>
#include <medViewContainer.h>
#include <medViewContainerSplitter.h>
#include <medViewContainerManager.h>
#include <medAbstractWorkspaceLegacy.h>
#include <medAbstractView.h>
#include <medAbstractNavigator.h>
#include <medAbstractViewNavigator.h>
#include <medAbstractParameterL.h>
#include <medParameterPoolL.h>
#include <medStringListParameterL.h>

/**
 * @brief Private class that contains data members for medTabbedView Containers
 */
class medTabbedViewContainersPrivate
{
public:
    QShortcut *closeShortcut;
    QPushButton *addTabButton;
    QHash <int, QList<QUuid> > containerSelectedForTabIndex;
    medParameterPoolL *pool;
    medAbstractWorkspaceLegacy * owningWorkspace;
    bool bKeepLeastOne;
    int currentIdx;
};

medTabbedViewContainers::medTabbedViewContainers(medAbstractWorkspaceLegacy* owningWorkspace, QWidget *parent) : QTabWidget(parent), d(new medTabbedViewContainersPrivate)
{
    d->bKeepLeastOne = false;

    d->owningWorkspace = owningWorkspace;
    this->setTabsClosable(true);
    this->setMovable(true);

    d->addTabButton = new QPushButton(this);
    d->addTabButton->setIcon(QIcon::fromTheme("plus"));
    d->addTabButton->setToolTip("Add a tab");
    d->addTabButton->setShortcut(Qt::ControlModifier + Qt::Key_T);
    this->setCornerWidget(d->addTabButton);

    d->closeShortcut = new QShortcut(this);
    d->closeShortcut->setKey(Qt::ControlModifier + Qt::Key_W);
    
    // Connect for creation, removal and move of tabs
    connect(d->addTabButton, SIGNAL(clicked()),    this, SLOT(addContainerInTabUnNamed()));
    connect(this, SIGNAL(tabCloseRequested(int)),  this, SLOT(closeTab(int)));
    connect(d->closeShortcut, SIGNAL(activated()), this, SLOT(closeCurrentTab()));
    connect(tabBar(), SIGNAL(tabMoved(int,int)), this, SLOT(movedTabs(int,int)));

    // Connect group of view handling
    connect(&medViewContainerManager::instance(), SIGNAL(containerAboutToBeDestroyed(QUuid)), this, SLOT(removeContainerFromSelection(QUuid)));
    connect(this, SIGNAL(containersSelectedChanged()), this, SLOT(buildTemporaryPool()));
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(connectContainerSelectedForCurrentTab()));

    d->pool = new medParameterPoolL(this);
}

medTabbedViewContainers::~medTabbedViewContainers(void)
{
    delete d;
    d = nullptr;
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

void medTabbedViewContainers::hideTabBar()
{
    QTabBar *tabBar = this->tabBar();
    tabBar->hide();
}

void medTabbedViewContainers::adjustContainersSize()
{
    medViewContainerSplitter *splitter = static_cast<medRootContainer*>(this->widget(this->currentIndex()))->getSplitter();
    if (splitter)
    {
        splitter->adjustContainersSize();
    }
}

QList <QUuid> medTabbedViewContainers::containersSelected()
{
    return d->containerSelectedForTabIndex.value(this->currentIndex());
}

medViewContainer* medTabbedViewContainers::getFirstSelectedContainer()
{
    QList<QUuid> listUuid = containersSelected();

    medViewContainer* view = nullptr;

    if (listUuid.count() > 0)
    {
        // only keep the first selected container
        view = medViewContainerManager::instance().container(listUuid[0]);
    }

    return view;
}

medAbstractView* medTabbedViewContainers::getFirstSelectedContainerView()
{
    medViewContainer* currentFirstContainer = getFirstSelectedContainer();

    medAbstractView* view = nullptr;

    if (currentFirstContainer)
    {
        view = currentFirstContainer->view();
    }
    return view;
}

QList<medAbstractView*> medTabbedViewContainers::viewsInTab(int index)
{
    QList<medAbstractView*> views;
    medRootContainer *root = dynamic_cast<medRootContainer*>(this->widget(index));
    if (!root)
        return views;

    medViewContainerSplitter *splitter = root->getSplitter();

    if (!splitter)
        return views;

    QList<medViewContainer*> containers = splitter->containers();
    for(medViewContainer *container : containers)
    {
        if(container->view())
        {
            views << container->view();
        }
    }

    return views;
}

QList<medViewContainer*> medTabbedViewContainers::containersInTab(int index)
{
    medRootContainer *root = dynamic_cast<medRootContainer*>(this->widget(index));
    if (!root)
        return QList<medViewContainer*>();

    medViewContainerSplitter *splitter = root->getSplitter();

    if (!splitter)
        return QList<medViewContainer*>();

    return splitter->containers();
}

void medTabbedViewContainers::setKeepLeastOne(bool pi_bVal)
{
    d->bKeepLeastOne = pi_bVal;
}

void medTabbedViewContainers::setSplitter(int index, medViewContainerSplitter *splitter)
{
    if (this->widget(index))
        this->closeTab(index);
    
    medRootContainer* poRootContainer = new medRootContainer(this);
    poRootContainer->replaceSplitter(splitter);

    int idx = this->insertTab(index, poRootContainer, QString("%0 %1").arg(d->owningWorkspace->name()).arg(count()));
    this->setCurrentIndex(idx);
    d->containerSelectedForTabIndex.insert(idx, QList<QUuid>());
    connect(splitter, SIGNAL(newContainer(QUuid)), this, SLOT(connectContainer(QUuid)), Qt::UniqueConnection);
    for(medViewContainer* container : splitter->containers())
    {
        this->connectContainer(container->uuid());
    }
}


// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC SLOTS
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

medViewContainer* medTabbedViewContainers::addContainerInTabUnNamed()
{
    return this->addContainerInTabNamed(QString("%0 %1").arg(d->owningWorkspace->name()).arg(count()+1));
}

medViewContainer* medTabbedViewContainers::addContainerInTabNamed(const QString &name)
{
    return this->insertNewTab(this->count(), name);
}

medViewContainer *medTabbedViewContainers::insertNewTab(int index, const QString &name)
{
    medRootContainer* poRootContainer = new medRootContainer(this);
    medViewContainerSplitter *splitter = new medViewContainerSplitter;
    poRootContainer->replaceSplitter(splitter);

    int idx = this->insertTab(index, poRootContainer, name);
    this->setCurrentIndex(idx);
    d->containerSelectedForTabIndex.insert(idx, QList<QUuid>());
    connect(splitter, SIGNAL(newContainer(QUuid)), this, SLOT(connectContainer(QUuid)), Qt::UniqueConnection);
    medViewContainer* container = new medViewContainer;
    splitter->addViewContainer(container);

    return container;
}

void medTabbedViewContainers::closeCurrentTab()
{
    this->closeTab(this->currentIndex());
}

void medTabbedViewContainers::closeTab(int index)
{
    //Remove widget of tab and delete it 
    QWidget *poTmp = widget(index);
    this->removeTab(index);
    poTmp->disconnect(this);
    medViewContainerSplitter* poMainSpliter = static_cast<medRootContainer*>(poTmp)->getSplitter();
    if (poMainSpliter)
    {
        poMainSpliter->disconnect(this);
    }
    poTmp->deleteLater();
    
    //Reset tab name if it follows the rule '<Workspace Name>###' is follows 
    for (int i = 0; i < this->count(); ++i)
    {
        if (this->tabText(i).isEmpty() || this->tabText(i).startsWith(d->owningWorkspace->name(), Qt::CaseSensitive))
        {
            this->setTabText(i, d->owningWorkspace->name() + " " + QString::number(i + 1));
        }
    }

    // If medTabbedViewContainers is empty and empty is not allowed, we recreate one tab
    if (d->bKeepLeastOne && (this->count() < 1))
    {
        this->addContainerInTabUnNamed();
    }
    else
    {
        // Update the remaining tab indexes
        d->containerSelectedForTabIndex.remove(index);
        QHash <int, QList<QUuid> > copyHash;
        for(auto currentTabIndex : d->containerSelectedForTabIndex.keys())
        {
            auto previousTab = d->containerSelectedForTabIndex.take(currentTabIndex);
            copyHash.insert(copyHash.count(), previousTab);
        }
        d->containerSelectedForTabIndex.clear();
        d->containerSelectedForTabIndex = copyHash;
    
        // Update the current tab to the last one
        setCurrentIndex(this->count()-1);
        emit containersSelectedChanged();
    }
}

void medTabbedViewContainers::tabBarDoubleClickedHandler(int index)
{
    if (index < 0)
    {
        addContainerInTabUnNamed();
    }
}

// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVET SLOTS
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void medTabbedViewContainers::disconnectTabFromSplitter(int index)
{
    Q_UNUSED(index);
    medViewContainerSplitter* splitter = dynamic_cast<medViewContainerSplitter*>(this->sender());
    if(!splitter)
        return;

    splitter->disconnect(this);
}



void medTabbedViewContainers::connectContainer(QUuid container)
{
    medViewContainer* cont = medViewContainerManager::instance().container(container);
    connect(cont, SIGNAL(containerSelected(QUuid)), this, SLOT(addContainerToSelection(QUuid)), Qt::UniqueConnection);
    connect(cont, SIGNAL(containerUnSelected(QUuid)), this, SLOT(removeContainerFromSelection(QUuid)), Qt::UniqueConnection);
    connect(cont, SIGNAL(maximized(QUuid,bool)), this, SLOT(minimizeOtherContainers(QUuid,bool)));
    cont->setSelected(true);
}



void medTabbedViewContainers::addContainerToSelection(QUuid container)
{
    QList<QUuid> containersSelected = d->containerSelectedForTabIndex.value(this->currentIndex());

    if(QApplication::keyboardModifiers() != Qt::ControlModifier)
    {
        for(QUuid uuid : containersSelected)
        {
            medViewContainerManager::instance().container(uuid)->setSelected(false);
        }
    }
    // containersSelected may have been modified in removeContainerFromSelection so
    // we have to get it back again
    containersSelected = d->containerSelectedForTabIndex.value(this->currentIndex());
    containersSelected.append(container);
    medViewContainer *newSelectedContainer =  medViewContainerManager::instance().container(container);
    d->containerSelectedForTabIndex.insert(this->currentIndex(), containersSelected);

    connect(newSelectedContainer, SIGNAL(viewRemoved()),        this, SIGNAL(containersSelectedChanged()), Qt::UniqueConnection);
    connect(newSelectedContainer, SIGNAL(viewContentChanged()), this, SIGNAL(containersSelectedChanged()), Qt::UniqueConnection);

    emit containersSelectedChanged();
}

void medTabbedViewContainers::removeContainerFromSelection(QUuid container)
{
    for(QList<QUuid> containersSelected : d->containerSelectedForTabIndex.values())
    {
        int tabIndex = d->containerSelectedForTabIndex.key(containersSelected);
        if(containersSelected.removeOne(container))
        {
            d->containerSelectedForTabIndex.insert(tabIndex, containersSelected);
            medViewContainer *unSelectedContainer =  medViewContainerManager::instance().container(container);
            this->disconnect(unSelectedContainer, SIGNAL(viewRemoved()), this, 0);
            this->disconnect(unSelectedContainer, SIGNAL(viewContentChanged()), this, 0);
            emit containersSelectedChanged();
            break;
        }
    }
}

void medTabbedViewContainers::buildTemporaryPool()
{
    d->pool->clear();
    for(QUuid uuid : this->containersSelected())
    {
        medViewContainer *container = medViewContainerManager::instance().container(uuid);

        if(!container->view())
        {
            continue;
        }

        for(medAbstractParameterL *param : container->view()->linkableParameters())
        {
            d->pool->append(param);
        }
    }
}

void medTabbedViewContainers::connectContainerSelectedForCurrentTab()
{
    for(QList<QUuid> containersSelected : d->containerSelectedForTabIndex.values())
    {
        int tabIndex = d->containerSelectedForTabIndex.key(containersSelected);
        if(tabIndex == this->currentIndex())
        {
            for(QUuid uuid : containersSelected)
            {
                medViewContainer *container =  medViewContainerManager::instance().container(uuid);
                connect(container, SIGNAL(viewRemoved()),        this, SIGNAL(containersSelectedChanged()), Qt::UniqueConnection);
                connect(container, SIGNAL(viewContentChanged()), this, SIGNAL(containersSelectedChanged()), Qt::UniqueConnection);
            }
        }
        else
        {
            for(QUuid uuid : containersSelected)
            {
                medViewContainer *container =  medViewContainerManager::instance().container(uuid);
                this->disconnect(container, SIGNAL(viewRemoved()), this, 0);
                this->disconnect(container, SIGNAL(viewContentChanged()), this, 0);
            }
        }
    }
    emit containersSelectedChanged();
}

void medTabbedViewContainers::minimizeOtherContainers(QUuid containerMaximized, bool maximized)
{
    medViewContainerSplitter *splitter = static_cast<medRootContainer*>(this->widget(this->currentIndex()))->getSplitter();

    if(!splitter)
        return;

    // hide/show all other containers and their splitter
    minimizeSplitterContainers(containerMaximized, maximized, splitter);

    if(maximized)
    {
        medViewContainer *container = medViewContainerManager::instance().container(containerMaximized);

        QWidget *parent = container->parentWidget();

        //iterates through parents to restablish splitter visibility (hidden by minimizeSplitterContainers)
        while(parent)
        {
            parent->show();
            parent = parent->parentWidget();
        }
    }
}

// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVET
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void medTabbedViewContainers::minimizeSplitterContainers(QUuid containerMaximized, bool maximized, medViewContainerSplitter *splitter)
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
                {
                    container->hide();
                }
                else
                {
                    container->show();
                }
            }
        }
    }

    if(maximized)
    {
        splitter->hide();
    }
    else
    {
        splitter->show();
    }
}

/**
 * @brief Launched when a tab from the tabs bar is moved to a new position
 * 
 * @param from index tab before
 * @param to index tab after
 */
void medTabbedViewContainers::movedTabs(int from, int to)
{
    auto previousFrom = d->containerSelectedForTabIndex.take(from);
    auto previousTo   = d->containerSelectedForTabIndex.take(to);
    d->containerSelectedForTabIndex.insert(from, previousTo);
    d->containerSelectedForTabIndex.insert(to,   previousFrom);

    emit containersSelectedChanged();
}