/* medStackedViewContainers.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 12:39:09 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 22:12:57 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 8
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <QtCore>

#include "medStackedViewContainers.h"

#include "medViewContainer.h"
#include "medViewContainerCustom.h"
#include "medViewContainerMulti.h"
#include "medViewContainerSingle.h"

class medStackedViewContainersPrivate
{
public:
   QHash<QString, medViewContainer*> containers;
   QString currentName;
    
   QPushButton *addButton;
};

medStackedViewContainers::medStackedViewContainers(QWidget *parent) : QTabWidget(parent), d(new medStackedViewContainersPrivate)
{
    this->setTabsClosable(true);
    this->setMovable(true);
    
    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(deleteContainerClicked(int)));
    connect(this,SIGNAL(currentChanged(int)),this,SLOT(onCurrentContainerChanged(int)));

    d->addButton = new QPushButton();
    d->addButton->setStyleSheet("background-image: url(:medGui/pixmaps/plus_button.png);background-position: center;background-repeat: no-repeat;");
    this->setCornerWidget(d->addButton);
    
    connect(d->addButton,SIGNAL(clicked()),this,SIGNAL(addTabButtonClicked()));
}

medStackedViewContainers::~medStackedViewContainers(void)
{
    delete d;

    d = NULL;
}

void medStackedViewContainers::lockTabs()
{
    this->setTabsClosable(false);
    this->setMovable(false);
    d->addButton->hide();
}

void medStackedViewContainers::unlockTabs()
{
    this->setTabsClosable(true);
    this->setMovable(true);
    d->addButton->show();
}

/*
void medStackedViewContainers::addNewTabContainer()
{
    // This slot should disappear, instead the creation signal should be sent to the parent who should call for the creation of a new tab
    QString name = "Tab ";
    name += QString::number(this->count());

    if (!this->container(name))
    {
        medViewContainer *new_container = new medViewContainerMulti();
        addContainer(name, new_container);
        this->setContainer(name);
    }
    else
        qDebug() << "Container" << name << "already exists in this configurations";
}
*/

void medStackedViewContainers::deleteContainerClicked(int index)
{
    if (this->count() > 1)
    {
        d->containers.remove(this->tabText(index));
        this->removeTab(index);
    }
}

void medStackedViewContainers::addContainer(const QString &name, medViewContainer *container)
{
    if (!container)
        return;

    d->containers[name] = container;
    
    connect( container, SIGNAL( focused( dtkAbstractView * ) ),
             this,      SIGNAL( focused( dtkAbstractView * ) ) );
    connect( container, SIGNAL( dropped( const medDataIndex & ) ),
             this,      SIGNAL( dropped( const medDataIndex & ) ) );
    connect( container, SIGNAL( viewAdded( dtkAbstractView * ) ),
             this,      SIGNAL( viewAdded( dtkAbstractView * ) ) );
    connect( container, SIGNAL( viewRemoved( dtkAbstractView * ) ),
             this,      SIGNAL( viewRemoved( dtkAbstractView * ) ) );
    
    if (!this->count())
        d->currentName = name;
    this->addTab(container, name);
}

void medStackedViewContainers::insertContainer(int index, const QString &name, medViewContainer *container)
{
    if (!container)
        return;

    d->containers[name] = container;

    connect( container, SIGNAL( focused( dtkAbstractView * ) ),
             this,      SIGNAL( focused( dtkAbstractView * ) ) );
    connect( container, SIGNAL( dropped( const medDataIndex & ) ),
             this,      SIGNAL( dropped( const medDataIndex & ) ) );
    connect( container, SIGNAL( focused( dtkAbstractView * ) ),
             this,      SIGNAL( focused( dtkAbstractView * ) ) );
    connect( container, SIGNAL( dropped( const medDataIndex & ) ),
             this,      SIGNAL( dropped( const medDataIndex & ) ) );

    connect( container, SIGNAL( viewAdded( dtkAbstractView * ) ),
             this,      SIGNAL( viewAdded( dtkAbstractView * ) ) );
    connect( container, SIGNAL( viewRemoved( dtkAbstractView * ) ),
             this,      SIGNAL( viewRemoved( dtkAbstractView * ) ) );

    if (!this->count())
        d->currentName = name;
    this->insertTab(index,container, name);
}

void medStackedViewContainers::changeCurrentContainerType(const QString &name)
{
    //qDebug() << "Changing container type to " << name << " from " << this->current()->description();
    //qDebug() << "Current index is " << this->currentIndex() << " and tab name " << this->tabText(this->currentIndex());

    if (name != this->current()->description())
    {
        medViewContainer *newTab = NULL;
        if (name == "Single")
            newTab = new medViewContainerSingle();
        else if (name == "Custom")
            newTab = new medViewContainerCustom();
        else if (name == "Multi")
            newTab = new medViewContainerMulti();

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

medViewContainer* medStackedViewContainers::container(const QString &name) const
{
    if (!d->containers.contains(name))
        return NULL;
    
    return d->containers[name];
}

void medStackedViewContainers::setContainer(const QString &name)
{
    if (!d->containers.contains(name))
    {
        qWarning()<<"container does not contain any container of name:" << name;
        return;
    }
    
    d->currentName = name;
    this->setCurrentWidget(d->containers[name]);
}

medViewContainer *medStackedViewContainers::current(void) const
{
    return dynamic_cast<medViewContainer*> (currentWidget());
}

QString medStackedViewContainers::currentName(void) const
{
    return d->currentName;
}

QList<QString> medStackedViewContainers::keys()
{
    return d->containers.keys();
}

void medStackedViewContainers::removeContainer(const QString& name)
{
    if (d->containers.contains(name))
    {
        medViewContainer* container = d->containers[name];
        //removeWidget(container);
        delete container;
        d->containers.remove(name);
    }
}

void medStackedViewContainers::onCurrentContainerChanged(int index)
{
    QString name = this->tabText(index);
    emit currentChanged(name);
}
