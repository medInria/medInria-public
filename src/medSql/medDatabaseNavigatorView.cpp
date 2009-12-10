#include "medDatabaseNavigatorArrow.h"
#include "medDatabaseNavigatorController.h"
#include "medDatabaseNavigatorItem.h"
#include "medDatabaseNavigatorView.h"
#include "medDatabaseNavigatorTooltip.h"

class medDatabaseNavigatorViewPrivate
{
public:
};

medDatabaseNavigatorView::medDatabaseNavigatorView(QWidget *parent) : QGraphicsView(parent), d(new medDatabaseNavigatorViewPrivate)
{
    this->viewport()->setMouseTracking(true);

    this->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    if(medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal)
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    else
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    this->setFrameShape(QFrame::NoFrame);

    this->setStyleSheet("background: transparent;");
}

medDatabaseNavigatorView::~medDatabaseNavigatorView(void)
{
    delete d;

    d = NULL;
}

void medDatabaseNavigatorView::setScene(QGraphicsScene *scene)
{
    QGraphicsView::setScene(scene);
}

void medDatabaseNavigatorView::mouseMoveEvent(QMouseEvent *event)
{
    // Find hovered item

    medDatabaseNavigatorItem *target = dynamic_cast<medDatabaseNavigatorItem *>(this->scene()->itemAt(this->mapToScene(event->pos())));

    if(target)
        emit hovered(target);

    // Let graphics view framework handle the event as well

    QGraphicsView::mouseMoveEvent(event);
}
