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
#include "medToolBox.h"

class medFilteringSelectorToolBox;
class medFilteringAbstractToolBoxPrivate;
class dtkAbstractData;

class MEDGUI_EXPORT medFilteringAbstractToolBox : public medToolBox
{
    Q_OBJECT

public:
             medFilteringAbstractToolBox(QWidget *parentToolBox = 0);
    virtual ~medFilteringAbstractToolBox();

    void setFilteringToolBox(medFilteringSelectorToolBox *toolbox);

    virtual dtkAbstractData* processOutput() = 0;

protected:
    medFilteringSelectorToolBox *parentToolBox();

private:
    medFilteringAbstractToolBoxPrivate *d;
};


