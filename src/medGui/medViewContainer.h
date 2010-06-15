/* medViewContainer.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct 26 21:53:58 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 16:21:52 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 32
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
    enum Type {
        Single,
        Multi,
        Custom,
        Default
    };

     medViewContainer(QWidget *parent = 0);
    ~medViewContainer(void);

    Type type(void);

    medViewContainer *current(void);

    virtual dtkAbstractView *view(void);

    virtual void setView(dtkAbstractView *view);

signals:
    void dropped(const medDataIndex& index);
    void focused(dtkAbstractView *view);

public slots:
    virtual void split(int rows, int cols);

protected:
    void setCurrent(medViewContainer *container);
    
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void paintEvent(QPaintEvent *event);

protected:
    medViewContainerPrivate *d;
};

#endif // MEDVIEWCONTAINER_H
