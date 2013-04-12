/* medMultiViewContainer.h ---
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

#pragma once

#include "medGuiExport.h"

#include "medViewContainer.h"
#include "medSingleViewContainer.h"

class MEDGUI_EXPORT medSingleViewContainer2 : public medSingleViewContainer
{
    Q_OBJECT

public:
     medSingleViewContainer2(QWidget *parent = 0) :
         medSingleViewContainer(parent) {}
    ~medSingleViewContainer2();

    void setView(dtkAbstractView *view);

    /**
     * @brief Is this a leaf container?
     *
     * This method is useful in the case of a custom or multi
     * container.  Returns true if this object is supposed to contain
     * only views and no other containers (like
     * medSingleViewContainer2).
     *
     * @param void
     * @return bool
    */
    virtual bool isLeaf() const;

public slots:
    virtual void onViewClosing();

    virtual void onViewFocused (bool value);
};


class medMultiViewContainerPrivate;

class MEDGUI_EXPORT medMultiViewContainer : public medViewContainer
{
    Q_OBJECT

public:
    medMultiViewContainer(QWidget *parent = 0);
   ~medMultiViewContainer();


    void split(int rows, int cols);

    dtkAbstractView *view() const;

    QList<dtkAbstractView*> views() const;

    /**
     * @brief Overrides medViewContainer::setView.
     *
     * Tells a new container add the view to itslef and adds it to its layout.
     *
     * @param view
     */
    void setView(dtkAbstractView *view);

    void layout (QList<QWidget *> content);

    virtual QString identifier() {return "Multi";}

public slots:
    void onViewClosing();
    void onViewFullScreen (bool value);

private:
    medMultiViewContainerPrivate *d2;

    //Needed to access the setCurrent protected method
    //from within medSingleViewContainer2
    friend void medSingleViewContainer2::onViewFocused(bool);
};


