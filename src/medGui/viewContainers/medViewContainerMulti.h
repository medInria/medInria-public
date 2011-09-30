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

    /**
     * @brief Is this a leaf container?
     *
     * This method is useful in the case of a custom or multi
     * container.  Returns true if this object is supposed to contain
     * only views and no other containers (like
     * medViewContainerSingle2).
     *
     * @param void
     * @return bool
    */
    virtual bool isLeaf(void) const;

public slots:
    virtual void onViewClosing (void);

    virtual void onViewFocused (bool value);
};


class medViewContainerMultiPrivate;

class MEDGUI_EXPORT medViewContainerMulti : public medViewContainer
{
    Q_OBJECT

public:
    medViewContainerMulti(QWidget *parent = 0);
   ~medViewContainerMulti();


    void split(int rows, int cols);

    dtkAbstractView *view(void) const;

    QList<dtkAbstractView*> views (void) const;

    /**
     * @brief Overrides medViewContainer::setView.
     *
     * Tells a new container add the view to itslef and adds it to its layout.
     *
     * @param view
     */
    void setView(dtkAbstractView *view);

    void layout (QList<QWidget *> content);

    virtual QString description() {return "Multi";}

public slots:
    void onViewClosing    (void);
    void onViewFullScreen (bool value);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    medViewContainerMultiPrivate *d2;

    //Needed to access the setCurrent protected method
    //from within medViewContainerSingle2
    friend void medViewContainerSingle2::onViewFocused(bool);
};

#endif
