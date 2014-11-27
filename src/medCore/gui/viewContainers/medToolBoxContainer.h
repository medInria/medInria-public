/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <QtGui/QScrollArea>

class medToolBox;
class medToolBoxContainerPrivate;

class MEDCORE_EXPORT medToolBoxContainer : public QScrollArea
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
    
private:
    medToolBoxContainerPrivate *d;
};
