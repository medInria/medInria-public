#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCoreLegacyExport.h>
#include <medToolBox.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

class medAbstractData;
class medSelectorToolBox;
class medAbstractSelectableToolBoxPrivate;

//! Base class for custom toolboxes in combobox
class MEDCORELEGACY_EXPORT medAbstractSelectableToolBox : public medToolBox
{
    Q_OBJECT

public:
    medAbstractSelectableToolBox(QWidget *parent = nullptr);
    ~medAbstractSelectableToolBox() override;

    virtual dtkPlugin* plugin();

    virtual medAbstractData *processOutput() = 0;
    virtual QList<dtkSmartPointer<medAbstractData>> processOutputs();
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


