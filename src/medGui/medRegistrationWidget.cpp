/* medRegistrationWidget.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jan 18 11:14:54 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Jan 18 12:37:11 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 24
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medRegistrationWidget.h"

#include <QtCore>
#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medRegistrationWidgetItem
// /////////////////////////////////////////////////////////////////

class medRegistrationWidgetItemPrivate
{
public:
};

medRegistrationWidgetItem::medRegistrationWidgetItem(QWidget *parent) : QLabel(parent), d(new medRegistrationWidgetItemPrivate)
{
    setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
    setBackgroundRole(QPalette::Base);
}

medRegistrationWidgetItem::~medRegistrationWidgetItem(void)
{
    delete d;

    d = NULL;
}

QSize medRegistrationWidgetItem::sizeHint(void) const
{
    return QSize(128, 128);
}

void medRegistrationWidgetItem::dragEnterEvent(QDragEnterEvent *event)
{
    setBackgroundRole(QPalette::Highlight);

    event->acceptProposedAction();
}

void medRegistrationWidgetItem::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void medRegistrationWidgetItem::dragLeaveEvent(QDragLeaveEvent *event)
{
    setBackgroundRole(QPalette::Base);

    event->accept();
}

void medRegistrationWidgetItem::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasImage()) {
        setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
    }

    setBackgroundRole(QPalette::Base);

    event->acceptProposedAction();
}

void medRegistrationWidgetItem::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

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

// /////////////////////////////////////////////////////////////////
// medRegistrationWidget
// /////////////////////////////////////////////////////////////////

class medRegistrationWidgetPrivate
{
public:
    medRegistrationWidgetItem *item1;
    medRegistrationWidgetItem *item2;

    QPushButton *registerButton;
};

medRegistrationWidget::medRegistrationWidget(QWidget *parent) : QWidget(parent), d(new medRegistrationWidgetPrivate)
{
    d->item1 = new medRegistrationWidgetItem(this);
    d->item1->setText("Fixed");

    d->item2 = new medRegistrationWidgetItem(this);
    d->item2->setText("Moving");

    d->registerButton = new QPushButton("Register", this);

    QHBoxLayout *items_layout = new QHBoxLayout;
    items_layout->addWidget(d->item1);
    items_layout->addWidget(d->item2);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(items_layout);
    layout->addWidget(d->registerButton);
}

medRegistrationWidget::~medRegistrationWidget(void)
{
    delete d;

    d = NULL;
}
