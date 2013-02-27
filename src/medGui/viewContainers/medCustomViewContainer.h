/* medCustomViewContainer.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar 17 10:59:36 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri May 21 15:08:08 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 15
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDVIEWCONTAINERCUSTOM_H
#define MEDVIEWCONTAINERCUSTOM_H

#include "medGuiExport.h"

#include "medViewContainer.h"

class medCustomViewContainerPrivate;

class MEDGUI_EXPORT medCustomViewContainer : public medViewContainer
{
    Q_OBJECT

public:
    enum Preset {
        A = 1, // (1, 2)
        B = 2, // (2, 1)
        C = 3, // ((2, 1) s 1, 1 s 2)
        D = 4, // ((3, 1) s 1, 1 s 2)
        E = 5  // (2, 2)
    };

    medCustomViewContainer(QWidget *parent = 0);
    ~medCustomViewContainer(void);


    void split(int rows, int cols);

    void setPreset(int preset);
    void setView(dtkAbstractView *view);

    dtkAbstractView *view (void) const;
    QList<dtkAbstractView*> views (void) const;

    /**
     * @brief Is this a leaf container?
     *
     * This method is useful in the case of a custom or multi
     * container.  Returns true if this object is supposed to contain
     * only views and no other containers.
     *
     * @param void
     * @return bool
    */
    virtual bool isLeaf(void) const;

    virtual QString identifier() {return "Custom";}

public slots:
    void onViewClosing  (void);
    void onViewFullScreen (bool value);
    void clear (void);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    void synchronize_2 (dtkAbstractView *view);
    void desynchronize_2 (dtkAbstractView *view);

    void onViewFullScreen2 (bool value, dtkAbstractView *view);
    void fullScreen (bool value, dtkAbstractView *view);

    // QList<medCustomViewContainer*> children() const;
    medCustomViewContainerPrivate *d2;
};

#endif
