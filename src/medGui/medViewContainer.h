/* medViewContainer.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct 26 21:53:58 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Mar  4 21:51:59 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 16
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWCONTAINER_H
#define MEDVIEWCONTAINER_H

#include "medGuiExport.h"

#include <QtGui/QWidget>

class dtkAbstractView;
class medDataIndex;

class medViewContainerPrivate;

class MEDGUI_EXPORT medViewContainer : public QWidget
{
    Q_OBJECT

public:
     medViewContainer(QWidget *parent = 0);
    ~medViewContainer(void);

    medViewContainer *current(void);

    dtkAbstractView *view(void);

    void setView(dtkAbstractView *view);

signals:
    void dropped(const medDataIndex& index);
    void focused(dtkAbstractView *view);

public slots:
    void split(int rows, int cols);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    static medViewContainer *s_current;

private:
    medViewContainerPrivate *d;
};

#endif // MEDVIEWCONTAINER_H
