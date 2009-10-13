/* medViewerArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:43:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  7 15:59:14 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 125
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerAreaViewContainer.h"

#include <QtGui>

medViewerAreaViewContainer::medViewerAreaViewContainer(QWidget *parent) : QWidget(parent)
{
    m_layout = new QGridLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    this->setFocusPolicy(Qt::StrongFocus);
}

medViewerAreaViewContainer::~medViewerAreaViewContainer(void)
{
    delete m_layout;
}

medViewerAreaViewContainer *medViewerAreaViewContainer::current(void)
{
    return s_current;
}

void medViewerAreaViewContainer::split(int rows, int cols)
{
    medViewerAreaViewContainer *current = this->current();

    if (!current)
        return;

    if (current->m_layout->count())
        return;

    for(int i = 0 ; i < rows ; i++)
        for(int j = 0 ; j < cols ; j++)
            current->m_layout->addWidget(new medViewerAreaViewContainer(this), i, j);
}

void medViewerAreaViewContainer::addWidget (QWidget *widget, int row, int col)
{
    widget->setParent (this);
    m_layout->addWidget (widget, row, col);
}

void medViewerAreaViewContainer::focusInEvent(QFocusEvent *event)
{
    s_current = this;
    QWidget::focusInEvent(event);
}

void medViewerAreaViewContainer::focusOutEvent(QFocusEvent *event)
{
    QWidget::focusOutEvent(event);
}

static void paintLayout(QPainter *painter, QLayoutItem *item)
{
    if (QLayout *layout = item->layout())
        for (int i = 0; i < layout->count(); ++i)
            paintLayout(painter, layout->itemAt(i));

    medViewerAreaViewContainer *view_container = dynamic_cast<medViewerAreaViewContainer *>(item->widget());

    if(!view_container)
        return;

    view_container->hasFocus() ? painter->setPen(Qt::red) : painter->setPen(Qt::gray);

    painter->drawRect(item->geometry().adjusted(0, 0, -1, -1));
}

void medViewerAreaViewContainer::paintEvent(QPaintEvent *event)
{
    qDebug() << "medViewerAreaViewContainer::paintEvent(QPaintEvent *event)";
    QPainter painter(this);

    if (layout())
        paintLayout(&painter, layout());
}

medViewerAreaViewContainer *medViewerAreaViewContainer::s_current = NULL;
