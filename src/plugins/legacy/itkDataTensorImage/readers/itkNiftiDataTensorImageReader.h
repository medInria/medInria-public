#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataTensorImageReaderBase.h>
#include <itkDataTensorImagePluginExport.h>

class ITKDATATENSORIMAGEPLUGIN_EXPORT itkNiftiDataTensorImageReader: public itkDataTensorImageReaderBase
{
    Q_OBJECT
	
public:

    itkNiftiDataTensorImageReader();
    virtual ~itkNiftiDataTensorImageReader();
	
    virtual QString identifier()  const;
    virtual QString description() const;
	
    static bool registered();

    static dtkAbstractDataReader * create();
};
