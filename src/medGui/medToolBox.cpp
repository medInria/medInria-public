/* medToolBox.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 19:46:22 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct 16 09:48:07 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 161
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medStyle.h"
#include "medToolBox.h"

// /////////////////////////////////////////////////////////////////
// medToolBoxControl
// /////////////////////////////////////////////////////////////////

medToolBoxControl::medToolBoxControl(QWidget *parent) : QAbstractButton(parent)
{
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

medToolBoxControl::~medToolBoxControl(void)
{

}

QSize medToolBoxControl::sizeHint(void) const
{
    return QSize(16, 16);
}

void medToolBoxControl::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHints(QPainter::Antialiasing);
    this->isDown() ? painter.setPen(Qt::gray) : painter.setPen(Qt::lightGray);
    this->isDown() ? painter.setBrush(Qt::gray) : painter.setBrush(Qt::lightGray);
    painter.drawEllipse(event->rect().adjusted(2, 2, -2, -2));
    painter.setPen(QColor(0x49, 0x49, 0x49));
    painter.setBrush(QColor(0x49, 0x49, 0x49));
    painter.drawEllipse(event->rect().adjusted(4, 4, -4, -4));
    painter.end();
}

// /////////////////////////////////////////////////////////////////
// medToolBoxHeader
// /////////////////////////////////////////////////////////////////

class medToolBoxHeaderPrivate
{
public:
    QLabel *label;

    medToolBoxControl *control;
};

medToolBoxHeader::medToolBoxHeader(QWidget *parent) : QWidget(parent), d(new medToolBoxHeaderPrivate)
{
    d->label = new QLabel(this);
    d->label->setText("Untitled");
    d->label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    d->control = new medToolBoxControl(this);

    connect(d->control, SIGNAL(clicked()), this, SIGNAL(clicked()));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->setSpacing(0);
    layout->addWidget(d->label);
    layout->addWidget(d->control);

    this->setStyleSheet(
     "color: white;"
     "font-size: 10px;"
    );

    this->setFixedHeight(20);
}

medToolBoxHeader::~medToolBoxHeader(void)
{
    delete d->label;
    delete d->control;
    delete d;

    d = NULL;
}

void medToolBoxHeader::setTitle(const QString& title)
{
    d->label->setText(title);
}

void medToolBoxHeader::paintEvent(QPaintEvent *event)
{
    QPainter p(this); p.setRenderHint(QPainter::Antialiasing);

    QColor gradientUp = QColor(0x4b, 0x4b, 0x4b);
    QColor gradientMd = QColor(0x2e, 0x2e, 0x2e);
    QColor gradientDw = QColor(0x4b, 0x4b, 0x4b);

    medStyle::medFillRoundRectWithDoubleLinearGradient(&p, event->rect(), gradientUp, gradientMd, gradientMd, gradientDw, 4, 0, 4, 0);

    p.end();
}

// /////////////////////////////////////////////////////////////////
// medToolBoxBody
// /////////////////////////////////////////////////////////////////

class medToolBoxBodyPrivate
{
public:
    int height;
    int row;
    int col;
    QGridLayout *layout;

    QTimeLine timeline;
};

medToolBoxBody::medToolBoxBody(QWidget *parent) : QWidget(parent), d(new medToolBoxBodyPrivate)
{    
    d->layout = new QGridLayout(this);

    d->row = 0;
    d->col = 0;

    d->timeline.setDuration(500);
    d->timeline.setCurveShape(QTimeLine::EaseInOutCurve);

    connect(&(d->timeline), SIGNAL(frameChanged(int)), this, SLOT(animate(int)));
}

medToolBoxBody::~medToolBoxBody(void)
{
    delete d->layout;
    delete d;

    d = NULL;
}

void medToolBoxBody::addWidget(QWidget *widget)
{
    d->layout->addWidget(widget, d->row, d->col);

    if (d->col > 3) {
        d->row++;
        d->col = 0;
    } else {
        d->col++;
    }
}

void medToolBoxBody::toggle(void)
{
    if (this->height())
        this->collapse();
    else
        this->expand();
}

void medToolBoxBody::expand(void)
{
    if (d->timeline.currentFrame())
        return;

    setUpdatesEnabled(false);
    
    d->timeline.toggleDirection();
    d->timeline.start();

    setUpdatesEnabled(true);
}

void medToolBoxBody::collapse(void)
{
    if (d->timeline.currentFrame() < d->height)
        return;

    setUpdatesEnabled(false);
    
    d->height = d->layout->geometry().height();

    int oldHeight = d->height;
    int newHeight = 0;

    if (oldHeight > 0) {
	d->timeline.setFrameRange(oldHeight, newHeight);
        d->timeline.setDirection(QTimeLine::Forward);
	d->timeline.start();
    } else {
	this->setFixedHeight(newHeight);
    }

    setUpdatesEnabled(true);
}

void medToolBoxBody::animate(int frame) 
{
    this->setFixedHeight(frame);
}

void medToolBoxBody::paintEvent(QPaintEvent *event)
{
    QPainter p(this); p.setRenderHint(QPainter::Antialiasing);

    medStyle::medFillRoundRectWithColor(&p, event->rect().adjusted(0, -1, 0, 0), palette().color(QPalette::Base), 0, 4, 0, 4);

    p.end();
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

    connect(d->header, SIGNAL(clicked()), d->body, SLOT(toggle()));

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
}

void medToolBox::addWidget(QWidget *widget)
{
    d->body->addWidget(widget);
}

void medToolBox::setTitle(const QString &title)
{
    d->header->setTitle(title);
}
