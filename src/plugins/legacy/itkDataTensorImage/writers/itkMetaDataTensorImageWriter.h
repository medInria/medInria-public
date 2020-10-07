#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <itkDataTensorImageWriterBase.h>

#include <itkDataTensorImagePluginExport.h>

class ITKDATATENSORIMAGEPLUGIN_EXPORT itkMetaDataTensorImageWriter : public itkDataTensorImageWriterBase
{
    Q_OBJECT

public:
    itkMetaDataTensorImageWriter();
    virtual ~itkMetaDataTensorImageWriter();

    virtual QString identifier()  const;
    virtual QString description() const;

    static bool registered();

    static dtkAbstractDataWriter* create();
};
