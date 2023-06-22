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
#include <meshMappingPluginExport.h>

#include <itkImage.h>

class meshMappingPrivate;

typedef itk::Image<unsigned char, 3>  MaskType;

class MESHMAPPINGPLUGIN_EXPORT meshMapping : public medAbstractProcessLegacy
{
    Q_OBJECT
    
public:
    meshMapping();
    virtual ~meshMapping();
    
    virtual QString description() const;
    
    static bool registered();
    
public slots:
    
    //! Input data to the plugin is set through here
    void setInput(medAbstractData *data, int channel);
    
    //! Method to actually start the filter
    int update();
    
    //! The output will be available through here
    medAbstractData *output();
    
private:
    meshMappingPrivate *d;
};

dtkAbstractProcess *createMeshMapping();
