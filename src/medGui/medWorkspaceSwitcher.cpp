/* medWorkspaceSwitcher.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 11 14:43:13 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Nov 12 09:58:36 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 45
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medWorkspaceSwitcher.h"

medWorkspaceSwitcherItem::medWorkspaceSwitcherItem(int index, const QString& text, const QPixmap& pixmap)
{
    m_index = index;

    m_pixmap = new QGraphicsPixmapItem(pixmap, this);

    m_text = new QGraphicsTextItem(text, this);
    m_text->setDefaultTextColor(Qt::white);
    m_text->setPos(m_pixmap->boundingRect().bottomLeft());

    m_drop_shadow_effect = new QGraphicsDropShadowEffect;
    m_drop_shadow_effect->setColor(Qt::white);
    m_drop_shadow_effect->setBlurRadius(5);
    m_drop_shadow_effect->setOffset(3);

    this->setGraphicsEffect(m_drop_shadow_effect);

    m_hovered = false;

    this->setAcceptHoverEvents(true);
}

QRectF medWorkspaceSwitcherItem::boundingRect(void) const
{
    return m_pixmap->boundingRect();
}

void medWorkspaceSwitcherItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(m_hovered) {
        painter->save();
        painter->setPen(QPen(Qt::cyan, 2, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
        painter->drawRect(option->rect);
        painter->restore();
    }
}

void medWorkspaceSwitcherItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    emit triggered(m_index);
}

void medWorkspaceSwitcherItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_hovered = true;

    QGraphicsItem::hoverEnterEvent(event);
}

void medWorkspaceSwitcherItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_hovered = false;

    QGraphicsItem::hoverEnterEvent(event);
}

////////////////////////////////////////////////////////////////////////////////////////

medWorkspaceSwitcherScene::medWorkspaceSwitcherScene()
{

}

medWorkspaceSwitcherScene::~medWorkspaceSwitcherScene()
{

}

////////////////////////////////////////////////////////////////////////////////////////

class medWorkspaceSwitcherViewPrivate
{
public:

};

medWorkspaceSwitcherView::medWorkspaceSwitcherView(medWorkspaceSwitcherScene *scene, QWidget *parent) : QGraphicsView(scene, parent), d(new medWorkspaceSwitcherViewPrivate)
{
    this->viewport()->setMouseTracking(true);

//    this->setCacheMode(QGraphicsView::CacheNone);
//    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    this->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setFrameStyle(QFrame::NoFrame);
}

medWorkspaceSwitcherView::~medWorkspaceSwitcherView(void)
{
    delete d;

    d = NULL;
}

void medWorkspaceSwitcherView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
}

////////////////////////////////////////////////////////////////////////////////////////

medWorkspaceSwitcher::medWorkspaceSwitcher(QWidget *parent) : QWidget(parent)
{
    m_scene = new medWorkspaceSwitcherScene;
    m_scene->addItem(new QGraphicsTextItem("prout"));

    m_view = new medWorkspaceSwitcherView(m_scene, this);
    m_view->setStyleSheet("background: transparent;");
    m_view->hide();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_view);

    m_fade_in_animation = new QPropertyAnimation(this, "opacity");
    m_fade_in_animation->setDuration(175);

    m_fade_ot_animation = new QPropertyAnimation(this, "opacity");
    m_fade_ot_animation->setDuration(175);

    connect(m_fade_in_animation, SIGNAL(finished()), m_view, SLOT(show()));
    connect(m_fade_ot_animation, SIGNAL(finished()), this, SLOT(hide()));

    m_opacity = 0.0;

    this->setVisible(false);
}

void medWorkspaceSwitcher::addWorkspace(QWidget *widget)
{
    m_workspaces << widget;
}

qreal medWorkspaceSwitcher::opacity(void)
{
    return m_opacity;
}

void medWorkspaceSwitcher::setOpacity(qreal opacity)
{
    m_opacity = opacity;

    this->update();
}

void medWorkspaceSwitcher::start(void)
{
    if(m_opacity == 0.8)
        return;

    m_scene->clear();

    static int margins = 10;
    static int spacing = 10;

    int count = 0;

    foreach(QWidget *widget, m_workspaces) {
        qreal width  = this->size().width()/m_workspaces.count() - 2*margins - spacing;
        qreal height = this->size().height()/m_workspaces.count() - 2*margins - spacing;

        QPixmap pixmap(this->size()); widget->render(&pixmap);

        medWorkspaceSwitcherItem *item = new medWorkspaceSwitcherItem(count, widget->objectName(), pixmap.scaledToWidth(width, Qt::SmoothTransformation));
        item->setPos(margins + count*spacing + count*width, margins);

        connect(item, SIGNAL(triggered(int)), this, SIGNAL(triggered(int)));
        connect(item, SIGNAL(triggered(int)), this, SLOT(stop()));

        m_scene->addItem(item);

        count++;
    }

    this->show();

    m_fade_in_animation->setStartValue(0.0);
    m_fade_in_animation->setEndValue(0.8);
    m_fade_in_animation->start();
}

void medWorkspaceSwitcher::stop(void)
{
    if(m_opacity == 0.0)
        return;

    m_view->hide();

    m_fade_ot_animation->setStartValue(0.8);
    m_fade_ot_animation->setEndValue(0.0);
    m_fade_ot_animation->start();
}

void medWorkspaceSwitcher::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(m_opacity);
    painter.fillRect(event->rect(), Qt::black);
}
