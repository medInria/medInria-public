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

class medN4BiasCorrectionToolBoxPrivate;

/*! \brief Toolbox to apply a N4 bias field correction algorithm to a volume.
 *
 * This toolbox has several named widgets which can be accessed in python pipelines:\n\n
 * "runButton" : QPushButton\n
 * "splineDistanceLayout" : QHBoxLayout\n
 * "widthLayout" : QHBoxLayout\n
 * "nbOfIterationsLayout" : QHBoxLayout\n
 * "convThresholdLayout" : QHBoxLayout\n
 * "bsplineOrderLayout" : QHBoxLayout\n
 * "shrinkFactorLayout" : QHBoxLayout\n
 * "nbHistogramBinsLayout" : QHBoxLayout\n
 * "wienerNoiseLayout" : QHBoxLayout\n
 * "saveBiasLayout" : QHBoxLayout
 */
class medN4BiasCorrectionToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("N4 Bias Correction",
                          "Apply the N4 bias field correction algorithm.",
                          <<"Filtering")

public:
    medN4BiasCorrectionToolBox(QWidget *parent = nullptr);
    ~medN4BiasCorrectionToolBox();
    
    medAbstractData *processOutput();
    
    static bool registered();
    dtkPlugin * plugin();
    std::vector<int> extractValue(QString text);

public slots:
    void update(medAbstractData*){}
    
signals:
    void success();
    void failure();
    
    public slots:
    void run();
    
private:
    medN4BiasCorrectionToolBoxPrivate *d;
};
