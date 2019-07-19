/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medFilteringAbstractToolBox.h>

class ITKProcessExampleToolBoxPrivate;

class ITKProcessExampleToolBox : public medFilteringAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("itkGaussianBlurExample", "ITK Gaussian Blur Filter Example", << "filtering")

public:
     ITKProcessExampleToolBox(QWidget *parentToolBox = 0);
    ~ITKProcessExampleToolBox();

    static bool registered();
    medAbstractData* processOutput();
    dtkPlugin* plugin();
    void update(medAbstractData *data);

signals:
    void success();
    void failure();

public slots:

    void run();

private:
    ITKProcessExampleToolBoxPrivate *d;
};
