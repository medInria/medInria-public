#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtWidgets/QScrollArea>

#include <medCoreLegacyExport.h>

class medToolBox;
class medToolBoxContainerPrivate;

class MEDCORELEGACY_EXPORT medToolBoxContainer : public QScrollArea
{
    Q_OBJECT

public:
     medToolBoxContainer(QWidget *parent = nullptr);
    ~medToolBoxContainer();

    void addToolBox(medToolBox *toolBox);
    void removeToolBox(medToolBox *toolBox);
    void insertToolBox(int index, medToolBox* toolBox);
    void clear();

    QList<medToolBox*> toolBoxes() const;

private:
    medToolBoxContainerPrivate *d;
};
