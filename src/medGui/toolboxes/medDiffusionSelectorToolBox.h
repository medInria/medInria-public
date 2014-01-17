/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>
#include "medGuiExport.h"

class dtkAbstractView;
class dtkAbstractData;
class dtkAbstractProcess;
class medDiffusionSelectorToolBoxPrivate;
class medDataIndex;

class MEDGUI_EXPORT medDiffusionSelectorToolBox : public medToolBox
{
    Q_OBJECT
public:
     medDiffusionSelectorToolBox(QWidget *parent = 0);
    ~medDiffusionSelectorToolBox();

    dtkAbstractData *output() const;

signals:
    void newOutput(dtkAbstractData * data);

public slots:
    // void run();
    void clear();

    void onToolBoxChosen(int id);

private:
    medDiffusionSelectorToolBoxPrivate *d;

};


