/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTabbedViewContainers.h>

#include <QUuid>
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
    medViewContainerSplitter *splitter  = new medViewContainerSplitter;
    int idx = this->insertTab(index, splitter, name);
    this->setCurrentIndex(idx);
    connect(splitter, SIGNAL(destroyed()), this, SLOT(repopulateCurrentTab()));
    connect(splitter, SIGNAL(newContainer(QUuid&)), this, SIGNAL(newContainer(QUuid&)));
    connect(splitter, SIGNAL(newContainer(QUuid&)), this, SLOT(addContainerToContainerInTab(QUuid&)));
    splitter->addViewContainer(new medViewContainer);
}

void medTabbedViewContainers::hideTabBar()
{
    QTabBar *tabBar = this->tabBar();
    tabBar->hide();
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

