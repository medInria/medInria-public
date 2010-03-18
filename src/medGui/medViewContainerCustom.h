/* medViewContainerCustom.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 10:59:36 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar 17 18:06:37 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWCONTAINERCUSTOM_H
#define MEDVIEWCONTAINERCUSTOM_H

#include "medViewContainer.h"

class medViewContainerCustom : public medViewContainer
{
    Q_OBJECT

public:
    medViewContainerCustom(QWidget *parent = 0) : medViewContainer(parent) {}

    Type type(void);

    void split(int rows, int cols);

    void setView(dtkAbstractView *view);    

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
};

#endif
