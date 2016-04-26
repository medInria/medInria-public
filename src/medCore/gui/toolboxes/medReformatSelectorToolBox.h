/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>
#include <medCoreExport.h>

class medViewEventFilter;
class medReformatAbstractToolBox;

class medReformatSelectorToolBoxPrivate;
class MEDCORE_EXPORT medReformatSelectorToolBox : public medToolBox
{
    Q_OBJECT

public:
     medReformatSelectorToolBox(QWidget *parent = 0);
    ~medReformatSelectorToolBox();

     medReformatAbstractToolBox* currentToolBox();

signals:
     void inputChanged();

public slots:
    void changeCurrentToolBox(int index);
    void updateView(){}

private:
    medReformatSelectorToolBoxPrivate *d;
};


