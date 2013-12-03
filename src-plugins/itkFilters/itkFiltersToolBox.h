/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
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

public:
    itkFiltersToolBox(QWidget *parentToolBox = 0);
    ~itkFiltersToolBox();

public:
    static bool registered();
    dtkAbstractData* processOutput();
    dtkPlugin* plugin();

signals:
    void success();
    void failure();

public slots:

    void onFiltersActivated(int index);

    void clear();
    void update(dtkAbstractView *view);

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
    
    itkFiltersToolBoxPrivate *d;
};

medToolBox *createitkFiltersToolBox(QWidget *parent);


