/* medWorkspaceShifter.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 11:11:47 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Jun  9 12:47:50 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 8
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDWORKSPACESHIFTER_H
#define MEDWORKSPACESHIFTER_H

#include "medGuiExport.h"

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medWorkspaceShifterMenu
// /////////////////////////////////////////////////////////////////

class medWorkspaceShifterMenuPrivate;

class medWorkspaceShifterMenu : public QWidget
{
    Q_OBJECT
    
public:
     medWorkspaceShifterMenu(QWidget *parent = 0);
    ~medWorkspaceShifterMenu(void);

    void addAction(QAction *action);
    QAction *addAction(const QString& text);

    QSize sizeHint(void) const;

signals:
    void triggered(QAction *action);
    
protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    
private:
    medWorkspaceShifterMenuPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////

class medWorkspaceShifterActionPrivate;

class MEDGUI_EXPORT medWorkspaceShifterAction : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(QString text READ text WRITE setText)

public:
     medWorkspaceShifterAction(QObject *parent = 0);
     medWorkspaceShifterAction(const QString& text, QObject *parent = 0);
     medWorkspaceShifterAction(const QIcon& icon, const QString& text, QObject *parent = 0);
    ~medWorkspaceShifterAction(void);

signals:
    void triggered(bool checked = false);
    void toggled(bool checked);
    void hovered(void);

public:
    bool isChecked(void) const;
    bool isHovered(void) const;

    QIcon icon(void) const;
    void setIcon(const QIcon& icon);

    QString text(void) const;
    void setText(const QString& text);

    medWorkspaceShifterMenu *menu(void);
    void setMenu(medWorkspaceShifterMenu *menu);
    
public slots:
    void toggle(void);
    void trigger(void);
    void hover(bool isHovered = false);
    void setChecked(bool checked);

private:
    class medWorkspaceShifterActionPrivate *d;
};

class medWorkspaceShifterPrivate;

class MEDGUI_EXPORT medWorkspaceShifter : public QWidget
{
    Q_OBJECT

public:
     medWorkspaceShifter(QWidget *parent = 0);
    ~medWorkspaceShifter(void);

public:
    medWorkspaceShifterAction *addAction(medWorkspaceShifterAction *action);
    medWorkspaceShifterAction *addAction(const QString& text);

    QSize minimumSizeHint(void) const;

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    void drawUnselectedButton(QPainter *painter, const QRect& rect, const medWorkspaceShifterAction *action);
    void drawSelectedButton(QPainter *painter, const QRect& rect, const medWorkspaceShifterAction *action);
    void drawButton(QPainter *painter, const QRect& rect, const medWorkspaceShifterAction *action);
    void drawButton(QPainter *painter, const QRect& rect, const QLinearGradient& gradient, const QColor& color, const medWorkspaceShifterAction *action);

    medWorkspaceShifterAction *hoveredAction(const QPoint& pos) const;

    int calculateButtonWidth(void) const;

private:
    medWorkspaceShifterPrivate *d;
};

#endif
