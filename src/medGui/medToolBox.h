/* medToolBox.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 19:41:48 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  7 13:04:19 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 59
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

///////////////////////////////////////////////////////////////////
// medToolBoxStack
///////////////////////////////////////////////////////////////////

class medToolBoxStackPrivate;

class MEDGUI_EXPORT medToolBoxStack : public QStackedWidget
{
    Q_OBJECT

public:
     medToolBoxStack(QWidget *parent = 0);
    ~medToolBoxStack(void);

private:
    medToolBoxStackPrivate *d;
};

///////////////////////////////////////////////////////////////////
// medToolBoxTab
///////////////////////////////////////////////////////////////////

class medToolBoxTabPrivate;

class MEDGUI_EXPORT medToolBoxTab : public QTabWidget
{
    Q_OBJECT

    Q_PROPERTY(int height READ height WRITE setFixedHeight)

public:
     medToolBoxTab(QWidget *parent = 0);
    ~medToolBoxTab(void);

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
