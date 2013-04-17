/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "itkDataTensorImageWriterBase.h"

#include "itkDataTensorImageWriterPluginExport.h"

class ITKDATATENSORIMAGEWRITERPLUGIN_EXPORT itkMetaDataTensorImageWriter : public itkDataTensorImageWriterBase
{
    Q_OBJECT

public:
    itkMetaDataTensorImageWriter();
    virtual ~itkMetaDataTensorImageWriter();

    virtual QString description() const;
    virtual QString identifier() const;

    static bool registered();

private:
    static const char ID[];
};

dtkAbstractDataWriter *createItkMetaDataTensorImageWriter();


