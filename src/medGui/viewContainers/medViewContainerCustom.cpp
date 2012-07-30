/* medViewContainerCustom.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 11:01:46 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 20 11:26:53 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 69
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medViewContainer_p.h"
#include "medViewContainerCustom.h"
#include "medViewPool.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <medAbstractView.h>
#include <medViewManager.h>

class medViewContainerCustomPrivate
{
public:
    int rowMax;
    int columnMax;
    int preset;
};

medViewContainerCustom::medViewContainerCustom ( QWidget *parent ) : medViewContainer ( parent ), d2 ( new medViewContainerCustomPrivate )
{
    d2->rowMax    = 5;
    d2->columnMax = 5;
    d2->preset = 0;
}

medViewContainerCustom::~medViewContainerCustom()
{
    delete d2;
    d2 = NULL;
}


void medViewContainerCustom::split ( int rows, int cols )
{
    if ( d->view )
        return;

    this->clear();

    for ( int i = 0 ; i < rows ; i++ )
    {
        d->layout->setRowStretch ( i, 0 );
        for ( int j = 0 ; j < cols ; j++ )
        {
            medViewContainerCustom *container = new medViewContainerCustom ( this );
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

void medViewContainerCustom::setPreset ( int preset )
{
    if ( d2->preset == preset )
        return;

    d2->preset = preset;

    this->clear();

    medViewContainerCustom *custom1 = NULL;
    medViewContainerCustom *custom2 = NULL;
    medViewContainerCustom *custom3 = NULL;
    medViewContainerCustom *custom4 = NULL;

    switch ( preset )
    {
    case B:
        custom1 = new medViewContainerCustom ( this );
        custom2 = new medViewContainerCustom ( this );
        d->layout->addWidget ( custom1, 0, 0 );
        d->layout->addWidget ( custom2, 1, 0 );
        d->layout->setRowStretch ( 0, 0 );
        d->layout->setRowStretch ( 1, 0 );
        break;

    case C:
        custom1 = new medViewContainerCustom ( this );
        custom1->split ( 2, 1 );
        custom2 = new medViewContainerCustom ( this );
        d->layout->addWidget ( custom1, 0, 0 );
        d->layout->addWidget ( custom2, 0, 1 );
        d->layout->setColumnStretch ( 0, 1 );
        d->layout->setColumnStretch ( 1, 2 );
        break;

    case D:
        custom1 = new medViewContainerCustom ( this );
        custom1->split ( 3, 1 );
        custom2 = new medViewContainerCustom ( this );
        d->layout->addWidget ( custom1, 0, 0 );
        d->layout->addWidget ( custom2, 0, 1 );
        d->layout->setColumnStretch ( 0, 1 );
        d->layout->setColumnStretch ( 1, 2 );
        break;

    case E:
        custom1 = new medViewContainerCustom ( this );
        custom2 = new medViewContainerCustom ( this );
        custom3 = new medViewContainerCustom ( this );
        custom4 = new medViewContainerCustom ( this );

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
        custom1 = new medViewContainerCustom ( this );
        custom2 = new medViewContainerCustom ( this );
        d->layout->addWidget ( custom1, 0, 0 );
        d->layout->addWidget ( custom2, 0, 1 );
        d->layout->setColumnStretch ( 0, 0 );
        d->layout->setColumnStretch ( 1, 0 );
        break;

    }

    // connect all new custom containers
    QList< medViewContainerCustom * > containers;
    containers << custom1 << custom2 << custom3 << custom4;
    foreach ( medViewContainerCustom * container, containers )
    if ( container != NULL )
    {
        connect ( container, SIGNAL ( viewAdded ( dtkAbstractView * ) ),
                  this,      SIGNAL ( viewAdded ( dtkAbstractView * ) ) );
        connect ( container, SIGNAL ( viewRemoved ( dtkAbstractView * ) ),
                  this,      SIGNAL ( viewRemoved ( dtkAbstractView * ) ) );
    }

    this->setCurrent ( NULL );
}

void medViewContainerCustom::setView ( dtkAbstractView *view )
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

dtkAbstractView *medViewContainerCustom::view ( void ) const
{
    const medViewContainerCustom* currentContainer =
            qobject_cast<const medViewContainerCustom*> ( current() );
    //Only go into the recursion if we are not a leaf
    //if this is the currentContainer, and not a leaf, we will
    //have an infinite recursion.
    if ( !isLeaf() && currentContainer!= this && currentContainer != NULL  )
    {
        return currentContainer->view();
    }
    return d->view;
}

QList<dtkAbstractView *> medViewContainerCustom::views ( void ) const
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

bool medViewContainerCustom::isLeaf ( void ) const
{
    // this implementation relies on the fact that for the root custom
    // container the leaf custom containers are already set (in setPreset)
    return this->childContainers().count() == 0;
}

void medViewContainerCustom::synchronize_2 ( dtkAbstractView *view )
{
    if ( medViewContainerCustom *parent = qobject_cast<medViewContainerCustom*> ( this->parent() ) )
    {
        parent->synchronize_2 ( view );
    }
    else
    { // top level medViewContainerCustom
        if ( medAbstractView *medView = qobject_cast<medAbstractView*> ( view ) )
            d->pool->appendView ( medView );
        connect ( view, SIGNAL ( becomeDaddy ( bool ) ), this, SLOT ( repaint() ) );
    }
}

void medViewContainerCustom::desynchronize_2 ( dtkAbstractView *view )
{
    if ( medViewContainerCustom *parent = qobject_cast<medViewContainerCustom*> ( this->parent() ) )
    {
        parent->desynchronize_2 ( view );
    }
    else   // top level medViewContainerCustom
    {
        if ( medAbstractView *medView = qobject_cast<medAbstractView*> ( view ) )
        {
            d->pool->removeView ( medView );
            disconnect ( view, SIGNAL ( becomeDaddy ( bool ) ), this, SLOT ( repaint() ) );
        }
    }
}

void medViewContainerCustom::onViewClosing ( void )
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

void medViewContainerCustom::onViewFullScreen ( bool value )
{
    if ( medViewContainerCustom *parent = qobject_cast<medViewContainerCustom*> ( this->parent() ) )
    {
        parent->onViewFullScreen2 ( value, qobject_cast<dtkAbstractView *> ( this->sender() ) );
    }
    else   // top level medViewContainerCustom
    {
        this->fullScreen ( value, qobject_cast<dtkAbstractView *> ( this->sender() ) );
    }
}

void medViewContainerCustom::onViewFullScreen2 ( bool value, dtkAbstractView *view )
{
    if ( medViewContainerCustom *parent = qobject_cast<medViewContainerCustom*> ( this->parent() ) )
    {
        parent->onViewFullScreen2 ( value, view );
    }
    else   // top level medViewContainerCustom
    {
        this->fullScreen ( value, view );
    }
}

void medViewContainerCustom::fullScreen ( bool value, dtkAbstractView *view )
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
            medViewContainerCustom * custom =
                qobject_cast< medViewContainerCustom * > ( container );
            if ( custom != NULL )
                custom->fullScreen ( value, view );
        }
    }
}

void medViewContainerCustom::dragEnterEvent ( QDragEnterEvent *event )
{
    this->setAttribute ( Qt::WA_UpdatesDisabled, true );
    medViewContainer::dragEnterEvent ( event );
}

void medViewContainerCustom::dragMoveEvent ( QDragMoveEvent *event )
{
    medViewContainer::dragMoveEvent ( event );
}

void medViewContainerCustom::dragLeaveEvent ( QDragLeaveEvent *event )
{
    this->setAttribute ( Qt::WA_UpdatesDisabled, false );

    medViewContainer::dragLeaveEvent ( event );
}

void medViewContainerCustom::dropEvent ( QDropEvent *event )
{
    this->setCurrent ( this );
    this->setAttribute ( Qt::WA_UpdatesDisabled, false );
    medViewContainer::dropEvent ( event );
}

void medViewContainerCustom::clear ( void )
{
    if ( d->view )
        this->onViewClosing();

    foreach ( medViewContainer *container, this->childContainers() )
    {
        medViewContainerCustom * custom = qobject_cast< medViewContainerCustom * > ( container );
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
