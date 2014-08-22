/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medFilteringAbstractToolBox.h>

class itkFiltersToolBoxPrivate;

class itkFiltersToolBox : public medFilteringAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("ITK Basic Filters",
                          "ITK filters, processing examples",
                          <<"filtering")
public:
    itkFiltersToolBox(QWidget *parentToolBox = 0);
    ~itkFiltersToolBox();

public:
    static bool registered();
    medAbstractData* processOutput();
    dtkPlugin* plugin();

signals:
    void success();
    void failure();

public slots:

    void onFiltersActivated(int index);

    void clear();
    void update(medAbstractData *data);

    void run();

private:
    void setupItkAddProcess();
    void setupItkSubtractProcess();
    void setupItkMultiplyProcess();
    void setupItkDivideProcess();
    void setupItkGaussianProcess();
    void setupItkMedianProcess();
    void setupItkInvertProcess();
    void setupItkNormalizeProcess();
    void setupItkShrinkProcess();
    void setupItkWindowingProcess();
    void setupItkThresholdingProcess();
    
    itkFiltersToolBoxPrivate *d;
};

medToolBox *createitkFiltersToolBox(QWidget *parent);


