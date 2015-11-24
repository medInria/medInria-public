/*=========================================================================

 Plugin medInria - Super Resolution

 Copyright (c) IHU LIRYC 2013. All rights reserved.

 Author : Mathilde Merle -- mathilde.merle @ ihu-liryc.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractProcess.h>
#include <medAbstractData.h>
#include "reformatPluginExport.h"
//#include <medToolBox.h>
//#include <itkImage.h>

class superResolutionProcessPrivate;

class REFORMATPLUGIN_EXPORT superResolutionProcess : public medAbstractProcess
{
    Q_OBJECT

public:

    superResolutionProcess(void);
    virtual ~superResolutionProcess(void);
    virtual QString description(void) const;
    static bool registered(void);

public slots:

    //! Input data to the plugin is set through here
    void setInput(medAbstractData *data, int channel);
    //! Method to actually start the filter
    int update(void);

    //! The output will be available through here
    medAbstractData *output(void);

    medAbstractData* castToUChar3(medAbstractData* image);

private:
    template <class ImageType> void runButton();
    template <typename IMAGE> medAbstractData* cast(medAbstractData* image);
    superResolutionProcessPrivate *d;
};

dtkAbstractProcess *createsuperResolutionProcess();
