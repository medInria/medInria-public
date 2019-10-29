/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSelectableToolBox.h>

class itkMorphologicalFiltersToolBoxPrivate;

/*! \brief Toolbox to apply morphological filters to a volume.
 *
 * This toolbox has several named widgets which can be accessed in python pipelines:\n\n
 * "Dilate" : medComboBox\n
 * "kernelSize" : QDoubleSpinBox\n
 * "mm" : QRadioButton\n
 * "pixels" : QRadioButton\n
 * "ballKernelButton" : QRadioButton\n
 * "boxKernelButton" : QRadioButton\n
 * "crossKernelButton" : QRadioButton\n
 * "Run" : QPushButton
 */
class itkMorphologicalFiltersToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Morphological Filters",
                          "Apply ITK morphological filters.",
                          <<"Filtering")
public:
    itkMorphologicalFiltersToolBox(QWidget *parentToolBox = nullptr);
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
