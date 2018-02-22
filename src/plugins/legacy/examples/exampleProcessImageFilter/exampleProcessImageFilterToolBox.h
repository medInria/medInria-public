/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medDiffusionAbstractToolBox.h>

class exampleProcessImageFilterToolBoxPrivate;

/**
* @class exampleProcessImageFilterToolBox
* @brief Creates a toolbox for the exampleProcessImageFilter that is registered with the CustomDiffusionToolBox
* It defines the GUI interface to use exampleProcessImageFilter
* Gets two 3D unsigned short images from medDropSit(es)
* Selects from three filters from QCheckBox(es)
* Sets the voxel seed coordinates and LowerThreshold from QSpinBox(es)
**/

class exampleProcessImageFilterToolBox : public medDiffusionAbstractToolBox {

    Q_OBJECT

public:

     exampleProcessImageFilterToolBox(QWidget *parent = 0);
    ~exampleProcessImageFilterToolBox();

    QString description() const;

    dtkAbstractProcess* create();
    dtkAbstractData*    output() const;
    dtkPlugin*          plugin() const;

protected slots:

    /**
    * Get and Set the voxel seed coordinates to be used by the itkConnectedThresholdImageFilter.h
    **/

    void onXValueChanged(int value);
    void onYValueChanged(int value);
    void onZValueChanged(int value);

    /**
    * Gets and Sets the LowerThreshold to be used by the itkConnectedThresholdImageFilter.h, the HigherThreshold=LowerThreshold+150
    **/

    void onAValueChanged(int value);

    /**
    * Gets and Sets the option to select the image filter to be used
    **/

    void onOptionClicked(QAbstractButton *button);

    /**
    * When an image is drop in one of the medDropSite(s) it set it as an input for the process
    **/

    void onObjectDropped();
    void run();

private:

    exampleProcessImageFilterToolBoxPrivate *d;
};

medDiffusionAbstractToolBox *createExampleProcessImageFilterToolBox(QWidget *);


