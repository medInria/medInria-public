/*=========================================================================

 medInria

 Copyright (c) INRIA 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medAbstractProcessLegacy.h>
#include <medAbstractData.h>
#include <reformatPluginExport.h>

class resampleProcessPrivate;
class REFORMATPLUGIN_EXPORT resampleProcess : public medAbstractProcessLegacy
{
    Q_OBJECT

public:
    resampleProcess(void);
    virtual ~resampleProcess(void);
    virtual QString description(void) const;
    static bool registered(void);

public slots:
    //! Input data to the plugin is set through here
    void setInput(medAbstractData *data);
    void setInput(medAbstractData *data, int channel);

    //! Parameters are set through here, channel allows to handle multiple parameters
    void setParameter(double data, int channel);

    //! Method to actually start the filter
    int update();

    //! The output will be available through here
    medAbstractData *output();

private:
    template <class ImageType> int resample(medAbstractData* inputData);
    resampleProcessPrivate *d;
};
dtkAbstractProcess *createResampleProcess();
