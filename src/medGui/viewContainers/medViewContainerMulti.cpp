/* medViewContainerMulti.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 11:01:46 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 20 11:26:25 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 56
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewContainer_p.h"
#include "medViewContainerMulti.h"
#include "medViewPool.h"

#include <dtkCore/dtkAbstractView.h>

#include <medAbstractView.h>
#include <medViewManager.h>

medViewContainerSingle2::~medViewContainerSingle2()
{
    d->layout->setContentsMargins(0, 0, 0, 0);
}

void medViewContainerSingle2::setView (dtkAbstractView *view)
{
    if (d->view==view)
        return;

    if (d->view) {
        this->onViewClosing();
    }

    d->view = view;

    if (view) {
        d->layout->addWidget(view->widget(), 0, 0);    
        // d->pool->appendView (view); // only difference with medViewContainerSingle: do not add the view to the pool
        connect (view, SIGNAL (closing()), this, SLOT (onViewClosing()));
    }
}

bool medViewContainerSingle2::isLeaf(void) const
{
    return true;
}

void medViewContainerSingle2::onViewClosing (void)
{
    if (d->view) {
        d->layout->removeWidget(d->view->widget());
        //d->view->widget()->hide();

        disconnect (d->view, SIGNAL (closing()), this, SLOT (onViewClosing()));
        // d->pool->removeView (d->view); // do not remove it from the pool

        d->view->close();
        d->view = NULL;
    }

    // qDebug() << this << __func__;
    // qDebug() << "isRoot:    " << this->isRoot();
    // qDebug() << "isLeaf:    " << this->isLeaf();
    // qDebug() << "isEmpty:   " << this->isEmpty();
    // qDebug() << "isCurrent: " << this->isCurrent();
}

void medViewContainerSingle2::onViewFocused (bool value)
{
    if ( !value )
        return;

    //if ( !this->isEmpty() )
    //    this->setCurrent( this );

    if (dtkAbstractView *view = this->view())
        emit focused(view);

    this->update();
}

class medViewContainerMultiPrivate
{
public:
    QList< dtkAbstractView* >  views;    
};

medViewContainerMulti::medViewContainerMulti (QWidget *parent) : medViewContainer (parent), d2 (new medViewContainerMultiPrivate)
{
}

medViewContainerMulti::~medViewContainerMulti()
{
    medViewContainer::setView(0);
  
    delete d2;
    d2 = NULL;
}


void medViewContainerMulti::split(int rows, int cols)
{
    Q_UNUSED(rows);
    Q_UNUSED(cols);

    // Don't split a multi view container

    return;
}

dtkAbstractView *medViewContainerMulti::view(void) const
{
    return NULL;
}

QList<dtkAbstractView*> medViewContainerMulti::views (void) const
{
    return d2->views;
}

void medViewContainerMulti::setView(dtkAbstractView *view)
{
    if (!view)
        return;

    if (d2->views.contains (view))
        return;
  
    QList<QWidget *> content;
    for(int i = 0; i < d->layout->rowCount() ; i++) {
        for(int j = 0; j < d->layout->columnCount() ; j++) {
            if(QLayoutItem *item = d->layout->itemAtPosition(i, j)) {
                content << item->widget();
                d->layout->removeItem(item);
            }
        }
    }

    // retrieve the list of containers before creating a new one
    // so it is not in the list
    QList <medViewContainer *> containers = this->childContainers();

    medViewContainer *container = new medViewContainerSingle2(this);
    container->setAcceptDrops(false);
    container->setView(view);    
    
    foreach (medViewContainer *cont, containers) {
        if (cont->isLeaf()) {
            connect (container, SIGNAL (clicked()), cont, SLOT (onContainerClicked()), Qt::UniqueConnection);
            connect (cont, SIGNAL (clicked()), container, SLOT (onContainerClicked()), Qt::UniqueConnection);
        }
    }

    content << container;
    this->layout(content);

    // medViewContainer::setView (view); //no, since d->view does not make sense in multi

    d2->views << view;
    
    if (medAbstractView *medView = dynamic_cast<medAbstractView*> (view))
        d->pool->appendView (medView);
    
    connect (view, SIGNAL (closing()),         this, SLOT (onViewClosing()));
    connect (view, SIGNAL (fullScreen(bool)),  this, SLOT (onViewFullScreen(bool)));
    connect (view, SIGNAL (changeDaddy(bool)),
             this, SLOT (onDaddyChanged(bool)));

    //this->setCurrent( container ); // no, single2 cannot be current, "this" is always current in multiview
    emit viewAdded (view);
}

void medViewContainerMulti::layout(QList<QWidget *> content)
{
    int row = 0;
    int col = 0, colmax = 0;
    
    for(int i = 0; i < content.count()-1; i++) {
        
        if(((col+1)*(row+1)) <= content.count()-1) {
            
            qreal rratio = qMin(((qreal)this->height()/(qreal)(row+2)), ((qreal)this->width()/(qreal)(col+1)));
            qreal cratio = qMin(((qreal)this->height()/(qreal)(row+1)), ((qreal)this->width()/(qreal)(col+2)));
            
            if(rratio > cratio) {
                row++;
                col = 0;
            } else {
                col++;
            }
            
            colmax = qMax(col, colmax);
        }
    }
    
    int layout_row = 0;
    int layout_col = 0;
    
    for(int i = 0; i < content.size(); i++) {
        
        d->layout->addWidget(content.at(i), layout_row, layout_col);
        
        if(layout_col == colmax) {
            layout_row++;
            layout_col = 0;
        } else {
            layout_col++;
        }
    }
}

void medViewContainerMulti::onViewClosing (void)
{
    if (dtkAbstractView *view =
        dynamic_cast<dtkAbstractView *>(this->sender())) {

        // needed for selecting another container as current afterwards
        QWidget * predContainer   = NULL;
        QWidget * succContainer   = NULL;
        bool      closedItemFound = false;

        QWidget * closedContainer =
            dynamic_cast< QWidget * >( view->widget()->parent() );
        QList<QWidget *> content;
        for (int i = 0; i < d->layout->rowCount(); i++) {
            for (int j = 0; j < d->layout->columnCount(); j++) {
                QLayoutItem * item = d->layout->itemAtPosition(i, j);
                if ( item != NULL ) {
                    QWidget * container = item->widget();
                    if ( container == closedContainer ) {
                        container->hide();
                        closedItemFound = true;
                    }
                    else {
                        content << container; // keep the container in layout
                        container->show(); // in case view was hidden

                        // remember the predecessor resp. successor of
                        // the closed container
                        if ( closedItemFound ) {
                            if ( succContainer == NULL )
                                succContainer = container;
                        }
                        else
                            predContainer = container;
                    }

                    d->layout->removeItem(item);
                }
            }
        }

        disconnect (view, SIGNAL (closing()),
                    this, SLOT (onViewClosing()));
        disconnect (view, SIGNAL (fullScreen(bool)),
                    this, SLOT (onViewFullScreen(bool)));
        disconnect (view, SIGNAL (changeDaddy(bool)),
                    this, SLOT (onDaddyChanged(bool)));

        if (medAbstractView *medView = dynamic_cast<medAbstractView*> (view))
            d->pool->removeView (medView);

        d2->views.removeOne (view);

        emit viewRemoved (view);

        // view->close(); // the container will close the view once deleted

        closedContainer->deleteLater();

        this->layout (content);

        medViewContainer * current =
            dynamic_cast< medViewContainer * >( succContainer );
        if ( current == NULL )
            current = dynamic_cast< medViewContainer * >( predContainer );
        if ( current == NULL )
            current = this;

        this->setCurrent(this);

        current->onViewFocused( true );

        this->update();

        // qDebug() << this << __func__;
        // qDebug() << "isRoot:    " << this->isRoot();
        // qDebug() << "isLeaf:    " << this->isLeaf();
        // qDebug() << "isEmpty:   " << this->isEmpty();
        // qDebug() << "isCurrent: " << this->isCurrent();
    }
}

void medViewContainerMulti::onViewFullScreen (bool value)
{
    if (dtkAbstractView *view = dynamic_cast<dtkAbstractView *>(this->sender())) {
        if (value) {
            QList<QWidget *> content;
	        for(int i = 0; i < d->layout->rowCount() ; i++) {
                for(int j = 0; j < d->layout->columnCount() ; j++) {
	            if(QLayoutItem *item = d->layout->itemAtPosition(i, j)) {
		            if(item->widget()!=view->widget()->parent())
		                item->widget()->hide();
	                }
                }
	    }        
    }
    else {
	    QList<QWidget *> content;
        for(int i = 0; i < d->layout->rowCount() ; i++) {
	        for(int j = 0; j < d->layout->columnCount() ; j++) {
	            if(QLayoutItem *item = d->layout->itemAtPosition(i, j)) {
	                if(item->widget()!=view->widget()->parent())
		                item->widget()->show();
                    }
	            }       
	        }
      }
  }
}

void medViewContainerMulti::dragEnterEvent(QDragEnterEvent *event)
{
    medViewContainer::dragEnterEvent(event);
}

void medViewContainerMulti::dragMoveEvent(QDragMoveEvent *event)
{
    medViewContainer::dragMoveEvent(event);
}

void medViewContainerMulti::dragLeaveEvent(QDragLeaveEvent *event)
{
    medViewContainer::dragLeaveEvent(event);
}

void medViewContainerMulti::dropEvent(QDropEvent *event)
{
    if(medViewContainerMulti *container = dynamic_cast<medViewContainerMulti *>(this->parentWidget())) {
        this->setCurrent(container);
    }
    else {
        this->setCurrent(this);
    }

    medViewContainer::dropEvent(event);
}

void medViewContainerMulti::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
}

void medViewContainerMulti::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
}
