#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medMaskApplicationPluginExport.h"

#include <medAbstractData.h>
#include <medAbstractProcessLegacy.h>

class medMaskApplicationPrivate;

class MEDMASKAPPLICATIONPLUGIN_EXPORT medMaskApplication : public medAbstractProcessLegacy
{
    Q_OBJECT
    
public:
    medMaskApplication();
    virtual ~medMaskApplication();
    
    virtual QString description() const;
    
    static bool registered();

    template <typename IMAGE> int updateMaskType();

    void clearInput(int channel);

    void setParameter(double data);
    
public slots:
    
    //! Input data to the plugin is set through here
    void setInput(medAbstractData *data, int channel);
    
    //! Method to actually start the filter
    int update();
    
    //! The output will be available through here
    medAbstractData *output();
    
private:
    medMaskApplicationPrivate *d;
};

dtkAbstractProcess *createMedMaskApplication();


