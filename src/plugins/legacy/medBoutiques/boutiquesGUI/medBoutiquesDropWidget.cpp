#include "medBoutiquesDropWidget.h"

medBoutiquesDropWidget::medBoutiquesDropWidget(QWidget *parent) : QWidget(parent)
{
    this->setAcceptDrops(true);
}

void medBoutiquesDropWidget::dragEnterEvent(QDragEnterEvent *event)
{
    emit dragEnter(event);
}

void medBoutiquesDropWidget::dragMoveEvent(QDragMoveEvent *event)
{
    emit dragMove(event);
}

void medBoutiquesDropWidget::dropEvent(QDropEvent *event)
{
    emit drop(event);
}
