/* medViewContainerSingle.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 10:55:32 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar 17 14:39:35 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 10
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWCONTAINERSINGLE_H
#define MEDVIEWCONTAINERSINGLE_H

#include "medGuiExport.h"

#include "medViewContainer.h"

class MEDGUI_EXPORT medViewContainerSingle : public medViewContainer
{
    Q_OBJECT

public:
    medViewContainerSingle(QWidget *parent = 0) : medViewContainer(parent) {}


    void split(int rows, int cols);

    virtual void setView(dtkAbstractView *view);
    virtual dtkAbstractView *view (void) const;

    /**
     * @brief Is this a leaf container?
     *
     * This method is useful in the case of a custom or multi
     * container.  Returns true if this object is supposed to contain
     * only views and no other containers (like
     * medViewContainerSingle)
     *
     * @param void
     * @return bool
    */
    virtual bool isLeaf(void) const;

public slots:
    virtual void onViewClosing (void);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif
