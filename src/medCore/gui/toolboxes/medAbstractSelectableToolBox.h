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

class medAbstractData;
class medSelectorToolBox;
class medAbstractSelectableToolBoxPrivate;

//! Base class for custom toolboxes in combobox
class MEDCORE_EXPORT medAbstractSelectableToolBox : public medToolBox
{
    Q_OBJECT

public:
    medAbstractSelectableToolBox(QWidget *parent = 0);
    virtual ~medAbstractSelectableToolBox();

    virtual dtkPlugin* plugin() = 0;

    virtual medAbstractData *processOutput() = 0;

    virtual void updateType(medAbstractData *data){}

    void setSelectorToolBox(medSelectorToolBox *toolbox);

    //! Launched when scrolled in medSelectorToolBox
    void showEvent(QShowEvent *event);

public slots:

    void updateView(){}

protected:
    medSelectorToolBox *selectorToolBox();

private:
    medAbstractSelectableToolBoxPrivate *d;
};


