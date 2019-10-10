/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include <medAbstractData.h>
#include <medAbstractProcessLegacy.h>
#include <medRemeshingPluginExport.h>

#include <vtkMetaDataSet.h>

class medSmoothMeshProcessPrivate;

class MEDREMESHINGPLUGIN_EXPORT medSmoothMeshProcess : public medAbstractProcessLegacy
{
    Q_OBJECT
    
public:
    medSmoothMeshProcess();
    virtual ~medSmoothMeshProcess();
    
    virtual QString description() const;
    
    static bool registered();

signals:
    void polyDataCastFailure();
    
public slots:
    
    //! Input data to the plugin is set through here
    void setInput(medAbstractData *data, int channel = 0);
    
    //! Parameters are set through here, channel allows to handle multiple parameters
    void setParameter(double data, int channel);
    
    //! Method to actually start the filter
    int update();
    
    //! The output will be available through here
    medAbstractData *output();

protected:
    vtkMetaDataSet* smoothOneMetaDataSet(vtkMetaDataSet *inputMetaDataSet);

private:
    medSmoothMeshProcessPrivate *d;
};

dtkAbstractProcess *createmedSmoothMeshProcess();


