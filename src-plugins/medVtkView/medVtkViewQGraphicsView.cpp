/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkViewQGraphicsView.h>


#include <QVTKGraphicsItem.h>
#include <QGraphicsView>
#include <QDebug>

class medVtkViewQGraphicsViewPrivate
{
public:
    QVTKGraphicsItem *vtkItem;
};


medVtkViewQGraphicsView::medVtkViewQGraphicsView(QWidget *parent):
    d(new medVtkViewQGraphicsViewPrivate)
{
    this->setAcceptDrops(true);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setCursor(QCursor(Qt::CrossCursor));
    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    d->vtkItem = NULL;
}


medVtkViewQGraphicsView::~medVtkViewQGraphicsView()
{
    delete d;
    d = NULL;
}

void medVtkViewQGraphicsView::setQVtkGraphicsItem(QVTKGraphicsItem *vtkItem)
{
    d->vtkItem = vtkItem;
}

QVTKGraphicsItem* medVtkViewQGraphicsView::qVtkGraphicsItem() const
{
    return d->vtkItem;
}

void medVtkViewQGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    if(d->vtkItem)
        d->vtkItem->resize(event->size());
}

void medVtkViewQGraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
    QWidget::dragEnterEvent(event);
}

void medVtkViewQGraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    QWidget::dragMoveEvent(event);
}

void medVtkViewQGraphicsView::dragLeaveEvent(QDragLeaveEvent *event)
{
    QWidget::dragLeaveEvent(event);
}

void medVtkViewQGraphicsView::dropEvent(QDropEvent *event)
{
    QWidget::dropEvent(event);
}

void medVtkViewQGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    QWidget::mousePressEvent(event);
}
