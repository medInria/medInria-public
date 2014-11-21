/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medItkImageDataWriterBase.h>
#include <medItkImageExport.h>

class MEDITKIMAGEPLUGIN_EXPORT medItkImageMetaDataWriter: public medItkImageDataWriterBase {
public:
    medItkImageMetaDataWriter();
    virtual ~medItkImageMetaDataWriter();

    virtual QString identifier()  const;
    virtual QString description() const;
    virtual QStringList handled() const;

    static bool registered();

    static dtkAbstractDataWriter * create();
};
