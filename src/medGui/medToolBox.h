/* medToolBox.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 19:41:48 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct  9 23:28:25 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 32
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOX_H
#define MEDTOOLBOX_H

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medToolBoxControl
// /////////////////////////////////////////////////////////////////

class medToolBoxControl : public QAbstractButton
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
// medToolBoxHeader
///////////////////////////////////////////////////////////////////

class medToolBoxHeaderPrivate;

class medToolBoxHeader : public QWidget
{
    Q_OBJECT

public:
     medToolBoxHeader(QWidget *parent = 0);
    ~medToolBoxHeader(void);

    void setTitle(const QString& title);

signals:
    void clicked(void);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medToolBoxHeaderPrivate *d;
};

///////////////////////////////////////////////////////////////////
// medToolBoxBody
///////////////////////////////////////////////////////////////////

class medToolBoxBodyPrivate;

class medToolBoxBody : public QWidget
{
    Q_OBJECT

public:
     medToolBoxBody(QWidget *parent = 0);
    ~medToolBoxBody(void);

    void addWidget(QWidget *widget);

public slots:
    void toggle(void);
    void expand(void);
    void collapse(void);
    void animate(int frame);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medToolBoxBodyPrivate *d;
};

///////////////////////////////////////////////////////////////////
// medToolBox
///////////////////////////////////////////////////////////////////

class medToolBoxPrivate;

class medToolBox : public QWidget
{
    Q_OBJECT

public:
     medToolBox(QWidget *parent = 0);
    ~medToolBox(void);

    void addWidget(QWidget *widget);

    void setTitle(const QString& title);

private:
    medToolBoxPrivate *d;
};

#endif
