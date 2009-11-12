/* medWorkspaceSwitcher.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 11 14:41:33 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 11 20:11:02 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDWORKSPACESWITCHER_H
#define MEDWORKSPACESWITCHER_H

#include <QtGui>

class medWorkspaceSwitcherItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_INTERFACES(QGraphicsItem)

public:
    medWorkspaceSwitcherItem(int index, const QString& text, const QPixmap& pixmap);

    QRectF boundingRect(void) const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    void triggered(int);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    int m_index;

    QGraphicsTextItem *m_text;
    QGraphicsPixmapItem *m_pixmap;

    QGraphicsDropShadowEffect *m_drop_shadow_effect;

    bool m_hovered;
};

//////////////////////////////////////////////////////////////

class medWorkspaceSwitcherScene : public QGraphicsScene
{
public:
     medWorkspaceSwitcherScene(void);
    ~medWorkspaceSwitcherScene(void);
};

//////////////////////////////////////////////////////////////

class medWorkspaceSwitcherViewPrivate;

class medWorkspaceSwitcherView : public QGraphicsView
{
    Q_OBJECT

public:
     medWorkspaceSwitcherView(medWorkspaceSwitcherScene *scene, QWidget *parent = 0);
    ~medWorkspaceSwitcherView(void);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    medWorkspaceSwitcherViewPrivate *d;
};

//////////////////////////////////////////////////////////////

class medWorkspaceSwitcher : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    medWorkspaceSwitcher(QWidget *parent);

    void addWorkspace(QWidget *widget);

    qreal opacity(void);

    void setOpacity(qreal opacity);

signals:
    void triggered(int);

public slots:
    void start(void);
    void stop(void);

protected:
    void paintEvent(QPaintEvent *event);

private:
    qreal m_opacity;

    QPropertyAnimation *m_fade_in_animation;
    QPropertyAnimation *m_fade_ot_animation;

    medWorkspaceSwitcherScene *m_scene;
    medWorkspaceSwitcherView *m_view;

    QList<QWidget *> m_workspaces;
};

#endif
