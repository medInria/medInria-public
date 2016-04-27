/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractToolBox.h>

class itkN4BiasCorrectionToolBoxPrivate;

class itkN4BiasCorrectionToolBox : public medAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("itkN4BiasCorrectionToolBox","N4 Bias Correction",<<"filtering")

public:
    itkN4BiasCorrectionToolBox(QWidget *parent = 0);
    ~itkN4BiasCorrectionToolBox();
    
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
    itkN4BiasCorrectionToolBoxPrivate *d;
};


