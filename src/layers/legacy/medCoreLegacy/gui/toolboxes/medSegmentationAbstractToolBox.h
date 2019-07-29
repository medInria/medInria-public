/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medToolBox.h>

#include <medCoreLegacyExport.h>

class medAbstractData;
class medSegmentationSelectorToolBox;
class medSegmentationAbstractToolBoxPrivate;

//! Base class for custom segmentation toolboxes
class MEDCORELEGACY_EXPORT medSegmentationAbstractToolBox : public medToolBox
{
    Q_OBJECT

public:
    medSegmentationAbstractToolBox(QWidget *parent = nullptr);
    virtual ~medSegmentationAbstractToolBox();

    virtual dtkPlugin* plugin() = 0;

    virtual medAbstractData *processOutput() = 0;

public slots:
    void updateView(){}

protected:

    //! Called when toolbox is hidden (scroll in medSelectorToolBox, workspace changed, etc)
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

    //! Get the selectorToolBox (usually one instance)
    medSegmentationSelectorToolBox *selectorToolBox();

private:
    medSegmentationAbstractToolBoxPrivate *d;
};


