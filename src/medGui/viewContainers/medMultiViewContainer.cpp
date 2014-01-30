/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewContainer_p.h>
#include <medMultiViewContainer.h>

#include <dtkCore/dtkAbstractView.h>

#include <medAbstractView.h>
#include <medViewManager.h>


medSingleViewContainer2::medSingleViewContainer2(QWidget *parent) :
    medSingleViewContainer(parent)
{
    d->multiLayer = false;
}

medSingleViewContainer2::~medSingleViewContainer2()
{
    d->layout->setContentsMargins(0, 0, 0, 0);
}

void medSingleViewContainer2::setView (dtkAbstractView *view)
{
    // Never replace an existing view: we are in multi, we always add a new one.

    // if we already have a view, we should transfer the request to the root.
    if (d->view)
    {
        if (medMultiViewContainer* container =
                qobject_cast<medMultiViewContainer*>(parentContainer()))
        {
            container->setView( view );
            return;
        }

        return;
    }

    d->view = view;
    d->view->setParent(this);

    if (view)
    {
        d->layout->addWidget(view->widget(), 0, 0);
        // d->pool->appendView (view); // only difference with medSingleViewContainer: do not add the view to the pool
        connect (view, SIGNAL (closing()), this, SLOT (onViewClosing()));
        medAbstractView * medView = qobject_cast<medAbstractView*>(view);
        if (medView) {
            connect(medView, SIGNAL(selected()), this, SLOT(select()));
        }
    }
    this->setFocus(Qt::MouseFocusReason);
}


void medSingleViewContainer2::onViewClosing()
{
    if (d->view) {
        d->layout->removeWidget(d->view->widget());
        //d->view->widget()->hide();

        disconnect (d->view, SIGNAL (closing()), this, SLOT (onViewClosing()));
        // d->pool->removeView (d->view); // do not remove it from the pool

        d->view->close();
        d->view = NULL;
    }

}


class medMultiViewContainerPrivate
{
public:
    QList< dtkSmartPointer<dtkAbstractView> >  views;
};

medMultiViewContainer::medMultiViewContainer (QWidget *parent) : medViewContainer (parent), d2 (new medMultiViewContainerPrivate)
{
}

medMultiViewContainer::~medMultiViewContainer()
{
    foreach ( QObject* obj, d->layout->children())
    {
        obj->deleteLater();
    }

    delete d2;
    d2 = NULL;
}


void medMultiViewContainer::split(int rows, int cols)
{
    Q_UNUSED(rows);
    Q_UNUSED(cols);

    // Don't split a multi view container

    return;
}

dtkAbstractView *medMultiViewContainer::view(void) const
{
    return NULL;
}

QList<dtkAbstractView*> medMultiViewContainer::views() const
{
    QList<dtkAbstractView *> views;
    foreach(dtkAbstractView *view, d2->views)
        views.append(view);

    return views;
}

void medMultiViewContainer::setView(dtkAbstractView *view)
{
    if (!view)
        return;

//    if (d2->views.contains (view))
//        return;

    qDebug() << "setting the view" << view << "from multi container" << this;
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

    medViewContainer *container = new medSingleViewContainer2(this);
    container->setView(view);

    foreach (medViewContainer *cont, containers) {
        if (cont->isLeaf()) {
            connect (container, SIGNAL (selected()), cont, SLOT (unselect()), Qt::UniqueConnection);
            connect (cont, SIGNAL (selected()), container, SLOT (unselect()), Qt::UniqueConnection);
        }
    }

    content << container;
    this->layout(content);

    d2->views << view;

    connect (view, SIGNAL (closing()),         this, SLOT (onViewClosing()));
    connect (view, SIGNAL (fullScreen(bool)),  this, SLOT (onViewFullScreen(bool)));

    this->setCurrent( container );
    emit viewAdded (view);
    //the focus is given in single2, no need to give it to the root container.
//    container->setFocus(Qt::MouseFocusReason);
}

void medMultiViewContainer::layout(QList<QWidget *> content)
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

void medMultiViewContainer::onViewClosing()
{
//    qDebug()<<"containerMulti closing a view";
    if (dtkAbstractView *view =
        qobject_cast<dtkAbstractView *>(this->sender())) {

        // needed for selecting another container as current afterwards
        QWidget * predContainer   = NULL;
        QWidget * succContainer   = NULL;
        bool      closedItemFound = false;

        QWidget * closedContainer =
            qobject_cast< QWidget * >( view->widget()->parent() );
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

        // it is safer to emit view removed now, because the next
        // line may trigger the deletion of the view
        emit viewRemoved (view);

        d2->views.removeOne (view);

        // view->close(); // the container will close the view once deleted

        //tests showed it's safe to call directly delete here: deleteLater
        //doesn't work: container is still a child of "this" for some time afterward.
        //delete closedContainer;
        // BEGIN FIXME
        closedContainer->setParent (NULL);
        closedContainer->deleteLater();
        // END FIXME

        this->layout (content);
        //Choose the new current container based on who is next or previous
        medViewContainer * current =
            qobject_cast< medViewContainer * >( succContainer );
        if ( current == NULL )
            current = qobject_cast< medViewContainer * >( predContainer );
        if ( current == NULL )
            current = this;
//        qDebug() << "multi:"<< this;
//        qDebug() << "pred" << predContainer;
//        qDebug() << "succ" << succContainer;
//        qDebug() << "new current" << current;
        this->setCurrent(current);
        current->setFocus();

        this->update();

        // qDebug() << this << __func__;
        // qDebug() << "isRoot:    " << this->isRoot();
        // qDebug() << "isLeaf:    " << this->isLeaf();
        // qDebug() << "isEmpty:   " << this->isEmpty();
        // qDebug() << "isCurrent: " << this->isCurrent();
    }
}

void medMultiViewContainer::onViewFullScreen (bool value)
{
    if (dtkAbstractView *view = qobject_cast<dtkAbstractView *>(this->sender())) {
        if (value) {
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
