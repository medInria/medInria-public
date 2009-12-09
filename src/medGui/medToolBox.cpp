/* medToolBox.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 19:46:22 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Nov  3 16:19:03 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 188
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBox.h"

// /////////////////////////////////////////////////////////////////
// medToolBoxControl
// /////////////////////////////////////////////////////////////////

medToolBoxControl::medToolBoxControl(QWidget *parent) : QAbstractButton(parent)
{
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
    painter.setBackgroundMode(Qt::TransparentMode);
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
// medToolBoxStack
// /////////////////////////////////////////////////////////////////

class medToolBoxStackPrivate
{
public:
    QPropertyAnimation *height_animation;
};

medToolBoxStack::medToolBoxStack(QWidget *parent) : QStackedWidget(parent), d(new medToolBoxStackPrivate)
{
    d->height_animation = new QPropertyAnimation(this, "height");

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
}

medToolBoxStack::~medToolBoxStack(void)
{
    delete d;

    d = NULL;
}

void medToolBoxStack::onCurrentChanged(int index)
{
    static int old_height = 0;
    static int new_height = 0;

    new_height = this->widget(index)->sizeHint().height() + 33 + 1;

    if(old_height) {
//        qDebug() << old_height;
//        qDebug() << new_height;
        d->height_animation->setStartValue(old_height);
        d->height_animation->setEndValue(new_height);
        d->height_animation->start();
    }

    old_height = new_height;
}

// /////////////////////////////////////////////////////////////////
// medToolBoxTab
// /////////////////////////////////////////////////////////////////

class medToolBoxTabPrivate
{
public:
    QPropertyAnimation *height_animation;
};

medToolBoxTab::medToolBoxTab(QWidget *parent) : QTabWidget(parent), d(new medToolBoxTabPrivate)
{
    d->height_animation = new QPropertyAnimation(this, "height");

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
}

medToolBoxTab::~medToolBoxTab(void)
{
    delete d;

    d = NULL;
}

void medToolBoxTab::onCurrentChanged(int index)
{
    static int old_height = 0;
    static int new_height = 0;

    new_height = this->widget(index)->sizeHint().height() + 33 + 1;

    if(old_height) {
//        qDebug() << old_height;
//        qDebug() << new_height;
        d->height_animation->setStartValue(old_height);
        d->height_animation->setEndValue(new_height);
        d->height_animation->start();
    }

    old_height = new_height;
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

    medToolBoxControl *control;
};

medToolBoxHeader::medToolBoxHeader(QWidget *parent) : QFrame(parent), d(new medToolBoxHeaderPrivate)
{
    d->label = new QLabel(this);
    d->label->setText("Untitled");
    d->label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

//    d->control = new medToolBoxControl(this);

//    connect(d->control, SIGNAL(clicked()), this, SIGNAL(clicked()));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->setSpacing(0);
    layout->addWidget(d->label);
//    layout->addWidget(d->control);
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
    int height;

    QVBoxLayout *layout;

    QTimeLine timeline;
};

medToolBoxBody::medToolBoxBody(QWidget *parent) : QFrame(parent), d(new medToolBoxBodyPrivate)
{    
    d->layout = new QVBoxLayout(this);
    d->layout->setContentsMargins(0, 0, 0, 0);

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

void medToolBoxBody::setWidget(QWidget *widget)
{
    d->layout->addWidget(widget);

    widget->setParent(this);
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
    
    d->height = this->height();

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
