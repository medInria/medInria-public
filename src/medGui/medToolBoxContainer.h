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

#include <QtGui/QScrollArea>

class medToolBox;
class medToolBoxContainerPrivate;

class MEDGUI_EXPORT medToolBoxContainer : public QScrollArea
{
    Q_OBJECT
public:
     medToolBoxContainer(QWidget *parent = 0);
    ~medToolBoxContainer();

    void addToolBox(medToolBox *toolBox);
    void removeToolBox(medToolBox *toolBox);
    void insertToolBox(int index, medToolBox* toolBox);
    void clear();
    
    QList<medToolBox*> toolBoxes() const;

    /**
     * @brief Set margin and spacing of the layout
     *
     * @param spacing
    */
    void setLayoutSpacing(int spacing);
    

private:
    medToolBoxContainerPrivate *d;
};


