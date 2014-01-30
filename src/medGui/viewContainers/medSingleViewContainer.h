/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medGuiExport.h>

#include <medViewContainer.h>

class MEDGUI_EXPORT medSingleViewContainer : public medViewContainer
{
    Q_OBJECT

public:
    medSingleViewContainer(QWidget *parent = 0) : medViewContainer(parent) {}


    void split(int rows, int cols);
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
    virtual void setView(dtkAbstractView *view);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
};


