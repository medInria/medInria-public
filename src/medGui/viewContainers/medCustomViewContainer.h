/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

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
    ~medCustomViewContainer();


    void split(int rows, int cols);

    void setPreset(int preset);
    void setView(dtkAbstractView *view);

    dtkAbstractView *view() const;
    QList<dtkAbstractView*> views() const;

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
    virtual bool isLeaf() const;

    virtual QString identifier() {return "Custom";}

public slots:
    void onViewClosing();
    void onViewFullScreen (bool value);
    void clear();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    void onViewFullScreen2 (bool value, dtkAbstractView *view);
    void fullScreen (bool value, dtkAbstractView *view);

    // QList<medCustomViewContainer*> children() const;
    medCustomViewContainerPrivate *d2;
};


