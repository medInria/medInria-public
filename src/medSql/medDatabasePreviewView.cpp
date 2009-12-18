/* medDatabasePreviewView.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:44:41 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:44:41 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabasePreviewArrow.h"
#include "medDatabasePreviewController.h"
#include "medDatabasePreviewItem.h"
#include "medDatabasePreviewView.h"
#include "medDatabasePreviewTooltip.h"

class medDatabasePreviewViewPrivate
{
public:
    medDatabasePreviewArrow *arrow_up;
    medDatabasePreviewArrow *arrow_dw;

    medDatabasePreviewTooltip *tooltip_patient;
    medDatabasePreviewTooltip *tooltip_study;
    medDatabasePreviewTooltip *tooltip_series;
    medDatabasePreviewTooltip *tooltip_image;

    QTimeLine *timeLine;
};

medDatabasePreviewView::medDatabasePreviewView(QWidget *parent) : QGraphicsView(parent), d(new medDatabasePreviewViewPrivate)
{
    d->arrow_up = new medDatabasePreviewArrow(QPixmap(":/pixmaps/database_preview_arrow_up.tiff"), QPixmap(":/pixmaps/database_preview_arrow_up_pressed.tiff"));
    d->arrow_dw = new medDatabasePreviewArrow(QPixmap(":/pixmaps/database_preview_arrow_down.tiff"), QPixmap(":/pixmaps/database_preview_arrow_down_pressed.tiff"));
    d->arrow_up->setOpacity(0.0);
    d->arrow_dw->setOpacity(0.0);
    connect(d->arrow_up, SIGNAL(clicked()), this, SIGNAL(slideUp()));
    connect(d->arrow_dw, SIGNAL(clicked()), this, SIGNAL(slideDw()));

    d->tooltip_patient = NULL;
    d->tooltip_study = NULL;
    d->tooltip_series = NULL;
    d->tooltip_image = NULL;

    d->timeLine = new QTimeLine(500, this);
    d->timeLine->setFrameRange(0, 100);
    connect(d->timeLine, SIGNAL(frameChanged(int)), this, SLOT(onFrameChanged(int)));

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

    scene->addItem(d->arrow_up);
    scene->addItem(d->arrow_dw);
}

void medDatabasePreviewView::setPatientTooltip(medDatabasePreviewTooltip *tooltip)
{
    d->tooltip_patient = tooltip;
}

void medDatabasePreviewView::setStudyTooltip(medDatabasePreviewTooltip *tooltip)
{
    d->tooltip_study = tooltip;
}

void medDatabasePreviewView::setSeriesTooltip(medDatabasePreviewTooltip *tooltip)
{
    d->tooltip_series = tooltip;
}

void medDatabasePreviewView::setImageTooltip(medDatabasePreviewTooltip *tooltip)
{
    d->tooltip_image = tooltip;
}

void medDatabasePreviewView::onFrameChanged(int frame)
{
    d->arrow_up->setOpacity((qreal)(frame/100.0));
    d->arrow_dw->setOpacity((qreal)(frame/100.0));
}

void medDatabasePreviewView::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Up:
        // emit moveUp();
        emit slideDw();
        break;
    case Qt::Key_Down:
        // emit moveDw();
        emit slideUp();
        break;
    case Qt::Key_Left:
        emit moveLt();
        break;
    case Qt::Key_Right:
        emit moveRt();
        break;
    default:
        break;
    }
}

void medDatabasePreviewView::mouseMoveEvent(QMouseEvent *event)
{
    // Dealing with buttons

    static bool shown = false;

    if (d->timeLine->state() == QTimeLine::Running)
        return;

    if ((event->pos().x() > this->rect().width() - medDatabasePreviewController::instance()->queryOffset()) && !shown) {
        d->timeLine->setDirection(QTimeLine::Forward);
        d->timeLine->start();
        shown = !shown;
    } else if ((event->pos().x() < this->rect().width() - medDatabasePreviewController::instance()->queryOffset()) && shown){
        d->timeLine->setDirection(QTimeLine::Backward);
        d->timeLine->start();
        shown = !shown;
    }

    // Find hovered item

    medDatabasePreviewItem *target = dynamic_cast<medDatabasePreviewItem *>(this->scene()->itemAt(this->mapToScene(event->pos())));

    if(target)
        emit hovered(target);

    // Let graphics view framework handle the event as well

    QGraphicsView::mouseMoveEvent(event);
}

void medDatabasePreviewView::resizeEvent(QResizeEvent *event)
{
    d->arrow_dw->setPos(event->size().width() - 50,  10);
    d->arrow_up->setPos(event->size().width() - 50,  50);

    if (d->tooltip_patient)
        d->tooltip_patient->setPos(event->size().width()/2 - d->tooltip_patient->boundingRect().width()/2, 10);

    if (d->tooltip_study)
        d->tooltip_study->setPos(event->size().width()/2 - d->tooltip_study->boundingRect().width()/2, 10);

    if (d->tooltip_series)
        d->tooltip_series->setPos(event->size().width()/2 - d->tooltip_series->boundingRect().width()/2, 10);

    if (d->tooltip_image)
        d->tooltip_image->setPos(event->size().width()/2 - d->tooltip_image->boundingRect().width()/2, 10);
}
