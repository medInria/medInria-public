#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcessLegacy.h>
#include <medAbstractData.h>
#include <medN4BiasCorrectionPluginExport.h>

class medN4BiasCorrectionPrivate;

class MEDN4BIASCORRECTIONPLUGIN_EXPORT medN4BiasCorrection : public medAbstractProcessLegacy
{
    Q_OBJECT
    
public:
    medN4BiasCorrection();
    virtual ~medN4BiasCorrection();
    
    virtual QString description() const;
    
    static bool registered();
    
public slots:    
    //! Input data to the plugin is set through here
    void setInput(medAbstractData *data, int channel = 0);
    
    //! Parameters are set through here, channel allows to handle multiple parameters
    void setParameter(double  data, int channel);
    
    //! Method to actually start the filter
    int update();

    //! The output will be available through here
    medAbstractData *output();

    void onCanceled();

protected:
    template <class InputImageType>
        int update(medAbstractData *inputData);

private:
    medN4BiasCorrectionPrivate *d;
};

dtkAbstractProcess *createMedN4BiasCorrection();
