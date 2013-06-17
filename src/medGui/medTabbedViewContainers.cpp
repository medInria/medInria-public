/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtCore>
#include <QShortcut>

#include "medTabbedViewContainers.h"

#include "medViewContainer.h"
#include "medCustomViewContainer.h"
#include "medMultiViewContainer.h"
#include "medSingleViewContainer.h"

class medTabbedViewContainersPrivate
{
public:
    QHash<QString, medViewContainer*> containers;
    QString currentName;
    QShortcut *closeShortcut;
    
    QPushButton *addButton;
};

medTabbedViewContainers::medTabbedViewContainers(QWidget *parent) : QTabWidget(parent), d(new medTabbedViewContainersPrivate)
{
    this->setTabsClosable(true);
    this->setMovable(true);

    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(deleteContainerClicked(int)));
    connect(this,SIGNAL(currentChanged(int)),this,SLOT(onCurrentContainerChanged(int)));

    d->addButton = new QPushButton(this);
    d->addButton->setStyleSheet("background-image: url(:medGui/pixmaps/plus_button.png);background-position: center;background-repeat: no-repeat;");
    d->addButton->setShortcut(Qt::ControlModifier + Qt::Key_T);
    this->setCornerWidget(d->addButton);

    connect(d->addButton,SIGNAL(clicked()),this,SIGNAL(addTabButtonClicked()));
    
    d->closeShortcut = new QShortcut(this);
    d->closeShortcut->setKey(Qt::ControlModifier + Qt::Key_W);
    connect(d->closeShortcut,SIGNAL(activated()),this,SLOT(deleteContainerShortcutActivated()));
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
    d->addButton->hide();
}

void medTabbedViewContainers::unlockTabs()
{
    this->setTabsClosable(true);
    this->setMovable(true);
    d->addButton->show();
}

/*
void medTabbedViewContainers::addNewTabContainer()
{
    // This slot should disappear, instead the creation signal should be sent to the parent who should call for the creation of a new tab
    QString name = "Tab ";
    name += QString::number(this->count());

    if (!this->container(name))
    {
        medViewContainer *new_container = new medMultiViewContainer();
        addContainer(name, new_container);
        this->setContainer(name);
    }
    else
        qDebug() << "Container" << name << "already exists in this workspaces";
}
*/

void medTabbedViewContainers::deleteContainerShortcutActivated()
{
    int index = this->currentIndex();
    this->deleteContainerClicked(index);
}

void medTabbedViewContainers::deleteContainerClicked(int index)
{
    if (this->count() == 1)
    {
        QString name = this->current()->identifier();

        medViewContainer *newTab = NULL;
        if (name == "Single")
            newTab = new medSingleViewContainer();
        else if (name == "Custom")
            newTab = new medCustomViewContainer();
        else if (name == "Multi")
            newTab = new medMultiViewContainer();

        if (newTab != NULL)
        {
            this->blockSignals(true);
            QString tabName = this->tabText(index);
            this->removeTab(index);
            this->insertContainer(index,tabName,newTab);
            this->setCurrentIndex(index);
            this->blockSignals(false);

            this->onCurrentContainerChanged(index);
        }
    }
    else
    {
        d->containers.remove(this->tabText(index));
        this->removeTab(index);
    }
}

void medTabbedViewContainers::addContainer(const QString &name, medViewContainer *container)
{
    if (!container)
        return;
    
    d->containers[name] = container;

    connect( container, SIGNAL( focused( dtkAbstractView * ) ),
             this,      SIGNAL( focused( dtkAbstractView * ) ) );
    connect( container, SIGNAL( imageSet( const medDataIndex & ) ),
             this,      SIGNAL( imageSet( const medDataIndex & ) ) );
    connect( container, SIGNAL( viewAdded( dtkAbstractView * ) ),
             this,      SIGNAL( viewAdded( dtkAbstractView * ) ) );
    connect( container, SIGNAL( viewRemoved( dtkAbstractView * ) ),
             this,      SIGNAL( viewRemoved( dtkAbstractView * ) ) );

    if (!this->count())
        d->currentName = name;

    this->addTab(container, name);
    
    emit focused(NULL);
}

