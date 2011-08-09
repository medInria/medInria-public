/* medTabbedViewContainers.cpp ---
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

#include "medTabbedViewContainers.h"

#include "medViewContainer.h"
#include "medViewContainerCustom.h"
#include "medViewContainerMulti.h"
#include "medViewContainerSingle.h"

class medTabbedViewContainersPrivate
{
public:
   QHash<QString, medViewContainer*> containers;
   QString currentName;
    
   QPushButton *addButton;
};

medTabbedViewContainers::medTabbedViewContainers(QWidget *parent) : QTabWidget(parent), d(new medTabbedViewContainersPrivate)
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
        medViewContainer *new_container = new medViewContainerMulti();
        addContainer(name, new_container);
        this->setContainer(name);
    }
    else
        qDebug() << "Container" << name << "already exists in this configurations";
}
*/

void medTabbedViewContainers::deleteContainerClicked(int index)
{
    if (this->count() > 1)
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

void medTabbedViewContainers::insertContainer(int index, const QString &name, medViewContainer *container)
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
    this->insertTab(index,container, name);
}

void medTabbedViewContainers::changeCurrentContainerType(const QString &name)
{
    //qDebug() << "Changing container type to " << name << " from " << this->current()->description();
    //qDebug() << "Current index is " << this->currentIndex() << " and tab name " << this->tabText(this->currentIndex());

    if ((!this->current()->views().isEmpty()) && (name != this->current()->description()))
    {
        QMessageBox msgBox ( this );
        msgBox.setIcon ( QMessageBox::Warning );
        msgBox.setText(tr("Current tab is not empty. Changing tab type will delete current views."));
        msgBox.setInformativeText(tr("Are you sure you wish to change tab type?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int reply = msgBox.exec();

        if( reply == QMessageBox::No )
            return;
    }

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

medViewContainer* medTabbedViewContainers::container(const QString &name) const
{
    if (!d->containers.contains(name))
        return NULL;
    
    return d->containers[name];
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
    return dynamic_cast<medViewContainer*> (currentWidget());
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
        delete container;
        d->containers.remove(name);
    }
}

void medTabbedViewContainers::onCurrentContainerChanged(int index)
{
    QString name = this->tabText(index);
    emit currentChanged(name);
}
