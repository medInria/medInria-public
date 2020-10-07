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

#include <itkDataDiffusionGradientListPluginExport.h>

class itkDataDiffusionGradientListPrivate;

class ITKDATADIFFUSIONGRADIENTLISTPLUGIN_EXPORT itkDataDiffusionGradientList : public medAbstractData
{
    Q_OBJECT
    MED_DATA_INTERFACE("Itk Data Diffusion Gradient List",
                       "Data type to store diffusion gradients.")
public:
    itkDataDiffusionGradientList();
    virtual ~itkDataDiffusionGradientList();

    static bool registered();

    virtual void *data();
    virtual void *data(int channel);
    virtual void setData(void *data);

public slots:
    bool read (const QString& file);
    bool write(const QString& file);
    
private:
    itkDataDiffusionGradientListPrivate *d;
};
