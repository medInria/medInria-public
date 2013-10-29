/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <itkDataSHImageWriterBase.h>
#include <itkDataSHImageWriterPluginExport.h>

class ITKDATASHIMAGEWRITERPLUGIN_EXPORT itkMetaDataSHImageWriter : public itkDataSHImageWriterBase
{
    Q_OBJECT

public:
    itkMetaDataSHImageWriter();
    virtual ~itkMetaDataSHImageWriter();

    virtual QString identifier()  const;
    virtual QString description() const;

    static bool registered();

    static dtkAbstractDataWriter* create();
};
