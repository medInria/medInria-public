/* medStatusPanel.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Oct 11 13:02:28 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct 20 09:59:04 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 92
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medStatusPanel.h"

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medStatusWidget
// /////////////////////////////////////////////////////////////////

// class medStatusWidgetPrivate
// {
// public:
//     QFormLayout *layout;
// };

// medStatusWidget::medStatusWidget(QWidget *parent) : QWidget(parent), d(new medStatusWidgetPrivate)
// {
//     d->layout = new QFormLayout(this);
// }

// medStatusWidget::~medStatusWidget(void)
// {
//     delete d;

//     d = NULL;
// }

// void medStatusWidget::addField(const QString& label, QString field)
// {
//     d->layout->addRow(field, new QLabel(field, this));
// }

// void medStatusWidget::setField(const QString& label, QString field)
// {
//     if(QLabel *label = d->labelForField(label))
//     label->setText(value);
// }

// /////////////////////////////////////////////////////////////////
// medStatusPanel
// /////////////////////////////////////////////////////////////////

class medStatusPanelPrivate
{
public:
    QHBoxLayout *layout;
    int spacing;
};

medStatusPanel::medStatusPanel(QWidget *parent) : QWidget(parent), d(new medStatusPanelPrivate)
{
    d->spacing = 12;

    d->layout = new QHBoxLayout(this);
    d->layout->setContentsMargins(5, 0, 5, 0);
    d->layout->setSpacing(d->spacing);

    this->setStyleSheet("font-size: 9px;");
    this->setFixedHeight(32);
}

medStatusPanel::~medStatusPanel(void)
{
    delete d;

    d = NULL;
}

void medStatusPanel::addSpacer(void)
{
    d->layout->addStretch(2);
}

void medStatusPanel::addWidget(QWidget *widget)
{
    d->layout->addWidget(widget);
}

void medStatusPanel::removeWidget(QWidget *widget)
{
    d->layout->removeWidget(widget);
}

static void paintLayout(QPainter *painter, QLayout *layout, int spacing)
{
    for (int i = 1; i < layout->count(); ++i) {

        QLayoutItem *item = layout->itemAt(i);
          
        QPoint tl(item->geometry().topLeft().x() - spacing/2, item->geometry().topLeft().y());
        QPoint bl(item->geometry().bottomLeft().x() - spacing/2, item->geometry().bottomLeft().y());

        painter->save();
        painter->setPen(Qt::black);
        painter->drawLine(tl, bl);
        painter->restore();
    }
}

void medStatusPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setPen(Qt::black);
    painter.setBrush(QColor(0x31, 0x31, 0x31).lighter(150));
    painter.drawRect(event->rect().adjusted(-1, 0, 0, 0));
    paintLayout(&painter, this->layout(), d->spacing);
    painter.end();

    QWidget::paintEvent(event);
}
