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

class itkFiltersToolBoxPrivate;

/*! \brief Toolbox to apply some itk filters.
 *
 * This toolbox has several named widgets which can be accessed in python pipelines:\n\n
 * "thresholdValue" : medDoubleParameter\n
 * "lowerValue" : medDoubleParameter\n
 * "upperValue" : medDoubleParameter\n
 * "outsideThresholdValue" : QSpinBox\n
 * "greaterButton" : QRadioButton\n
 * "lowerButton" : QRadioButton\n
 * "outsideButton" : QRadioButton\n
 * "binaryThresholdButton" : QRadioButton\n
 * "componentSizeThresholdFilterValue" : QSpinBox\n
 * "Run" : QPushButton
 */
class itkFiltersToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("ITK Basic Filters",
                          "ITK filters, processing examples",
                          <<"Filtering")
public:
    itkFiltersToolBox(QWidget *parentToolBox = 0);
    ~itkFiltersToolBox();

public:
    static bool registered();
    medAbstractData* processOutput();
    dtkPlugin* plugin();

public slots:

    void onFiltersActivated(int index);

    void clear();
    void update();

    void run();

private slots:
    void updateThresholdToolboxBehaviour(int id);
    void checkBinaryThreshold(bool checked);

private:
    template <typename ImageType> int setupSpinBoxValues(medAbstractData*);

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
    void setupItkComponentSizeThresholdProcess();
    
    itkFiltersToolBoxPrivate *d;
};

medToolBox *createitkFiltersToolBox(QWidget *parent);


