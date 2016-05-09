/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSelectableToolBox.h>

class itkMorphologicalFiltersToolBoxPrivate;

class itkMorphologicalFiltersToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Morphological Filters",
                          "ITK morphological filters, processing examples",
                          <<"filtering")
public:
    itkMorphologicalFiltersToolBox(QWidget *parentToolBox = 0);
    ~itkMorphologicalFiltersToolBox();

public:
    static bool registered();
    medAbstractData* processOutput();
    dtkPlugin* plugin();

signals:
    void success();
    void failure();

public slots:

    void roundSpinBox(bool param);
    void run();

private:  
    itkMorphologicalFiltersToolBoxPrivate *d;
};


