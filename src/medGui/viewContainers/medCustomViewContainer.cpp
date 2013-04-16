/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medViewContainer_p.h"
#include "medCustomViewContainer.h"
#include "medViewPool.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <medAbstractView.h>
#include <medViewManager.h>

class medCustomViewContainerPrivate
{
public:
    int rowMax;
    int columnMax;
    int preset;
};

medCustomViewContainer::medCustomViewContainer ( QWidget *parent ) : medViewContainer ( parent ), d2 ( new medCustomViewContainerPrivate )
{
    d2->rowMax    = 5;
    d2->columnMax = 5;
    d2->preset = 0;
}

medCustomViewContainer::~medCustomViewContainer()
{
    delete d2;
    d2 = NULL;
}


void medCustomViewContainer::split ( int rows, int cols )
{
    if ( d->view )
        return;

    this->clear();

    for ( int i = 0 ; i < rows ; i++ )
    {
        d->layout->setRowStretch ( i, 0 );
        for ( int j = 0 ; j < cols ; j++ )
        {
            medCustomViewContainer *container = new medCustomViewContainer ( this );
            connect ( container, SIGNAL ( viewAdded ( dtkAbstractView* ) ),   this, SIGNAL ( viewAdded ( dtkAbstractView* ) ) );
            connect ( container, SIGNAL ( viewRemoved ( dtkAbstractView* ) ), this, SIGNAL ( viewRemoved ( dtkAbstractView* ) ) );
            d->layout->addWidget ( container, i, j );
            d->layout->setColumnStretch ( j, 0 );
        }
    }

    // in split, the preset is no valid anymore
    d2->preset = 0;

    this->setCurrent ( NULL );
}

void medCustomViewContainer::setPreset ( int preset )
{
    if ( d2->preset == preset )
        return;

    d2->preset = preset;

    this->clear();

    medCustomViewContainer *custom1 = NULL;
    medCustomViewContainer *custom2 = NULL;
    medCustomViewContainer *custom3 = NULL;
    medCustomViewContainer *custom4 = NULL;

    switch ( preset )
    {
    case B:
        custom1 = new medCustomViewContainer ( this );
        custom2 = new medCustomViewContainer ( this );
        d->layout->addWidget ( custom1, 0, 0 );
        d->layout->addWidget ( custom2, 1, 0 );
        d->layout->setRowStretch ( 0, 0 );
        d->layout->setRowStretch ( 1, 0 );
        break;

    case C:
        custom1 = new medCustomViewContainer ( this );
        custom1->split ( 2, 1 );
        custom2 = new medCustomViewContainer ( this );
        d->layout->addWidget ( custom1, 0, 0 );
        d->layout->addWidget ( custom2, 0, 1 );
        d->layout->setColumnStretch ( 0, 1 );
        d->layout->setColumnStretch ( 1, 2 );
        break;

    case D:
        custom1 = new medCustomViewContainer ( this );
        custom1->split ( 3, 1 );
        custom2 = new medCustomViewContainer ( this );
        d->layout->addWidget ( custom1, 0, 0 );
        d->layout->addWidget ( custom2, 0, 1 );
        d->layout->setColumnStretch ( 0, 1 );
        d->layout->setColumnStretch ( 1, 2 );
        break;

    case E:
        custom1 = new medCustomViewContainer ( this );
        custom2 = new medCustomViewContainer ( this );
        custom3 = new medCustomViewContainer ( this );
        custom4 = new medCustomViewContainer ( this );

        custom1->setViewProperty ( "Orientation", "Axial" );
        custom2->setViewProperty ( "Orientation", "Sagittal" );
        custom3->setViewProperty ( "Orientation", "Coronal" );
        custom4->setViewProperty ( "Orientation", "3D" );

        d->layout->addWidget ( custom1, 0, 0 );
        d->layout->addWidget ( custom2, 0, 1 );
        d->layout->addWidget ( custom3, 1, 0 );
        d->layout->addWidget ( custom4, 1, 1 );
        d->layout->setColumnStretch ( 0, 0 );
        d->layout->setColumnStretch ( 1, 0 );
        d->layout->setRowStretch ( 0, 0 );
        d->layout->setRowStretch ( 1, 0 );
        break;

    case A:
    default:
        custom1 = new medCustomViewContainer ( this );
        custom2 = new medCustomViewContainer ( this );
        d->layout->addWidget ( custom1, 0, 0 );
        d->layout->addWidget ( custom2, 0, 1 );
        d->layout->setColumnStretch ( 0, 0 );
        d->layout->setColumnStretch ( 1, 0 );
        break;

    }

    // connect all new custom containers
    QList< medCustomViewContainer * > containers;
    containers << custom1 << custom2 << custom3 << custom4;
    foreach ( medCustomViewContainer * container, containers )
    if ( container != NULL )
    {
        connect ( container, SIGNAL ( viewAdded ( dtkAbstractView * ) ),
                  this,      SIGNAL ( viewAdded ( dtkAbstractView * ) ) );
        connect ( container, SIGNAL ( viewRemoved ( dtkAbstractView * ) ),
                  this,      SIGNAL ( viewRemoved ( dtkAbstractView * ) ) );
    }

    this->setCurrent ( NULL );
}

