#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractData.h>
#include <medAbstractProcessLegacy.h>
#include <medRemeshingPluginExport.h>

#include <vtkMetaDataSet.h>

class medDecimateMeshProcessPrivate;

class MEDREMESHINGPLUGIN_EXPORT medDecimateMeshProcess : public medAbstractProcessLegacy
{
    Q_OBJECT
    
public:
    medDecimateMeshProcess();
    virtual ~medDecimateMeshProcess();
    
    virtual QString description() const;
    
    static bool registered();

signals:
    void warning();
    void polyDataCastFailure();
    
public slots:
    
    //! Input data to the plugin is set through here
    void setInput(medAbstractData *data, int channel = 0);
    
    //! Parameters are set through here, channel allows to handle multiple parameters
    void setParameter(double data, int channel);
    void setParameter(int data, int channel);

    //! Method to actually start the filter
    int update();
    
    //! The output will be available through here
    medAbstractData *output();

protected:
    vtkMetaDataSet* decimateOneMetaDataSet(vtkMetaDataSet *inputMetaDataSet);

private:
    medDecimateMeshProcessPrivate *d;
};

dtkAbstractProcess *createmedDecimateMeshProcess();


