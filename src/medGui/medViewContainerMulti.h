/* medViewContainerMulti.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 10:58:53 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar 17 18:47:49 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWCONTAINERMULTI_H
#define MEDVIEWCONTAINERMULTI_H

#include "medGuiExport.h"

#include "medViewContainer.h"
#include "medViewContainerSingle.h"

class MEDGUI_EXPORT medViewContainerSingle2 : public medViewContainerSingle
{
    Q_OBJECT

public:
     medViewContainerSingle2(QWidget *parent = 0) : medViewContainerSingle(parent) {}
    ~medViewContainerSingle2();
    
    void setView(dtkAbstractView *view);

public slots:
    virtual void onViewClosing (void);
};


class medViewContainerMultiPrivate;

class MEDGUI_EXPORT medViewContainerMulti : public medViewContainer
{
    Q_OBJECT

public:
    medViewContainerMulti(QWidget *parent = 0);
   ~medViewContainerMulti();

    Type type(void);

    void split(int rows, int cols);

    dtkAbstractView *view(void) const;

    QList<dtkAbstractView*> views (void) const;

    void setView(dtkAbstractView *view);

    void layout (QList<QWidget *> content);

public slots:
    void onViewClosing    (void);
    void onViewFullScreen (bool value);
   
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

    medViewContainerMultiPrivate *d2;
};

#endif
