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


class medTabbedViewContainersPrivate
{
public:
    QShortcut *closeShortcut;    
    QPushButton *addTabButton;
    QHash <int, QList<QUuid> > containerSelectedForTabIndex;
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

void medTabbedViewContainers::addContainerInTab()
{
    this->addContainerInTab("TODO find fancy name - RDE");
}

void medTabbedViewContainers::addContainerInTab(const QString &name)
{
    this->insertContainerInTab(this->count(), name);
}

void medTabbedViewContainers::insertContainerInTab(int index, const QString &name)
{
    qDebug() << "insert new Tab wit empty container";
    medViewContainerSplitter *splitter  = new medViewContainerSplitter;
    int idx = this->insertTab(index, splitter, name);
    this->setCurrentIndex(idx);
    d->containerSelectedForTabIndex.insert(idx, QList<QUuid>());
    connect(splitter, SIGNAL(destroyed()), this, SLOT(repopulateCurrentTab()));
    connect(splitter, SIGNAL(newContainer(QUuid)), this, SLOT(connectContainer(QUuid)));
    medViewContainer* container = new medViewContainer;
    splitter->addViewContainer(container);
    container->setSelected(true);
}

void medTabbedViewContainers::hideTabBar()
{
    QTabBar *tabBar = this->tabBar();
    tabBar->hide();
}

void medTabbedViewContainers::connectContainer(QUuid container)
{
    qDebug() << "connectContainer !";
    medViewContainer* cont = medViewContainerManager::instance()->container(container);
    connect(cont, SIGNAL(containerSelected(QUuid)), this, SLOT(addContainerToSelection(QUuid)));
    connect(cont, SIGNAL(containerUnSelected(QUuid)), this, SLOT(removeContainerFromSelection(QUuid)));
    emit newContainer(container);
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
    qDebug() << "add to selection" << container;
    QList<QUuid> containersSelected = d->containerSelectedForTabIndex.value(this->currentIndex());
    containersSelected.append(container);
    d->containerSelectedForTabIndex.insert(this->currentIndex(), containersSelected);
    emit selectionChanged();
}

void medTabbedViewContainers::removeContainerFromSelection(QUuid container)
{
    qDebug() << "remove from selection" << container;
    QList<QUuid> containersSelected = d->containerSelectedForTabIndex.value(this->currentIndex());
    containersSelected.removeOne(container);
    d->containerSelectedForTabIndex.insert(this->currentIndex(), containersSelected);
    emit selectionChanged();
}
