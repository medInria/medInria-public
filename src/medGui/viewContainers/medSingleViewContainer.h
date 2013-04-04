/* medSingleViewContainer.h ---
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

#pragma once

#include "medGuiExport.h"

#include "medViewContainer.h"

class MEDGUI_EXPORT medSingleViewContainer : public medViewContainer
{
    Q_OBJECT

public:
    medSingleViewContainer(QWidget *parent = 0) : medViewContainer(parent) {}


    void split(int rows, int cols);

    virtual void setView(dtkAbstractView *view);
    virtual dtkAbstractView *view() const;

    /**
     * @brief Is this a leaf container?
     *
     * This method is useful in the case of a custom container.
     * Returns true if this object is supposed to contain
     * only views and no other containers (like
     * medSingleViewContainer)
     *
     * @param void
     * @return bool
    */
    virtual bool isLeaf() const;

    virtual QString identifier() {return "Single";}

    virtual QString description() {return "Single";}

public slots:
    virtual void onViewClosing();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
};