void medCustomViewContainer::setView ( dtkAbstractView *view )
{
    qDebug() << "Set view";
    if ( this->isLeaf() )
    {
        if ( view != d->view )
        {

            if ( d->layout->count() )
                d->layout->removeItem ( d->layout->itemAt ( 0 ) );

            if ( d->view )
                this->onViewClosing();

            medViewContainer::setView ( view );

            // BEGIN FIXME
            if (d->view) {
                d->layout->setContentsMargins ( 0, 0, 0, 0 );
                d->layout->addWidget ( view->widget(), 0, 0 );

                //d->view = view; // already called in medViewContainer::setView()
                // d->view->reset();

                // retrieve the list of child containers and connect clicked signal
                // to warn other containers that another one was clicked
                medViewContainer *root = this->root();
                if ( root )
                {
                    QList<medViewContainer *> containers = root->childContainers();
                    foreach ( medViewContainer *container, containers )
                    {
                        if ( container->isLeaf() && container!=this )
                        {
                            connect ( this,      SIGNAL ( clicked() ), container, SLOT ( onContainerClicked() ), Qt::UniqueConnection );
                            connect ( container, SIGNAL ( clicked() ), this,      SLOT ( onContainerClicked() ), Qt::UniqueConnection );
                        }
                    }
                }

                this->synchronize_2 ( view );

                connect ( view, SIGNAL ( closing() ),         this, SLOT ( onViewClosing() ) );
                connect ( view, SIGNAL ( fullScreen ( bool ) ),  this, SLOT ( onViewFullScreen ( bool ) ) );
                connect ( view, SIGNAL ( changeDaddy ( bool ) ), this, SLOT ( onDaddyChanged ( bool ) ) );

                this->recomputeStyleSheet();
                emit viewAdded ( view );
            }
            // END FIXME
        }
    }
    else
    {
        // BEGIN FIXME
        if (current())
            current()->setView ( view );
        // END FIXME
        return;
    }
}

dtkAbstractView *medCustomViewContainer::view() const
{
    const medCustomViewContainer* currentContainer =
            qobject_cast<const medCustomViewContainer*> ( current() );
    //Only go into the recursion if we are not a leaf
    //if this is the currentContainer, and not a leaf, we will
    //have an infinite recursion.
    if ( !isLeaf() && currentContainer!= this && currentContainer != NULL  )
    {
        return currentContainer->view();
    }
    return d->view;
}

QList<dtkAbstractView *> medCustomViewContainer::views() const
{
    QList<dtkAbstractView *> views;
    if ( this->childContainers().count() ==0 )
    {
        if ( d->view )
            views << d->view;
    }
    else
    {
        foreach ( medViewContainer * container, this->childContainers() )
            views << container->views();
    }
    return views;
}

bool medCustomViewContainer::isLeaf() const
{
    // this implementation relies on the fact that for the root custom
    // container the leaf custom containers are already set (in setPreset)
    return this->childContainers().count() == 0;
}

void medCustomViewContainer::synchronize_2 ( dtkAbstractView *view )
{
    if ( medCustomViewContainer *parent = qobject_cast<medCustomViewContainer*> ( this->parent() ) )
    {
        parent->synchronize_2 ( view );
    }
    else
    { // top level medCustomViewContainer
        if ( medAbstractView *medView = qobject_cast<medAbstractView*> ( view ) )
            d->pool->appendView ( medView );
        connect ( view, SIGNAL ( becomeDaddy ( bool ) ), this, SLOT ( repaint() ) );
    }
}

void medCustomViewContainer::desynchronize_2 ( dtkAbstractView *view )
{
    if ( medCustomViewContainer *parent = qobject_cast<medCustomViewContainer*> ( this->parent() ) )
    {
        parent->desynchronize_2 ( view );
    }
    else   // top level medCustomViewContainer
    {
        if ( medAbstractView *medView = qobject_cast<medAbstractView*> ( view ) )
        {
            d->pool->removeView ( medView );
            disconnect ( view, SIGNAL ( becomeDaddy ( bool ) ), this, SLOT ( repaint() ) );
        }
    }
}

void medCustomViewContainer::onViewClosing()
{
    if ( d->view )
    {
        this->onViewFullScreen2 ( false, d->view ); // in case view is full screen
        d->layout->removeWidget ( d->view->widget() );
        this->desynchronize_2 ( d->view );
        disconnect ( d->view, SIGNAL ( closing() ),         this, SLOT ( onViewClosing() ) );
        disconnect ( d->view, SIGNAL ( fullScreen ( bool ) ),  this, SLOT ( onViewFullScreen ( bool ) ) );
        disconnect ( d->view, SIGNAL ( changeDaddy ( bool ) ), this, SLOT ( onDaddyChanged ( bool ) ) );

        emit viewRemoved ( d->view );

        d->view->close();
        d->view = NULL;
    }

    medViewContainer * parent = this->parentContainer();
    while ( parent != NULL )
    {
        QList< medViewContainer * > leaves = parent->leaves ( true );
        if ( leaves.count() > 0 )
        {
            leaves.first()->onViewFocused ( true );
            break;
        }
        parent = parent->parentContainer();
    }

    this->recomputeStyleSheet();

    // qDebug() << this << __func__;
    // qDebug() << "isRoot:    " << this->isRoot();
    // qDebug() << "isLeaf:    " << this->isLeaf();
    // qDebug() << "isEmpty:   " << this->isEmpty();
    // qDebug() << "isCurrent: " << this->isCurrent();
}

void medCustomViewContainer::onViewFullScreen ( bool value )
{
    if ( medCustomViewContainer *parent = qobject_cast<medCustomViewContainer*> ( this->parent() ) )
    {
        parent->onViewFullScreen2 ( value, qobject_cast<dtkAbstractView *> ( this->sender() ) );
    }
    else   // top level medCustomViewContainer
    {
        this->fullScreen ( value, qobject_cast<dtkAbstractView *> ( this->sender() ) );
    }
}

void medCustomViewContainer::onViewFullScreen2 ( bool value, dtkAbstractView *view )
{
    if ( medCustomViewContainer *parent = qobject_cast<medCustomViewContainer*> ( this->parent() ) )
    {
        parent->onViewFullScreen2 ( value, view );
    }
    else   // top level medCustomViewContainer
    {
        this->fullScreen ( value, view );
    }
}

void medCustomViewContainer::fullScreen ( bool value, dtkAbstractView *view )
{
    if ( this->childContainers().count() == 0 )   // no children = end widget
    {
        if ( !d->view || ( d->view && d->view!=view ) )
        {
            if ( value )
                this->hide();
            else
                this->show();
        }
    }
    else
    {
        foreach ( medViewContainer *container, this->childContainers() )
        {
            medCustomViewContainer * custom =
                qobject_cast< medCustomViewContainer * > ( container );
            if ( custom != NULL )
                custom->fullScreen ( value, view );
        }
    }
}

void medCustomViewContainer::dragEnterEvent ( QDragEnterEvent *event )
{
    this->setAttribute ( Qt::WA_UpdatesDisabled, true );
    medViewContainer::dragEnterEvent ( event );
}

void medCustomViewContainer::dragMoveEvent ( QDragMoveEvent *event )
{
    medViewContainer::dragMoveEvent ( event );
}

void medCustomViewContainer::dragLeaveEvent ( QDragLeaveEvent *event )
{
    this->setAttribute ( Qt::WA_UpdatesDisabled, false );

    medViewContainer::dragLeaveEvent ( event );
}

void medCustomViewContainer::dropEvent ( QDropEvent *event )
{
    this->setCurrent ( this );
    this->setAttribute ( Qt::WA_UpdatesDisabled, false );
    medViewContainer::dropEvent ( event );
}

void medCustomViewContainer::clear()
{
    if ( d->view )
        this->onViewClosing();

    foreach ( medViewContainer *container, this->childContainers() )
    {
        medCustomViewContainer * custom = qobject_cast< medCustomViewContainer * > ( container );
        if ( custom != NULL )
        {
            custom->clear();
        }
        d->layout->removeWidget ( container );
        // BEGIN FIXME
        container->close();
        container->setParent(NULL);
        // END FIXME
        container->deleteLater(); // safer than delete container
    }

    for ( int i=0; i<d2->rowMax; i++ )
        d->layout->setRowStretch ( i, 0 );

    for ( int i=0; i<d2->columnMax; i++ )
        d->layout->setColumnStretch ( i, 0 );
}
