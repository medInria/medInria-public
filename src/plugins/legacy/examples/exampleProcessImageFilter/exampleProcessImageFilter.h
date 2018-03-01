/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractProcess.h>

#include <exampleProcessImageFilterPluginExport.h>

class exampleProcessImageFilterPrivate;

/**
* @class exampleProcessImageFilter
* @brief Uses one of the itkMaskImageFilter.h, itkAddImageFilter.h, itkConnectedThresholdImageFilter.h filters, settings of the
* last filter can be specified by the spin boxes of the exampleProcessImageFilterToolBox.h
**/

class EXAMPLEPROCESSIMAGEFILTERPLUGIN_EXPORT exampleProcessImageFilter: public dtkAbstractProcess
{
    Q_OBJECT

public:
             exampleProcessImageFilter();
    virtual ~exampleProcessImageFilter();

    enum Option {
        optionA,
        optionB,
        optionC
    };

    virtual QString description() const;

    static bool registered();

public:

    /**
    * Set the LowerThreshold for the itkConnectedThresholdImageFilter.h, the HigherThreshold=LowerThreshold+150
    **/
    void setLowerThreshold(int th);

    /**
    * Sets the seed coordinates of the itkConnectedThresholdImageFilter.h
    **/
    void setPoint(int x, int y, int z);

    void setInput(dtkAbstractData *data);

    /**
    * Sets the UnsignedShort input image at specified channel
    **/
    void setInput(dtkAbstractData *data, int channel);

    /**
    * Sets the option to select the image filter to be used
    **/
    void setOption(Option option);

    dtkAbstractData *output();

    int update();

private:
    exampleProcessImageFilterPrivate *d;
};

dtkAbstractProcess *createExampleProcessImageFilter();


