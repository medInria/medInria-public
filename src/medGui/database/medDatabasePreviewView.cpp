/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabasePreviewArrow.h>
#include <medDatabasePreviewController.h>
#include <medDatabasePreviewItem.h>
#include <medDatabasePreviewView.h>
#include <medDatabasePreviewTooltip.h>

#include <medDataIndex.h>

class medDatabasePreviewViewPrivate
{
public:
    bool acceptWheelEvent;
};

medDatabasePreviewView::medDatabasePreviewView(QWidget *parent) : QGraphicsView(parent), d(new medDatabasePreviewViewPrivate)
{
    d->acceptWheelEvent = false;

    this->viewport()->setMouseTracking(true);

    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    if(medDatabasePreviewController::instance()->orientation() == Qt::Horizontal) {
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    } else {
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

medDatabasePreviewView::~medDatabasePreviewView(void)
{
    delete d;

    d = NULL;
}

void medDatabasePreviewView::setScene(QGraphicsScene *scene)
{
    QGraphicsView::setScene(scene);
}

void medDatabasePreviewView::setAcceptWheelEvent(bool accept)
{
    d->acceptWheelEvent = accept;
}

void medDatabasePreviewView::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
        case Qt::Key_Up: emit moveUp(); break;
        case Qt::Key_Down: emit moveDw(); break;
        case Qt::Key_Left: emit moveLt(); break;
        case Qt::Key_Right: emit moveRt(); break;
        default:
            QGraphicsView::keyPressEvent(event);
            break;
    }
}

void medDatabasePreviewView::mouseMoveEvent(QMouseEvent *event)
{
    medDatabasePreviewItem *target = dynamic_cast<medDatabasePreviewItem *>(this->scene()->itemAt(this->mapToScene(event->pos())));

    if(target)
        emit hovered(target);

    QGraphicsView::mouseMoveEvent(event);
}

void medDatabasePreviewView::wheelEvent(QWheelEvent* event)
{
    if(d->acceptWheelEvent)
    {
        QGraphicsView::wheelEvent(event);
    }
}

void medDatabasePreviewView::dragEnterEvent(QDragEnterEvent *event)
{
    setBackgroundRole(QPalette::Highlight);

    event->acceptProposedAction();
}

void medDatabasePreviewView::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void medDatabasePreviewView::dragLeaveEvent(QDragLeaveEvent *event)
{
    setBackgroundRole(QPalette::Base);

    event->accept();
}

void medDatabasePreviewView::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasImage()) {
//        setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
    }

    medDataIndex index( medDataIndex::readMimeData(mimeData) );
    if (index.isValid()) {
//        d->index = index;
    }

    setBackgroundRole(QPalette::Base);

    event->acceptProposedAction();

    emit objectDropped(index);
}

void medDatabasePreviewView::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);

//    // Optionally draw something (e.g. a tag) over the label in case it is a pixmap

//    if(!this->pixmap())
//        return;
//
//    QPainter painter;
//    painter.begin(this);
//    painter.setPen(Qt::white);
//    painter.drawText(event->rect(), "Overlay", QTextOption(Qt::AlignHCenter | Qt::AlignCenter));
//    painter.end();
}
