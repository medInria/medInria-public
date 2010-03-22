/* medToolBox.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 19:46:22 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Sat Mar 20 20:06:52 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 202
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBox.h"

// /////////////////////////////////////////////////////////////////
// medToolBoxStack
// /////////////////////////////////////////////////////////////////

class medToolBoxStackPrivate
{
public:
};

medToolBoxStack::medToolBoxStack(QWidget *parent) : QStackedWidget(parent), d(new medToolBoxStackPrivate)
{

}

medToolBoxStack::~medToolBoxStack(void)
{
    delete d;

    d = NULL;
}

// /////////////////////////////////////////////////////////////////
// medToolBoxTab
// /////////////////////////////////////////////////////////////////

class medToolBoxTabPrivate
{
public:
};

medToolBoxTab::medToolBoxTab(QWidget *parent) : QTabWidget(parent), d(new medToolBoxTabPrivate)
{

}

medToolBoxTab::~medToolBoxTab(void)
{
    delete d;

    d = NULL;
}

void medToolBoxTab::paintEvent(QPaintEvent *event)
{
    static int height = 33;

    QLinearGradient gradient;
    gradient.setStart(0, 0);
    gradient.setFinalStop(0, height);
    gradient.setColorAt(0, QColor("#3b3b3c"));
    gradient.setColorAt(1, QColor("#2d2d2f"));

    QPainter painter(this);
    painter.setPen(QColor("#2c2c2e"));
    painter.setBrush(gradient);
    painter.drawRect(QRect(0, 0, this->width(), height));
    painter.end();

    QTabWidget::paintEvent(event);
}

// /////////////////////////////////////////////////////////////////
// medToolBoxHeader
// /////////////////////////////////////////////////////////////////

class medToolBoxHeaderPrivate
{
public:
    QLabel *label;
};

medToolBoxHeader::medToolBoxHeader(QWidget *parent) : QFrame(parent), d(new medToolBoxHeaderPrivate)
{
    d->label = new QLabel(this);
    d->label->setText("Untitled");
    d->label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->setSpacing(0);
    layout->addWidget(d->label);
}

medToolBoxHeader::~medToolBoxHeader(void)
{
    delete d;

    d = NULL;
}

void medToolBoxHeader::setTitle(const QString& title)
{
    d->label->setText(title);
}

// /////////////////////////////////////////////////////////////////
// medToolBoxBody
// /////////////////////////////////////////////////////////////////

class medToolBoxBodyPrivate
{
public:
    QVBoxLayout *layout;
};

medToolBoxBody::medToolBoxBody(QWidget *parent) : QFrame(parent), d(new medToolBoxBodyPrivate)
{    
    d->layout = new QVBoxLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);
}

medToolBoxBody::~medToolBoxBody(void)
{
    delete d->layout;
    delete d;

    d = NULL;
}

void medToolBoxBody::setWidget(QWidget *widget)
{
    d->layout->addWidget(widget);

    widget->setParent(this);
}

// /////////////////////////////////////////////////////////////////
// medToolBox
// /////////////////////////////////////////////////////////////////

class medToolBoxPrivate
{
public:
    medToolBoxHeader *header;
    medToolBoxBody *body;
};

medToolBox::medToolBox(QWidget *parent) : QWidget(parent), d(new medToolBoxPrivate)
{
    d->header = new medToolBoxHeader(this);
    d->body = new medToolBoxBody(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->header);
    layout->addWidget(d->body);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

medToolBox::~medToolBox(void)
{
    delete d->header;
    delete d->body;
    delete d;

    d = NULL;
}

void medToolBox::setWidget(QWidget *widget)
{
    d->body->setWidget(widget);
}

void medToolBox::setTitle(const QString &title)
{
    d->header->setTitle(title);
}
