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
#include <medCoreLegacyExport.h>

class medAbstractData;
class medSelectorToolBox;
class medAbstractSelectableToolBoxPrivate;

//! Base class for custom toolboxes in combobox
class MEDCORELEGACY_EXPORT medAbstractSelectableToolBox : public medToolBox
{
    Q_OBJECT

public:
    medAbstractSelectableToolBox(QWidget *parent = nullptr);
    virtual ~medAbstractSelectableToolBox();

    virtual dtkPlugin* plugin() = 0;

    virtual medAbstractData *processOutput() = 0;

    void setSelectorToolBox(medSelectorToolBox *toolbox);

public slots:

    void updateView(){}

protected:

    //! Called when toolbox is hidden (scroll in medSelectorToolBox, workspace changed, etc)
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

    //! Get the selectorToolBox (usually one instance)
    medSelectorToolBox *selectorToolBox();

private:
    medAbstractSelectableToolBoxPrivate *d;
};


