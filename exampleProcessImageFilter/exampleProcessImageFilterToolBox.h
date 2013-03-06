/* exampleProcessImageFilterToolBox.h ---
 *
 * Author: Jaime Garcia_Guevara
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Mar  2 15:01:32 2011 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar  2 16:39:24 2011 (+0100)
 *           By: Jaime Garcia_Guevara
 *     Update #: 10
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

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

    void onObjectDropped(void);
    void run(void);

private:

    exampleProcessImageFilterToolBoxPrivate *d;
};

medDiffusionAbstractToolBox *createExampleProcessImageFilterToolBox(QWidget *);


