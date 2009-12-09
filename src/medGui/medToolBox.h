/* medToolBox.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 19:41:48 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Nov  3 14:55:21 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 47
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOX_H
#define MEDTOOLBOX_H

#include "medGuiExport.h"

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medToolBoxControl
// /////////////////////////////////////////////////////////////////

class MEDGUI_EXPORT medToolBoxControl : public QAbstractButton
{
    Q_OBJECT

public:
     medToolBoxControl(QWidget *parent);
    ~medToolBoxControl(void);

    QSize sizeHint(void) const;

protected:
    void paintEvent(QPaintEvent *event);
};

///////////////////////////////////////////////////////////////////
// medToolBoxStack
///////////////////////////////////////////////////////////////////

class medToolBoxStackPrivate;

class medToolBoxStack : public QStackedWidget
{
    Q_OBJECT

    Q_PROPERTY(int height READ height WRITE setFixedHeight)

public:
     medToolBoxStack(QWidget *parent = 0);
    ~medToolBoxStack(void);

public slots:
    void onCurrentChanged(int index);

private:
    medToolBoxStackPrivate *d;
};

///////////////////////////////////////////////////////////////////
// medToolBoxTab
///////////////////////////////////////////////////////////////////

class medToolBoxTabPrivate;

class medToolBoxTab : public QTabWidget
{
    Q_OBJECT

    Q_PROPERTY(int height READ height WRITE setFixedHeight)

public:
     medToolBoxTab(QWidget *parent = 0);
    ~medToolBoxTab(void);

public slots:
    void onCurrentChanged(int index);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medToolBoxTabPrivate *d;
};

///////////////////////////////////////////////////////////////////
// medToolBoxHeader
///////////////////////////////////////////////////////////////////

class medToolBoxHeaderPrivate;

class MEDGUI_EXPORT medToolBoxHeader : public QFrame
{
    Q_OBJECT

public:
     medToolBoxHeader(QWidget *parent = 0);
    ~medToolBoxHeader(void);

    void setTitle(const QString& title);

signals:
    void clicked(void);

private:
    medToolBoxHeaderPrivate *d;
};

///////////////////////////////////////////////////////////////////
// medToolBoxBody
///////////////////////////////////////////////////////////////////

class medToolBoxBodyPrivate;

class MEDGUI_EXPORT medToolBoxBody : public QFrame
{
    Q_OBJECT

public:
     medToolBoxBody(QWidget *parent = 0);
    ~medToolBoxBody(void);

    void setWidget(QWidget *widget);

public slots:
    void toggle(void);
    void expand(void);
    void collapse(void);
    void animate(int frame);

private:
    medToolBoxBodyPrivate *d;
};

///////////////////////////////////////////////////////////////////
// medToolBox
///////////////////////////////////////////////////////////////////

class medToolBoxPrivate;

class MEDGUI_EXPORT medToolBox : public QWidget
{
    Q_OBJECT

public:
     medToolBox(QWidget *parent = 0);
    ~medToolBox(void);

    void setWidget(QWidget *widget);

    void setTitle(const QString& title);

private:
    medToolBoxPrivate *d;
};

#endif
