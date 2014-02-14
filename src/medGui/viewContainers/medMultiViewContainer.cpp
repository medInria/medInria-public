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

void medSingleViewContainer2::setView (medAbstractView *view)
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
        d->layout->addWidget(view->widget(), 0, 0);

    this->setFocus(Qt::MouseFocusReason);
}



class medMultiViewContainerPrivate
{
public:
    QList< dtkSmartPointer<medAbstractView> >  views;
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

medAbstractView *medMultiViewContainer::view(void) const
{
    return NULL;
}

QList<medAbstractView*> medMultiViewContainer::views() const
{
    QList<medAbstractView *> views;
    foreach(medAbstractView *view, d2->views)
        views.append(view);

    return views;
}

void medMultiViewContainer::setView(medAbstractView *view)
{
    if (!view)
        return;

    qDebug() << "setting the view" << view << "from multi container" << this;
    QList<QWidget *> content;
    for(int i = 0; i < d->layout->rowCount() ; i++)
    {
        for(int j = 0; j < d->layout->columnCount() ; j++)
        {
            if(QLayoutItem *item = d->layout->itemAtPosition(i, j))
            {
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
        if (cont->isLeaf())
        {
            connect (container, SIGNAL (selected()), cont, SLOT (unselect()), Qt::UniqueConnection);
            connect (cont, SIGNAL (selected()), container, SLOT (unselect()), Qt::UniqueConnection);
        }
    }

    content << container;
    this->layout(content);

    d2->views << view;

    this->setCurrent(container);
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

void medMultiViewContainer::onViewFullScreen (bool value)
{
    if (medAbstractView *view = qobject_cast<medAbstractView *>(this->sender())) {
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