void medTabbedViewContainers::insertContainer(int index, const QString &name, medViewContainer *container)
{
    if (!container)
        return;

    d->containers[name] = container;

    connect( container, SIGNAL( focused( dtkAbstractView * ) ),
             this,      SIGNAL( focused( dtkAbstractView * ) ) );
    connect( container, SIGNAL( imageSet( const medDataIndex & ) ),
             this,      SIGNAL( imageSet( const medDataIndex & ) ) );

    connect( container, SIGNAL( viewAdded( dtkAbstractView * ) ),
             this,      SIGNAL( viewAdded( dtkAbstractView * ) ) );
    connect( container, SIGNAL( viewRemoved( dtkAbstractView * ) ),
             this,      SIGNAL( viewRemoved( dtkAbstractView * ) ) );

    if (!this->count())
        d->currentName = name;

    this->insertTab(index,container, name);
}

void medTabbedViewContainers::changeCurrentContainerName(const QString &name)
{
    unsigned int index = this->currentIndex();
    QString currentName = this->tabText(index);
    medViewContainer *currentContainer = d->containers[currentName];
    d->containers.remove(currentName);
    
    QString newName = name;
    if (this->container(newName))
    {
        unsigned int i = 1;
        newName = name + " ";
        newName += QString::number(i);
        while (this->container(newName))
        {
            ++i;
            newName = name + " ";
            newName += QString::number(i);
        }
    }
    
    this->setTabText(index,newName);
    d->containers[newName] = currentContainer;
}

void medTabbedViewContainers::changeCurrentContainerType(const QString &name)
{
    //qDebug() << "Changing container type to " << name << " from " << this->current()->identifier();
    //qDebug() << "Current index is " << this->currentIndex() << " and tab name " << this->tabText(this->currentIndex());

    if ((!this->current()->views().isEmpty()) && (name != this->current()->identifier()))
    {
        QMessageBox msgBox ( this );
        msgBox.setIcon ( QMessageBox::Warning );
        msgBox.setText(tr("Current tab is not empty. Changing tab type will delete current views."));
        msgBox.setInformativeText(tr("Are you sure you wish to change tab type?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int reply = msgBox.exec();

        if( reply == QMessageBox::No )
        {
            this->onCurrentContainerChanged(this->currentIndex());
            return;
        }
    }

    if (name != this->current()->identifier())
    {
        medViewContainer *newTab = NULL;
        if (name == "Single")
            newTab = new medSingleViewContainer();
        else if (name == "Custom")
            newTab = new medCustomViewContainer();
        else if (name == "Multi")
            newTab = new medMultiViewContainer();

        if (newTab != NULL)
        {
            this->blockSignals(true);
            int index = this->currentIndex();
            QString tabName = this->tabText(index);
            this->removeTab(index);
            this->insertContainer(index,tabName,newTab);
            this->setCurrentIndex(index);
            this->blockSignals(false);

            this->onCurrentContainerChanged(index);
        }
    }
}

medViewContainer* medTabbedViewContainers::container(const QString &name) const
{
    if (!d->containers.contains(name))
        return NULL;

    return d->containers[name];
}

void medTabbedViewContainers::hideTabBar()
{
    QTabBar *tabBar = this->tabBar();
    tabBar->hide();
}

void medTabbedViewContainers::setContainer(const QString &name)
{
    if (!d->containers.contains(name))
    {
        qWarning()<<"container does not contain any container of name:" << name;
        return;
    }

    d->currentName = name;
    this->setCurrentWidget(d->containers[name]);
}

medViewContainer *medTabbedViewContainers::current(void) const
{
    return qobject_cast<medViewContainer*> (currentWidget());
}

QString medTabbedViewContainers::currentName(void) const
{
    return d->currentName;
}

QList<QString> medTabbedViewContainers::keys()
{
    return d->containers.keys();
}

void medTabbedViewContainers::removeContainer(const QString& name)
{
    if (d->containers.contains(name))
    {
        medViewContainer* container = d->containers[name];
        //removeWidget(container);
        container->deleteLater();
        d->containers.remove(name);

        this->blockSignals(true); // to prevent emission of currentChanged (int)
        this->removeTab( this->indexOf(container) );
        this->blockSignals(false);
    }
}

void medTabbedViewContainers::onCurrentContainerChanged(int index)
{
    QString name = this->tabText(index);

    emit focused(container(name)->current()->view());
    emit currentChanged(name);
}
