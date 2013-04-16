/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medGuiExport.h"
#include "medToolBox.h"

class medVisualizationLayoutToolBoxPrivate;

class MEDGUI_EXPORT medVisualizationLayoutToolBox : public medToolBox
{
    Q_OBJECT

public:
     medVisualizationLayoutToolBox(QWidget *parent = 0);
    ~medVisualizationLayoutToolBox();

signals:
    void modeChanged(const QString&);
    void split(int, int);
    void presetClicked(int);

public slots:
    void clear();
    /**
     * @brief Sets the current tab according to container type
     *
     * @param name the container description
    */
    void setTab(const QString &name);

protected slots:
    void onButtonAClicked();
    void onButtonBClicked();
    void onButtonCClicked();
    void onButtonDClicked();
    void onButtonEClicked();
    void modeChanged(int);

private:
    medVisualizationLayoutToolBoxPrivate *d;
};


