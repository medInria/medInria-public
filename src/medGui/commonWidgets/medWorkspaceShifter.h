/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medGuiExport.h"

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medWorkspaceShifterMenu
// /////////////////////////////////////////////////////////////////

class medWorkspaceShifterMenuPrivate;

class MEDGUI_EXPORT medWorkspaceShifterMenu : public QWidget
{
    Q_OBJECT
    
public:
     medWorkspaceShifterMenu(QWidget *parent = 0);
    ~medWorkspaceShifterMenu();

    void addAction(QAction *action);
    QAction *addAction(const QString& text);

    QSize sizeHint() const;

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
     medWorkspaceShifterAction(const QIcon& icon, const QString& text = "", QObject *parent = 0);
    ~medWorkspaceShifterAction();

signals:
    void triggered(bool checked = false);
    void toggled(bool checked);
    void hovered();

public:
    bool isChecked() const;
    bool isHovered() const;

    QIcon icon() const;
    void setIcon(const QIcon& icon);

    QString text() const;
    void setText(const QString& text);

    medWorkspaceShifterMenu *menu();
    void setMenu(medWorkspaceShifterMenu *menu);
    
public slots:
    void toggle();
    void trigger();
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
    ~medWorkspaceShifter();

public:
    medWorkspaceShifterAction *addAction(medWorkspaceShifterAction *action);
    medWorkspaceShifterAction *addAction(const QString& text);

    QSize minimumSizeHint() const;

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

    int calculateButtonWidth() const;

private:
    medWorkspaceShifterPrivate *d;
};


