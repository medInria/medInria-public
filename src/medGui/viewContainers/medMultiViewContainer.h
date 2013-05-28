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
#include "medSingleViewContainer.h"

class MEDGUI_EXPORT medSingleViewContainer2 : public medSingleViewContainer
{
    Q_OBJECT

public:
     medSingleViewContainer2(QWidget *parent = 0) :
         medSingleViewContainer(parent) {}
    ~medSingleViewContainer2();

    void setView(dtkAbstractView *view);

public slots:

    virtual void onViewClosing();
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

};


